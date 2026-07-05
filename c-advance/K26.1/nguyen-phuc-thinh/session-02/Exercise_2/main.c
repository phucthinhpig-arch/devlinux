/**
 * @file main.c
 * @brief Stack depth monitor for Session 02 - Exercise 2.
 */

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#define MAX_RECURSION_DEPTH     (100U)
#define STACK_LIMIT_BYTES       (4096U)
#define RESULT_SUCCESS          ((int8_t)0)
#define RESULT_STACK_LIMIT_HIT  ((int8_t)-1)
#define PROGRAM_SUCCESS         (0)
#define PROGRAM_FAILURE         (1)

/**
 * @brief Calculate absolute distance between two addresses.
 *
 * @param[in] first_addr  First address value.
 * @param[in] second_addr Second address value.
 * @return Distance in bytes between the two addresses.
 */
static uintptr_t get_address_distance(uintptr_t first_addr, uintptr_t second_addr);

/**
 * @brief Monitor stack depth during recursion and abort if limit is exceeded.
 *
 * MISRA-C 2012 Rule 17.2 forbids recursion because recursive calls can grow
 * stack usage without a fixed compile-time bound. This exercise intentionally
 * uses recursion for learning, and the stack_limit_bytes guard stops recursion
 * before the configured stack budget is exceeded.
 *
 * @param[in] current_depth     Current recursion depth, starting from 0.
 * @param[in] max_depth         Maximum recursion depth to attempt.
 * @param[in] stack_base_addr   Address of a local variable in main().
 * @param[in] stack_limit_bytes Maximum allowed stack consumption in bytes.
 * @return 0 on success, or -1 if the stack limit was reached.
 */
int8_t recurse_with_monitor(uint32_t current_depth,
                            uint32_t max_depth,
                            const uintptr_t stack_base_addr,
                            uint32_t stack_limit_bytes);

/**
 * @brief Program entry point.
 *
 * @return 0 on success, non-zero on error.
 */
int main(void)
{
    uint8_t stack_base_marker = 0U;
    const uintptr_t stack_base_addr = (uintptr_t)&stack_base_marker;
    int8_t result = RESULT_SUCCESS;
    int program_status = PROGRAM_SUCCESS;

    printf("=== Stack Depth Monitor (limit: %" PRIu32 " bytes) ===\n",
           (uint32_t)STACK_LIMIT_BYTES);
    printf("[BASE]     stack_base addr:   %p\n", (void *)&stack_base_marker);

    result = recurse_with_monitor(0U,
                                  (uint32_t)MAX_RECURSION_DEPTH,
                                  stack_base_addr,
                                  (uint32_t)STACK_LIMIT_BYTES);

    if (result == RESULT_SUCCESS)
    {
        printf("Result: %d (max depth reached safely)\n", (int)result);
        program_status = PROGRAM_SUCCESS;
    }
    else
    {
        printf("Result: %d (stack limit reached)\n", (int)result);
        program_status = PROGRAM_FAILURE;
    }

    return program_status;
}

static uintptr_t get_address_distance(uintptr_t first_addr, uintptr_t second_addr)
{
    uintptr_t distance = 0U;

    if (first_addr >= second_addr)
    {
        distance = first_addr - second_addr;
    }
    else
    {
        distance = second_addr - first_addr;
    }

    return distance;
}

int8_t recurse_with_monitor(uint32_t current_depth,
                            uint32_t max_depth,
                            const uintptr_t stack_base_addr,
                            uint32_t stack_limit_bytes)
{
    uint8_t stack_marker = 0U;
    const uintptr_t stack_marker_addr = (uintptr_t)&stack_marker;
    const uintptr_t stack_used = get_address_distance(stack_base_addr,
                                                      stack_marker_addr);
    int8_t result = RESULT_SUCCESS;

    printf("[Depth %3" PRIu32 "] stack_marker addr: %p, stack used: %5" PRIuPTR " bytes\n",
           current_depth,
           (void *)&stack_marker,
           stack_used);

    if (stack_used >= (uintptr_t)stack_limit_bytes)
    {
        printf("[Depth %3" PRIu32 "] WARNING: Stack usage (%" PRIuPTR
               " bytes) exceeds limit! Aborting recursion.\n",
               current_depth,
               stack_used);
        result = RESULT_STACK_LIMIT_HIT;
    }
    else if (current_depth >= max_depth)
    {
        result = RESULT_SUCCESS;
    }
    else
    {
        result = recurse_with_monitor(current_depth + 1U,
                                      max_depth,
                                      stack_base_addr,
                                      stack_limit_bytes);
    }

    return result;
}
