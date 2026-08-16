#include "i_display.h"
#include "console_display.h"
#include "dummy_display.h"
#include <stdio.h>

/**
 * @brief Vẽ hình chữ nhật 2x2 bằng interface
 */
static void draw_rectangle(i_display_t *p_disp)
{
    if ((p_disp == NULL) || (p_disp->draw_pixel == NULL)) {
        return;
    }

    p_disp->draw_pixel(0U, 0U, 1U);
    p_disp->draw_pixel(1U, 0U, 1U);
    p_disp->draw_pixel(0U, 1U, 1U);
    p_disp->draw_pixel(1U, 1U, 1U);
}

int main(void)
{
    
    display_config_t *p_cfg = console_config_create(9600U);
    if (console_display.init != NULL) {
        console_display.init(p_cfg);
    }
    draw_rectangle(&console_display);

    
    if (dummy_display.init != NULL) {
        dummy_display.init(NULL);
    }
    draw_rectangle(&dummy_display);
    printf("Dummy display was called %u times.\n", dummy_draw_count);

    return 0;
}
