#include <stdio.h>
#include <stdbool.h>

#include "menu.h"
#include "mock_hal_display.h"
#include "mock_hal_buttons.h"
#include "mock_delay.h"
#include "mock_hal_usart.h"

extern bool _redrawMenu;

void test_displayShouldPrintMainMenu() {
    uint16_t runs = 2;
    displayWriteString_Expect("Show next BU   >\nActions        >");    
    for (uint16_t c = 0; c<runs; c++) {
        USART0_sendString_w_newline_eol_Ignore();
        button0_IgnoreAndReturn(not_pressed);
        button1_IgnoreAndReturn(not_pressed);
        delayMs_Expect(10);
    }
    TEST_ASSERT_TRUE(_redrawMenu);
    menuShow(runs);
}

void test_displayShouldGoIntoSubmenu_if_button0Pressed() {
    USART0_sendString_w_newline_eol_Ignore();
    button0_ExpectAndReturn(pressed);
    button1_ExpectAndReturn(not_pressed);
    displayWriteString_Ignore();
    delayMs_Expect(10);
    menuShow(2);
}