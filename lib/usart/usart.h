#ifndef usart_h__ 
#define usart_h__

#include "flags.h"

#define DIMENSION_OF(a) (sizeof(a) / sizeof(a[0]) )

typedef enum {
    write_to_display_line1 = 1,
    write_to_display_line2 = 2,
    set_display_brightness = 3,
    set_led_brightness = 4,
    set_seconds_to_next_bu = 5,
    send_readable_timestamp_of_next_bu = 6,
    measure_current = 7,
    measure_vcc3v = 8,
    measure_temperature = 9,
    request_shutdown = 10,
    abort_shutdown = 11,
    request_wakeup_reason = 12,
    set_wakeup_reason = 13,
    __end__ = -1
} usartCommands_t;

typedef struct {
    usartCommands_t usartCommand;
    char msgCode[3];
    void (*callback)(const char* payload);
}usartCommandsStruct ;

typedef struct {
    void (*callback)();
    char payload[34];
} usartDecodedMsg_t;

void callback_write_to_display_line1(const char* payload);
void callback_write_to_display_line2(const char* payload);
void callback_set_display_brightness(const char* payload);
void callback_set_led_brightness(const char* payload);
void callback_set_seconds_to_next_bu(const char* payload);
void callback_send_readable_timestamp_of_next_bu(const char* payload);
void callback_measure_current(const char* payload);
void callback_measure_vcc3v(const char* payload);
void callback_measure_temperature(const char* payload);
void callback_request_shutdown(const char* payload);
void callback_abort_shutdown(const char* payload);
void callback_request_wakeup_reason(const char* payload);
void callback_set_wakeup_reason(const char* payload);

baseSbuError_t usartDecodeIncomingMessage(usartDecodedMsg_t *decodedMsg);

#endif