#include <gui/lightscreen_screen/LightScreenView.hpp>
#include <gui/lightscreen_screen/LightScreenPresenter.hpp>

LightScreenPresenter::LightScreenPresenter(LightScreenView& v)
    : view(v)
{

}

void LightScreenPresenter::activate()
{
  // pass color from model to view init function
  view.initDefaultColor(model->getDefaultColor());
}

void LightScreenPresenter::deactivate()
{

}

void LightScreenPresenter::updateDefaultColor(unsigned long color)
{
  // passing default color value to be stored in model
  model->saveDefaultColor(color);
}

void LightScreenPresenter::updateColor(unsigned long color)
{
  // passing color value just to update PWM channels
  model->updateColor(color);
}
