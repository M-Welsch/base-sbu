#include <avr/io.h>
#include <avr/delay.h>
#include "hal.h"
#include "statemachine.h"
#include "usart.h"


void mainloop(void) {
  ledOn();
  _delay_ms(100);
  ledOff();
  _delay_ms(100);
  g_currentState = stateStandby;
  USART0_sendString_w_newline_eol("Stuff!");
  ledOn();
  _delay_ms(500);
  g_currentState = stateBcuRunning;
}

int main(void) 
{
  g_currentState = stateBcuRunning;
  halSetup();
  usartInit();

  
  while (1) {
    mainloop();
  }
}