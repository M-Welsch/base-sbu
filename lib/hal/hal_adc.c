#include <avr/io.h>

#include "hal_adc.h"

void _disableDigitalInputBuffersForAdc() {
	PORTA.PIN1CTRL &= ~PORT_ISC_gm;
	PORTA.PIN1CTRL |= PORT_ISC_INPUT_DISABLE_gc;
	PORTA.PIN5CTRL &= ~PORT_ISC_gm;
	PORTA.PIN5CTRL |= PORT_ISC_INPUT_DISABLE_gc;
}

void _adc_start_conversion() {
	ADC0.COMMAND = ADC_STCONV_bm;
}

void _adc_wait_for_convesion_to_complete()
{
	while ( !(ADC0.INTFLAGS & ADC_RESRDY_bm) )
	{
		;
	}
}

void _adc_init_10bit_resolution()
{
	ADC0.CTRLA |= ADC_RESSEL_10BIT_gc;
}

void _adc_enable()
{
	ADC0.CTRLA |= ADC_ENABLE_bm;
}

void _adc_4V34_reference()
{
	VREF.CTRLA = VREF_ADC0REFSEL_4V34_gc;
}

void _adc_do_conversion()
{
	_adc_start_conversion();
	_adc_wait_for_convesion_to_complete();
	
	/* Clear the interrupt flag by writing 1: */
	ADC0.INTFLAGS = ADC_RESRDY_bm;
}

void _adcConfigForTemperatureMeasurement() {
	// to be implemented correctly, see datasheet s.437, chapter: 30.3.2.6
	VREF.CTRLA = VREF_ADC0REFSEL_1V1_gc;
	ADC0.CTRLC &= ~(0x0);  // write REFSEL to 0x0
	ADC0.MUXPOS = ADC_MUXPOS_TEMPSENSE_gc;  // double check
	ADC0.CTRLD = ADC_SAMPLEN0_bm; // corect bit for INITDLY > 32µs x fclk_adc
	ADC0.CTRLC = ADC_SAMPCAP_bm; // correct val for 5pF
	int8_t sigrow_offset = SIGROW.TEMPSENSE1; // Read signed value from signature row
	uint8_t sigrow_gain = SIGROW.TEMPSENSE0;
	// Read unsigned value from signature row
	uint16_t adc_reading = ADC0.RES;
	// ADC conversion result with 1.1 V internal reference
	uint32_t temp = adc_reading - sigrow_offset;
	temp *= sigrow_gain; // Result might overflow 16 bit variable (10bit+8bit)
	temp += 0x80;
	// Add 1/2 to get correct rounding on division below
	temp >>= 8;
	// Divide result to get Kelvin
	uint16_t temperature_in_K = temp;
}

void adcInit() {
	/* Digital Input buffers are being disabled during pin init */
	_disableDigitalInputBuffersForAdc();
	_adc_init_10bit_resolution();
	_adc_4V34_reference();
	_adc_enable();
}


uint16_t adcResult3v3() {
	ADC0.MUXPOS = ADC_MUXPOS_AIN5_gc;
	_adc_do_conversion();
	return ADC0.RES;
}

uint16_t adcResultInCur() {
	ADC0.MUXPOS = ADC_MUXPOS_AIN1_gc;
	_adc_do_conversion();
	return ADC0.RES;
}

uint16_t adcTemperature() {
    //_adcConfigForTemperatureMeasurement();  // Todo: implement
	ADC0.MUXPOS = ADC_MUXPOS_TEMPSENSE_gc;
	_adc_do_conversion();
	return ADC0.RES;
}

bool adc3v3present() {
	if (adcResult3v3() > 900) {
		return true;
	}
	else {
		return false;
	}
}
