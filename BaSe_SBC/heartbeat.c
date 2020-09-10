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
	setup_interrupt_for_hb_edge_detection();
	setup_interrupt_timeout_detection();
}

void setup_interrupt_timeout_detection() {
	
	TCA0.SINGLE.PER = 0xA2B; // with prescaler of 256 and fclk=3.333.333Hz this results to a period of 200ms. Means, every 200ms the timer throws an interrupt (unless it is resetted before)
	
	TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;
}


void heartbeat_monitor(void) {
	if(flag_heartbeat) {
		reset_bpi_hb_timeout_timer();
		flag_heartbeat = false;
	}
}

void reset_bpi_hb_timeout_timer() {
	;
}

void send_sbc_heartbeat_count_to_bpi() {
	sprintf(buffer, "H:%i", increment_or_reset_sbc_heartbeat_count());
	USART0_sendString_w_newline_eol(buffer);
}

int get_sbc_heartbeat_count() {
	return heartbeat_count;
}

int increment_or_reset_sbc_heartbeat_count() {
	if (heartbeat_count == 255) {
		heartbeat_count = 0;
	} else {
		heartbeat_count++;
	}
	return heartbeat_count;
}

ISR(TCA0_CMP0_vect) {
	flag_bpi_heartbeat_ok = false;
}