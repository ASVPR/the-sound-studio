/*
  ==============================================================================

    CustomRotarySlider.h
    Created: 30 Jun 2019 11:16:32am
    Author:  Gary Jones

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"


class CustomRotarySlider : public Slider
{
public:
    enum ROTARY_TYPE {
        ROTARY_AMPLITUDE = 0,
        ROTARY_ATTACK,
        ROTARY_DECAY,
        ROTARY_SUSTAIN,
        ROTARY_RELEASE
    } rotaryType;
    
    CustomRotarySlider(ROTARY_TYPE newType)
    {
        rotaryType = newType;
        
    }
};
