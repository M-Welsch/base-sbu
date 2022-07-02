#include <avr/io.h>
#include <avr/delay.h>
#include "hal.h"
#include "hal_led.h"
#include "hal_display.h"
#include "statemachine.h"
#include "usart.h"


void mainloop(void) {
  ledOn();
  _delay_ms(100);
  ledOff();
  _delay_ms(100);
  USART0_sendString_w_newline_eol("Stuff!");
}

int main(void) 
{
  statemachineInit();
  halInit();
  usartInit();
  _delay_ms(100);
  displayInit();
  displayWriteString("Test");
  
  while (1) {
    mainloop();
  }
}