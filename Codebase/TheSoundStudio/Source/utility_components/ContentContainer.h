/*
  ==============================================================================

    ContentContainer.h
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace asvpr
{

class ArrowButton : public juce::Button
{
public:
    enum class Direction
    {
        Up,
        Down,
        Left,
        Right
    };

    ArrowButton(Direction direction) :
    juce::Button(""),
    arrowDirection(direction)
    {

    }

    void setDirection(Direction newDirection)
    {
        arrowDirection = newDirection;
    }

    void paintButton(juce::Graphics& graphics, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
    {
        graphics.setColour(juce::Colours::white);

        auto bounds = getLocalBounds().toFloat();
        bounds = bounds.withSizeKeepingCentre(bounds.getWidth() * 0.3,
                                              bounds.getHeight() * 0.3);

        graphics.strokePath(getPath(arrowDirection, bounds),
                            juce::PathStrokeType(2,
                                                 juce::PathStrokeType::JointStyle::curved,
                                                 juce::PathStrokeType::EndCapStyle::rounded));
    }

    static juce::Path getPath(Direction direction, const juce::Rectangle<float>& bounds)
    {
        juce::Path path;

        juce::Point<float> startPoint;

        juce::Point<float> midPoint;

        juce::Point<float> endPoint;

        juce::Point<float> topMid { bounds.getX() + bounds.getWidth() / 2, bounds.getY()  };

        juce::Point<float> leftMid { bounds.getX(), bounds.getY() + bounds.getHeight() / 2};

        juce::Point<float> rightMid { bounds.getX() + bounds.getWidth(),
                                      bounds.getY() + bounds.getHeight() / 2 };

        juce::Point<float> bottomMid { bounds.getX() + bounds.getWidth() / 2,
                                       bounds.getY() + bounds.getHeight() };

        if (direction == Direction::Left)
        {
            startPoint = topMid;
            midPoint = leftMid;
            endPoint= bottomMid;
        }
        else if (direction == Direction::Right)
        {
            startPoint = topMid;
            midPoint = rightMid;
            endPoint= bottomMid;
        }
        else if (direction == Direction::Up)
        {
            startPoint = leftMid;
            midPoint = topMid;
            endPoint= rightMid;
        }
        else if (direction == Direction::Down)
        {
            startPoint = leftMid;
            midPoint = bottomMid;
            endPoint= rightMid;
        }

        path.startNewSubPath(startPoint);

        path.lineTo(midPoint);

        path.lineTo(endPoint);

        return path;
    }

    Direction arrowDirection;
};

class ContentContainer : public juce::Component,
public juce::Button::Listener
{
public:
    ContentContainer(const juce::String& titleText, bool withArrowButton) : title(titleText)
    {
        if (titleText.isNotEmpty())
        {
            setName(titleText);
        }

        if (withArrowButton)
        {
            arrowButton = std::make_unique<ArrowButton>(ArrowButton::Direction::Down);
            addAndMakeVisible(arrowButton.get());
            arrowButton->addListener(this);
        }
    }

    void paint(juce::Graphics& graphics) override
    {
        graphics.setColour({70, 75, 84});
        graphics.fillRoundedRectangle(getLocalBounds().toFloat(), 3);


        paintInBackground(graphics, getLocalBounds());

        graphics.setFont(juce::Font().boldened().withHeight(12));

        graphics.setColour(juce::Colours::white);

        if (showTitle)
        {
            auto localBounds = getLocalBounds();
            localBounds.removeFromTop(10);
            localBounds.removeFromLeft(25);
            graphics.drawFittedText(title,
                                    localBounds.removeFromTop(20),
                                    juce::Justification::centredLeft,
                                    1);
        }
    }

    void resized() override
    {
        auto bounds = getLocalBounds();

        if (arrowButton)
        {
            auto arrowBounds = bounds;
            arrowBounds.removeFromTop(10);
            arrowBounds = arrowBounds.removeFromLeft(25);
            arrowButton->setBounds(arrowBounds.removeFromTop(20));
        }
    }

    void disableTitle(bool disable)
    {
        showTitle = !disable;
    }

    void showContent(bool show)
    {
        auto arrowDirection = show ? ArrowButton::Direction::Down : ArrowButton::Direction::Right;

        arrowButton->setDirection(arrowDirection);

        showingContent = show;
    }

    bool isShowingContent() const
    {
        if (!isEnabled())
        {
            return false;
        }
        
        return showingContent;
    }

    void buttonClicked(juce::Button * button) override
    {
        if (button == arrowButton.get())
        {
            showContent(!isShowingContent());
            if (auto parent = getParentComponent())
            {
                parent->resized();
            }
        }
    }


    std::function<void(juce::Graphics&, const juce::Rectangle<int>&)> paintInBackground = [] (auto& g, const auto& r) {};

private:

    bool showingContent { true };

    bool showTitle { true };

    juce::String title;

    std::unique_ptr<asvpr::ArrowButton> arrowButton;

    std::unique_ptr<juce::Viewport> viewport;

    ArrowButton::Direction arrowDirection;

    int width { 100 };

    int height { 60 };
};


} /* asvpr */
