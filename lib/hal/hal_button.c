#include "hal_buttons.h"
#include "hal.h"

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
