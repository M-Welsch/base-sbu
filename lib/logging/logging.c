#include <stdio.h>

#include "logging.h"

#define LOG_BUFFER_SIZE 32

char buffer[LOG_BUFFER_SIZE+3];  // add 3 chars for level indicator like "I: "

void loggingPutMsg(char loglevel, char* msg) {
    // *(msg+LOG_BUFFER_SIZE-1) = '\0';
    sprintf(buffer, "%c: %s", loglevel, msg);
    USART0_sendString_w_newline_eol(buffer);
}