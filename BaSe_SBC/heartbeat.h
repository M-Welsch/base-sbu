/*
 * heartbeat.h
 *
 * Created: 07.06.2020 21:57:46
 *  Author: Max
 */ 


#ifndef HEARTBEAT_H_
#define HEARTBEAT_H_

void init_heatbeat_monitor(void);
void heartbeat_monitor(void);

int get_heartbeat_count();
void send_heartbeat_count_to_bpi();
int increment_or_reset_heartbeat_count();

#endif /* HEARTBEAT_H_ */