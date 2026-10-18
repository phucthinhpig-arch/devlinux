#include "unity.h"
#include "sensor_mgr.h"
#include "hal_timer.h"

void setUp(void) {
    hal_timer_init();
}

void tearDown(void) {
}

void test_sensor_mgr_init_and_read(void) {
    sensor_data_t data = {0};
    
    /* Chạy 1 tick để mô phỏng thời gian hệ thống trôi qua */
    hal_timer_tick();
    
    TEST_ASSERT_EQUAL(STATUS_OK, sensor_mgr_init());
    TEST_ASSERT_EQUAL(STATUS_OK, sensor_mgr_read_all(&data));
    
    /* Kiểm tra timestamp đã được cập nhật từ HAL Timer chưa */
    TEST_ASSERT_EQUAL(1U, data.timestamp);
}

int main(void) {
    UNITY_BEGIN();
    (void)RUN_TEST(test_sensor_mgr_init_and_read);
    return UNITY_END();
}