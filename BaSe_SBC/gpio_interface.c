/*
 * gpio_interface.c
 *
 * Created: 03.06.2020 21:52:48
 *  Author: Max
 */ 

#define F_CPU 20000000

#include <avr/io.h>
#include <util/delay.h>
#include "gpio_interface.h"
#include "flags.h"
#include "usart.h"

#define display_data_mask 0x0f //Display Data Pins are connected to PORTC0-3

void init_pins(void) {
	// Todo: remove all |= from DIRSET, DIRCLR Commands!!
	dis_e_port.OUTCLR = dis_e;
	dis_e_port.DIRSET = dis_e;
	
	dis_rs_port.DIRSET = dis_rs;	
	
	en_5v_port.OUTSET = en_5v;
	en_5v_port.DIRSET = en_5v;
	
	button_port.DIRCLR = (button_0 | button_1);
		
	button_port.PIN2CTRL |= PORT_PULLUPEN_bm; //enable Pullup for Button 0
	button_port.PIN3CTRL |= PORT_PULLUPEN_bm; //enable Pullup for Button 1
	
	PORTB.DIRSET = (dis_pwm | dis_bpi_sply | led_hmi);
	
	bpi_heartbeat_port.DIRCLR = bpi_heartbeat;
	
	dis_data_port.DIRSET = (dis_db4 | dis_db5 | dis_db6 | dis_db7);
	dis_data_port.OUTSET = 0x00;
	
	/* USART initialization has its own gpio direction and state setter function */
}

int button_0_pressed(void) {
	if (~PORTA.IN & button_0) {
		return 1;
		} else {
		return 0;
	}
}

int button_1_pressed(void) {
	if (~PORTA.IN & button_1) {
		return 1;
		} else {
		return 0;
	}
}

/* HMI LED */

void led_hmi_on(void) {
	PORTB.OUTSET = led_hmi;
}

void led_hmi_off(void) {
	PORTB.OUTCLR = led_hmi;
}

void toggle_hmi_led(void) {
	PORTB.OUTTGL = led_hmi;
}

/* HMI Display */

void display_enable(uint8_t duration_ms) {
	dis_e_port.OUTSET = dis_e;
	_delay_us(10);
	dis_e_port.OUTCLR = dis_e;
	_delay_us(10);
}

void set_display_data_pins(uint8_t data_nibble) {
	dis_data_port.OUT &= ~display_data_mask;
	dis_data_port.OUT |= (data_nibble & display_data_mask);
}

void toggle_rs_pin(void) {
	display_set_rs_pin();
	_delay_ms(10);
	display_clear_rs_pin();
	_delay_ms(10);
}

void display_set_rs_pin(void) {
	dis_rs_port.OUTSET = dis_rs;
}

void display_clear_rs_pin(void) {
	dis_rs_port.OUTCLR = dis_rs;
}

/* USART */
void set_pb2_txd_and_pb3_rxd(void) {
	PORTB.OUTSET = PIN2_bm;
	PORTB.DIRSET = PIN2_bm;
}

void disable_usart_tx(void) {
	PORTB.OUTCLR = PIN2_bm;
}

/* Heartbeat */
void setup_heartbeat_interrupt(void) {
	bpi_heartbeat_port.bpi_heartbeat_ctrl |= PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;
}

/* Interrupt Service Routines regarding pin interrupts */
ISR(PORTB_PORT_vect)
{
	/* Writing something to display here freezes the MCU. Perhaps because the ISR will be called over and over again. */
	if(PORTB_INTFLAGS & bpi_heartbeat) {
		led_hmi_on();
		flag_heartbeat = true;
		PORTB_INTFLAGS &= bpi_heartbeat;
	}
}