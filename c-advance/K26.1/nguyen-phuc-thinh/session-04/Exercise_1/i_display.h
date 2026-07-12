#ifndef I_DISPLAY_H
#define I_DISPLAY_H

#include <stdint.h>

/**
 * @brief Opaque pointer để ẩn dữ liệu cấu hình
 */
typedef struct display_config_s display_config_t;

/**
 * @brief Interface HAL cho display
 */
typedef struct {
    void (*init)(display_config_t *p_config);
    void (*draw_pixel)(uint16_t x, uint16_t y, uint8_t color);
} i_display_t;

#endif /* I_DISPLAY_H */
