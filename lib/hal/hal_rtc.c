#include <stdint.h>
#include "hal_rtc.h"
#include "hal.h"

void rtcSetWakeupInSeconds(uint16_t secondsToNextBackup) {
	uint16_t register_value_next_bu = secondsToNextBackup / 32;
    rtcSetCmpRegister(register_value_next_bu);
}
