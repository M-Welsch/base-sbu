#ifndef hal_display_h__
#define hal_display_h__

#include <stdint.h>

void displayDim(uint16_t value);

void displayInit(void);
void displayClear(void);
void displayWriteString(const char *s);

#endif