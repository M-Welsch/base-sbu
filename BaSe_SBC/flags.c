/*
 * flags.c
 *
 * Created: 03.06.2020 23:06:05
 *  Author: Max
 */ 
#include <stdbool.h>
#include "flags.h"

void init_flags(void) {
	flag_button_0_pressed = false;	
	flag_heartbeat = false;
}

bool flag_bpi_ready_for_shutdown = false;
bool flag_bpi_shutdown_timer_elapsed = false;

bool flag_button_1_pressed = false;

bool flag_bpi_heartbeat_ok = false;
bool flag_string_for_display_received = false;

bool flag_pwr_state_change_request = false;

bool flag_human_readable_timestamp_next_bu_received = false;

bool flag_usart_string_receive_complete = false;
bool flag_received_seconds_to_next_bu = false;
bool flag_goto_sleep = false;