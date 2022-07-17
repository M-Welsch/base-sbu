#include "flags.h"
#include "hal_led.h"

uint16_t _ledDimmingValue = 0;

void ledPinHigh(void) {
    setLedPwm(_ledDimmingValue);
}

void setLedPwmOff() {
     TCA0.SINGLE.CMP1 = 0;
}

void ledPinLow(void) {
    setLedPwmOff();
}

void setLedPwm(uint16_t dimming_value) {
    _ledDimmingValue = dimming_value;
    TCA0.SINGLE.CMP1 = _ledDimmingValue;
}

void ledInit() {
    LED_PORT.DIRSET = LED_PIN;
}

baseSbuError_t ledDim(uint16_t dimmingValue) {
    if (g_currentState == stateStandby) return hardware_call_refused;
    setLedPwm(dimmingValue);
    return success;
}

baseSbuError_t ledOn(void) {
    if (g_currentState == stateStandby) return hardware_call_refused;
    ledPinHigh();
    return success;
}

baseSbuError_t ledOff(void) {
    ledPinLow();
    return success;
}
