#include <avr/delay.h>

#include "hal.h"

void _disableDigitalInputBuffersForAdc() {
	PORTA.PIN1CTRL &= ~PORT_ISC_gm;
	PORTA.PIN1CTRL |= PORT_ISC_INPUT_DISABLE_gc;
	PORTA.PIN5CTRL &= ~PORT_ISC_gm;
	PORTA.PIN5CTRL |= PORT_ISC_INPUT_DISABLE_gc;
}

void _configureUsartPins() {
    USART_PORT.DIRSET = USART_PIN_TX;
    USART_PORT.DIRCLR = USART_PIN_RX;
}

void ledPinHigh(void) {
    LED_PORT.OUTSET = LED_PIN;
}

void ledPinLow(void) {
    LED_PORT.OUTCLR = LED_PIN;
}

/* USART */

void set_pb2_txd_and_pb3_rxd(void) {
	PORTB.OUTSET = PIN2_bm;
	PORTB.DIRSET = PIN2_bm;
}

void disable_usart_tx(void) {
	PORTB.OUTCLR = PIN2_bm;
}

/* Dimming */

void setDisplayPWM(uint16_t dimming_value) {
    TCA0.SINGLE.CMP0 = dimming_value;
}

void setLedPWM(uint16_t dimming_value) {
    TCA0.SINGLE.CMP1 = dimming_value;
}


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
	setDisplayPWM(_getDefaultDisplayDimmingValueFromEeprom());
	setLedPWM(_getDefaultLedDimmingValueFromEeprom());
	
	/* enable TCA */
	TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;
}

/* Display */

void _configureDisplayPins(void) {
    DISPLAY_PWM_PORT.DIRSET = dis_pwm;
    DISPLAY_E_PORT.DIRSET = DISPLAY_E_PIN;
    dis_rs_port.DIRSET = dis_rs;
    DISPLAY_DATA_PORT.DIRSET = (dis_db4 | dis_db5 | dis_db6 | dis_db7);
}

void displayEnable(uint8_t duration_ms) {
	DISPLAY_E_PORT.OUTSET = DISPLAY_E_PIN;
	_delay_us(10);
	DISPLAY_E_PORT.OUTCLR = DISPLAY_E_PIN;
	_delay_us(10);
}

#define DISPLAY_DATA_MASK 0x0f //Display Data Pins are connected to PORTC0-3

void displayDataPins(uint8_t data_nibble) {
	DISPLAY_DATA_PORT.OUT &= ~DISPLAY_DATA_MASK
;
	DISPLAY_DATA_PORT.OUT |= (data_nibble & DISPLAY_DATA_MASK
);
}

void displaySetRs(void) {
	dis_rs_port.OUTSET = dis_rs;
}

void displayClearRs(void) {
	dis_rs_port.OUTCLR = dis_rs;
}

/* Interface */

void halInit(void) {
    LED_PORT.DIRSET = LED_PIN;
    _disableDigitalInputBuffersForAdc();
    _configureUsartPins();
    _configureDisplayPins();
    _dimmerForLedAndDisplayInit();
}