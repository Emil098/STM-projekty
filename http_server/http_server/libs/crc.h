/*
 * crc.h
 *
 *  Created on: 9 lut 2016
 *      Author: admin
 */

#ifndef APPLICATION_USER_LIB_CRC_H_
#define APPLICATION_USER_LIB_CRC_H_

#include <stdint.h>

#define CRC_START_VALUE  0x00000000

uint32_t crc32(uint32_t crc, const void *buf, uint32_t size);

uint32_t crc32_byte(uint32_t crc, uint8_t d);

uint32_t calc_crc32(uint8_t *data, uint32_t len);

#endif /* APPLICATION_USER_LIB_CRC_H_ */
