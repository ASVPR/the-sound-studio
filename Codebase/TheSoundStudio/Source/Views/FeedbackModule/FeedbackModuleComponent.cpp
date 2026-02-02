/*
  ==============================================================================

    FeedbackModuleComponent.cpp

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#include <utility_components/PlayerButton.h>
#include "FeedbackModuleComponent.h"

FeedbackModuleComponent::FeedbackModuleComponent(FeedbackModuleProcessor &feedbackModule) :
    processor(feedbackModule),
    projectManager(processor.getProjectManager()),
    frequencyPlayerSettingsComponent(&projectManager),
    chordPlayerSettingsComponent(&projectManager),
    parameterSettingsComponent(projectManager),
    inputOutputComponent(projectManager, feedbackModule, this)
{
    projectManager.addUIListener(this);

    setParametersButton.setButtonText("Set Parameters");
    setParametersButton.setMouseCursor(MouseCursor::PointingHandCursor);
    setParametersButton.addListener(this);
    addAndMakeVisible(setParametersButton);

    Image blueButtonNormal = ImageCache::getFromMemory(BinaryData::BlueButton_Normal_png, BinaryData::BlueButton_Normal_pngSize);
    Image blueButtonSelected = ImageCache::getFromMemory(BinaryData::BlueButton_Selected_png, BinaryData::BlueButton_Selected_pngSize);
    Image imageNoiseButton = ImageCache::getFromMemory(BinaryData::PanicButton2x_png, BinaryData::PanicButton2x_pngSize);

    semiAutomaticModeButton.setTriggeredOnMouseDown(true);
    semiAutomaticModeButton.setMouseCursor(MouseCursor::PointingHandCursor);
    semiAutomaticModeButton.setClickingTogglesState(true);
    semiAutomaticModeButton.setRadioGroupId(1, dontSendNotification);
    semiAutomaticModeButton.setToggleState(true, dontSendNotification);
    semiAutomaticModeButton.setImages (false, true, true,
        blueButtonNormal, 0.999f, Colour (0x00000000),
        Image(), 1.000f, Colour (0x00000000),
        blueButtonSelected, 1.0, Colour (0x00000000));
    semiAutomaticModeButton.addListener(this);
    addAndMakeVisible(semiAutomaticModeButton);

    automaticModeButton.setTriggeredOnMouseDown(true);
    automaticModeButton.setMouseCursor(MouseCursor::PointingHandCursor);
    automaticModeButton.setClickingTogglesState(true);
    automaticModeButton.setRadioGroupId(1, dontSendNotification);
    automaticModeButton.setImages (false, true, true,
        blueButtonNormal, 0.999f, Colour (0x00000000),
        Image(), 1.000f, Colour (0x00000000),
        blueButtonSelected, 1.0, Colour (0x00000000));
    automaticModeButton.addListener(this);
    addAndMakeVisible(automaticModeButton);

    chordButton.setButtonText("Chord");
    chordButton.setClickingTogglesState(true);
    chordButton.setRadioGroupId(2);
    chordButton.setMouseCursor(MouseCursor::PointingHandCursor);
    chordButton.addListener(this);
    addAndMakeVisible(chordButton);

    frequencyButton.setButtonText("Frequency");
    frequencyButton.setClickingTogglesState(true);
    frequencyButton.setRadioGroupId(2);
    frequencyButton.setMouseCursor(MouseCursor::PointingHandCursor);
    frequencyButton.addListener(this);
    addAndMakeVisible(frequencyButton);

    algorithmResultLabel.setColour(Label::ColourIds::textColourId, Colours::white);
    addAndMakeVisible(algorithmResultLabel);

    viewport.setScrollBarsShown(true, false);
    addAndMakeVisible(viewport);
    viewport.setViewedComponent(&inputOutputComponent, false);

    playPauseButton.setTriggeredOnMouseDown(true);
    playPauseButton.addListener(this);
    addAndMakeVisible(playPauseButton);

    stopButton.setTriggeredOnMouseDown(true);
    stopButton.addListener(this);
    addAndMakeVisible(stopButton);

    noiseButton.setTriggeredOnMouseDown(true);
    noiseButton.setImages (false, true, true,
        imageNoiseButton, 0.999f, Colour (0x00000000),
        Image(), 1.000f, Colour (0x00000000),
        imageNoiseButton, 0.6, Colour (0x00000000));
    noiseButton.addListener(this);
    addAndMakeVisible(noiseButton);

    addChildComponent(frequencyPlayerSettingsComponent);
    addChildComponent(chordPlayerSettingsComponent);
    addChildComponent(parameterSettingsComponent);

    chordPlayerSettingsComponent.updateScalesComponents();
}

FeedbackModuleComponent::~FeedbackModuleComponent()
{
    projectManager.removeUIListener(this);
    setParametersButton.removeListener(this);
    semiAutomaticModeButton.removeListener(this);
    automaticModeButton.removeListener(this);
    chordButton.removeListener(this);
    frequencyButton.removeListener(this);
    playPauseButton.removeListener(this);
    stopButton.removeListener(this);
    noiseButton.removeListener(this);
}

void FeedbackModuleComponent::paint(Graphics &g)
{
    g.fillAll({ 45, 44, 44 });

    g.setColour(Colour::fromString("ff464b55"));
    g.fillRoundedRectangle(Bounds::get(Bounds::setParameterContainer).toFloat(), 5);
    g.fillRoundedRectangle(Bounds::get(Bounds::algorithmContainer).toFloat(), 5);
    g.setColour(Colour::fromString("ff626773"));
    g.fillRoundedRectangle(Bounds::get(Bounds::algorithmResultLabel).toFloat(), 5);

    g.setColour(Colours::white);
    g.setFont(34 * Bounds::scale);
    g.drawFittedText("Results", Bounds::get(Bounds::resultsText), Justification::centred, 1);

    g.setFont(28 * Bounds::scale);
    g.drawFittedText("Semi Automatic", Bounds::get(Bounds::semiAutomaticText), Justification::centredLeft, 1);
    g.drawFittedText("Automatic", Bounds::get(Bounds::automaticText), Justification::centredLeft, 1);
}

void FeedbackModuleComponent::resized()
{
    setParametersButton.setBounds(Bounds::get(Bounds::setParametersButton));
    semiAutomaticModeButton.setBounds(Bounds::get(Bounds::semiAutomaticButton));
    automaticModeButton.setBounds(Bounds::get(Bounds::automaticButton));

    chordButton.setBounds(Bounds::get(Bounds::chordButton));
    frequencyButton.setBounds(Bounds::get(Bounds::frequencyButton));
    algorithmResultLabel.setFont(Font{28 * Bounds::scale});
    algorithmResultLabel.setBounds(Bounds::get(Bounds::algorithmResultLabel).translated(5, 0));

    frequencyPlayerSettingsComponent.setBounds(getLocalBounds());
    chordPlayerSettingsComponent.setBounds(getLocalBounds());
    parameterSettingsComponent.setBounds(getLocalBounds());

    playPauseButton.setBounds(Bounds::get(Bounds::playPauseButton));
    stopButton.setBounds(Bounds::get(Bounds::stopButton));
    noiseButton.setBounds(Bounds::get(Bounds::noiseButton));

    {
        auto bounds = getLocalBounds();
        bounds.removeFromTop(340 * Bounds::scale);
        bounds.removeFromBottom(143 * Bounds::scale);
        viewport.setBounds(bounds);
        if (auto* content = viewport.getViewedComponent())
            content->setSize(viewport.getMaximumVisibleWidth(), inputOutputComponent.getContainerHeight());
    }
}

void FeedbackModuleComponent::buttonClicked(Button *button)
{
    if (button == &frequencyButton && frequencyButton.getToggleState())
        frequencyPlayerSettingsComponent.openView(0);
    if (button == &chordButton && chordButton.getToggleState())
        chordPlayerSettingsComponent.openView(0);
    if (button == &setParametersButton)
        parameterSettingsComponent.setVisible(true);
    if (button == &noiseButton)
        projectManager.setPanicButton();
}

void FeedbackModuleComponent::comboBoxChanged(ComboBox *comboBoxThatHasChanged)
{
}

void FeedbackModuleComponent::createContainers()
{
    auto inputs = processor.getInputNames();
    auto outputs = processor.getOutputNames();


    /*for (auto name: inputs)
    {
        auto &container = inputBlocks.createContainer(name, true);
        inputContainer.addAndMakeVisible(container);
    }

    for (auto name: outputs)
    {
        auto &container = outputBlocks.createContainer(name, true);
        outputContainer.addAndMakeVisible(container);
    }*/
}

