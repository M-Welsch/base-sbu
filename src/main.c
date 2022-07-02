#include <avr/io.h>
#include <avr/delay.h>
#include "hal.h"
#include "hal_led.h"
#include "hal_display.h"
#include "statemachine.h"
#include "usart.h"
#include "logging.h"


void mainloop(void) {
  loggingPutDebug("Mainloop");
  ledOn();
  _delay_ms(100);
  ledOff();
  _delay_ms(100);
}

int main(void) 
{
  statemachineInit();
  halInit();
  usartInit();
  powerpathInit();
  _delay_ms(100);
  displayInit();
  displayWriteString("Test");
  
  while (1) {
    mainloop();
  }
}