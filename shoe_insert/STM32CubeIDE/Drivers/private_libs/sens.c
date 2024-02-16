/*
 * sens.c
 *
 *  Created on: 14 lis 2023
 *      Author: emilr
 */
#include "../my_libs/sens.h"

#include "main.h"
#include <stdio.h>
#include <stdbool.h>

#include "../my_libs/ble.h"
#include "../my_libs/motor.h"

extern uint16_t adc_dma_result[5];
extern bool adc_conv_complete_flag;

bool reset_cnt = false;
bool no_sens_pressed = true;
uint32_t last_sens_presssed_ts = 0;

uint8_t big_diff_cnt = 0;
all_sens_info sens = {{},{},{},{},{5,5,5,5,5}};
feet_info feet_prev;
feet_position_info pos_prev;


float map(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

static void floatToUintTab(float nr,char *tab)
{
  // weight is represented by 2 bytes  (integer and decimal part)
  uint16_t u_nr = (uint16_t)(nr*100);
  // ble can't send '0' bytes so we need to add 1 to every byte to make sure that none of them get this value
  tab[0] = (u_nr / 100) + 1;
  tab[1] = (u_nr % 100) + 1;
}

static void transform_adc_to_weight()
{
  for(uint8_t i=0;i<5;i++)
  {
    if(adc_dma_result[i] < ADC_MIN_KG)
      sens.s[i].weight = 0;
      // if adc value is below minimum don't convert this
    else
      sens.s[i].weight  = map((float)adc_dma_result[i], ADC_MIN_KG, ADC_MAX_KG, MIN_KG, MAX_KG);
      // if adc value is greater than minimum then map it to actual weight represented by sensor's resistance

    floatToUintTab(sens.s[i].weight, sens.wei_tx_data + (i*2)); // converting weight to binary data which will be sent by ble

  }
}
void generate_colours()
{
  // in the phone app every sensor is presented with color related to him, it's easier to calculate those colors in C
  for(uint8_t i=0;i<5;i++)
  {
    if(sens.s[i].weight == 0)
    {
      // non pressed sensor's color is green
      sens.s[i].col_red = 0;
      sens.s[i].col_green = 0xFF;
    }
    else if(sens.s[i].weight < (MAX_KG / 2))
    {
      // below weight middle value, the amount of red will increase and (green = max)
      sens.s[i].col_green = 0xFF;
      sens.s[i].col_red = (uint8_t)(map(sens.s[i].weight, MIN_KG, (MAX_KG / 2), 1, 255));
    }
    else if(sens.s[i].weight < MAX_KG)
    {
      // below full and above middle weight value, the amount of green will decrease and (red = max)
      sens.s[i].col_red = 0xFF;
      sens.s[i].col_green = (uint8_t)(map(sens.s[i].weight,(MAX_KG / 2),MAX_KG, 255, 1));
    }
    else
    {
      // fully pressed sensor's color is red
      sens.s[i].col_red = 0xFF;
      sens.s[i].col_green = 0;
    }

    // converting color to ble data
    sens.col_tx_data[i*2]        = sens.s[i].col_red   != 0xFF ? sens.s[i].col_red + 1   : sens.s[i].col_red;
    sens.col_tx_data[(i*2) + 1]  = sens.s[i].col_green != 0xFF ? sens.s[i].col_green + 1 : sens.s[i].col_green;

  }
}

static bool am_i_sitting()
{
  uint8_t below_ok_cnt = 0;

  for(uint8_t i=0;i<5;i++)
  {
    if(sens.s[i].weight > (sens.calibrated_weight[i] + POSITION_WEIGHT_MAX_DIFF))
      return false;

    if(sens.s[i].weight < (sens.calibrated_weight[i] - POSITION_WEIGHT_MAX_DIFF))
      below_ok_cnt++;
  }
  // if user is sitting then more than 2 sensors are significantly below calibrated values
  // but if any sensor's value is significantly above calibrated value then it means that user is standing
  return (below_ok_cnt > 2 ? true : false);
}

uint32_t prev, time;

static void sensors_check()
{
  uint32_t now = HAL_GetTick();
  time = now - prev;
  prev = now;
  // measurement frequency (for debug only)


  feet_info feet_now = feet_prev;

  feet_now.soft_pressed_sens_qt = 0;
  feet_now.normal_pressed_sens_qt = 0;


  for(uint8_t i=0;i<5;i++)
  {
    if(sens.s[i].weight >= SOFT_PRESSED_KG_VAL)
      feet_now.soft_pressed_sens_qt++;

    if(sens.s[i].weight >= NORMAL_PRESSED_KG_VAL)
         feet_now.normal_pressed_sens_qt++;
  }

  feet_now.any_sens_pressed = feet_now.soft_pressed_sens_qt == 0 ? false : true;

  feet_now.last_sens_soft_pressed_ts = feet_now.normal_pressed_sens_qt == 0 ? now : feet_now.last_sens_soft_pressed_ts;

  // eliminate single measurement mistake
  if(feet_now.any_sens_pressed && feet_prev.any_sens_pressed)
    feet_now.last_sens_pressed_ts = now;

  // after reconnecting app with ble we need to update time stamps to prevent from entering sleep mode too early
  if(reset_cnt)
  {
    feet_now.last_sens_pressed_ts = now;
    feet_now.last_step_ts = now;
    reset_cnt = false;
  }

  if(ble_am_i_sleeping() && feet_now.any_sens_pressed)
    ble_disable_sleep_mode();

  if((feet_now.normal_pressed_sens_qt > 2 && feet_prev.normal_pressed_sens_qt <= 2) && (feet_prev.last_step_ts + 20) < feet_now.last_sens_soft_pressed_ts)
  {
    // if in the last cycle user pressed less than 3 sensors and in this he pressed more than 2 and last step was recognized more than 20 ms ago
    // then it means that user just made another step (this step recognizing method is very accurate)
    feet_now.last_step_ts = now;
    feet_now.steps++;
  }

  // by checking last step time stamp we can assume that user is walking or running (if difference between current and last step time stamp is low enough)
  if((now - feet_now.last_step_ts) < WALK_TIMEOUT)
    feet_now.user = WALKING;

  else if((now - feet_now.last_sens_pressed_ts) > NO_PREASURE_SLEEP_TIMEOUT)
  {
    // in this condition we are checking the last time when user pressed any sensor
    // if ts difference is greater than NO_PREASURE_SLEEP_TIMEOUT then firstly ble should enter sleep mode and after this rest of modules
    // finally uC enters standby mode (can be woke only by step interrupt from lsm6 or reconnecting phone app)
    ble_enable_sleep_mode();
    feet_now.user = NOT_USING;
  }
  else if((now - feet_now.last_step_ts) > STAND_SIT_TIMEOUT)
  {
    // if last step was recognized more than 3 sec ago than we should check is user standing or sitting
    feet_now.user = am_i_sitting() ? SITTING : STANDING;
  }

  // preparing ble data (position and steps quantity)
  sens.stat_tx_data[0] = feet_now.user;
  sens.stat_tx_data[1] = (uint8_t)(feet_now.steps);
  sens.stat_tx_data[2] = (uint8_t)(feet_now.steps >> 8);

  feet_prev = feet_now;
}

static void position_check()
{
  float sen_min_val;
  float sen_max_val;
  feet_position_info pos_now = pos_prev;
  uint32_t now = HAL_GetTick();

  pos_now.position_wrong = false;

  // loop below is checking for significant differences between current and calibrated sensors values (and checking is user standing now)
  // if this condition is fulfilled for any sensor then we can assume that user is standing in the wrong position

  for(uint8_t i=0;i<5;i++)
  {
    sen_min_val = sens.calibrated_weight[i] - POSITION_WEIGHT_MAX_DIFF;
    sen_max_val = sens.calibrated_weight[i] + POSITION_WEIGHT_MAX_DIFF;

    // if any sensor's value is significantly different from calibrated value (and user is standing) then we can assume that user is standing in the wrong position
    if(sens.s[i].weight > sen_max_val || sens.s[i].weight < sen_min_val)
    {
      pos_now.position_wrong = feet_prev.user == STANDING ? true : false;
      break;
    }

  }

  // if we just recognized wrong position then we need to calculate time stamp for the warning
  // warning isn't instant if user is standing in the wrong position for more than 2 sec then it occurs
  // correcting position will cause resetting warning time stamp to '0'
  pos_now.warning_at_ts = (pos_now.position_wrong && !pos_prev.position_wrong) ? (now + MOTOR_TIME_TO_WARNING) : 0;

  // we don't want to annoy user with constant warnings so there is extra ts for last warning
  // which is also checked before executing next one
  if((now >= pos_now.warning_at_ts) && (now - pos_now.last_warning_ts) > MOTOR_NEXT_WARNING_TIMEOUT)
  {
    pos_now.last_warning_ts = now;
    // device is giving the warning by long vibration
    motor_vibrate(LONG_WIBRATION);
  }

  pos_prev = pos_now;
}

void sens_handler()
{

  if(adc_conv_complete_flag)
  {
    // after successful adc conversion of all channels execute all calculations
    transform_adc_to_weight();
    generate_colours();
    sensors_check();
    position_check();

    adc_conv_complete_flag = false;
  }
}

void sens_reset_timestamp() { reset_cnt = true; };

void sens_calibrate()
{
  // updating calibrated sensors values and keeping the distance from boundary values

  for(uint8_t i=0;i<5;i++)
  {
    if((sens.s[i].weight - POSITION_WEIGHT_MAX_DIFF) < MIN_KG)
      sens.calibrated_weight[i] = (MIN_KG + POSITION_WEIGHT_MAX_DIFF);
    else if((sens.s[i].weight + POSITION_WEIGHT_MAX_DIFF) > MAX_KG)
      sens.calibrated_weight[i] = (MAX_KG - POSITION_WEIGHT_MAX_DIFF);
    else
      sens.calibrated_weight[i] = sens.s[i].weight;
  }
}

// checking whether the calibrated values ​​keep the distance from minimal and maximal sensors values
bool check_are_calibrated_values_ok()
{
  for(uint8_t i=0;i<5;i++)
  {
    if(((sens.calibrated_weight[i] - POSITION_WEIGHT_MAX_DIFF) < MIN_KG) ||  \
        ((sens.calibrated_weight[i] + POSITION_WEIGHT_MAX_DIFF) > MAX_KG))
      return false;
  }
  return true;
}
