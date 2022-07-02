#include "statemachine.h"
#include "hal.h"

void ledOn(void) {
    if (g_currentState == stateStandby) return;
    ledPinHigh();
}

void ledOff(void) {
    ledPinLow();
}
