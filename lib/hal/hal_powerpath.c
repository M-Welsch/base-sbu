#include "hal_powerpath.h"
#include "hal.h"

void activate5vRail() {
    pin5vEnToHigh();
}

void deactivate5vRail() {
    pin5vEnToLow();
}

void activateBcuSupply() {
    disableBpiSplyToLow();
}

void deactivateBcuSupply() {
    disableBpiSplyToHigh();
}
