#include "hal_timer.h"

static uint32_t s_tick = 0U;

/**
 * @brief Initialize the hardware timer simulation.
 */
void hal_timer_init(void)
{
    s_tick = 0U;
}

/**
 * @brief Get the current system tick count.
 * @return Current tick value.
 */
uint32_t hal_timer_get_tick(void)
{
    return s_tick;
}

/**
 * @brief Increment the system tick counter.
 */
void hal_timer_tick(void)
{
    ++s_tick;
}