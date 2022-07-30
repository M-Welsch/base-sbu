#include <stdint.h>

#include "statemachine.h"
#include "hal.h"

/**
 * @brief basic string to unsigned integer conversion
 * 
 * @param payload 
 * @return uint16_t 
 */
uint16_t _decodeBU(const char* payload) {
    uint16_t seconds = 0;
    uint8_t shifter = 0;
    uint16_t multiplier = 1;
    while (*(payload+shifter) != '\0') {

        /* extract the digit from the char digit by subtracting 0x30. 0x30 is where the '0'..'9' chars start in ascii */
        uint8_t currentDigit = (((uint8_t) *(payload+shifter)) - 0x30);

        /* compose the unsigned integer, if char decoded to single digit */
        seconds *= multiplier;
        if (currentDigit < 10)
            seconds += currentDigit;
        shifter++;

        /* the multiplyer is 1 for the first digit, and 10 from then on */
        multiplier = 10;
    }
    return seconds;
}

void _startsWithB(const char* messageCode, const char* payload) {
    if (*payload == '\0')
        return;
    uint16_t secondsToWakeup = 0;
    switch (*(messageCode+1)) {
        case 'U':  // Seconds to next Wakeup. LSB first, important!!
            secondsToWakeup = _decodeBU(payload);
            rtcSetWakeupInSeconds(secondsToWakeup);
            rtcActivateCompareInterrupt();
            break;
        default:
            break;
    }
}

void _startsWithS(const char* messageCode, const char* payload) {
    switch (*(messageCode+1)) {
        case 'R':
            statemachineGotoShutdownRequested();
            break;
        case 'A':
            statemachineGotoBcuRunning();
            break;
        default:
            break;
    }
}

void _startsWithT(const char* messageCode, const char* payload) {
    if (*(messageCode+1) == 'e')
        USART0SendString("Echo");
}

void decodeMessageAndRunAction(const char *receiveBuffer) {
    const char *messageCode = receiveBuffer;
    const char *payload = receiveBuffer+3;

    switch (*messageCode) {
        case 'B':  
            _startsWithB(messageCode, payload);
            break;

        case 'S':
            _startsWithS(messageCode, payload);
            break;

        case 'T':
            _startsWithT(messageCode, payload);
            break;

        default:
            break;
    }
}