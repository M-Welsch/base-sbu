#include <stdint.h>

#include "globals.h"
#include "hal.h"

void decodeMessageAndRunAction() {
    char messageCode = *g_usartReceiveBuffer;
    char *payload = g_usartReceiveBuffer+1;

    uint16_t secondsToWakeup = 0;
    switch (messageCode) {
        case 0x01:  // Seconds to next Wakeup. LSB first, important!!
            if (*(payload+1) == '\0')
                secondsToWakeup = (uint16_t) *payload;
            else
                secondsToWakeup = *payload | (*payload+1 << 8);
            rtcSetWakeupInSeconds(secondsToWakeup);
            break;

        case 0x02:  // shutdown request
            break;

        case 0x03:  // shutdown request abort
            break;

        default:
            break;
    }
}