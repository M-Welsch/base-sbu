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


#include "flags.h"
#include "gpio_interface.h"
#include "powerpath_control.h"
#include "hmi.h"
#include "usart.h"
#include "heartbeat.h"


int main(void)
{
	init_flags();
	init_pins();
	init_heatbeat_monitor();
	set_interrupts_for_buttons();
	init_uart();
	init_display();
	sei();
	
	display_write_string("Standby Unit\nready!");
	USART0_sendString("I:STARTED");
	
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
		
		/* let hmi led toggle as a sbc heartbeat */
		
		if (flag_usart_string_receive_complete == true) {
			USART0_process_incoming_message(); //this leads to the display being dark all the time
			display_clear();
			flag_usart_string_receive_complete = false;
			//display_write_string("Received TS\nBaSe Ready.");
			display_write_string(usart_receive_buffer);
		}
		
		_delay_ms(100);		
		toggle_hmi_led();
		send_sbc_heartbeat_count_to_bpi();
    }
}

