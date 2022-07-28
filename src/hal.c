#include <avr/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>

#include "hal.h"

/* USART */

#define F_CPU 3333333
#define BAUD_RATE 9600
#define UROUND(x) ((2UL*(x)+1)/2)

#define USART_PORT PORTB
#define USART_PIN_TX PIN2_bm
#define USART_PIN_RX PIN3_bm

void _usartConfigurePins() {
    USART_PORT.DIRSET = USART_PIN_TX;
    USART_PORT.DIRCLR = USART_PIN_RX;
}

void _USART0DisableTx(void) {
	PORTB.DIRCLR = PIN2_bm;
}

void _USART0SetBaudRate() {
	USART0.BAUD = UROUND(64UL*F_CPU/16/BAUD_RATE);
}

void _USART0EnablePeriperials() {
	USART0.CTRLB = USART_RXEN_bm | USART_TXEN_bm;
}

void _USART0EnableRxCompleteInterrupt() {
	USART0.CTRLA |= USART_RXCIE_bm;
}

void _usartInit(void) {
	_usartConfigurePins();
	_USART0SetBaudRate();
	_USART0EnablePeriperials();
	_USART0EnableRxCompleteInterrupt();
}

void _USART0SendChar(const char c) {
	while((USART0.STATUS & USART_DREIF_bm) == 0)
		;
	USART0.TXDATAL = c;
}

void _USART0SendString(const char *s) {
	for(uint16_t i = 0; i < strlen(s); i++) {
		_USART0SendChar(s[i]);
	}
}

void USART0_sendString_w_newline_eol(const char *s) {
	_USART0SendString(s);
	_USART0SendChar('\n');
	_USART0SendChar('\0');  // might be necessary for bcu
}

uint8_t _USART0ReceiveComplete() {
	return USART0.STATUS & USART_RXCIF_bm;
}

uint8_t _USART0Read()
{
	
	while (!(_USART0ReceiveComplete()))
	{
		;
	}
	return USART0.RXDATAL;
}

void USART0_read_string(char *receive_buffer, uint8_t maxlen) {
	if (maxlen >= LEN_USART_RECEIVE_BUFFER) {
		maxlen = LEN_USART_RECEIVE_BUFFER-1;  // one byte is needed for '\0'
	}
	uint8_t i = 0;
	while(i <= maxlen) {
		i++;
		*receive_buffer = _USART0Read();
		if ((*receive_buffer == '\n') || (*receive_buffer == '\0')) { // || (*receive_buffer == '\r')) {  //remove this \r stuff when HIL Tests run
			*receive_buffer = '\0';
			break;
		}
		receive_buffer++;
	}
	if (i > maxlen) {
		*receive_buffer = '\0';
	}
}

/* ADC */

void _adcDisableDigitalInputBuffersForAdc() {
	PORTA.PIN1CTRL &= ~PORT_ISC_gm;
	PORTA.PIN1CTRL |= PORT_ISC_INPUT_DISABLE_gc;
	PORTA.PIN5CTRL &= ~PORT_ISC_gm;
	PORTA.PIN5CTRL |= PORT_ISC_INPUT_DISABLE_gc;
}

void _adcStartConversion() {
	ADC0.COMMAND = ADC_STCONV_bm;
}

void _adcWaitForConvesionComplete()
{
	while ( !(ADC0.INTFLAGS & ADC_RESRDY_bm) )
	{
		;
	}
}

void _adcInit10bitResolution()
{
	ADC0.CTRLA |= ADC_RESSEL_10BIT_gc;
}

void _adcEnable()
{
	ADC0.CTRLA |= ADC_ENABLE_bm;
}

void _adc4V34Reference()
{
	VREF.CTRLA = VREF_ADC0REFSEL_4V34_gc;
}

void _adcDoConversion()
{
	_adcStartConversion();
	_adcWaitForConvesionComplete();
	
	/* Clear the interrupt flag by writing 1: */
	ADC0.INTFLAGS = ADC_RESRDY_bm;
}

