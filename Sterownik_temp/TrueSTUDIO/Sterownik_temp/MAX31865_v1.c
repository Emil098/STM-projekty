#include <math.h>
#include "max31865_v1.h"


void  max31865_gpio_init(SPI_HandleTypeDef *spi){
	HAL_GPIO_WritePin(ZASOBNIK_GPIO,ZASOBNIK_PIN,1);
	HAL_GPIO_WritePin(SOLAR_GPIO,SOLAR_PIN,1);
	HAL_GPIO_WritePin(PIEC_GPIO,PIEC_PIN,1);
	HAL_SPI_Init(spi);
	max.spi = spi;
}
void  max31865_init(uint8_t sensor_nr){
//	GPIO_TypeDef  *cs_gpio;
//	uint16_t cs_pin;
//	max.spi = spi;
	HAL_GPIO_WritePin(ZASOBNIK_GPIO,ZASOBNIK_PIN,1);
	HAL_GPIO_WritePin(SOLAR_GPIO,SOLAR_PIN,1);
	HAL_GPIO_WritePin(PIEC_GPIO,PIEC_PIN,1);

	switch(sensor_nr){
	case ZASOBNIK:
		max.cs_gpio = ZASOBNIK_GPIO;
		max.cs_pin = ZASOBNIK_PIN;
		break;
	case SOLAR:
		max.cs_gpio = SOLAR_GPIO;
		max.cs_pin = SOLAR_PIN;
		break;
	case PIEC:
		max.cs_gpio = PIEC_GPIO;
		max.cs_pin = PIEC_PIN;
		break;
	}
	uint8_t data_to_send[2] = {MAX_CONFIG_WRITE_REG_ADDR,MAX_CONFIG_REG_VAL};
	HAL_GPIO_WritePin(max.cs_gpio,max.cs_pin,0);
	HAL_SPI_Transmit(max.spi,data_to_send, 2, 100);
	HAL_GPIO_WritePin(max.cs_gpio,max.cs_pin,1);
}

Max_read max31865_getTemp(uint8_t sensor_nr){
	Max_read ret = {0};
	uint8_t rec[3],data_to_send;
	ret.status = STATUS_OK;
	switch(sensor_nr){
		case ZASOBNIK:
			max.cs_gpio = ZASOBNIK_GPIO;
			max.cs_pin = ZASOBNIK_PIN;
			break;
		case SOLAR:
			max.cs_gpio = SOLAR_GPIO;
			max.cs_pin = SOLAR_PIN;
			break;
		case PIEC:
			max.cs_gpio = PIEC_GPIO;
			max.cs_pin = PIEC_PIN;
			break;
		}

	double Z1, Z2, Z3, Z4, Rt,resistance=0;
	double RTDa = 3.9083e-3;
	double RTDb = -5.775e-7;
	float temperature;

	data_to_send = MAX_CONFIG_READ_REG_ADDR;
	HAL_GPIO_WritePin(max.cs_gpio,max.cs_pin,0);
	HAL_SPI_Transmit(max.spi,&data_to_send, 1, 100);
	HAL_SPI_Receive(max.spi,rec,3,100);
	HAL_GPIO_WritePin(max.cs_gpio,max.cs_pin,1);

	if(rec[1] == 0xFF && rec[2] == 0xFF){
		ret.status = STATUS_READ_TEMP_ERR;
		return ret;
	}
	if(rec[0] != (MAX_CONFIG_REG_VAL)){
		ret.status = STATUS_READ_CONF_ERR;
		return ret;
	}

	resistance = (rec[1]<<8 | rec[2])>>1;

	Rt = resistance;
	Rt /= 32768;
	Rt *= 398; //This is now the real resistance in Ohms

	Z1 = -RTDa;
	Z2 = RTDa * RTDa - (4 * RTDb);
	Z3 = (4 * RTDb) / 100;
	Z4 = 2 * RTDb;

	temperature = Z2 + (Z3 * Rt);
	temperature = (sqrt(temperature) + Z1) / Z4;

	ret.temp = temperature;

	return ret;
}

