#include "app_core.h"
#include "sensor_mgr.h"
#include "event_log.h"
#include "telemetry.h"
#include "data_proc.h"
#include "common_types.h"
#include "contract.h"
#include <stddef.h>
#include <stdint.h>

static app_config_t *s_p_cfg = NULL;

status_t app_core_init(app_config_t *p_config) {
    REQUIRE(p_config != NULL);
    s_p_cfg = p_config;
    
    /* Khởi tạo bộ lọc trung bình động với window_size = 5 */
    data_proc_init(&s_p_cfg->temp_filter, 5);
    data_proc_init(&s_p_cfg->humi_filter, 5);
    
    status_t err = sensor_mgr_init();
    if (err != STATUS_OK) { return err; }
    
    event_log_init();
    return STATUS_OK;
}

status_t app_core_process_cycle(uint8_t *p_tx_buf, uint32_t buf_size, uint32_t *p_tx_len) {
    REQUIRE(s_p_cfg != NULL);
    REQUIRE(p_tx_buf != NULL);
    REQUIRE(p_tx_len != NULL);
    
    /* 1. Đọc dữ liệu thô từ cảm biến */
    sensor_data_t raw_data = {0};
    status_t err = sensor_mgr_read_all(&raw_data);
    if (err != STATUS_OK) {
        event_node_t evt = { .type = EVENT_SENSOR_ERROR, .timestamp = 0, .value = 0 };
        (void)event_log_add(&evt);
        return err;
    }
    
    /* 2. Đưa qua bộ lọc trung bình động */
    (void)data_proc_add_sample(&s_p_cfg->temp_filter, raw_data.temperature);
    (void)data_proc_add_sample(&s_p_cfg->humi_filter, raw_data.humidity);
    
    sensor_data_t proc_data = {
        .temperature = data_proc_get_average(&s_p_cfg->temp_filter),
        .humidity    = data_proc_get_average(&s_p_cfg->humi_filter),
        .timestamp   = raw_data.timestamp
    };
    
    /* 3. Kiểm tra ngưỡng cảnh báo & ghi Log sự kiện */
    if (proc_data.temperature > s_p_cfg->temp_threshold_high) {
        event_node_t evt = { .type = EVENT_ALERT_TEMP_HIGH, .timestamp = proc_data.timestamp, .value = proc_data.temperature };
        (void)event_log_add(&evt);
    }
    if (proc_data.humidity > s_p_cfg->humi_threshold_high) {
        event_node_t evt = { .type = EVENT_ALERT_HUMI_HIGH, .timestamp = proc_data.timestamp, .value = proc_data.humidity };
        (void)event_log_add(&evt);
    }
    
    /* 4. Đóng gói TLV truyền đi */
    return telemetry_serialize(&proc_data, p_tx_buf, buf_size, p_tx_len);
}