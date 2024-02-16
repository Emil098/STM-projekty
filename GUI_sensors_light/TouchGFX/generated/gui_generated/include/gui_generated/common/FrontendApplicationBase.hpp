/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#ifndef FRONTENDAPPLICATIONBASE_HPP
#define FRONTENDAPPLICATIONBASE_HPP

#include <mvp/MVPApplication.hpp>
#include <gui/model/Model.hpp>

class FrontendHeap;

class FrontendApplicationBase : public touchgfx::MVPApplication
{
public:
    FrontendApplicationBase(Model& m, FrontendHeap& heap);
    virtual ~FrontendApplicationBase() { }

    virtual void changeToStartScreen()
    {
        gotoDefaultScreenScreenNoTransition();
    }

    // DefaultScreen
    void gotoDefaultScreenScreenNoTransition();

    void gotoDefaultScreenScreenSlideTransitionNorth();

    // LightScreen
    void gotoLightScreenScreenSlideTransitionSouth();

protected:
    touchgfx::Callback<FrontendApplicationBase> transitionCallback;
    FrontendHeap& frontendHeap;
    Model& model;

    // DefaultScreen
    void gotoDefaultScreenScreenNoTransitionImpl();

    void gotoDefaultScreenScreenSlideTransitionNorthImpl();

    // LightScreen
    void gotoLightScreenScreenSlideTransitionSouthImpl();
};

#endif // FRONTENDAPPLICATIONBASE_HPP
