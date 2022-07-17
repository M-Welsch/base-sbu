#ifndef hal_buttons_h__
#define hal_buttons_h__

#include <stdbool.h>

#ifdef __AVR_ATtiny816__
#include <avr/io.h>

#define BUTTON_PORT PORTA
#define BUTTON_0 PIN2_bm //PA2
#define BUTTON_0_CTRL PIN2CTRL //PA2
#define BUTTON_1 PIN3_bm //PA3
#define BUTTON_1_CTRL PIN3CTRL //PA3

#else
#include <stdint.h>
uint8_t PORTA, PIN2_bm, PIN2CTRL, PIN3CTRL;
#define BUTTON_PORT PORTA
#define BUTTON_0 PIN2_bm //PA2
#define BUTTON_0_CTRL PIN2CTRL //PA2
#define BUTTON_1 PIN3_bm //PA3
#define BUTTON_1_CTRL PIN3CTRL //PA3
#endif

typedef enum {
    pressed,
    not_pressed
} buttonState_t;

void buttonsInit(); 
buttonState_t button0(void);
buttonState_t button1(void);
volatile bool button0Pressed;
volatile bool button1Pressed;
#endif