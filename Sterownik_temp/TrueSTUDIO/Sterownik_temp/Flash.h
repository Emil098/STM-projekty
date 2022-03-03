#ifndef _FLASH_H
#define _FLASH_H

#include "stm32f3xx.h"
#include "D:\stm projekty\Sterownik_temp\TrueSTUDIO\Sterownik_temp\disp_irq.h"

#define FLASH_START_ADDR 0x08010000
#define FLASH_END_ADDR 0x08010030
#define FLASH_END_PAGE 0x080107FF


void Flash_Read_Data (uint32_t StartPageAddress, uint32_t *RxBuf, uint16_t numberofwords);
uint32_t Flash_Write_Data (uint32_t StartPageAddress, uint32_t *Data, uint16_t numberofwords);
void Flash_save_diff(differntial_vals diffs);
differntial_vals Flash_read_diff(void);

#endif

