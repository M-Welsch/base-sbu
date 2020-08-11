/*
 * IncFile1.h
 *
 * Created: 17.05.2020 22:33:02
 *  Author: Max
 */ 
#include <stdbool.h>

#ifndef INCFILE1_H_
#define INCFILE1_H_

bool flag_bpi_ready_for_shutdown;
bool flag_bpi_shutdown_timer_elapsed;
volatile bool flag_button_0_pressed;
bool flag_button_1_pressed;
volatile bool flag_heartbeat;
bool flag_bpi_heartbeat_ok;
bool flag_usart_string_receive_complete;
bool flag_string_for_display_received;

/* function prototypes */
void init_flags(void);

#endif /* INCFILE1_H_ */