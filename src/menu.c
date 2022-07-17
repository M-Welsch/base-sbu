#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hal_display.h"
#include "hal_buttons.h"
#include "delay.h"

bool _redrawMenu = true;

typedef enum {
    main_menu, show_timestamp_menu, show_actions_menu
} menus_t;

menus_t _currentMenu = main_menu;

typedef struct {
    menus_t idx;
    void (*show_menu)();
    void (*button0_action)();
    void (*button1_action)();
} menu_struct_t;

void mainMenuShow() {
    displayWriteString("Show next BU   >\nActions        >");
}

void mainMenuButton0() {
    _currentMenu = show_timestamp_menu;
}

void mainMenuButton1() {
    _currentMenu = show_actions_menu;
}

void showTimestampMenuShow() {
    displayWriteString("Timestamp!");
}

void showTimestampMenuButton0() {
    _currentMenu = show_timestamp_menu;
}

void showTimestampMenuButton1() {
    _currentMenu = show_actions_menu;
}
menu_struct_t menus[3] = {
    {main_menu, mainMenuShow, mainMenuButton0, mainMenuButton1},
    {show_timestamp_menu, showTimestampMenuShow, showTimestampMenuButton0, showTimestampMenuButton1},
    {show_actions_menu, NULL, NULL, NULL}
};

void menuShow() {
    button0Pressed = button1Pressed = false;
    menus[_currentMenu].show_menu();

    uint16_t _timeoutCounter = 0;
    while (_timeoutCounter < 1000) {
        if (button0Pressed) {
            menus[_currentMenu].button0_action();
            break;
        }
        if (button1Pressed) {
            menus[_currentMenu].button1_action();
            break;
        }
        delayMs(10);
    }
}