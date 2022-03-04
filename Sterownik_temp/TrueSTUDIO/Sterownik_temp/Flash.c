//#include <math.h>
#include "Flash.h"

uint32_t Flash_Write_Data (uint32_t StartPageAddress, uint32_t *Data, uint16_t numberofwords)
{

	static FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t PAGEError;
	int sofar=0;

	// odblokowanie pami�ci flash
	HAL_FLASH_Unlock();

	// przypisanie adresu pocz�tku i ko�ca strony
//	  uint32_t StartPage = 0x08004C90;
	  uint32_t StartPage = FLASH_START_ADDR;
//	  uint32_t EndPageAdress = StartPageAddress + numberofwords*4;
	  uint32_t EndPage = FLASH_END_ADDR;

//	   ustawienie struktury do wyczyszczenia strony przed zapisem
	   EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
	   EraseInitStruct.PageAddress = StartPage;
	   EraseInitStruct.NbPages     = ((EndPage - StartPage)/FLASH_PAGE_SIZE) +1;

	   // wyczyszczenie strony
	   if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
	   {
		  return HAL_FLASH_GetError ();
	   }


	   // nadpisywanie kolejnych kom�rek pami�ci do momentu a� wszystkie dane z Data zostan� przekazane
	   while (sofar<numberofwords)
	   {
	     if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, StartPageAddress, Data[sofar]) == HAL_OK)
	     {
	    	 // pe�ne s�owo to 4 bajty
	    	 StartPageAddress += 4;
	    	 sofar++;
	     }
	     else
	     {
	    	 // na wypadek wyst�pienia b��du podczas zapisu
	    	 return HAL_FLASH_GetError ();
	     }
	   }

	   // po zapisaniu wszystkich danych ponowne zablokowanie pami�ci flash
	   HAL_FLASH_Lock();

	   return 0;
}

void Flash_Read_Data (uint32_t StartPageAddress, uint32_t *RxBuf, uint16_t numberofwords)
{
	while (1)
	{
		// bezpo�redni odczyt pojedynczego s�owa z pami�ci flash i zapisanie do bufora
		*RxBuf = *(__IO uint32_t *)StartPageAddress;
		// zwi�kszenie adresu o pe�ne s�owo
		StartPageAddress += 4;
		// zwi�kszenie znacznika zapisywanego bufora
		RxBuf++;
		// gdy wszystkie dane zostan� odczytane nast�puje wyj�cie z p�tli
		if (!(numberofwords--)) break;
	}
}

void Flash_save_diff(differntial_vals diffs){
	// funkcja zapisuj�ca wszystkie r�nice temp. do pami�ci flash

	// przekazanie danych ze struktury do tablicy
	uint32_t tab_to_save[6] = {diffs.turn_on_val[0],diffs.turn_on_val[1],diffs.turn_on_val[2],
			diffs.turn_12V_val[0],diffs.turn_12V_val[1],diffs.turn_12V_val[2]};
	// zapis danych
	Flash_Write_Data(FLASH_START_ADDR,tab_to_save,6);
}
differntial_vals Flash_read_diff(void){
	// funkcja odczytuj�ca z flash-a zapisane r�nice temp.

	differntial_vals ret = {0};
	uint32_t tab_to_read[6];
	//odczyt danych w postaci tablicy
	Flash_Read_Data(FLASH_START_ADDR,tab_to_read,6);

	// zapisanie danych do strutury r�nic temp.
	ret.turn_on_val[0] = tab_to_read[0];
	ret.turn_on_val[1] = tab_to_read[1];
	ret.turn_on_val[2] = tab_to_read[2];
	ret.turn_12V_val[0] = tab_to_read[3];
	ret.turn_12V_val[1] = tab_to_read[4];
	ret.turn_12V_val[2] = tab_to_read[5];

	return ret;
}

