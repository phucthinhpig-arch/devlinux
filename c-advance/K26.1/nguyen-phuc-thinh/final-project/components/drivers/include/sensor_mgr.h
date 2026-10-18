#ifndef SENSOR_MGR_H
#define SENSOR_MGR_H

#include "common_types.h"

status_t sensor_mgr_init(void);
status_t sensor_mgr_read_all(sensor_data_t *p_data);

#endif /* SENSOR_MGR_H */