void _adcConfigForTemperatureMeasurement() {
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

void _adcInit() {
	/* Digital Input buffers are being disabled during pin init */
	_adcDisableDigitalInputBuffersForAdc();
	_adcInit10bitResolution();
	_adc4V34Reference();
	_adcEnable();
}

uint16_t adcResult3v3() {
	ADC0.MUXPOS = ADC_MUXPOS_AIN5_gc;
	_adcDoConversion();
	return ADC0.RES;
}

uint16_t adcResultInCur() {
	ADC0.MUXPOS = ADC_MUXPOS_AIN1_gc;
	_adcDoConversion();
	return ADC0.RES;
}

uint16_t adcTemperature() {
    //_adcConfigForTemperatureMeasurement();  // Todo: implement
	ADC0.MUXPOS = ADC_MUXPOS_TEMPSENSE_gc;
	_adcDoConversion();
	return ADC0.RES;
}

bool adc3v3present() {
	if (adcResult3v3() > 900) {
		return true;
	}
	else {
		return false;
	}
}

/* Buttons */

#define BUTTON_PORT PORTA
#define BUTTON_0 PIN2_bm //PA2
#define BUTTON_0_CTRL PIN2CTRL //PA2
#define BUTTON_1 PIN3_bm //PA3
#define BUTTON_1_CTRL PIN3CTRL //PA3

void _buttonsInit() {
	BUTTON_PORT.DIRCLR = (BUTTON_0 | BUTTON_1);
	BUTTON_PORT.BUTTON_0_CTRL |= (PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc);
	BUTTON_PORT.BUTTON_1_CTRL |= (PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc);
    button0Pressed = false;
    button1Pressed = false;
}

bool button0PinHigh(void) {
	return (BUTTON_PORT.IN & BUTTON_0);
}

bool button1PinHigh(void) {
	return (BUTTON_PORT.IN & BUTTON_1);
}

buttonState_t button0(void) {
    buttonState_t retval = not_pressed;
    if (!button0PinHigh())
        retval = pressed;
    return retval;
}

buttonState_t button1(void) {
    buttonState_t retval = not_pressed;
    if (!button1PinHigh())
        retval = pressed;
    return retval;
}

/* Display */

#define dis_rs_port PORTA
#define dis_rs PIN6_bm //PA6

#define DISPLAY_E_PORT PORTA
#define DISPLAY_E_PIN PIN7_bm //PA7

#define DISPLAY_PWM_PORT PORTB
#define dis_pwm PIN0_bm //PB0

/* PC0 - PC3: Display Data Bus */
#define DISPLAY_DATA_PORT PORTC
#define dis_db4 PIN0_bm
#define dis_db5 PIN1_bm
#define dis_db6 PIN2_bm
#define dis_db7 PIN3_bm

char _displayLine1Buffer[17];
uint16_t _displayDimmingValue = 0;

void setDisplayPwm(uint16_t dimming_value) {
    _displayDimmingValue = dimming_value;
    TCA0.SINGLE.CMP0 = _displayDimmingValue;
}

void _displayConfigurePins(void) {
    DISPLAY_PWM_PORT.DIRSET = dis_pwm;
    DISPLAY_E_PORT.DIRSET = DISPLAY_E_PIN;
    dis_rs_port.DIRSET = dis_rs;
    DISPLAY_DATA_PORT.DIRSET = (dis_db4 | dis_db5 | dis_db6 | dis_db7);
}

void _displayEnable() {
	DISPLAY_E_PORT.OUTSET = DISPLAY_E_PIN;
	_delay_us(10);
	DISPLAY_E_PORT.OUTCLR = DISPLAY_E_PIN;
	_delay_us(10);
}

#define DISPLAY_DATA_MASK 0x0f //Display Data Pins are connected to PORTC0-3

void _displayDataPins(uint8_t data_nibble) {
	DISPLAY_DATA_PORT.OUT &= ~DISPLAY_DATA_MASK;
	DISPLAY_DATA_PORT.OUT |= (data_nibble & DISPLAY_DATA_MASK);
}

void _displaySetRs(void) {
	dis_rs_port.OUTSET = dis_rs;
}

void _displayClearRs(void) {
	dis_rs_port.OUTCLR = dis_rs;
}


void displayPwmLow(void) {
    TCA0.SINGLE.CMP0 = 0;
}

void displayPinsAllLow(void) {
	_displayClearRs();
	_displayDataPins(0x00);
	displayPwmLow();
}

void displayPwmHigh(void) {
    setDisplayPwm(_displayDimmingValue);
}

void displayInit(void) {
	/* refering to datasheet SPLC780D and (more importantly) lcd.py of "old" BaSe implementation*/
	_delay_ms(100);
	_displayClearRs();
	
	_displayDataPins(0x03);
	_displayEnable(1);
	_delay_ms(1);
	_displayDataPins(0x03);
	_displayEnable(1);
	_delay_ms(1);
	
	_displayDataPins(0x03);
	_displayEnable(1);
	_delay_ms(1);
	_displayDataPins(0x02);
	_displayEnable(1);
	_delay_ms(1);
	
	_displayDataPins(0x02);
	_displayEnable(1);
	_delay_ms(1);
	/* the following instruction ...
	   DB7 DB6 DB5 DB4 |
	   N   F   X   X   | N = HIGH => 2 lined display. F = don't care = 0
	   1   0   0   0 => 0x8 */
	_displayDataPins(0x08);
	_displayEnable(1);
	_delay_ms(1);
	
	_displayDataPins(0x00);
	_displayEnable(1);
	_delay_ms(1);
	/* The following instruction ...
	   DB7 DB6 DB5 DB4 |
	   1   D   C   B   |
	       |   |   +------B: Cursor ON/OFF control bit. B = LOW => Cursor blink is off
		   |   +----------C: Cursor Control Bit. C = LOW => Cursor disappears
		   +--------------D: Display ON/OFF control bit. D = HIGH => Display is on
	   1   1   0   0 => 0xC */
	_displayDataPins(0x0C);
	_displayEnable(1);
	_delay_ms(1);
	
	_displayDataPins(0x00);
	_displayEnable(1);
	_delay_ms(1);
	_displayDataPins(0x06);
	_displayEnable(1);
	_delay_ms(1);
	
	_displayDataPins(0x00);
	_displayEnable(1);
	_delay_ms(1);
	
	/* DB7 DB6 DB5 DB4 |
	   0   1   I/D S
	           |   +--- S: LOW => no shift
			   +------- I/D: HIGH => shift to the right (if S == HIGH)
			   0   1   1   0x6*/
	_displayDataPins(0x06);
	_displayEnable(1);
	_delay_ms(1);
	
	_displayDataPins(0x00);
	_displayEnable(1);
	_delay_ms(1);
	_displayDataPins(0x01);
	_displayEnable(1);
	_delay_ms(1);
}

void displayWriteChar(char s) {
	_displaySetRs();
	
	_displayDataPins(s >> 4);
	_displayEnable(1);
	
	_displayDataPins(s);
	_displayEnable(1);
}

void displayNextLine(void) {
	_displayClearRs();
	_displayDataPins(0xC);
	_displayEnable(1);
	_displayDataPins(0x0);
	_displayEnable(1);
}

void displayDim(uint16_t value) {
    if (g_currentState != stateStandby)
        setDisplayPwm(value);
}

void displayBufferLine1(const char *line1) {
	strcpy(_displayLine1Buffer, line1);
}

void displayWriteBothLines(const char *line2) {
	char buffer[33];
	sprintf(buffer, "%s\n%s", _displayLine1Buffer, line2);
	displayClear();
	displayWriteString(buffer);
}

void displayWriteString(const char *s) {
	displayClear();
    if (g_currentState != stateStandby) {
        _displaySetRs();
        while(*s!='\0') {
            if(*s == '\n') {
                displayNextLine();
            }
            else {
                displayWriteChar(*s);
            }
            s++;
        }
    }
}

void display_return_home(void) {
	_displayClearRs();
	_displayDataPins(0x00);
	_displayEnable(1);
	_displayDataPins(0x02);
	_displayEnable(1);
	_delay_ms(1);
}

void displayClear(void) {
	_displayClearRs();
	_displayDataPins(0x00);
	_displayEnable(1);
	_displayDataPins(0x01);
	_displayEnable(1);
	_delay_ms(1);
}

/* LED */

#define LED_PORT PORTB
#define LED_PIN PIN4_bm

uint16_t _ledDimmingValue = 0;

void ledPinHigh(void) {
    setLedPwm(_ledDimmingValue);
}

void setLedPwmOff() {
     TCA0.SINGLE.CMP1 = 0;
}

void ledPinLow(void) {
    setLedPwmOff();
}

void setLedPwm(uint16_t dimming_value) {
    _ledDimmingValue = dimming_value;
    TCA0.SINGLE.CMP1 = _ledDimmingValue;
}

void _ledInit() {
    LED_PORT.DIRSET = LED_PIN;
}

baseSbuError_t ledDim(uint16_t dimmingValue) {
    if (g_currentState == stateStandby) return hardware_call_refused;
    setLedPwm(dimmingValue);
    return success;
}

baseSbuError_t ledOn(void) {
    if (g_currentState == stateStandby) return hardware_call_refused;
    ledPinHigh();
    return success;
}

baseSbuError_t ledOff(void) {
    ledPinLow();
    return success;
}

/* Powerpath */

#define SPLY_5V_EN_PORT PORTA
#define SPLY_5V_EN_PIN PIN4_bm

#define SPLY_BCU_DIS_PORT PORTB
#define SPLY_BCU_DIS_PIN PIN1_bm

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

void activate5vRail() {
    USART0_sendString_w_newline_eol("Activating 5V");
    pin5vEnToHigh();
}

void deactivate5vRail() {
    USART0_sendString_w_newline_eol("Deactivating 5V");
    pin5vEnToLow();
}

void activateBcuSupply() {
    USART0_sendString_w_newline_eol("Activating BCU");
    disableBpiSplyToLow();
}

void deactivateBcuSupply() {
    USART0_sendString_w_newline_eol("Deactivating BCU");
    disableBpiSplyToHigh();
}

void _powerpathInit() {
    activate5vRail();
    activateBcuSupply();
	SPLY_BCU_DIS_PORT.DIRSET = SPLY_BCU_DIS_PIN;
	SPLY_5V_EN_PORT.DIRSET = SPLY_5V_EN_PIN;
}

/* RTC */

void _rtcResetCounter()
{
	RTC.CNTL = 0;
	RTC.CNTH = 0;
}

void _rtcWaitForAllStatusBitsZero()
{
	while (RTC.STATUS > 0) {
		;
	}
}

void _rtcSetPrescalerAndEnable()
{
	/* active in standby mode, prescaler = 32768, periperial enabled */
	//Todo: uncomment 32 seconds configuration !!
	RTC.CTRLA = RTC_RUNSTDBY_bm | RTC_PRESCALER_DIV32768_gc | RTC_RTCEN_bm; // <- one counter increment represents 32 seconds
	//RTC.CTRLA = RTC_RUNSTDBY_bm | RTC_PRESCALER_DIV1024_gc | RTC_RTCEN_bm; // <- 1 second
}

void rtcActivateCompareInterrupt()
{
	RTC.INTCTRL = RTC_CMP_bm;
}

void rtcDeactivateCompareInterrupt()
{
	RTC.INTCTRL &= ~RTC_CMP_bm;
}

void _rtcUseInternal1k024Oscillator()
{
	/* actually its the 32768khz oscillator, divided by 32 => fclk = 1,024kHz */
	RTC.CLKSEL = RTC_CLKSEL_INT1K_gc;
}

void _rtcInit(void) {
	_rtcWaitForAllStatusBitsZero();
	_rtcSetPrescalerAndEnable();
	// rtcActivateCompareInterrupt();
	_rtcUseInternal1k024Oscillator();
}


void _rtcSetCmpRegister(uint16_t register_value_next_bu) {
	_rtcResetCounter();
	if (register_value_next_bu == 0) {
		register_value_next_bu = 1;
	}
	RTC.CMP = register_value_next_bu;
}

void rtcSetWakeupInSeconds(uint16_t secondsToNextBackup) {
	uint16_t register_value_next_bu = secondsToNextBackup / 32;
    _rtcSetCmpRegister(register_value_next_bu);
}

void rtcGetSecondsToNextBuAndCount(char *buffer) {
    sprintf(buffer, "CMP:%d, CNT:%d", RTC.CMP, RTC.CNT);
}

/**
 * @brief checks if ctc counter reached the compare value and rtc cmp interrupt is enabled
 * 
 * @return true
 * @return false 
 */
bool rtcTimerDue() {
	char _buffer[48];
	sprintf(_buffer, "CMP:%d, CNT:%d, INT:%d", RTC.CMP, RTC.CNT, RTC.INTCTRL);
	USART0_sendString_w_newline_eol(_buffer);
	return (RTC.CNT > RTC.CMP) && (RTC.INTCTRL & RTC_CMP_bm);
}

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

void halInit(void) {
	_powerpathInit();
	_ledInit();
    _usartInit();
    _displayConfigurePins();
	_buttonsInit();
    _dimmerForLedAndDisplayInit();
	_adcInit();
	_rtcInit();
	sei();
}

/* Interrupt Service Routines */

ISR(BADISR_vect) {
	/* This routine is called if a non defined interrupt-vector is requested */
	char _buffer[32];
	sprintf(_buffer, "bad ISR: %x", CPUINT_LVL0PRI);
	USART0_sendString_w_newline_eol(_buffer);
	for(uint8_t c = 0; c <10; c++ ) {
		ledOn();
		_delay_ms(10);
		ledOff();
	_delay_ms(100);
	}
}

ISR(RTC_CNT_vect) {
	RTC.INTFLAGS |= RTC_CMP_bm;
	USART0_sendString_w_newline_eol("CMP");
	rtcDeactivateCompareInterrupt();
}

ISR(USART0_RXC_vect) {
	g_usartReceiveBuffer[0] = '\0';
	USART0_read_string(g_usartReceiveBuffer, 20);
	USART0_sendString_w_newline_eol(g_usartReceiveBuffer);
	g_usart0ReceiveComplete = true;
}

ISR(PORTA_PORT_vect)
{
	// use the following lines to see the interrupt level (Datasheet p.111)
	//sprintf(buffer,"iflags = %d, status = %d\n", PORTA_INTFLAGS, CPUINT.STATUS);
	//USART0_sendString(buffer);
	if(PORTA_INTFLAGS & BUTTON_0) {
		button0Pressed = true;
		PORTA_INTFLAGS &= BUTTON_0;
	}
	
	if(PORTA_INTFLAGS & BUTTON_1) {
		button1Pressed = true;
		PORTA_INTFLAGS &= BUTTON_1;
	}
}
