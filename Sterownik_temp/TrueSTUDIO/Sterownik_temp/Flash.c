//#include <math.h>
#include "Flash.h"

uint32_t Flash_Write_Data (uint32_t StartPageAddress, uint32_t *Data, uint16_t numberofwords)
{

	static FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t PAGEError;
	int sofar=0;

	  /* Unlock the Flash to enable the flash control register access *************/
	   HAL_FLASH_Unlock();

	   /* Erase the user Flash area*/

//	  uint32_t StartPage = 0x08004C90;
	  uint32_t StartPage = FLASH_START_ADDR;
//	  uint32_t EndPageAdress = StartPageAddress + numberofwords*4;
	  uint32_t EndPage = FLASH_END_ADDR;

	   /* Fill EraseInit structure*/
	   EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
	   EraseInitStruct.PageAddress = StartPage;
	   EraseInitStruct.NbPages     = ((EndPage - StartPage)/FLASH_PAGE_SIZE) +1;

	   if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
	   {
	     /*Error occurred while page erase.*/
		  return HAL_FLASH_GetError ();
	   }

	   /* Program the user Flash area word by word*/

	   while (sofar<numberofwords)
	   {
	     if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, StartPageAddress, Data[sofar]) == HAL_OK)
	     {
	    	 StartPageAddress += 4;  // use StartPageAddress += 2 for half word and 8 for double word
	    	 sofar++;
	     }
	     else
	     {
	       /* Error occurred while writing data in Flash memory*/
	    	 return HAL_FLASH_GetError ();
	     }
	   }

	   /* Lock the Flash to disable the flash control register access (recommended
	      to protect the FLASH memory against possible unwanted operation) *********/
	   HAL_FLASH_Lock();

	   return 0;
}

void Flash_Read_Data (uint32_t StartPageAddress, uint32_t *RxBuf, uint16_t numberofwords)
{
	while (1)
	{

		*RxBuf = *(__IO uint32_t *)StartPageAddress;
		StartPageAddress += 4;
		RxBuf++;
		if (!(numberofwords--)) break;
	}
}

void Flash_save_diff(differntial_vals diffs){

	uint32_t tab_to_save[6] = {diffs.turn_on_val[0],diffs.turn_on_val[1],diffs.turn_on_val[2],
			diffs.turn_12V_val[0],diffs.turn_12V_val[1],diffs.turn_12V_val[2]};

	Flash_Write_Data(FLASH_START_ADDR,tab_to_save,6);
}
differntial_vals Flash_read_diff(void){
	differntial_vals ret = {0};
	uint32_t tab_to_read[6];
	Flash_Read_Data(FLASH_START_ADDR,tab_to_read,6);

	ret.turn_on_val[0] = tab_to_read[0];
	ret.turn_on_val[1] = tab_to_read[1];
	ret.turn_on_val[2] = tab_to_read[2];
	ret.turn_12V_val[0] = tab_to_read[3];
	ret.turn_12V_val[1] = tab_to_read[4];
	ret.turn_12V_val[2] = tab_to_read[5];

	return ret;
}

