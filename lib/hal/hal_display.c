#include <avr/delay.h>
#include <string.h>
#include <stdio.h>

#include "hal_display.h"
#include "flags.h"

char _displayLine1Buffer[17];
uint16_t _displayDimmingValue = 0;

void setDisplayPwm(uint16_t dimming_value) {
    _displayDimmingValue = dimming_value;
    TCA0.SINGLE.CMP0 = _displayDimmingValue;
}


void configureDisplayPins(void) {
    DISPLAY_PWM_PORT.DIRSET = dis_pwm;
    DISPLAY_E_PORT.DIRSET = DISPLAY_E_PIN;
    dis_rs_port.DIRSET = dis_rs;
    DISPLAY_DATA_PORT.DIRSET = (dis_db4 | dis_db5 | dis_db6 | dis_db7);
}

void displayEnable() {
	DISPLAY_E_PORT.OUTSET = DISPLAY_E_PIN;
	_delay_us(10);
	DISPLAY_E_PORT.OUTCLR = DISPLAY_E_PIN;
	_delay_us(10);
}

#define DISPLAY_DATA_MASK 0x0f //Display Data Pins are connected to PORTC0-3

void displayDataPins(uint8_t data_nibble) {
	DISPLAY_DATA_PORT.OUT &= ~DISPLAY_DATA_MASK;
	DISPLAY_DATA_PORT.OUT |= (data_nibble & DISPLAY_DATA_MASK);
}

void displaySetRs(void) {
	dis_rs_port.OUTSET = dis_rs;
}

void displayClearRs(void) {
	dis_rs_port.OUTCLR = dis_rs;
}


void displayPwmLow(void) {
    TCA0.SINGLE.CMP0 = 0;
}

void displayPinsAllLow(void) {
	displayClearRs();
	displayDataPins(0x00);
	displayPwmLow();
}

void displayPwmHigh(void) {
    setDisplayPwm(_displayDimmingValue);
}

void displayInit(void) {
	/* refering to datasheet SPLC780D and (more importantly) lcd.py of "old" BaSe implementation*/
	_delay_ms(100);
	displayClearRs();
	
	displayDataPins(0x03);
	displayEnable(1);
	_delay_ms(1);
	displayDataPins(0x03);
	displayEnable(1);
	_delay_ms(1);
	
	displayDataPins(0x03);
	displayEnable(1);
	_delay_ms(1);
	displayDataPins(0x02);
	displayEnable(1);
	_delay_ms(1);
	
	displayDataPins(0x02);
	displayEnable(1);
	_delay_ms(1);
	/* the following instruction ...
	   DB7 DB6 DB5 DB4 |
	   N   F   X   X   | N = HIGH => 2 lined display. F = don't care = 0
	   1   0   0   0 => 0x8 */
	displayDataPins(0x08);
	displayEnable(1);
	_delay_ms(1);
	
	displayDataPins(0x00);
	displayEnable(1);
	_delay_ms(1);
	/* The following instruction ...
	   DB7 DB6 DB5 DB4 |
	   1   D   C   B   |
	       |   |   +------B: Cursor ON/OFF control bit. B = LOW => Cursor blink is off
		   |   +----------C: Cursor Control Bit. C = LOW => Cursor disappears
		   +--------------D: Display ON/OFF control bit. D = HIGH => Display is on
	   1   1   0   0 => 0xC */
	displayDataPins(0x0C);
	displayEnable(1);
	_delay_ms(1);
	
	displayDataPins(0x00);
	displayEnable(1);
	_delay_ms(1);
	displayDataPins(0x06);
	displayEnable(1);
	_delay_ms(1);
	
	displayDataPins(0x00);
	displayEnable(1);
	_delay_ms(1);
	
	/* DB7 DB6 DB5 DB4 |
	   0   1   I/D S
	           |   +--- S: LOW => no shift
			   +------- I/D: HIGH => shift to the right (if S == HIGH)
			   0   1   1   0x6*/
	displayDataPins(0x06);
	displayEnable(1);
	_delay_ms(1);
	
	displayDataPins(0x00);
	displayEnable(1);
	_delay_ms(1);
	displayDataPins(0x01);
	displayEnable(1);
	_delay_ms(1);
}

void displayWriteChar(char s) {
	displaySetRs();
	
	displayDataPins(s >> 4);
	displayEnable(1);
	
	displayDataPins(s);
	displayEnable(1);
}

void displayNextLine(void) {
	displayClearRs();
	displayDataPins(0xC);
	displayEnable(1);
	displayDataPins(0x0);
	displayEnable(1);
}

void displayDim(uint16_t value) {
    if (g_currentState != stateStandby)
        setDisplayPwm(value);
}

void displayBufferLine1(const char *line1) {
	strcpy(_displayLine1Buffer, line1);
}

void displayWriteBothLines(const char *line2) {
	char buffer[33];
	sprintf(buffer, "%s\n%s", _displayLine1Buffer, line2);
	displayClear();
	displayWriteString(buffer);
}

void displayWriteString(const char *s) {
	displayClear();
    if (g_currentState != stateStandby) {
        displaySetRs();
        while(*s!='\0') {
            if(*s == '\n') {
                displayNextLine();
            }
            else {
                displayWriteChar(*s);
            }
            s++;
        }
    }
}

void display_return_home(void) {
	displayClearRs();
	displayDataPins(0x00);
	displayEnable(1);
	displayDataPins(0x02);
	displayEnable(1);
	_delay_ms(1);
}

void displayClear(void) {
	displayClearRs();
	displayDataPins(0x00);
	displayEnable(1);
	displayDataPins(0x01);
	displayEnable(1);
	_delay_ms(1);
}