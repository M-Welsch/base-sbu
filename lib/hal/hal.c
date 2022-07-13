#include <avr/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <string.h>
#include <stdio.h>

#include "hal.h"

uint16_t _ledDimmingValue = 0;
uint16_t _displayDimmingValue = 0;

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

/* USART */

void USART0_sendChar(const char c) {
	while((USART0.STATUS & USART_DREIF_bm) == 0)
		;
	USART0.TXDATAL = c;
}

void USART0_sendString(const char *s) {
	for(uint16_t i = 0; i < strlen(s); i++) {
		USART0_sendChar(s[i]);
	}
}

void disable_usart_tx(void) {
	PORTB.DIRCLR = PIN2_bm;
}

/* Dimming */

void setDisplayPwm(uint16_t dimming_value) {
    _displayDimmingValue = dimming_value;
    TCA0.SINGLE.CMP0 = _displayDimmingValue;
}

void setDisplayPwmOff() {
    TCA0.SINGLE.CMP0 = 0;
}

void setLedPwm(uint16_t dimming_value) {
    _ledDimmingValue = dimming_value;
    TCA0.SINGLE.CMP1 = _ledDimmingValue;
}

void setLedPwmOff() {
     TCA0.SINGLE.CMP1 = 0;
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
	setDisplayPwm(_getDefaultDisplayDimmingValueFromEeprom());
	setLedPwm(_getDefaultLedDimmingValueFromEeprom());
	
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

void displayEnable() {
	DISPLAY_E_PORT.OUTSET = DISPLAY_E_PIN;
	_delay_us(10);
	DISPLAY_E_PORT.OUTCLR = DISPLAY_E_PIN;
	_delay_us(10);
}

#define DISPLAY_DATA_MASK 0x0f //Display Data Pins are connected to PORTC0-3

void displayDataPins(uint8_t data_nibble) {
	DISPLAY_DATA_PORT.OUT &= ~DISPLAY_DATA_MASK;
	DISPLAY_DATA_PORT.OUT |= (data_nibble & DISPLAY_DATA_MASK);
}

void displaySetRs(void) {
	dis_rs_port.OUTSET = dis_rs;
}

void displayClearRs(void) {
	dis_rs_port.OUTCLR = dis_rs;
}

void displayPwmLow(void) {
    setDisplayPwm(0);
}

void displayPwmHigh(void) {
    setDisplayPwm(_displayDimmingValue);
}

/* LED */

void _configureLedPin() {
    LED_PORT.DIRSET = LED_PIN;
}

void ledPinHigh(void) {
    setLedPwm(_ledDimmingValue);
}

void ledPinLow(void) {
    setLedPwmOff();
}

/* Powerpath */

void _configurePowerpathPins() {
	SPLY_BCU_DIS_PORT.DIRSET = SPLY_BCU_DIS_PIN;
	SPLY_5V_EN_PORT.DIRSET = SPLY_5V_EN_PIN;
}

void disableBpiSplyToHigh() {
	SPLY_BCU_DIS_PORT.OUTSET = SPLY_BCU_DIS_PIN;
}

void disableBpiSplyToLow() {
	SPLY_BCU_DIS_PORT.OUTCLR = SPLY_BCU_DIS_PIN;
}

void pin5vEnToHigh() {
	SPLY_5V_EN_PORT.OUTSET = SPLY_5V_EN_PIN;
}

void pin5vEnToLow() {
	SPLY_5V_EN_PORT.OUTCLR = SPLY_5V_EN_PIN;
}

/* Buttons */

void _configureButtonPins() {
	BUTTON_PORT.DIRCLR = (BUTTON_0 | BUTTON_1);
	BUTTON_PORT.BUTTON_0_CTRL |= (PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc);
	BUTTON_PORT.BUTTON_1_CTRL |= (PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc);
}

bool button0PinHigh(void) {
	return (BUTTON_PORT.IN & BUTTON_0);
}

bool button1PinHigh(void) {
	return (BUTTON_PORT.IN & BUTTON_1);
}

/* ADC */

void adc_start_conversion() {
	ADC0.COMMAND = ADC_STCONV_bm;
}

void adc_wait_for_convesion_to_complete()
{
	while ( !(ADC0.INTFLAGS & ADC_RESRDY_bm) )
	{
		;
	}
}

void adc_init_10bit_resolution()
{
	ADC0.CTRLA |= ADC_RESSEL_10BIT_gc;
}

void adc_enable()
{
	ADC0.CTRLA |= ADC_ENABLE_bm;
}

void adc_4V34_reference()
{
	VREF.CTRLA = VREF_ADC0REFSEL_4V34_gc;
}

void adc_do_conversion()
{
	adc_start_conversion();
	adc_wait_for_convesion_to_complete();
	
	/* Clear the interrupt flag by writing 1: */
	ADC0.INTFLAGS = ADC_RESRDY_bm;
}

uint16_t adc_measure_3v3() {
	ADC0.MUXPOS = ADC_MUXPOS_AIN5_gc;
	adc_do_conversion();
	return ADC0.RES;
}

uint16_t adc_measure_input_current() {
	ADC0.MUXPOS = ADC_MUXPOS_AIN1_gc;
	adc_do_conversion();
	return ADC0.RES;
}

void adcConfigForTemperatureMeasurement() {
	// to be implemented correctly, see datasheet s.437, chapter: 30.3.2.6
	VREF.CTRLA = VREF_ADC0REFSEL_1V1_gc;
	ADC0.CTRLC &= ~(0x0);  // write REFSEL to 0x0
	ADC0.MUXPOS = ADC_MUXPOS_TEMPSENSE_gc;  // double check
	ADC0.CTRLD = ADC_SAMPLEN0_bm; // corect bit for INITDLY > 32µs x fclk_adc
	ADC0.CTRLC = ADC_SAMPCAP_bm; // correct val for 5pF
	int8_t sigrow_offset = SIGROW.TEMPSENSE1; // Read signed value from signature row
	uint8_t sigrow_gain = SIGROW.TEMPSENSE0;
	// Read unsigned value from signature row
	uint16_t adc_reading = ADC0.RES;
	// ADC conversion result with 1.1 V internal reference
	uint32_t temp = adc_reading - sigrow_offset;
	temp *= sigrow_gain; // Result might overflow 16 bit variable (10bit+8bit)
	temp += 0x80;
	// Add 1/2 to get correct rounding on division below
	temp >>= 8;
	// Divide result to get Kelvin
	uint16_t temperature_in_K = temp;
}

uint16_t adc_measure_temperature() {
	// Fixme: measurement like datasheet s.437, chapter: 30.3.2.6
	ADC0.MUXPOS = ADC_MUXPOS_TEMPSENSE_gc;
	adc_do_conversion();
	return ADC0.RES;
}

bool vcc3v3_present() {
	if (adc_measure_3v3() > 900) {
		return true;
	}
	else {
		return false;
	}
}

void _adcInit() {
	/* Digital Input buffers are being disabled during pin init */
	adc_init_10bit_resolution();
	adc_4V34_reference();
	adc_enable();
}

/* RTC */

void rtc_reset_counter()
{
	RTC.CNTL = 0;
	RTC.CNTH = 0;
}

void rtc_wait_for_all_status_bits_zero()
{
	while (RTC.STATUS > 0) {
		;
	}
}

void rtc_set_prescaler_and_enable()
{
	/* active in standby mode, prescaler = 32768, periperial enabled */
	//Todo: uncomment 32 seconds configuration !!
	RTC.CTRLA = RTC_RUNSTDBY_bm | RTC_PRESCALER_DIV32768_gc | RTC_RTCEN_bm; // <- one counter increment represents 32 seconds
	//RTC.CTRLA = RTC_RUNSTDBY_bm | RTC_PRESCALER_DIV1024_gc | RTC_RTCEN_bm; // <- 1 second
}

bool rtc_enabled() {
	return RTC.CTRLA & RTC_RTCEN_bm;
}

void rtc_set_compare_interrupt()
{
	RTC.INTCTRL = RTC_CMP_bm;
}

void rtc_use_internal_1k024_oscillator()
{
	/* actually its the 32768khz oscillator, divided by 32 => fclk = 1,024kHz */
	RTC.CLKSEL = RTC_CLKSEL_INT1K_gc;
}

void _rtcInit(void) {
	//flag_wakeup_by_rtc = false;
	rtc_wait_for_all_status_bits_zero();
	rtc_set_prescaler_and_enable();
	rtc_set_compare_interrupt();
	rtc_use_internal_1k024_oscillator();
}


void rtcSetCmpRegister(uint16_t register_value_next_bu) {
	rtc_reset_counter();
	if (register_value_next_bu == 0) {
		register_value_next_bu = 1;
	}
	RTC.CMP = register_value_next_bu;
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
	_configureLedPin();
    _configureUsartPins();
    _configureDisplayPins();
	_configurePowerpathPins();
	_configureButtonPins();
    _disableDigitalInputBuffersForAdc();
    _dimmerForLedAndDisplayInit();
	_adcInit();
	_rtcInit();
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
