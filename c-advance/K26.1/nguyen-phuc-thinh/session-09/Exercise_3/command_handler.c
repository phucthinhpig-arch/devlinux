/**
 * @file command_handler.c
 * @brief Command handler module implementation.
 */
#include "uart_hal.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Processes an echo command by receiving data and transmitting it back.
 * 
 * @return true if data was received and echoed, false otherwise.
 */
bool cmd_process_echo(void) 
{
    uint8_t buffer[64];
    uint32_t bytes_read = uart_hal_receive(buffer, sizeof(buffer));

    if (bytes_read > 0) 
    {
        return uart_hal_transmit(buffer, bytes_read);
    }
    
    return false;
}