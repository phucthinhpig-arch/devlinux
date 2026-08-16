/**
 * @file main.c
 * @brief Capstone - The Industrial Logger Module
 */

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>

/* Define log levels */
#define LOG_LEVEL_ERROR   0
#define LOG_LEVEL_WARNING 1
#define LOG_LEVEL_INFO    2
#define LOG_LEVEL_DEBUG   3

/* Default to DEBUG if not defined via compiler flag */
#ifndef LOG_LEVEL_MAX
#define LOG_LEVEL_MAX LOG_LEVEL_DEBUG
#endif

/**
 * @brief Core logging function that formats and prints the message.
 *        Uses vprintf to simulate UART output.
 * 
 * @param level The log level of the message.
 * @param file  The source file name (__FILE__).
 * @param line  The line number (__LINE__).
 * @param func  The function name (__func__).
 * @param fmt   The format string.
 * @param ...   Variadic arguments for the format string.
 */
__attribute__((format(printf, 5, 6)))
void log_write(uint8_t level, const char *file, uint32_t line, 
               const char *func, const char *fmt, ...)
{
    /* Only print if the level is within the maximum allowed level */
    if (level <= LOG_LEVEL_MAX)
    {
        const char *level_str = "UNKN ";
        
        /* Map log level to string representation */
        switch (level)
        {
            case LOG_LEVEL_ERROR:
                level_str = "ERROR";
                break;
            case LOG_LEVEL_WARNING:
                level_str = "WARN ";
                break;
            case LOG_LEVEL_INFO:
                level_str = "INFO ";
                break;
            case LOG_LEVEL_DEBUG:
                level_str = "DEBUG";
                break;
            default:
                break;
        }

        /* Print the log prefix */
        printf("[%s] %s:%u (%s) | ", level_str, file, line, func);

        /* Process and print variadic arguments */
        va_list ap;
        va_start(ap, fmt);
        vprintf(fmt, ap); /* Using vprintf to simulate UART output as required */
        va_end(ap);

        printf("\n");
    }
}

/**
 * @brief Macro for ERROR level logging.
 *        Automatically captures __FILE__, __LINE__, and __func__.
 */
#define LOG_ERROR(fmt, ...) \
    do { \
        if (LOG_LEVEL_ERROR <= LOG_LEVEL_MAX) { \
            log_write(LOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__); \
        } \
    } while(0)

/**
 * @brief Macro for WARNING level logging.
 */
#define LOG_WARNING(fmt, ...) \
    do { \
        if (LOG_LEVEL_WARNING <= LOG_LEVEL_MAX) { \
            log_write(LOG_LEVEL_WARNING, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__); \
        } \
    } while(0)

/**
 * @brief Macro for INFO level logging.
 */
#define LOG_INFO(fmt, ...) \
    do { \
        if (LOG_LEVEL_INFO <= LOG_LEVEL_MAX) { \
            log_write(LOG_LEVEL_INFO, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__); \
        } \
    } while(0)

/**
 * @brief Macro for DEBUG level logging.
 */
#define LOG_DEBUG(fmt, ...) \
    do { \
        if (LOG_LEVEL_DEBUG <= LOG_LEVEL_MAX) { \
            log_write(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__); \
        } \
    } while(0)

/**
 * @brief Main entry point of the program.
 * 
 * @return int Exit code.
 */
int main(void)
{
    printf("=== Exercise 3: Industrial Logger ===\n");
    printf("Compiled with LOG_LEVEL_MAX = %d\n\n", LOG_LEVEL_MAX);

    LOG_INFO("System boot. Build time: 12:00:00");
    LOG_DEBUG("Discovered %d sensors on I2C bus.", 4);
    LOG_WARNING("Sensor %d reading is unstable.", 2);
    LOG_ERROR("Watchdog timeout! Rebooting in %d ms.", 500);
    LOG_INFO("Boot sequence complete.");

    return 0;
}