/*
 * motor.c
 *
 *  Created on: 23 lis 2023
 *      Author: emilr
 */

#include "../my_libs/motor.h"

#include "main.h"
#include "cmsis_os.h"
#include "../my_libs/drv2605.h"

motor_info motor = {};
uint32_t vibrate_untill_ts = 0;

void motor_vibrate(motor_state_info type)
{
  // long vibration is a wrong position warning
  // short one informs that device is ready for establishing new bt connection
  switch(type)
  {
  case LONG_WIBRATION:
    vibrate_untill_ts = HAL_GetTick() + LONG_VIBRATION_TIME;
    break;
  case SHORT_WIBRATION:
    Haptic_DRV2605_go();
    break;
  case NO_WIBRATION:

    break;
  }
}

void handle_vibration()
{
  if(vibrate_untill_ts > HAL_GetTick())
    Haptic_DRV2605_go();

}

