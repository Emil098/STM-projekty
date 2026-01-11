/*
 * debug_uart.h
 *
 *  Created on: Jan 11, 2026
 *      Author: emilr
 */
/**
 * @file debug_uart.h
 * @brief Interrupt-driven debug UART driver with RX/TX ring buffers.
 *
 * This module implements a simple UART driver intended for debugging/console
 * output and basic input. It maintains separate TX and RX ring buffers and
 * services them from an interrupt handler.
 *
 * Key points:
 *  - Non-blocking byte transmit (enqueue into TX buffer)
 *  - Byte receive from RX buffer (dequeue)
 *  - Interrupt handler must be called from the MCU USARTx_IRQHandler()
 *  - Buffer sizes must be powers of two (mask arithmetic)
 *
 * Host-based unit testing:
 *  - When USE_HAL_DRIVER is not defined, the module can be built against a
 *    fake UART backend (see fakeUart.h in your project).
 *  - Public functions are easy to mock using Unity/CMock in higher layers.
 */


#ifndef DRIVERS_LIBS_DEBUG_UART_H_
#define DRIVERS_LIBS_DEBUG_UART_H_

//#include "main.h"
#include <stdbool.h>
#include <stdint.h>

/* Ring buffer sizes (must be power-of-two for mask arithmetic) */
#define DEBUG_TX_SIZE  256
#define DEBUG_RX_SIZE   64

#define DEBUG_TX_MASK (DEBUG_TX_SIZE - 1)
#define DEBUG_RX_MASK (DEBUG_RX_SIZE - 1)

void debug_init(void);
void debug_UART_irq(void);
bool dbg_getc(uint8_t *d);
bool dbg_putc_nonblock(uint8_t d);

#endif /* DRIVERS_LIBS_DEBUG_UART_H_ */
