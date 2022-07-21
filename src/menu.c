#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hal_display.h"
#include "hal_buttons.h"
#include "delay.h"
#include "hal_usart.h"
#include "statemachine.h"

bool _redrawMenu = true;


typedef enum {
    main_menu, show_timestamp_menu, show_actions_menu, confirm_wakeup_backup, confirm_wakeup_config
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

void gotoTimestampMenu() {
    USART0_sendString_w_newline_eol("mmb0");
    _currentMenu = show_timestamp_menu;
}

void gotoActionsMenu() {
    USART0_sendString_w_newline_eol("mmb1");
    _currentMenu = show_actions_menu;
}

void showTimestampMenuShow() {
    displayWriteString("Timestamp!");
}

void gotoMainMenu() {
    _currentMenu = main_menu;
}

void showActionsMenuShow() {
    displayWriteString("Do Backup Now  >\nWake for config>");
}

void gotoConfirmBackup() {
    _currentMenu = confirm_wakeup_backup;
}

void gotoConfirmConfig() {
    _currentMenu = confirm_wakeup_config;
}

void showConfirmWakeupBackupMenuShow() {
    displayWriteString("Sure?       No >\n           Yes >");
}

void triggerBcuWakeupForBackup() {
    USART0_sendString_w_newline_eol("BU");
    g_wakeupReason = BACKUP_NOW;
    statemachineGotoBcuRunning();
}

void triggerBcuWakeupForConfig() {
    USART0_sendString_w_newline_eol("Cfg");
    g_wakeupReason = CONFIGURATION;
    statemachineGotoBcuRunning();
}

void showConfirmWakeupConfigMenuShow() {
    displayWriteString("Sure?      Yes >\n            No >"); 
}

menu_struct_t menus[5] = {
    {main_menu, mainMenuShow, gotoTimestampMenu, gotoActionsMenu},
    {show_timestamp_menu, showTimestampMenuShow, gotoMainMenu, gotoMainMenu},
    {show_actions_menu, showActionsMenuShow, gotoConfirmBackup, gotoConfirmConfig},
    {confirm_wakeup_backup, showConfirmWakeupBackupMenuShow, gotoMainMenu, triggerBcuWakeupForBackup},
    {confirm_wakeup_config, showConfirmWakeupConfigMenuShow, triggerBcuWakeupForConfig, gotoMainMenu}
};

void menuShow(uint16_t runs) {
    menus[_currentMenu].show_menu();

    uint16_t _timeoutCounter = 0;
    void (*action)() = NULL;
    while (_timeoutCounter < runs) {
        USART0_sendString_w_newline_eol("l");
        if (button0() == pressed) {
            action = menus[_currentMenu].button0_action;
        }
        if (button1() == pressed) {
            action = menus[_currentMenu].button1_action;
        }
        if (action != NULL) {
            action();
            break;
        }
        delayMs(10);
        _timeoutCounter++;
    }
    if (_timeoutCounter == runs) {
        USART0_sendString_w_newline_eol("sleep");
        statemachineGotoStandby();
        // sleeping here
        statemachineGotoMenu();
    }
}