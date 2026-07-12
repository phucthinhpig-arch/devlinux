#include "console_display.h"
#include <stdio.h>

struct display_config_s {
    uint32_t baud_rate;
};


static void console_display_init(display_config_t *p_config)
{
    if (p_config == NULL) {
        return;
    }
    
    struct display_config_s *p_cfg = (struct display_config_s*)p_config;
    printf("[Console] Init with baud %u\n", p_cfg->baud_rate);
}

static void console_display_draw_pixel(uint16_t x, uint16_t y, uint8_t color)
{
    printf("[Console] Drawing pixel at (%u,%u) with color %u\n", x, y, color);
}

display_config_t* console_config_create(uint32_t baud_rate)
{
    static struct display_config_s cfg;
    cfg.baud_rate = baud_rate;
    return (display_config_t*)&cfg;
}

i_display_t console_display = {
    .init = console_display_init,
    .draw_pixel = console_display_draw_pixel
};
