#include <avr/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

// #include "globals.h"
#include "hal.h"

/* USART */

#define F_CPU 3333333
#define BAUD_RATE 9600
#define UROUND(x) ((2UL*(x)+1)/2)

#define USART_PORT PORTB
#define USART_PIN_TX PIN2_bm
#define USART_PIN_RX PIN3_bm

void _usartInit(void) {
	/* configure pins */
    USART_PORT.DIRSET = USART_PIN_TX;
    USART_PORT.DIRCLR = USART_PIN_RX;	
	/* set baud rate */
	USART0.BAUD = UROUND(64UL*F_CPU/16/BAUD_RATE);
	/* enable periperials */
	USART0.CTRLB = USART_RXEN_bm | USART_TXEN_bm;
	/* enable rx interrupt */
	USART0.CTRLA |= USART_RXCIE_bm;
}

void _USART0SendChar(const char c) {
	while((USART0.STATUS & USART_DREIF_bm) == 0)
		;
	USART0.TXDATAL = c;
}

void USART0SendString(const char *s) {
	for(uint16_t i = 0; i < strlen(s); i++) {
		_USART0SendChar(s[i]);
	}
	_USART0SendChar('\n');
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
}

bool button0PinHigh(void) {
	return (BUTTON_PORT.IN & BUTTON_0);
}

bool button1PinHigh(void) {
	return (BUTTON_PORT.IN & BUTTON_1);
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
    USART0SendString("A 5V");
    pin5vEnToHigh();
}

void deactivate5vRail() {
    USART0SendString("Deactivating 5V");
    pin5vEnToLow();
}

void activateBcuSupply() {
    USART0SendString("Activating BCU");
    disableBpiSplyToLow();
}

void deactivateBcuSupply() {
    USART0SendString("Deactivating BCU");
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

void rtcActivateCompareInterrupt()
{
	RTC.INTCTRL = RTC_CMP_bm;
}

void rtcDeactivateCompareInterrupt()
{
	RTC.INTCTRL &= ~RTC_CMP_bm;
}

void _rtcInit(void) {
	/* wait until all statusbits are zero */
	while (RTC.STATUS > 0) {
		;
	}

	/* active in standby mode, prescaler = 32768, periperial enabled */
	RTC.CTRLA = RTC_RUNSTDBY_bm | RTC_PRESCALER_DIV32768_gc | RTC_RTCEN_bm; // <- one counter increment represents 32 seconds
	
	/* activate internal 1.024 kHz oscillator
	   actually its the 32768khz oscillator, divided by 32 => fclk = 1,024kHz */
	RTC.CLKSEL = RTC_CLKSEL_INT1K_gc;
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
	USART0SendString(_buffer);
	return (RTC.CNT > RTC.CMP) && (RTC.INTCTRL & RTC_CMP_bm);
}

void halInit(void) {
	_powerpathInit();
    _usartInit();
	_buttonsInit();
	_rtcInit();
	sei();
}

/* Interrupt Service Routines */

// ISR(RTC_CNT_vect) {
// 	RTC.INTFLAGS |= RTC_CMP_bm;
// 	//USART0SendString("CMP");
// 	rtcDeactivateCompareInterrupt();
// }