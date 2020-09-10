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
#include "usart.h"

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

/* Dimming */

uint16_t get_default_display_dimming_value_from_eeprom() {
	return 0x0100;
}

void dimmer_init() {
	
	/* disable all TCA0 related interrupts */
	TCA0.SINGLE.INTCTRL &= ~(TCA_SINGLE_CMP2EN_bm | TCA_SINGLE_CMP1EN_bm | TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_OVF_bm);

	/* following steps in datasheet from chapter 20.3.3.4.1 */
	
	/* step 1.0: select waveform generation mode (single-slope pwm) */
	TCA0.SINGLE.CTRLB |= TCA_SINGLE_WGMODE_SINGLESLOPE_gc;
	
	/* step 1.1: setting up clock (prescaler = 1) */
	TCA0.SINGLE.CTRLA |= TCA_SINGLE_CLKSEL_DIV1_gc;
	
	/* step 1.2 setting up period time (maximum for max. resolution) */
	TCA0.SINGLE.PERBUF = 0xFFFF;
	
	/* step 2: TCA is counting tick not events */
	TCA0.SINGLE.EVCTRL &= ~(TCA_SINGLE_CNTEI_bm);
	
	/* step 3.1: map TCA0 WO1 to alternate hw-pin (for HMI LED)
		         WO0 (PB0) is used for the Display Backlight
	             WO1 (PB4) is used for the HMI LED
				      default hw-pin would be PB1
					  HMI led is connected to PB4 */
	PORTMUX.CTRLC |= PORTMUX_TCA01_bm;
	
	/* step 3.2: override port output settings */
	
	TCA0.SINGLE.CTRLB |= TCA_SINGLE_CMP0EN_bm; // Display BL: WO0 (PB0)
	TCA0.SINGLE.CTRLB |= TCA_SINGLE_CMP1EN_bm; // HMI LED: WO1 (PB4)
	
	/* initial dimming values for display and hmi led */
	dimming_value_display = get_default_display_dimming_value_from_eeprom();
	TCA0.SINGLE.CMP0 = dimming_value_display;
	dimming_value_hmi_led = 0xFFFF;
	TCA0.SINGLE.CMP1 = dimming_value_hmi_led;
	
	/* enable TCA */
	TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;
}



void set_dimming_value_display_bl(uint16_t dimming_value) {
	TCA0.SINGLE.CMP0 = dimming_value;
}

void set_dimming_value_led(uint16_t dimming_value) {
	TCA0.SINGLE.CMP1 = dimming_value;
}

/* HMI LED */

void set_hmi_led_dimming_value(uint16_t dimming_value) {
	TCA0.SINGLE.CMP1 = dimming_value;
}

void led_hmi_on(void) {
	set_hmi_led_dimming_value(dimming_value_hmi_led);
}

void led_hmi_off(void) {
	set_hmi_led_dimming_value(0);
}

void toggle_hmi_led(void) {
	if(TCA0.SINGLE.CMP1 == 0x0000) {
		led_hmi_on();
	} else {
		led_hmi_off();
	}
}