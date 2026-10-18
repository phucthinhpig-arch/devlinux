#include "logger.h"
#include "hal_timer.h"
#include <stddef.h>
#include <stdint.h> /* Khắc phục cảnh báo thiếu thư viện cho uint32_t */
#include <stdio.h>

/* Mảng hằng số dùng để ánh xạ từ Enum sang chuỗi (String) */
static const char* const s_level_strings[LOG_LEVEL_COUNT] = {
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR"
};

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
void logger_format(char* buffer, size_t max_size, log_level_t level, const char* msg)
{
    if ((buffer == NULL) || (max_size == 0U) || (msg == NULL) || (level >= LOG_LEVEL_COUNT)) {
        return;
    }

    uint32_t current_tick = hal_timer_get_tick();
    
    /* Vô hiệu hóa cảnh báo C11 do hệ thống nhúng của ta quản lý an toàn size_t max_size */
    // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
    (void)snprintf(buffer, max_size, "[%08u] [%s] %s", current_tick, s_level_strings[level], msg);
}