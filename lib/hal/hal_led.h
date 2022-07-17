#ifndef hal_led_h__
#define hal_led_h__

#include <avr/io.h>
#include "flags.h"

#define LED_PORT PORTB
#define LED_PIN PIN4_bm

void ledInit();
void setLedPwm(uint16_t dimming_value);
baseSbuError_t ledDim(uint16_t dimmingValue);
baseSbuError_t ledOn(void);
baseSbuError_t ledOff(void);

#endif