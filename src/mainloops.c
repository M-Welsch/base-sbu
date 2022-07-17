#include <stdio.h>

#include "mainloops.h"
#include "statemachine.h"
#include "hal_display.h"
#include "flags.h"
#include "usart.h"

usartDecodedMsg_t decodedMsg;


void mainloopBcuRunning() {
    if (g_usart0ReceiveComplete) {
        g_usart0ReceiveComplete = false;
        if(usartDecodeIncomingMessage(&decodedMsg) == success) {
            decodedMsg.callback(decodedMsg.payload);
        }
    }
}

void mainloopShutdownRequested() {
    statemachineGotoBcuRunning();
}

void mainloopMenu() {
}
