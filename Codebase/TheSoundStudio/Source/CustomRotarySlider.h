/*
  ==============================================================================

    CustomRotarySlider.h
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

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
