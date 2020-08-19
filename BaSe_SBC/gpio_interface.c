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
	
	dis_pwm_port.DIRSET = dis_pwm; //for some reason this has to be there!
	
	/* USART initialization has its own gpio direction and state setter function */
	
	disable_digital_input_buffers_for_adc();
}

void disable_digital_input_buffers_for_adc() {
	PORTA.PIN1CTRL &= ~PORT_ISC_gm;
	PORTA.PIN1CTRL |= PORT_ISC_INPUT_DISABLE_gc;
	PORTA.PIN5CTRL &= ~PORT_ISC_gm;
	PORTA.PIN5CTRL |= PORT_ISC_INPUT_DISABLE_gc;
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

/* HMI Buttons */

void set_interrupts_for_buttons(void) {
	button_port.button_0_ctrl |= PORT_ISC_FALLING_gc;
	button_port.button_1_ctrl |= PORT_ISC_FALLING_gc;
}

/* HMI LED */

void led_hmi_on(void) {
	if(current_pwr_state != standby) {
		PORTB.OUTSET = led_hmi;
	}
}

void led_hmi_off(void) {
	PORTB.OUTCLR = led_hmi;
}

void toggle_hmi_led(void) {
	if(current_pwr_state != standby) {
		PORTB.OUTTGL = led_hmi;
	} else {
		led_hmi_off();
	}
}

/* HMI Display */

void dim_display(int dimming_value) {
	//To be implemented properly with pwm
	if ((dimming_value > 0) && (current_pwr_state != standby)) {
		dis_pwm_port.OUTSET = dis_pwm;
	} else {
		dis_pwm_port.OUTCLR = dis_pwm;
	}
}

void toggle_display_backlight(void) {
	dis_pwm_port.OUTTGL = dis_pwm;
}

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
void setup_interrupt_for_hb_edge_detection(void) {
	bpi_heartbeat_port.bpi_heartbeat_ctrl |= PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;
}

/* Interrupt Service Routines regarding pin interrupts */

ISR(PORTA_PORT_vect)
{
	/* Writing something to display here freezes the MCU. Perhaps because the ISR will be called over and over again. */
	/* EDIT: still true?? */
	if(PORTA_INTFLAGS & button_0) {
		flag_button_0_pressed = true;
		PORTA_INTFLAGS &= button_0;
	}
	
	if(PORTA_INTFLAGS & button_1) {
		flag_button_1_pressed = true;
		PORTA_INTFLAGS &= button_1;
	}
}

ISR(PORTB_PORT_vect)
{
	/* Writing something to display here freezes the MCU. Perhaps because the ISR will be called over and over again. */
	if(PORTB_INTFLAGS & bpi_heartbeat) {
		flag_heartbeat = true;
		PORTB_INTFLAGS &= bpi_heartbeat;
		/* reset timer. This has to be done within the ISR to avoid false timeouts due to the slowness of the mainloop */
	}
}

ISR(BADISR_vect)
{
	/* This routine is called if a non defined interrupt-vector is requested. It flashes the HMI_LED real fast */
	for(int i = 0; i < 20; i++)
	{
		led_hmi_on();
		_delay_ms(10);
		led_hmi_off();
		_delay_ms(10);	
	}
}