#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "bcu.h"
#include "globals.h"

void mainloopBcuRunning() {

}

void mainloopShutdownRequested() {

}

int main(void) 
{
  halInit();
  while(1) {
    if(g_usart0ReceiveComplete) {
      decodeMessageAndRunAction(g_usartReceiveBuffer); 
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
	USART0_read_string(g_usartReceiveBuffer, 20);
	USART0SendString(g_usartReceiveBuffer);
}