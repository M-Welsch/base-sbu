#include <avr/io.h>
#include <stdbool.h>
#include "flags.h"
#include "gpio_interface.h"
#include "powerpath_control.h"
#include "adc.h"
#include "hmi.h"

void enable_5v0_smps(void) {
	PORTA.OUT |= en_5v;
	flag_5v0_up = true;
}

void disable_5v0_smps(void) {
	PORTA.OUT &= ~en_5v;
	flag_5v0_up = false;
}

void enable_bpi_sply(void) {
	PORTB.OUT &= ~dis_bpi_sply;
	flag_bcu_sply_up = true;
}

void disable_bpi_sply(void) {
	PORTB.OUT |= dis_bpi_sply;
	flag_bcu_sply_up = false;
}

void goto_pwr_state(enum pwr_states pwr_state) {
	switch(pwr_state) {
		case standby:
			wait_until_bpi_ready_for_shutdown();
			disable_bpi_sply();
			dim_display(0);
			led_hmi_off();
			disable_5v0_smps();
			flag_goto_sleep = true;
			current_pwr_state = standby;
			break;
		
		case display_on:
			current_pwr_state = display_on;
			enable_5v0_smps();
			init_display();
			break;
			
		case active:
			// Todo: check if conditions for save startup are met ... if there are any
			display_write_string("Hello Folks!\nStarting BCU ..");
			enable_bpi_sply();
			current_pwr_state = active;
			break;
	}
}

bool wait_until_bpi_ready_for_shutdown() {
	display_clear();
	display_write_string("Waiting for BCU\nHB to stop");
	while (flag_bpi_heartbeat_ok == true) {
		;
	}
	display_clear();
	display_write_string("Waiting for 3V3\nto go low");
	while (vcc3v3_present() == true) {
		;
	}
	return true;
}