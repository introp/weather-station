#ifndef AVALON_TIME_H_
#define AVALON_TIME_H_
//! @file
//! @author Jon Dubovsky, 28 Oct 2015
//! General time support with durations, timers, and clocks.

#include "FreeRTOS.h"	// required for task.h
#include "task.h"		// uses portTickType


//! Initialize the time subsystem.  Call early.
void time_init();


//! Time value in minutes.
class Minutes {
	unsigned int v_;
	friend class Seconds;
	friend class Milliseconds;
public:
	explicit Minutes(unsigned int m) :
		v_(m)
	{
	}
};


//! Time value in seconds.
class Seconds {
	unsigned int v_;
	friend class Milliseconds;
public:
	explicit Seconds(unsigned int sec) :
		v_(sec)
	{
	}
	Seconds(Minutes m) :
		v_(m.v_ * 60u)
	{
	}
};


//! Time value in milliseconds.
class Milliseconds {
	unsigned int v_;
public:
	explicit Milliseconds(unsigned int millisec) :
		v_(millisec)
	{
	}
	Milliseconds(Seconds sec) :
		v_(sec.v_ * 1000u)
	{
	}
	Milliseconds(Minutes m) :
		v_(m.v_ * 60u * 1000u)
	{
	}

	//! Access to the raw underlying millisecond count.
	unsigned int rawMilliseconds() const { return v_; }

	bool operator >=(const Milliseconds & rhs) const {
		return v_ >= rhs.v_;
	}

	//! Convert the given time to ticks.
	explicit operator portTickType() const {
		return v_ * 1000u * configTICK_RATE_HZ;
	}

};


//! Steady-ticking clock.
class SteadyClock {
public:
	typedef portTickType Timepoint;
	typedef portTickType Duration;

	static Timepoint now() {
		return xTaskGetTickCount();
	}

	//! Convert ticks to milliseconds.
	static Milliseconds ticksToMilliseconds(Duration duration) {
		static_assert(configTICK_RATE_HZ == 1000, "this only works if the tick rate is 1000 Hz; math needed otherwise");
		return Milliseconds(duration);
	}

	//! Convert milliseconds to ticks.
	static Duration millisecondsToTicks(Milliseconds duration) {
		static_assert(configTICK_RATE_HZ == 1000, "this only works if the tick rate is 1000 Hz; math needed otherwise");
		return duration.rawMilliseconds();
	}

	//! Delay the current task by some length of time.
	static void Delay(Milliseconds duration) {
		vTaskDelay(duration.rawMilliseconds() / 1000 * configTICK_RATE_HZ);
	}

	//! Ticks since the given time, NOT converted to real wall clock time.
	//! @see elapsedSince
	static Duration ticksSince(Timepoint previousTime) {
		return now() - previousTime;	// underflow okay
	}
	//! Milliseconds since the given time.
	static Milliseconds elapsedSince(Timepoint previousTime) {
		return ticksToMilliseconds(now() - previousTime);
	}
};


class SteadyTimer {
	SteadyClock::Timepoint start_;
	Milliseconds period_;
public:
	SteadyTimer(Milliseconds period) :
		start_(SteadyClock::now()),
		period_(period)
	{
	}
	SteadyTimer(SteadyClock::Timepoint timeStart, Milliseconds period) :
		start_(timeStart),
		period_(period)
	{
	}
	//! @returns true if the timer has elapsed; does not restart the timer.
	bool hasElapsed() const {
		return (SteadyClock::elapsedSince(start_) >= period_) ? true : false;
	}
	//! Delays the current task until the timer has elapsed and automatically
	//! resets the timer for the next period.
	void delayUntilElapsed() {
		// vTaskDelayUntil will immediately return if the time given is
		// already in the past.  It also advances start_ by the given period
		vTaskDelayUntil(&start_, SteadyClock::millisecondsToTicks(period_));
	}
};


//! Represents a wall clock time (in terms of seconds since the RTC was
//! calibrated).  This value is not guaranteed to be monotonically
//! increasing.
class WallClock {
public:
	typedef Seconds Timepoint;
	//typedef Seconds Duration;

	static Timepoint now();

	//static Timepoint ticksToRtc(portTickType ticks);
	//static portTickType rtcToTicks(Timepoint rtcValue);
};


// TODO: dummy RTC function for testing on LPC111x
inline uint32_t Chip_RTC_GetCount() { return 0; }

#endif /* AVALON_TIME_H_ */
