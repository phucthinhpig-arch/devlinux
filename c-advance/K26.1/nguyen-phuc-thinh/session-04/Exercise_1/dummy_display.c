#include "dummy_display.h"

uint32_t dummy_draw_count = 0U;


static void dummy_display_init(display_config_t *p_config)
{
    (void)p_config; 
}

static void dummy_display_draw_pixel(uint16_t x, uint16_t y, uint8_t color)
{
    (void)x;
    (void)y;
    (void)color;
    dummy_draw_count++;
}

i_display_t dummy_display = {
    .init = dummy_display_init,
    .draw_pixel = dummy_display_draw_pixel
};
