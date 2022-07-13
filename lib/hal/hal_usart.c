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
#include "hal.h"
#include "flags.h"

#define F_CPU 3333333
#define BAUD_RATE 9600
#define UROUND(x) ((2UL*(x)+1)/2)

void USART0_set_baud_rate() {
	USART0.BAUD = UROUND(64UL*F_CPU/16/BAUD_RATE);
}

void USART0_enable_periperials() {
	USART0.CTRLB = USART_RXEN_bm | USART_TXEN_bm;
}

void USART0_enable_rx_complete_interrupt() {
	USART0.CTRLA |= USART_RXCIE_bm;
}

void USART0_sendString_w_newline_eol(const char *s) {
	USART0_sendString(s);
	USART0_sendChar('\n');
	//USART0_sendChar('\0');  // might be necessary for bcu
}

void USART0_send_ready() {
	USART0_sendString_w_newline_eol("Ready");
}

void usartInit(void) {
	USART0_set_baud_rate();
	USART0_enable_periperials();
	USART0_enable_rx_complete_interrupt();
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

void USART0_read_string(char *receive_buffer, int maxlen) {
	if (maxlen > 32) {
		maxlen = 32; //increase size of receive buffer in uart.h if you want to have more space!
	}
	int i = 0;
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
	USART0_read_string(g_usartReceiveBuffer, 32);
	if(strcmp(g_usartReceiveBuffer, "Test") == 0) {
		USART0_sendString_w_newline_eol("Echo");
	}
	g_usart0ReceiveComplete = true;
} 
