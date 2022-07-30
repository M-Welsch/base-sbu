#define F_CPU 3333333

#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>
#include <string.h>
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

void transition_to_pwr_state(enum pwr_states pwr_state) {
	switch(pwr_state) {
		case standby:
			if (current_pwr_state == active) {
				wait_until_bpi_ready_for_shutdown();
			}
			disable_bpi_sply();
			dim_display(0);
			led_hmi_off();
			set_all_display_pins_to_input();
			disable_5v0_smps();
			//flag_goto_sleep = true; //why was that set true here??
			current_pwr_state = standby;
			break;
		
		case display_on:
			current_pwr_state = display_on;
			set_interrupts_for_buttons();
			enable_5v0_smps();
			set_all_display_pins_to_output();
			_delay_ms(5);
			init_display();
			flag_entering_mainloop_display_on = true;
			if(flag_wakeup_by_rtc) {
				/* if rtc woke the sbu, it has to go to the active state immediately */
				pwr_state = active;
			} else {
				break;
			}
			
		case active:
			// Todo: check if conditions for save startup are met ... if there are any
			strcpy(display_line1_content,"BCU starting up");
			strcpy(display_line2_content,"Please wait ...");
			enable_5v0_smps();
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