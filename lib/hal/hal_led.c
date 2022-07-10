#include "flags.h"
#include "hal.h"

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
