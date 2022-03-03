#ifndef _MAX31865_V1_H
#define _MAX31865_V1_H

/*
  Author:     Nima Askari
  WebSite:    http://www.github.com/NimaLTD
  Instagram:  http://instagram.com/github.NimaLTD
  Youtube:    https://www.youtube.com/channel/UCUhY7qY1klJm1d2kulr9ckw

  Version:    1.0.0


  Reversion History:

  (1.0.0)
  First Release.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f3xx.h"
#include "main.h"
#include <stdbool.h>

#define STATUS_OK 1
#define STATUS_READ_TEMP_ERR 2
#define STATUS_READ_CONF_ERR 3

#define SOLAR 0
#define PIEC 1
#define OBIEG 2
#define ZASOBNIK 3

#define ZASOBNIK_PIN CS1_Pin
#define SOLAR_PIN CS2_Pin
#define PIEC_PIN CS3_Pin
#define ZASOBNIK_GPIO CS1_GPIO_Port
#define SOLAR_GPIO CS2_GPIO_Port
#define PIEC_GPIO CS3_GPIO_Port

#define MAX_CONFIG_WRITE_REG_ADDR 0x80
#define MAX_CONFIG_REG_VAL (1<<7)|(1<<6)
//bit 7 - BIAS on, bit 6 - constant reading temp, bit 5 - not in use,
// bit 4 - 2 wire, bit 3, 2, 1 fault(no need), bit 0 - 60Hz mode,
#define MAX_CONFIG_READ_REG_ADDR 0x00
#define MAX_TEMP_READ_REG_ADDR 0x01


#define RTD_A 3.9083e-3
#define RTD_B -5.775e-7

typedef struct
{
	GPIO_TypeDef      *cs_gpio;
	uint16_t          cs_pin;
	SPI_HandleTypeDef *spi;

}Max;

typedef struct
{
	float temp;
	uint8_t status;
}Max_read;

Max max;

void  max31865_gpio_init(SPI_HandleTypeDef *spi);

void  max31865_init(uint8_t sensor_nr);

Max_read max31865_getTemp(uint8_t sensor_nr);

#ifdef __cplusplus
}
#endif



#endif
