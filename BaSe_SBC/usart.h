/*
 * usart.h
 *
 * Created: 06.06.2020 21:58:51
 *  Author: Max
 */ 


#ifndef USART_H_
#define USART_H_

char buffer[48];
char usart_receive_buffer[32];
char temp_receive_buffer;
char display_line1_content_from_bcu[17];
char display_line2_content_from_bcu[17];
char human_readable_timestamp_next_bu[33];


void init_uart(void);
void USART0_set_baud_rate();
void USART0_enable_periperials();
void USART0_enable_rx_complete_interrupt();

void USART0_sendChar(char c);
void USART0_sendString(char *s);
void USART0_sendString_w_eol(char *s);

uint8_t USART0_read();
void USART0_read_string(char *receive_buffer, int maxlen);
uint8_t USART0_receive_complete();
void USART0_process_incoming_message();

long convert_str_to_long(char *str);

void append_current_byte_to_receive_buffer(uint8_t byte_of_data);

#endif /* USART_H_ */