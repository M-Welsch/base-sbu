#ifndef hal_led_h__
#define hal_led_h__

#include "flags.h"

baseSbuError_t ledDim(uint16_t dimmingValue);
baseSbuError_t ledOn(void);
baseSbuError_t ledOff(void);

#endif