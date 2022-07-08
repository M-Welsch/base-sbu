#ifndef statemachine_h__
#define statemachine_h__

typedef enum {
    success,
    invalid_transfer
} statemachineError;

typedef enum {
    stateInit,
    stateBcuRunning,
    stateShutdownRequested,
    stateStandby,
    stateMenu
} states_t;

volatile states_t g_currentState;

void statemachineInit();

statemachineError statemachineGotoBcuRunning(void);
statemachineError statemachineGotoShutdownRequested(void);
statemachineError statemachineGotoStandby(void);
statemachineError statemachineGotoMenu(void);

char *stringifyCurrentState();

#endif