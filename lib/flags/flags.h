#ifndef flags_h__
#define flags_h__
#include <stdbool.h>
#include <stdint.h>

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

typedef struct {
    wakeupReasons_t reason;
    char* keyword;
} wakeupReasonsMap_t;

/**
 * @brief success or error return values
 * @returns 0: success
 *          <0: generic error (like some fail)
 *          >0: specific error (like invalid_transfer)
 */
typedef enum {
    success = 0,
    fail = -1,
    invalid_transfer = 1,
    hardware_call_refused = 2
} baseSbuError_t;

typedef struct {
    uint16_t secondsToWakeup;
    char humanReadableTimestamp;
    bool secondsToWakeupReceived;
    bool humanReadableTimestampReceived;
} nextBackupInfo_t;

states_t g_currentState;
wakeupReasons_t g_wakeupReason;
volatile bool g_usart0ReceiveComplete;
nextBackupInfo_t g_nextBackupInfo;
#endif