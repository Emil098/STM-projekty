#ifndef _FLASH_H
#define _FLASH_H

#include "stm32f3xx.h"
#include "D:\stm projekty\Sterownik_temp\TrueSTUDIO\Sterownik_temp\disp_irq.h"

#define FLASH_START_ADDR 0x08010000 // adres pocz¹tku strony
#define FLASH_END_ADDR 0x08010030  // adres koñca zapisu danych o ró¿nicy temp.
#define FLASH_END_PAGE 0x080107FF  // adres koñca strony


// uniwersalna funkcja odczytuj¹ca dane bezpoœrednio z flash-a
void Flash_Read_Data (uint32_t StartPageAddress, uint32_t *RxBuf, uint16_t numberofwords);

// uniwersalna funkcja zapisuj¹ca dane w postaci tablicy do flash-a
uint32_t Flash_Write_Data (uint32_t StartPageAddress, uint32_t *Data, uint16_t numberofwords);

// funkcja zapisuj¹ca wszystkie ró¿nice temp. do pamiêci flash
void Flash_save_diff(differntial_vals diffs);

// funkcja odczytuj¹ca z flash-a zapisane ró¿nice temp.
differntial_vals Flash_read_diff(void);


#endif

