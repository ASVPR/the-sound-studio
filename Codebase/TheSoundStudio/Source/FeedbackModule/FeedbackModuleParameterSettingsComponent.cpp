/*
  ==============================================================================

    FeedbackModuleParameterSettingsComponent.cpp
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#include "FeedbackModuleParameterSettingsComponent.h"

FeedbackModuleParameterSettingsComponent::FeedbackModuleParameterSettingsComponent(ProjectManager &pm) :
    projectManager(pm)
{
    Image closeButtonImage = ImageCache::getFromMemory(BinaryData::CloseButton2x_png, BinaryData::CloseButton2x_pngSize);
    Image applyButtonImage = ImageCache::getFromMemory(BinaryData::ApplyButton2x_png, BinaryData::ApplyButton2x_pngSize);
    Image blueButtonNormal = ImageCache::getFromMemory(BinaryData::BlueButton_Normal_png, BinaryData::BlueButton_Normal_pngSize);
    Image blueButtonSelected = ImageCache::getFromMemory(BinaryData::BlueButton_Selected_png, BinaryData::BlueButton_Selected_pngSize);

    closeButton.setImages(false, true, true,
        closeButtonImage, 1.f, Colour{},
        Image{}, 1.f, Colour{},
        Image{}, 1.f, Colour{});
    closeButton.setMouseCursor(MouseCursor::PointingHandCursor);
    closeButton.addListener(this);
    addAndMakeVisible(closeButton);

    applyButton.setImages(false, true, true,
        applyButtonImage, 1.f, Colour{},
        Image{}, 1.f, Colour{},
        Image{}, 1.f, Colour{});
    applyButton.setMouseCursor(MouseCursor::PointingHandCursor);
    applyButton.addListener(this);
    addAndMakeVisible(applyButton);

    simpleFrequencyButton.setImages(false, true, true,
        blueButtonNormal, 1.f, Colour{},
        Image{}, 1.f, Colour{},
        blueButtonSelected, 1.f, Colour{});
    simpleFrequencyButton.setMouseCursor(MouseCursor::PointingHandCursor);
    simpleFrequencyButton.addListener(this);
    simpleFrequencyButton.setClickingTogglesState(true);
    simpleFrequencyButton.setRadioGroupId(1, sendNotificationAsync);
    addAndMakeVisible(simpleFrequencyButton);

    phaseShiftingButton.setImages(false, true, true,
        blueButtonNormal, 1.f, Colour{},
        Image{}, 1.f, Colour{},
        blueButtonSelected, 1.f, Colour{});
    phaseShiftingButton.setMouseCursor(MouseCursor::PointingHandCursor);
    phaseShiftingButton.addListener(this);
    phaseShiftingButton.setClickingTogglesState(true);
    phaseShiftingButton.setRadioGroupId(1, sendNotificationAsync);
    addAndMakeVisible(phaseShiftingButton);
}

FeedbackModuleParameterSettingsComponent::~FeedbackModuleParameterSettingsComponent()
{
    closeButton.removeListener(this);
    applyButton.removeListener(this);
    simpleFrequencyButton.removeListener(this);
    phaseShiftingButton.removeListener(this);
}

void FeedbackModuleParameterSettingsComponent::resized()
{
    closeButton.setBounds(Bounds::get(Bounds::closeButton));
    applyButton.setBounds(Bounds::get(Bounds::applyButton));
    simpleFrequencyButton.setBounds(Bounds::get(Bounds::simpleFrequencyButton));
    phaseShiftingButton.setBounds(Bounds::get(Bounds::phaseShiftingButton));
}

void FeedbackModuleParameterSettingsComponent::paint(Graphics &g)
{
    g.setColour(Colours::black);
    g.setOpacity(0.88);
    g.fillAll();

    g.setOpacity(1.0);
    g.setColour(Colour::fromString("ff2c2c2c"));
    g.fillRoundedRectangle(Bounds::get(Bounds::container).toFloat(), 5);

    g.setColour(Colour::fromString("ff464b55"));
    g.fillRoundedRectangle(Bounds::get(Bounds::tuningMethodsContainer).toFloat(), 5);

    g.setColour(Colours::white);
    g.setFont(40 * Bounds::scale);
    g.drawFittedText("Set Feedback Parameters", Bounds::get(Bounds::setFeedbackParametersText), Justification::centred, 1);

    g.setFont(Font{}.boldened().withHeight(28 * Bounds::scale));
    g.drawFittedText("Tuning Methods", Bounds::get(Bounds::tuningMethodsText), Justification::centredLeft, 1);
    g.setFont(28 * Bounds::scale);
    g.drawFittedText("Use simple frequency changes", Bounds::get(Bounds::simpleFrequencyText), Justification::centredLeft, 1);
    g.drawFittedText("Use phase shifting", Bounds::get(Bounds::phaseShiftingText), Justification::centredLeft, 1);
}

void FeedbackModuleParameterSettingsComponent::setScale(float scaleFactor)
{
    Bounds::scale = scaleFactor;
    repaint();
    resized();
}

void FeedbackModuleParameterSettingsComponent::buttonClicked(Button *button)
{
    if (button == &closeButton) {
        setVisible(false);
    }
    if (button == &applyButton) {
        setVisible(false);
    }
}

Rectangle<int> FeedbackModuleParameterSettingsComponent::Bounds::get(Rectangle<int> rectangle)
{
    return rectangle.transformedBy(AffineTransform::scale(scale));
}