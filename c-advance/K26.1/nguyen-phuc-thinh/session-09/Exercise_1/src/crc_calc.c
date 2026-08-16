/**
 * @file crc_calc.c
 * @brief Implementation of the CRC calculation module.
 */
#include "crc_calc.h"

uint8_t crc8_calc(const uint8_t *data, uint32_t length) 
{
    uint8_t crc = 0;
    
    for (uint32_t i = 0; i < length; i++) 
    {
        crc ^= data[i]; /* Simplified CRC calculation */
    }
    
    return crc;
}