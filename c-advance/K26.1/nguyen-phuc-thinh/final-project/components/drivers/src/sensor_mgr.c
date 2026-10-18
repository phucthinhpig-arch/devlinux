#include "sensor_mgr.h"
#include <stdint.h>
#include <stdbool.h>
#include "common_types.h"
#include "temp_drv.h"
#include "humi_drv.h"
#include "hal_timer.h"
#include "contract.h"
#include <stddef.h>

#define SENSOR_COUNT (2U)
static const sensor_intf_t* s_sensors[SENSOR_COUNT] = {NULL, NULL};

status_t sensor_mgr_init(void) {
    s_sensors[0] = temp_drv_get_interface();
    s_sensors[1] = humi_drv_get_interface();
    
    for (uint8_t i = 0U; i < SENSOR_COUNT; ++i) {
        if ((s_sensors[i] != NULL) && (s_sensors[i]->init != NULL)) {
            (void)s_sensors[i]->init();
        }
    }
    return STATUS_OK;
}

status_t sensor_mgr_read_all(sensor_data_t *p_data) {
    REQUIRE(p_data != NULL);
    
    if ((s_sensors[0] != NULL) && (s_sensors[0]->read != NULL)) {
        (void)s_sensors[0]->read(&p_data->temperature);
    }
    if ((s_sensors[1] != NULL) && (s_sensors[1]->read != NULL)) {
        (void)s_sensors[1]->read(&p_data->humidity);
    }
    
    p_data->timestamp = hal_timer_get_tick();
    return STATUS_OK;
}