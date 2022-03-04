/*
 * touch.h
 *
 *  Created on: 02.06.2021
 *      Author: Emil
 */

#ifndef TOUCH_H
#define TOUCH_H

#include "lcd.h"
#include "main.h"
#include "stm32f3xx_hal.h"

// datasheet uk³adu odczytu wskazañ matrycy dotykowej wyœwietlacza (4 pina ADC na UART)
// https://datasheetspdf.com/pdf-file/1266150/Microchip/AR1011/1

//#define DO_DOMU
#define DO_DOMKU

// sta³e ADC do wyliczania pozycji dotkniêtego pkt
#define TOUCH_ADC_X_MAX 2015
#define TOUCH_ADC_X_MIN 0
#define TOUCH_ADC_Y_MIN 0
#define TOUCH_ADC_Y_MAX 2015

// wymiary wyœwietlacza
#define TFT_HEIGHT 320
#define TFT_WIDTH 240

// brak dotyku
#define NO_TOUCH 0x10

// u¿ywany uart w projekcie
#define uart huart3


typedef struct {
	uint16_t x;
	uint16_t y;
	uint8_t p; // informuje czy matryca zosta³a dotknieta
}point;

UART_HandleTypeDef huart3;

uint8_t recive[10];

// funkcje mapuj¹ce przeliczaj¹ wartoœci zmiennych
int32_t map(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max);
float map_float(float x, float in_min, float in_max, float out_min, float out_max);

// sprawdzenie adresu id
void check_version();

// wys³anie komendy
void send_comand(uint8_t cmd, uint8_t size, uint8_t *rcv);

// wys³anie komendy i danych
void send_comand_n_data(uint8_t size_cmd, uint8_t* cmd, uint8_t size_rcv, uint8_t *rcv);

// wy³¹czenie odczytu
void disable_touch();

// w³¹czenie odczytu
void enable_touch();

// zapisanie rejestru REGISTER_START_ADDRESS_REQUEST
void reg_start();

// predefiniowana kalibracja odczytywanych danych
void calibration_inset();

// zapisanie rejestru REGISTER_READ
void register_read();

// kalibracja odczytywanych danych (za pomoc¹ dotykania rogów matrycy wyœwietlacza
void calibration_mode();

// odczyt pozycji dotkniêtego pkt matrycy (w postaci ADC)
point get_pos();

// zainicjowanie odczytu z panela dotykowego
void touch_init();


#endif /* APPLICATION_USER_CORE_DISPLAY_TOUCH_H_ */
