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
void display_next_line(void);

/* Dimming */
void dimmer_init();
void set_dimming_value_display_bl(uint8_t dimming_value);
void set_dimming_value_led(uint8_t dimming_value);

/* HMI LED */

void set_hmi_led_dimming_value(uint16_t dimming_value);
void led_hmi_on(void);
void led_hmi_off(void);
void toggle_hmi_led(void);

#endif /* HMI_H_ */