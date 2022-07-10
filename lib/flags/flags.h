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

/**
 * @brief success or error return values
 * @returns 0: success
 *          <0: generic error (like some fail)
 *          >0: specific error (like invalid_transfer)
 */
typedef enum {
    success = 0,
    fail = -1,
    invalid_transfer = 1
} baseSbuError_t;

states_t g_currentState;
wakeupReasons_t g_wakeupReason;
volatile bool g_usart0ReceiveComplete;
#endif