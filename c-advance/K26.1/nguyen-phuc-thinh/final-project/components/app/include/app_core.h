#ifndef APP_CORE_H
#define APP_CORE_H

#include "common_types.h"
#include "data_proc.h"

typedef struct {
    data_proc_t temp_filter;
    data_proc_t humi_filter;
    int16_t     temp_threshold_high;
    int16_t     humi_threshold_high;
} app_config_t;

status_t app_core_init(app_config_t *p_config);
status_t app_core_process_cycle(uint8_t *p_tx_buf, uint32_t buf_size, uint32_t *p_tx_len);

#endif /* APP_CORE_H */