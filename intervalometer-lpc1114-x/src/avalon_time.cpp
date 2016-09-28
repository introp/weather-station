//! @file
//! @author Jon Dubovsky, 28 Oct 2015
//! Periodic data logger.

#include "avalon_time.h"
#include <cstdint>
#include "FreeRTOS.h"	// required for task.h
#include "task.h"		// uses portTickType

using namespace std;


//! RTC value at T-zero (when time_init was called)
//! @see ticksZero
static uint32_t rtcZero;
//! Ticks value at T-zero (when time_init was called)
//! @see rtcZero
static portTickType ticksZero;


void time_init() {
	// TODO: enable
	// Chip_RTC_Init(
	// Chip_RTC_Enable
	// Chip_RTC_EnableOptions(
	rtcZero = Chip_RTC_GetCount();
	ticksZero = xTaskGetTickCount();
}

/*
WallClock::Timepoint WallClock::ticksToRtc(portTickType ticks) {
	ticks -= ticksZero;
	// convert to seconds (truncate)
	ticks /= configTICK_RATE_HZ;
	ticks += rtcZero;
	return ticks;
}

portTickType WallClock::rtcToTicks(uint32_t rtcValue) {
	rtcValue -= rtcZero;
	// convert to ticks
	// WARNING: overflow?
	portTickType ticks = static_cast<portTickType>(rtcValue) * configTICK_RATE_HZ;
	ticks += ticksZero;
	return ticks;
	static_assert(sizeof(portTickType) == 4, "what");
}
*/

WallClock::Timepoint WallClock::now() {
	return Seconds{Chip_RTC_GetCount()};
}

