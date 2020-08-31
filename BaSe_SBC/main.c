/*
 * BaSe_SBC.c
 *
 * Created: 17.05.2020 21:02:05
 * Author : Max
 */ 

#define F_CPU 8000000
#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <avr/sleep.h>
#include <string.h>

#include <avr/interrupt.h>

#include "flags.h"
#include "gpio_interface.h"
#include "powerpath_control.h"
#include "hmi.h"
#include "usart.h"
#include "heartbeat.h"
#include "rtc.h"
#include "adc.h"
#include "menu.h"

/* function prototypes for mainloops */

void mainloop_active();
void mainloop_standby();
void show_menu_timestamp();
void mainloop_display_on();
void back_to_main_menu();
void wake_bcu_and_do_backup_now();
void wake_bcu();
void show_menu_actions();
void show_main_menu();

void init_sbu()
{
	init_flags();
	init_pins();
	init_heatbeat_monitor();
	set_interrupts_for_buttons();
	init_uart();
	init_display();
	adc_init();
	strcpy(human_readable_timestamp_next_bu, "next BU in 168h\n09.01.1990 12:57");
	sei();
}

int main(void)
{
	init_sbu();

	display_write_string("Standby Unit\nready!");
	USART0_sendString("I:STARTED");
	dim_display(1);
	
	current_pwr_state = active;
	void (*mainloop)() = mainloop_active;

	
    while (1) 
    {
		switch(current_pwr_state) {
			case active:
				mainloop = mainloop_active;
				break;
			case standby:
				mainloop = mainloop_standby;
				break;
			case display_on:
				mainloop = mainloop_display_on;
				break;
		}
		mainloop();
    }
}

void goto_sleep_standby()
{
	SLPCTRL.CTRLA |= SLPCTRL_SMODE_STDBY_gc;
	SLPCTRL.CTRLA |= SLPCTRL_SEN_bm;
	sleep_cpu();
}

void goto_sleep_idle()
{
	SLPCTRL.CTRLA |= SLPCTRL_SMODE_IDLE_gc;
	SLPCTRL.CTRLA |= SLPCTRL_SEN_bm;
	sleep_cpu();
}

void mainloop_active()
{
	dim_display(1);
	heartbeat_monitor();
	if(!flag_bpi_heartbeat_ok) {
		/* if heartbeat from bpi is missing, stop loading it with a high-level voltage on its rx-pin! */
		disable_usart_tx();
	}
	
	if (flag_button_0_pressed == true) {
		flag_button_0_pressed = false;
		led_hmi_off();
	}
	
	if (flag_button_1_pressed == true) {
		flag_button_1_pressed = false;
		USART0_sendString("C:Hello\r\n");
	}
	
	if (flag_usart_string_receive_complete == true) {
		USART0_process_incoming_message();
		flag_usart_string_receive_complete = false;
	}
	
	if (flag_string_for_display_received == true) {
		flag_string_for_display_received = false;
		display_clear();
		display_write_string(display_line1_content);
		display_next_line();
		display_write_string(display_line2_content);
		USART0_sendString_w_eol("New Display\n"); //<- line is being received, but no new content on display??
	}
	
	if (flag_pwr_state_change_request == true) {
		display_clear();
		display_write_string("Received Shut-\ndown Request!");
		flag_pwr_state_change_request = false;
		goto_pwr_state(next_pwr_state);
	}
	
	if (flag_human_readable_timestamp_next_bu_received == true) {
		display_clear();
		display_write_string(human_readable_timestamp_next_bu);
		flag_human_readable_timestamp_next_bu_received = false;
	}
	
	if (flag_received_seconds_to_next_bu == true) {
		sprintf(buffer, "Will wake up in %ld seconds\n", seconds_to_next_bu);
		USART0_sendString_w_eol(buffer);
		flag_received_seconds_to_next_bu = false;
		rtc_write_seconds_to_cmp();
		rtc_setup();
	}
	
	if (flag_goto_sleep == true) {
		flag_goto_sleep = false;
		USART0_sendString_w_eol("going to sleep ...\n");
		_delay_ms(100);
		goto_sleep_standby();

	}
	
	if (flag_request_current_measurement == true) {
		flag_request_current_measurement = false;
		uint16_t input_current = adc_measure_input_current();
		sprintf(buffer,"Cur: %d", input_current);
		USART0_sendString_w_eol(buffer);
		display_clear();
		display_write_string(buffer);
	}
	
	if	(flag_request_temperature_measurement == true) {
		flag_request_temperature_measurement = false;
		uint16_t temperature = adc_measure_temperature();
		sprintf(buffer, "TMP: %d", temperature);
		USART0_sendString_w_eol(buffer);
		display_clear();
		display_write_string(buffer);
	}
	
	if (flag_request_3v3_measurement == true) {
		flag_request_3v3_measurement = false;
		uint16_t voltage_3v3 = adc_measure_3v3();
		sprintf(buffer, "3V3: %d", voltage_3v3);
		USART0_sendString_w_eol(buffer);
		display_clear();
		display_write_string(buffer);
	}
	
	_delay_ms(100);
	
	/* let hmi led toggle as a sbc heartbeat */
	toggle_hmi_led();
	// send_sbc_heartbeat_count_to_bpi();
}

void mainloop_standby() {
	if (flag_button_0_pressed | flag_button_1_pressed) {
		flag_button_0_pressed = false;
		flag_button_1_pressed = false;
		
		next_pwr_state = display_on;
		goto_pwr_state(next_pwr_state);
	}	
	_delay_ms(100);
}

void mainloop_display_on() {
	if (flag_entering_mainloop_display_on) {
		flag_entering_mainloop_display_on = false;
		show_menu = show_main_menu;
		button0_action = show_menu_timestamp;
		button1_action = show_menu_actions;
		dim_display(1);
	}
	show_menu();
	sprintf(buffer, "flag0: %d, flag1: %d\n",flag_button_0_pressed,flag_button_1_pressed);
	USART0_sendString(buffer);
	_delay_ms(100);
	while(!flag_button_0_pressed & !flag_button_1_pressed) {
		;
	}
	//goto_sleep_idle();
	//Fixme: button1 press cannot wake mcu from standby!
	
	if (flag_button_0_pressed) {
		flag_button_0_pressed = false;
		button0_action();
	}
	
	if (button_1_pressed()) {
		flag_button_1_pressed = false;
		button1_action();
	}
	
}