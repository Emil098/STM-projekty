#ifndef _MAX31865_V1_H
#define _MAX31865_V1_H


// strona z datasheet-em
// https://datasheets.maximintegrated.com/en/ds/MAX31865.pdf


#include "stm32f3xx.h"
#include "main.h"
#include <stdbool.h>

// makra okreœlaj¹ce stan odczytu danych z max-a
#define STATUS_OK 1
#define STATUS_READ_TEMP_ERR 2
#define STATUS_READ_CONF_ERR 3

// makra dla poszczególnych czujników
#define SOLAR 0
#define PIEC 1
#define OBIEG 2
#define ZASOBNIK 3

// makra pinów CS dla poszczególnych czujników
#define ZASOBNIK_PIN CS1_Pin
#define SOLAR_PIN CS2_Pin
#define PIEC_PIN CS3_Pin
#define ZASOBNIK_GPIO CS1_GPIO_Port
#define SOLAR_GPIO CS2_GPIO_Port
#define PIEC_GPIO CS3_GPIO_Port

// makra rejestrów max-a (datasheet strona 13)
#define MAX_CONFIG_WRITE_REG_ADDR 0x80  // rejestr config max-a
#define MAX_CONFIG_REG_VAL (1<<7)|(1<<6)  //(7) BIAS - on (w³¹czenie czujnika), (6) ci¹g³y odczyt
// opis bitów rejestru config
// bit 7 - BIAS on, bit 6 - constant reading temp, bit 5 - not in use,
// bit 4 - 2 wire, bit 3, 2, 1 fault(no need), bit 0 - 60Hz mode,
#define MAX_CONFIG_READ_REG_ADDR 0x00
#define MAX_TEMP_READ_REG_ADDR 0x01 // rejestr odczytanych danych (w postaci ADC)


typedef struct
{
	GPIO_TypeDef      *cs_gpio;
	uint16_t          cs_pin;
	SPI_HandleTypeDef *spi;

}Max; // gpio odczytywanego czujnika

typedef struct
{
	float temp;
	uint8_t status;
}Max_read; // info o odczycie z max-a

Max max;


// funkcja inicjuj¹ca gpio pinów CS ka¿dego max-a oraz spi
void  max31865_gpio_init(SPI_HandleTypeDef *spi);

// funkcja inicjuj¹ca rejestry poszczególnych max-ow do ci¹g³ego odczytu temperatury
void  max31865_init(uint8_t sensor_nr);

// funkcja odczytuj¹ca rejestry: config, RTD MSB i RTB LSB. Konweruje odczytan¹ wart. ADC na temp.
Max_read max31865_getTemp(uint8_t sensor_nr);


#endif
