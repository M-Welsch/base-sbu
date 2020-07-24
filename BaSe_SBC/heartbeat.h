/*
 * heartbeat.h
 *
 * Created: 07.06.2020 21:57:46
 *  Author: Max
 */ 


#ifndef HEARTBEAT_H_
#define HEARTBEAT_H_

void init_heatbeat_monitor(void);
void setup_interrupt_timeout_detection();
void heartbeat_monitor(void);
void reset_bpi_hb_timeout_timer();

int get_sbc_heartbeat_count();
void send_sbc_heartbeat_count_to_bpi();
int increment_or_reset_sbc_heartbeat_count();

#endif /* HEARTBEAT_H_ */