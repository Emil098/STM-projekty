/*
 * settings.c
 * @brief Persistent device settings stored in internal Flash.
 *
 * Responsibilities:
 *  - Store configuration structure in Flash
 *  - Protect access with FreeRTOS mutex
 *  - Validate integrity using CRC32
 *  - Restore defaults when CRC mismatch
 *
 * Thread safety:
 *  All Flash read/write operations are protected by SettingsMutex.
 *
 * Storage:
 *  Settings are stored in dedicated Flash sector defined by:
 *    IFLASH_SETTINGS_SECTOR / IFLASH_SETTINGS_SECTOR_ADDR
 *
 *  Created on: Feb 20, 2026
 *      Author: emilr
 */
#include "settings.h"

#include <string.h>
#include "console.h"
#include "crc.h"
#include "ip_format.h"

#include "FreeRTOS.h"
#include "semphr.h"


SemaphoreHandle_t SettingsMutex;

Flash_settings Settings;

/**
 * @brief Initialize settings module.
 *
 * - Creates static mutex
 * - Reads settings from Flash
 * - Copies loaded settings to user structure
 *
 * @param settings  Pointer to output structure
 *
 * @return INT_FLASH_OK if valid settings loaded
 * @return INT_FLASH_ERR if CRC mismatch (defaults restored)
 */
flash_status settings_init(Flash_settings *settings)
{
  conpln("Init Settings...");

  static StaticSemaphore_t SettingsMutexBuffer;
  SettingsMutex = xSemaphoreCreateMutexStatic(&SettingsMutexBuffer);

  flash_status status = flash_read_settings();
  memcpy(settings, &Settings, sizeof(Flash_settings));

  return status;
}

/**
 * @brief Get current runtime settings.
 *
 * @param settings  Output buffer
 *
 * Copies internal Settings structure to caller.
 */
void get_settings(Flash_settings *settings)
{
  memcpy(settings, &Settings, sizeof(Flash_settings));
}

/**
 * @brief Restore factory default settings and save to Flash.
 *
 * @return Flash write status
 */
flash_status restore_default_settings()
{
  get_default_settings(&Settings);

  return flash_write_settings();
}

/**
 * @brief Populate structure with factory default values.
 *
 * @param settings  Structure to initialize
 *
 * Default values include:
 *  - admin credentials
 *  - default IP configuration
 */
void get_default_settings(Flash_settings *settings)
{
  strcpy(settings->user_name, "admin");
  strcpy(settings->user_pwd , "admin01");

  settings->IP    = IPv4TOU32(192,168,1,242);
  settings->mask  = IPv4TOU32(255,255,255,0);
  settings->gate  = IPv4TOU32(192,168,1,1);

}

/**
 * @brief Read settings from Flash and verify integrity.
 *
 * @return INT_FLASH_OK if CRC valid
 * @return INT_FLASH_ERR if CRC mismatch or mutex timeout
 *
 * If CRC mismatch occurs:
 *  - Default settings are restored in RAM
 *  - Flash is NOT automatically overwritten
 */
flash_status flash_read_settings(void)
{
  flash_status status = INT_FLASH_OK;

  if (xSemaphoreTake(SettingsMutex, 1000) != pdTRUE) {
    conpln_fn("SettingsMutex busy");
    return INT_FLASH_ERR;
  }

  int_flash_read(IFLASH_SETTINGS_SECTOR_ADDR, (uint8_t*)&Settings, sizeof(Settings));
  if (Settings.crc != calc_crc32((uint8_t*)&Settings + sizeof(Settings.crc), sizeof(Settings)-sizeof(Settings.crc))) {
    conpln("Settings CRC mismatch. Restoring defaults...");
    get_default_settings(&Settings);
    status = INT_FLASH_ERR;
  } else
    conpln(GREEN "Settings loaded from flash" CLRST);

  xSemaphoreGive(SettingsMutex);

  return status;
}

/**
 * @brief Update runtime settings structure (RAM only).
 *
 * @param new_settings  Pointer to new configuration
 *
 * Does NOT write to Flash.
 * Use flash_write_settings() to persist changes.
 */
flash_status flash_update_settings(Flash_settings *new_settings)
{
  if(new_settings != NULL)
    memcpy(&Settings, new_settings, sizeof(Flash_settings));

  return INT_FLASH_OK;
}

/**
 * @brief Save current settings to Flash.
 *
 * Process:
 *  - Calculate CRC32
 *  - Unlock Flash
 *  - Erase target sector
 *  - Program new data
 *  - Lock Flash
 *
 * @return INT_FLASH_OK on success
 * @return INT_FLASH_ERR on failure
 *
 * Protected by FreeRTOS mutex.
 */
flash_status flash_write_settings(void)
{
  flash_status status;

  if (xSemaphoreTake(SettingsMutex, 1000) != pdTRUE) {
    conpln_fn("SettingsMutex busy");
    return INT_FLASH_ERR;
  }

  Settings.crc = calc_crc32((uint8_t*)&Settings + sizeof(Settings.crc), sizeof(Settings) - sizeof(Settings.crc));
  int_flash_disable_protection();

  status = int_flash_erase(IFLASH_SETTINGS_SECTOR);
  if(status == INT_FLASH_ERR)
  {
    int_flash_enable_hardware_protection();
    return status;
  }

  status = int_flash_write(IFLASH_SETTINGS_SECTOR_ADDR, (uint8_t*)&Settings, sizeof(Settings));
  int_flash_enable_hardware_protection();

  xSemaphoreGive(SettingsMutex);
  conpln(GREEN "Settings saved to flash" CLRST);

  return status;
}
