#include "mock_hal.h"

// #include "globals.h"
#include "bcu.h"

void test_decodeMessageAndRunAction_secondsShouldDecode() {
    //g_usartReceiveBuffer[0] = 0x01;
    decodeMessageAndRunAction();
}