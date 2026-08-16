/**
 * @file ring_buffer.h
 * @brief Ring buffer module for safe data queuing.
 */
#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdbool.h>

/**
 * @brief Pushes a value into the ring buffer.
 * 
 * @param val Value to push into the buffer.
 * @return true if the push was successful, false otherwise.
 */
bool ring_buf_push(int val);

/**
 * @brief Pops a value from the ring buffer.
 * 
 * @param val Pointer to the variable where the popped value will be stored.
 * @return true if the pop was successful, false otherwise.
 */
bool ring_buf_pop(int *val);

#endif /* RING_BUFFER_H */