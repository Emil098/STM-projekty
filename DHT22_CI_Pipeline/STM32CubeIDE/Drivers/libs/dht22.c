/*
 * dht22.c
 *
 *  Created on: Jan 4, 2026
 *      Author: emilr
 *
 * @brief DHT22 Temperature and Humidity Sensor Implementation
 *
 * This file implements the 1-Wire communication protocol for the DHT22
 * sensor. The implementation follows the timing specifications from the
 * DHT22 datasheet and uses the hardware abstraction layer for portability.
 *
 * Timing Specifications:
 * - Start signal: 18ms LOW, 20-40µs HIGH
 * - Response: 80µs LOW, 80µs HIGH
 * - Bit '0': 50µs LOW, 26-28µs HIGH
 * - Bit '1': 50µs LOW, 70µs HIGH
 * - Total frame: ~4ms
 *
 * Error Handling:
 * - All functions validate input parameters
 * - Communication timeouts prevent infinite blocking
 * - Checksum validation ensures data integrity
 * - Sensor readiness checking prevents protocol violations
 */

#include "dht22.h"
#include "dht22_hw.h"
#include <string.h>


/* Data packet size */
#define DHT22_DATA_BYTES             5    // 5 bytes: H, H, T, T, CS



/**
 * @brief Initialize a DHT22 sensor instance
 *
 * Prepares the sensor structure for communication. Must be called
 * before any other sensor operations.
 *
 * @param[in,out] sensor Pointer to dht22_t structure to initialize
 * @param[in] data_pin GPIO pin configuration for sensor data line
 * @retval DHT22_OK Initialization successful
 * @retval DHT22_ERROR_INVALID_DATA NULL sensor pointer provided
 *
 * @note The sensor requires 1-2 seconds to stabilize after power-on
 * @post Sensor is ready for communication (dht22_is_ready() returns true)
 */
dht22_status_t dht22_init(dht22_t* sensor, gpio_pin_t data_pin)
{
  if (sensor == NULL)
  {
    return DHT22_ERROR_INVALID_DATA;
  }

  hal_delay_init();

  sensor->data_pin = data_pin;
  sensor->initialized = true;

  // Set pin as input initially (idle state with pull-up)
  hal_gpio_set_mode_input(data_pin);

  return DHT22_OK;
}

/**
 * @brief Deinitialize a DHT22 sensor instance
 *
 * Resets the sensor structure to its default state. Should be called
 * when the sensor is no longer needed or before reinitialization.
 *
 * @param[in,out] sensor Pointer to dht22_t structure to deinitialize
 * @note Safe to call with NULL pointer (no operation performed)
 */
void dht22_deinit(dht22_t* sensor)
{
  if (sensor != NULL)
  {
    memset(sensor, 0, sizeof(dht22_t));
  }
}

/**
 * @brief Check if sensor is ready for operation
 *
 * Verifies that the sensor instance has been properly initialized
 * and is ready for communication.
 *
 * @param[in] sensor Pointer to dht22_t structure to check
 * @return true Sensor is initialized and ready
 * @return false Sensor is not initialized or pointer is NULL
 */
bool dht22_is_ready(dht22_t* sensor)
{
  if (sensor == NULL || !sensor->initialized)
  {
    return false;
  }
  return true;
}

/**
 * @brief Read temperature and humidity as floating-point values
 *
 * Performs the complete communication sequence with the sensor and
 * returns the measured values in human-readable format.
 *
 * @param[in] sensor Pointer to initialized dht22_t structure
 * @param[out] temperature Pointer to store temperature in Celsius (optional)
 * @param[out] humidity Pointer to store relative humidity in % (optional)
 * @retval DHT22_OK Read successful
 * @retval DHT22_ERROR_UNINITIALIZED Sensor not initialized
 * @retval DHT22_ERROR_NO_RESPONSE Sensor didn't respond
 * @retval DHT22_ERROR_CHECKSUM Data integrity check failed
 * @retval DHT22_ERROR_COMMUNICATION Protocol violation detected
 */
dht22_status_t dht22_read_temperature_humidity(dht22_t* sensor,
    float* temperature,
    float* humidity)
{
  dht22_data_t data;
  uint8_t raw_data[DHT22_DATA_BYTES];
  dht22_status_t status;

  if (sensor == NULL || !sensor->initialized)
  {
    return DHT22_ERROR_UNINITIALIZED;
  }

  // 1. Send start signal
  dht22_start_signal(sensor);

  // 2. Check response
  status = dht22_check_response(sensor);
  if (status != DHT22_OK)
  {
    return status;
  }

  // 3. Read 5 bytes of data
  raw_data[0] = dht22_read_byte(sensor);  // Humidity integral
  raw_data[1] = dht22_read_byte(sensor);  // Humidity decimal
  raw_data[2] = dht22_read_byte(sensor);  // Temperature integral
  raw_data[3] = dht22_read_byte(sensor);  // Temperature decimal
  raw_data[4] = dht22_read_byte(sensor);  // Checksum

  // 4. Validate checksum
  if (!dht22_validate_checksum(raw_data))
  {
    return DHT22_ERROR_CHECKSUM;
  }

  // 5. Parse data into structure
  data.humidity_integral = raw_data[0];
  data.humidity_decimal = raw_data[1];
  data.temperature_integral = raw_data[2];
  data.temperature_decimal = raw_data[3];
  data.checksum = raw_data[4];

  // 6. Convert to float (optional outputs)
  if (temperature != NULL)
  {
    /* DHT22 format: MSB of temperature_integral is sign bit */
    if (data.temperature_integral & 0x80)  /* Negative temperature */
    {
      /* Clear sign bit and combine with decimal */
      uint16_t temp_raw = ((data.temperature_integral & 0x7F) << 8) | data.temperature_decimal;
      *temperature = -(float)temp_raw / 10.0f;
    }
    else  /* Positive temperature */
    {
      uint16_t temp_raw = (data.temperature_integral << 8) | data.temperature_decimal;
      *temperature = (float)temp_raw / 10.0f;
    }
  }

  if (humidity != NULL)
  {
    *humidity = (float)((data.humidity_integral << 8) | data.humidity_decimal) / 10.0f;
  }

  return DHT22_OK;
}


/**
 * @brief Convert status code to human-readable string
 *
 * Provides a descriptive string representation of error codes
 * for debugging and logging purposes.
 *
 * @param[in] status Status code to convert
 * @return const char* Descriptive string representation
 */
const char* dht22_status_to_string(dht22_status_t status)
{
  switch (status)
  {
  case DHT22_OK:                return "OK";
  case DHT22_ERROR_TIMEOUT:     return "Timeout error";
  case DHT22_ERROR_CHECKSUM:    return "Checksum error";
  case DHT22_ERROR_NOT_READY:   return "Sensor not ready";
  case DHT22_ERROR_NO_RESPONSE: return "No response";
  case DHT22_ERROR_COMMUNICATION: return "Communication error";
  case DHT22_ERROR_INVALID_DATA: return "Invalid data";
  case DHT22_ERROR_UNINITIALIZED: return "Sensor uninitialized";
  default:                      return "Unknown error";
  }
}
