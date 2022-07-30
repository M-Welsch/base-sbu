#ifndef bcu_h__ 
#define bcu_h__ 

#include <stdint.h>

uint16_t _decodeBU(const char* payload);
void decodeMessageAndRunAction(const char *receiveBuffer);

#endif