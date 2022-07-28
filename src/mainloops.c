#include <stdio.h>

#include "mainloops.h"
#include "statemachine.h"
#include "hal_display.h"
#include "hal_adc.h"
#include "flags.h"
#include "usart.h"
#include "hal_usart.h"
#include "hal_buttons.h"
#include "menu.h"

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
    if (shutdownCounter < 30) {
        sprintf(_buffer, "SCnt: %d", shutdownCounter);
        USART0_sendString_w_newline_eol(_buffer);
        shutdownCounter++;
        _processCommunication();
    }
    else {
        statemachineGotoStandby();
        // sleep here, wake up here. state=standby here

        USART0_sendString_w_newline_eol("woke");
        if (g_rtcTriggered) {
            g_rtcTriggered = false;
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
