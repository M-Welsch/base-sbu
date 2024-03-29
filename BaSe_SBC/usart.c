/*
 * usart.c
 *
 * Created: 06.06.2020 21:58:36
 *  Author: Max
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include "gpio_interface.h"
#include "hmi.h"
#include "usart.h"
#include "flags.h"
#include "string.h"
#include "powerpath_control.h"
#include <string.h>

#define F_CPU 3333333
#define BAUD_RATE 9600
#define UROUND(x) ((2UL*(x)+1)/2)

void init_uart(void) {
	set_pb2_txd_and_pb3_rxd();
	USART0_set_baud_rate();
	USART0_enable_periperials();
	USART0_enable_rx_complete_interrupt();
}

void USART0_set_baud_rate() {
	USART0.BAUD = UROUND(64UL*F_CPU/16/BAUD_RATE);
}

void USART0_enable_periperials() {
	USART0.CTRLB = USART_RXEN_bm | USART_TXEN_bm;
}

void USART0_enable_rx_complete_interrupt() {
	USART0.CTRLA |= USART_RXCIE_bm;
}

void USART0_sendChar(char c) {
	while((USART0.STATUS & USART_DREIF_bm) == 0)
	;
	USART0.TXDATAL = c;
}

void USART0_sendString_w_newline_eol(char *s) {
	USART0_sendString(s);
	USART0_sendChar('\n');
	USART0_sendChar('\0');
}

void USART0_send_ready() {
	USART0_sendString_w_newline_eol("Ready");
}

void USART0_sendString(char *s) {
	for(size_t i = 0; i < strlen(s); i++) {
		USART0_sendChar(s[i]);
	}
}

uint8_t USART0_read()
{
	
	while (!(USART0_receive_complete()))
	{
		;
	}
	return USART0.RXDATAL;
}

uint8_t USART0_receive_complete() {
	return USART0.STATUS & USART_RXCIF_bm;
}

void USART0_read_string(char *receive_buffer, int maxlen) {
	if (maxlen > 32) {
		maxlen = 32; //increase size of receive buffer in uart.h if you want to have more space!
	}
	int i = 0;
	while(i <= maxlen) {
		i++;
		*receive_buffer = USART0_read();
		if ((*receive_buffer == '\n') || (*receive_buffer == '\0') || (*receive_buffer == '\r')) {
			*receive_buffer = '\0';
			break;
		}
		receive_buffer++;
	}
	if (i > maxlen) {
		*receive_buffer = '\0';
	}
}

void USART0_process_incoming_message() {
	char *message_code, *payload;
	
	strcpy(usart_receive_copy, usart_receive_buffer);
		
    message_code = strtok(usart_receive_buffer, ":");
    payload = strtok(NULL,"\0");
	sprintf(buffer,"MC: %s, PL: %s\n",message_code, payload);
	
	if (strcmp(message_code, "Test") == 0) {
		USART0_sendString_w_newline_eol("ACK:Test");
		USART0_sendString_w_newline_eol("Echo");
		USART0_send_ready();
	}
	
	if (strcmp(message_code, "D1") == 0) {
		USART0_sendString_w_newline_eol("ACK:D1");
		strcpy(display_line1_content, payload);
		USART0_send_ready();
	}
	
	if (strcmp(message_code, "D2") == 0) {
		USART0_sendString_w_newline_eol("ACK:D2");
		strcpy(display_line2_content, payload);
		flag_string_for_display_received = true;
	}
	
	if (strcmp(message_code, "SR") == 0) {
		USART0_sendString_w_newline_eol("ACK:SR");
		flag_pwr_state_change_request = true;
		next_pwr_state = standby;
		USART0_send_ready();
	}
	
	if (strcmp(message_code, "SA") == 0) {
		USART0_sendString_w_newline_eol("ACK:SA");
		flag_abort_shutdown = true;
		USART0_send_ready();
	}
	
	if (strcmp(message_code, "BU") == 0) {
		sprintf(buffer,"ACK:BU:%s\n", payload);
		USART0_sendString_w_newline_eol(buffer);
		seconds_to_next_bu = convert_str_to_long(payload);
		flag_received_seconds_to_next_bu = true;
	}
	
	if (strcmp(message_code, "BR") == 0) {
		USART0_sendString_w_newline_eol("ACK:BR");
		/* received human readable timestamp for next backup */
		strcpy(human_readable_timestamp_next_bu, payload);
		human_readable_timestamp_next_bu[16] = '\n';
		flag_human_readable_timestamp_next_bu_received = true;
	}	
	
	if (strcmp(message_code, "SB") == 0) {
		flag_goto_sleep = true;
	}
	
	if (strcmp(message_code, "CC") == 0) {
		USART0_sendString_w_newline_eol("ACK:CC");
		flag_request_current_measurement = true;
	}
	
	if (strcmp(message_code, "TP") == 0) {
		USART0_sendString_w_newline_eol("ACK:TP");
		flag_request_temperature_measurement = true;
	}
	
	if (strcmp(message_code, "3V") == 0) {
		USART0_sendString_w_newline_eol("ACK:3V");
		flag_request_3v3_measurement = true;
	}
	
	if(strcmp(message_code, "ON") == 0) {
		USART0_sendString_w_newline_eol("ACK:ON");
		if(flag_schedule_backup_now) {
			flag_schedule_backup_now = false;
			USART0_sendString_w_newline_eol("User Backup");
		}
	}
	
	if(strcmp(message_code, "DB") == 0) {
		USART0_sendString_w_newline_eol("ACK:DB");
		dimming_value_display = atoi(payload);
		set_dimming_value_display_bl(dimming_value_display);
		flag_dim_display = true;
		USART0_send_ready();
	}
	
	if(strcmp(message_code, "DL") == 0) {
		USART0_sendString_w_newline_eol("ACK:DL");
		dimming_value_hmi_led = atoi(payload);
		flag_dim_hmi_led = true;
	}
	
	if(strcmp(message_code, "WR") == 0) {
		USART0_sendString_w_newline_eol("ACK:WR");
		USART0_sendString_w_newline_eol(wakeup_reason);
		strcpy(wakeup_reason, "");
		USART0_send_ready();
	}
	
	if(strcmp(message_code, "WD") == 0) {
		USART0_sendString_w_newline_eol("ACK:WD");
		strcpy(wakeup_reason, payload);
		USART0_send_ready();
	}
}

long convert_str_to_long(char *str) {
	char *ptr;
	long ret;

	ret = strtol(str, &ptr, 10);
	return ret;
}

/* Interrupts */

ISR(USART0_RXC_vect) {
	USART0_read_string(usart_receive_buffer, 32);
	flag_usart_string_receive_complete = true;
}