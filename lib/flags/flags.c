#include <string.h>

#include "flags.h"

void flagsInit() {
    g_usart0ReceiveComplete = false;
    g_wakeupReason = NO_REASON;
    strcpy(g_nextBackupInfo.humanReadableTimestamp, "No Timestamp");
    g_nextBackupInfo.humanReadableTimestampReceived = false;
    g_nextBackupInfo.secondsToWakeupReceived = false;
    g_nextBackupInfo.secondsToWakeup = 0;
    g_rtcTriggered = false;
}