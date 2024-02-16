#include <gui/containers/envContainer.hpp>


envContainer::envContainer()
{

}

void envContainer::initialize()
{
    envContainerBase::initialize();
}

#ifndef SIMULATOR
void envContainer::updateEnvContent(enviroment_struct e_info)
{
  int TempProgress, HumProgress, presProgress;
  char line_1[45] = {};
  char *line_2_pt;
  int size;

  if(strlen(e_info.alert) != 0)
  {
    line_2_pt = strchr(e_info.alert, ':'); // new line before error reason
    line_2_pt += 2; // in alert there is "api_name : reason" so we need to add 2
    size = line_2_pt - ((char *) e_info.alert);  // line 1 size

    strncpy(line_1, e_info.alert, size - 1); // Unicode::strncpy doesn't work very well and copy whole string so we need to create separate string for line 1

    // copying strings to text area buffers
    Unicode::strncpy(textAreaAlert1Buffer, line_1, size );
    Unicode::strncpy(textAreaAlert2Buffer, line_2_pt , TEXTAREAALERT2_SIZE);

    // setting visible all elements and invalidating them
    alertBgImage.setVisible(true);
    textAreaAlert1.setVisible(true);
    textAreaAlert2.setVisible(true);
    alertBgImage.invalidate();
    textAreaAlert1.invalidate();
    textAreaAlert2.invalidate();
    // exiting function after updating alert
    return;
  }

  //if there is no alert and alert background is visible we need to hide all alert elements
  if(alertBgImage.isVisible())
  {
    alertBgImage.setVisible(false);
    textAreaAlert1.setVisible(false);
    textAreaAlert2.setVisible(false);
    alertBgImage.invalidate();
    textAreaAlert1.invalidate();
    textAreaAlert2.invalidate();
  }

  // mapping values for progress circles
  TempProgress = MAP(e_info.temp, 0, 35, 0, 100);
  HumProgress  = (uint8_t)e_info.humi;
  presProgress = MAP(e_info.preas, 950, 1050, 0, 100);

  circleProgressTemp.setValue(TempProgress);
  circleProgressHum.setValue(HumProgress);
  circleProgressPreas.setValue(presProgress);

  circleProgressTemp.invalidate();
  circleProgressHum.invalidate();
  circleProgressPreas.invalidate();

  // updating values in text areas
  Unicode::snprintfFloat(textAreaTempBuffer, TEXTAREATEMP_SIZE, "%.1f", e_info.temp);
  textAreaTemp.invalidate();

  Unicode::snprintfFloat(textAreaHumBuffer, TEXTAREAHUM_SIZE, "%.1f",e_info.humi);
  textAreaHum.invalidate();

  Unicode::snprintf(textAreaPreasBuffer, TEXTAREAPREAS_SIZE, "%d", e_info.preas);
  textAreaPreas.invalidate();
}
#endif
