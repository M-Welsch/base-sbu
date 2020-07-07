#include <avr/io.h>
#include <stdbool.h>
#include "flags.h"
#include "gpio_interface.h"
#include "powerpath_control.h"

void enable_5v0_smps(void) {
	PORTA.OUT |= en_5v;
}

void disable_5v0_smps(void) {
	PORTA.OUT &= ~en_5v;
}

void enable_bpi_sply(void) {
	PORTB.OUT &= ~dis_bpi_sply;
}

void disable_bpi_sply(void) {
	PORTB.OUT |= dis_bpi_sply;
}

void goto_pwr_state(enum pwr_states pwr_state) {
	switch(pwr_state) {
		case standby:
		if (flag_bpi_ready_for_shutdown == true && flag_bpi_shutdown_timer_elapsed == true) {
			disable_bpi_sply();
			disable_5v0_smps();
			disable_ltc4359();
		}
		break;
		
		case display_on:
		
		break;
		case active:
			// Todo: check if conditions for save startup are met ... if there are any
			enable_5v0_smps();
			enable_ltc4359();
			enable_bpi_sply();
			break;
	}
}