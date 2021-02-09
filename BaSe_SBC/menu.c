#include <avr/io.h>
#include <stdio.h>
#include "menu.h"
#include "flags.h"
#include "hmi.h"
#include "powerpath_control.h"

/* Menus */

void (*show_menu)() = show_main_menu;
void (*button0_action)() = NULL;
void (*button1_action)() = NULL;

void show_main_menu() {
	display_clear();
	display_write_string("Show next BU   >\nActions        >");
	button0_action = show_menu_timestamp;
	button1_action = show_menu_actions;
}

void show_menu_actions() {
	show_menu = show_menu_actions;
	display_clear();
	display_write_string("Do Backup Now  >\nWake for config>");
	button0_action = show_menu_sure_about_waking_bcu_and_backup;
	button1_action = show_menu_sure_about_waking_bcu;
}

void show_menu_timestamp() {
	show_menu = show_menu_timestamp;
	
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
	
	display_clear();
	display_write_string(human_readable_timestamp_next_bu);
	display_next_line();
	display_write_string(buffer);
	button0_action = back_to_main_menu;
	button1_action = back_to_main_menu;
}

void show_menu_sure_about_waking_bcu_and_backup() {
	show_menu = show_menu_sure_about_waking_bcu_and_backup;
	display_clear();
	display_write_string("Sure?       No >\n           Yes >");
	button0_action = back_to_main_menu;
	button1_action = wake_bcu_and_do_backup_now;
}

void show_menu_sure_about_waking_bcu() {
	show_menu = show_menu_sure_about_waking_bcu;
	display_clear();
	display_write_string("Sure?      Yes >\n            No >");
	button0_action = wake_bcu;
	button1_action = back_to_main_menu;
}

/* Button Actions */

void back_to_main_menu() {
	show_menu = show_main_menu;
}

void wake_bcu_and_do_backup_now() {
	display_clear();
	display_write_string("Waking BCU and\ndo Backup now ..");
	flag_schedule_backup_now = true;
	next_pwr_state = active;
	transition_to_pwr_state(next_pwr_state);
}

void wake_bcu() {
	display_clear();
	display_write_string("Waking BCU and\ndo Backup now ..");
	next_pwr_state = active;
	transition_to_pwr_state(next_pwr_state);
}