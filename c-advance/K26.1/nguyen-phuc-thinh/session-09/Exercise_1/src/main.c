/**
 * @file main.c
 * @brief Main executable integrating CRC and Ring Buffer modules.
 */
#include <stdio.h>
#include <stdint.h>
#include "crc_calc.h"
#include "ring_buffer.h"

/**
 * @brief Main entry point of the program.
 * 
 * @return int Exit status (0 on success).
 */
int main(void) 
{
    uint8_t data[] = {0x01, 0x02, 0x03};
    
    printf("=== CRC-8 of {0x01, 0x02, 0x03} = 0x%02X ===\n", crc8_calc(data, 3));

    int val = 0;
    
    /* Ensure both push and pop succeed before printing */
    if (ring_buf_push(3) && ring_buf_pop(&val)) 
    {
        printf("=== Ring Buffer: Push 3, Pop %d | OK ===\n", val);
    }
    
    return 0;
}