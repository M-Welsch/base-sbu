#include "hal_powerpath.h"
#include "hal_usart.h"

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
    USART0_sendString_w_newline_eol("Activating 5V");
    pin5vEnToHigh();
}

void deactivate5vRail() {
    USART0_sendString_w_newline_eol("Deactivating 5V");
    pin5vEnToLow();
}

void activateBcuSupply() {
    USART0_sendString_w_newline_eol("Activating BCU");
    disableBpiSplyToLow();
}

void deactivateBcuSupply() {
    USART0_sendString_w_newline_eol("Deactivating BCU");
    disableBpiSplyToHigh();
}

void powerpathInit() {
    activate5vRail();
    activateBcuSupply();
	SPLY_BCU_DIS_PORT.DIRSET = SPLY_BCU_DIS_PIN;
	SPLY_5V_EN_PORT.DIRSET = SPLY_5V_EN_PIN;
}
