/*
 * dht22_hw.c
 *
 *  Created on: Jan 5, 2026
 *      Author: emilr
 */

/**
 * @file dht22_hw.c
 * @brief Hardware layer implementation for DHT22 1-Wire protocol
 *
 * @ingroup dht22_hw_internal
 *
 * This module implements the precise timing requirements of the
 * DHT22 1-Wire communication protocol. All timing values are
 * based on the DHT22 datasheet specifications.
 *
 * @section timing_constants Timing Constants
 * Constant               | Value  | Description
 * ----------------------|--------|-----------------------------
 * DHT22_START_DELAY_MS  | 18 ms  | Start signal low time
 * DHT22_RESPONSE_DELAY_US| 40 µs | Delay before checking response
 * DHT22_RESPONSE_TIMEOUT_US| 80 µs| Timeout for sensor response
 * DHT22_BIT_READ_DELAY_US | 40 µs| Delay before sampling bit value
 * DHT22_MIN_READ_INTERVAL_MS| 1000 ms| Minimum time between reads
 *
 * @section error_handling Error Handling
 * - All blocking loops have implicit timeouts via HAL functions
 * - Invalid parameters are validated at runtime
 * - Communication errors return appropriate status codes
 */

#include "dht22_hw.h"
#include <string.h>

/* Private constants */
#define DHT22_START_DELAY_MS         18   // 18ms low signal for start
#define DHT22_RESPONSE_DELAY_US      40   // 40us delay before checking response
#define DHT22_RESPONSE_TIMEOUT_US    80   // 80us timeout for response
#define DHT22_BIT_READ_DELAY_US      40   // 40us delay for bit reading
#define DHT22_BIT_THRESHOLD_US       40   // Threshold for 0/1 bit (40us)
#define DHT22_MIN_READ_INTERVAL_MS   1000 // Minimum 1 second between reads


/**
 * @brief Send start signal to DHT22 sensor
 *
 * Initiates communication by pulling the data line low for 18ms,
 * then releasing it to allow the sensor to respond.
 *
 * @param[in] sensor Pointer to initialized sensor instance
 * @note This function is blocking for 18ms
 */
void dht22_start_signal(dht22_t* sensor)
{

  if(sensor == NULL || sensor->initialized == 0)
    return;
    
  // 1. Set pin as output
  hal_gpio_set_mode_output(sensor->data_pin);

  // 2. Pull the pin low
  hal_gpio_write(sensor->data_pin, false);

  // 3. Wait for 18ms
  hal_delay_ms(DHT22_START_DELAY_MS);

  // 4. Set as input
  hal_gpio_set_mode_input(sensor->data_pin);
}

/**
 * @brief Check for sensor response after start signal
 *
 * Waits for the sensor to acknowledge the start signal by pulling
 * the line low for 80µs, then high for 80µs.
 *
 * @param[in] sensor Pointer to initialized sensor instance
 * @retval DHT22_OK Response received correctly
 * @retval DHT22_ERROR_NO_RESPONSE Sensor didn't respond
 * @note Implements timeout detection to prevent infinite blocking
 */
dht22_status_t dht22_check_response(dht22_t* sensor)
{
  
  if(sensor == NULL)
    return DHT22_ERROR_UNINITIALIZED;
    
  // Wait 40us
  hal_delay_us(DHT22_RESPONSE_DELAY_US);

  // Check if pin is low
  if (!hal_gpio_read(sensor->data_pin))
  {
    // Wait 80us
    hal_delay_us(DHT22_RESPONSE_TIMEOUT_US);

    // Check if pin is now high
    if (!hal_gpio_read(sensor->data_pin))
      return DHT22_ERROR_NO_RESPONSE; // pin is low (missing response)

    // Response OK
    // Wait for pin to go low
    while (hal_gpio_read(sensor->data_pin));

    return DHT22_OK;
  }

  return DHT22_ERROR_NO_RESPONSE; // Pin wasn't low after 40us - no response
}

/**
 * @brief Read single byte (8 bits) from sensor
 *
 * @param[in] sensor Pointer to initialized sensor instance
 * @return uint8_t 8-bit value read from sensor
 *
 * @implnote Bit timing:
 * - Each bit starts with 50µs low
 * - '0' bit: 26-28µs high
 * - '1' bit: 70µs high
 * - Sampling at 40µs distinguishes between them
 */
uint8_t dht22_read_byte(dht22_t* sensor)
{
  uint8_t value = 0;

  if(sensor == NULL)
    return 0;

  for (uint8_t i = 0; i < 8; i++)
  {
    // Wait for pin to go high (start of bit)
    while (!hal_gpio_read(sensor->data_pin));

    // Wait for 40us
    hal_delay_us(DHT22_BIT_READ_DELAY_US);

    // Read pin state after 40us
    if (!hal_gpio_read(sensor->data_pin))
    {
      // Pin is low after 40us -> bit '0'
      // Clear the bit (already 0 from initialization)
    }
    else
    {
      // Pin is high after 40us -> bit '1'
      value |= (uint8_t)(1 << (7 - i));
    }

    // Wait for pin to go low (end of bit)
    while (hal_gpio_read(sensor->data_pin));
  }

  return value;
}

/**
 * @brief Validate checksum of received data
 *
 * Calculates the 8-bit sum of the first 4 data bytes and compares
 * it with the 5th byte (checksum).
 *
 * @param[in] data Array of 5 bytes from sensor
 * @return true Checksum valid (data integrity confirmed)
 * @return false Checksum invalid (data corrupted)
 */
bool dht22_validate_checksum(const uint8_t* data)
{
  if (data == NULL)
  {
    return false;
  }

  uint8_t sum = 0;
  for (int i = 0; i < 4; i++)
  {
    sum = (data[i] + sum) & 0xFF;
  }

  return (sum == data[4]);
}
