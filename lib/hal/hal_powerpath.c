#include "hal_powerpath.h"
#include "hal.h"
#include "logging.h"

void powerpathInit() {
    activate5vRail();
    activateBcuSupply();
}

void activate5vRail() {
    loggingPutInfo("Activating 5V rail");
    pin5vEnToHigh();
}

void deactivate5vRail() {
    loggingPutInfo("Deactivating 5V rail");
    pin5vEnToLow();
}

void activateBcuSupply() {
    loggingPutInfo("Activating BCU supply");
    disableBpiSplyToLow();
}

void deactivateBcuSupply() {
    loggingPutInfo("Deactivating BCU supply");
    disableBpiSplyToHigh();
}
