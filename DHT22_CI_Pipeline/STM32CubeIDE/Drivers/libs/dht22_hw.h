/*
 * dht22_hw.h
 *
 *  Created on: Jan 5, 2026
 *      Author: emilr
 */

/**
 * @file dht22_hw.h
 * @brief Hardware Abstraction Layer for DHT22 1-Wire Protocol
 *
 * @note This header is part of the internal implementation and should not
 *       be included directly by application code. Use dht22.h instead.
 *
 * This module implements the low-level 1-Wire communication protocol
 * for the DHT22 sensor. It handles the precise timing requirements
 * specified in the DHT22 datasheet.
 *
 * @section timing Timing Specifications
 * - Start signal: 18ms LOW, 20-40µs HIGH
 * - Response: 80µs LOW, 80µs HIGH
 * - Bit '0': 50µs LOW, 26-28µs HIGH
 * - Bit '1': 50µs LOW, 70µs HIGH
 * - Total frame: ~4ms
 *
 * @section testing Unit Testing
 * This layer can be mocked during unit tests of the high-level layer
 * (dht22.c) to enable hardware-independent testing.
 */

#ifndef DRIVERS_LIBS_DHT22_HW_H_
#define DRIVERS_LIBS_DHT22_HW_H_

#include <stdint.h>
#include <stdbool.h>
#include "dht22_types.h"


void dht22_start_signal(dht22_t* sensor);
dht22_status_t dht22_check_response(dht22_t* sensor);
uint8_t dht22_read_byte(dht22_t* sensor);
bool dht22_validate_checksum(const uint8_t* data);


#endif /* DRIVERS_LIBS_DHT22_HW_H_ */
