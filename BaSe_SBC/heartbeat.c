/*
 * heartbeat.c
 *
 * Created: 07.06.2020 21:57:36
 *  Author: Max
 */ 

#include "heartbeat.h"
#include "gpio_interface.h"
#include "flags.h"

void init_heatbeat_monitor(void) {
	setup_heartbeat_interrupt();
}

void heartbeat_monitor(void) {
	if(flag_heartbeat) {
		toggle_display_backlight();
		flag_heartbeat = false;
	}
}