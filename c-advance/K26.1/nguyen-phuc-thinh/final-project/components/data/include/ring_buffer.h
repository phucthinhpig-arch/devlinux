#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include "common_types.h"

#define RING_BUFFER_CAPACITY (16U)

typedef struct {
    int16_t  data[RING_BUFFER_CAPACITY];
    uint32_t head;
    uint32_t tail;
    uint32_t count;
    uint32_t capacity;
} ring_buffer_t;

void     ring_init(ring_buffer_t *p_rb);
void     ring_push(ring_buffer_t *p_rb, int16_t value);
int16_t  ring_pop(ring_buffer_t *p_rb);
int16_t  ring_peek(const ring_buffer_t *p_rb);
bool     ring_is_full(const ring_buffer_t *p_rb);
bool     ring_is_empty(const ring_buffer_t *p_rb);
uint32_t ring_count(const ring_buffer_t *p_rb);

#endif /* RING_BUFFER_H */