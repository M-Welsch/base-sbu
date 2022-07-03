#ifndef logging_h__
#define logging_h__

#include "usart.h"

#define loggingLevelDebug 'D'
#define loggingLevelInfo 'I'
#define loggingLevelWarning 'W'
#define loggingLevelError 'E'
#define loggingLevelCritical 'C'

void loggingPutMsg(char loglevel, char* msg);

#define loggingPutDebug(msg) loggingPutMsg(loggingLevelDebug, msg)
#define loggingPutInfo(msg) loggingPutMsg(loggingLevelInfo, msg)
#define loggingPutWarning(msg) loggingPutMsg(loggingLevelWarning, msg)
#define loggingPutError(msg) loggingPutMsg(loggingLevelError, msg)
#define loggingPutCritical(msg) loggingPutMsg(loggingLevelCritical, msg)

#endif