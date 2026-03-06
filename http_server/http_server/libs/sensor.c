/*
 * dht22_handler.c
 * @brief DHT22 sensor integration wrapper (application-level adapter).
 *
 * This module wraps an external DHT22 driver library and exposes a minimal API
 * used by the REST layer:
 *  - init_sensor()   : initialize the DHT22 driver
 *  - update_sensor() : read and cache latest measurements
 *  - get_sensor()    : return the last cached values (non-blocking)
 *
 * Design notes:
 *  - Sensor reads are performed explicitly by calling update_sensor().
 *  - The most recent valid readings are stored in static variables and can be
 *    fetched at any time using get_sensor().
 *  - This keeps REST endpoints fast and deterministic (no sensor I/O inside HTTP path).
 *
 * Dependencies:
 *  - dht22 driver library (external module)
 *  - FreeRTOS is included for project consistency (module itself is task-agnostic)
 *
 *  Created on: Feb 20, 2026
 *      Author: emilr
 */

#include "dht22.h"
#include "console.h"

#include "FreeRTOS.h"
#include "task.h"

#define ONE_W_Pin GPIO_PIN_2
#define ONE_W_GPIO_Port GPIOG

/* External driver instance and pin configuration */
dht22_t sensor;
const gpio_pin_t DHT22_PIN = {ONE_W_GPIO_Port, ONE_W_Pin};

/* Cached measurements (updated only on successful read) */
static float temp_f, hum_f;

/* Last driver status (for debugging/diagnostics) */
static dht22_status_t status = DHT22_ERROR_NOT_READY;

/**
 * @brief Initialize DHT22 sensor driver instance.
 *
 * Must be called once during system startup before update_sensor().
 * Configures the GPIO pin and prepares the driver state.
 */
void init_sensor()
{
  dht22_init(&sensor, DHT22_PIN);
}

/**
 * @brief Perform a sensor read and update cached values.
 *
 * Reads temperature and humidity using the external DHT22 driver.
 * On success:
 *  - updates internal cached values (temp_f, hum_f)
 * On failure:
 *  - keeps previous cached values unchanged
 *  - prints diagnostic information to console
 *
 * @note
 * This function is intended to be called periodically (e.g. from a FreeRTOS task)
 * to keep cached values fresh without blocking the HTTP/REST path.
 */
void update_sensor()
{
  float temp, hum;
  // Read values (similar to original get_dht22_values())
  status = dht22_read_temperature_humidity(&sensor, &temp, &hum);

  if (status == DHT22_OK) {
    // Cache the most recent valid measurement.
    temp_f = temp;
    hum_f = hum;
  } else {
    // Keep last valid cached values; report current error status.
    conpln("Error: %s\n", dht22_status_to_string(status));
  }

}

/**
 * @brief Get last cached sensor readings.
 *
 * @param[out] temp Pointer to receive temperature (°C).
 * @param[out] hum  Pointer to receive humidity (%RH).
 *
 * @note
 * This is a non-blocking accessor. It does not communicate with the sensor.
 * Call update_sensor() periodically to refresh the cached values.
 */
void get_sensor(float *temp, float *hum)
{
  *temp = temp_f;
  *hum = hum_f;
}
