#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <avr/io.h>

#include "delay.h"
#include "statemachine.h"
#include "hal.h"

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
	/* there are simpler ways ... on a not 8bit mcu */
	uint32_t seconds = (uint32_t) (RTC_CMP - RTC_CNT) * 32;
	uint32_t days = seconds / 3600;
	days = days / 24;
	for(int i = 0; i < days ; i++) {
	   for(int j = 0; j < 24; j++) {
		    seconds = seconds - 3600;
	    }
	}

	uint32_t hours = seconds / 3600;
	seconds = seconds % 3600;

	uint32_t minutes = seconds / 60;
	seconds = seconds % 60;
    char buffer[17];
	switch (days) {
		case 0:
			sprintf(buffer, "ETA:    %02lu:%02lu:%02lu", hours, minutes, seconds);
			break;
		case 1 ... 9:
			sprintf(buffer, "ETA %lu  %02lu:%02lu:%02lu", days, hours, minutes, seconds);
			break;
		default:
			sprintf(buffer, "ETA %lud %02lu:%02lu:%02lu", days, hours, minutes, seconds);
	}
	
	displayClear();
	displayBufferLine1(g_nextBackupInfo.humanReadableTimestamp);
	displayWriteBothLines(buffer);
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
    _currentMenu = main_menu;
}

void triggerBcuWakeupForConfig() {
    USART0_sendString_w_newline_eol("Cfg");
    g_wakeupReason = CONFIGURATION;
    statemachineGotoBcuRunning();
    _currentMenu = main_menu;
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
    char loopbuffer[4];
    while (_timeoutCounter < runs) {
        sprintf(loopbuffer, "%d", _timeoutCounter);
        USART0_sendString_w_newline_eol(loopbuffer);
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
        if (rtcTimerDue()) {
            rtcDeactivateCompareInterrupt();
            statemachineGotoBcuRunning();
            break;
        }
    }
    if (_timeoutCounter == runs) {
        USART0_sendString_w_newline_eol("sleep");
        statemachineGotoStandby();
        // sleeping here
        statemachineGotoMenu();
    }
}