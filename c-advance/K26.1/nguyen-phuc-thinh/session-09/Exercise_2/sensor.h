/**
 * @file sensor.h
 * @brief Sensor driver module API.
 */

#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h>

/**
 * @brief Convert raw ADC (12-bit) to millivolts.
 * 
 * @param raw_adc Valid range: 0-4095.
 * @return Millivolts, or 0 if out of range.
 */
uint32_t sensor_adc_to_mv(uint32_t raw_adc);

/**
 * @brief Calculate the moving average of an array of samples.
 * 
 * @param samples Array of millivolt readings.
 * @param count Number of samples.
 * @return Average, or 0 if count is 0.
 */
uint32_t sensor_moving_average(const uint32_t *samples, uint32_t count);

#endif /* SENSOR_H */