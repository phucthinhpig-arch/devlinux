#ifndef HAL_TIMER_H
#define HAL_TIMER_H

#include <stdint.h>

void     hal_timer_init(void);
uint32_t hal_timer_get_tick(void);
void     hal_timer_tick(void);

#endif /* HAL_TIMER_H */