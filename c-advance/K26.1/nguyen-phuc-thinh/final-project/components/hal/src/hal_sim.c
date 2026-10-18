#include "hal_sim.h"
#include <stdlib.h>
#include "contract.h"

/* Khởi tạo mảng bằng {0} theo chuẩn MISRA */
static volatile uint16_t s_registers[HAL_REG_COUNT] = {0};

/**
 * @brief Initialize the hardware simulation layer.
 * @param[in] seed Random seed for generating sensor data.
 */
void hal_sim_init(uint32_t seed)
{
    srand(seed); // NOLINT(cert-msc30-c, cert-msc50-cpp)
    
    /* Khai báo biến vòng lặp ngay bên trong for theo chuẩn C99 */
    for (uint8_t i = 0U; i < HAL_REG_COUNT; ++i) 
    {
        s_registers[i] = 0U;
    }
}

/**
 * @brief Read a value from a simulated hardware register.
 * @param[in] reg_addr The address of the register to read.
 * @return The 16-bit value stored in the register.
 */
uint16_t hal_sim_read_register(uint8_t reg_addr)
{
    REQUIRE(reg_addr < HAL_REG_COUNT);
    return s_registers[reg_addr];
}

/**
 * @brief Write a value to a simulated hardware register.
 * @param[in] reg_addr The address of the register to write to.
 * @param[in] value The 16-bit value to write.
 */
void hal_sim_write_register(uint8_t reg_addr, uint16_t value)
{
    REQUIRE(reg_addr < HAL_REG_COUNT);
    s_registers[reg_addr] = value;
}

/**
 * @brief Generate new simulated sensor values and update registers.
 */
void hal_sim_update(void)
{
    /* Thêm NOLINT để tắt lỗi CERT-C cho mục đích mô phỏng */
    s_registers[HAL_REG_TEMP_RAW] = (uint16_t)((uint32_t)rand() & 0xFFFFU); // NOLINT(cert-msc30-c, cert-msc50-cpp)
    s_registers[HAL_REG_HUMI_RAW] = (uint16_t)((uint32_t)rand() & 0xFFFFU); // NOLINT(cert-msc30-c, cert-msc50-cpp)
    s_registers[HAL_REG_STATUS]   = (uint16_t)((uint32_t)rand() & 0x0001U); // NOLINT(cert-msc30-c, cert-msc50-cpp)
}