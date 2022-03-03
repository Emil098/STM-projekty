#ifndef _TC74A0_H
#define _TC74A0_H

#include "stm32f3xx.h"

#define TC74A0_ADDR 0b10010000
#define TC_CONFIG_REG_ADDR 0x01
#define TC_CONFIG_REG_VAL 0x00 // turn-off standbay mode
#define TC_TEMP_REG_ADDR 0x00

#define STATUS_OK 1
#define STATUS_READ_TEMP_ERR 2
#define STATUS_READ_CONF_ERR 3

I2C_HandleTypeDef *i2c;

typedef struct{
	float temp;
	uint8_t status;
}tc;

void TC74A0_init(I2C_HandleTypeDef *i2c_in);

tc TC74A0_read_temp(void);

#endif
