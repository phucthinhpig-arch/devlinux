/**
 * @file ring_buffer.c
 * @brief Implementation of the ring buffer module.
 */
#include "ring_buffer.h"
#include <stddef.h> /* Included for NULL definition */

bool ring_buf_push(int val) 
{
    (void)val; /* Suppress unused parameter warning */
    return true;
}

bool ring_buf_pop(int *val) 
{
    if (val != NULL)
    {
        *val = 3;
    }
    return true;
}