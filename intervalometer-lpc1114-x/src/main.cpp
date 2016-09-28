//! @file
//! @author Jon Dubovsky, 28 Oct 2015
//! Effectively main.cpp

#include "avalon_time.h"
#include "camera.h"
#include "eeprom.h"
#include "board.h"
#include "FreeRTOS.h"
#include "task.h"

// Sets up system hardware
static void setup_hardware(void)
{
	SystemCoreClockUpdate();
	Board_Init();

	camera_bsp_init();
}

// lazy externs
//void vLEDTask0(void *pvParameters);
//void vLEDTask1(void *pvParameters);
//void vLEDTask2(void *pvParameters);
void adc_task(void *pvParameters);

extern "C" {
/**
Main routine that initializes hardware, starts tasks, etc.
@returns nothing; never returns.
*/
int main(void)
{
	setup_hardware();
	time_init();
	eeprom_init();

	// create threads
	xTaskCreate(camera_task, "camTask",
				configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1UL,
				(xTaskHandle *) NULL);

	/* LED0 toggle thread */
	/*
	xTaskCreate(vLEDTask0, (signed char *) "vTaskLed0",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
				(xTaskHandle *) NULL);
				*/

	// A/D thread
	xTaskCreate(adc_task, "adcTask",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
				(xTaskHandle *) NULL);

	// Start the scheduler; blocks forever.
	vTaskStartScheduler();
	return 1;	// should never get here
}

}		// end extern "C"
