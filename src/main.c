#include <avr/io.h>
#include <avr/delay.h>
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
      decodeMessageAndRunAction(); 
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