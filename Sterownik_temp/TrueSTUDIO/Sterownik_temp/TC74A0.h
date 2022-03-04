#ifndef _TC74A0_H
#define _TC74A0_H

// strona z datasheet-em
// https://ww1.microchip.com/downloads/en/DeviceDoc/21462D.pdf

#include "stm32f3xx.h"

#define TC74A0_ADDR 0b10010000
#define TC_CONFIG_REG_ADDR 0x01
#define TC_CONFIG_REG_VAL 0x00 // turn-off standbay mode
#define TC_TEMP_REG_ADDR 0x00  // strona 8 datasheet bit 7 = 0 w³¹czenie normalnego trybu pracy

// makra okreœlaj¹ce stan odczytu danych z tc74a0
#define STATUS_OK 1
#define STATUS_READ_TEMP_ERR 2
#define STATUS_READ_CONF_ERR 3

I2C_HandleTypeDef *i2c; // zainicjowanie strukry i2c dla biblioteki

typedef struct{
	float temp;
	uint8_t status;
}tc;  // info o odczycie z tc74a0


// funkcja inicjuj¹ca rejestr config tc74a0
void TC74A0_init(I2C_HandleTypeDef *i2c_in);

// funkcja odczytuj¹ca temp z tc74a0
tc TC74A0_read_temp(void);

#endif
