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

#include "flags.h"
#include "gpio_interface.h"
#include "powerpath_control.h"
#include "hmi.h"
#include "usart.h"
#include "heartbeat.h"


int main(void)
{
	init_pins();
	init_heatbeat_monitor();
	set_interrupts_for_buttons();
	init_uart();
	init_display();
	sei();
	
	display_write_string("Hallo Welt!\n");
	USART0_sendString("I:STARTED");
	
    while (1) 
    {
		//heartbeat_monitor();
		//if(!flag_heartbeat_ok) {
			///* if heartbeat from bpi is missing, stop loading it with a high-level voltage on its rx-pin! */
			//disable_usart_tx();
		//}
		
		/* HEART Beat processing doesn't work! */
		
		if (flag_button_0_pressed == true) {
			led_hmi_off();
		}
		if (button_1_pressed() == 1) {
			dim_display(1);
			USART0_sendString("C:Hello\r\n");
			} else {
			dim_display(0);
		}
		
    }
}

