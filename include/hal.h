#ifndef hal_h__
#define hal_h__

#ifdef __AVR_ATtiny816__
#include <avr/io.h>
#endif

#include <stdbool.h>
#include "flags.h"

/* USART */
void USART0_send_ready();
void USART0_sendString_w_newline_eol(const char *s);
void USART0_process_incoming_message();

/* ADC */
uint16_t adcResult3v3();
uint16_t adcResultInCur();
uint16_t adcTemperature();
bool adc3v3present();

/* Buttons */
typedef enum {
    pressed,
    not_pressed
} buttonState_t;

buttonState_t button0(void);
buttonState_t button1(void);
volatile bool button0Pressed;
volatile bool button1Pressed;

/* Display */
void displayPinsAllLow(void);
void displayPwmHigh(void);
void _displayConfigurePins(void); 
void setDisplayPwm(uint16_t dimming_value); 
void displayDim(uint16_t value);
void displayBufferLine1(const char *line1);
void displayWriteBothLines(const char *line2);
void displayInit(void);
void displayClear(void);
void displayWriteString(const char *s);

/* LED */
void setLedPwm(uint16_t dimming_value);
baseSbuError_t ledDim(uint16_t dimmingValue);
baseSbuError_t ledOn(void);
baseSbuError_t ledOff(void);

/* Powerpath */
void activate5vRail();
void deactivate5vRail();
void activateBcuSupply();
void deactivateBcuSupply();

/* RTC */
void rtcSetWakeupInSeconds(uint16_t secondsToNextBackup);
void rtcGetSecondsToNextBuAndCount(char *buffer);
void rtcActivateCompareInterrupt();
void rtcDeactivateCompareInterrupt();
bool rtcTimerDue();

void halInit();

#endif