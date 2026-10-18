#include <stdio.h>

#include "hal_sim.h"
#include "hal_timer.h"

int main(void)
{
    hal_sim_init(42U);
    hal_timer_init();

    (void)printf("=== HAL Simulation Demo ===\n");
    (void)printf("HAL initialized with seed: 42\n");

    hal_sim_update();
    (void)printf("Register[0x00] = 0x%04X (raw temp)\n",
                  (unsigned int)hal_sim_read_register(HAL_REG_TEMP_RAW));
    (void)printf("Register[0x01] = 0x%04X (raw humi)\n",
                  (unsigned int)hal_sim_read_register(HAL_REG_HUMI_RAW));
    (void)printf("Timer tick: %u\n", (unsigned int)hal_timer_get_tick());

    hal_timer_tick();
    (void)printf("Timer tick: %u\n", (unsigned int)hal_timer_get_tick());

    hal_timer_tick();
    (void)printf("Timer tick: %u\n", (unsigned int)hal_timer_get_tick());

    return 0;
}
