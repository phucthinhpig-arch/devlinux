#include "unity.h"
#include "telemetry.h"

void setUp(void) {}
void tearDown(void) {}

void test_buffer_too_small_rejected(void) {
    sensor_data_t data = { .temperature = 253, .humidity = 601, .timestamp = 0 };
    uint8_t buf[2];
    uint32_t written = 0;
    
    TEST_ASSERT_EQUAL(STATUS_ERR_FULL, telemetry_serialize(&data, buf, sizeof(buf), &written));
}

int main(void) {
    UNITY_BEGIN();
    (void)RUN_TEST(test_buffer_too_small_rejected);
    return UNITY_END();
}