#ifndef statemachine_h__
#define statemachine_h__

typedef enum {
    success,
    invalid_transfer
} statemachineError;

void statemachineInit();

statemachineError statemachineGotoBcuRunning(void);
statemachineError statemachineGotoShutdownRequested(void);
statemachineError statemachineGotoStandby(void);
statemachineError statemachineGotoMenu(void);

char *stringifyCurrentState();

#endif