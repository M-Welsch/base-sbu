#include "hal.h"
#include "hal_adc.h"

uint16_t adcResult3v3() {
    return adc_measure_3v3();
}

uint16_t adcResultInCur() {
    return adc_measure_input_current();
}

uint16_t adcTemperature() {
    return adc_measure_temperature();
}

bool adc3v3present() {
    return vcc3v3_present();
}
