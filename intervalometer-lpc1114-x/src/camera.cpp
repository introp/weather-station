//! @file
//! Camera work task and support functions.

#include "avalon_time.h"
#include "FreeRTOS.h"
#include "task.h"

using namespace std;


//! How often we wake up the camera to run its picture-taking script.
static const auto CAMERA_SNAPSHOT_PERIOD = Seconds{10};
//! How long to let the Vcam battery eliminator power settle before trying
//! to power button.
static const auto CAMERA_VCAM_SETTLE_PERIOD = Milliseconds{100};
//! How long to hold the camera power button down to start it.
static const auto CAMERA_POWER_BUTTON_HOLD_PERIOD = Milliseconds{500};
//! How long to let the camera "run" after power-up
static const auto CAMERA_AUTOEXEC_RUNTIME = Seconds{5};


//! Turn on or off Vcam (battery eliminator power supply).
static void camera_vcam_enable(bool turnOn) {
	// TODO: wiggle hardware line Board_Vcam_Enable(turnOn ? 1 : 0)
}


//! Press the camera power button for the given duration.
static void camera_pressbutton_power(Milliseconds holdDuration) {
	// TODO: press button Board_CamButtonPress_Power(1)
	SteadyClock::Delay(CAMERA_POWER_BUTTON_HOLD_PERIOD);	// hold
	// TODO: release button Board_CamButtonPress_Power(0)
}


void camera_bsp_init() {
	// TODO: pin init, etc.

	camera_vcam_enable(false);
	// TODO Board_CamButtonPress_Power(0)
}


void camera_task(void *pvParameters) {
	// camera_bsp_init already called by the time we're here

	// Loop: turn on the camera, let it take a picture, turn it off.
	SteadyTimer timerImage(CAMERA_SNAPSHOT_PERIOD);
	while (true) {
		// Apply power to the camera and push the power button.
		camera_vcam_enable(true);
		SteadyClock::Delay(CAMERA_VCAM_SETTLE_PERIOD);
		camera_pressbutton_power(CAMERA_POWER_BUTTON_HOLD_PERIOD);

		// Wait for the camera to start up, take a picture, and shut itself
		// down.
		SteadyClock::Delay(CAMERA_AUTOEXEC_RUNTIME);

		// Yank the battery.
		camera_vcam_enable(false);

		// Go to sleep until it's time to take the next picture.
		timerImage.delayUntilElapsed();		// resets the timer, too
	}
}
