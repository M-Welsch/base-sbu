#ifndef flags_h__
#define flags_h__
#include <stdbool.h>

typedef enum {
    stateInit,
    stateBcuRunning,
    stateShutdownRequested,
    stateStandby,
    stateMenu
} states_t;

volatile states_t g_currentState;
volatile bool g_usart0Receive;
#endif