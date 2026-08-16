#include "sensor.h"
#include <stdio.h>

void parse_sensor_data(const uint8_t *p_buffer, sensor_data_t *p_out_data)
{
    if ((p_buffer == NULL) || (p_out_data == NULL)) {
        return; 
    }

    p_out_data->temperature =
        ((uint16_t)p_buffer[0] << 8U) |
        ((uint16_t)p_buffer[1]);

    p_out_data->timestamp =
        ((uint32_t)p_buffer[2] << 24U) |
        ((uint32_t)p_buffer[3] << 16U) |
        ((uint32_t)p_buffer[4] << 8U)  |
        ((uint32_t)p_buffer[5]);
}

int main(void)
{
    uint8_t raw_buffer[6] = {0x01U, 0x2CU, 0x00U, 0x00U, 0x1AU, 0x0AU};

    sensor_data_t data;
    parse_sensor_data(raw_buffer, &data);

    printf("Temperature: %u\n", data.temperature);
    printf("Timestamp: %u\n", data.timestamp);

    return 0;
}
