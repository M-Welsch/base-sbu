#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/interrupt.h>
#include "usart.h"
#include "statemachine.h"
#include "hal.h"
#include "flags.h"

#define UNUSED_PARAM(x) (void)(x)

char _returnBuffer[100];

void _usartSendReady() {
	USART0_sendString_w_newline_eol("Ready");
}

void callback_test_for_echo(const char* payload) {
    USART0_sendString_w_newline_eol("Echo");
    _usartSendReady();
}

void callback_write_to_display_line1(const char* payload) {
    displayBufferLine1(payload);
    _usartSendReady();
}

void callback_write_to_display_line2(const char* payload) {
    displayWriteBothLines(payload);
    _usartSendReady();
}

void callback_set_display_brightness(const char* payload) {
    uint16_t _dimmingValue = (uint16_t) strtol(payload, NULL, 10);
    displayDim(_dimmingValue);
    _usartSendReady();
}

void callback_set_led_brightness(const char* payload) {
    uint16_t _dimmingValue = (uint16_t) strtol(payload, NULL, 10);
    ledDim(_dimmingValue);
    _usartSendReady();
}

void callback_set_seconds_to_next_bu(const char* payload) {
    uint16_t _seconds = (uint16_t) strtol(payload, NULL, 10);
    rtcSetWakeupInSeconds(_seconds);
    rtcActivateCompareInterrupt();
    g_nextBackupInfo.secondsToWakeupReceived = true;
    USART0_sendString_w_newline_eol("CMP:123");  // Todo: get rid of this. BCU shouldn't rely on that.
    _usartSendReady();
}

void callback_get_seconds_to_next_bu(const char* payload) {
    char _buffer[48];
    rtcGetSecondsToNextBuAndCount(_buffer);
    USART0_sendString_w_newline_eol(_buffer);
    _usartSendReady();
}

void callback_send_readable_timestamp_of_next_bu(const char* payload) {
    strcpy(g_nextBackupInfo.humanReadableTimestamp, payload);
    g_nextBackupInfo.humanReadableTimestamp[16] = '\0';
    g_nextBackupInfo.humanReadableTimestampReceived = true;
    _usartSendReady();
}

void callback_read_readable_timestamp_of_next_bu(const char* payload) {
    USART0_sendString_w_newline_eol(g_nextBackupInfo.humanReadableTimestamp);
    _usartSendReady();
}

void callback_measure_current(const char* payload) {
    UNUSED_PARAM(payload);
    sprintf(_returnBuffer, "CC:%d", adcResultInCur());
    USART0_sendString_w_newline_eol(_returnBuffer);
    _usartSendReady();
}

void callback_measure_vcc3v(const char* payload) {
    UNUSED_PARAM(payload);
    sprintf(_returnBuffer, "3V:%d", adcResult3v3());
    USART0_sendString_w_newline_eol(_returnBuffer);
    _usartSendReady();
}

void callback_measure_temperature(const char* payload) {
    UNUSED_PARAM(payload);
    sprintf(_returnBuffer, "TP:%d", adcResult3v3());
    USART0_sendString_w_newline_eol(_returnBuffer);
    _usartSendReady();
}

void callback_request_shutdown(const char* payload) {
    UNUSED_PARAM(payload);
    statemachineGotoShutdownRequested();
    _usartSendReady();
}

void callback_abort_shutdown(const char* payload) {
    UNUSED_PARAM(payload);
    statemachineGotoBcuRunning();
    _usartSendReady();
}

void callback_get_reset_reason(const char* payload) {
    char buffer[32];
    sprintf(buffer, "RST: 0x%x", RSTCTRL_RSTFR);
    USART0_sendString_w_newline_eol(buffer);
    _usartSendReady();
}

typedef struct {
    wakeupReasons_t reason;
    char keyword[10];
} wakeupReasonsMap_t;

wakeupReasonsMap_t wakeupReasonMap[] = {
    {BACKUP_NOW, "BACKUP"},
    {SCHEDULED_BACKUP, "SCHEDULED"},
    {CONFIGURATION, "CONFIG"},
    {HEARTBEAT_TIMEOUT, "HEARTBEAT"},
    {NO_REASON,  "NO_REASON"}
};


void callback_request_wakeup_reason(const char* payload) {
    UNUSED_PARAM(payload);
    USART0_sendString_w_newline_eol(wakeupReasonMap[g_wakeupReason].keyword);
    _usartSendReady();
}

