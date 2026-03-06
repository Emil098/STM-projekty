/*
 * settings.h
 *
 *  Created on: Feb 20, 2026
 *      Author: emilr
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "int_flash.h"

// Maximum length of stored username (without terminating null).
#define MAX_USER_NAME 20
// Maximum length of stored user password (without terminating null).
#define MAX_USER_PWD  20


#pragma pack(push, 1)

/**
 * @brief Persistent configuration structure stored in internal Flash.
 *
 * Layout:
 *  - CRC32 stored at beginning of structure
 *  - User credentials
 *  - Network configuration (IPv4)
 *
 * Notes:
 *  - Packed to 1-byte alignment to ensure deterministic Flash layout.
 *  - CRC is calculated over entire structure except the crc field itself.
 */
typedef struct{
  uint32_t crc;
  char user_name[MAX_USER_NAME + 1];
  char user_pwd[MAX_USER_PWD + 1];

  uint32_t IP;
  uint32_t mask;
  uint32_t gate;
} Flash_settings;

#pragma pack(pop)

flash_status settings_init(Flash_settings *settings);
void get_settings(Flash_settings *settings);
flash_status restore_default_settings();
void get_default_settings(Flash_settings *settings);
flash_status flash_update_settings(Flash_settings *new_settings);
flash_status flash_read_settings(void);
flash_status flash_write_settings(void);

#endif /* SETTINGS_H_ */
