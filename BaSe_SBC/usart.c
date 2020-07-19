/*
 * usart.c
 *
 * Created: 06.06.2020 21:58:36
 *  Author: Max
 */ 

#include <avr/io.h>
#include "gpio_interface.h"
#include "usart.h"
#include <string.h>

#define F_CPU 3333333
//#define USART0_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)
#define BAUD_RATE 9600
#define UROUND(x) ((2UL*(x)+1)/2)

void init_uart(void) {
	set_pb2_txd_and_pb3_rxd();
	USART0.BAUD = UROUND(64UL*F_CPU/16/BAUD_RATE);
	USART0.CTRLB = USART_RXEN_bm | USART_TXEN_bm;
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