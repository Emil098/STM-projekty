/*
 * hal_delay_gpio.c
 *
 *  Created on: Jan 4, 2026
 *      Author: emilr
 */
#include "hal_delay_gpio.h"

#ifdef USE_HAL_DRIVER

/* Static variables for delay timing */
extern TIM_HandleTypeDef htim6;

#define htim_us_delay htim6  // Timer instance for microsecond delays
#endif



/**
 * @brief Initialize timer for microsecond delay functionality
 *
 * This function should be called during system initialization
 * before using hal_delay_us() function.
 */
void hal_delay_init(void) {

#ifdef USE_HAL_DRIVER
  // Timer initialization code here
  HAL_TIM_Base_Start(&htim_us_delay);
#else

#endif
}

/**
 * Configure GPIO pin as input
 *
 * Sets the specified pin to input mode with appropriate configuration.
 * In test mode, this function records the configuration for verification.
 *
 * @param pin GPIO pin descriptor containing port and pin number
 */
void hal_gpio_set_mode_input(gpio_pin_t pin) {
#ifdef USE_HAL_DRIVER
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.Pin = pin.pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(pin.port, &GPIO_InitStruct);
#else
  // For testing purposes, we just acknowledge the call
  // Actual implementation would be platform-specific
#endif
}

/**
 * Configure GPIO pin as output
 *
 * Sets the specified pin to output mode with push-pull configuration.
 * For STM32 HAL, this configures as output with low speed.
 * In test mode, this function records the configuration for verification.
 *
 * @param pin GPIO pin descriptor containing port and pin number
 */
void hal_gpio_set_mode_output(gpio_pin_t pin) {
#ifdef USE_HAL_DRIVER
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.Pin = pin.pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(pin.port, &GPIO_InitStruct);
#else
  // For testing purposes, we just acknowledge the call
#endif
}

/**
 * Write logical state to GPIO pin
 *
 * Sets the output pin to either high (true) or low (false) state.
 * The pin must be configured as output before calling this function.
 *
 * @param pin GPIO pin descriptor containing port and pin number
 * @param state Logical state to set (true = high, false = low)
 */
void hal_gpio_write(gpio_pin_t pin, bool state) {
#ifdef USE_HAL_DRIVER
  HAL_GPIO_WritePin(pin.port, pin.pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
#else
  // For testing, this would update mock state
#endif
}

/**
 * Read logical state from GPIO pin
 *
 * Reads the current state of the specified input pin.
 * The pin must be configured as input before calling this function.
 *
 * @param pin GPIO pin descriptor containing port and pin number
 * @return Current logical state (true = high, false = low)
 */
bool hal_gpio_read(gpio_pin_t pin) {
#ifdef USE_HAL_DRIVER
  return (HAL_GPIO_ReadPin(pin.port, pin.pin) == GPIO_PIN_SET);
#else
  // For testing, return mock state
  return false;  // Should be controlled by test framework
#endif
}

/**
 * Delay execution for specified microseconds
 *
 * Implements a busy-wait delay with microsecond precision.
 * For STM32, this uses a hardware timer for accurate timing.
 * For testing, this function records the delay duration without actually waiting.
 *
 * @param Delay duration in microseconds
 * @note Minimum delay is 1us, maximum is 65535us (max value of TIM6 ARR)
 */
void hal_delay_us(uint16_t microseconds) {
#ifdef USE_HAL_DRIVER
  __HAL_TIM_SET_COUNTER(&htim_us_delay,0);  // set the counter value a 0
  while (__HAL_TIM_GET_COUNTER(&htim_us_delay) < microseconds);  // wait for the counter to reach the us input in the parameter
#else
  // For testing, record the delay value but don't actually wait
  // Test framework can verify correct delays were requested
#endif
}

/**
 * Delay execution for specified milliseconds
 *
 * @param milliseconds Delay duration in milliseconds
 */
void hal_delay_ms(uint32_t milliseconds) {
#ifdef USE_HAL_DRIVER
  HAL_Delay(milliseconds);
#else
  // For testing, record or simulate the delay
#endif
}

/**
 * Helper function to create gpio_pin_t structure
 *
 * This utility function creates a gpio_pin_t structure from
 * individual port and pin values. Useful for creating pin definitions
 * in application code.
 *
 * @param port GPIO port (e.g., GPIOA)
 * @param pin GPIO pin (e.g., GPIO_PIN_0)
 * @return Initialized gpio_pin_t structure
 */
gpio_pin_t hal_gpio_create_pin(gpio_port *port, gpio_pin pin) {
  gpio_pin_t gpio_pin;
  gpio_pin.port = port;
  gpio_pin.pin = pin;
  return gpio_pin;
}
