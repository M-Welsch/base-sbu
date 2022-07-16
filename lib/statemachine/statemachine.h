#ifndef statemachine_h__
#define statemachine_h__

#include "flags.h"

void statemachineInit();

baseSbuError_t statemachineGotoBcuRunning(void);
baseSbuError_t statemachineGotoShutdownRequested(void);
baseSbuError_t statemachineGotoStandby(void);
baseSbuError_t statemachineGotoMenu(void);

void stringifyCurrentState(char *buffer);

#endif