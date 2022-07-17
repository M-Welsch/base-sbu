#ifndef hal_powerpath_h__
#define hal_powerpath_h__

#ifdef __AVR_ATtiny816__
#include <avr/io.h>
#endif

#define SPLY_5V_EN_PORT PORTA
#define SPLY_5V_EN_PIN PIN4_bm

#define SPLY_BCU_DIS_PORT PORTB
#define SPLY_BCU_DIS_PIN PIN1_bm

void powerpathInit();
void activate5vRail();
void deactivate5vRail();
void activateBcuSupply();
void deactivateBcuSupply();

#endif