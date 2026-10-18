#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <stdint.h>
#include <stdbool.h>

/** @brief System FSM states. */
typedef enum {
    SYS_INIT = 0U,
    SYS_IDLE,
    SYS_MONITORING,
    SYS_ALERT,
    SYS_ERROR,
    SYS_NUM_STATES
} system_state_t;

/** @brief Supported sensor types. */
typedef enum {
    SENSOR_TEMPERATURE = 0U,
    SENSOR_HUMIDITY,
    SENSOR_NUM_TYPES
} sensor_type_t;

/** @brief Common return status codes. */
typedef enum {
    STATUS_OK = 0U,
    STATUS_ERR_NULL_PTR,
    STATUS_ERR_INVALID_PARAM,
    STATUS_ERR_TIMEOUT,
    STATUS_ERR_FULL,
    STATUS_ERR_EMPTY
} status_t;

/** @brief Sensor reading data (fixed-point: value * 10, e.g., 251 = 25.1 degrees). */
typedef struct {
    int16_t  temperature;  /**< Temperature in tenths of degree C (Q7.1). */
    int16_t  humidity;     /**< Humidity in tenths of percent (Q7.1).     */
    uint32_t timestamp;    /**< Tick count when reading was taken.        */
} sensor_data_t;

#endif /* COMMON_TYPES_H */