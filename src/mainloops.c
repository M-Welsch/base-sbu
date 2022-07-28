#include <stdio.h>

#include "mainloops.h"
#include "statemachine.h"
#include "flags.h"
#include "usart.h"
#include "menu.h"
#include "hal.h"

void _processCommunication() {
    if (g_usart0ReceiveComplete) {
        g_usart0ReceiveComplete = false;
        usartDecodeIncomingMessage();
    }
}

void mainloopBcuRunning() {
    _processCommunication();
}

uint8_t shutdownCounter = 0;
char _buffer[32];
void mainloopShutdownRequested() {
    if (shutdownCounter < 10) {
        sprintf(_buffer, "SCnt: %d", shutdownCounter);
        USART0_sendString_w_newline_eol(_buffer);
        shutdownCounter++;
        _processCommunication();
    }
    else {
        shutdownCounter = 0;
        statemachineGotoStandby();
        // sleep here, wake up here. state=standby here

        USART0_sendString_w_newline_eol("woke");
        if (rtcTimerDue()) {
            rtcDeactivateCompareInterrupt();
            g_wakeupReason = SCHEDULED_BACKUP;
            statemachineGotoBcuRunning();
        }
        else {
            statemachineGotoMenu();
        }
    }
}

void mainloopMenu() {
    USART0_sendString_w_newline_eol("menu");
    menuShow(1000);
}
