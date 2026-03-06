/*
 * hal_delay_gpio.h
 *
 *  Created on: Jan 4, 2026
 *      Author: emilr
 */

#ifndef LIBS_HAL_DELAY_GPIO_H_
#define LIBS_HAL_DELAY_GPIO_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * @file hal_delay_gpio.h
 * @brief Hardware Abstraction Layer for GPIO and delay operations
 *
 * This module provides platform-independent interface for GPIO control
 * and timing functions. It abstracts the hardware-specific implementations
 * to enable portable sensor drivers and simplified unit testing.
 *
 * The module uses a structure-based pin definition that encapsulates
 * both port and pin number, making it suitable for various MCU architectures.
 */

#ifdef USE_HAL_DRIVER

#include "main.h"
#define gpio_port GPIO_TypeDef
#define gpio_pin  uint16_t

#else

#define gpio_port uint16_t
#define gpio_pin  uint16_t

#define ONE_W_Pin       0x0001
#define ONE_W_GPIO_Port ((uint16_t *)0x0001)

#endif


/**
 * @brief GPIO pin descriptor structure
 *
 * This structure contains both the GPIO port and pin number,
 * providing a complete hardware reference for pin operations.
 */

typedef struct {
  gpio_port *port;   /**< GPIO port (e.g., GPIOA, GPIOB, etc.) */
  gpio_pin  pin;    /**< Pin number within the port (e.g., GPIO_PIN_0) */

} gpio_pin_t;

void hal_delay_init(void);
void hal_gpio_set_mode_input(gpio_pin_t pin);
void hal_gpio_set_mode_output(gpio_pin_t pin);
void hal_gpio_write(gpio_pin_t pin, bool state);
bool hal_gpio_read(gpio_pin_t pin);
void hal_delay_us(uint16_t microseconds);
void hal_delay_ms(uint32_t milliseconds);


#endif /* LIBS_HAL_DELAY_GPIO_H_ */
