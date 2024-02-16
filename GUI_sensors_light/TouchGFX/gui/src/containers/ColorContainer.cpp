#include <gui/containers/ColorContainer.hpp>
#include <string.h>

ColorContainer::ColorContainer()
{

}

void ColorContainer::initialize()
{
    ColorContainerBase::initialize();
}

// function to select button according to passed color
void ColorContainer::select_button(uint32_t color,char *btn_name)
{
  // in case of no color detection
  strcpy(btn_name,"Custom");

  // if any button is selected we need to deselect it
  int index = radioButtonColors.getSelectedRadioButtonIndex();
  if(index != -1)
    radioButtonColors.getSelectedRadioButton()->setSelected(false);

  for(uint8_t i=0;i<15;i++)
  {
    // if passed color matches button color then select the button
    if(btn_info[i].color == color)
    {
      radioButtonColors.getRadioButton(i)->setSelected(true);
      strcpy(btn_name, btn_info[i].name);
    }
  }
}

// function passing the color name of selected button
void ColorContainer::get_button_name(char *name)
{
  int32_t index = radioButtonColors.getSelectedRadioButtonIndex();

  if(index != -1)
    strcpy(name,btn_info[index].name);
  else
    strcpy(name,"Custom");
}
