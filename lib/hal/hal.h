#ifndef hal_h__
#define hal_h__

#include <stdint.h>

#ifdef __AVR_ATtiny816__
#include <avr/io.h>
#endif


#define LED_PORT PORTB
#define LED_PIN PIN4_bm

#define USART_PORT PORTB
#define USART_PIN_TX PIN2_bm
#define USART_PIN_RX PIN3_bm

#define dis_rs_port PORTA
#define dis_rs PIN6_bm //PA6

#define DISPLAY_E_PORT PORTA
#define DISPLAY_E_PIN PIN7_bm //PA7

#define DISPLAY_PWM_PORT PORTB
#define dis_pwm PIN0_bm //PB0


/* PC0 - PC3: Display Data Bus */
#define DISPLAY_DATA_PORT PORTC
#define dis_db4 PIN0_bm
#define dis_db5 PIN1_bm
#define dis_db6 PIN2_bm
#define dis_db7 PIN3_bm

#define SPLY_5V_EN_PORT PORTA
#define SPLY_5V_EN_PIN PIN4_bm

#define SPLY_BCU_DIS_PORT PORTB
#define SPLY_BCU_DIS_PIN PIN1_bm

#define BUTTON_PORT PORTA
#define BUTTON_0 PIN2_bm //PA2
#define BUTTON_0_CTRL PIN2CTRL //PA2
#define BUTTON_1 PIN3_bm //PA3
#define BUTTON_1_CTRL PIN3CTRL //PA3


void halInit();

void ledPinHigh(void);
void ledPinLow(void);

void setDisplayPwm(uint16_t dimming_value);
void displayEnable(uint8_t duration_ms);
void displayDataPins(uint8_t data_nibble);
void displaySetRs(void);
void displayClearRs(void);

void disableBpiSplyToHigh();
void disableBpiSplyToLow();

void pin5vEnToHigh();
void pin5vEnToLow();

#endif