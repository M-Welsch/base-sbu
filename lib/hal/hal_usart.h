#ifndef USART_H_
#define USART_H_

#include <stdint.h>
#include <stdbool.h>

char g_usartReceiveBuffer[38];

void usartInit(void);

void USART0_send_ready();
void USART0_sendString(char *s);
void USART0_sendString_w_newline_eol(char *s);

uint8_t USART0_read();
void USART0_read_string(char *receive_buffer, int maxlen);
void USART0_process_incoming_message();

void print_buffer(char *fromWhere);

#endif /* USART_H_ */