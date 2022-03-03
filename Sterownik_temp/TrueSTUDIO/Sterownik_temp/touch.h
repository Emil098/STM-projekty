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

//#define DO_DOMU
#define DO_DOMKU

#define TOUCH_ADC_X_MAX 2015
#define TOUCH_ADC_X_MIN 0
#define TOUCH_ADC_Y_MIN 0
#define TOUCH_ADC_Y_MAX 2015

#define TFT_HEIGHT 320
#define TFT_WIDTH 240

#define NO_TOUCH 0x10

#define uart huart3


typedef struct {
	uint16_t x;
	uint16_t y;
	uint8_t p;
}point;

UART_HandleTypeDef huart3;

uint8_t recive[10];

int32_t map(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max);

float map_float(float x, float in_min, float in_max, float out_min, float out_max);

void check_version();

void send_comand(uint8_t cmd, uint8_t size, uint8_t *rcv);

void send_comand_n_data(uint8_t size_cmd, uint8_t* cmd, uint8_t size_rcv, uint8_t *rcv);

void disable_touch();

void enable_touch();

void reg_start();

void calibration_inset();

void register_read();

void calibration_mode();

point get_pos();

void touch_init();


#endif /* APPLICATION_USER_CORE_DISPLAY_TOUCH_H_ */
