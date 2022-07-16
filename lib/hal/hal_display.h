#ifndef hal_display_h__
#define hal_display_h__

#include <stdint.h>

void displayDim(uint16_t value);
void displayBufferLine1(const char *line1);
void displayWriteBothLines(const char *line2);
void displayInit(void);
void displayClear(void);
void displayWriteString(const char *s);

#endif