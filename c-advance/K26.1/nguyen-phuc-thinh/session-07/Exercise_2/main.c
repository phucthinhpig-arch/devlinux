/**
 * @file main.c
 * @brief Exercise 2: Variadic Stats
 */

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>

/**
 * @brief Structure to hold statistics.
 */
typedef struct {
    int32_t min;
    int32_t max;
    int32_t average;
} stats_t;

/**
 * @brief Computes min, max, and average for a variable number of integer arguments.
 * 
 * @param count The number of variable arguments.
 * @param ... The integer arguments.
 * @return stats_t The computed statistics.
 */
stats_t compute_stats(uint32_t count, ...)
{
    stats_t result = {0, 0, 0};

    if (count == 0U)
    {
        return result;
    }

    va_list ap;
    va_start(ap, count);

    /* Read the first argument to initialize min and max */
    int32_t first_val = (int32_t)va_arg(ap, int);
    result.min = first_val;
    result.max = first_val;
    
    /* Use int64_t to prevent potential overflow when summing */
    int64_t sum = (int64_t)first_val;

    for (uint32_t i = 1U; i < count; ++i)
    {
        int32_t val = (int32_t)va_arg(ap, int);
        
        if (val < result.min)
        {
            result.min = val;
        }
        if (val > result.max)
        {
            result.max = val;
        }
        
        sum += (int64_t)val;
    }

    va_end(ap);

    result.average = (int32_t)(sum / (int64_t)count);

    return result;
}

/**
 * @brief Main function.
 * 
 * @return int Exit status.
 */
int main(void)
{
    printf("=== Exercise 2: Variadic Stats ===\n\n");

    stats_t test1 = compute_stats(5U, 10, -5, 20, 0, 5);
    printf("Test 1 (5, 10, -5, 20, 0, 5):\n");
    printf("Min: %d\n", test1.min);
    printf("Max: %d\n", test1.max);
    printf("Avg: %d\n\n", test1.average);

    stats_t test2 = compute_stats(2U, 42, 42);
    printf("Test 2 (2, 42, 42):\n");
    printf("Min: %d\n", test2.min);
    printf("Max: %d\n", test2.max);
    printf("Avg: %d\n\n", test2.average);

    stats_t test3 = compute_stats(0U);
    printf("Test 3 (0 args):\n");
    printf("Min: %d\n", test3.min);
    printf("Max: %d\n", test3.max);
    printf("Avg: %d\n", test3.average);

    return 0;
}