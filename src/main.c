#include <avr/io.h>
#include <avr/delay.h>
#include "hal.h"
#include "hal_rtc.h"
#include "hal_led.h"
#include "hal_adc.h"
#include "hal_display.h"
#include "statemachine.h"
#include "usart.h"
#include "logging.h"
#include "hal_buttons.h"

char buffer2[40];

void mainloop(void) {
  loggingPutDebug("Mainloop");
  sprintf(buffer2, "3v3: %i\nIin: %i", adcResult3v3(), adcResultInCur());
  displayClear();
  displayWriteString(buffer2);
  _delay_ms(100);
}

int main(void) 
{
  statemachineInit();
  halInit();
  usartInit();
  statemachineGoto5vActive();
  statemachineGotoBcuRunning();

  while (1) {
    mainloop();
  }
}