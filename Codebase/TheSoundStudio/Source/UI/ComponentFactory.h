/*
  ==============================================================================

    ComponentFactory.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once

#include "../../JuceLibraryCode/JuceHeader.h"
#include <memory>

namespace TSS { namespace UI {

template<typename ButtonType = juce::ImageButton>
std::unique_ptr<ButtonType> createImageButton(
    const juce::String& name,
    const juce::Image& normalImage,
    juce::Button::Listener* listener,
    juce::Component* parent,
    bool triggeredOnMouseDown = true)
{
    auto button = std::make_unique<ButtonType>(name);
    button->setTriggeredOnMouseDown(triggeredOnMouseDown);
    button->setImages(false, true, true,
                      normalImage, 0.999f, juce::Colour(0x00000000),
                      juce::Image(), 1.0f, juce::Colour(0x00000000),
                      normalImage, 0.75f, juce::Colour(0x00000000));
    button->addListener(listener);
    parent->addChildComponent(*button);
    return button;
}

template<typename ButtonType = juce::ImageButton>
std::unique_ptr<ButtonType> createImageButtonWithOverState(
    const juce::String& name,
    const juce::Image& normalImage,
    const juce::Image& overImage,
    juce::Button::Listener* listener,
    juce::Component* parent,
    bool triggeredOnMouseDown = true)
{
    auto button = std::make_unique<ButtonType>(name);
    button->setTriggeredOnMouseDown(triggeredOnMouseDown);
    button->setImages(false, true, true,
                      normalImage, 0.999f, juce::Colour(0x00000000),
                      overImage, 1.0f, juce::Colour(0x00000000),
                      normalImage, 0.75f, juce::Colour(0x00000000));
    button->addListener(listener);
    parent->addChildComponent(*button);
    return button;
}

}} // namespace TSS::UI
