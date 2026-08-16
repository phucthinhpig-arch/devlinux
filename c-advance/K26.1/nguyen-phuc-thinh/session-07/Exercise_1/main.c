/**
 * @file main.c
 * @brief Exercise 1: Safe Macros and Inline Functions.
 */

#include <stdio.h>
#include <stdint.h>

/**
 * @brief Macro to find the minimum of two values.
 * 
 * @param a First value.
 * @param b Second value.
 */
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

/**
 * @brief Macro to find the absolute value of a number.
 * 
 * @param x The input value.
 */
#define ABS(x) (((x) < 0) ? -(x) : (x))

/**
 * @brief Macro to clamp a value between a lower and upper bound.
 *        Modifies the variable in-place.
 * 
 * @param val The variable to clamp.
 * @param lo The lower bound.
 * @param hi The upper bound.
 */
#define CLAMP(val, lo, hi)          \
    do                              \
    {                               \
        if ((val) < (lo))           \
        {                           \
            (val) = (lo);           \
        }                           \
        else if ((val) > (hi))      \
        {                           \
            (val) = (hi);           \
        }                           \
    } while (0)

/**
 * @brief Type-safe minimum using static inline.
 *
 * @param a First value.
 * @param b Second value.
 * @return The smaller of the two values.
 */
static inline uint32_t safe_min_u32(uint32_t a, uint32_t b)
{
    return ((a < b) ? a : b);
}

/**
 * @brief Main entry point of the program.
 * 
 * @return Exit code (0 on success).
 */
int main(void)
{
    printf("=== Exercise 1: Safe Macros ===\n");

    printf("MIN(3, 5)          = %d\n", MIN(3, 5));
    printf("ABS(-7)            = %d\n", ABS(-7));
    printf("ABS(5 - 10)        = %d\n", ABS(5 - 10));

    /* For CLAMP, since it is a multi-statement macro using do-while(0), 
       it must modify a variable in-place rather than returning a value. */
    int clamp_val1 = 15;
    CLAMP(clamp_val1, 0, 10);
    printf("CLAMP(15, 0, 10)   = %d\n", clamp_val1);

    int clamp_val2 = -3;
    CLAMP(clamp_val2, 0, 10);
    printf("CLAMP(-3, 0, 10)   = %d\n", clamp_val2);

    /* Use unsigned literals (3U, 5U) to perfectly match uint32_t parameters */
    printf("safe_min_u32(3, 5) = %u\n", safe_min_u32(3U, 5U));

    return 0;
}/**
 * @file main.c
 * @brief Exercise 1: Safe Macros and Inline Functions.
 */

#include <stdio.h>
#include <stdint.h>

/**
 * @brief Macro to find the minimum of two values.
 * 
 * @param a First value.
 * @param b Second value.
 */
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

/**
 * @brief Macro to find the absolute value of a number.
 * 
 * @param x The input value.
 */
#define ABS(x) (((x) < 0) ? -(x) : (x))

/**
 * @brief Macro to clamp a value between a lower and upper bound.
 *        Modifies the variable in-place.
 * 
 * @param val The variable to clamp.
 * @param lo The lower bound.
 * @param hi The upper bound.
 */
#define CLAMP(val, lo, hi)          \
    do                              \
    {                               \
        if ((val) < (lo))           \
        {                           \
            (val) = (lo);           \
        }                           \
        else if ((val) > (hi))      \
        {                           \
            (val) = (hi);           \
        }                           \
    } while (0)

/**
 * @brief Type-safe minimum using static inline.
 *
 * @param a First value.
 * @param b Second value.
 * @return The smaller of the two values.
 */
static inline uint32_t safe_min_u32(uint32_t a, uint32_t b)
{
    return ((a < b) ? a : b);
}

/**
 * @brief Main entry point of the program.
 * 
 * @return Exit code (0 on success).
 */
int main(void)
{
    printf("=== Exercise 1: Safe Macros ===\n");

    printf("MIN(3, 5)          = %d\n", MIN(3, 5));
    printf("ABS(-7)            = %d\n", ABS(-7));
    printf("ABS(5 - 10)        = %d\n", ABS(5 - 10));

    /* For CLAMP, since it is a multi-statement macro using do-while(0), 
       it must modify a variable in-place rather than returning a value. */
    int clamp_val1 = 15;
    CLAMP(clamp_val1, 0, 10);
    printf("CLAMP(15, 0, 10)   = %d\n", clamp_val1);

    int clamp_val2 = -3;
    CLAMP(clamp_val2, 0, 10);
    printf("CLAMP(-3, 0, 10)   = %d\n", clamp_val2);

    /* Use unsigned literals (3U, 5U) to perfectly match uint32_t parameters */
    printf("safe_min_u32(3, 5) = %u\n", safe_min_u32(3U, 5U));

    return 0;
}