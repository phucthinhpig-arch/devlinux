#include "data_proc.h"
#include "contract.h"
#include <stddef.h>

void data_proc_init(data_proc_t *p_proc, uint8_t window_size) {
    REQUIRE(p_proc != NULL);
    REQUIRE((window_size > 0U) && (window_size <= RING_BUFFER_CAPACITY));
    
    ring_init(&p_proc->buffer);
    p_proc->running_sum = 0;
    p_proc->window_size = window_size;
}

status_t data_proc_add_sample(data_proc_t *p_proc, int16_t sample) {
    if (p_proc == NULL) { return STATUS_ERR_NULL_PTR; }
    
    /* Nếu buffer đã đạt kích thước cửa sổ, trừ phần tử cũ nhất khỏi tổng */
    if (ring_count(&p_proc->buffer) >= p_proc->window_size) {
        int16_t oldest = ring_pop(&p_proc->buffer);
        p_proc->running_sum -= oldest;
    }
    
    ring_push(&p_proc->buffer, sample);
    p_proc->running_sum += sample;
    
    return STATUS_OK;
}

int16_t data_proc_get_average(const data_proc_t *p_proc) {
    REQUIRE(p_proc != NULL);
    
    uint32_t count = ring_count(&p_proc->buffer);
    if (count == 0U) { return 0; }
    
    return (int16_t)(p_proc->running_sum / (int32_t)count);
}