#include <gui/lightscreen_screen/LightScreenView.hpp>
#include <string.h>

LightScreenView::LightScreenView() : color(0), force_slider_exit(true)
{

}

void LightScreenView::setupScreen()
{
    LightScreenViewBase::setupScreen();
}

void LightScreenView::tearDownScreen()
{
    LightScreenViewBase::tearDownScreen();
}

void LightScreenView::initDefaultColor(unsigned int col)
{
  color = col;
  char color_name[TEXTAREACOLORNAME_SIZE] = {};

  // this variable prevent harmful entering to sliderChaged
  force_slider_exit = true;

  // during screen initialization we need to adjust sliders to default color received from model
  // invalidating slider will cause sliderChaged callback (it would be harmful feedback)
  sliderRed.setValue((uint8_t)(color >> 16));
  sliderRed.invalidate();
  sliderGreen.setValue((uint8_t)(color >> 8));
  sliderGreen.invalidate();
  sliderBlue.setValue((uint8_t)(color));
  sliderBlue.invalidate();
  // according to default color, check for any predefined buttons color (select button and pass the name if color matches)
  colorContainer1.select_button((uint32_t)col,color_name);
  Unicode::strncpy(textAreaColorNameBuffer, color_name, TEXTAREACOLORNAME_SIZE);
  textAreaColorName.invalidate();

  // after initializing user can adjust sliders
  force_slider_exit = false;
}

// "Set as default color" button callback
void LightScreenView::updateDefaultColor()
{
  // save color value in model (pass through presenter)
  presenter->updateDefaultColor(color);
}

// callback common for all radio buttons
// all radio buttons have assigned their own color (uint32_t value)
void LightScreenView::NewColorSelected(uint32_t value)
{
  char color_name[TEXTAREACOLORNAME_SIZE] = {};

  // ignore sliderChaged callback
  force_slider_exit = true;

  // udpate PWM channels in model
  color = value;
  presenter->updateColor(color);

  // update sliders positions
  sliderRed.setValue((uint8_t)(color >> 16));
  sliderRed.invalidate();
  sliderGreen.setValue((uint8_t)(color >> 8));
  sliderGreen.invalidate();
  sliderBlue.setValue((uint8_t)(color));
  sliderBlue.invalidate();

  // get color name from colorContainer and pass it to text area
  colorContainer1.get_button_name(color_name);
  Unicode::strncpy(textAreaColorNameBuffer, color_name, TEXTAREACOLORNAME_SIZE);
  textAreaColorName.invalidate();

  force_slider_exit = false;

}

// function called after changing position of any slider on this screen
void LightScreenView::sliderChaged(uint8_t value)
{
  // if anySlider.invalidate() is called we don't want to enter this callback
  if(force_slider_exit)
    return;

  char color_name[TEXTAREACOLORNAME_SIZE] = {};

  // creating color based on sliders current values
  color = (((uint8_t)sliderRed.getValue()) << 16) | (((uint8_t)sliderGreen.getValue()) << 8) | ((uint8_t)sliderBlue.getValue());
  // udpate PWM channels in model
  presenter->updateColor(color);

  // checking if new color matches any predefined from colorContainer
  // and if so then selecting proper radio button and passing color name to text area
  colorContainer1.select_button(color,color_name);
  Unicode::strncpy(textAreaColorNameBuffer, color_name, TEXTAREACOLORNAME_SIZE);
  textAreaColorName.invalidate();

}


