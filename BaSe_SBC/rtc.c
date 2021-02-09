#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdbool.h>

#include "usart.h"
#include "gpio_interface.h"
#include "flags.h"
#include "powerpath_control.h"

void rtc_reset_counter()
{
	RTC.CNTL = 0;
	RTC.CNTH = 0;
}

void rtc_wait_for_all_status_bits_zero()
{
	while (RTC.STATUS > 0) {
		;
	}
}

void rtc_set_prescaler_and_enable()
{
	/* active in standby mode, prescaler = 32768, periperial enabled */
	//Todo: uncomment 32 seconds configuration !!
	RTC.CTRLA = RTC_RUNSTDBY_bm | RTC_PRESCALER_DIV32768_gc | RTC_RTCEN_bm; // <- one counter increment represents 32 seconds
	//RTC.CTRLA = RTC_RUNSTDBY_bm | RTC_PRESCALER_DIV1024_gc | RTC_RTCEN_bm; // <- 1 second
}

bool rtc_enabled() {
	return RTC.CTRLA & RTC_RTCEN_bm;
}

void rtc_set_compare_interrupt()
{
	RTC.INTCTRL = RTC_CMP_bm;
}

void rtc_use_internal_1k024_oscillator()
{
	/* actually its the 32768khz oscillator, divided by 32 => fclk = 1,024kHz */
	RTC.CLKSEL = RTC_CLKSEL_INT1K_gc;
}

void rtc_setup(void) {
	flag_wakeup_by_rtc = false;
	rtc_wait_for_all_status_bits_zero();
	rtc_set_prescaler_and_enable();
	rtc_set_compare_interrupt();
	rtc_use_internal_1k024_oscillator();
}

/* Important Stuff */

void rtc_write_seconds_to_cmp() {
	long register_value_next_bu = seconds_to_next_bu / 32;
	sprintf(buffer, "Writing %ld to CMP Register\n", register_value_next_bu);
	USART0_sendString_w_newline_eol(buffer);
	rtc_reset_counter();
	if (register_value_next_bu == 0) {
		register_value_next_bu = 1;
	}
	RTC.CMP = register_value_next_bu;
}

/* Interrupts */

ISR(RTC_CNT_vect) {
	RTC.INTFLAGS |= RTC_CMP_bm;
	USART0_sendString("CMP\n");
	flag_wakeup_by_rtc = true;
	next_pwr_state = display_on;
	transition_to_pwr_state(next_pwr_state);
}
