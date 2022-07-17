#ifndef hal_buttons_h__
#define hal_buttons_h__

#include <avr/io.h>

#define BUTTON_PORT PORTA
#define BUTTON_0 PIN2_bm //PA2
#define BUTTON_0_CTRL PIN2CTRL //PA2
#define BUTTON_1 PIN3_bm //PA3
#define BUTTON_1_CTRL PIN3CTRL //PA3


typedef enum {
    pressed,
    not_pressed
} buttonState_t;

void buttonsInit(); 
buttonState_t button0(void);
buttonState_t button1(void);

#endif