/*
 * uart.h
 *
 *  Created on: 14 maj 2018
 *      Author: Dv8
 */

#ifndef APPLICATION_USER_UART_UART_H_
#define APPLICATION_USER_UART_UART_H_

#include "stm32f4xx_hal.h"

#define uart_port huart2
UART_HandleTypeDef uart_port;

void uart_text(char* StringPtr);
void uart_nr_int(int16_t nr);
void uart_ln(void);

#endif /* APPLICATION_USER_UART_UART_H_ */
