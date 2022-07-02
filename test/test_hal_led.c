#include <unity.h>
#include "hal_led.h"
#include "statemachine.h"
#include "mock_hal.h"

void test_led_should_lightUp_if_stateIsBcuRunningOr5VActive() {
    g_currentState = stateBcuRunning;
    ledPinHigh_Expect();
    ledOn();

    g_currentState = state5vActive;
    ledPinHigh_Expect();
    ledOn();
}

void test_led_shouldNot_lightUp_if_stateIsStandBy() {
    g_currentState = stateStandby;
    ledPinHigh_Ignore();
    ledOn();
}

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}