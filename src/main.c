#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "hal.h"
#include "hal_rtc.h"
#include "hal_led.h"
#include "hal_adc.h"
#include "hal_display.h"
#include "statemachine.h"
#include "usart.h"
#include "logging.h"
#include "hal_buttons.h"
#include "mainloops.h"
#include "flags.h"
#include "hal_usart.h"


int main(void) 
{
  statemachineInit();
  halInit();
  usartInit();
  flagsInit();  // caused hardfault

  statemachineGotoBcuRunning();

  char _buffer[128];
  while (1) {
    sprintf(_buffer, "While Loop: %s", stringifyCurrentState());
    loggingPutDebug(_buffer);
    switch (g_currentState)
    {
    case stateBcuRunning:
      mainloopBcuRunning();
      break;
    case stateMenu:
      mainloopMenu();
      break;
    case stateShutdownRequested:
      mainloopShutdownRequested();
      break;
    default:
      loggingPutCritical("Shouldn't ever get here!");
      break;
    }
    _delay_ms(100);
  }
}