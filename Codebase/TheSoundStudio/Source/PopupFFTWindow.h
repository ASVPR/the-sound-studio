/*
  ==============================================================================

    PopupFFTWindow.h
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class PopupFFTWindow : public DocumentWindow
{
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PopupFFTWindow)

public:
    PopupFFTWindow (const String& name, Component* newContentComponent, Colour backgroundColour, int buttonsNeeded, bool addToDesktop)
    : DocumentWindow (name, backgroundColour, buttonsNeeded, addToDesktop)
    {
        setContentOwned(newContentComponent, true);
        
//        setContentNonOwned(newContentComponent, true);
    }

    void closeButtonPressed() override
    {
        setVisible(false);
    }
    
    Image logoImage;
    
};
