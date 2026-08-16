/**
 * @file crc_calc.h
 * @brief CRC calculation module.
 */
#ifndef CRC_CALC_H
#define CRC_CALC_H

#include <stdint.h>

/**
 * @brief Calculates a simplified CRC-8 over a data array.
 * 
 * @param data Pointer to the data array.
 * @param length Number of bytes in the data array.
 * @return uint8_t The calculated CRC-8 value.
 */
uint8_t crc8_calc(const uint8_t *data, uint32_t length);

#endif /* CRC_CALC_H */