/*
  ==============================================================================

    TransportToolbarComponent.cpp
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#include "TransportToolbarComponent.h"

using Layout = TSS::Design::Layout;

//==============================================================================
// Vector icon factories
//==============================================================================

DrawablePath TransportToolbarComponent::createPlayIcon(float size, juce::Colour colour)
{
    Path p;
    p.addTriangle(0.0f, 0.0f, size, size * 0.5f, 0.0f, size);
    DrawablePath dp;
    dp.setPath(p);
    dp.setFill(colour);
    return dp;
}

DrawablePath TransportToolbarComponent::createStopIcon(float size, juce::Colour colour)
{
    Path p;
    p.addRectangle(0.0f, 0.0f, size, size);
    DrawablePath dp;
    dp.setPath(p);
    dp.setFill(colour);
    return dp;
}

DrawablePath TransportToolbarComponent::createRecordIcon(float size, juce::Colour colour)
{
    Path p;
    p.addEllipse(0.0f, 0.0f, size, size);
    DrawablePath dp;
    dp.setPath(p);
    dp.setFill(colour);
    return dp;
}

DrawablePath TransportToolbarComponent::createPanicIcon(float size, juce::Colour colour)
{
    Path p;
    // Exclamation mark icon
    float cx = size * 0.5f;
    float barW = size * 0.15f;
    p.addRoundedRectangle(cx - barW, size * 0.1f, barW * 2.0f, size * 0.55f, barW);
    p.addEllipse(cx - barW, size * 0.75f, barW * 2.0f, barW * 2.0f);
    DrawablePath dp;
    dp.setPath(p);
    dp.setFill(colour);
    return dp;
}

//==============================================================================
TransportToolbarComponent::TransportToolbarComponent(ButtonVisibility visibility)
    : vis(visibility)
{
    const float iconSize = 24.0f;

    if (vis.play)
    {
        auto normalIcon = std::make_unique<DrawablePath>(createPlayIcon(iconSize, Colours::white));
        auto overIcon   = std::make_unique<DrawablePath>(createPlayIcon(iconSize, Colours::lightgrey));
        auto downIcon   = std::make_unique<DrawablePath>(createPlayIcon(iconSize, Colours::grey));
        button_Play = std::make_unique<DrawableButton>("Play", DrawableButton::ImageFitted);
        button_Play->setImages(normalIcon.get(), overIcon.get(), downIcon.get());
        button_Play->addListener(this);
        addAndMakeVisible(button_Play.get());
    }

    if (vis.stop)
    {
        auto normalIcon = std::make_unique<DrawablePath>(createStopIcon(iconSize, Colours::white));
        auto overIcon   = std::make_unique<DrawablePath>(createStopIcon(iconSize, Colours::lightgrey));
        auto downIcon   = std::make_unique<DrawablePath>(createStopIcon(iconSize, Colours::grey));
        button_Stop = std::make_unique<DrawableButton>("Stop", DrawableButton::ImageFitted);
        button_Stop->setImages(normalIcon.get(), overIcon.get(), downIcon.get());
        button_Stop->addListener(this);
        addAndMakeVisible(button_Stop.get());
    }

    if (vis.record)
    {
        auto normalIcon = std::make_unique<DrawablePath>(createRecordIcon(iconSize, Colour(0xFFCC3333)));
        auto overIcon   = std::make_unique<DrawablePath>(createRecordIcon(iconSize, Colour(0xFFFF4444)));
        auto downIcon   = std::make_unique<DrawablePath>(createRecordIcon(iconSize, Colour(0xFF991111)));
        button_Record = std::make_unique<DrawableButton>("Record", DrawableButton::ImageFitted);
        button_Record->setImages(normalIcon.get(), overIcon.get(), downIcon.get());
        button_Record->addListener(this);
        addAndMakeVisible(button_Record.get());
    }

    if (vis.panic)
    {
        auto normalIcon = std::make_unique<DrawablePath>(createPanicIcon(iconSize, Colour(0xFFFF8800)));
        auto overIcon   = std::make_unique<DrawablePath>(createPanicIcon(iconSize, Colour(0xFFFFAA33)));
        auto downIcon   = std::make_unique<DrawablePath>(createPanicIcon(iconSize, Colour(0xFFCC6600)));
        button_Panic = std::make_unique<DrawableButton>("Panic", DrawableButton::ImageFitted);
        button_Panic->setImages(normalIcon.get(), overIcon.get(), downIcon.get());
        button_Panic->addListener(this);
        addAndMakeVisible(button_Panic.get());
    }

    if (vis.progress)
    {
        progressBar = std::make_unique<CustomProgressBar>();
        addAndMakeVisible(progressBar.get());
    }

    if (vis.playingLabel)
    {
        label_Playing = std::make_unique<Label>();
        label_Playing->setText("Playing (00:00)", dontSendNotification);
        label_Playing->setJustificationType(Justification::left);
        label_Playing->setColour(Label::textColourId, Colours::white);
        addAndMakeVisible(label_Playing.get());
    }

    if (vis.loop)
    {
        button_Loop = std::make_unique<ToggleButton>("Loop");
        button_Loop->setLookAndFeel(&lookAndFeel);
        button_Loop->addListener(this);
        addAndMakeVisible(button_Loop.get());
    }

    if (vis.simultaneous)
    {
        button_Simultaneous = std::make_unique<ToggleButton>("Simultaneous");
        button_Simultaneous->setLookAndFeel(&lookAndFeel);
        button_Simultaneous->addListener(this);
        addAndMakeVisible(button_Simultaneous.get());
    }

    if (vis.save)
    {
        button_Save = std::make_unique<TextButton>("Save");
        button_Save->addListener(this);
        button_Save->setLookAndFeel(&lookAndFeel);
        addAndMakeVisible(button_Save.get());
    }

    if (vis.load)
    {
        button_Load = std::make_unique<TextButton>("Load");
        button_Load->addListener(this);
        button_Load->setLookAndFeel(&lookAndFeel);
        addAndMakeVisible(button_Load.get());
    }
}

TransportToolbarComponent::~TransportToolbarComponent()
{
    if (button_Loop)         button_Loop->setLookAndFeel(nullptr);
    if (button_Simultaneous) button_Simultaneous->setLookAndFeel(nullptr);
    if (button_Save)         button_Save->setLookAndFeel(nullptr);
    if (button_Load)         button_Load->setLookAndFeel(nullptr);
}

void TransportToolbarComponent::paint(Graphics& g)
{
    // Transparent — parent draws background
}

void TransportToolbarComponent::resized()
{
    auto bounds = getLocalBounds();
    const int h = bounds.getHeight();
    const int w = bounds.getWidth();

    // Transport button size proportional to toolbar height
    int btnSize = (int)(h * Layout::kTransportButtonSizeRatio);
    int recordSize = (int)(h * Layout::kTransportRecordSizeRatio);
    int progressH = (int)(h * Layout::kTransportProgressHeight);
    int padding = jmax(4, h / 20);

    // Row 1: Playing label + progress bar (top portion)
    auto topRow = bounds.removeFromTop(h / 3);
    if (label_Playing)
    {
        label_Playing->setBounds(topRow.removeFromLeft(w / 4));
        float localScale = w / Layout::kRefContentWidth;
        label_Playing->setFont(jmax(10.0f, 33.0f * localScale));
    }
    if (progressBar)
        progressBar->setBounds(topRow.reduced(padding, padding / 2));

    // Row 2: Toggles (middle portion)
    auto midRow = bounds.removeFromTop(h / 3);
    int toggleW = jmax(60, w / 8);
    if (button_Simultaneous)
        button_Simultaneous->setBounds(midRow.removeFromLeft(toggleW + 40));
    if (button_Loop)
        button_Loop->setBounds(midRow.removeFromLeft(toggleW));

    // Save/Load on the right of middle row
    int saveBtnW = jmax(50, w / 16);
    if (button_Load)
        button_Load->setBounds(midRow.removeFromRight(saveBtnW).reduced(2));
    if (button_Save)
        button_Save->setBounds(midRow.removeFromRight(saveBtnW).reduced(2));

    // Row 3: Transport buttons (bottom portion)
    auto botRow = bounds;
    int centerX = w / 2;

    if (button_Record)
    {
        button_Record->setBounds(botRow.getX() + padding,
                                 botRow.getCentreY() - recordSize / 2,
                                 recordSize, recordSize);
    }
    if (button_Play)
    {
        button_Play->setBounds(centerX - btnSize - padding / 2,
                               botRow.getCentreY() - btnSize / 2,
                               btnSize, btnSize);
    }
    if (button_Stop)
    {
        button_Stop->setBounds(centerX + padding / 2,
                               botRow.getCentreY() - btnSize / 2,
                               btnSize, btnSize);
    }
    if (button_Panic)
    {
        button_Panic->setBounds(botRow.getRight() - recordSize - padding,
                                botRow.getCentreY() - recordSize / 2,
                                recordSize, recordSize);
    }
}

void TransportToolbarComponent::buttonClicked(Button* button)
{
    if (button == button_Play.get())
        listeners.call(&Listener::transportPlayClicked);
    else if (button == button_Stop.get())
        listeners.call(&Listener::transportStopClicked);
    else if (button == button_Record.get())
        listeners.call(&Listener::transportRecordClicked);
    else if (button == button_Panic.get())
        listeners.call(&Listener::transportPanicClicked);
    else if (button == button_Loop.get())
        listeners.call(&Listener::transportLoopToggled, button_Loop->getToggleState());
    else if (button == button_Simultaneous.get())
        listeners.call(&Listener::transportSimultaneousToggled, button_Simultaneous->getToggleState());
    else if (button == button_Save.get())
        listeners.call(&Listener::transportSaveClicked);
    else if (button == button_Load.get())
        listeners.call(&Listener::transportLoadClicked);
}

void TransportToolbarComponent::setProgressValue(float value)
{
    if (progressBar)
        progressBar->setValue(value);
}

void TransportToolbarComponent::setPlayingText(const juce::String& text)
{
    if (label_Playing)
        label_Playing->setText(text, dontSendNotification);
}

void TransportToolbarComponent::setLoopState(bool isOn)
{
    if (button_Loop)
        button_Loop->setToggleState(isOn, dontSendNotification);
}

void TransportToolbarComponent::setSimultaneousState(bool isOn)
{
    if (button_Simultaneous)
        button_Simultaneous->setToggleState(isOn, dontSendNotification);
}
