#include "humi_drv.h"
#include <stdint.h>
#include <stdbool.h>
#include "common_types.h"
#include "hal_sim.h"
#include "contract.h"
#include <stddef.h>

typedef struct {
    bool    is_initialized;
    int16_t offset_cal;
} humi_drv_config_t;

static humi_drv_config_t s_config = { .is_initialized = false, .offset_cal = 0 };

static status_t humi_init(void) {
    s_config.is_initialized = true;
    return STATUS_OK;
}

static status_t humi_read(int16_t *p_value) {
    REQUIRE(p_value != NULL);
    if (!s_config.is_initialized) { return STATUS_ERR_INVALID_PARAM; }
    
    uint16_t raw = hal_sim_read_register(HAL_REG_HUMI_RAW);
    *p_value = (int16_t)((int16_t)raw + s_config.offset_cal);
    return STATUS_OK;
}

static const char* humi_get_name(void) { return "Humidity"; }

static const sensor_intf_t s_humi_intf = {
    .init     = humi_init,
    .read     = humi_read,
    .get_name = humi_get_name,
};

const sensor_intf_t* humi_drv_get_interface(void) {
    return &s_humi_intf;
}