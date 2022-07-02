#ifndef hal_h__
#define hal_h__

#include "statemachine.h"
#include <avr/io.h>

#define LED_PORT PORTB
#define LED_PIN PIN4_bm

#define USART_PORT PORTB
#define USART_PIN_TX PIN2_bm
#define USART_PIN_RX PIN3_bm

void halSetup();

void ledOn(void);
void ledOff(void);

#endif