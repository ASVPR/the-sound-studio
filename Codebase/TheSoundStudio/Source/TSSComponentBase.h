/*
  ==============================================================================

    TSSComponentBase.h
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "CustomLookAndFeel.h"
#include "TSSConstants.h"

class TSSComponentBase : public juce::Component
{
public:
    TSSComponentBase()
    {
        setLookAndFeel(&lookAndFeel);
    }

    ~TSSComponentBase() override
    {
        setLookAndFeel(nullptr);
    }

    virtual void setScale(float factor)
    {
        if (scaleFactor != factor)
        {
            scaleFactor = factor;
            lookAndFeel.setScale(scaleFactor);
            onScaleChanged();
            resized();
            repaint();
        }
    }

    float getScaleFactor() const { return scaleFactor; }

protected:
    virtual void onScaleChanged() {}

    float scaleFactor = TSS::UI::kDefaultScaleFactor;
    CustomLookAndFeel lookAndFeel;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TSSComponentBase)
};
