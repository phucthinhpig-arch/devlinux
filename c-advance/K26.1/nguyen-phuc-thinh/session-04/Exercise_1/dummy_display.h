#ifndef DUMMY_DISPLAY_H
#define DUMMY_DISPLAY_H

#include "i_display.h"

/** @brief Instance toàn cục cho dummy display */
extern i_display_t dummy_display;

/** @brief Counter để kiểm tra số lần gọi */
extern uint32_t dummy_draw_count;

#endif /* DUMMY_DISPLAY_H */
