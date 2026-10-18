#ifndef HAL_SIM_H
#define HAL_SIM_H

#include <stdint.h>

/** @brief Simulated register addresses. */
#define HAL_REG_TEMP_RAW    (0x00U)
#define HAL_REG_HUMI_RAW    (0x01U)
#define HAL_REG_STATUS      (0x02U)
#define HAL_REG_COUNT       (3U)

void     hal_sim_init(uint32_t seed);
uint16_t hal_sim_read_register(uint8_t reg_addr);
void     hal_sim_write_register(uint8_t reg_addr, uint16_t value);
void     hal_sim_update(void);

#endif /* HAL_SIM_H */