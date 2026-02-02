/*
  ==============================================================================

    FoldableContainer.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "utility_components/ContentContainer.h"

namespace Feedback
{
    class FoldableContainer : public Component, Button::Listener, public ChangeBroadcaster
    {
    public:
        using ArrowButton = asvpr::ArrowButton;

        FoldableContainer(Component &header, Component &body) :
            header(header),
            body(body),
            headerHeight(header.getHeight()),
            bodyHeight(body.getHeight()),
            arrowButton(ArrowButton::Direction::Right)
        {
            addAndMakeVisible(header);
            addAndMakeVisible(body);

            arrowButton.setClickingTogglesState(true);
            arrowButton.setToggleState(false, dontSendNotification);
            arrowButton.setMouseCursor(MouseCursor::PointingHandCursor);
            arrowButton.addListener(this);
            addAndMakeVisible(arrowButton);
        }

        void paint(Graphics& g) override {
            g.setColour(Colours::white.withAlpha(0.2f));
            g.drawLine(Line<int>{getLocalBounds().getTopLeft(), getLocalBounds().getTopRight()}.toFloat(), 0.5f);
            g.drawLine(Line<int>{getLocalBounds().getBottomLeft(), getLocalBounds().getBottomRight()}.toFloat(), 0.5f);
        }

        void resized() override
        {
            auto bounds = getLocalBounds();
            header.setBounds(bounds.removeFromTop(headerHeight));
            body.setBounds(bounds.removeFromTop(bodyHeight));

            arrowButton.setBounds(header.getLocalBounds().withSizeKeepingCentre(getWidth(), 24)
                .removeFromLeft(24).translated(10, 0));
        }

        int getContainerHeight() const
        {
            if (arrowButton.getToggleState())
            {
                return headerHeight + bodyHeight;
            }
            return headerHeight;
        }

        void buttonClicked(juce::Button *button) override
        {
            if (button == &arrowButton)
            {
                arrowButton.getToggleState() ? arrowButton.setDirection(ArrowButton::Direction::Down)
                    : arrowButton.setDirection(ArrowButton::Direction::Right);
                arrowButton.repaint();
                sendChangeMessage();
            }
        }

    private:
        Component &header;
        Component &body;
        int headerHeight, bodyHeight;
        ArrowButton arrowButton;
    };
}