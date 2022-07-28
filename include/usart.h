#ifndef usart_h__ 
#define usart_h__

#include "flags.h"

#define DIMENSION_OF(a) (sizeof(a) / sizeof(a[0]) )

baseSbuError_t usartDecodeIncomingMessage();

#endif