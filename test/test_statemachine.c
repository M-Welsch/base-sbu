#include "statemachine.h"
#include "mock_hal.h"

extern states_t g_currentState;

/* test transitions to menu */

 void test_statemachineGotoMenu_shouldNot_gotoInitFromBcu() {
    g_currentState = stateBcuRunning;

    TEST_ASSERT_EQUAL_INT(stateBcuRunning, g_currentState);
 }

 void test_statemachineGotoMenu_shouldNot_gotoInitFromShutdownRequested() {
    g_currentState = stateShutdownRequested;
    USART0SendString_Ignore();

    statemachineGotoMenu();
    TEST_ASSERT_EQUAL_INT(stateShutdownRequested, g_currentState);
 }

 void test_statemachineGotoMenu_shouldNot_gotoInitFromMenu() {
    g_currentState = stateMenu;
    USART0SendString_Ignore();

    statemachineGotoMenu();
    TEST_ASSERT_EQUAL_INT(stateMenu, g_currentState);
 }

 void test_statemachineGotoMenu_shouldNot_gotoInitFromInit() {
    g_currentState = stateInit;
    USART0SendString_Ignore();

    statemachineGotoMenu();
    TEST_ASSERT_EQUAL_INT(stateInit, g_currentState);
 }

 void test_statemachineGotoMenu_should_gotoInitFromStandby() {
    USART0SendString_Ignore();
    activate5vRail_Expect();
    //displayInit_Expect();
    g_currentState = stateStandby;

    statemachineGotoMenu();
    TEST_ASSERT_EQUAL_INT(stateMenu, g_currentState);
 }

/* test transfers to BcuRunning */

void test_statemachineGotoBcuRunning_should_gotoBcuRunningFromInit() {
    activate5vRail_Expect();
    //displayInit_Expect();
    activateBcuSupply_Expect();
    g_currentState = stateInit;

    statemachineGotoBcuRunning();

    TEST_ASSERT_EQUAL_INT(stateBcuRunning, g_currentState);
}

void test_statemachineGotoBcuRunning_should_gotoBcuRunningFromShutdownRequested() {
    g_currentState = stateShutdownRequested;

    statemachineGotoBcuRunning();

    TEST_ASSERT_EQUAL_INT(stateBcuRunning, g_currentState);
}

void test_statemachineGotoBcuRunning_should_gotoBcuRunningFromStandby() {
    activate5vRail_Expect();
    //displayInit_Expect();
    activateBcuSupply_Expect();
    g_currentState = stateStandby;

    statemachineGotoBcuRunning();

    TEST_ASSERT_EQUAL_INT(stateBcuRunning, g_currentState);
}

void test_statemachineGotoBcuRunning_should_gotoBcuRunningFromMenu() {
    activateBcuSupply_Expect();
    g_currentState = stateMenu;

    statemachineGotoBcuRunning();

    TEST_ASSERT_EQUAL_INT(stateBcuRunning, g_currentState);
}

/* test transfers to shutdown requested */

void test_statemachineGotoShutdownRequested_should_gotoShutdownRequestedFromBcuRunning() {
    g_currentState = stateBcuRunning;
    USART0SendString_Ignore();

    statemachineGotoShutdownRequested();
    TEST_ASSERT_EQUAL_INT(stateShutdownRequested, g_currentState);
}

void test_statemachineGotoShutdownRequested_shouldNot_gotoShutdownRequestedFromInit() {
    g_currentState = stateInit;
    USART0SendString_Ignore();

    statemachineGotoShutdownRequested();
    TEST_ASSERT_EQUAL_INT(stateInit, g_currentState);
}

void test_statemachineGotoShutdownRequested_shouldNot_gotoShutdownRequestedFromStandby() {
    g_currentState = stateStandby;
    USART0SendString_Ignore();

    statemachineGotoShutdownRequested();
    TEST_ASSERT_EQUAL_INT(stateStandby, g_currentState);
}

void test_statemachineGotoShutdownRequested_shouldNot_gotoShutdownRequestedFromMenu() {
    g_currentState = stateMenu;
    USART0SendString_Ignore();

    statemachineGotoShutdownRequested();
    TEST_ASSERT_EQUAL_INT(stateMenu, g_currentState);
}


/* test transitions to standby */

void _test_statemachineGotoStandby_should_gotoStandbyFromShutdownRequested() {
    deactivateBcuSupply_Expect();
    deactivate5vRail_Expect();

    g_currentState = stateShutdownRequested;

    statemachineGotoStandby();
    TEST_ASSERT_EQUAL_INT(stateStandby, g_currentState);
}

void _test_statemachineGotoStandby_should_gotoStandbyFromShutdownMenu() {
    deactivateBcuSupply_Expect();
    deactivate5vRail_Expect();

    g_currentState = stateMenu;

    statemachineGotoStandby();
    TEST_ASSERT_EQUAL_INT(stateStandby, g_currentState);
}

void test_statemachineGotoStandby_shouldNot_gotoStandbyFromBcuRunning() {
    g_currentState = stateBcuRunning;
    USART0SendString_Ignore();

    statemachineGotoStandby();
    TEST_ASSERT_EQUAL_INT(stateBcuRunning, g_currentState);
}

void test_statemachineGotoStandby_shouldNot_gotoStandbyFromInit() {
    g_currentState = stateInit;
    USART0SendString_Ignore();

    statemachineGotoStandby();
    TEST_ASSERT_EQUAL_INT(stateInit, g_currentState);
}

void test_statemachineGotoStandby_shouldNot_gotoStandbyFromStandby() {
    g_currentState = stateStandby;
    USART0SendString_Ignore();

    statemachineGotoStandby();
    TEST_ASSERT_EQUAL_INT(stateStandby, g_currentState);
}