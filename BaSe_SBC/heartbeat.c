/*
 * heartbeat.c
 *
 * Created: 07.06.2020 21:57:36
 *  Author: Max
 */ 

#include "heartbeat.h"
#include "gpio_interface.h"
#include "flags.h"
#include "usart.h"
#include <stdio.h>

int heartbeat_count = 0;

void init_heatbeat_monitor(void) {
	setup_heartbeat_interrupt();
}

void heartbeat_monitor(void) {
	if(flag_heartbeat) {
		toggle_display_backlight();
		flag_heartbeat = false;
	}
}

void send_heartbeat_count_to_bpi() {
	sprintf(buffer, "H:%i", increment_or_reset_heartbeat_count());
	USART0_sendString_w_eol(buffer);
}

int get_heartbeat_count() {
	return heartbeat_count;
}

int increment_or_reset_heartbeat_count() {
	if (heartbeat_count == 255) {
		heartbeat_count = 0;
	} else {
		heartbeat_count++;
	}
	return heartbeat_count;
}