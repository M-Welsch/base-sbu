#include <avr/interrupt.h>
#include <stdbool.h>

#include "hal_buttons.h"

void buttonsInit() {
	BUTTON_PORT.DIRCLR = (BUTTON_0 | BUTTON_1);
	BUTTON_PORT.BUTTON_0_CTRL |= (PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc);
	BUTTON_PORT.BUTTON_1_CTRL |= (PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc);
    button0Pressed = false;
    button1Pressed = false;
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

ISR(PORTA_PORT_vect)
{
	// use the following lines to see the interrupt level (Datasheet p.111)
	//sprintf(buffer,"iflags = %d, status = %d\n", PORTA_INTFLAGS, CPUINT.STATUS);
	//USART0_sendString(buffer);
	if(PORTA_INTFLAGS & BUTTON_0) {
		button0Pressed = true;
		PORTA_INTFLAGS &= BUTTON_0;
	}
	
	if(PORTA_INTFLAGS & BUTTON_1) {
		button1Pressed = true;
		PORTA_INTFLAGS &= BUTTON_1;
	}
}