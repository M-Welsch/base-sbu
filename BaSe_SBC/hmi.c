/*
 * hmi.c
 *
 * Created: 01.06.2020 22:37:26
 *  Author: Max
 */ 

#define F_CPU 20000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "flags.h"
#include "gpio_interface.h"

/* Display Control */

void sweep_display_pins(void) {
	set_display_data_pins(0x01);
	_delay_ms(100);
	set_display_data_pins(0x02);
	_delay_ms(100);
	set_display_data_pins(0x04);
	_delay_ms(100);
	set_display_data_pins(0x08);
	_delay_ms(100);
	set_display_data_pins(0x00);
	_delay_ms(100);
	display_enable(100);
	_delay_ms(100);
	toggle_rs_pin();
	_delay_ms(100);
	PORTA.OUT = 0x00;
}

void init_display(void) {
	/* refering to datasheet SPLC780D and (more importantly) lcd.py of "old" BaSe implementation*/
	_delay_ms(100);
	display_clear_rs_pin();
	
	set_display_data_pins(0x03);
	display_enable(1);
	_delay_ms(1);
	set_display_data_pins(0x03);
	display_enable(1);
	_delay_ms(1);
	
	set_display_data_pins(0x03);
	display_enable(1);
	_delay_ms(1);
	set_display_data_pins(0x02);
	display_enable(1);
	_delay_ms(1);
	
	set_display_data_pins(0x02);
	display_enable(1);
	_delay_ms(1);
	/* the following instruction ...
	   DB7 DB6 DB5 DB4 |
	   N   F   X   X   | N = HIGH => 2 lined display. F = don't care = 0
	   1   0   0   0 => 0x8 */
	set_display_data_pins(0x08);
	display_enable(1);
	_delay_ms(1);
	
	set_display_data_pins(0x00);
	display_enable(1);
	_delay_ms(1);
	/* The following instruction ...
	   DB7 DB6 DB5 DB4 |
	   1   D   C   B   |
	       |   |   +------B: Cursor ON/OFF control bit. B = LOW => Cursor blink is off
		   |   +----------C: Cursor Control Bit. C = LOW => Cursor disappears
		   +--------------D: Display ON/OFF control bit. D = HIGH => Display is on
	   1   1   0   0 => 0xC */
	set_display_data_pins(0x0C);
	display_enable(1);
	_delay_ms(1);
	
	set_display_data_pins(0x00);
	display_enable(1);
	_delay_ms(1);
	set_display_data_pins(0x06);
	display_enable(1);
	_delay_ms(1);
	
	set_display_data_pins(0x00);
	display_enable(1);
	_delay_ms(1);
	
	/* DB7 DB6 DB5 DB4 |
	   0   1   I/D S
	           |   +--- S: LOW => no shift
			   +------- I/D: HIGH => shift to the right (if S == HIGH)
			   0   1   1   0x6*/
	set_display_data_pins(0x06);
	display_enable(1);
	_delay_ms(1);
	
	set_display_data_pins(0x00);
	display_enable(1);
	_delay_ms(1);
	set_display_data_pins(0x01);
	display_enable(1);
	_delay_ms(1);
}

void display_write_char(char s) {
	display_set_rs_pin();
	
	set_display_data_pins(s >> 4);
	display_enable(1);
	
	set_display_data_pins(s);
	display_enable(1);
}

void display_next_line(void) {
	display_clear_rs_pin();
	set_display_data_pins(0xC);
	display_enable(1);
	set_display_data_pins(0x0);
	display_enable(1);
}

void display_write_string(char *s) {
	display_set_rs_pin();
	while(*s!='\0') {
		if(*s == '\n') {
			display_next_line();
		}
		else {
			display_write_char(*s);
		}
		s++;
	}
}

void display_return_home(void) {
	display_clear_rs_pin();
	set_display_data_pins(0x00);
	display_enable(1);
	set_display_data_pins(0x02);
	display_enable(1);
	_delay_ms(5);
}

void display_clear(void) {
	display_clear_rs_pin();
	set_display_data_pins(0x00);
	display_enable(1);
	set_display_data_pins(0x01);
	display_enable(1);
	_delay_ms(5);
}