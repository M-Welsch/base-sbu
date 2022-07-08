#ifndef statemachine_h__
#define statemachine_h__

void (*g_mainloop)();
void mainloopBcuRunning();
void mainloopShutdownRequested();
void mainloopMenu();

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

states_t g_currentState;

void statemachineInit();

statemachineError statemachineGotoBcuRunning(void);
statemachineError statemachineGoto5vActive(void);
statemachineError statemachineGotoStandby(void);
statemachineError statemachineGotoMenu(void);

#endif