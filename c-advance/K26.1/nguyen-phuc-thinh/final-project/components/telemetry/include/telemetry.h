#ifndef TELEMETRY_H
#define TELEMETRY_H

#include "common_types.h"

#define TLV_SYNC_BYTE      (0xAAU)
#define TLV_MAX_PACKET_SIZE (8U)

status_t telemetry_serialize(const sensor_data_t *p_data, uint8_t *p_buf, uint32_t buf_size, uint32_t *p_written);
status_t telemetry_deserialize(const uint8_t *p_buf, uint32_t buf_len, sensor_data_t *p_data);
uint8_t  telemetry_crc8(const uint8_t *p_data, uint32_t len);

#endif /* TELEMETRY_H */