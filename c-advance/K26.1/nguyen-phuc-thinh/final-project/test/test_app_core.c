#include "unity.h"
#include "app_core.h"
#include "hal_timer.h"
#include "common_types.h"

void setUp(void) { 
    hal_timer_init(); 
}
void tearDown(void) {}

void test_app_core_init_success(void) {
    app_config_t cfg = { 
        .temp_threshold_high = 400,
        .humi_threshold_high = 800
    };
    TEST_ASSERT_EQUAL(STATUS_OK, app_core_init(&cfg));
}

int main(void) {
    UNITY_BEGIN();
    (void)RUN_TEST(test_app_core_init_success);
    return UNITY_END();
}