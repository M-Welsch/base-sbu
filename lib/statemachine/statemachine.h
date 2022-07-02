#ifndef statemachine_h__
#define statemachine_h__

typedef enum {
    stateBcuRunning,
    state5vActive,
    stateStandby
} states;

states g_currentState;

#endif