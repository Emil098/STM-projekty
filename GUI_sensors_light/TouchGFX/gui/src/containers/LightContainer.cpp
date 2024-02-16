#include <gui/containers/LightContainer.hpp>

LightContainer::LightContainer()
{

}

void LightContainer::initialize()
{
    LightContainerBase::initialize();
}

void LightContainer::updateContent(uint8_t brightness, bool LEDsOn)
{
  // forcing btn state and slider value during screen initializing
  LedOnOffBtn.forceState(LEDsOn);
  sliderBrightness.setValue(brightness);
}

void LightContainer::getContent(uint8_t *brightness, bool *LEDsOn)
{
  // passing btn state and slider value to view
  *LEDsOn = LedOnOffBtn.getState();
  *brightness = (uint8_t)sliderBrightness.getValue();
}
