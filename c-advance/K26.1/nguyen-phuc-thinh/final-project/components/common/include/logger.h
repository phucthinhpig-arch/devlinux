#ifndef LOGGER_H
#define LOGGER_H

#include <stddef.h>

/** @brief Các mức độ cảnh báo của hệ thống */
typedef enum {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_COUNT
} log_level_t;

void logger_format(char* buffer, size_t max_size, log_level_t level, const char* msg);

#endif /* LOGGER_H */