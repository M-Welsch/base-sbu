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

baseSbuError_t usartDecodeIncomingMessage();

#endif