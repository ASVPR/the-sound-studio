/*
  ==============================================================================

    MenuViewInterface.h
    Created: 19 Feb 2022 10:09:19am
    Author:  Akash Murthy

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class MenuViewInterface : public Component
{
public:
    virtual void setScale(float scaleFactor) = 0;
};
