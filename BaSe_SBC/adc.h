/*
 * adc.h
 *
 * Created: 17.08.2020 22:13:43
 *  Author: Max
 */ 


#ifndef ADC_H_
#define ADC_H_

void adc_init();

/*
 * adc.c
 *
 * Created: 17.08.2020 22:13:32
 *  Author: Max
 */ 

#include "adc.h"
#include "flags.h"

void adc_init_10bit_resolution();
void adc_enable();
void adc_init();

uint16_t adc_measure_3v3();
uint16_t adc_measure_input_current();

uint16_t adc_measure_temperature();

void adc_do_conversion();

void adc_start_conversion();
void adc_wait_for_convesion_to_complete();
bool vcc3v3_present();


#endif /* ADC_H_ */