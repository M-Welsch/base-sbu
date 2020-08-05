/*
 * usart.c
 *
 * Created: 06.06.2020 21:58:36
 *  Author: Max
 */ 

#include <avr/io.h>
#include "gpio_interface.h"
#include "usart.h"
#include "flags.h"
#include "string.h"
#include <string.h>

#define F_CPU 3333333
//#define USART0_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)
#define BAUD_RATE 9600
#define UROUND(x) ((2UL*(x)+1)/2)

void init_uart(void) {
	set_pb2_txd_and_pb3_rxd();
	USART0_set_baud_rate();
	USART0_enable_periperials();
	USART0_enable_rx_complete_interrupt();
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

void USART0_sendChar(char c) {
	while((USART0.STATUS & USART_DREIF_bm) == 0)
	;
	USART0.TXDATAL = c;
}

void USART0_sendString_w_eol(char *s) {
	USART0_sendString(s);
	USART0_sendChar('\n');
}
void USART0_sendString(char *s) {
	for(size_t i = 0; i < strlen(s); i++) {
		USART0_sendChar(s[i]);
	}
}

uint8_t USART0_read()
{
	
	while (!(USART0_receive_complete()))
	{
		;
	}
	return USART0.RXDATAL;
}

uint8_t USART0_receive_complete() {
	return USART0.STATUS & USART_RXCIF_bm;
}

void USART0_read_string(char *receive_buffer, int maxlen) {
	if (maxlen > 32) {
		maxlen = 32; //increase size of receive buffer in uart.h if you want to have more space!
	}
	int i = 0;
	while(i <= maxlen) {
		i++;
		*receive_buffer = USART0_read();
		if ((*receive_buffer == '\n') || (*receive_buffer == '\0') || (*receive_buffer == '\r')) {
			*receive_buffer = '\0';
			break;
		}
		receive_buffer++;
	}
	if (i > maxlen) {
		*receive_buffer = '\0';
	}
}

void USART0_process_incoming_message() {
	if (strcmp(usart_receive_buffer, "Test") == 0) {
		USART0_sendString_w_eol("Echo");
	}
}

/* to be implemented */
ISR(USART0_RXC_vect) {
	USART0_read_string(usart_receive_buffer, 32);
	flag_usart_string_receive_complete = true;
}