#ifndef CONSOLE_DISPLAY_H
#define CONSOLE_DISPLAY_H

#include "i_display.h"

/**
 * @brief Tạo config cho console display
 * @param baud_rate tốc độ baud giả lập
 * @return opaque pointer tới config
 */
display_config_t* console_config_create(uint32_t baud_rate);

/** @brief Instance toàn cục cho console display */
extern i_display_t console_display;

#endif /* CONSOLE_DISPLAY_H */
