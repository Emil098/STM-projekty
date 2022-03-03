/*
 * uart.c
 *
 *  Created on: 14 maj 2018
 *      Author: Dv8
 */
#include "uart.h"

void uart_text(char* StringPtr){

	while(*StringPtr != 0){
		HAL_UART_Transmit(&uart_port,StringPtr,1,1000);
		StringPtr++;
	}
}
void uart_nr_int(int16_t nr){
	char str[26];
	uint16_t n;
	uint8_t i=0,d=1;
	n=nr;

	if(nr<0){
		n*=(-1);
		while(n > 0){
			i++;
			n = n/10;
		}
		i++;
	}else{
		while(n > 0){
			i++;
			n = n/10;
		}
	}
	itoa(nr, str,10);
	HAL_UART_Transmit(&uart_port,str,i,1000);
}
void uart_ln(void){
	HAL_UART_Transmit(&uart_port,"\n",1,1000);
}
