/*
 * adc.c
 *
 * Created: 17.08.2020 22:13:32
 *  Author: Max
 */ 
#include <avr/io.h>
#include <stdbool.h>
#include "adc.h"
#include "flags.h"

void adc_init_10bit_resolution()
{
	ADC0.CTRLA |= ADC_RESSEL_10BIT_gc;
}

void adc_enable()
{
	ADC0.CTRLA |= ADC_ENABLE_bm;
}

void adc_4V34_reference()
{
	VREF.CTRLA = VREF_ADC0REFSEL_4V34_gc;
}

void adc_init() {
	/* Digital Input buffers are being disabled during pin init */
	adc_init_10bit_resolution();
	adc_4V34_reference();
	adc_enable();
}

uint16_t adc_measure_3v3() {
	ADC0.MUXPOS = ADC_MUXPOS_AIN5_gc;
	adc_do_conversion();
	return ADC0.RES;
}

uint16_t adc_measure_input_current() {
	ADC0.MUXPOS = ADC_MUXPOS_AIN1_gc;
	adc_do_conversion();
	return ADC0.RES;
}

uint16_t adc_measure_temperature() {
	// Fixme: measurement like datasheet s.437, chapter: 30.3.2.6
	ADC0.MUXPOS = ADC_MUXPOS_TEMPSENSE_gc;
	adc_do_conversion();
	return ADC0.RES;
}

void adc_do_conversion()
{
	adc_start_conversion();
	adc_wait_for_convesion_to_complete();
	
	/* Clear the interrupt flag by writing 1: */
	ADC0.INTFLAGS = ADC_RESRDY_bm;
}

void adc_start_conversion() {
	ADC0.COMMAND = ADC_STCONV_bm;
}

void adc_wait_for_convesion_to_complete()
{
	while ( !(ADC0.INTFLAGS & ADC_RESRDY_bm) )
	{
		;
	}
}

bool vcc3v3_present() {
	if (adc_measure_3v3() > 900) {
		return true;
	}
	else {
		return false;
	}
}