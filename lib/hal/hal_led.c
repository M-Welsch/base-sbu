#include "statemachine.h"
#include "hal.h"

hwAccessError_t ledOn(void) {
    if (g_currentState == stateStandby) return hardware_call_refused;
    ledPinHigh();
    return hardware_call_accepted;
}

hwAccessError_t ledOff(void) {
    ledPinLow();
    return hardware_call_accepted;
}
