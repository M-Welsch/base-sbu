#ifndef hal_buttons_h__
#define hal_buttons_h__

typedef enum {
    pressed,
    not_pressed
} buttonState_t;

buttonState_t button0(void);
buttonState_t button1(void);

#endif