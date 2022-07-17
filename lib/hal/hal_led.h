#ifndef hal_led_h__
#define hal_led_h__

#include "flags.h"

#ifdef __AVR_ATtiny816__
#include <avr/io.h>
#else
typedef struct {uint8_t CMP1;} single_double_t ;
typedef struct {single_double_t SINGLE;} tca0_t;
tca0_t TCA0;
typedef struct {uint8_t DIRSET;} port_t;
port_t PORTB;
uint8_t PIN4_bm;
#endif

#define LED_PORT PORTB
#define LED_PIN PIN4_bm

void ledInit();
void setLedPwm(uint16_t dimming_value);
baseSbuError_t ledDim(uint16_t dimmingValue);
baseSbuError_t ledOn(void);
baseSbuError_t ledOff(void);

#endif