/*
 * gui_handle.h
 *
 *  Created on: 11 lut 2024
 *      Author: emilr
 */

#ifndef APPLICATION_USER_PRIVETE_LIBS_GUI_HANDLE_H_
#define APPLICATION_USER_PRIVETE_LIBS_GUI_HANDLE_H_

#include "main.h"
#include "gui_defs.h"
#include <stdbool.h>
#include <stm32f7xx_hal.h>

// assigning pwm outputs to colors
#define PWM_RED(x)    TIM1->CCR1=x
#define PWM_GREEN(x)  TIM12->CCR2=x
#define PWM_BLUE(x)   TIM2->CCR1=x

// updating colors with 1 byte range
#define SET_COL_RED(x)    PWM_RED(MAP(x, 0, 255, 0, 1000))
#define SET_COL_GREEN(x)  PWM_GREEN(MAP(x, 0, 255, 0, 1000))
#define SET_COL_BLUE(x)   PWM_BLUE(MAP(x, 0, 255, 0, 1000))

// update all pwm channels with color hex value
#define COLOR_UPDATE(x) SET_COL_RED((uint8_t)(x >> 16));    \
                        SET_COL_GREEN((uint8_t)(x >> 8));   \
                        SET_COL_BLUE((uint8_t)(x))


void set_pwm_color(uint32_t col);
void set_brightness(uint8_t brightness, bool save);
void set_led_status(bool status);
void get_light_struct(color_settings_st *light);
void get_env_struct(enviroment_struct *env);
void update_env_struct(enviroment_struct env);

#endif /* APPLICATION_USER_PRIVETE_LIBS_GUI_HANDLE_H_ */
