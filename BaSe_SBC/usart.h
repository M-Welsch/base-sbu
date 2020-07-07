/*
 * usart.h
 *
 * Created: 06.06.2020 21:58:51
 *  Author: Max
 */ 


#ifndef USART_H_
#define USART_H_

char buffer[32];

void init_uart(void);
void USART0_sendChar(char c);
void USART0_sendString(char *s);


#endif /* USART_H_ */