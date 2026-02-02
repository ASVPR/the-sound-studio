/*
  ==============================================================================

    PlayerButton.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

namespace asvpr
{

class PlayerButton : public juce::TextButton
{
public:
    enum class Type
    {
        PlayPause,
        Stop
    };

    class LookAndFeel : public juce::LookAndFeel_V4
    {
        juce::Font getTextButtonFont(juce::TextButton & button, int buttonHeight) override
        {
            return juce::Font(15).boldened();
        }

        void drawButtonBackground(juce::Graphics& g,
                                  juce::Button& button,
                                  const juce::Colour& backgroundColour,
                                  bool shouldDrawButtonAsHighlighted,
                                  bool shouldDrawButtonAsDown) override
        {
            g.setColour(backgroundColour);
            g.fillRoundedRectangle(button.getLocalBounds().toFloat(), 4);
        }
    };

    PlayerButton(Type buttonType)
    {
        type = buttonType;
        setLookAndFeel(&lookAndFeel);
        setColour(juce::TextButton::ColourIds::buttonColourId, getTypeColour(type));
        setButtonText(getTypeText(type));
    }

    ~PlayerButton()
    {
        setLookAndFeel(nullptr);
    }

    static juce::Colour getTypeColour(Type type)
    {

        if (type == Type::Stop)
        {
            return {241, 51, 72};
        }

        if (type == Type::PlayPause)
        {
            return {0, 122, 205};
        }

        return {};
    }

    static juce::String getTypeText(Type type)
    {
        switch (type)
        {
            case Type::PlayPause:
                return {"Play"};
                break;

            case Type::Stop:
                return {"Stop"};
                break;
        }
    }

    void setButtonColour(juce::Colour colour)
    {
        setColour(juce::TextButton::ColourIds::buttonColourId, colour);
    }

    void resetButtonColour()
    {
        setColour(juce::TextButton::ColourIds::buttonColourId, getTypeColour(type));
    }

    void resetButtonText()
    {
        setButtonText(getTypeText(type));
    }

private:
    Type type { Type::PlayPause };

    LookAndFeel lookAndFeel;
};

}
