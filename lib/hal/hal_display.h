#ifndef hal_display_h__
#define hal_display_h__

#include <stdint.h>
#include <avr/io.h>

#define dis_rs_port PORTA
#define dis_rs PIN6_bm //PA6

#define DISPLAY_E_PORT PORTA
#define DISPLAY_E_PIN PIN7_bm //PA7

#define DISPLAY_PWM_PORT PORTB
#define dis_pwm PIN0_bm //PB0

/* PC0 - PC3: Display Data Bus */
#define DISPLAY_DATA_PORT PORTC
#define dis_db4 PIN0_bm
#define dis_db5 PIN1_bm
#define dis_db6 PIN2_bm
#define dis_db7 PIN3_bm

void configureDisplayPins(void); 
void setDisplayPwm(uint16_t dimming_value); 
void displayDim(uint16_t value);
void displayBufferLine1(const char *line1);
void displayWriteBothLines(const char *line2);
void displayInit(void);
void displayClear(void);
void displayWriteString(const char *s);

#endif