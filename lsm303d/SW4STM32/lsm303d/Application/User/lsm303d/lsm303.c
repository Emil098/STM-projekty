/*
 * lsm303.c
 *
 *  Created on: 14 maj 2018
 *      Author: Dv8
 */
#include "lsm303.h"

void lsm_write_1(uint8_t reg,uint8_t val){
	HAL_I2C_Mem_Write(&i2c_port,lsmWriteAddr,reg,1,&val,1,1000);
}
uint8_t lsm_read_1(uint8_t reg){
	uint8_t a;
	HAL_I2C_Mem_Read(&i2c_port,lsmReadAddr,reg,1,&a,1,1000);
	return a;
}
uint16_t lsm_read_2(uint8_t reg){
	uint8_t a1,a2;
	a1 = lsm_read_1(reg);
	reg++;
	a2 = lsm_read_1(reg);
	uint16_t a=(a2<<8)|a1;
	return a;
}
int16_t lsm_read_value(uint8_t reg){
	int16_t val=0;
	val = lsm_read_2(reg);
	return val;
}
int lsm_getAcc_x(void){
	int16_t xa=lsm_read_value(OUT_X_L_A);
	float xf = xa * 2.0f / 32678.0f;
	int x = xf*100;
	return x;
}
int lsm_getAcc_y(void){
	int16_t ya=lsm_read_value(OUT_Y_L_A);
	float yf = ya * 2.0f / 32678.0f;
	int y = yf*100;
	return y;
}
int lsm_getAcc_z(void){
	int16_t za=lsm_read_value(OUT_Z_L_A);
	float zf = za * 2.0f / 32678.0f;
	int z = zf*100;
	return z;
}
uint8_t lsm_init(void){
	lsm_write_1(CTRL_REG5_A,0b10011000);  // w³ odczyt temp
	lsm_write_1(CTRL_REG1_A,0x40|0x07);  //w³ akcelerometr ustaw f=25hz
	uint8_t ver,ok=0;
	ver=lsm_read_1(WHO_AM_I);
	if(ver==73)ok=1;
	return ok;
}

