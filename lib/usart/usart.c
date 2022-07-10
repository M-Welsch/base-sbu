#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hal_usart.h"
#include "usart.h"
#include "hal_display.h"

char _displayLine1Buffer[17];

void callback_write_to_display_line1(char* payload) {
    strcpy(_displayLine1Buffer, payload);
    USART0_send_ready();
}

void callback_write_to_display_line2(char* payload) {
    char displaybuffer[34];
    sprintf(displaybuffer, "%s\n%s", _displayLine1Buffer, payload);
    displayClear();
    displayWriteString(displaybuffer);
    USART0_send_ready();
}

void callback_set_display_brightness(char* payload) {
    USART0_send_ready();
}

void callback_set_led_brightness(char* payload) {
    USART0_send_ready();
}

void callback_set_seconds_to_next_bu(char* payload) {
    USART0_send_ready();
}

void callback_send_readable_timestamp_of_next_bu(char* payload) {
    USART0_send_ready();
}

void callback_measure_current(char* payload) {
    USART0_send_ready();
}

void callback_measure_vcc3v(char* payload) {
    USART0_send_ready();
}

void callback_measure_temperature(char* payload) {
    USART0_send_ready();
}

void callback_request_shutdown(char* payload) {
    USART0_send_ready();
}

void callback_abort_shutdown(char* payload) {
    USART0_send_ready();
}

void callback_request_wakeup_reason(char* payload) {
    USART0_send_ready();
}

void callback_set_wakeup_reason(char* payload) {
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