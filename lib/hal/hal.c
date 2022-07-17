#include <avr/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <string.h>
#include <stdio.h>

#include "hal.h"
#include "hal_adc.h"
#include "hal_led.h"
#include "hal_display.h"
#include "hal_powerpath.h"
#include "hal_buttons.h"
#include "hal_rtc.h"
#include "hal_usart.h"

/* Dimming */

uint16_t _getDefaultDisplayDimmingValueFromEeprom() {
	return 0xFFFF;
}

uint16_t _getDefaultLedDimmingValueFromEeprom() {
	return 0xFFFF;
}

void _dimmerForLedAndDisplayInit() {
	
	/* disable all TCA0 related interrupts */
	TCA0.SINGLE.INTCTRL &= ~(TCA_SINGLE_CMP2EN_bm | TCA_SINGLE_CMP1EN_bm | TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_OVF_bm);

	/* following steps in datasheet from chapter 20.3.3.4.1 */
	
	/* step 1.0: select waveform generation mode (single-slope pwm) */
	TCA0.SINGLE.CTRLB |= TCA_SINGLE_WGMODE_SINGLESLOPE_gc;
	
	/* step 1.1: setting up clock (prescaler = 1) */
	TCA0.SINGLE.CTRLA |= TCA_SINGLE_CLKSEL_DIV1_gc;
	
	/* step 1.2 setting up period time (maximum for max. resolution) */
	TCA0.SINGLE.PERBUF = 0xFFFF;
	
	/* step 2: TCA is counting tick not events */
	TCA0.SINGLE.EVCTRL &= ~(TCA_SINGLE_CNTEI_bm);
	
	/* step 3.1: map TCA0 WO1 to alternate hw-pin (for HMI LED)
		         WO0 (PB0) is used for the Display Backlight
	             WO1 (PB4) is used for the HMI LED
				      default hw-pin would be PB1
					  HMI led is connected to PB4 */
	PORTMUX.CTRLC |= PORTMUX_TCA01_bm;
	
	/* step 3.2: override port output settings */
	
	TCA0.SINGLE.CTRLB |= TCA_SINGLE_CMP0EN_bm; // Display BL: WO0 (PB0)
	TCA0.SINGLE.CTRLB |= TCA_SINGLE_CMP1EN_bm; // HMI LED: WO1 (PB4)
	
	/* initial dimming values for display and hmi led */
	setDisplayPwm(_getDefaultDisplayDimmingValueFromEeprom());
	setLedPwm(_getDefaultLedDimmingValueFromEeprom());
	
	/* enable TCA */
	TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;
}

/* Sleep */

void goto_sleep_standby()
{
	SLPCTRL.CTRLA |= SLPCTRL_SMODE_STDBY_gc;
	SLPCTRL.CTRLA |= SLPCTRL_SEN_bm;
	sleep_cpu();
}

/* Interface */

void halInit(void) {
	ledInit();
    usartInit();
    configureDisplayPins();
	powerpathInit();
	buttonsInit();
    _dimmerForLedAndDisplayInit();
	adcInit();
	rtcInit();
	sei();
}

/* ISRs */

ISR(BADISR_vect) {
	/* This routine is called if a non defined interrupt-vector is requested */
	char _buffer[32];
	sprintf(_buffer, "bad ISR: %x\n", CPUINT_LVL0PRI);
	USART0_sendString(_buffer);
	for(uint8_t c = 0; c <10; c++ ) {
		ledPinHigh();
		_delay_ms(10);
		ledPinLow();
	_delay_ms(100);
	}
}

ISR(RTC_CNT_vect) {
	RTC.INTFLAGS |= RTC_CMP_bm;
	ledPinHigh();
	//USART0_sendString("CMP\n");
	//flag_wakeup_by_rtc = true;
	//next_pwr_state = display_on;
	//transition_to_pwr_state(next_pwr_state);
}
