/*
 * hm10.h
 *
 *  Created on: 23 paź 2023
 *      Author: emilr
 */

#ifndef DRIVERS_BT_HM10_H_
#define DRIVERS_BT_HM10_H_

#include "main.h"
#include <stdbool.h>

#define MIN(a, b)  ((a) < (b) ? (a) : (b))

void HM10_io_init(void);
void HM10_io_deinit(void);

void HM10_io_cmd_irq_handler(void);
bool HM10_io_cmd_getc(uint8_t *d);
bool HM10_io_cmd_putc(uint8_t d, uint32_t timeout);
void HM10_io_cmd_clear_rx_buf(void);

/*void HM10_io_data_irq_handler(void);
bool HM10_io_data_getc(uint8_t *d);
void HM10_io_data_putc(uint8_t d);*/

void HM10_Power(bool up);


#endif /* DRIVERS_BT_RN4871_H_ */
