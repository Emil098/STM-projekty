/*
 * dht22.h
 *
 *  Created on: Jan 4, 2026
 *      Author: emilr
 */

#ifndef LIBS_DHT22_H_
#define LIBS_DHT22_H_

#include <stdint.h>
#include <stdbool.h>
#include "hal_delay_gpio.h"
#include "dht22_types.h"


/**
 * @file dht22.h
 * @brief Main interface for DHT22 Temperature and Humidity Sensor Driver
 *
 * @mainpage DHT22 Temperature and Humidity Sensor Driver
 *
 * This driver provides a high-level interface for reading temperature and
 * humidity data from the DHT22 sensor. The implementation is divided into:
 * - **High-level layer** (`dht22.c`): Public API and data processing
 * - **Hardware layer** (`dht22_hw.c`): 1-Wire protocol implementation
 *
 * @section features
 * - Temperature reading in Celsius (range: -40°C to 80°C)
 * - Humidity reading in percentage (range: 0% to 100%)
 * - Data validation with 8-bit checksum
 * - Comprehensive error handling and status reporting
 * - Hardware-independent design via HAL abstraction
 *
 * @section Usage Example
 * @code
 * dht22_t sensor;
 * float temperature, humidity;
 *
 * // Initialize sensor
 * if (dht22_init(&sensor, DHT22_PIN) == DHT22_OK) {
 *     // Read data
 *     if (dht22_read_temperature_humidity(&sensor, &temperature, &humidity) == DHT22_OK) {
 *         printf("Temperature: %.1f°C, Humidity: %.1f%%\n", temperature, humidity);
 *     }
 * }
 * @endcode
 *
 * @note Minimum interval between readings: 1 second (per DHT22 datasheet)
 * @note Requires precise timing (microsecond delays) for 1-Wire protocol
 * @note Thread-safe for single-sensor access
 */


/* Initialization and Configuration */
dht22_status_t dht22_init(dht22_t* sensor, gpio_pin_t data_pin);
void dht22_deinit(dht22_t* sensor);

/* Data Reading Function */
dht22_status_t dht22_read_temperature_humidity(dht22_t* sensor,
                                                float* temperature,
                                                float* humidity);
/* Utility Functions */
const char* dht22_status_to_string(dht22_status_t status);
bool dht22_is_ready(dht22_t* sensor);

#endif /* LIBS_DHT22_H_ */
