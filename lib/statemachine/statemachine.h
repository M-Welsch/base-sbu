#ifndef statemachine_h__
#define statemachine_h__

typedef enum {
    success,
    invalid_transfer
} statemachineError;

typedef enum {
    stateBcuRunning,
    state5vActive,
    stateStandby
} states;

states g_currentState;

void statemachineInit();

statemachineError statemachineGotoBcuRunning(void);
statemachineError statemachineGoto5vActive(void);
statemachineError statemachineGotoStandby(void);

#endif