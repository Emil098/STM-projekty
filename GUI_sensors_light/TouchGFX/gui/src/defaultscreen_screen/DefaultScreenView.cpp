#include <gui/defaultscreen_screen/DefaultScreenView.hpp>

#include <string.h>


DefaultScreenView::DefaultScreenView() : last_selected_page(1)
{

}

void DefaultScreenView::setupScreen()
{
    DefaultScreenViewBase::setupScreen();
}

void DefaultScreenView::tearDownScreen()
{
    DefaultScreenViewBase::tearDownScreen();
}

void DefaultScreenView::ScreenRoutine()
{
  // TouchGFX does not support callback for new selected page so we need to do it in tick routine
  uint8_t selected_page = swipeContainer1.getSelectedPage();

  if(last_selected_page != selected_page)
  {
    // assigning new title text according to present page
    switch(selected_page)
    {
    case PAGE_ENV:
      Unicode::strncpy(Title_textBuffer, "Environmental page", TITLE_TEXT_SIZE);
      break;
    case PAGE_LIGHT:
      Unicode::strncpy(Title_textBuffer, "Light page", TITLE_TEXT_SIZE);
      break;
    }
    Title_text.invalidate();
  }

  last_selected_page = swipeContainer1.getSelectedPage();

#ifndef SIMULATOR
  enviroment_struct e;

  // receiving enviromental data from presenter and passing them to the envContainer
  presenter->getDefaultScreenData(NULL, &e);

  if(memcmp((void *) &e, (void *) &e_info, sizeof(enviroment_struct)) != 0)
  {
    envContainer1.updateEnvContent(e);
    memcpy((void *) &e_info, (void *) &e, sizeof(enviroment_struct));
  }
#endif
}

#ifndef SIMULATOR

// function callback after changing any lightContainer element
// function is passing widgets state from the container to the presenter
void DefaultScreenView::LightWidgetUpdate()
{
  color_settings_st c_info;
  lightContainer1.getContent(&c_info.brightness, &c_info.LEDs_on);
  presenter->updateLightInfo(c_info);

}

// receiving light settings from presenter and passing them to the lightContainer
void DefaultScreenView::initLightPage(color_settings_st c_info)
{
  swipeContainer1.setSelectedPage(last_selected_page);
  lightContainer1.updateContent(c_info.brightness, c_info.LEDs_on);
}

#endif
