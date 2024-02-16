/*
 * lsm6.c
 *
 *  Created on: 12 gru 2023
 *      Author: emilr
 */
#include "../my_libs/lsm6.h"

#include "cmsis_os.h"
#include "main.h"
#include "stdbool.h"

extern I2C_HandleTypeDef hi2c2;
volatile bool lsm_inited = false;


// read / write operations made on i2c hal functions

static bool LSM_readReg(uint8_t reg, uint8_t *data, uint8_t size) {

  HAL_StatusTypeDef status;

  if((status = HAL_I2C_Master_Transmit(&hi2c2, LSM6_I2C_ADDR, &reg, 1, 20)) == HAL_OK)
    status = HAL_I2C_Master_Receive(&hi2c2, LSM6_I2C_ADDR, data, size, 20);

  return(status);
}

static bool LSM_writeReg(uint8_t reg, uint8_t data) {
  uint8_t tx_data[2] = {reg,data};
  if(HAL_I2C_Master_Transmit(&hi2c2, LSM6_I2C_ADDR, tx_data, 2, 20) == HAL_OK)
    return true;

  return false;
}

void lsm_init()
{
  uint8_t who_i_am;
  LSM_readReg(LSM6_WHO_I_AM_REG, &who_i_am, 1);

  //  1101011
  if(who_i_am == LSM6DSO_WHO_I_AM_REG_VAL) // just checking
  {


    LSM_writeReg(LSM6DSO_FUNC_CFG_ACCESS, 0x80);// Enable access to embedded functions registers
    LSM_writeReg(LSM6DSO_PAGE_RW,         0x40);// Select read operation mode
    LSM_writeReg(LSM6DSO_PAGE_SEL,        0x11);// Select page 1
    LSM_writeReg(LSM6DSO_PAGE_ADDR,       0x83);// Set address
    LSM_writeReg(LSM6DSO_PAGE_VALUE,      0x04);// Set register value
    LSM_writeReg(LSM6DSO_PAGE_RW,         0x00);// Write operation disabled
    LSM_writeReg(LSM6DSO_EMB_FUNC_EN_A,   0x08);// Enable step counting
    LSM_writeReg(LSM6DSO_EMB_FUNC_INT1,   0x08);// Interrupt to set GPIO pin after step detection
    LSM_writeReg(LSM6DSO_EMB_FUNC_INT2,   0x00);// Disable it
    LSM_writeReg(LSM6DSO_FUNC_CFG_ACCESS, 0x00);// Disable access to embedded functions registers
//    LSM_writeReg(LSM6DSO_MD1_CFG,         0x02); testing
//    LSM_writeReg(LSM6DSO_CTRL1_XL,        0x28);
    lsm_inited = true;
  }
}

uint16_t steps = 0;

void handle_lsm_steps()
{
  if(!lsm_inited)
    return;

  // function read quantity o detected steps
  uint8_t steps_reg_val[2];
  LSM_readReg(LSM6_STEP_COUNTER_L, steps_reg_val, 2);

  steps = steps_reg_val[0] | (steps_reg_val[1] << 8);

}


