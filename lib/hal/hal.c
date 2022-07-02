#include "hal.h"

void _disableDigitalInputBuffersForAdc() {
	PORTA.PIN1CTRL &= ~PORT_ISC_gm;
	PORTA.PIN1CTRL |= PORT_ISC_INPUT_DISABLE_gc;
	PORTA.PIN5CTRL &= ~PORT_ISC_gm;
	PORTA.PIN5CTRL |= PORT_ISC_INPUT_DISABLE_gc;
}

void _setupUsartPins() {
    USART_PORT.DIRSET = USART_PIN_TX;
    USART_PORT.DIRCLR = USART_PIN_RX;
}

void halSetup(void) {
    LED_PORT.DIRSET = LED_PIN;
    _disableDigitalInputBuffersForAdc();
    _setupUsartPins();
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