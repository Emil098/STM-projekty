/*
 * sens.h
 *
 *  Created on: 14 lis 2023
 *      Author: emilr
 */

#ifndef MY_LIBS_SENS_H_
#define MY_LIBS_SENS_H_

#include "main.h"
#include "stm32l1xx_hal_conf.h"
#include <stdio.h>
#include <stdbool.h>

#define ADC_MIN_KG  10.0
#define ADC_MAX_KG  900.0
#define MIN_KG      0.0
#define MAX_KG      10.0

#define NO_PREASURE_SLEEP_TIMEOUT 30*1000
#define WALK_TIMEOUT 2*1000
#define STAND_SIT_TIMEOUT 3*1000
#define POSITION_WEIGHT_MAX_DIFF 1.5

#define SOFT_PRESSED_KG_VAL 0.8
#define NORMAL_PRESSED_KG_VAL 2.0

#define MOTOR_NEXT_WARNING_TIMEOUT 10*1000
#define MOTOR_TIME_TO_WARNING      2*1000

typedef enum
{
  WALKING = 1,
  STANDING,
  SITTING,
  NOT_USING
}user_status;

typedef struct
{
  uint8_t col_red;
  uint8_t col_green;
  float   weight;
}single_sens_info;

typedef struct
{
  single_sens_info  s[5];
//  single_sens_info  neigbour[5];
  char              stat_tx_data[3];
  char              col_tx_data[10];
  char              wei_tx_data[10];
  float             calibrated_weight[5];
}all_sens_info;

typedef struct
{
  bool        any_sens_pressed;
  uint32_t    last_sens_pressed_ts;
  uint32_t    last_sens_soft_pressed_ts;
  uint32_t    last_step_ts;
  uint8_t     soft_pressed_sens_qt;
  uint8_t     normal_pressed_sens_qt;
  user_status user;
  uint32_t    steps;

}feet_info;

typedef struct
{
  bool        position_wrong;
  uint32_t    last_warning_ts;
  uint32_t    warning_at_ts;
}feet_position_info;

void sens_handler();
void sens_reset_timestamp();
bool check_are_calibrated_values_ok();
void sens_calibrate();

#endif /* MY_LIBS_SENS_H_ */
