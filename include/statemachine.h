#ifndef statemachine_h__
#define statemachine_h__

typedef enum {
    stateInit,
    stateBcuRunning,
    stateShutdownRequested,
    stateStandby,
    stateMenu
} states_t;

void statemachineInit();

void statemachineGotoBcuRunning(void);
void statemachineGotoShutdownRequested(void);
void statemachineGotoStandby(void);
void statemachineGotoMenu(void);

#endif