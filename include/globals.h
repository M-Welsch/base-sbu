#ifndef globals_h__
#define globals_h__
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    BACKUP_NOW,
    SCHEDULED_BACKUP,
    CONFIGURATION,
    HEARTBEAT_TIMEOUT,
    NO_REASON 
} wakeupReasons_t;

typedef struct {
    uint16_t secondsToWakeup;
    char humanReadableTimestamp[48];
    bool secondsToWakeupReceived;
    bool humanReadableTimestampReceived;
} nextBackupInfo_t;

wakeupReasons_t g_wakeupReason;
nextBackupInfo_t g_nextBackupInfo;
// states_t g_currentState;

#define LEN_USART_RECEIVE_BUFFER 38

//char g_usartReceiveBuffer[LEN_USART_RECEIVE_BUFFER];
//volatile bool g_usart0ReceiveComplete;

void flagsInit();
#endif