void FeedbackModuleComponent::updateFrequencyPlayerUIParameter(int shortcutRef, int paramIndex)
{
    if (shortcutRef == 0 && paramIndex == FREQUENCY_PLAYER_SHORTCUT_PARAMS::FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE) {
        if (projectManager.getFrequencyPlayerParameter(shortcutRef, FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE)) {
            String frequency = String {(float)projectManager.getFrequencyPlayerParameter(shortcutRef, FREQUENCY_PLAYER_CHOOSE_FREQ), 2};
            algorithmResultLabel.setText(frequency + " Hz", sendNotificationAsync);
        }
    }
    frequencyPlayerSettingsComponent.syncUI();
}

void FeedbackModuleComponent::updateChordPlayerUIParameter(int shortcutRef, int paramIndex)
{
    if (shortcutRef == 0 && paramIndex == CHORD_PLAYER_SHORTCUT_PARAMS::SHORTCUT_IS_ACTIVE) {
        if (projectManager.getChordPlayerParameter(shortcutRef, SHORTCUT_IS_ACTIVE)) {
            int oct                 = projectManager.getChordPlayerParameter(shortcutRef, OCTAVE).operator int() + 1;
            int chordRef            = projectManager.getChordPlayerParameter(shortcutRef, KEYNOTE).operator int();
            String chordString(ProjectStrings::getKeynoteArray().getReference(chordRef-1));
            chordString.append("-", 3);
            String octString(oct-1);
            chordString.append(octString, 3);
            algorithmResultLabel.setText(chordString, sendNotificationAsync);
        }
    }
    chordPlayerSettingsComponent.syncUI();
}


void FeedbackModuleComponent::setScale(float scaleFactor)
{
    Bounds::scale = scaleFactor;
    frequencyPlayerSettingsComponent.setScale(scaleFactor);
    chordPlayerSettingsComponent.setScale(scaleFactor);

    repaint();
    resized();
}

Rectangle<int> FeedbackModuleComponent::Bounds::get(Rectangle<int> rectangle)
{
    return rectangle.transformedBy(AffineTransform::scale(scale));
}
