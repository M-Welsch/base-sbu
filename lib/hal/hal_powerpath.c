#include "hal_powerpath.h"
#include "usart.h"

void powerpathInit() {
	SPLY_BCU_DIS_PORT.DIRSET = SPLY_BCU_DIS_PIN;
	SPLY_5V_EN_PORT.DIRSET = SPLY_5V_EN_PIN;
}

void disableBpiSplyToHigh() {
	SPLY_BCU_DIS_PORT.OUTSET = SPLY_BCU_DIS_PIN;
}

void disableBpiSplyToLow() {
	SPLY_BCU_DIS_PORT.OUTCLR = SPLY_BCU_DIS_PIN;
}

void pin5vEnToHigh() {
	SPLY_5V_EN_PORT.OUTSET = SPLY_5V_EN_PIN;
}

void pin5vEnToLow() {
	SPLY_5V_EN_PORT.OUTCLR = SPLY_5V_EN_PIN;
}

void activate5vRail() {
    USART0_sendString("Activating 5V\n");
    pin5vEnToHigh();
}

void deactivate5vRail() {
    USART0_sendString("Deactivating 5V\n");
    pin5vEnToLow();
}

void activateBcuSupply() {
    USART0_sendString("Activating BCU\n");
    disableBpiSplyToLow();
}

void deactivateBcuSupply() {
    USART0_sendString("Deactivating BCU\n");
    disableBpiSplyToHigh();
}
