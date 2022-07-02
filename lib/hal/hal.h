#ifndef hal_h__
#define hal_h__

#include <stdint.h>

#ifdef __AVR_ATtiny816__
#include <avr/io.h>
#else
// struct port {
//     uint8_t PIN1CTRL;
//     uint8_t PIN5CTRL;
// } PORTA, PORTB;

#endif


#define LED_PORT PORTB
#define LED_PIN PIN4_bm

#define USART_PORT PORTB
#define USART_PIN_TX PIN2_bm
#define USART_PIN_RX PIN3_bm

void halSetup();

void ledPinHigh(void);
void ledPinLow(void);

#endif