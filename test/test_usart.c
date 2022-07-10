#include <string.h>

#include "usart.h"
#include "flags.h"
#include "hal_usart.h"
#include "mock_hal_led.h"
#include "mock_hal_usart.h"

#define DIMENSION_OF(a) (sizeof(a) / sizeof(a[0]) )
void test_usartDecodeIncomingMessage_should_returnSuccessAndValidStuff_if_Match() {
    char incomingMessage[] = "3V:";
    strcpy(usart_receive_buffer, incomingMessage);
    USART0_sendString_w_newline_eol_Ignore();
    usartDecodedMsg_t outputMsg;

    TEST_ASSERT_EQUAL_INT(success, usartDecodeIncomingMessage(&outputMsg));
    TEST_ASSERT_EQUAL_STRING("", outputMsg.payload);
    TEST_ASSERT_EQUAL_PTR(callback_measure_vcc3v, outputMsg.callback);
}

void test_usartDecodeIncomingMessage_should_returnSuccess_if_noColonTolken() {
    char incomingMessage[] = "3V";
    strcpy(usart_receive_buffer, incomingMessage);
    USART0_sendString_w_newline_eol_Ignore();
    usartDecodedMsg_t outputMsg;

    TEST_ASSERT_EQUAL_INT(success, usartDecodeIncomingMessage(&outputMsg));
    TEST_ASSERT_EQUAL_PTR(callback_measure_vcc3v, outputMsg.callback);
}

void test_usartDecodeIncomingMessage_should_returnFail_if_noMatch() {
    char incomingMessage[] = "nix:pl";
    strcpy(usart_receive_buffer, incomingMessage);
    USART0_sendString_w_newline_eol_Ignore();
    usartDecodedMsg_t outputMsg;

    TEST_ASSERT_EQUAL_INT(fail, usartDecodeIncomingMessage(&outputMsg));
}

void test_usartDecodeIncomingMessage_should_returnFail_if_noMatchAndNoColonTolken() {
    char incomingMessage[] = "sdf";
    strcpy(usart_receive_buffer, incomingMessage);
    USART0_sendString_w_newline_eol_Ignore();
    usartDecodedMsg_t outputMsg;

    TEST_ASSERT_EQUAL_INT(fail, usartDecodeIncomingMessage(&outputMsg));
}

void test_usartDecodeIncomingMessage_should_returnSuccessAndValidStuff_if_Matches() {
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
    char incomingPayload[33];
    usartDecodedMsg_t outputMsg;
    for (uint8_t c = 0; c < DIMENSION_OF(usartCommands); c++) {
        usartCommandsStruct cur = usartCommands[c];
        strcpy(incomingPayload, "myPayload");
        sprintf(usart_receive_buffer, "%s:%s", cur.msgCode, incomingPayload);
        USART0_sendString_w_newline_eol_Ignore();

        TEST_ASSERT_EQUAL_INT(success, usartDecodeIncomingMessage(&outputMsg));
        TEST_ASSERT_EQUAL_STRING(incomingPayload, outputMsg.payload);
        TEST_ASSERT_EQUAL_PTR(cur.callback, outputMsg.callback);
    }
}