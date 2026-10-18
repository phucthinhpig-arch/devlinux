#include "unity.h"
#include "hal_sim.h"
#include "hal_timer.h"

/**
 * @brief Chạy trước mỗi bài test để thiết lập môi trường sạch.
 */
void setUp(void)
{
    hal_sim_init(42U);
    hal_timer_init();
}

/**
 * @brief Chạy sau mỗi bài test để dọn dẹp (nếu cần).
 */
void tearDown(void)
{
}

void test_hal_sim_init_sets_registers_to_zero(void)
{
    hal_sim_write_register(HAL_REG_TEMP_RAW, 0x1234U);
    hal_sim_init(42U); /* Gọi lại để kiểm tra logic reset */

    TEST_ASSERT_EQUAL_HEX16(0U, hal_sim_read_register(HAL_REG_TEMP_RAW));
    TEST_ASSERT_EQUAL_HEX16(0U, hal_sim_read_register(HAL_REG_HUMI_RAW));
    TEST_ASSERT_EQUAL_HEX16(0U, hal_sim_read_register(HAL_REG_STATUS));
}

void test_hal_sim_read_write_register(void)
{
    hal_sim_write_register(HAL_REG_TEMP_RAW, 0x1234U);
    hal_sim_write_register(HAL_REG_HUMI_RAW, 0xABCDU);

    TEST_ASSERT_EQUAL_HEX16(0x1234U, hal_sim_read_register(HAL_REG_TEMP_RAW));
    TEST_ASSERT_EQUAL_HEX16(0xABCDU, hal_sim_read_register(HAL_REG_HUMI_RAW));
}

void test_hal_sim_update_changes_values(void)
{
    /* Môi trường đã được setUp() reset về 0U */
    hal_sim_update();
    
    /* Kiểm tra có dữ liệu ngẫu nhiên được ghi vào thanh ghi */
    TEST_ASSERT_NOT_EQUAL(0U, hal_sim_read_register(HAL_REG_TEMP_RAW));
}

void test_hal_timer_init_resets(void)
{
    hal_timer_tick();
    hal_timer_tick();
    hal_timer_init();

    TEST_ASSERT_EQUAL_UINT32(0U, hal_timer_get_tick());
}

void test_hal_timer_tick_increments(void)
{
    /* Đã được reset ở setUp(), tick = 0 */
    TEST_ASSERT_EQUAL_UINT32(0U, hal_timer_get_tick());
    
    hal_timer_tick();
    TEST_ASSERT_EQUAL_UINT32(1U, hal_timer_get_tick());
    
    hal_timer_tick();
    TEST_ASSERT_EQUAL_UINT32(2U, hal_timer_get_tick());
}

int main(void)
{
    UNITY_BEGIN();
    (void)RUN_TEST(test_hal_sim_init_sets_registers_to_zero);
    (void)RUN_TEST(test_hal_sim_read_write_register);
    (void)RUN_TEST(test_hal_sim_update_changes_values);
    (void)RUN_TEST(test_hal_timer_init_resets);
    (void)RUN_TEST(test_hal_timer_tick_increments);
    return UNITY_END();
}