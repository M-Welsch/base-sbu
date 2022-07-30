#include <stdint.h>

#ifdef __AVR_ATtiny816__
#include <avr/sleep.h>
#include <util/delay.h>
#else
uint8_t SLPCTRL_SMODE_STDBY_gc, SLPCTRL_SEN_bm;
void _delay_ms(double __ms) {
    ;
}
void sleep_cpu() {
    ;
}
typedef struct {
    uint8_t CTRLA;
}SLPCTRL_t;
SLPCTRL_t SLPCTRL;
#endif

#include "statemachine.h"
#include "hal.h"

states_t g_currentState = stateInit;

void _gotoSleepStandby()
{
	SLPCTRL.CTRLA |= SLPCTRL_SMODE_STDBY_gc;
	SLPCTRL.CTRLA |= SLPCTRL_SEN_bm;
	sleep_cpu();
}

void activate5vRailAndDisplay() {
    activate5vRail();
    // displayInit();
    // displayPwmHigh();
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
void statemachineGotoBcuRunning(void) {
    if ((g_currentState == stateStandby) || (g_currentState == stateInit)) {
        activate5vRailAndDisplay();
    }
    if (g_currentState != stateShutdownRequested) {
        activateBcuSupply();
    }
    g_currentState = stateBcuRunning;
    //displayWriteString("Backup Server\nis booting ...");
}

/**
 * @brief set state to ShutdownRequested
 * @details waits for break away of bcu's 3.3V rail (full shutdown of bananapi sbc) or 
 *          for command to leave the state and go to BcuRunning again
 */
 void statemachineGotoShutdownRequested(void) {
    USART0SendString("req stateShutdownRequested");
    if (g_currentState == stateBcuRunning) {
        g_currentState = stateShutdownRequested;
    }
}

/**
 * @brief sets state to StandBy
 * @details deactivates 5v and bcu supply.
 */
void statemachineGotoStandby(void) {
    USART0SendString("req stateStandby");
    if ((g_currentState == stateShutdownRequested) || (g_currentState == stateMenu)) {
        // displayPinsAllLow();
        // deactivateBcuSupply();
        // _delay_ms(100);
        // deactivate5vRail();
        _delay_ms(100);
        
        g_currentState = stateStandby;
        //activate interrupts, mask them if necessary
        _gotoSleepStandby();
        _delay_ms(100);  // important but probably more than sufficient
    }
}

/**
 * @brief activates menu
 * @details activates 5v rail, initialises display and starts up the menu
 */
void statemachineGotoMenu(void) {
    USART0SendString("req stateMenu");
    if (g_currentState == stateStandby) {
        activate5vRailAndDisplay();
        g_currentState = stateMenu;
    }
}