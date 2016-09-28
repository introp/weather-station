//! @file
//! @author Jon Dubovsky, 28 Oct 2015
//! Periodic data logger.

#include "avalon_time.h"
#include "avalon_debug.h"
#include "eeprom.h"
#include "bytemanip.h"

using namespace std;


static const auto LOGGER_WRITE_PERIOD = Minutes{ 10 };
//! First EE address of logger data.
static const eeaddr_t LOGGER_EEADDR_BEGIN{ 0 };
// TODO: are any EE byte reserved by the BIOS/CPU?  last 32?
//! One past the last EE address of logger data.
static const eeaddr_t LOGGER_EEADDR_END{ 4096 };

#if 0
//! Anemometer timer struct.
static LPC_TIMER_T * const ANEMOMETER_TIMER = LPC_TIMER16_0;
//! Anemometer timer capture channel (register/pin) number.
static const uint8_t ANEMOMETER_CAPTURE_NUM = 0;

bool anemometer_initialized = false;
uint16_t anemometer_timer_last_click;

void anemometer_init() {
	Chip_TIMER_Init(ANEMOMETER_TIMER);
	// set up rate (count on PCLK / N)
	Chip_TIMER_TIMER_SetCountClockSrc(ANEMOMETER_TIMER, TIMER_CAPSRC_RISING_PCLK, 0 /* unused */);
	Chip_TIMER_PrescaleSet(ANEMOMETER_TIMER, uint32_t prescale);
	// turn on capture
	Chip_TIMER_CaptureRisingEdgeEnable(ANEMOMETER_TIMER, ANEMOMETER_CAPTURE_NUM);
	Chip_TIMER_CaptureEnableInt(ANEMOMETER_TIMER, ANEMOMETER_CAPTURE_NUM);
	// and start the timer running
	Chip_TIMER_Enable(ANEMOMETER_TIMER);
}

//! ISR invoked when anemometer switch closes.
void anemometer_edge_capture_isr() {
	Chip_TIMER_ClearCapture(ANEMOMETER_TIMER, ANEMOMETER_CAPTURE_NUM);	// clear interrupt
	uint16_t when = Chip_TIMER_ReadCapture(ANEMOMETER_TIMER, ANEMOMETER_CAPTURE_NUM);
	if (!anemometer_initialized) {
		// nothing to do on first click
		anemometer_initialized = true;
	} else {

	}
	anemometer_timer_last_click = when;
}
#endif

extern unsigned int temperature_get();

/*
===== Logged Data Size Considerations =====
EEPROM is 4kB.
Assume worst-case record size of 16 bytes, can hold 256 entries;
at 4 hrs/entry (6/day), that's 42 days of data.  That's much
longer than the SD card can hold photos, so works out okay.
*/
struct __attribute__((packed)) LogRecord_t {
	//! TDB (will contain POR, etc.);
	//! bit 0x80 == invalid record if set
	uint8_t flags;

	//! real time clock time, in seconds; 0xFFFFFF is an invalid record.
	uint16_t rtctime_lo;
	uint8_t rtctime_hi;
// 4 bytes

	int8_t temp_avg;		//!< temperature in degrees C (-128 to 127)
	int8_t temp_min;
	int8_t temp_max;
	// note: 1 bit spare in each temperature (only care about -40 to +70 C)
// +3 == 7 bytes

	//! Wind direction and rates as bits (MSb first on left):
	//! ddddAAAA AAMMMMMM
	//! Where d is wind direction,
	//! A is average rate in clicks/sec (0-63 at 2.4 kph/click),
	//! M is max rate.
	uint16_t wind;
// +2 == 9 bytes

	//! Precipitation since last log.
	uint8_t precip;
	// Sensor is 0.011" per click.  We only care up 2"/hr rate (181 clicks).
	// Log interval is 4 hours, so 256 counts over 8" == 0.03125"/count.
// +1 == 10 bytes

	//! @returns true if this record is valid (has a valid timestamp).
	bool isValid() const { return (flags & 0x80) ? false : true; }
	//! Sets this record to invalid (invalid timestamp).
	void setInvalid() { flags |= 0x80; }
};


//! Initializes a logger record read for later iteration with logger_readnext.
//! @see logger_readnext
void logger_readfirst(eeaddr_t * pAddr) {
	*pAddr = LOGGER_EEADDR_BEGIN;
}


/**
Given a pAddr from an earlier call to logger_readfirst, reads the next
logger record from EE and stores it in pRec, advancing pAddr to the
next logger record.
@returns true if there is now a valid record in pRec, false if not and
 the search is complete.
@see logger_readfirst
@code
Intended to be used as:
readfirst(&addr);
while (readnext(&addr, &rec)) {
	... process record ...;
}
@endcode
*/
bool logger_readnext(eeaddr_t * pAddr, LogRecord_t * pRec) {
	if (*pAddr >= LOGGER_EEADDR_END) {
		pRec->setInvalid();
	} else {
		VERIFY(ee_readn(*pAddr, pRec, sizeof(LogRecord_t)));
		// if this record is valid, advance to the next
		if (pRec->isValid()) {
			*pAddr += sizeof(LogRecord_t);
		}
	}
	return pRec->isValid();
}


void logger_task(void *pvParameters) {
#ifdef SUP_LOGGING
	// read EEPROM and figure out the last record we wrote
	eeaddr_t addrNextRec;
	LogRecord_t rec;
	auto rtcLastWrite = WallClock::now();	// if no records in EE, write in N hours
	logger_readfirst(&addrNextRec);
	while (logger_readnext(&addrNextRec, &rec)) {
		rtcLastWrite = Seconds{makedword(rec.rtctime_hi, rec.rtctime_lo)};
	}
	// the next record should get written at that time +4 hours (may be in the past)
	/* TODO: don't write "bootup + N hours" but "last record + N hours"
	rtcNextLogWrite = rtcLastWrite + LOGGER_WRITE_PERIOD;
	// TODO: and finally go from wall clock to ticks (steady clock)
	*/

	SteadyTimer timerWrite(LOGGER_WRITE_PERIOD);
	while (true) {
		auto rtcNow = WallClock::now();

		// Fill a logging record with current data.
		rec.flags = 0x00;
		rec.rtctime_hi = hiword(rtcNow);
		rec.rtctime_lo = loword(rtcNow);
		unsigned int avg, min, max;
		// temperature
		temperature_get_and_reset(&avg, &min, &max);
		rec.temp_avg = avg;
		rec.temp_min = min;
		rec.temp_max = max;
		// wind
		unsigned int dir;
		wind_get_and_reset(&dir, &avg, &max);
		rec.wind = (dir << 12) | (avg << 6) | max;
		// precip
		avg = precip_get_and_reset();
		rec.precip = avg;

		// Write the record
		VERIFY(ee_writen(*addrNextRec, &rec, sizeof(LogRecord_t)));
		addrNextRec += sizeof(LogRecord_t);

		// Go to sleep until it's time to write to the log again.
		ASSERT(rtcNow < rtcNextLogWrite);
		Milliseconds sleepLength{(rtcNow < rtcNextLogWrite) ? (rtcNextLogWrite - rtcNow) : 1};
		SteadyTimer t{sleepLength};
		t.delayUntilElapsed();
	}
#else
	// no logging; dummy task
	while (true) {
		vTaskDelay(configTICK_RATE_HZ);
	}
#endif
}

