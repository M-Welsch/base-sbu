#ifndef hal_rtc_h__
#define hal_rtc_h__

void rtcInit(void);
void rtcSetWakeupInSeconds(uint16_t secondsToNextBackup);

#endif