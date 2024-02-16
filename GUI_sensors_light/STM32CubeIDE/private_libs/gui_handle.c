/*
 * gui_handle.c
 *
 *  Created on: 11 lut 2024
 *      Author: emilr
 */

#include <string.h>
#include "gui_handle.h"

color_settings_st c_info;
enviroment_struct env_info = {1010, 22.1, 34.1, 1011};  // dummy struct just to check is mvp working properly
// setting new color
void set_pwm_color(uint32_t col)
{
  c_info.color = col;
  c_info.brightness = 255;

  COLOR_UPDATE(col);
}

// updating brightness (function called by the Model)
void set_brightness(uint8_t brightness, bool save)
{
  uint8_t r,g,b;
  uint32_t new_col = 0;
  // saving brightness if needed
  if(save)
  {
    c_info.brightness = brightness;
//    c_info.LEDs_on = true;
  }
  // updating brightness only if toggle button is on
  if(!c_info.LEDs_on)
    return;

  // separating red, green and blue values from color
  r = ((uint8_t)(c_info.color >> 16));
  g = ((uint8_t)(c_info.color >> 8));
  b = ((uint8_t)(c_info.color));

  // mapping them for new brightness
  r = MAP(r,0,255,0,brightness);
  g = MAP(g,0,255,0,brightness);
  b = MAP(b,0,255,0,brightness);

  // creating color value
  new_col = (r << 16) | (g << 8) | b;

  // updating pwm channels
  COLOR_UPDATE(new_col);
}

// enabling/disabling led's
void set_led_status(bool status)
{
  c_info.LEDs_on = status;

  if(status)
    set_brightness(c_info.brightness, false);
  else
    COLOR_UPDATE(0);

}

// receiving measurements from bme680 sensor
void update_env_struct(enviroment_struct env)
{
  memcpy((void *)&env_info,(void *)&env,sizeof(enviroment_struct));
}

// passing light settings to gui model
void get_light_struct(color_settings_st *light) { *light = c_info; }

// passing received measurements to gui model
void get_env_struct(enviroment_struct *env) {*env = env_info;}
