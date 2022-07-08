#include "statemachine.h"
//#include "mock_statemachine.h"
#include "mock_hal_powerpath.h"
#include "mock_hal_display.h"
#include "mock_logging.h"


void test_statemachineGotoBcuRunning_should_gotoBcuRunningFromInit() {
    loggingPutMsg_Ignore();
    activateBcuSupply_Expect();
    g_currentState = stateInit;

    statemachineGotoBcuRunning();

    TEST_ASSERT_EQUAL_INT(stateBcuRunning, g_currentState);
}

void test_statemachineGotoBcuRunning_should_gotoBcuRunningFromShutdownRequested() {
    loggingPutMsg_Ignore();
    g_currentState = stateShutdownRequested;

    statemachineGotoBcuRunning();

    TEST_ASSERT_EQUAL_INT(stateBcuRunning, g_currentState);
}

void test_statemachineGotoBcuRunning_should_gotoBcuRunningFromStandby() {
    loggingPutMsg_Ignore();
    activateBcuSupply_Expect();
    activate5vRail_Expect();
    displayInit_Expect();
    g_currentState = stateStandby;

    statemachineGotoBcuRunning();

    TEST_ASSERT_EQUAL_INT(stateBcuRunning, g_currentState);
}

void test_statemachineGotoBcuRunning_should_gotoBcuRunningFromMenu() {
    loggingPutMsg_Ignore();
    activateBcuSupply_Expect();
    g_currentState = stateMenu;

    statemachineGotoBcuRunning();

    TEST_ASSERT_EQUAL_INT(stateBcuRunning, g_currentState);
}

void statemachineGotoShutdownRequested_should_gotoShutdownRequested() {
    g_currentState = stateInit;
    g_currentState = stateShutdownRequested;
    g_currentState = stateStandby;
    g_currentState = stateMenu;
}