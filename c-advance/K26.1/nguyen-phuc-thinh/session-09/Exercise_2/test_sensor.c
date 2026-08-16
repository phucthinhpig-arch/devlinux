/**
 * @file test_sensor.c
 * @brief Unit tests for the sensor driver module using Unity.
 */

#include "unity.h"
#include "sensor.h"

/** @brief Unity setup function (runs before each test). */
void setUp(void) 
{
    /* No setup needed for this module */
}

/** @brief Unity teardown function (runs after each test). */
void tearDown(void) 
{
    /* No teardown needed for this module */
}

void test_adc_zero(void) 
{
    TEST_ASSERT_EQUAL_INT32(0, sensor_adc_to_mv(0));
}

void test_adc_max(void) 
{
    /* Assuming a standard 3300mV reference for a 12-bit ADC */
    TEST_ASSERT_EQUAL_INT32(3300, sensor_adc_to_mv(4095));
}

void test_adc_out_of_range(void) 
{
    TEST_ASSERT_EQUAL_INT32(0, sensor_adc_to_mv(4096));
}

void test_moving_average_valid(void) 
{
    uint32_t samples[4] = {1000, 2000, 3000, 4000};
    TEST_ASSERT_EQUAL_INT32(2500, sensor_moving_average(samples, 4));
}

void test_moving_average_zero_count(void) 
{
    uint32_t samples[4] = {1000, 2000, 3000, 4000};
    TEST_ASSERT_EQUAL_INT32(0, sensor_moving_average(samples, 0));
}

/**
 * @brief Main test runner.
 * 
 * @return int Exit status.
 */
int main(void) 
{
    UNITY_BEGIN();
    RUN_TEST(test_adc_zero);
    RUN_TEST(test_adc_max);
    RUN_TEST(test_adc_out_of_range);
    RUN_TEST(test_moving_average_valid);
    RUN_TEST(test_moving_average_zero_count);
    return UNITY_END();
}