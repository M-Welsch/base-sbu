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
baseSbuError_t statemachineGotoBcuRunning(void) {
    loggingPutDebug("requested stateBcuRunning");
    if ((g_currentState == stateStandby) || (g_currentState == stateInit)) {
        activate5vRailAndDisplay();
    }
    if (g_currentState != stateShutdownRequested) {
        activateBcuSupply();
    }
    g_currentState = stateBcuRunning;
    return success;
}

/**
 * @brief set state to ShutdownRequested
 * @details waits for break away of bcu's 3.3V rail (full shutdown of bananapi sbc) or 
 *          for command to leave the state and go to BcuRunning again
 */
baseSbuError_t statemachineGotoShutdownRequested(void) {
    loggingPutInfo("requested stateShutdownRequested");
    baseSbuError_t retval = invalid_transfer;
    if (g_currentState == stateBcuRunning) {
        g_currentState = stateShutdownRequested;
        retval = success;
    }
    return retval;
}

/**
 * @brief sets state to StandBy
 * @details deactivates 5v and bcu supply.
 */
baseSbuError_t statemachineGotoStandby(void) {
    loggingPutDebug("requested stateStandby");
    baseSbuError_t retval = invalid_transfer;
    if ((g_currentState == stateShutdownRequested) || (g_currentState == stateMenu)) {
        states_t lastState = g_currentState;
        deactivateBcuSupply();
        delayMs(100);
        deactivate5vRail();
        
        g_currentState = stateStandby;
        //activate interrupts, mask them if necessary
        //go to sleep
        switch (lastState) {
            case stateShutdownRequested:
                retval = statemachineGotoBcuRunning();
                break;
            case stateMenu:
                retval = statemachineGotoMenu();
                break;
            default:
                loggingPutError("uncatched invalid transfer from state");
        }
    }
    return retval;
}

/**
 * @brief activates menu
 * @details activates 5v rail, initialises display and starts up the menu
 */
baseSbuError_t statemachineGotoMenu(void) {
    baseSbuError_t retval = invalid_transfer;
    loggingPutDebug("requested stateMenu");
    if (g_currentState == stateStandby) {
        activate5vRailAndDisplay();
        g_currentState = stateMenu;
        retval = success;
    }
    return retval;
}
    
char _myBuffer[33];

char *stringifyCurrentState() {
    switch (g_currentState) {
        case stateInit:
            sprintf(_myBuffer, "Init (%i)", g_currentState);
            break;
        case stateBcuRunning:
            sprintf(_myBuffer, "BcuR (%i)", g_currentState);
            break;
        case stateShutdownRequested:
            sprintf(_myBuffer, "SdRes (%i)", g_currentState);
            break;
        case stateStandby:
            sprintf(_myBuffer, "Stby (%i)", g_currentState);
            break;
        case stateMenu :
            sprintf(_myBuffer, "Menu (%i)", g_currentState);
            break;
    }
    return _myBuffer;
}