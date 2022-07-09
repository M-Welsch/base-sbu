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

typedef enum {
    BACKUP_NOW,
    SCHEDULED_BACKUP,
    CONFIGURATION,
    HEARTBEAT_TIMEOUT,
    NO_REASON 
} wakeupReasons_t;

states_t g_currentState;
wakeupReasons_t g_wakeupReason;
volatile bool g_usart0Receive;
#endif