/*
  ==============================================================================

    MenuViewInterface.h
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class MenuViewInterface : public Component
{
public:
    virtual void setScale(float scaleFactor) = 0;
};
