#ifndef hal_adc_h__
#define hal_adc_h__

#include <stdbool.h>

#include "hal.h"

uint16_t adcResult3v3();
uint16_t adcResultInCur();
uint16_t adcTemperature();
bool adc3v3present();

#endif