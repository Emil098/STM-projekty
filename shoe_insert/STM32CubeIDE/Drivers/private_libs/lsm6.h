/*
 * lsm6.h
 *
 *  Created on: 12 gru 2023
 *      Author: emilr
 */

#ifndef MY_LIBS_LSM6_H_
#define MY_LIBS_LSM6_H_

#define LSM6_I2C_ADDR (0b1101011 << 1)

#define LSM6_WHO_I_AM_REG   0x0F

#define LSM6DSO_FUNC_CFG_ACCESS 0x01
#define LSM6DSO_PAGE_RW         0x17
#define LSM6DSO_PAGE_SEL        0x02
#define LSM6DSO_PAGE_ADDR       0x08
#define LSM6DSO_PAGE_VALUE      0x09
#define LSM6DSO_EMB_FUNC_EN_A   0x04
#define LSM6DSO_EMB_FUNC_INT1   0x0A
#define LSM6DSO_EMB_FUNC_INT2   0x0E
#define LSM6DSO_MD1_CFG         0x5E
#define LSM6DSO_CTRL1_XL        0x10

#define LSM6_FUNC_CFG_ACCESS  0x01
#define LSM6_SM_THS           0x13

#define LSM6_STEP_COUNTER_L 0x4B
#define LSM6_STEP_COUNTER_H 0x4C

#define LSM6DSO_WHO_I_AM_REG_VAL 0x6C

void lsm_init();
void handle_lsm_steps();


#endif /* MY_LIBS_LSM6_H_ */
