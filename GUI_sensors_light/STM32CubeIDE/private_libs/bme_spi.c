/**
 * Copyright (C) 2023 Bosch Sensortec GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "main.h"
#include "bme68x.h"
#include "cmsis_os.h"
#include "bme_spi.h"

/******************************************************************************/
/*!                 Macro definitions                                         */
/*! BME68X shuttle board ID */
#define BME68X_SHUTTLE_ID  0x93

/******************************************************************************/
/*!                Static variable definition                                 */
static uint8_t dev_addr = 0xEE;
/******************************************************************************/
/*!                User interface functions                                   */
//extern I2C_HandleTypeDef hi2c1;
extern SPI_HandleTypeDef hspi5;

// SPI read function (based on HAL library)
BME68X_INTF_RET_TYPE bme68x_spi_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
  HAL_StatusTypeDef status;

  HAL_GPIO_WritePin(CS_BME680_GPIO_Port, CS_BME680_Pin, GPIO_PIN_RESET);
  status = HAL_SPI_Transmit(&hspi5, (uint8_t *)&reg_addr, 1, 1000);
  status = HAL_SPI_Receive(&hspi5, (uint8_t *)reg_data, len, 1000);
  HAL_GPIO_WritePin(CS_BME680_GPIO_Port, CS_BME680_Pin, GPIO_PIN_SET);

  return (status == HAL_OK ? BME68X_INTF_RET_SUCCESS : BME68X_E_COM_FAIL);
}

// SPI write function (based on HAL library)
BME68X_INTF_RET_TYPE bme68x_spi_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
  uint8_t dTab[20] = {};
  HAL_StatusTypeDef status;

  dTab[0] = reg_addr;
  memcpy(dTab + 1, reg_data, len);

  HAL_GPIO_WritePin(CS_BME680_GPIO_Port, CS_BME680_Pin, GPIO_PIN_RESET);
  status = HAL_SPI_Transmit(&hspi5, dTab, len + 1, 1000);
  HAL_GPIO_WritePin(CS_BME680_GPIO_Port, CS_BME680_Pin, GPIO_PIN_SET);

  return (status == HAL_OK ? BME68X_INTF_RET_SUCCESS : BME68X_E_COM_FAIL);
}


/*!
 * Delay function map to COINES platform
 */
void bme68x_delay_us(uint32_t period, void *intf_ptr)
{
    (void)intf_ptr;
    osDelay((period / 1000)+1);
}

void bme68x_check_rslt(const char api_name[], int8_t rslt, char *alert)
{
  // updating alert text if there will be any error alert text will be shown on the screen
  switch (rslt)
  {
  case BME68X_OK:
    memset( (uint8_t *)alert, 0, sizeof(alert));
    break;
  case BME68X_E_NULL_PTR:
    sprintf(alert,"API name [%s]  Error [%d] : Null pointer", api_name, rslt);
    break;
  case BME68X_E_COM_FAIL:
    sprintf(alert,"API name [%s]  Error [%d] : Communication failure", api_name, rslt);
    break;
  case BME68X_E_INVALID_LENGTH:
    sprintf(alert,"API name [%s]  Error [%d] : Incorrect length parameter", api_name, rslt);
    break;
  case BME68X_E_DEV_NOT_FOUND:
    sprintf(alert,"API name [%s]  Error [%d] : Device not found", api_name, rslt);
    break;
  case BME68X_E_SELF_TEST:
    sprintf(alert,"API name [%s]  Error [%d] : Self test error", api_name, rslt);
    break;
  case BME68X_W_NO_NEW_DATA:
    sprintf(alert,"API name [%s]  Warning [%d] : No new data found", api_name, rslt);
    break;
  default:
    sprintf(alert,"API name [%s]  Error [%d] : Unknown error code", api_name, rslt);
    break;
  }
}

int8_t bme68x_interface_init(struct bme68x_dev *bme, uint8_t intf)
{
    int8_t rslt = BME68X_OK;

    if (bme != NULL)
    {
      if (intf == BME68X_SPI_INTF)
      {
        bme->read = bme68x_spi_read;
        bme->write = bme68x_spi_write;
        bme->intf = BME68X_SPI_INTF;
      }

      bme->delay_us = bme68x_delay_us;
      bme->intf_ptr = &dev_addr;
      bme->amb_temp = 25; /* The ambient temperature in deg C is used for defining the heater temperature */
    }
    else
    {
        rslt = BME68X_E_NULL_PTR;
    }

    return rslt;
}

