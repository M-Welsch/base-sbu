#include <stdio.h>

#ifdef __AVR_ATtiny816__
#include <avr/sleep.h>
#else
uint8_t SLPCTRL_SMODE_STDBY_gc, SLPCTRL_SEN_bm;
#endif

#include "statemachine.h"
#include "hal_powerpath.h"
#include "hal_display.h"
#include "usart.h"
#include "delay.h"

void _gotoSleepStandby()
{
	SLPCTRL.CTRLA |= SLPCTRL_SMODE_STDBY_gc;
	SLPCTRL.CTRLA |= SLPCTRL_SEN_bm;
	sleep_cpu();
}

void activate5vRailAndDisplay() {
    activate5vRail();
    displayInit();
    displayPwmHigh();
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
    USART0_sendString("req stateBcuRunning\n");
    if ((g_currentState == stateStandby) || (g_currentState == stateInit)) {
        activate5vRailAndDisplay();
    }
    if (g_currentState != stateShutdownRequested) {
        activateBcuSupply();
    }
    g_currentState = stateBcuRunning;
    displayWriteString("Backup Server\nis booting ...");
    return success;
}

/**
 * @brief set state to ShutdownRequested
 * @details waits for break away of bcu's 3.3V rail (full shutdown of bananapi sbc) or 
 *          for command to leave the state and go to BcuRunning again
 */
baseSbuError_t statemachineGotoShutdownRequested(void) {
    USART0_sendString("req stateShutdownRequested\n");
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
    USART0_sendString("req stateStandby\n");
    baseSbuError_t retval = invalid_transfer;
    if ((g_currentState == stateShutdownRequested) || (g_currentState == stateMenu)) {
        states_t lastState = g_currentState;
        displayPinsAllLow();
        deactivateBcuSupply();
        delayMs(100);
        deactivate5vRail();
        delayMs(100);
        
        g_currentState = stateStandby;
        g_wakeupReason = NO_REASON;
        //activate interrupts, mask them if necessary
        _gotoSleepStandby();
        delayMs(100);  // important but probably more than sufficient
    }
    return retval;
}

/**
 * @brief activates menu
 * @details activates 5v rail, initialises display and starts up the menu
 */
baseSbuError_t statemachineGotoMenu(void) {
    baseSbuError_t retval = invalid_transfer;
    USART0_sendString("req stateMenu\n");
    if (g_currentState == stateStandby) {
        activate5vRailAndDisplay();
        g_currentState = stateMenu;
        retval = success;
    }
    return retval;
}
    

void stringifyCurrentState(char *buffer) {
    switch (g_currentState) {
        case stateInit:
            sprintf(buffer, "Init (%i)", g_currentState);
            break;
        case stateBcuRunning:
            sprintf(buffer, "BcuR (%i)", g_currentState);
            break;
        case stateShutdownRequested:
            sprintf(buffer, "SdRes (%i)", g_currentState);
            break;
        case stateStandby:
            sprintf(buffer, "Stby (%i)", g_currentState);
            break;
        case stateMenu :
            sprintf(buffer, "Menu (%i)", g_currentState);
            break;
    }
}