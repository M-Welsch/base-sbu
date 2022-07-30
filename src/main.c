#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "bcu.h"
#include "globals.h"
#include "statemachine.h"

char g_usartReceiveBuffer[20];
volatile bool g_usart0ReceiveComplete;

extern states_t g_currentState;

void mainloopBcuRunning() {
  USART0SendString("BcuR");
}

void mainloopShutdownRequested() {

}

int main(void) 
{
  halInit();
  statemachineGotoBcuRunning();
  while(1) {
    if(g_usart0ReceiveComplete) {
      cli();
      g_usart0ReceiveComplete = false;
      decodeMessageAndRunAction(g_usartReceiveBuffer); 
      sei();
    }

    switch (g_currentState) {
      case stateBcuRunning:
        mainloopBcuRunning();
        break;
      case stateShutdownRequested:
        mainloopShutdownRequested();
        break;
      default:
        break;
    }
  }
}



ISR(USART0_RXC_vect) {
  cli();
  g_usart0ReceiveComplete = true;
	USART0_read_string(g_usartReceiveBuffer, 20);
	USART0SendString(g_usartReceiveBuffer);
  sei();
}

ISR(RTC_CNT_vect) {
	RTC.INTFLAGS |= RTC_CMP_bm;
	USART0SendString("CMP");
	rtcDeactivateCompareInterrupt();
}