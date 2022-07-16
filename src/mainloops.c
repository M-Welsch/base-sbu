#include <stdio.h>

#include "mainloops.h"
#include "logging.h"
#include "statemachine.h"
#include "hal_display.h"
#include "flags.h"
#include "usart.h"

usartDecodedMsg_t decodedMsg;

char _localBuffer[45];

void mainloopBcuRunning() {
    if (g_usart0ReceiveComplete) {
        g_usart0ReceiveComplete = false;
        if(usartDecodeIncomingMessage(&decodedMsg) == success) {
            decodedMsg.callback(decodedMsg.payload);
        }
    }
}

void mainloopShutdownRequested() {
    stringifyCurrentState(_localBuffer);
    loggingPutDebug(_localBuffer);
    displayWriteString(_localBuffer);
    statemachineGotoBcuRunning();
}

void mainloopMenu() {
    stringifyCurrentState(_localBuffer);
    loggingPutDebug(_localBuffer);
    displayWriteString(_localBuffer);
}
