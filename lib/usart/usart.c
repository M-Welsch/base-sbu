#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hal_usart.h"
#include "usart.h"
#include "hal_display.h"
#include "hal_led.h"
#include "hal_rtc.h"
#include "hal_adc.h"
#include "statemachine.h"
#include "logging.h"

#define UNUSED_PARAM(x) (void)(x)

char _displayLine1Buffer[17];
char _returnBuffer[33];

void callback_write_to_display_line1(const char* payload) {
    strcpy(_displayLine1Buffer, payload);
    USART0_send_ready();
}

void callback_write_to_display_line2(const char* payload) {
    char displaybuffer[34];
    sprintf(displaybuffer, "%s\n%s", _displayLine1Buffer, payload);
    displayClear();
    displayWriteString(displaybuffer);
    USART0_send_ready();
}

void callback_set_display_brightness(const char* payload) {
    uint16_t _dimmingValue = atoi(payload);
    displayDim(_dimmingValue);
    USART0_send_ready();
}

void callback_set_led_brightness(const char* payload) {
    uint16_t _dimmingValue = atoi(payload);
    ledDim(_dimmingValue);
    USART0_send_ready();
}

void callback_set_seconds_to_next_bu(const char* payload) {
    uint16_t _seconds = atoi(payload);
    rtcSetWakeupInSeconds(_seconds);
    g_nextBackupInfo.secondsToWakeupReceived = true;
    USART0_sendString_w_newline_eol("CMP:123");  // Todo: get rid of this. BCU shouldn't rely on that.
    USART0_send_ready();
}

void callback_send_readable_timestamp_of_next_bu(const char* payload) {
    strcpy(g_nextBackupInfo.humanReadableTimestamp, payload);
    g_nextBackupInfo.humanReadableTimestampReceived = true;
    USART0_send_ready();
}

void callback_measure_current(const char* payload) {
    UNUSED_PARAM(payload);
    sprintf(_returnBuffer, "CC:%d", adcResultInCur());
    USART0_sendString_w_newline_eol(_returnBuffer);
    USART0_send_ready();
}

void callback_measure_vcc3v(const char* payload) {
    UNUSED_PARAM(payload);
    sprintf(_returnBuffer, "3V:%d", adcResult3v3());
    USART0_sendString_w_newline_eol(_returnBuffer);
    USART0_send_ready();
}

void callback_measure_temperature(const char* payload) {
    UNUSED_PARAM(payload);
    sprintf(_returnBuffer, "TP:%d", adcResult3v3());
    USART0_sendString_w_newline_eol(_returnBuffer);
    USART0_send_ready();
}

void callback_request_shutdown(const char* payload) {
    UNUSED_PARAM(payload);
    statemachineGotoShutdownRequested();
    USART0_send_ready();
}

void callback_abort_shutdown(const char* payload) {
    UNUSED_PARAM(payload);
    statemachineGotoBcuRunning();
    USART0_send_ready();
}

wakeupReasonsMap_t wakeupReasonMap[] = {
    {BACKUP_NOW, "BACKUP_NOW"},
    {SCHEDULED_BACKUP, "SCHEDULED_BACKUP"},
    {CONFIGURATION, "CONFIGURATION"},
    {HEARTBEAT_TIMEOUT, "HEARTBEAT_TIMEOUT"},
    {NO_REASON,  "NO_REASON"}
};

void callback_request_wakeup_reason(const char* payload) {
    UNUSED_PARAM(payload);
    USART0_send_ready(wakeupReasonMap[g_wakeupReason].keyword);
}

void callback_set_wakeup_reason(const char* payload) {
    char _buffer[32];
    for (uint8_t c = 0; c < DIMENSION_OF(wakeupReasonMap); c++) {
        if (strcmp(payload, wakeupReasonMap[c].keyword) == 0) {
            g_wakeupReason = wakeupReasonMap[c].reason;
            sprintf(_buffer, "set %s (%d) as Wakeup Reason", wakeupReasonMap[c].keyword, wakeupReasonMap[c].reason);
            USART0_sendString_w_newline_eol(_buffer);
            USART0_send_ready();
            return;
        }
    }
    sprintf(_buffer, "no such WakeupReason as %s", payload);
    loggingPutWarning(_buffer);
    USART0_send_ready();
}

usartCommandsStruct usartCommands[] = {
    {write_to_display_line1, "D1", "", callback_write_to_display_line1},
    {write_to_display_line2, "D2", "", callback_write_to_display_line2},
    {set_display_brightness, "DB", "", callback_set_display_brightness},
    {set_led_brightness, "DL", "", callback_set_led_brightness},
    {set_seconds_to_next_bu, "BU", "CMP", callback_set_seconds_to_next_bu},
    {send_readable_timestamp_of_next_bu, "BR", "", callback_send_readable_timestamp_of_next_bu},
    {measure_current, "CC", "CC", callback_measure_current},
    {measure_vcc3v, "3V", "3V", callback_measure_vcc3v},
    {measure_temperature, "TP", "TP", callback_measure_temperature},
    {request_shutdown, "SR", "", callback_request_shutdown},
    {abort_shutdown, "SA", "", callback_abort_shutdown},
    {request_wakeup_reason, "WR", "", callback_request_wakeup_reason},
    {set_wakeup_reason, "WD", "", callback_set_wakeup_reason}
};

/**
 * @brief sends an acknowledge back over usart 0
 * @example for the message "D1:Stuff to Display" this function sends "ACK:D1" back.
 * 
 * @param[in] msgCode 2-letter message code of the message that shall be acknowledged
 */
void _Acknowledge(char *msgCode) {
    static char _usartLocalBuffer[38];
    sprintf(_usartLocalBuffer, "ACK:%s", msgCode);
    USART0_sendString_w_newline_eol(_usartLocalBuffer);
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
baseSbuError_t usartDecodeIncomingMessage(usartDecodedMsg_t *decodedMsg) {
    char const *messageCode; 
    char const *payload;
    char usart_receive_copy[33];

    strcpy(usart_receive_copy, g_usartReceiveBuffer);

    messageCode = strtok(usart_receive_copy, ":");
    for (uint8_t i = 0; i < DIMENSION_OF(usartCommands); i++) {
        usartCommandsStruct cur = usartCommands[i];
        if(strcmp(messageCode, cur.msgCode) == 0) {
            _Acknowledge(cur.msgCode);
            decodedMsg->callback = cur.callback;
            payload = strtok(NULL, "\0");
            if (payload != NULL)
                strcpy(decodedMsg->payload, payload);
            return success;
        }
    }
    return fail;
}