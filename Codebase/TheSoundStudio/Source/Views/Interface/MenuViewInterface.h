/*
  ==============================================================================

    MenuViewInterface.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class MenuViewInterface : public Component
{
public:
    virtual void setScale(float scaleFactor) = 0;
};
