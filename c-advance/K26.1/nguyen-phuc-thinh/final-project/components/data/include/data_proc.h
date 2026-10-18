#ifndef DATA_PROC_H
#define DATA_PROC_H

#include "ring_buffer.h"

typedef struct {
    ring_buffer_t buffer;
    int32_t       running_sum;
    uint8_t       window_size;
} data_proc_t;

void     data_proc_init(data_proc_t *p_proc, uint8_t window_size);
status_t data_proc_add_sample(data_proc_t *p_proc, int16_t sample);
int16_t  data_proc_get_average(const data_proc_t *p_proc);

#endif /* DATA_PROC_H */