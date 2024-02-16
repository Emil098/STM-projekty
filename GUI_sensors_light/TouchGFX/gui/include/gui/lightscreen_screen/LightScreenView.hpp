#ifndef LIGHTSCREENVIEW_HPP
#define LIGHTSCREENVIEW_HPP

#include <gui_generated/lightscreen_screen/LightScreenViewBase.hpp>
#include <gui/lightscreen_screen/LightScreenPresenter.hpp>

class LightScreenView : public LightScreenViewBase
{
public:
    LightScreenView();
    virtual ~LightScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void initDefaultColor(unsigned int col);

    virtual void updateDefaultColor();              // "Set as default color" button callback
    virtual void NewColorSelected(uint32_t value);  // common radio buttons callback
    virtual void sliderChaged(uint8_t value);       // function called after changing position of any slider on this screen

protected:
    uint32_t color;         // color hex value
    bool force_slider_exit; // this variable prevent harmful entering to sliderChaged

};

#endif // LIGHTSCREENVIEW_HPP