void callback_set_wakeup_reason(const char* payload) {
    char _buffer[32];
    for (uint8_t c = 0; c < DIMENSION_OF(wakeupReasonMap); c++) {
        if (strcmp(payload, wakeupReasonMap[c].keyword) == 0) {
            g_wakeupReason = wakeupReasonMap[c].reason;
            sprintf(_buffer, "set %s (%d) as Wakeup Reason", wakeupReasonMap[c].keyword, wakeupReasonMap[c].reason);
            USART0_sendString_w_newline_eol(_buffer);
            _usartSendReady();
            return;
        }
    }
    sprintf(_buffer, "ival. WR: %s", payload);
    USART0_sendString_w_newline_eol(_buffer);
    _usartSendReady();
}

typedef enum {
    test_for_echo = 0,
    write_to_display_line1 = 1,
    write_to_display_line2 = 2,
    set_display_brightness = 3,
    set_led_brightness = 4,
    set_seconds_to_next_bu = 5,
    get_seconds_to_next_bu = 6,
    send_readable_timestamp_of_next_bu = 7,
    read_readable_timestamp_of_next_bu = 8,
    measure_current = 9,
    measure_vcc3v = 10,
    measure_temperature = 11,
    request_shutdown = 12,
    abort_shutdown = 13,
    request_wakeup_reason = 14,
    set_wakeup_reason = 15,
    get_reset_reason = 16,
    __end__ = -1
} usartCommands_t;

typedef struct {
    usartCommands_t usartCommand;
    char msgCode[3];
    void (*callback)(const char* payload);
}usartCommandsStruct ;

usartCommandsStruct usartCommands[] = {
    {test_for_echo, "Te", callback_test_for_echo},
    {write_to_display_line1, "D1", callback_write_to_display_line1},
    {write_to_display_line2, "D2", callback_write_to_display_line2},
    {set_display_brightness, "DB", callback_set_display_brightness},
    {set_led_brightness, "DL", callback_set_led_brightness},
    {set_seconds_to_next_bu, "BU", callback_set_seconds_to_next_bu},
    {get_seconds_to_next_bu, "BG", callback_get_seconds_to_next_bu},
    {send_readable_timestamp_of_next_bu, "BR", callback_send_readable_timestamp_of_next_bu},
    {read_readable_timestamp_of_next_bu, "BS", callback_read_readable_timestamp_of_next_bu},
    {measure_current, "CC", callback_measure_current},
    {measure_vcc3v, "3V", callback_measure_vcc3v},
    {measure_temperature, "TP", callback_measure_temperature},
    {request_shutdown, "SR", callback_request_shutdown},
    {abort_shutdown, "SA", callback_abort_shutdown},
    {request_wakeup_reason, "WR", callback_request_wakeup_reason},
    {set_wakeup_reason, "WD", callback_set_wakeup_reason},
    {get_reset_reason, "RS", callback_get_reset_reason}
};

/**
 * @brief sends an acknowledge back over usart 0
 * @example for the message "D1:Stuff to Display" this function sends "ACK:D1" back.
 * 
 * @param[in] msgCode 2-letter message code of the message that shall be acknowledged
 */
void _acknowledge(const char *msgCode) {
    _USART0SendString("ACK:");
    USART0_sendString_w_newline_eol(msgCode);
}

/**
 * @brief decodes messages from sbu
 * @details deconstructs the message into its message code and payload. Then match the
 *          message code with a callback and call it with the payload as argument.
 *          For example: "D1:Stuff to Display" is decomposed into
 *          - msgCode "D1"
 *          - payload "Stuff to Display"
 *          a struct with a callback function pointer and the payload is returned
 * @return struct with the callback function pointer and the payload.
 */
baseSbuError_t usartDecodeIncomingMessage() {
    baseSbuError_t retval = fail;
    char messageCode[3]; 
    char payload[40];
    cli();
    strncpy(messageCode, g_usartReceiveBuffer, 2);
    messageCode[2] = '\0';
    strcpy(payload, g_usartReceiveBuffer+3);
    payload[16] = '\0';
    sprintf(_returnBuffer, "proc: %s", g_usartReceiveBuffer);
    USART0_sendString_w_newline_eol(_returnBuffer);

    for (uint8_t i = 0; i < DIMENSION_OF(usartCommands); i++) {
        usartCommandsStruct cur = usartCommands[i];
        sprintf(_returnBuffer, "check %s == %s", cur.msgCode, messageCode);
        USART0_sendString_w_newline_eol(_returnBuffer);
        if(strcmp(messageCode, cur.msgCode) == 0) {
            _acknowledge(cur.msgCode);
            cur.callback(payload);
            retval = success;
            break;
        }
    }
    sei();
    return retval;
}