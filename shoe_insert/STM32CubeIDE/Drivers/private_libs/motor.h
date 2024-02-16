/*
 * motor.h
 *
 *  Created on: 23 lis 2023
 *      Author: emilr
 */

#ifndef MY_LIBS_MOTOR_H_
#define MY_LIBS_MOTOR_H_

#include "main.h"
#include <stdio.h>
#include <stdbool.h>


#define LONG_VIBRATION_TIME 1500  // 1,5 sec.

typedef enum
{
  NO_WIBRATION,
  SHORT_WIBRATION,
  LONG_WIBRATION,
}motor_state_info;

typedef struct
{
  motor_state_info state;
}motor_info;

void motor_vibrate(motor_state_info type);
void handle_vibration();

#endif /* MY_LIBS_MOTOR_H_ */
