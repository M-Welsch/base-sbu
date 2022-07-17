#define norun

#ifndef norun

#include "statemachine.h"
#include "flags.h"
#include "mock_hal_powerpath.h"
#include "mock_hal_display.h"

/* test transitions to menu */

 void test_statemachineGotoMenu_shouldNot_gotoInitFromBcu() {
    loggingPutMsg_Ignore();
    g_currentState = stateBcuRunning;

    TEST_ASSERT_EQUAL_INT(invalid_transfer, statemachineGotoMenu());
    TEST_ASSERT_EQUAL_INT(stateBcuRunning, g_currentState);
 }

 void test_statemachineGotoMenu_shouldNot_gotoInitFromShutdownRequested() {
    loggingPutMsg_Ignore();
    g_currentState = stateShutdownRequested;

    TEST_ASSERT_EQUAL_INT(invalid_transfer, statemachineGotoMenu());
    TEST_ASSERT_EQUAL_INT(stateShutdownRequested, g_currentState);
 }

 void test_statemachineGotoMenu_shouldNot_gotoInitFromMenu() {
    loggingPutMsg_Ignore();
    g_currentState = stateMenu;

    TEST_ASSERT_EQUAL_INT(invalid_transfer, statemachineGotoMenu());
    TEST_ASSERT_EQUAL_INT(stateMenu, g_currentState);
 }

 void test_statemachineGotoMenu_shouldNot_gotoInitFromInit() {
    loggingPutMsg_Ignore();
    g_currentState = stateInit;

    TEST_ASSERT_EQUAL_INT(invalid_transfer, statemachineGotoMenu());
    TEST_ASSERT_EQUAL_INT(stateInit, g_currentState);
 }

 void test_statemachineGotoMenu_should_gotoInitFromStandby() {
    loggingPutMsg_Ignore();
    activate5vRail_Expect();
    displayInit_Expect();
    g_currentState = stateStandby;

    TEST_ASSERT_EQUAL_INT(success, statemachineGotoMenu());
    TEST_ASSERT_EQUAL_INT(stateMenu, g_currentState);
 }

/* test transfers to BcuRunning */

void test_statemachineGotoBcuRunning_should_gotoBcuRunningFromInit() {
    loggingPutMsg_Ignore();
    activate5vRail_Expect();
    displayInit_Expect();
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
    activate5vRail_Expect();
    displayInit_Expect();
    activateBcuSupply_Expect();
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

/* test transfers to shutdown requested */

void test_statemachineGotoShutdownRequested_should_gotoShutdownRequestedFromBcuRunning() {
    loggingPutMsg_Ignore();
    
    g_currentState = stateBcuRunning;

    TEST_ASSERT_EQUAL_INT(success, statemachineGotoShutdownRequested());
    TEST_ASSERT_EQUAL_INT(stateShutdownRequested, g_currentState);
}

void test_statemachineGotoShutdownRequested_shouldNot_gotoShutdownRequestedFromInit() {
    loggingPutMsg_Ignore();

    g_currentState = stateInit;

    TEST_ASSERT_EQUAL_INT(invalid_transfer, statemachineGotoShutdownRequested());
    TEST_ASSERT_EQUAL_INT(stateInit, g_currentState);
}

void test_statemachineGotoShutdownRequested_shouldNot_gotoShutdownRequestedFromStandby() {
    loggingPutMsg_Ignore();

    g_currentState = stateStandby;

    TEST_ASSERT_EQUAL_INT(invalid_transfer, statemachineGotoShutdownRequested());
    TEST_ASSERT_EQUAL_INT(stateStandby, g_currentState);
}

void test_statemachineGotoShutdownRequested_shouldNot_gotoShutdownRequestedFromMenu() {
    loggingPutMsg_Ignore();
    
    g_currentState = stateMenu;

    TEST_ASSERT_EQUAL_INT(invalid_transfer, statemachineGotoShutdownRequested());
    TEST_ASSERT_EQUAL_INT(stateMenu, g_currentState);
}


/* test transitions to standby */

void _test_statemachineGotoStandby_should_gotoStandbyFromShutdownRequested() {
    loggingPutMsg_Ignore();
    deactivateBcuSupply_Expect();
    deactivate5vRail_Expect();

    g_currentState = stateShutdownRequested;

    TEST_ASSERT_EQUAL_INT(success, statemachineGotoStandby());
    TEST_ASSERT_EQUAL_INT(stateStandby, g_currentState);
}

void _test_statemachineGotoStandby_should_gotoStandbyFromShutdownMenu() {
    loggingPutMsg_Ignore();
    deactivateBcuSupply_Expect();
    deactivate5vRail_Expect();

    g_currentState = stateMenu;

    TEST_ASSERT_EQUAL_INT(success, statemachineGotoStandby());
    TEST_ASSERT_EQUAL_INT(stateStandby, g_currentState);
}

void test_statemachineGotoStandby_shouldNot_gotoStandbyFromBcuRunning() {
    loggingPutMsg_Ignore();

    g_currentState = stateBcuRunning;

    TEST_ASSERT_EQUAL_INT(invalid_transfer, statemachineGotoStandby());
    TEST_ASSERT_EQUAL_INT(stateBcuRunning, g_currentState);
}

void test_statemachineGotoStandby_shouldNot_gotoStandbyFromInit() {
    loggingPutMsg_Ignore();

    g_currentState = stateInit;

    TEST_ASSERT_EQUAL_INT(invalid_transfer, statemachineGotoStandby());
    TEST_ASSERT_EQUAL_INT(stateInit, g_currentState);
}

void test_statemachineGotoStandby_shouldNot_gotoStandbyFromStandby() {
    loggingPutMsg_Ignore();

    g_currentState = stateStandby;

    TEST_ASSERT_EQUAL_INT(invalid_transfer, statemachineGotoStandby());
    TEST_ASSERT_EQUAL_INT(stateStandby, g_currentState);
}
#endif                                                                          