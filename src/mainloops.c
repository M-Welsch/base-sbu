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

usartDecodedMsg_t decodedMsg;

void _processCommunication() {
    if (g_usart0ReceiveComplete) {
        g_usart0ReceiveComplete = false;
        if(usartDecodeIncomingMessage(&decodedMsg) == success) {
            decodedMsg.callback(decodedMsg.payload);
        }
    }
}

void mainloopBcuRunning() {
    _processCommunication();
}

void mainloopShutdownRequested() {
    if (adc3v3present()) 
        _processCommunication();
    else {
        statemachineGotoStandby();
        // sleep here, wake up here. state=standby here

        USART0_sendString_w_newline_eol("woke");
        if (g_rtcTriggered) {
            g_rtcTriggered = false;
            g_wakeupReason = SCHEDULED_BACKUP;
            statemachineGotoBcuRunning();
        }
        else
            statemachineGotoMenu();
    }
}

void mainloopMenu() {
    USART0_sendString_w_newline_eol("menu");
    menuShow(1000);
}
