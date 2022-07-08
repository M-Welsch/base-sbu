#include "mainloops.h"
#include "logging.h"
#include "statemachine.h"
#include "hal_display.h"

void mainloopBcuRunning() {
    loggingPutDebug(stringifyCurrentState());
    displayWriteString(stringifyCurrentState());
    statemachineGotoShutdownRequested();
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
