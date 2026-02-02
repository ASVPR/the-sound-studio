/*
  ==============================================================================

    ChannelHeader.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "FeedbackSpectrumSettingsComponent.h"

namespace Feedback {
    class ChannelHeader : public Component {
    public:
        ChannelHeader(ProjectManager& pm, FundamentalFrequencyProcessor& ffp, const String& name, Component* parent) :
            name(name), settings(pm, ffp), parent(parent)
        {
            const auto settingsImage = ImageCache::getFromMemory(BinaryData::settings2x_png, BinaryData::settings2x_pngSize);
            settingsButton.setImages(false, true, true, settingsImage, 1.f, Colour{}, Image{}, 1.f, Colour{}, Image{}, 1.f, Colour{});
            settingsButton.setMouseCursor(MouseCursor::PointingHandCursor);
            settingsButton.onClick = [&] {
                settings.setVisible(true);
                settings.toFront(true);
            };

            addAndMakeVisible(settingsButton);
            parent->addChildComponent(settings);
            setSize(0, 40);
        }
        void paint(Graphics& g) override {
            g.setFont(15.f);
            g.setColour(Colours::white);
            g.drawFittedText(name, getLocalBounds().reduced(5).translated(30, 0), Justification::centredLeft, 1);
        }
        void resized() override {
            auto bounds = getLocalBounds().reduced(5);
            bounds.removeFromRight(20);
            settingsButton.setBounds(bounds.removeFromRight(20));
            settings.setBounds(parent->getLocalBounds());
        }

    private:
        Component* parent;
        String name;
        ImageButton settingsButton;
        FeedbackSpectrumSettingsComponent settings;
    };
}
