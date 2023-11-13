#include <unity.h>

#include "teensystep4.h"

void test_pos_initialized() {
    TS4::Stepper stepper(/*stepPin*/ 0, /*dirPin*/ 1);
    
    TEST_ASSERT_EQUAL_INT(0, stepper.getPosition());
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_pos_initialized);
    return UNITY_END();
}
