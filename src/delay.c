#ifndef delay_h__
#define delay_h__

#ifdef __AVR_ATtiny816__
#include <avr/delay.h>
#else
#include <stdio.h>
void _delay_ms(double __ms) {
    printf("delaying for %fms\n", __ms);
}
#endif

void delayMs(double ms) {
    _delay_ms(ms);
}

#endif