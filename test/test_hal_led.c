#include <unity.h>
#include "hal_led.h"
#include "statemachine.h"
#include "flags.h"
#include "mock_hal.h"
#include "mock_hal_powerpath.h"
#include "mock_logging.h"
#include "mock_hal_display.h"

void test_led_should_lightUp_if_stateIsBcuRunningOr5VActive() {
    g_currentState = stateBcuRunning;
    ledPinHigh_Expect();
    TEST_ASSERT_EQUAL_INT(hardware_call_accepted, ledOn());

    g_currentState = stateShutdownRequested;
    ledPinHigh_Expect();
    TEST_ASSERT_EQUAL_INT(hardware_call_accepted, ledOn());

    g_currentState = stateMenu;
    ledPinHigh_Expect();
    TEST_ASSERT_EQUAL_INT(hardware_call_accepted, ledOn());
}

void test_led_shouldNot_lightUp_if_stateIsStandBy() {
    g_currentState = stateStandby;
    ledPinHigh_Ignore();
    TEST_ASSERT_EQUAL_INT(hardware_call_refused, ledOn());
}

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}