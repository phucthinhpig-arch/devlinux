#ifndef SENSOR_INTF_H
#define SENSOR_INTF_H

#include "common_types.h"

typedef struct {
    status_t    (*init)(void);
    status_t    (*read)(int16_t *p_value);
    const char* (*get_name)(void);
} sensor_intf_t;

#endif /* SENSOR_INTF_H */