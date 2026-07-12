#ifndef POOL_H
#define POOL_H

#include <stdint.h>
#include <stdbool.h>

/** @brief Kích thước của pool */
#define POOL_SIZE 5U

/**
 * @brief Cấu trúc gói tin mạng
 */
typedef struct {
    uint32_t id;              /**< ID gói tin */
    uint8_t payload[64];      /**< Dữ liệu gói tin */
} network_packet_t;

/**
 * @brief Khởi tạo pool (đánh dấu tất cả là rảnh)
 */
void pool_init(void);

/**
 * @brief Cấp phát một packet từ pool
 * @return Con trỏ tới packet, hoặc NULL nếu pool đầy
 */
network_packet_t* packet_alloc(void);

/**
 * @brief Giải phóng một packet về pool
 * @param p_packet Con trỏ tới packet cần giải phóng
 */
void packet_free(network_packet_t* p_packet);

#endif /* POOL_H */
