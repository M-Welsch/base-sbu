#include "logging.h"
#include "mock_usart.h"

void test_logging_should_limitMessagestoBuffersize() {
    loggingPutDebug("123456789 123456789 123456789 12");
    USART0_sendString_Ignore();
    USART0_sendString_Ignore();
    USART0_sendString_w_newline_eol_Expect("123456789 123456789 123456789 12");
}