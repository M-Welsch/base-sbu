#include "statemachine.h"
#include "hal_powerpath.h"
#include "logging.h"
#include "delay.h"

void statemachineInit() {
    g_currentState = stateBcuRunning;
}

/**
 * @brief sets state to BcuRunning
 * @details activates bcu supply. Can transfer from state5vActive only.
 */
statemachineError statemachineGotoBcuRunning(void) {
    loggingPutDebug("requested stateBcuRunning");
    statemachineError retval = invalid_transfer;
    if (g_currentState == state5vActive) {
        activateBcuSupply();
        g_currentState = stateBcuRunning;
        retval = success;
    }
    return retval;
}

/**
 * @brief sets state to 5vActive
 * @details activates 5v supply. Can only be transfered to if in standby.
 */
statemachineError statemachineGoto5vActive(void) {
    loggingPutDebug("requested state5vActive");
    statemachineError retval = invalid_transfer;
    if (g_currentState == stateStandby) {
        activate5vRail();
        g_currentState = state5vActive;
        retval = success;
    }
    return retval;
}

/**
 * @brief sets state to StandBy
 * @details deactivates 5v and bcu supply. Can transfer from all other states.
 */
statemachineError statemachineGotoStandby(void) {
    loggingPutDebug("requested stateStandby");
    deactivateBcuSupply();
    delayMs(100);
    deactivate5vRail();
    g_currentState = stateStandby;
    return success;
}