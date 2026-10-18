#include "unity.h"
#include "logger.h"
#include "hal_timer.h"

void setUp(void)
{
    hal_timer_init();
}

void tearDown(void)
{
}

void test_logger_format_info_message(void)
{
    char buffer[128] = {0};
    
    /* Giả lập hệ thống đã chạy được 15 ticks */
    for(uint8_t i = 0U; i < 15U; ++i) {
        hal_timer_tick();
    }

    logger_format(buffer, sizeof(buffer), LOG_LEVEL_INFO, "System initialized");

    /* Kỳ vọng: Chuỗi đầu ra phải chứa đủ [Tick] [Mức độ] Thông điệp */
    TEST_ASSERT_EQUAL_STRING("[00000015] [INFO] System initialized", buffer);
}

int main(void)
{
    UNITY_BEGIN();
    (void)RUN_TEST(test_logger_format_info_message);
    return UNITY_END();
}