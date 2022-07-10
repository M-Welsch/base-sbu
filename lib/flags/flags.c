#include "flags.h"

void flagsInit() {
    g_usart0ReceiveComplete = false;
    g_wakeupReason = NO_REASON;
}