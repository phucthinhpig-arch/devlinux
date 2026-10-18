#include "unity.h"
#include "ring_buffer.h"
#include "data_proc.h"
#include "event_log.h"

void setUp(void) {}
void tearDown(void) {}

void test_ring_buffer_init_empty(void) {
    ring_buffer_t rb;
    ring_init(&rb);
    TEST_ASSERT_TRUE(ring_is_empty(&rb));
}

void test_data_proc_startup_phase(void) {
    data_proc_t filter;
    data_proc_init(&filter, 4);
    TEST_ASSERT_EQUAL(STATUS_OK, data_proc_add_sample(&filter, 100));
}

void test_event_log_sorted_insert(void) {
    event_log_init();
    event_node_t new_event = {
        .type = EVENT_SYSTEM_RESET,
        .timestamp = 10,
        .value = 0,
        .p_next = NULL
    };
    TEST_ASSERT_EQUAL(STATUS_OK, event_log_add(&new_event));
}

int main(void) {
    UNITY_BEGIN();
    (void)RUN_TEST(test_ring_buffer_init_empty);
    (void)RUN_TEST(test_data_proc_startup_phase);
    (void)RUN_TEST(test_event_log_sorted_insert);
    return UNITY_END();
}