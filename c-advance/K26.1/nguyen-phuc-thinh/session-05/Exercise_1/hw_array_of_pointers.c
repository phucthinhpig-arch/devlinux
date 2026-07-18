#include <stdint.h>
#include <stdio.h>

/**
 * @brief Enumeration of system error codes.
 */
typedef enum {
    ERR_OK = 0,
    ERR_TIMEOUT,
    ERR_HW_FAIL,
    ERR_COUNT /* Automatically equals the number of real errors */
} error_code_t;

/**
 * @brief Array of pointers to string literals.
 * Stored as pointers to save RAM instead of padded 2D char arrays.
 */
static const char * const p_error_strings[] = {
    [ERR_OK]      = "OK",
    [ERR_TIMEOUT] = "TIMEOUT_ERROR",
    [ERR_HW_FAIL] = "HARDWARE_FAILURE"
};

/**
 * @brief Converts an error code into a human-readable string.
 * 
 * @param err_code The error code to look up.
 * @return const char* Pointer to the error string, or "UNKNOWN_ERROR" if invalid.
 */
const char *get_error_string(uint8_t err_code)
{
    /* MISRA-C Directive 4.14 & CERT-C ARR30-C: Bounds-checking */
    if (err_code < (uint8_t)ERR_COUNT)
    {
        return p_error_strings[err_code];
    }
    
    return "UNKNOWN_ERROR";
}

int main(void)
{
    /* Test valid error code */
    printf("Error code 1: %s\n", get_error_string((uint8_t)ERR_TIMEOUT));
    
    /* Test invalid error code (bounds checking test) */
    printf("Error code 99: %s\n", get_error_string(99));
    
    return 0;
}