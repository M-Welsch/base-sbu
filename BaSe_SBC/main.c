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

#include "flags.h"
#include "gpio_interface.h"
#include "powerpath_control.h"
#include "hmi.h"
#include "usart.h"
#include "heartbeat.h"
#include "rtc.h"
#include "adc.h"


int main(void)
{
	init_flags();
	init_pins();
	init_heatbeat_monitor();
	set_interrupts_for_buttons();
	init_uart();
	init_display();
	adc_init();
	sei();
	
	display_write_string("Standby Unit\nready!");
	USART0_sendString("I:STARTED");
	dim_display(1);
	
    while (1) 
    {
		heartbeat_monitor();
		if(!flag_bpi_heartbeat_ok) {
			/* if heartbeat from bpi is missing, stop loading it with a high-level voltage on its rx-pin! */
			disable_usart_tx();
		}
		
		if (flag_button_0_pressed == true) {
			led_hmi_off();
			flag_button_0_pressed = false;
		}
		if (button_1_pressed() == 1) {
			USART0_sendString("C:Hello\r\n");
		}
		
		if (flag_usart_string_receive_complete == true) {
			USART0_process_incoming_message();
			flag_usart_string_receive_complete = false;
		}
		
		if (flag_string_for_display_received == true) {
			flag_string_for_display_received = false;
			display_clear();
			display_write_string(display_line1_content_from_bcu);
			display_next_line();
			display_write_string(display_line2_content_from_bcu);
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
			//Todo: acknowledge to bcu
			flag_received_seconds_to_next_bu = false;
			rtc_write_seconds_to_cmp();
			rtc_setup();
		}
		
		if (flag_goto_sleep == true) {
			flag_goto_sleep = false;
			USART0_sendString_w_eol("going to sleep ...\n");
			_delay_ms(100);
			SLPCTRL.CTRLA |= SLPCTRL_SMODE_STDBY_gc;
			SLPCTRL.CTRLA |= SLPCTRL_SEN_bm;
			sleep_cpu();
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
}

