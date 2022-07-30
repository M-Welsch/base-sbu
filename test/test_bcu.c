
#include "bcu.h"
#include "mock_hal.h"

#define DIMENSION_OF(x) (sizeof(x)/sizeof(x[0]))

void test_decodeBU_should_returnSecondsFromArray() {
    typedef struct {char inArray[6]; uint16_t outInt;} testcaseMap_t;
    testcaseMap_t map[] = {
        {"0", 0},
        {"1", 1},
        {"10", 10},
        {"65", 65},
        {"595", 595},
        {"11583", 11583},
        {"65535", 65535},
    };

    for (uint8_t c=0; c<DIMENSION_OF(map); c++)
        TEST_ASSERT_EQUAL_INT(map[c].outInt, _decodeBU(map[c].inArray));
}

void test_decodeMessageAndRunAction_should_decoudeMeaningfullTwoBytePayload() {
    typedef struct {char inArray[33]; uint16_t outInt;} testcaseMap_t;
    testcaseMap_t map[] = {
        {"BU:0", 0},
        {"BU:1", 1},
        {"BU:10", 10},
        {"BU:65", 65},
        {"BU:595", 595},
        {"BU:11583", 11583},
        {"BU:65535", 65535},
    };

    for (uint8_t c=0; c<DIMENSION_OF(map); c++) {
        rtcSetWakeupInSeconds_Expect(map[c].outInt);
        decodeMessageAndRunAction(map[c].inArray);
    }
}