/*
  ==============================================================================

    LissajousCurveComponent.cpp
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "LissajousCurveComponent.h"
#include "ResponsiveUIHelper.h"
#include "UI/DesignSystem.h"

//==============================================================================
LissajousCurveComponent::LissajousCurveComponent(ProjectManager * pm)
{
    projectManager = pm;
    projectManager->addUIListener(this);
    
    
    
    // Font
    Typeface::Ptr AssistantLight        = Typeface::createSystemTypefaceFor(BinaryData::AssistantLight_ttf, BinaryData::AssistantLight_ttfSize);
    Typeface::Ptr AssistantSemiBold     = Typeface::createSystemTypefaceFor(BinaryData::AssistantSemiBold_ttf, BinaryData::AssistantSemiBold_ttfSize);
    Font fontSemiBold(AssistantSemiBold);
    Font fontLight(AssistantLight);
    
    fontSemiBold.setHeight(33);
    fontLight.setHeight(33);
    
    
    background                          = ImageCache::getFromMemory(BinaryData::LissajousCurveBackground2_png, BinaryData::LissajousCurveBackground2_pngSize);
    imagePanicButton                    = ImageCache::getFromMemory(BinaryData::ButtonNoisePanic, BinaryData::ButtonNoisePanicSize);
    imagePlayButton                     = ImageCache::getFromMemory(BinaryData::playPause2x_png, BinaryData::playPause2x_pngSize);
    imageStopButton                     = ImageCache::getFromMemory(BinaryData::stop2x_png, BinaryData::stop2x_pngSize);
    imageAddChordButton                 = ImageCache::getFromMemory(BinaryData::LissajousAddChordButtonImage_png, BinaryData::LissajousAddChordButtonImage_pngSize);
    imageAddWavefileButton              = ImageCache::getFromMemory(BinaryData::LissajousAddFileButtonImage_png, BinaryData::LissajousAddFileButtonImage_pngSize);
    imageSettingsIcon                   = ImageCache::getFromMemory(BinaryData::settings2x_png, BinaryData::settings2x_pngSize);
    imageMuteIcon                       = ImageCache::getFromMemory(BinaryData::ShortcutMute2x_png, BinaryData::ShortcutMute2x_pngSize);
    imageDelete                         = ImageCache::getFromMemory(BinaryData::ShortcutClose2x_png, BinaryData::ShortcutClose2x_pngSize);
    imageBlueButtonNormal               = ImageCache::getFromMemory(BinaryData::BlueButton_Normal_png, BinaryData::BlueButton_Normal_pngSize);
    imageBlueButtonSelected             = ImageCache::getFromMemory(BinaryData::BlueButton_Selected_png, BinaryData::BlueButton_Selected_pngSize);
    
    imageChordFreqButtonNormal              = ImageCache::getFromMemory(BinaryData::ButtonChordFrequencyNormal, BinaryData::ButtonChordFrequencyNormalSize);
    imageChordFreqButtonSelected             = ImageCache::getFromMemory(BinaryData::ButtonChordFrequencySelected, BinaryData::ButtonChordFrequencySelectedSize);
    
    imageSourceActive = ImageCache::getFromMemory(BinaryData::LissajousAddButtonActive_png, BinaryData::LissajousAddButtonActive_pngSize);
    imageSourceInActive = ImageCache::getFromMemory(BinaryData::LissajousAddButtonInactive_png, BinaryData::LissajousAddButtonInactive_pngSize);
    
    
    int stopLeftMargin      = 875 + 249 + 24;
    int playLeftMargin      = 875;
    int playTopMargin       = 1330;
    int playWidth           = 249;
    int playHeight          = 61;
    int panicLeftMargin     = 200;
    int panicTopMargin      = 1264;
    int panicWidth          = 180;
    int panicHeight         = panicWidth;
    
    
    button_Play = std::make_unique<ImageButton>();
    button_Play->setTriggeredOnMouseDown(true);
    button_Play->setImages (false, true, true,
                            imagePlayButton, 0.999f, Colour (0x00000000),
                            Image(), 1.000f, Colour (0x00000000),
                            imagePlayButton, 0.6, Colour (0x00000000));
    button_Play->addListener(this);
    button_Play->setBounds(playLeftMargin, playTopMargin, playWidth, playHeight);
    addAndMakeVisible(button_Play.get());
    
    button_Stop = std::make_unique<ImageButton>();
    button_Stop->setTriggeredOnMouseDown(true);
    button_Stop->setImages (false, true, true,
                            imageStopButton, 0.999f, Colour (0x00000000),
                            Image(), 1.000f, Colour (0x00000000),
                            imageStopButton, 0.6, Colour (0x00000000));
    button_Stop->addListener(this);
    button_Stop->setBounds(stopLeftMargin, playTopMargin, playWidth, playHeight);
    addAndMakeVisible(button_Stop.get());
    
    button_Panic = std::make_unique<ImageButton>();
    button_Panic->setTriggeredOnMouseDown(true);
    button_Panic->setImages (false, true, true,
                             imagePanicButton, 0.999f, Colour (0x00000000),
                             Image(), 1.000f, Colour (0x00000000),
                             imagePanicButton, 0.6, Colour (0x00000000));
    button_Panic->addListener(this);
    addAndMakeVisible(button_Panic.get());
    
    // Play In Loop Button
    button_PlayInLoop = std::make_unique<ToggleButton>("Play in Loop");
    button_PlayInLoop->setBounds(1173, 1137, 300, 80);
    button_PlayInLoop->setLookAndFeel(&lookAndFeel);
    button_PlayInLoop->addListener(this);
    addAndMakeVisible(button_PlayInLoop.get());
    
    
    
    int knobL = 136;
    int knobY = 1054;
    
    slider_X    = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_AMPLITUDE);
    slider_X    ->setRange (0, 100.0, 0);
    slider_X    ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_X    ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_X    ->addListener (this);
    slider_X    ->setTextValueSuffix("%");
    slider_X    ->setBounds(knobL, knobY, 130, 158);
    slider_X    ->setNumDecimalPlacesToDisplay(1);
    slider_X    ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_X.get());
    
    
    slider_Y    = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_AMPLITUDE);
    slider_Y    ->setRange (0, 100.0, 0);
    slider_Y    ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_Y    ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_Y    ->addListener (this);
    slider_Y    ->setTextValueSuffix("%");
    slider_Y    ->setBounds(knobL + 260, knobY, 130, 158);
    slider_Y    ->setNumDecimalPlacesToDisplay(1);
    slider_Y    ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_Y.get());
    
    
    slider_Z    = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_AMPLITUDE);
    slider_Z    ->setRange (0, 100.0, 0);
    slider_Z    ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_Z    ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_Z    ->addListener (this);
    slider_Z    ->setTextValueSuffix("%");
    slider_Z    ->setBounds(knobL + 524, knobY, 130, 158);
    slider_Z    ->setNumDecimalPlacesToDisplay(1);
    slider_Z    ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_Z.get());
    
    slider_PhaseX       = std::make_unique<Slider>("");
    slider_PhaseX    ->setRange (-100, 100.0, 0);
    slider_PhaseX    ->setSliderStyle (Slider::LinearHorizontal);
    slider_PhaseX    ->setTextBoxStyle (Slider::NoTextBox, false, 78, 28);
    slider_PhaseX    ->addListener (this);
    slider_PhaseX    ->setTextValueSuffix("%");
    slider_PhaseX    ->setNumDecimalPlacesToDisplay(1);
    slider_PhaseX    ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_PhaseX.get());
    
    slider_PhaseY = std::make_unique<Slider>("");
    slider_PhaseY    ->setRange (-100, 100.0, 0);
    slider_PhaseY    ->setSliderStyle (Slider::LinearHorizontal);
    slider_PhaseY    ->setTextBoxStyle (Slider::NoTextBox, false, 78, 28);
    slider_PhaseY    ->addListener (this);
    slider_PhaseY    ->setTextValueSuffix("%");
    slider_PhaseY    ->setNumDecimalPlacesToDisplay(1);
    slider_PhaseY    ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_PhaseY.get());
    
    slider_PhaseZ = std::make_unique<Slider>("");
    slider_PhaseZ    ->setRange (-100, 100.0, 0);
    slider_PhaseZ    ->setSliderStyle (Slider::LinearHorizontal);
    slider_PhaseZ    ->setTextBoxStyle (Slider::NoTextBox, false, 78, 28);
    slider_PhaseZ    ->addListener (this);
    slider_PhaseZ    ->setTextValueSuffix("%");
    slider_PhaseZ    ->setNumDecimalPlacesToDisplay(1);
    slider_PhaseZ    ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_PhaseZ.get());
    
    for (int i = 0; i < 3; i++)
    {
        buttonFreeFlow[i] = std::make_unique<ImageButton>();
        buttonFreeFlow[i]->setTriggeredOnMouseDown(true);
        buttonFreeFlow[i]->setImages (false, true, true,
                                    imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                    Image(), 1.000f, Colour (0x00000000),
                                    imageBlueButtonSelected, 1.0, Colour (0x00000000));
        buttonFreeFlow[i]->addListener(this);
        addAndMakeVisible(buttonFreeFlow[i].get());

        
        buttonSequencer[i] = std::make_unique<ImageButton>();
        buttonSequencer[i]->setTriggeredOnMouseDown(true);
        buttonSequencer[i]->setImages (false, true, true,
                                    imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                    Image(), 1.000f, Colour (0x00000000),
                                    imageBlueButtonSelected, 1.0, Colour (0x00000000));
        buttonSequencer[i]->addListener(this);
        addAndMakeVisible(buttonSequencer[i].get());
        
        buttonChordOrFrequency[i] = std::make_unique<ImageButton>();
        buttonChordOrFrequency[i]->setTriggeredOnMouseDown(true);
        buttonChordOrFrequency[i]->setImages (false, true, true,
                                    imageChordFreqButtonNormal, 0.999f, Colour (0x00000000),
                                    Image(), 1.000f, Colour (0x00000000),
                                    imageChordFreqButtonSelected, 1.0, Colour (0x00000000));
        buttonChordOrFrequency[i]->addListener(this);
        addAndMakeVisible(buttonChordOrFrequency[i].get());
        
        
        
        
        buttonOpenSettings[i] = std::make_unique<ImageButton>();
        buttonOpenSettings[i]->setTriggeredOnMouseDown(true);
        buttonOpenSettings[i]->setImages (false, true, true,
                                    imageSettingsIcon, 0.999f, Colour (0x00000000),
                                    Image(), 1.000f, Colour (0x00000000),
                                    imageSettingsIcon, 0.75, Colour (0x00000000));
        buttonOpenSettings[i]->addListener(this);
        addAndMakeVisible(buttonOpenSettings[i].get());
        
        buttonMute[i] = std::make_unique<ImageButton>();
        buttonMute[i]->setTriggeredOnMouseDown(true);
        buttonMute[i]->setImages (false, true, true,
                                    imageMuteIcon, 0.999f, Colour (0x00000000),
                                    Image(), 1.000f, Colour (0x00000000),
                                    imageMuteIcon, 1.0, Colour (0x00000000));
        buttonMute[i]->addListener(this);
        addAndMakeVisible(buttonMute[i].get());
        
        labelFrequency[i] = std::make_unique<Label>("");
        labelFrequency[i]   ->setText("432hz", dontSendNotification);
        labelFrequency[i]   ->setJustificationType(Justification::left);
        labelFrequency[i]   ->setColour(Label::textColourId, Colours::darkgrey);
        addAndMakeVisible(labelFrequency[i].get());
        
        
    }
    
    textEditor_SliderX = std::make_unique<TextEditor>();
    textEditor_SliderX->setInputRestrictions(3, "0123456789.");
    textEditor_SliderX->addListener(this);
    textEditor_SliderX->setLookAndFeel(&lookAndFeel);
    textEditor_SliderX->setJustification(Justification::centred);
    textEditor_SliderX->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditor_SliderX->applyColourToAllText(Colours::darkgrey);
    addAndMakeVisible(textEditor_SliderX.get());
    
    textEditor_SliderY = std::make_unique<TextEditor>();
    textEditor_SliderY->setInputRestrictions(3, "0123456789.");
    textEditor_SliderY->addListener(this);
    textEditor_SliderY->setLookAndFeel(&lookAndFeel);
    textEditor_SliderY->setJustification(Justification::centred);
    textEditor_SliderY->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditor_SliderY->applyColourToAllText(Colours::darkgrey);
    addAndMakeVisible(textEditor_SliderY.get());
    
    textEditor_SliderZ = std::make_unique<TextEditor>();
    textEditor_SliderZ->setInputRestrictions(3, "0123456789.");
    textEditor_SliderZ->addListener(this);
    textEditor_SliderZ->setLookAndFeel(&lookAndFeel);
    textEditor_SliderZ->setJustification(Justification::centred);
    textEditor_SliderZ->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditor_SliderZ->applyColourToAllText(Colours::darkgrey);
    addAndMakeVisible(textEditor_SliderZ.get());
    
    textEditor_SliderPhaseX = std::make_unique<TextEditor>();
    textEditor_SliderPhaseX->setInputRestrictions(3, "0123456789.-");
    textEditor_SliderPhaseX->addListener(this);
    textEditor_SliderPhaseX->setLookAndFeel(&lookAndFeel);
    textEditor_SliderPhaseX->setJustification(Justification::centred);
    textEditor_SliderPhaseX->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditor_SliderPhaseX->applyColourToAllText(Colours::darkgrey);
    addAndMakeVisible(textEditor_SliderPhaseX.get());
    
    textEditor_SliderPhaseY = std::make_unique<TextEditor>();
    textEditor_SliderPhaseY->setInputRestrictions(3, "0123456789.-");
    textEditor_SliderPhaseY->addListener(this);
    textEditor_SliderPhaseY->setLookAndFeel(&lookAndFeel);
    textEditor_SliderPhaseY->setJustification(Justification::centred);
    textEditor_SliderPhaseY->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditor_SliderPhaseY->applyColourToAllText(Colours::darkgrey);
    addAndMakeVisible(textEditor_SliderPhaseY.get());
    
    textEditor_SliderPhaseZ = std::make_unique<TextEditor>();
    textEditor_SliderPhaseZ->setInputRestrictions(3, "0123456789.-");
    textEditor_SliderPhaseZ->addListener(this);
    textEditor_SliderPhaseZ->setLookAndFeel(&lookAndFeel);
    textEditor_SliderPhaseZ->setJustification(Justification::centred);
    textEditor_SliderPhaseZ->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditor_SliderPhaseZ->applyColourToAllText(Colours::darkgrey);
    addAndMakeVisible(textEditor_SliderPhaseZ.get());

    
    
    viewerComponent = std::make_unique<LissajousCurveViewerComponent>(projectManager, true);
    viewerComponent->setBounds(38, 48, 1482, 884);
    addAndMakeVisible(viewerComponent.get());
    
    // popups
    
    for (int i = 0; i < 3; i++)
    {
        settingsChordComponent[i] = std::make_unique<LissajousChordPlayerSettingsComponent>(projectManager, i);
        addAndMakeVisible(settingsChordComponent[i].get());
        settingsChordComponent[i]->setBounds(0, 0, 1566, 1440);
        settingsChordComponent[i]->setVisible(false);
        
        settingsFrequencyComponent[i] = std::make_unique<LissajousFrequencyPlayerSettingsComponent>(projectManager, i);
        addAndMakeVisible(settingsFrequencyComponent[i].get());
        settingsFrequencyComponent[i]->setBounds(0, 0, 1566, 1440);
        settingsFrequencyComponent[i]->setVisible(false);
    }

    viewerComponent->setShouldUpdate(true);
}

LissajousCurveComponent::~LissajousCurveComponent()
{
    // Ensure children don't reference a destroyed LookAndFeel
    if (button_PlayInLoop) button_PlayInLoop->setLookAndFeel(nullptr);
    if (slider_X)          slider_X->setLookAndFeel(nullptr);
    if (slider_Y)          slider_Y->setLookAndFeel(nullptr);
    if (slider_Z)          slider_Z->setLookAndFeel(nullptr);
    if (slider_PhaseX)     slider_PhaseX->setLookAndFeel(nullptr);
    if (slider_PhaseY)     slider_PhaseY->setLookAndFeel(nullptr);
    if (slider_PhaseZ)     slider_PhaseZ->setLookAndFeel(nullptr);
    if (textEditor_SliderX)       textEditor_SliderX->setLookAndFeel(nullptr);
    if (textEditor_SliderY)       textEditor_SliderY->setLookAndFeel(nullptr);
    if (textEditor_SliderZ)       textEditor_SliderZ->setLookAndFeel(nullptr);
    if (textEditor_SliderPhaseX)  textEditor_SliderPhaseX->setLookAndFeel(nullptr);
    if (textEditor_SliderPhaseY)  textEditor_SliderPhaseY->setLookAndFeel(nullptr);
    if (textEditor_SliderPhaseZ)  textEditor_SliderPhaseZ->setLookAndFeel(nullptr);

    if (projectManager != nullptr)
        projectManager->removeUIListener(this);
}

void LissajousCurveComponent::paint (Graphics& g)
{
    g.drawImage(background, 0, 0, 1562*scaleFactor, 1440*scaleFactor, 0, 0, 1562, 1440);
}

void LissajousCurveComponent::resized()
{
    auto bounds = getLocalBounds();
    auto layout = ResponsiveUIHelper::getLetterboxLayout(
        bounds, TSS::Design::Layout::kRefContentWidth, TSS::Design::Layout::kRefContentHeight);

    this->layoutScale = layout.scale;
    this->layoutBounds = layout.bounds;

    lookAndFeel.setScale(this->layoutScale);
    if (viewerComponent)
        viewerComponent->setScale(this->layoutScale);

    auto scaleRect = [&layout](float x, float y, float w, float h) -> juce::Rectangle<int>
    {
        return ResponsiveUIHelper::scaleRect(layout, x, y, w, h);
    };

    // Main Viewer
    viewerComponent->setBounds(scaleRect(38, 20, 1482, 884));

    // Knobs section (Amplitude X, Y, Z)
    float knobL = 300;
    float knobY_xaxis = 956;
    float knobY_yaxis = 1076;
    float knobY_zaxis = 1200;
    float knobW = 126;
    float knobH = 126;
    
    slider_X->setBounds(scaleRect(knobL, knobY_xaxis, knobW, knobH));
    slider_Y->setBounds(scaleRect(knobL, knobY_yaxis, knobW, knobH));
    slider_Z->setBounds(scaleRect(knobL, knobY_zaxis, knobW, knobH));

    // Phase Sliders
    float phaseL = 1117;
    float phaseSliderW = 333;
    float phaseSliderH = 50;
    float phaseY_xaxis = 983;
    float phaseY_yaxis = 1108;
    float phaseY_zaxis = 1230;

    slider_PhaseX->setBounds(scaleRect(phaseL, phaseY_xaxis, phaseSliderW, phaseSliderH));
    slider_PhaseY->setBounds(scaleRect(phaseL, phaseY_yaxis, phaseSliderW, phaseSliderH));
    slider_PhaseZ->setBounds(scaleRect(phaseL, phaseY_zaxis, phaseSliderW, phaseSliderH));

    // Value Text Editors
    float textEdL = 210;
    float textEditorW = 70;
    float textEditorH = 58;
    float yDif = 24;
    textEditor_SliderX->setBounds(scaleRect(textEdL, phaseY_xaxis + yDif + 3, textEditorW, textEditorH));
    textEditor_SliderY->setBounds(scaleRect(textEdL, phaseY_yaxis + yDif, textEditorW, textEditorH));
    textEditor_SliderZ->setBounds(scaleRect(textEdL, phaseY_zaxis + yDif, textEditorW, textEditorH));

    // Phase Text Editors
    float textEditorPhaseL = 1440;
    yDif = 9;
    textEditor_SliderPhaseX->setBounds(scaleRect(textEditorPhaseL, phaseY_xaxis + yDif, textEditorW, textEditorH));
    textEditor_SliderPhaseY->setBounds(scaleRect(textEditorPhaseL, phaseY_yaxis + yDif, textEditorW, textEditorH));
    textEditor_SliderPhaseZ->setBounds(scaleRect(textEditorPhaseL, phaseY_zaxis + yDif, textEditorW, textEditorH));

    // Unit Controls
    float buttonW = 38;
    float buttonL = 930;
    float buttonYArr[3] = { 978, 1100, 1224 };
    float iconW = 26;
    float settingsL = 600;
    float labelL = 460;
    float labelW = 100;

    for (int i = 0; i < 3; i++)
    {
        buttonChordOrFrequency[i]->setBounds(scaleRect(586, buttonYArr[i], 224, 32));
        buttonFreeFlow[i]->setBounds(scaleRect(buttonL, buttonYArr[i], buttonW, buttonW));
        buttonSequencer[i]->setBounds(scaleRect(buttonL, buttonYArr[i] + 40, buttonW, buttonW));
        
        buttonOpenSettings[i]->setBounds(scaleRect(settingsL, buttonYArr[i] + 44, iconW, iconW));
        labelFrequency[i]->setBounds(scaleRect(labelL, buttonYArr[i] + 44, labelW, iconW));
        
        const float freqFontSize = ResponsiveUIHelper::getReadableFontSize(24.0f, this->layoutScale, TSS::Design::Usability::toolbarLabelMinFont);
        labelFrequency[i]->setFont(ProjectManager::getAssistantFont(ProjectManager::FontType::SemiBold).withHeight(freqFontSize));
    }

    // Loop Toggle
    button_PlayInLoop->setBounds(scaleRect(1220, 884, 300, 80));

    // Transport buttons - bottom row
    int stopLeftMargin = 875 + 249 + 24;
    int playLeftMargin = 875;
    int playTopMargin = 1346;
    int playWidth = 249;
    int playHeight = 61;
    int panicLeftMargin = 20;
    int panicTopMargin = 1320;
    int panicWidth = 110;

    button_Play->setBounds(scaleRect(playLeftMargin, playTopMargin, playWidth, playHeight));
    button_Stop->setBounds(scaleRect(stopLeftMargin, playTopMargin, playWidth, playHeight));
    button_Panic->setBounds(scaleRect(panicLeftMargin, panicTopMargin, panicWidth, panicWidth));
}


void LissajousCurveComponent::buttonClicked (Button*button)
{

    if (button == button_Play.get())
    {
        projectManager->setPlayerCommand(PLAYER_COMMANDS::COMMAND_PLAYER_PLAYPAUSE);
    }
    else if (button == button_Stop.get())
    {
        projectManager->setPlayerCommand(PLAYER_COMMANDS::COMMAND_PLAYER_STOP);
    }
    else if (button == button_Panic.get())
    {
        projectManager->setPanicButton();
    }
    else if (button == button_PlayInLoop.get())
    {
        if (button_PlayInLoop->getToggleState())
        {
            projectManager->setPlayerPlayMode(PLAY_MODE::LOOP);
        }
        else
        {
            projectManager->setPlayerPlayMode(PLAY_MODE::NORMAL); // might be trigger for shortcuts..
        }
    }
    
    else if (button == buttonFreeFlow[0].get())
    {
        projectManager->setLissajousParameter(UNIT_1_FREE_FLOW, 0);
    }
    else if (button == buttonFreeFlow[1].get())
    {
        projectManager->setLissajousParameter(UNIT_2_FREE_FLOW, 0);
    }
    else if (button == buttonFreeFlow[2].get())
    {
        projectManager->setLissajousParameter(UNIT_3_FREE_FLOW, 0);
    }
    else if (button == buttonSequencer[0].get())
    {
        projectManager->setLissajousParameter(UNIT_1_FREE_FLOW, 1);
    }
    else if (button == buttonSequencer[1].get())
    {
        projectManager->setLissajousParameter(UNIT_2_FREE_FLOW, 1);
    }
    else if (button == buttonSequencer[2].get())
    {
        projectManager->setLissajousParameter(UNIT_3_FREE_FLOW, 1);
    }
    
    else if (button == buttonChordOrFrequency[0].get())
    {
        projectManager->setLissajousParameter(UNIT_1_PROCESSOR_TYPE, !buttonChordOrFrequency[0]->getToggleState());
    }
    else if (button == buttonChordOrFrequency[1].get())
    {
        projectManager->setLissajousParameter(UNIT_2_PROCESSOR_TYPE, !buttonChordOrFrequency[1]->getToggleState());
    }
    else if (button == buttonChordOrFrequency[2].get())
    {
        projectManager->setLissajousParameter(UNIT_3_PROCESSOR_TYPE, !buttonChordOrFrequency[2]->getToggleState());
    }
    
    else if (button == buttonOpenSettings[0].get())
    {
        openSettingsView(0);
    }
    else if (button == buttonOpenSettings[1].get())
    {
        openSettingsView(1);
    }
    else if (button == buttonOpenSettings[2].get())
    {
        openSettingsView(2);
    }
    
    
    
}


void LissajousCurveComponent::sliderValueChanged (Slider* slider)
{
    if (slider == slider_X.get())
    {
        projectManager->setLissajousParameter(AMPLITUDE_X, slider_X->getValue());
    }
    else if (slider == slider_Y.get())
    {
        projectManager->setLissajousParameter(AMPLITUDE_Y, slider_Y->getValue());
    }
    else if (slider == slider_Z.get())
    {
        projectManager->setLissajousParameter(AMPLITUDE_Z, slider_Z->getValue());
    }
    
    else if (slider == slider_PhaseX.get())
    {
        projectManager->setLissajousParameter(UNIT_1_PHASE, slider_PhaseX->getValue());
    }
    else if (slider == slider_PhaseY.get())
    {
        projectManager->setLissajousParameter(UNIT_2_PHASE, slider_PhaseY->getValue());
    }
    else if (slider == slider_PhaseZ.get())
    {
        projectManager->setLissajousParameter(UNIT_3_PHASE, slider_PhaseZ->getValue());
    }
    
}

void LissajousCurveComponent::textEditorReturnKeyPressed (TextEditor&editor)
{
    if (&editor == textEditor_SliderX.get())
    {
        double value = editor.getText().getDoubleValue();

        // bounds check
        if (value >= 100) {  value = 100; } if (value <= 0) { value = 0; }
        
        projectManager->setLissajousParameter(AMPLITUDE_X, value);
    }
    else if (&editor == textEditor_SliderY.get())
    {
        double value = editor.getText().getDoubleValue();

        // bounds check
        if (value >= 100) {  value = 100; } if (value <= 0) { value = 0; }
        
        projectManager->setLissajousParameter(AMPLITUDE_Y, value);
    }
    else if (&editor == textEditor_SliderZ.get())
    {
        double value = editor.getText().getDoubleValue();

        // bounds check
        if (value >= 100) {  value = 100; } if (value <= 0) { value = 0; }
        
        projectManager->setLissajousParameter(AMPLITUDE_Z, value);
    }
    
    else if (&editor == textEditor_SliderPhaseX.get())
    {
        double value = editor.getText().getDoubleValue();

        // bounds check
        if (value >= 100) {  value = 100; } if (value <= -100) { value = -100; }
        
        projectManager->setLissajousParameter(UNIT_1_PHASE, value);
    }
    else if (&editor == textEditor_SliderPhaseY.get())
    {
        double value = editor.getText().getDoubleValue();

        // bounds check
        if (value >= 100) {  value = 100; } if (value <= -100) { value = -100; }
        
        projectManager->setLissajousParameter(UNIT_2_PHASE, value);
    }
    else if (&editor == textEditor_SliderPhaseZ.get())
    {
        double value = editor.getText().getDoubleValue();

        // bounds check
        if (value >= 100) {  value = 100; } if (value <= -100) { value = -100; }
        
        projectManager->setLissajousParameter(UNIT_3_PHASE, value);
    }
    
    
}

void LissajousCurveComponent::updateLissajousCurveUIParameter(int paramIndex)
{
    if (paramIndex == UNIT_1_PROCESSOR_TYPE)
    {
        bool val = projectManager->getLissajousParameter(paramIndex).operator bool();
        
        buttonChordOrFrequency[0]->setToggleState(val, dontSendNotification);
    }
    else if (paramIndex == UNIT_2_PROCESSOR_TYPE)
    {
        bool val = projectManager->getLissajousParameter(paramIndex).operator bool();
        
        buttonChordOrFrequency[1]->setToggleState(val, dontSendNotification);
    }
    else if (paramIndex == UNIT_3_PROCESSOR_TYPE)
    {
        bool val = projectManager->getLissajousParameter(paramIndex).operator bool();
        
        buttonChordOrFrequency[2]->setToggleState(val, dontSendNotification);
    }
    
    else if (paramIndex == UNIT_1_FREE_FLOW)
    {
        bool val = projectManager->getLissajousParameter(paramIndex).operator bool();
        
        buttonFreeFlow[0]->setToggleState(!val, dontSendNotification);
        buttonSequencer[0]->setToggleState(val, dontSendNotification);
    }
    else if (paramIndex == UNIT_2_FREE_FLOW)
    {
        bool val = projectManager->getLissajousParameter(paramIndex).operator bool();
        
        buttonFreeFlow[1]->setToggleState(!val, dontSendNotification);
        buttonSequencer[1]->setToggleState(val, dontSendNotification);
    }
    else if (paramIndex == UNIT_3_FREE_FLOW)
    {
        bool val = projectManager->getLissajousParameter(paramIndex).operator bool();
        
        buttonFreeFlow[2]->setToggleState(!val, dontSendNotification);
        buttonSequencer[2]->setToggleState(val, dontSendNotification);
    }
    
    else if (paramIndex == UNIT_1_PHASE)
    {
        double val = projectManager->getLissajousParameter(paramIndex).operator double();
        
        int intVal = (int)val;
        slider_PhaseX->setValue(val);
        String textVal(intVal); textVal.append("%", 1);
        textEditor_SliderPhaseX->setText(textVal);
    }
    else if (paramIndex == UNIT_2_PHASE)
    {
        double val = projectManager->getLissajousParameter(paramIndex).operator double();
        int intVal = (int)val;
        slider_PhaseY->setValue(val);
        String textVal(intVal); textVal.append("%", 1);
        textEditor_SliderPhaseY->setText(textVal);
    }
    else if (paramIndex == UNIT_3_PHASE)
    {
        double val = projectManager->getLissajousParameter(paramIndex).operator double();
        int intVal = (int)val;
        slider_PhaseZ->setValue(val);
        String textVal(intVal); textVal.append("%", 1);
        textEditor_SliderPhaseZ->setText(textVal);
    }
    
    else if (paramIndex == AMPLITUDE_X)
    {
        double val = projectManager->getLissajousParameter(paramIndex).operator double();
        int intVal = (int)val;
        slider_X->setValue(val);
        String textVal(intVal); textVal.append("%", 1);
        textEditor_SliderX->setText(textVal);
    }
    else if (paramIndex == AMPLITUDE_Y)
    {
        double val = projectManager->getLissajousParameter(paramIndex).operator double();
        int intVal = (int)val;
        slider_Y->setValue(val);
        String textVal(intVal); textVal.append("%", 1);
        textEditor_SliderY->setText(textVal);
    }
    else if (paramIndex == AMPLITUDE_Z)
    {
        double val = projectManager->getLissajousParameter(paramIndex).operator double();
        int intVal = (int)val;
        slider_Z->setValue(val);
        String textVal(intVal); textVal.append("%", 1);
        textEditor_SliderZ->setText(textVal);
    }
    
    // Freq Player Settings 1
    else if (paramIndex >= UNIT_1_FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE && paramIndex <= UNIT_1_FREQUENCY_PLAYER_NUM_DURATION)
    {
        settingsFrequencyComponent[0]->syncUI(paramIndex);
        
        updateLabelFrequency(0);
    }
    
    // Freq Player Settings 2
    else if (paramIndex >= UNIT_2_FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE && paramIndex <= UNIT_2_FREQUENCY_PLAYER_NUM_DURATION)
    {
        settingsFrequencyComponent[1]->syncUI(paramIndex);
        
        updateLabelFrequency(1);
    }
    
    // Freq Player Settings 3
    else if (paramIndex >= UNIT_3_FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE && paramIndex <= UNIT_3_FREQUENCY_PLAYER_NUM_DURATION)
    {
        settingsFrequencyComponent[2]->syncUI(paramIndex);
        
        updateLabelFrequency(2);
    }
    
    
    // chord Player Settings 1
    else if (paramIndex >= UNIT_1_CHORD_PLAYER_SHORTCUT_IS_ACTIVE && paramIndex <= UNIT_1_CHORD_PLAYER_CHORDPLAYER_SCALE)
    {
        settingsChordComponent[0]->syncUI(paramIndex);
        
        updateLabelFrequency(0);
    }
    
    // chord Player Settings 2
    else if (paramIndex >= UNIT_2_CHORD_PLAYER_SHORTCUT_IS_ACTIVE && paramIndex <= UNIT_2_CHORD_PLAYER_CHORDPLAYER_SCALE)
    {
        settingsChordComponent[1]->syncUI(paramIndex);
        
        updateLabelFrequency(1);
    }
    
    // chord Player Settings 3
    else if (paramIndex >= UNIT_3_CHORD_PLAYER_SHORTCUT_IS_ACTIVE && paramIndex <= UNIT_3_CHORD_PLAYER_CHORDPLAYER_SCALE)
    {
        settingsChordComponent[2]->syncUI(paramIndex);
        
        updateLabelFrequency(2);
    }
    
    
}

void LissajousCurveComponent::updateSettingsUIParameter(int settingIndex)
{
    if (settingIndex == PLAYER_PLAY_IN_LOOP)
    {
        button_PlayInLoop->setToggleState(projectManager->getProjectSettingsParameter(settingIndex), dontSendNotification);
    }
    else if (settingIndex == DEFAULT_SCALE)
    {
        settingsChordComponent[0]->updateScalesComponents();
        settingsChordComponent[1]->updateScalesComponents();
        settingsChordComponent[2]->updateScalesComponents();
    }
}
