#include <stdio.h>

#include "logging.h"
#include "hal_usart.h"

#define LOG_BUFFER_SIZE 128

char buffer[LOG_BUFFER_SIZE+3];  // add 3 chars for level indicator like "I: "

void loggingPutMsg(char loglevel, char* msg) {
    sprintf(buffer, "%c: %s", loglevel, msg);
    USART0_sendString_w_newline_eol(buffer);
}