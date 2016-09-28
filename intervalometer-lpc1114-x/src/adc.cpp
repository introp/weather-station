//! @file
/*
Based on example file from NXP.
*/

#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>


//! BSP init for A/D converter system.
static void adc_bsp_init() {
#if (defined(BOARD_NXP_XPRESSO_11U14) || defined(BOARD_NGX_BLUEBOARD_11U24))
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 11, FUNC2);
#elif defined(BOARD_NXP_XPRESSO_11C24)
	//Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_11, FUNC2);
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_11, IOCON_FUNC2);	// what is func2?
#else
	#error "Pin muxing for ADC not configured"
#endif
}

uint16_t zorks;
//! @returns a 0.16 fixed-point uint16_t from x (where 0.0 <= x <= 1.0).
constexpr uint16_t Q0_16(float x) {
	return static_cast<uint16_t>(65535/x);
}

//! Sample rate of the ADC loop, in Hz.
static const unsigned int ADC_SAMPLE_RATE = 100;
//! number of ADC channels to sample and filter
static const unsigned int ADC_NUM_CHANS = 2;
//! filtered ADC values
static unsigned int adc_filtered[ADC_NUM_CHANS];
//! ADC filter single-pole IIR coefficients, per channel.
//! RC time constant (d) equivalent gives COEFF=exp(-1/d)
//! so a d==1 per sample period => COEFF==0.3678
//! so at 10 Hz sample rate, d==0.1 (for 1 sec T) => COEFF==0.0000454, scales 0.16 to 2.97!
//! COEFF=1-exp(-2*pi*cutoff_freq/sample_freq)
//! Fs 100 Hz, Fc of 1 Hz ==> 0.0609 ==> 3991/65535
//! Assume sample rate of ADC_RATE (10 Hz),
static unsigned int adc_filter_term[ADC_NUM_CHANS] = {
	Q0_16(1.0/ADC_SAMPLE_RATE),		// temperature; Fc == 1 Hz
	Q0_16(1.0/ADC_SAMPLE_RATE),		// wind direction; Fc == 1 Hz
};


//! Handle interrupt from ADC
extern "C" void ADC_IRQHandler(void) {
	uint32_t result = LPC_ADC->GDR;		// read clears DONE flag
Chip_ADC_SetBurstCmd(LPC_ADC, DISABLE);
	if (ADC_DR_OVERRUN(result))
		return;		// overrun; invalid sample.
	if (!ADC_DR_DONE(result))
		return;		// not done!?! Invalid sample.
	unsigned int val = (result & 0x0000FFFFu);	// bottom 6 bits always read as zero (10 bit sample)
//! Macro to extract the ADC channel number from GDR results register.
#define ADC_DR_CHANNEL(_n) ((((_n) >> 24) & 0x07u))
	unsigned int chan = ADC_DR_CHANNEL(result);
	// run filter (on a copy to avoid update races)
	unsigned int filt = adc_filtered[chan] * adc_filter_term[chan];	// 0.16*0.16 => 0.32
	filt += (65535-adc_filter_term[chan]) * val;	// 0.16*0.16 => 0.32
	filt >>= 16;	// 0.32 => 0.16
	adc_filtered[chan] = filt;

	/* Waiting for A/D conversion complete */
	while (Chip_ADC_ReadStatus(LPC_ADC, ADC_CH0, ADC_DR_DONE_STAT) != SET) {
		taskYIELD();
	}

	/* Read ADC value */
	uint16_t dataADC;
	Chip_ADC_ReadValue(LPC_ADC, ADC_CH0, &dataADC);
	/* Print ADC value */
	DEBUGOUT("ADC=0x%x\r\n", dataADC);

}


//! A/D worker task.
void adc_task(void *pvParameters) {
	adc_bsp_init();

	// initialization
	ADC_CLOCK_SETUP_T ADCSetup;
	Chip_ADC_Init(LPC_ADC, &ADCSetup);		// defaults
#if 0
	Chip_ADC_SetSampleRate(LPC_ADC, &ADCSetup, ?????rate);
#endif
#ifdef SUP_ADC_POLLING
	Chip_ADC_EnableChannel(LPC_ADC, ADC_CH0, ENABLE);
#else
	// can't set global DONE interrupt in burst mode:
	//Chip_ADC_Int_SetGlobalCmd(LPC_ADC, ENABLE);
	Chip_ADC_EnableChannel(LPC_ADC, ADC_CH0, ENABLE);
	Chip_ADC_EnableChannel(LPC_ADC, ADC_CH1, ENABLE);
	// set to interrupt when the last channel is done
	Chip_ADC_Int_SetChannelCmd(LPC_ADC, ADC_CH1, ENABLE);
	Chip_ADC_SetStartMode(LPC_ADC, ADC_NO_START, ADC_TRIGGERMODE_RISING);
#endif

	while (true) {
#ifdef SUP_ADC_POLLING
		/* Start A/D conversion */
		Chip_ADC_SetStartMode(LPC_ADC, ADC_START_NOW, ADC_TRIGGERMODE_RISING);

		/* Waiting for A/D conversion complete */
		while (Chip_ADC_ReadStatus(LPC_ADC, ADC_CH0, ADC_DR_DONE_STAT) != SET) {
			taskYIELD();
		}

		/* Read ADC value */
		uint16_t dataADC;
		Chip_ADC_ReadValue(LPC_ADC, ADC_CH0, &dataADC);
		/* Print ADC value */
		DEBUGOUT("ADC=0x%x\r\n", dataADC);

		vTaskDelay(configTICK_RATE_HZ);		// 1 sec
#else
		// turn on burst mode
		Chip_ADC_SetBurstCmd(LPC_ADC, ENABLE);
		// interrupt will wake us when done
waitonsema
#endif		// #ifdef SUP_ADC_POLLING
	}
}
