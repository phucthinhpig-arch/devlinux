#include "telemetry.h"
#include "contract.h"
#include <stddef.h>

/* Thuật toán CRC-8 cơ bản (đa thức 0x07) */
uint8_t telemetry_crc8(const uint8_t *p_data, uint32_t len) {
    uint8_t crc = 0x00U;
    if (p_data == NULL) { return crc; }
    
    for (uint32_t i = 0U; i < len; ++i) {
        crc ^= p_data[i];
        for (uint8_t j = 0U; j < 8U; ++j) {
            if ((crc & 0x80U) != 0U) {
                crc = (uint8_t)((crc << 1U) ^ 0x07U);
            } else {
                crc = (uint8_t)(crc << 1U);
            }
        }
    }
    return crc;
}

status_t telemetry_serialize(const sensor_data_t *p_data, uint8_t *p_buf, uint32_t buf_size, uint32_t *p_written) {
    if ((p_data == NULL) || (p_buf == NULL) || (p_written == NULL)) {
        return STATUS_ERR_NULL_PTR;
    }
    
    if (buf_size < TLV_MAX_PACKET_SIZE) {
        return STATUS_ERR_FULL;
    }

    /* Đóng gói TLV: [SYNC] [TEMP_MSB] [TEMP_LSB] [HUMI_MSB] [HUMI_LSB] [RSV] [RSV] [CRC] */
    p_buf[0] = TLV_SYNC_BYTE;
    
    /* Dịch bit để lấy Big-Endian cho Temperature */
    p_buf[1] = (uint8_t)(((uint16_t)p_data->temperature >> 8U) & 0xFFU);
    p_buf[2] = (uint8_t)((uint16_t)p_data->temperature & 0xFFU);
    
    /* Dịch bit để lấy Big-Endian cho Humidity */
    p_buf[3] = (uint8_t)(((uint16_t)p_data->humidity >> 8U) & 0xFFU);
    p_buf[4] = (uint8_t)((uint16_t)p_data->humidity & 0xFFU);
    
    p_buf[5] = 0x00U; /* Reserved */
    p_buf[6] = 0x00U; /* Reserved */
    
    /* Tính CRC-8 cho 7 byte đầu tiên */
    p_buf[7] = telemetry_crc8(p_buf, 7U);
    
    *p_written = TLV_MAX_PACKET_SIZE;
    return STATUS_OK;
}

status_t telemetry_deserialize(const uint8_t *p_buf, uint32_t buf_len, sensor_data_t *p_data) {
    if ((p_buf == NULL) || (p_data == NULL)) {
        return STATUS_ERR_NULL_PTR;
    }
    if (buf_len < TLV_MAX_PACKET_SIZE) {
        return STATUS_ERR_INVALID_PARAM;
    }
    if (p_buf[0] != TLV_SYNC_BYTE) {
        return STATUS_ERR_INVALID_PARAM;
    }
    
    /* Xác thực toàn vẹn dữ liệu bằng CRC-8 */
    uint8_t expected_crc = telemetry_crc8(p_buf, 7U);
    if (expected_crc != p_buf[7]) {
        return STATUS_ERR_INVALID_PARAM; /* Checksum fail */
    }
    
    /* Dịch ngược Big-Endian để tái tạo số nguyên */
    p_data->temperature = (int16_t)(((uint16_t)p_buf[1] << 8U) | (uint16_t)p_buf[2]);
    p_data->humidity    = (int16_t)(((uint16_t)p_buf[3] << 8U) | (uint16_t)p_buf[4]);
    
    return STATUS_OK;
}