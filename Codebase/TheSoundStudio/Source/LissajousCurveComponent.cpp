/*
  ==============================================================================

    LissajousCurveComponent.cpp

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "LissajousCurveComponent.h"

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
    
    
    imageSettingsIcon                   = ImageCache::getFromMemory(BinaryData::settings2x_png, BinaryData::settings2x_pngSize);
    imageMuteIcon                       = ImageCache::getFromMemory(BinaryData::ShortcutMute2x_png, BinaryData::ShortcutMute2x_pngSize);
    imageBlueButtonNormal               = ImageCache::getFromMemory(BinaryData::BlueButton_Normal_png, BinaryData::BlueButton_Normal_pngSize);
    imageBlueButtonSelected             = ImageCache::getFromMemory(BinaryData::BlueButton_Selected_png, BinaryData::BlueButton_Selected_pngSize);

    imageChordFreqButtonNormal              = ImageCache::getFromMemory(BinaryData::ButtonChordFrequencyNormal, BinaryData::ButtonChordFrequencyNormalSize);
    imageChordFreqButtonSelected             = ImageCache::getFromMemory(BinaryData::ButtonChordFrequencySelected, BinaryData::ButtonChordFrequencySelectedSize);

    // Transport toolbar with Play/Stop/Panic/Loop
    TransportToolbarComponent::ButtonVisibility toolbarVis;
    toolbarVis.play = true;
    toolbarVis.stop = true;
    toolbarVis.record = false;
    toolbarVis.panic = true;
    toolbarVis.progress = false;
    toolbarVis.loop = true;
    toolbarVis.simultaneous = false;
    toolbarVis.save = false;
    toolbarVis.load = false;
    toolbarVis.playingLabel = false;
    transportToolbar = std::make_unique<TransportToolbarComponent>(toolbarVis);
    transportToolbar->addTransportListener(this);
    addAndMakeVisible(transportToolbar.get());
    
    
    
    slider_X    = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_AMPLITUDE);
    slider_X    ->setRange (0, 100.0, 0);
    slider_X    ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_X    ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_X    ->addListener (this);
    slider_X    ->setTextValueSuffix("%");
    slider_X    ->setNumDecimalPlacesToDisplay(1);
    slider_X    ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_X.get());

    slider_Y    = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_AMPLITUDE);
    slider_Y    ->setRange (0, 100.0, 0);
    slider_Y    ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_Y    ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_Y    ->addListener (this);
    slider_Y    ->setTextValueSuffix("%");
    slider_Y    ->setNumDecimalPlacesToDisplay(1);
    slider_Y    ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_Y.get());

    slider_Z    = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_AMPLITUDE);
    slider_Z    ->setRange (0, 100.0, 0);
    slider_Z    ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_Z    ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_Z    ->addListener (this);
    slider_Z    ->setTextValueSuffix("%");
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
    addAndMakeVisible(viewerComponent.get());
    
    // popups
    
    for (int i = 0; i < 3; i++)
    {
        settingsChordComponent[i] = std::make_unique<LissajousChordPlayerSettingsComponent>(projectManager, i);
        addAndMakeVisible(settingsChordComponent[i].get());
        settingsChordComponent[i]->setVisible(false);

        settingsFrequencyComponent[i] = std::make_unique<LissajousFrequencyPlayerSettingsComponent>(projectManager, i);
        addAndMakeVisible(settingsFrequencyComponent[i].get());
        settingsFrequencyComponent[i]->setVisible(false);
    }

    viewerComponent->setShouldUpdate(true);
}

LissajousCurveComponent::~LissajousCurveComponent()
{
    // Ensure children don't reference a destroyed LookAndFeel
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
    g.fillAll(juce::Colour(45, 44, 44));
}

void LissajousCurveComponent::resized()
{
    using Layout = TSS::Design::Layout;
    auto bounds = getLocalBounds();
    const int w = bounds.getWidth();
    const int h = bounds.getHeight();
    const float s = w / Layout::kRefContentWidth;

    // Toolbar at bottom
    int toolbarH = (int)(h * Layout::kToolbarHeightRatio * 0.7f);
    transportToolbar->setBounds(bounds.removeFromBottom(toolbarH));

    // Lissajous viewer at top (~61% of remaining height)
    int viewerH = (int)(h * 0.614f);
    viewerComponent->setBounds((int)(38 * s), (int)(20 * s), (int)(1482 * s), viewerH);

    // Knobs area
    float knobL = 300 * s;
    float knobW = 126 * s;
    float knobH = 126 * s;
    float boxW = 60 * s;
    float boxH = 26 * s;

    float knobY_x = h * 0.664f;
    float knobY_y = h * 0.747f;
    float knobY_z = h * 0.833f;

    slider_X->setBounds((int)knobL, (int)knobY_x, (int)knobW, (int)knobH);
    slider_X->setTextBoxStyle(Slider::NoTextBox, false, (int)boxW, (int)boxH);
    slider_Y->setBounds((int)knobL, (int)knobY_y, (int)knobW, (int)knobH);
    slider_Y->setTextBoxStyle(Slider::NoTextBox, false, (int)boxW, (int)boxH);
    slider_Z->setBounds((int)knobL, (int)knobY_z, (int)knobW, (int)knobH);
    slider_Z->setTextBoxStyle(Slider::NoTextBox, false, (int)boxW, (int)boxH);

    // Phase sliders
    float phaseL = 1117 * s;
    float phaseW = 333 * s;
    float phaseH = 50 * s;
    float phaseY_x = h * 0.683f;
    float phaseY_y = h * 0.770f;
    float phaseY_z = h * 0.854f;

    slider_PhaseX->setBounds((int)phaseL, (int)phaseY_x, (int)phaseW, (int)phaseH);
    slider_PhaseY->setBounds((int)phaseL, (int)phaseY_y, (int)phaseW, (int)phaseH);
    slider_PhaseZ->setBounds((int)phaseL, (int)phaseY_z, (int)phaseW, (int)phaseH);

    // Text editors for sliders
    float textEdL = 210 * s;
    float textEdW = 70 * s;
    float textEdH = 58 * s;
    float yDif = 24 * s;
    textEditor_SliderX->setBounds((int)textEdL, (int)(phaseY_x + yDif + 3 * s), (int)textEdW, (int)textEdH);
    textEditor_SliderY->setBounds((int)textEdL, (int)(phaseY_y + yDif), (int)textEdW, (int)textEdH);
    textEditor_SliderZ->setBounds((int)textEdL, (int)(phaseY_z + yDif), (int)textEdW, (int)textEdH);

    float phaseTextL = 1440 * s;
    float phaseYDif = 9 * s;
    textEditor_SliderPhaseX->setBounds((int)phaseTextL, (int)(phaseY_x + phaseYDif), (int)textEdW, (int)textEdH);
    textEditor_SliderPhaseY->setBounds((int)phaseTextL, (int)(phaseY_y + phaseYDif), (int)textEdW, (int)textEdH);
    textEditor_SliderPhaseZ->setBounds((int)phaseTextL, (int)(phaseY_z + phaseYDif), (int)textEdW, (int)textEdH);

    // Per-axis buttons and labels
    float btnW = 38 * s;
    float iconW = 26 * s;
    float settingsL = 600 * s;
    float labelL = 460 * s;
    float labelW = 100 * s;
    float buttonL = 930 * s;
    float buttonY[3];
    buttonY[0] = h * 0.679f;
    buttonY[1] = h * 0.764f;
    buttonY[2] = h * 0.850f;

    for (int i = 0; i < 3; i++)
    {
        buttonChordOrFrequency[i]->setBounds((int)(586 * s), (int)buttonY[i], (int)(224 * s), (int)(32 * s));
        buttonFreeFlow[i]->setBounds((int)buttonL, (int)buttonY[i], (int)btnW, (int)btnW);
        buttonSequencer[i]->setBounds((int)buttonL, (int)(buttonY[i] + 40 * s), (int)btnW, (int)btnW);

        settingsChordComponent[i]->setBounds(0, 0, (int)(1566 * s), (int)(1440 * s));
        settingsFrequencyComponent[i]->setBounds(0, 0, (int)(1566 * s), (int)(1440 * s));

        buttonOpenSettings[i]->setBounds((int)settingsL, (int)(buttonY[i] + 44 * s), (int)iconW, (int)iconW);
        labelFrequency[i]->setBounds((int)labelL, (int)(buttonY[i] + 44 * s), (int)labelW, (int)iconW);
        labelFrequency[i]->setFont(24 * s);
    }
}


void LissajousCurveComponent::buttonClicked (Button*button)
{

    if (button == buttonFreeFlow[0].get())
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

void LissajousCurveComponent::transportPlayClicked()
{
    projectManager->setPlayerCommand(PLAYER_COMMANDS::COMMAND_PLAYER_PLAYPAUSE);
}

void LissajousCurveComponent::transportStopClicked()
{
    projectManager->setPlayerCommand(PLAYER_COMMANDS::COMMAND_PLAYER_STOP);
}

void LissajousCurveComponent::transportPanicClicked()
{
    projectManager->setPanicButton();
}

void LissajousCurveComponent::transportLoopToggled(bool isOn)
{
    if (isOn)
        projectManager->setPlayerPlayMode(PLAY_MODE::LOOP);
    else
        projectManager->setPlayerPlayMode(PLAY_MODE::NORMAL);
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
        transportToolbar->setLoopState(projectManager->getProjectSettingsParameter(settingIndex));
    }
    else if (settingIndex == DEFAULT_SCALE)
    {
        settingsChordComponent[0]->updateScalesComponents();
        settingsChordComponent[1]->updateScalesComponents();
        settingsChordComponent[2]->updateScalesComponents();
    }
}
