#include "ring_buffer.h"
#include "contract.h"
#include <stddef.h>

void ring_init(ring_buffer_t *p_rb) {
    REQUIRE(p_rb != NULL);
    for (uint32_t i = 0U; i < RING_BUFFER_CAPACITY; ++i) {
        p_rb->data[i] = 0;
    }
    p_rb->head = 0U;
    p_rb->tail = 0U;
    p_rb->count = 0U;
    p_rb->capacity = RING_BUFFER_CAPACITY;
}

void ring_push(ring_buffer_t *p_rb, int16_t value) {
    REQUIRE(p_rb != NULL);
    REQUIRE(!ring_is_full(p_rb));
    p_rb->data[p_rb->head] = value;
    p_rb->head = (p_rb->head + 1U) % p_rb->capacity;
    p_rb->count++;
    INVARIANT(p_rb->count <= p_rb->capacity);
}

int16_t ring_pop(ring_buffer_t *p_rb) {
    REQUIRE(p_rb != NULL);
    REQUIRE(!ring_is_empty(p_rb));
    int16_t value = p_rb->data[p_rb->tail];
    p_rb->tail = (p_rb->tail + 1U) % p_rb->capacity;
    p_rb->count--;
    return value;
}

int16_t ring_peek(const ring_buffer_t *p_rb) {
    REQUIRE(p_rb != NULL);
    REQUIRE(!ring_is_empty(p_rb));
    return p_rb->data[p_rb->tail];
}

bool ring_is_full(const ring_buffer_t *p_rb) {
    REQUIRE(p_rb != NULL);
    return p_rb->count >= p_rb->capacity;
}

bool ring_is_empty(const ring_buffer_t *p_rb) {
    REQUIRE(p_rb != NULL);
    return p_rb->count == 0U;
}

uint32_t ring_count(const ring_buffer_t *p_rb) {
    REQUIRE(p_rb != NULL);
    return p_rb->count;
}