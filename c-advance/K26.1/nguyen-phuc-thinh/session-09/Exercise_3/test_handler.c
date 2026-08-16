/**
 * @file test_handler.c
 * @brief Unit tests for the command handler using Unity and FFF.
 */
#include "unity.h"
#include "fake_uart.h"
#include <stdbool.h>

/* Instantiate FFF globals */
DEFINE_FFF_GLOBALS;

/* External declaration of the function under test */
extern bool cmd_process_echo(void);

/** @brief Setup runs before every test. */
void setUp(void) 
{
    /* Reset fakes and history to ensure tests are isolated */
    RESET_FAKE(uart_hal_receive);
    RESET_FAKE(uart_hal_transmit);
    FFF_RESET_HISTORY();
}

/** @brief Teardown runs after every test. */
void tearDown(void) 
{
}

/** @brief Test normal echo behavior when data is received. */
void test_echo_success(void) 
{
    /* Arrange: Mock receive to return 5 bytes, transmit to succeed */
    uart_hal_receive_fake.return_val = 5;
    uart_hal_transmit_fake.return_val = true;

    /* Act */
    bool result = cmd_process_echo();

    /* Assert: Function returns true and transmit was called exactly once */
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT32(1, uart_hal_transmit_fake.call_count);
}

/** @brief Test behavior when no data is received. */
void test_echo_no_data(void) 
{
    /* Arrange: Mock receive to return 0 bytes */
    uart_hal_receive_fake.return_val = 0;

    /* Act */
    bool result = cmd_process_echo();

    /* Assert: Function returns false and transmit was never called */
    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_EQUAL_UINT32(0, uart_hal_transmit_fake.call_count);
}

/**
 * @brief Main test runner.
 * 
 * @return int Exit status.
 */
int main(void) 
{
    UNITY_BEGIN();
    RUN_TEST(test_echo_success);
    RUN_TEST(test_echo_no_data);
    return UNITY_END();
}