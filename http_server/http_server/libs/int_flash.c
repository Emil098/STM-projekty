/*
 * int_flash.c
 * @brief Internal Flash memory driver (STM32 HAL based).
 *
 * This module provides a thin abstraction layer over STM32 internal Flash.
 * It implements:
 *  - Flash unlock/lock control
 *  - Sector erase
 *  - Halfword programming
 *  - Raw read access
 *  - CRC32 calculation over Flash region
 *
 * Notes:
 *  - Write length must be aligned to 16-bit (halfword).
 *  - Erase must be executed before write.
 *  - Caller is responsible for address validity and sector mapping.
 *
 *  Created on: Feb 20, 2026
 *      Author: emilr
 */

#include <string.h>
#include "main.h"
#include "console.h"
#include "crc.h"
#include "int_flash.h"


/**
 * @brief Unlock Flash control register access.
 *
 * Must be called before erase or program operations.
 */
void int_flash_disable_protection(void)
{
  HAL_FLASH_Unlock();
}

/**
 * @brief Lock Flash control register.
 *
 * Should be called after finishing erase/program operations
 * to prevent accidental writes.
 */
void int_flash_enable_hardware_protection(void)
{
  HAL_FLASH_Lock();
}

/**
 * @brief Erase single Flash sector.
 *
 * @param addr  Flash sector number (NOT raw address).
 *
 * @return INT_FLASH_OK on success
 * @return INT_FLASH_ERR on failure
 *
 * Note:
 * Wrapper for internal erase implementation.
 */
static flash_status int_flash_erase_internal(uint32_t sector)
{
  HAL_StatusTypeDef hst;
  FLASH_EraseInitTypeDef einit;
  uint32_t berr;

  einit.TypeErase   = FLASH_TYPEERASE_SECTORS;
  einit.Banks       = IFLASH_BANK;
  einit.Sector      = sector;
  einit.NbSectors   = 1;

  if ((hst = HAL_FLASHEx_Erase(&einit, &berr)) != HAL_OK) {
    conpln("HAL_FLASHEx_Erase returned %d, berr = %d", hst, berr);
    return INT_FLASH_ERR;
  }

  return INT_FLASH_OK;
}

flash_status int_flash_erase(uint32_t addr)
{
  return int_flash_erase_internal(addr);
}

/**
 * @brief Read data from Flash memory.
 *
 * @param addr  Flash memory address
 * @param buf   Destination buffer
 * @param len   Number of bytes to read
 *
 * Flash memory is memory-mapped, so read is simple memcpy.
 */
void int_flash_read(uint32_t addr, uint8_t *buf, uint16_t len)
{
  memcpy(buf, (uint8_t*)addr, len);
}

/**
 * @brief Program data into Flash memory.
 *
 * @param addr  Flash memory address (must be halfword aligned)
 * @param buf   Source buffer
 * @param len   Length in bytes (must be multiple of 2)
 *
 * @return INT_FLASH_OK on success
 * @return INT_FLASH_ERR on failure
 *
 * Notes:
 *  - Data is written in 16-bit (halfword) units.
 *  - Flash must be erased before programming.
 *  - Length must be divisible by 2.
 */
flash_status int_flash_write(uint32_t addr, uint8_t *buf, uint32_t len)
{
  int a;
  HAL_StatusTypeDef hst;

  if (len % 2 != 0) {
    conpln("len must be a multiple of 2");
    return INT_FLASH_ERR;
  }

  for (a = 0; a < len / 2; a++) {
    if ((hst = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, addr + a*2, *(uint16_t*)(buf + a*2))) != HAL_OK) {
       conpln("HAL_FLASH_Program returned %d", hst);
       return INT_FLASH_ERR;
    }
  }

  return INT_FLASH_OK;
}

/**
 * @brief Calculate CRC32 over Flash memory region.
 *
 * @param addr  Start address
 * @param len   Number of bytes
 *
 * @return Calculated CRC32 value
 *
 * Data is processed in 256-byte chunks to limit stack usage.
 */
uint32_t flash_calc_crc32(uint32_t addr, uint32_t len)
{
  uint32_t crc = CRC_START_VALUE;
  uint8_t page[256];

  while (len > 0) {
    uint32_t lr = 256;
    if (len < lr) lr = len;
    int_flash_read(addr, page, lr);
    crc = crc32(crc, page, lr);
    len -= lr;
    addr += lr;
  }
  return crc;
}

