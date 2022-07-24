#ifndef USART_H_
#define USART_H_

#include <stdint.h>
#include <stdbool.h>

#define F_CPU 3333333
#define BAUD_RATE 9600
#define UROUND(x) ((2UL*(x)+1)/2)

#define USART_PORT PORTB
#define USART_PIN_TX PIN2_bm
#define USART_PIN_RX PIN3_bm

void usartInit(void);

void USART0_send_ready();
void USART0_sendString(const char *s);
void USART0_sendString_w_newline_eol(const char *s);

uint8_t USART0_read();
void USART0_read_string(char *receive_buffer, uint8_t maxlen);
void USART0_process_incoming_message();

void print_buffer(char *fromWhere);

#endif /* USART_H_ */