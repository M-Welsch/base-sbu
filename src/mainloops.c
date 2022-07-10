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
        sprintf(_localBuffer, "received: %s\n", g_usartReceiveBuffer);
        loggingPutDebug(_localBuffer);
        g_usart0ReceiveComplete = false;
        if(usartDecodeIncomingMessage(&decodedMsg) == success) {
            decodedMsg.callback(decodedMsg.payload);
        }
    }
    //loggingPutDebug(stringifyCurrentState());
    //displayWriteString(stringifyCurrentState());
    //statemachineGotoShutdownRequested();
}

void mainloopShutdownRequested() {
    loggingPutDebug(stringifyCurrentState());
    displayWriteString(stringifyCurrentState());
    statemachineGotoBcuRunning();
}

void mainloopMenu() {
    loggingPutDebug(stringifyCurrentState());
    displayWriteString(stringifyCurrentState());
}
