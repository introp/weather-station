//! @file
//! @author Jon Dubovsky, 28 Oct 2015
#include "eeprom.h"
#include "avalon_mutex.h"
#include "avalon_time.h"
#include "avalon_debug.h"

#include "FreeRTOS.h"	// required for task.h
#include "task.h"		// uses portTickType
#include "semphr.h"

using namespace std;

//#define EEPROM_SEMA_METHOD
#define EEPROM_QUEUE_METHOD


static int ee_writen_(eeaddr_t addr, const uint8_t * buf, size_t sizeBytes) {
	// TODO: implement write via IAP (don't forget to disable interrupts)
	return 0;
}


static int ee_readn_(eeaddr_t addr, uint8_t * buf, size_t sizeBytes) {
	// TODO: implement read via IAP (don't forget to disable interrupts)
	return 0;
}


#ifdef EEPROM_SEMA_METHOD
struct EepromWorkData {
	Mutex mutex;	//!< mutex for this struct
	Semaphore semaWorkToDo;	//!< signal EEPROM task to do something
	Semaphore semaWorkComplete;	//!< EEPROM task signals this when the "something" is done

	volatile bool write;		//!< true for write operations, false for reads
	volatile eeaddr_t addr;	//!< address to operate on
	union {
		volatile const uint8_t * writebuf;	//!< pointer to buffer to write from; valid only when "write" is true
		volatile uint8_t * readbuf;		//!< pointer to buffer to read into; valid only when "write" is false
	};
	volatile size_t bufBytes;	//!< number of bytes of buf that should be used
	//! Filled by the EE task when the operation (read/write) is complete,
	//! before signaling semaWorkComplete; 0 == success.
	volatile int operationResult;
};
static EepromWorkData *g_work;
#endif


#ifdef EEPROM_QUEUE_METHOD
//! Depth of the EE task work queue (how many pending operations the EEPROM
//! task can have from callers).
const int EEPROM_QUEUE_LENGTH = 2;

//! Work queue entry (passed by value).
struct EepromTask {
	volatile eeaddr_t addr;		//!< address to operate on
	volatile bool write;		//!< true for write operations, false for reads
	union {
		volatile const uint8_t * writebuf;	//!< pointer to buffer to write from; valid only when "write" is true
		volatile uint8_t * readbuf;		//!< pointer to buffer to read into; valid only when "write" is false
	};
	volatile size_t bufBytes;	//!< number of bytes of buf that should be used
	//! Filled by the EE task when the operation (read/write) is complete,
	//! before notifying taskToNotify; 0 == success.
	volatile int * pResult;
	TaskHandle_t taskToNotify;	//!< EEPROM task notifies this task when the "something" is done
};

//! Work queue.
QueueHandle_t g_queueWork;
#endif


//! Initialize EEPROM subsystem; don't call until after RTOS is initialized.
void eeprom_init() {
#ifdef EEPROM_SEMA_METHOD
	g_work = new EepromWorkData;
#endif
#ifdef EEPROM_QUEUE_METHOD
	g_queueWork = xQueueCreate(EEPROM_QUEUE_LENGTH, sizeof(EepromTask));
#endif
}


void eeprom_task(void *pvParameters) {
//#define EEPROM_QUEUE_METHOD
#ifdef EEPROM_QUEUE_METHOD
	EepromTask task;
	while (true) {
		// wait until we have a read/write task
		if (xQueueReceive(g_queueWork, &task, portMAX_DELAY) == pdPASS) {
			// do the task
			if (task.write) {
				*task.pResult = ee_writen_(task.addr,
						const_cast<const uint8_t *>(task.writebuf),	// remove volatile
						task.bufBytes);
			} else {
				*task.pResult = ee_readn_(task.addr,
						const_cast<uint8_t *>(task.readbuf),		// remove volatile
						task.bufBytes);
			}
			// signal the caller that we're done
			VERIFY(xTaskNotifyGive(task.taskToNotify) == pdPASS);
		}
	}
#endif

#ifdef EEPROM_SEMA_METHOD
	ASSERT(g_work);		// must have called eeprom_init

	while (true) {
		// sleep on the semaphore
		if (g_work->semaWorkToDo.Wait()) {
			// mutex is claimed by the client, so we're safe to use g_data contents
			if (g_work->write) {
				// write op
				g_work->operationResult = ee_writen_(g_work->addr,
						const_cast<const uint8_t *>(g_work->writebuf),	// remove volatile
						g_work->bufBytes);
			} else {
				// read op
				g_work->operationResult = ee_readn_(g_work->addr,
						const_cast<uint8_t *>(g_work->readbuf),		// remove volatile
						g_work->bufBytes);
			}
			g_work->semaWorkComplete.Raise();
		}
	}
#endif
}


// ========== public interface ==========

bool ee_writen(eeaddr_t addr, const void * buf, size_t sizeBytes) {
#ifdef EEPROM_SEMA_METHOD
	SingleLock lock(g_work->mutex, SingleLock::ClaimNow);	// wait forever
	g_work->write = true;
	g_work->addr = addr;
	g_work->writebuf = reinterpret_cast<const uint8_t *>(buf);	// safe: void -> char
	g_work->bufBytes = sizeBytes;
	// signal worker thread to do the write
	g_work->semaWorkToDo.Raise();
	// and wait (forever) on it to complete
	// TODO: there is a race condition here? are these semaphores manual?
	g_work->semaWorkComplete.Wait();
	return (g_work->operationResult == 0) ? true : false;
#endif
#ifdef EEPROM_QUEUE_METHOD
	EepromTask task;
	int result = 0;

	task.addr = addr;
	task.write = true;
	task.writebuf = reinterpret_cast<const uint8_t *>(buf);	// safe: void -> char
	task.bufBytes = sizeBytes;
	task.pResult = &result;
	task.taskToNotify = xTaskGetCurrentTaskHandle();

	// send the job to the EEPROM task
	xQueueSend(g_queueWork, &task, portMAX_DELAY);

	// and wait for it to signal we're done
	uint32_t notifyResult = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
	// we don't care about notifyResult

	return (*task.pResult == 0) ? true : false;
#endif
}


bool ee_readn(eeaddr_t addr, void * buf, std::size_t sizeBytes) {
#ifdef EEPROM_SEMA_METHOD
	SingleLock lock(g_work->mutex, SingleLock::ClaimNow);	// wait forever
	g_work->write = false;
	g_work->addr = addr;
	g_work->readbuf = reinterpret_cast<uint8_t *>(buf);	// safe: void -> char
	g_work->bufBytes = sizeBytes;
	// signal worker thread to do the write
	g_work->semaWorkToDo.Raise();
	// and wait (forever) on it to complete
	// TODO: there is a race condition here? are these semaphores manual?
	g_work->semaWorkComplete.Wait();
	return (g_work->operationResult == 0) ? true : false;
#endif
#ifdef EEPROM_QUEUE_METHOD
	EepromTask task;
	int result = 0;

	task.addr = addr;
	task.write = false;
	task.readbuf = reinterpret_cast<uint8_t *>(buf);	// safe: void -> char
	task.bufBytes = sizeBytes;
	task.pResult = &result;
	task.taskToNotify = xTaskGetCurrentTaskHandle();

	// send the job to the EEPROM task
	xQueueSend(g_queueWork, &task, portMAX_DELAY);

	// and wait for it to signal we're done
	uint32_t notifyResult = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
	// we don't care about notifyResult

	return (*task.pResult == 0) ? true : false;
#endif
}


