/**
 * @file sensor.c
 * @brief Implementation of the sensor driver module.
 */

#include "sensor.h"

#define VREF_MV  3300U
#define ADC_MAX  4095U

uint32_t sensor_adc_to_mv(uint32_t raw_adc) 
{
    if (raw_adc > ADC_MAX) 
    {
        return 0U; /* Safely handle out-of-range values */
    }
    
    return (raw_adc * VREF_MV) / ADC_MAX;
}

uint32_t sensor_moving_average(const uint32_t *samples, uint32_t count) 
{
    /* Prevent NULL pointer dereference and divide-by-zero */
    if ((samples == 0) || (count == 0U)) 
    {
        return 0U; 
    }

    uint32_t sum = 0U;
    
    for (uint32_t i = 0U; i < count; i++) 
    {
        sum += samples[i];
    }

    return (sum / count);
}