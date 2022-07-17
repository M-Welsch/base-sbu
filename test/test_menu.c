#include <stdio.h>
#include <stdbool.h>

#include "menu.h"
#include "mock_hal_display.h"
#include "mock_hal_buttons.h"

extern bool _redrawMenu;

void test_displayShouldPrintMainMenu() {
    displayWriteString_Expect("Show next BU   >\nActions        >");    
    TEST_ASSERT_TRUE(_redrawMenu);
    menuShow();
    TEST_ASSERT_FALSE(_redrawMenu);

    // run a second time to make sure displayWrite.. isn't called again
    menuShow();
}