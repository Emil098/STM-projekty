#include <math.h>
#include "TC74A0.h"

void TC74A0_init(I2C_HandleTypeDef *i2c_in){
	uint8_t data_to_send[2] = {TC_CONFIG_REG_ADDR,TC_CONFIG_REG_VAL};
	HAL_I2C_Init(i2c_in);
	HAL_I2C_Master_Transmit(i2c_in,TC74A0_ADDR,data_to_send,2,100);
	i2c = i2c_in;
}

tc TC74A0_read_temp(void){
	tc ret = {0};
	ret.status = STATUS_OK;
	uint8_t data_to_send = TC_TEMP_REG_ADDR,
			rec[2] = {0};

	data_to_send = TC_CONFIG_REG_ADDR;
	HAL_I2C_Master_Transmit(i2c,TC74A0_ADDR,&data_to_send,1,100);
	HAL_I2C_Master_Receive(i2c,TC74A0_ADDR,rec,1,100);

	if(rec[0] != 0x40){ // config reg normal value
		ret.status = STATUS_READ_CONF_ERR;
		return ret;
	}

	data_to_send = TC_TEMP_REG_ADDR;
	HAL_I2C_Master_Transmit(i2c,TC74A0_ADDR,&data_to_send,1,100);
	HAL_I2C_Master_Receive(i2c,TC74A0_ADDR,rec,2,100);

	if(rec[0] == 0xFF){ // config reg normal value
		ret.status = STATUS_READ_TEMP_ERR;
		return ret;
	}

	if((rec[0]>>7)&&1){ // temp under 0 degree
		rec[0] = ~ rec[0];
		ret.temp = rec[0];
	}
	ret.temp = rec[TC_TEMP_REG_ADDR];
	return ret;
}
