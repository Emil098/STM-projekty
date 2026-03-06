/*
 * int_flash.h
 *
 *  Created on: Feb 20, 2026
 *      Author: emilr
 */

#ifndef LIB_INT_FLASH_H_
#define LIB_INT_FLASH_H_

#include <stdint.h>


/**
 * @brief Internal Flash bank used to store settings.
 */
#define IFLASH_BANK                   FLASH_BANK_2

/**
 * @brief Flash sector number reserved for settings.
 */
#define IFLASH_SETTINGS_SECTOR        FLASH_SECTOR_23

/**
 * @brief Size of the settings Flash sector in bytes.
 *
 * Value provided by the linker script to ensure consistency between
 * firmware code and memory layout.
 */
#define IFLASH_SETTINGS_SECTOR_SIZE   (1024 * 128) /* 128 kB */

/**
 * @brief Base address of the settings Flash sector.
 *
 * This address is defined in the linker script and marks the beginning
 * of the Flash region dedicated to persistent configuration storage.
 */
#define IFLASH_SETTINGS_SECTOR_ADDR   (0x081E0000)

/**
 * @brief Return status for internal Flash operations.
 */
typedef enum {
  INT_FLASH_OK = 0,   /**< Operation succeeded */
  INT_FLASH_ERR       /**< Operation failed */
} flash_status;


void int_flash_disable_protection(void);
void int_flash_enable_hardware_protection(void);


void int_flash_read(uint32_t addr, uint8_t *buf, uint16_t len);

flash_status int_flash_write(uint32_t addr, uint8_t *buf, uint32_t len);

flash_status int_flash_erase(uint32_t addr);

uint32_t flash_calc_crc32(uint32_t addr, uint32_t len);

#endif /* LIB_INT_FLASH_H_ */
