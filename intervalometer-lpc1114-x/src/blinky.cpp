//! @file
//! @author Jon Dubovsky, 28 Oct 2015
//#include "board.h"
#include "FreeRTOS.h"
#include "task.h"

volatile bool g_ledState = false;

extern "C" {

/* LED0 toggle thread */
void vLEDTask0(void *pvParameters) {
	//bool LedState = false;
	while (1) {
		g_ledState = !g_ledState;
		Board_LED_Set(0, g_ledState);
		//Board_LED_Set(0, LedState);
		//LedState = (bool) !LedState;

		vTaskDelay(configTICK_RATE_HZ + configTICK_RATE_HZ/10);
	}
}

#if 0		// unused
/* LED1 toggle thread */
void vLEDTask1(void *pvParameters) {
	bool LedState = false;
	while (1) {
		Board_LED_Set(1, LedState);
		LedState = (bool) !LedState;

		vTaskDelay(configTICK_RATE_HZ*2);
	}
}
#endif

/* LED2 toggle thread */
void vLEDTask2(void *pvParameters) {
	//bool LedState = false;
	while (1) {
		g_ledState = !g_ledState;
		Board_LED_Set(0, g_ledState);
		//Board_LED_Set(2, LedState);
		//LedState = (bool) !LedState;

		vTaskDelay(configTICK_RATE_HZ);
	}
}

}	// end extern "C"
