/*
  ==============================================================================

    CustomRotarySlider.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

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
