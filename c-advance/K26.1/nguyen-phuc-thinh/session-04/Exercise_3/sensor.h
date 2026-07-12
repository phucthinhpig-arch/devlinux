#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h>

/**
 * @brief Cấu trúc dữ liệu cảm biến
 */
typedef struct {
    uint16_t temperature; 
    uint32_t timestamp;   
} sensor_data_t;

/**
 * @brief Phân tích dữ liệu cảm biến từ buffer big-endian
 *
 * @param[in]  p_buffer   Con trỏ tới mảng 6 byte raw payload
 * @param[out] p_out_data Con trỏ tới struct để ghi kết quả
 */
void parse_sensor_data(const uint8_t *p_buffer, sensor_data_t *p_out_data);

#endif 