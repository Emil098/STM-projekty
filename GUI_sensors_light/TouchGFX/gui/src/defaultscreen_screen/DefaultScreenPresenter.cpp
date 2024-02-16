#include <gui/defaultscreen_screen/DefaultScreenView.hpp>
#include <gui/defaultscreen_screen/DefaultScreenPresenter.hpp>


DefaultScreenPresenter::DefaultScreenPresenter(DefaultScreenView& v)
    : view(v)
{

}

void DefaultScreenPresenter::activate()
{
  // during screen initialization update screen view content with data received from the Model
#ifndef SIMULATOR
  color_settings_st c_info;
  model->getDefaultScreenData(&c_info,NULL);
  view.initLightPage(c_info);
#endif
}

void DefaultScreenPresenter::deactivate()
{

}

#ifndef SIMULATOR

// common function for receiving model data for default screen view
void DefaultScreenPresenter::getDefaultScreenData(color_settings_st *c_info, enviroment_struct *e_info)
{
  model->getDefaultScreenData(c_info,e_info);
}

// funtion updating light setting in model
void DefaultScreenPresenter::updateLightInfo(color_settings_st c_info)
{
  model->updateLightInfo(c_info);
}
#endif
