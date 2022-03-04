#ifndef _FLASH_H
#define _FLASH_H

#include "stm32f3xx.h"
#include "D:\stm projekty\Sterownik_temp\TrueSTUDIO\Sterownik_temp\disp_irq.h"

#define FLASH_START_ADDR 0x08010000 // adres pocz�tku strony
#define FLASH_END_ADDR 0x08010030  // adres ko�ca zapisu danych o r�nicy temp.
#define FLASH_END_PAGE 0x080107FF  // adres ko�ca strony


// uniwersalna funkcja odczytuj�ca dane bezpo�rednio z flash-a
void Flash_Read_Data (uint32_t StartPageAddress, uint32_t *RxBuf, uint16_t numberofwords);

// uniwersalna funkcja zapisuj�ca dane w postaci tablicy do flash-a
uint32_t Flash_Write_Data (uint32_t StartPageAddress, uint32_t *Data, uint16_t numberofwords);

// funkcja zapisuj�ca wszystkie r�nice temp. do pami�ci flash
void Flash_save_diff(differntial_vals diffs);

// funkcja odczytuj�ca z flash-a zapisane r�nice temp.
differntial_vals Flash_read_diff(void);


#endif

