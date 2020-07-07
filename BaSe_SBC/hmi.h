/*
 * hmi.h
 *
 * Created: 01.06.2020 22:37:59
 *  Author: Max
 */ 


#ifndef HMI_H_
#define HMI_H_

void led_hmi_on(void);
void led_hmi_off(void);

void display_enable(uint8_t duration_ms);
void sweep_display_pins(void);

void init_display(void);
void display_clear(void);
void display_return_home(void);
void display_write_char(char s);
void display_write_string(char *s);

#endif /* HMI_H_ */