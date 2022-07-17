#include <stdbool.h>

#include "hal_buttons.h"

void buttonsInit() {
	BUTTON_PORT.DIRCLR = (BUTTON_0 | BUTTON_1);
	BUTTON_PORT.BUTTON_0_CTRL |= (PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc);
	BUTTON_PORT.BUTTON_1_CTRL |= (PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc);
}

bool button0PinHigh(void) {
	return (BUTTON_PORT.IN & BUTTON_0);
}

bool button1PinHigh(void) {
	return (BUTTON_PORT.IN & BUTTON_1);
}

buttonState_t button0(void) {
    buttonState_t retval = not_pressed;
    if (!button0PinHigh())
        retval = pressed;
    return retval;
}

buttonState_t button1(void) {
    buttonState_t retval = not_pressed;
    if (!button1PinHigh())
        retval = pressed;
    return retval;
}
