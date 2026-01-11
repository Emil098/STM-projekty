/*
 * dht22_types.h
 *
 *  Created on: Jan 5, 2026
 *      Author: emilr
 */

/**
 * @file dht22_types.h
 * @brief Type definitions for DHT22 sensor driver
 *
 * This header defines the data structures and enumerations used
 * throughout the DHT22 driver. Separating types from function
 * declarations improves modularity and testability.
 */

#ifndef DRIVERS_LIBS_DHT22_TYPES_H_
#define DRIVERS_LIBS_DHT22_TYPES_H_

#include "hal_delay_gpio.h"

/**
 * @brief DHT22 sensor instance structure
 *
 * Contains the GPIO pin configuration for the sensor
 */
typedef struct {
    gpio_pin_t data_pin;      /* GPIO pin connected to sensor data line */
    bool initialized;         /* Initialization status flag */
} dht22_t;

/**
 * @brief Sensor data structure
 *
 * Contains the parsed temperature and humidity values
 */
typedef struct {
    uint8_t humidity_integral;     /* Integral part of humidity (0-99%) */
    uint8_t humidity_decimal;      /* Decimal part of humidity (always 0 for DHT22) */
    uint8_t temperature_integral;  /* Integral part of temperature (0-50°C) */
    uint8_t temperature_decimal;   /* Decimal part of temperature (always 0 for DHT22) */
    uint8_t checksum;              /* Calculated checksum for validation */
} dht22_data_t;

/**
 * @brief Error codes for DHT22 operations
 */
typedef enum {
    DHT22_OK = 0,               /* Operation successful */
    DHT22_ERROR_TIMEOUT,        /* Communication timeout */
    DHT22_ERROR_CHECKSUM,       /* Checksum verification failed */
    DHT22_ERROR_NOT_READY,      /* Sensor not ready (1 second interval required) */
    DHT22_ERROR_NO_RESPONSE,    /* No response from sensor */
    DHT22_ERROR_COMMUNICATION,  /* Communication error */
    DHT22_ERROR_INVALID_DATA,   /* Invalid data received */
    DHT22_ERROR_UNINITIALIZED   /* Sensor not initialized */
} dht22_status_t;

#endif /* DRIVERS_LIBS_DHT22_TYPES_H_ */
