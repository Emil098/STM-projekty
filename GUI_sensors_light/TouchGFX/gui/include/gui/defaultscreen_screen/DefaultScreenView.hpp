#ifndef DEFAULTSCREENVIEW_HPP
#define DEFAULTSCREENVIEW_HPP

#include <gui_generated/defaultscreen_screen/DefaultScreenViewBase.hpp>
#include <gui/defaultscreen_screen/DefaultScreenPresenter.hpp>

#ifndef SIMULATOR
#include "gui_defs.h"
#endif


enum pages
{
  PAGE_ENV = 0,
  PAGE_LIGHT
};

class DefaultScreenView : public DefaultScreenViewBase
{
public:
    DefaultScreenView();
    virtual ~DefaultScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    virtual void ScreenRoutine();

#ifndef SIMULATOR
    virtual void initLightPage(color_settings_st c_info);
    virtual void LightWidgetUpdate();
#endif


protected:
    uint8_t last_selected_page;
#ifndef SIMULATOR
    color_settings_st c_info;
    enviroment_struct e_info;
#endif
};

#endif // DEFAULTSCREENVIEW_HPP
