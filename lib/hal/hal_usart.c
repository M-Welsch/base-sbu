/*
 * usart.c
 *
 * Created: 06.06.2020 21:58:36
 *  Author: Max
 */ 

#ifdef __AVR_ATtiny816__
#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hal_usart.h"
#include "flags.h"

void _usartConfigurePins() {
    USART_PORT.DIRSET = USART_PIN_TX;
    USART_PORT.DIRCLR = USART_PIN_RX;
}

void disable_usart_tx(void) {
	PORTB.DIRCLR = PIN2_bm;
}

void USART0_set_baud_rate() {
	USART0.BAUD = UROUND(64UL*F_CPU/16/BAUD_RATE);
}

void USART0_enable_periperials() {
	USART0.CTRLB = USART_RXEN_bm | USART_TXEN_bm;
}

void USART0_enable_rx_complete_interrupt() {
	USART0.CTRLA |= USART_RXCIE_bm;
}

void usartInit(void) {
	_usartConfigurePins();
	USART0_set_baud_rate();
	USART0_enable_periperials();
	USART0_enable_rx_complete_interrupt();
}

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

void USART0_sendString_w_newline_eol(const char *s) {
	USART0_sendString(s);
	USART0_sendChar('\n');
	USART0_sendChar('\0');  // might be necessary for bcu
}

uint8_t USART0_receive_complete() {
	return USART0.STATUS & USART_RXCIF_bm;
}

uint8_t USART0_read()
{
	
	while (!(USART0_receive_complete()))
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
		*receive_buffer = USART0_read();
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

/* Interrupts */

ISR(USART0_RXC_vect) {
	g_usartReceiveBuffer[0] = '\0';
	USART0_read_string(g_usartReceiveBuffer, 20);
	USART0_sendString_w_newline_eol(g_usartReceiveBuffer);
	g_usart0ReceiveComplete = true;
}