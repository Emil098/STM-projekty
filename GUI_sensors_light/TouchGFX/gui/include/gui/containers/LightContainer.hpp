#ifndef LIGHTCONTAINER_HPP
#define LIGHTCONTAINER_HPP

#include <gui_generated/containers/LightContainerBase.hpp>

class LightContainer : public LightContainerBase
{
public:
    LightContainer();
    virtual ~LightContainer() {}

    virtual void initialize();

    virtual void updateContent(uint8_t brightness, bool LEDsOn);
    virtual void getContent(uint8_t *brightness, bool *LEDsOn);
protected:
};

#endif // LIGHTCONTAINER_HPP
