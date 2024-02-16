#ifndef MODEL_HPP
#define MODEL_HPP

#ifdef SIMULATOR
#include "gui_defs.h"
#else
extern "C"
{
#include "main.h"
#include "gui_handle.h"
}
#endif

class ModelListener;

class Model
{
public:
    Model();

    void bind(ModelListener* listener)
    {
        modelListener = listener;
    }

    void tick();

    void saveDefaultColor(unsigned int col);
    unsigned int getDefaultColor();
    void updateColor(unsigned int col);
#ifndef SIMULATOR
    void getDefaultScreenData(color_settings_st *c_info, enviroment_struct *e_info);
    void updateLightInfo(color_settings_st c_info);
#endif
protected:
    ModelListener* modelListener;
    unsigned int defaultColor;
    uint8_t brightness;
    bool LEDsOn;
};

#endif // MODEL_HPP
