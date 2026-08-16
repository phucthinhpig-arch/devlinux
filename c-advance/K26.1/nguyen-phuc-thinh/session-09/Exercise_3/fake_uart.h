/**
 * @file fake_uart.h
 * @brief FFF mock definitions for the UART HAL.
 */
#ifndef FAKE_UART_H
#define FAKE_UART_H

#include "fff.h"
#include "uart_hal.h"

/* Declare fake functions for the HAL */
DECLARE_FAKE_VALUE_FUNC(uint32_t, uart_hal_receive, uint8_t*, uint32_t);
DECLARE_FAKE_VALUE_FUNC(bool, uart_hal_transmit, const uint8_t*, uint32_t);

#endif /* FAKE_UART_H */