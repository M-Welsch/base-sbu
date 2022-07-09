#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

typedef enum {
    test,
    write_to_display_line1,
    write_to_display_line2,
    set_display_brightness,
    set_led_brightness,
    set_seconds_to_next_bu,
    send_readable_timestamp_of_next_bu,
    measure_current,
    measure_vcc3v,
    measure_temperature,
    request_shutdown,
    abort_shutdown,
    request_wakeup_reason,
    set_wakeup_reason
} usartCommands_t;

typedef struct {
    usartCommands_t usartCommand;
    char abbrev[5];
    char response_keyw[5];
    void (*callback)();
}usartCommandsStruct ;

void callback_test(void) {
}
void callback_write_to_display_line1(void) {
}
void callback_write_to_display_line2(void) {
}
void callback_set_display_brightness(void) {
}
void callback_set_led_brightness(void) {
}
void callback_set_seconds_to_next_bu(void) {
}
void callback_send_readable_timestamp_of_next_bu(void) {
}
void callback_measure_current(void) {
}
void callback_measure_vcc3v(void) {
}
void callback_measure_temperature(void) {
}
void callback_request_shutdown(void) {
}
void callback_abort_shutdown(void) {
}
void callback_request_wakeup_reason(void) {
}
void callback_set_wakeup_reason(void) {
}

usartCommandsStruct usartCommands[] = {
    {test, "Test", "Echo", callback_test},
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

int main(void) {
    uint8_t lenCommands = sizeof(usartCommands)/sizeof(usartCommands[0]);
    for (uint8_t c = 0; c < lenCommands; c++) {
        usartCommandsStruct cur = usartCommands[c];
        printf("%i: %s, %s\n", cur.usartCommand, cur.abbrev, cur.response_keyw);
        cur.callback();
    }
}