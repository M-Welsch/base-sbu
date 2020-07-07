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

bool flag_heartbeat_ok = false;