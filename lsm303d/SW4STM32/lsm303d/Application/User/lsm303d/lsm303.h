/*
 * lsm303.h
 *
 *  Created on: 14 maj 2018
 *      Author: Dv8
 */
#include "stm32f4xx_hal.h"   //////////////////  DO WPROWADZENIA


#ifndef APPLICATION_USER_LSM303D_LSM303_H_
#define APPLICATION_USER_LSM303D_LSM303_H_

#define i2c_port hi2c1   //////////////////  DO WPROWADZENIA
I2C_HandleTypeDef i2c_port;

#define lsmReadAddr 0x3B
#define lsmWriteAddr 0x3A
#define WHO_AM_I 0x0F
#define OUT_TEMP 0x05
#define OUT_X_L_A 0x28
#define OUT_X_H_A 0x29
#define OUT_Y_L_A 0x2A
#define OUT_Y_H_A 0x2B
#define OUT_Z_L_A 0x2C
#define OUT_Z_H_A 0x2D

#define CTRL_REG1_A 0x20
#define CTRL_REG2_A 0x21
#define CTRL_REG3_A 0x22
#define CTRL_REG4_A 0x23
#define CTRL_REG5_A 0x24
#define CTRL_REG6_A 0x25
#define CRA_REG_M 0x00
#define CRB_REG_M 0x01
#define MR_REG_M 0x02

void lsm_write_1(uint8_t reg,uint8_t val);
uint8_t lsm_read_1(uint8_t reg);
uint16_t lsm_read_2(uint8_t reg);
int16_t lsm_read_value(uint8_t reg);
int lsm_getAcc_x(void);
int lsm_getAcc_y(void);
int lsm_getAcc_z(void);
uint8_t lsm_init(void);

#endif /* APPLICATION_USER_LSM303D_LSM303_H_ */
