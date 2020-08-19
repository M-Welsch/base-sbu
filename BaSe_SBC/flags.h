/*
 * IncFile1.h
 *
 * Created: 17.05.2020 22:33:02
 *  Author: Max
 */ 
#include <stdbool.h>

#ifndef INCFILE1_H_
#define INCFILE1_H_

/* flags */

bool flag_bpi_ready_for_shutdown;
bool flag_bpi_shutdown_timer_elapsed;
bool flag_button_1_pressed;
bool flag_bpi_heartbeat_ok;
bool flag_usart_string_receive_complete;
bool flag_string_for_display_received;
bool flag_pwr_state_change_request;
bool flag_human_readable_timestamp_next_bu_received;
bool flag_usart_string_receive_complete;
bool flag_received_seconds_to_next_bu;
bool flag_goto_sleep;
bool flag_request_current_measurement;
bool flag_request_temperature_measurement;
bool flag_request_3v3_measurement;
bool flag_5v0_up;
bool flag_bcu_sply_up;

/* globals */
volatile bool flag_button_0_pressed;
volatile bool flag_heartbeat;
long seconds_to_next_bu;

enum pwr_states {
	standby = 0,
	display_on = 1,
	active = 2,
} next_pwr_state, current_pwr_state;

/* function prototypes */
void init_flags(void);

#endif /* INCFILE1_H_ */