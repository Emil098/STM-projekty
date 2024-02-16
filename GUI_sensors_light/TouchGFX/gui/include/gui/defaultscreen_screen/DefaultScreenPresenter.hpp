#ifndef DEFAULTSCREENPRESENTER_HPP
#define DEFAULTSCREENPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

#ifndef SIMULATOR
#include "gui_defs.h"
#endif


using namespace touchgfx;

class DefaultScreenView;

class DefaultScreenPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    DefaultScreenPresenter(DefaultScreenView& v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    virtual ~DefaultScreenPresenter() {};

#ifndef SIMULATOR
    virtual void getDefaultScreenData(color_settings_st *c_info, enviroment_struct *e_info);
    virtual void updateLightInfo(color_settings_st c_info);
#endif

private:
    DefaultScreenPresenter();

    DefaultScreenView& view;
};

#endif // DEFAULTSCREENPRESENTER_HPP
