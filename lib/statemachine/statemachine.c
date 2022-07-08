#include "statemachine.h"
#include "hal_powerpath.h"
#include "hal_display.h"
#include "logging.h"
#include "delay.h"

void activate5vRailAndDisplay() {
    activate5vRail();
    displayInit();
}

/**
 * @brief state where it all starts
 * @details cannot be reached during run
 */
void statemachineInit() {
    g_currentState = stateInit;
}

/**
 * @brief sets state to BcuRunning
 * @details activates bcu supply. Can transfer here from any other state
 */
statemachineError statemachineGotoBcuRunning(void) {
    loggingPutDebug("requested stateBcuRunning");
    if ((g_currentState == stateInit) || (g_currentState == stateMenu) || (g_currentState == stateStandby)) {
        activateBcuSupply();
    }
    if (g_currentState == stateStandby) {
        activate5vRailAndDisplay();
    }
    g_currentState = stateBcuRunning;
    g_mainloop = mainloopBcuRunning;
    return success;
}

/**
 * @brief set state to ShutdownRequested
 * @details waits for break away of bcu's 3.3V rail (full shutdown of bananapi sbc) or 
 *          for command to leave the state and go to BcuRunning again
 */
statemachineError statemachineGotoShutdownRequested(void) {
    loggingPutInfo("requested stateShutdownRequested");
    statemachineError retval = invalid_transfer;
    if (g_currentState == stateBcuRunning) {
        g_currentState = stateShutdownRequested;
        g_mainloop = mainloopShutdownRequested;
    }
    return success;
}

/**
 * @brief sets state to StandBy
 * @details deactivates 5v and bcu supply.
 */
statemachineError statemachineGotoStandby(void) {
    statemachineError retval = invalid_transfer;
    states_t lastState = g_currentState;
    loggingPutDebug("requested stateStandby");
    deactivateBcuSupply();
    delayMs(100);
    deactivate5vRail();
    
    g_currentState = stateStandby;
    //activate interrupts, mask them if necessary
    //go to sleep
    switch (lastState) {
        case stateBcuRunning:
            retval = statemachineGotoBcuRunning();
            break;
        case stateMenu:
            retval = statemachineGotoMenu();
            break;
        default:
            loggingPutError("invalid transfer from state");
    }
    return retval;
}

/**
 * @brief activates menu
 * @details activates 5v rail, initialises display and starts up the menu
 */
statemachineError statemachineGotoMenu(void) {
    statemachineError retval = invalid_transfer;
    loggingPutDebug("requested stateMenu");
    if (g_currentState == stateStandby) {
        activate5vRailAndDisplay();
        g_currentState = stateMenu;
        g_mainloop = mainloopMenu;
        retval = success;
    }
    return retval;
}

void mainloopBcuRunning() {
}

void mainloopShutdownRequested() {
}

void mainloopMenu() {
}
