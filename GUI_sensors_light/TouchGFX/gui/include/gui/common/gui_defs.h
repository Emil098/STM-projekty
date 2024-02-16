/*
 * gui_defs.h
 *
 *  Created on: Feb 14, 2024
 *      Author: emilr
 */

#ifndef GUI_DEFS_H_
#define GUI_DEFS_H_

#include <stdint.h>
#include <stdbool.h>

// mapping values due to different ranges
#define MAP(x, in_min, in_max, out_min, out_max) ((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)

typedef struct
{
  uint32_t color;
  uint8_t brightness;
  bool LEDs_on;
}color_settings_st;

typedef struct
{
  uint16_t co2;
  float temp;
  float humi;
  uint16_t preas;
}enviroment_struct;


#endif /* GUI_DEFS_H_ */
