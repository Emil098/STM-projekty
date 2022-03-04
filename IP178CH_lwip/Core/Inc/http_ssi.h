/*
 * http_ssi.h
 *
 *  Created on: 01.03.2022
 *      Author: Emil
 */

#ifndef INC_HTTP_SSI_H_
#define INC_HTTP_SSI_H_

#include "stm32f1xx_hal.h"

#define true 1
#define false 0

uint8_t LD3ON_last,LD3ON,LD2ON,LD1ON;

void http_server_init (void);

void mySSIinit(void);

void myCGIinit(void);

#endif /* INC_HTTP_SSI_H_ */
