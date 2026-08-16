/**
 * @file uart_hal.h
 * @brief Hardware Abstraction Layer for UART communication.
 */
#ifndef UART_HAL_H
#define UART_HAL_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Receive data from UART.
 * 
 * @param buf Pointer to the buffer where received data will be stored.
 * @param max_len Maximum number of bytes to receive.
 * @return uint32_t Number of bytes actually received.
 */
uint32_t uart_hal_receive(uint8_t *buf, uint32_t max_len);

/**
 * @brief Transmit data over UART.
 * 
 * @param buf Pointer to the buffer containing data to transmit.
 * @param len Number of bytes to transmit.
 * @return true if transmission is successful, false otherwise.
 */
bool uart_hal_transmit(const uint8_t *buf, uint32_t len);

#endif /* UART_HAL_H */