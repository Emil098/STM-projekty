/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#ifndef ENVCONTAINERBASE_HPP
#define ENVCONTAINERBASE_HPP

#include <gui/common/FrontendApplication.hpp>
#include <touchgfx/containers/Container.hpp>
#include <touchgfx/widgets/TextAreaWithWildcard.hpp>
#include <touchgfx/widgets/TextArea.hpp>
#include <touchgfx/widgets/ScalableImage.hpp>
#include <touchgfx/widgets/Image.hpp>
#include <touchgfx/containers/progress_indicators/CircleProgress.hpp>
#include <touchgfx/widgets/canvas/PainterRGB565Bitmap.hpp>

class envContainerBase : public touchgfx::Container
{
public:
    envContainerBase();
    virtual ~envContainerBase();
    virtual void initialize();

protected:
    FrontendApplication& application() {
        return *static_cast<FrontendApplication*>(touchgfx::Application::getInstance());
    }

    /*
     * Member Declarations
     */
    touchgfx::TextAreaWithOneWildcard textAreaTemp;
    touchgfx::TextArea PresLabel;
    touchgfx::TextArea PresUnitLabel;
    touchgfx::TextArea HumLabel;
    touchgfx::TextArea HumUnitLabel;
    touchgfx::TextArea TempLabel;
    touchgfx::TextArea TempUnitLabel;
    touchgfx::ScalableImage PreassureImage;
    touchgfx::Image HumImage;
    touchgfx::ScalableImage scalableImage1;
    touchgfx::CircleProgress circleProgressTemp;
    touchgfx::PainterRGB565Bitmap circleProgressTempPainter;
    touchgfx::CircleProgress circleProgressHum;
    touchgfx::PainterRGB565Bitmap circleProgressHumPainter;
    touchgfx::CircleProgress circleProgressPreas;
    touchgfx::PainterRGB565Bitmap circleProgressPreasPainter;
    touchgfx::TextAreaWithOneWildcard textAreaHum;
    touchgfx::TextAreaWithOneWildcard textAreaPreas;
    touchgfx::Image alertBgImage;
    touchgfx::TextAreaWithOneWildcard textAreaAlert1;
    touchgfx::TextAreaWithOneWildcard textAreaAlert2;

    /*
     * Wildcard Buffers
     */
    static const uint16_t TEXTAREATEMP_SIZE = 5;
    touchgfx::Unicode::UnicodeChar textAreaTempBuffer[TEXTAREATEMP_SIZE];
    static const uint16_t TEXTAREAHUM_SIZE = 5;
    touchgfx::Unicode::UnicodeChar textAreaHumBuffer[TEXTAREAHUM_SIZE];
    static const uint16_t TEXTAREAPREAS_SIZE = 7;
    touchgfx::Unicode::UnicodeChar textAreaPreasBuffer[TEXTAREAPREAS_SIZE];
    static const uint16_t TEXTAREAALERT1_SIZE = 45;
    touchgfx::Unicode::UnicodeChar textAreaAlert1Buffer[TEXTAREAALERT1_SIZE];
    static const uint16_t TEXTAREAALERT2_SIZE = 45;
    touchgfx::Unicode::UnicodeChar textAreaAlert2Buffer[TEXTAREAALERT2_SIZE];

private:

};

#endif // ENVCONTAINERBASE_HPP
