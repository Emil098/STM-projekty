#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>


Model::Model() : modelListener(0), defaultColor(0), brightness(0), LEDsOn(false)
{

}

void Model::tick()
{

}

void Model::saveDefaultColor(unsigned int col)
{
  defaultColor = col;
// when simulator is compiled there can't be any communication between model and backend
// if touchGFX is compiling simulator then to header file SIMULATOR macro is added
#ifndef SIMULATOR
  set_pwm_color(defaultColor);  // updating pwm channels and saving color hex value in backend
#endif
}

void Model::updateColor(unsigned int col)
{
#ifndef SIMULATOR
  COLOR_UPDATE(col);  // updating pwm channels only
#endif
}

unsigned int Model::getDefaultColor()
{
  COLOR_UPDATE(defaultColor);    //function called at LightScreen init, we need set pwm channels
  return defaultColor;  //passing color to DefaultScreen presenter
}

#ifndef SIMULATOR

void Model::getDefaultScreenData(color_settings_st *c_info, enviroment_struct *e_info)
{
  // receiving light settings and environment measurements from backend
  get_env_struct(e_info);
  get_light_struct(c_info);
}

void Model::updateLightInfo(color_settings_st c_info)
{
  // updating light settings in backend
  set_led_status(c_info.LEDs_on);
  set_brightness(c_info.brightness, true);

}

#endif
