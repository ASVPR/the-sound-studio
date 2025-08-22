/*
  ==============================================================================

    LissajousCurveComponent.cpp
    Created: 29 Sep 2019 10:38:19am
    Author:  Gary Jones

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
    
    
    button_Play = new ImageButton();
    button_Play->setTriggeredOnMouseDown(true);
    button_Play->setImages (false, true, true,
                            imagePlayButton, 0.999f, Colour (0x00000000),
                            Image(), 1.000f, Colour (0x00000000),
                            imagePlayButton, 0.6, Colour (0x00000000));
    button_Play->addListener(this);
    button_Play->setBounds(playLeftMargin, playTopMargin, playWidth, playHeight);
    addAndMakeVisible(button_Play);
    
    button_Stop = new ImageButton();
    button_Stop->setTriggeredOnMouseDown(true);
    button_Stop->setImages (false, true, true,
                            imageStopButton, 0.999f, Colour (0x00000000),
                            Image(), 1.000f, Colour (0x00000000),
                            imageStopButton, 0.6, Colour (0x00000000));
    button_Stop->addListener(this);
    button_Stop->setBounds(stopLeftMargin, playTopMargin, playWidth, playHeight);
    addAndMakeVisible(button_Stop);
    
    button_Panic = new ImageButton();
    button_Panic->setTriggeredOnMouseDown(true);
    button_Panic->setImages (false, true, true,
                             imagePanicButton, 0.999f, Colour (0x00000000),
                             Image(), 1.000f, Colour (0x00000000),
                             imagePanicButton, 0.6, Colour (0x00000000));
    button_Panic->addListener(this);
    addAndMakeVisible(button_Panic);
    
    // Play In Loop Button
    button_PlayInLoop = new ToggleButton("Play in Loop");
    button_PlayInLoop->setBounds(1173, 1137, 300, 80);
    button_PlayInLoop->setLookAndFeel(&lookAndFeel);
    button_PlayInLoop->addListener(this);
    addAndMakeVisible(button_PlayInLoop);
    
    
    
    int knobL = 136;
    int knobY = 1054;
    
    slider_X    = new CustomRotarySlider(CustomRotarySlider::ROTARY_AMPLITUDE);
    slider_X    ->setRange (0, 100.0, 0);
    slider_X    ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_X    ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_X    ->addListener (this);
    slider_X    ->setTextValueSuffix("%");
    slider_X    ->setBounds(knobL, knobY, 130, 158);
    slider_X    ->setNumDecimalPlacesToDisplay(1);
    slider_X    ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_X);
    
    
    slider_Y    = new CustomRotarySlider(CustomRotarySlider::ROTARY_AMPLITUDE);
    slider_Y    ->setRange (0, 100.0, 0);
    slider_Y    ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_Y    ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_Y    ->addListener (this);
    slider_Y    ->setTextValueSuffix("%");
    slider_Y    ->setBounds(knobL + 260, knobY, 130, 158);
    slider_Y    ->setNumDecimalPlacesToDisplay(1);
    slider_Y    ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_Y);
    
    
    slider_Z    = new CustomRotarySlider(CustomRotarySlider::ROTARY_AMPLITUDE);
    slider_Z    ->setRange (0, 100.0, 0);
    slider_Z    ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_Z    ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_Z    ->addListener (this);
    slider_Z    ->setTextValueSuffix("%");
    slider_Z    ->setBounds(knobL + 524, knobY, 130, 158);
    slider_Z    ->setNumDecimalPlacesToDisplay(1);
    slider_Z    ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_Z);
    
    slider_PhaseX       = new Slider("");
    slider_PhaseX    ->setRange (-100, 100.0, 0);
    slider_PhaseX    ->setSliderStyle (Slider::LinearHorizontal);
    slider_PhaseX    ->setTextBoxStyle (Slider::NoTextBox, false, 78, 28);
    slider_PhaseX    ->addListener (this);
    slider_PhaseX    ->setTextValueSuffix("%");
    slider_PhaseX    ->setNumDecimalPlacesToDisplay(1);
    slider_PhaseX    ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_PhaseX);
    
    slider_PhaseY = new Slider("");
    slider_PhaseY    ->setRange (-100, 100.0, 0);
    slider_PhaseY    ->setSliderStyle (Slider::LinearHorizontal);
    slider_PhaseY    ->setTextBoxStyle (Slider::NoTextBox, false, 78, 28);
    slider_PhaseY    ->addListener (this);
    slider_PhaseY    ->setTextValueSuffix("%");
    slider_PhaseY    ->setNumDecimalPlacesToDisplay(1);
    slider_PhaseY    ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_PhaseY);
    
    slider_PhaseZ = new Slider("");
    slider_PhaseZ    ->setRange (-100, 100.0, 0);
    slider_PhaseZ    ->setSliderStyle (Slider::LinearHorizontal);
    slider_PhaseZ    ->setTextBoxStyle (Slider::NoTextBox, false, 78, 28);
    slider_PhaseZ    ->addListener (this);
    slider_PhaseZ    ->setTextValueSuffix("%");
    slider_PhaseZ    ->setNumDecimalPlacesToDisplay(1);
    slider_PhaseZ    ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_PhaseZ);
    
    for (int i = 0; i < 3; i++)
    {
        buttonFreeFlow[i] = new ImageButton();
        buttonFreeFlow[i]->setTriggeredOnMouseDown(true);
        buttonFreeFlow[i]->setImages (false, true, true,
                                    imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                    Image(), 1.000f, Colour (0x00000000),
                                    imageBlueButtonSelected, 1.0, Colour (0x00000000));
        buttonFreeFlow[i]->addListener(this);
        addAndMakeVisible(buttonFreeFlow[i]);

        
        buttonSequencer[i] = new ImageButton();
        buttonSequencer[i]->setTriggeredOnMouseDown(true);
        buttonSequencer[i]->setImages (false, true, true,
                                    imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                    Image(), 1.000f, Colour (0x00000000),
                                    imageBlueButtonSelected, 1.0, Colour (0x00000000));
        buttonSequencer[i]->addListener(this);
        addAndMakeVisible(buttonSequencer[i]);
        
        buttonChordOrFrequency[i] = new ImageButton();
        buttonChordOrFrequency[i]->setTriggeredOnMouseDown(true);
        buttonChordOrFrequency[i]->setImages (false, true, true,
                                    imageChordFreqButtonNormal, 0.999f, Colour (0x00000000),
                                    Image(), 1.000f, Colour (0x00000000),
                                    imageChordFreqButtonSelected, 1.0, Colour (0x00000000));
        buttonChordOrFrequency[i]->addListener(this);
        addAndMakeVisible(buttonChordOrFrequency[i]);
        
        
        
        
        buttonOpenSettings[i] = new ImageButton();
        buttonOpenSettings[i]->setTriggeredOnMouseDown(true);
        buttonOpenSettings[i]->setImages (false, true, true,
                                    imageSettingsIcon, 0.999f, Colour (0x00000000),
                                    Image(), 1.000f, Colour (0x00000000),
                                    imageSettingsIcon, 0.75, Colour (0x00000000));
        buttonOpenSettings[i]->addListener(this);
        addAndMakeVisible(buttonOpenSettings[i]);
        
        buttonMute[i] = new ImageButton();
        buttonMute[i]->setTriggeredOnMouseDown(true);
        buttonMute[i]->setImages (false, true, true,
                                    imageMuteIcon, 0.999f, Colour (0x00000000),
                                    Image(), 1.000f, Colour (0x00000000),
                                    imageMuteIcon, 1.0, Colour (0x00000000));
        buttonMute[i]->addListener(this);
        addAndMakeVisible(buttonMute[i]);
        
        labelFrequency[i] = new Label("");
        labelFrequency[i]   ->setText("432hz", dontSendNotification);
        labelFrequency[i]   ->setJustificationType(Justification::left);
        labelFrequency[i]   ->setColour(Label::textColourId, Colours::darkgrey);
        addAndMakeVisible(labelFrequency[i]);
        
        
    }
    
    textEditor_SliderX = new TextEditor();
    textEditor_SliderX->setInputRestrictions(3, "0123456789.");
    textEditor_SliderX->addListener(this);
    textEditor_SliderX->setLookAndFeel(&lookAndFeel);
    textEditor_SliderX->setJustification(Justification::centred);
    textEditor_SliderX->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditor_SliderX->applyColourToAllText(Colours::darkgrey);
    addAndMakeVisible(textEditor_SliderX);
    
    textEditor_SliderY = new TextEditor();
    textEditor_SliderY->setInputRestrictions(3, "0123456789.");
    textEditor_SliderY->addListener(this);
    textEditor_SliderY->setLookAndFeel(&lookAndFeel);
    textEditor_SliderY->setJustification(Justification::centred);
    textEditor_SliderY->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditor_SliderY->applyColourToAllText(Colours::darkgrey);
    addAndMakeVisible(textEditor_SliderY);
    
    textEditor_SliderZ = new TextEditor();
    textEditor_SliderZ->setInputRestrictions(3, "0123456789.");
    textEditor_SliderZ->addListener(this);
    textEditor_SliderZ->setLookAndFeel(&lookAndFeel);
    textEditor_SliderZ->setJustification(Justification::centred);
    textEditor_SliderZ->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditor_SliderZ->applyColourToAllText(Colours::darkgrey);
    addAndMakeVisible(textEditor_SliderZ);
    
    textEditor_SliderPhaseX = new TextEditor();
    textEditor_SliderPhaseX->setInputRestrictions(3, "0123456789.-");
    textEditor_SliderPhaseX->addListener(this);
    textEditor_SliderPhaseX->setLookAndFeel(&lookAndFeel);
    textEditor_SliderPhaseX->setJustification(Justification::centred);
    textEditor_SliderPhaseX->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditor_SliderPhaseX->applyColourToAllText(Colours::darkgrey);
    addAndMakeVisible(textEditor_SliderPhaseX);
    
    textEditor_SliderPhaseY = new TextEditor();
    textEditor_SliderPhaseY->setInputRestrictions(3, "0123456789.-");
    textEditor_SliderPhaseY->addListener(this);
    textEditor_SliderPhaseY->setLookAndFeel(&lookAndFeel);
    textEditor_SliderPhaseY->setJustification(Justification::centred);
    textEditor_SliderPhaseY->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditor_SliderPhaseY->applyColourToAllText(Colours::darkgrey);
    addAndMakeVisible(textEditor_SliderPhaseY);
    
    textEditor_SliderPhaseZ = new TextEditor();
    textEditor_SliderPhaseZ->setInputRestrictions(3, "0123456789.-");
    textEditor_SliderPhaseZ->addListener(this);
    textEditor_SliderPhaseZ->setLookAndFeel(&lookAndFeel);
    textEditor_SliderPhaseZ->setJustification(Justification::centred);
    textEditor_SliderPhaseZ->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditor_SliderPhaseZ->applyColourToAllText(Colours::darkgrey);
    addAndMakeVisible(textEditor_SliderPhaseZ);

    
    
    viewerComponent = new LissajousCurveViewerComponent(projectManager, true);
    viewerComponent->setBounds(38, 48, 1482, 884);
    addAndMakeVisible(viewerComponent);
    
    // popups
    
    for (int i = 0; i < 3; i++)
    {
        settingsChordComponent[i] = new LissajousChordPlayerSettingsComponent(projectManager, i);
        addAndMakeVisible(settingsChordComponent[i]);
        settingsChordComponent[i]->setBounds(0, 0, 1566, 1440);
        settingsChordComponent[i]->setVisible(false);
        
        settingsFrequencyComponent[i] = new LissajousFrequencyPlayerSettingsComponent(projectManager, i);
        addAndMakeVisible(settingsFrequencyComponent[i]);
        settingsFrequencyComponent[i]->setBounds(0, 0, 1566, 1440);
        settingsFrequencyComponent[i]->setVisible(false);
    }

    viewerComponent->setShouldUpdate(true);
}

LissajousCurveComponent::~LissajousCurveComponent()
{
    
    
}

void LissajousCurveComponent::paint (Graphics& g)
{
    g.drawImage(background, 0, 0, 1562*scaleFactor, 1440*scaleFactor, 0, 0, 1562, 1440);
}

void LissajousCurveComponent::resized()
{
    int stopLeftMargin      = 875 + 249 + 24;
    int playLeftMargin      = 875;
    int playTopMargin       = 1346;
    int playWidth           = 249;
    int playHeight          = 61;
    int panicLeftMargin     = 20;
    int panicTopMargin      = 1320;
    int panicWidth          = 110;
    int panicHeight         = panicWidth;
    
    button_Play->setBounds(playLeftMargin * scaleFactor, playTopMargin * scaleFactor, playWidth * scaleFactor, playHeight * scaleFactor);
    button_Stop->setBounds(stopLeftMargin * scaleFactor, playTopMargin * scaleFactor, playWidth * scaleFactor, playHeight * scaleFactor);
    button_Panic->setBounds(panicLeftMargin * scaleFactor, panicTopMargin * scaleFactor, panicWidth * scaleFactor, panicHeight * scaleFactor);
    button_PlayInLoop->setBounds(1220 * scaleFactor, 884 * scaleFactor, 300 * scaleFactor, 80 * scaleFactor);
    
    
    
//    textEditor_Frequency->applyFontToAllText(33  * scaleFactor);
//    textEditor_Frequency->setBounds(190 * scaleFactor, 1334 * scaleFactor, 188 * scaleFactor, 40 * scaleFactor);
    
    float knobL = 300;
    float knobY_xaxis = 956;
    float knobY_yaxis = 1076;
    float knobY_zaxis = 1200;
    float boxW = 60;
    float boxH = 26;
    float knobW = 126;
    float knobH = 126;
    
    slider_X    ->setBounds(knobL * scaleFactor, knobY_xaxis * scaleFactor, knobW * scaleFactor, knobH * scaleFactor);
    slider_X    ->setTextBoxStyle (Slider::NoTextBox, false, boxW * scaleFactor, boxH * scaleFactor);
    
    slider_Y    ->setBounds(knobL * scaleFactor, knobY_yaxis * scaleFactor, knobW * scaleFactor, knobH * scaleFactor);
    slider_Y    ->setTextBoxStyle (Slider::NoTextBox, false, boxW * scaleFactor, boxH * scaleFactor);
    
    slider_Z    ->setTextBoxStyle (Slider::NoTextBox, false, boxW * scaleFactor, boxH * scaleFactor);
    slider_Z    ->setBounds(knobL * scaleFactor, knobY_zaxis * scaleFactor, knobW * scaleFactor, knobH * scaleFactor);
    
    float phaseL = 1117;
    float phaseSliderW = 333;
    float phaseSliderH = 50;
    
    float phaseY_xaxis = 983;
    float phaseY_yaxis = 1108;
    float phaseY_zaxis = 1230;

    slider_PhaseX    ->setBounds(phaseL * scaleFactor, phaseY_xaxis * scaleFactor, phaseSliderW  * scaleFactor, phaseSliderH * scaleFactor);
    slider_PhaseY    ->setBounds(phaseL * scaleFactor, phaseY_yaxis * scaleFactor, phaseSliderW  * scaleFactor, phaseSliderH * scaleFactor);
    slider_PhaseZ    ->setBounds(phaseL * scaleFactor, phaseY_zaxis * scaleFactor, phaseSliderW  * scaleFactor, phaseSliderH * scaleFactor);
    
    float textEdL = 210;
    float textEditorW = 70;
    float textEditorH = 58;
    float yDif = 24;
    textEditor_SliderX  ->setBounds(textEdL * scaleFactor, (phaseY_xaxis + yDif + 3) * scaleFactor, textEditorW  * scaleFactor, textEditorH * scaleFactor);
    textEditor_SliderY  ->setBounds(textEdL * scaleFactor, (phaseY_yaxis + yDif) * scaleFactor, textEditorW  * scaleFactor, textEditorH * scaleFactor);
    textEditor_SliderZ  ->setBounds(textEdL * scaleFactor, (phaseY_zaxis + yDif) * scaleFactor, textEditorW  * scaleFactor, textEditorH * scaleFactor);
    
    float textEditorPhaseL = 1440;
    yDif = 9;
    textEditor_SliderPhaseX ->setBounds(textEditorPhaseL * scaleFactor, (phaseY_xaxis + yDif) * scaleFactor, textEditorW  * scaleFactor, textEditorH * scaleFactor);
    textEditor_SliderPhaseY ->setBounds(textEditorPhaseL * scaleFactor, (phaseY_yaxis + yDif) * scaleFactor, textEditorW  * scaleFactor, textEditorH * scaleFactor);
    textEditor_SliderPhaseZ ->setBounds(textEditorPhaseL * scaleFactor, (phaseY_zaxis + yDif) * scaleFactor, textEditorW  * scaleFactor, textEditorH * scaleFactor);
    

    float buttonW = 38;
    float buttonL = 930;
    float buttonY[3];
    
    float iconW = 26;
    float settingsL = 600;
    float labelL = 460;
    float labelW = 100;
    buttonY[0] = 978; buttonY[1] = 1100; buttonY[2] = 1224;
    
    for (int i = 0; i < 3; i++)
    {
        buttonChordOrFrequency[i]  ->setBounds(586 * scaleFactor, buttonY[i] * scaleFactor, 224 * scaleFactor, 32 * scaleFactor);
        buttonFreeFlow[i]   ->setBounds(buttonL * scaleFactor, buttonY[i] * scaleFactor, buttonW * scaleFactor, buttonW * scaleFactor);

        buttonSequencer[i]  ->setBounds(buttonL * scaleFactor, (buttonY[i] + 40) * scaleFactor, buttonW * scaleFactor, buttonW * scaleFactor);
        
        settingsChordComponent[i]       ->setBounds(0, 0, 1566 * scaleFactor, 1440 * scaleFactor);
        
        settingsFrequencyComponent[i]   ->setBounds(0, 0, 1566 * scaleFactor, 1440 * scaleFactor);
        
//        buttonMute[i]                   ->setBounds(muteL * scaleFactor, (buttonY[i] + 40) * scaleFactor, iconW * scaleFactor, iconW * scaleFactor);
        
        buttonOpenSettings[i]           ->setBounds(settingsL * scaleFactor, (buttonY[i] + 44) * scaleFactor, iconW * scaleFactor, iconW * scaleFactor);
        
        labelFrequency[i]  ->setBounds(labelL * scaleFactor, (buttonY[i] + 44) * scaleFactor, labelW * scaleFactor, iconW * scaleFactor);
        labelFrequency[i]->setFont(24 * scaleFactor);
    }
    
    viewerComponent->setBounds(38 * scaleFactor, 20 * scaleFactor, 1482 * scaleFactor, 884 * scaleFactor);
    

}


void LissajousCurveComponent::buttonClicked (Button*button)
{

    if (button == button_Play)
    {
        projectManager->setPlayerCommand(PLAYER_COMMANDS::COMMAND_PLAYER_PLAYPAUSE);
    }
    else if (button == button_Stop)
    {
        projectManager->setPlayerCommand(PLAYER_COMMANDS::COMMAND_PLAYER_STOP);
    }
    else if (button == button_Panic)
    {
        projectManager->setPanicButton();
    }
    else if (button == button_PlayInLoop)
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
    
    else if (button == buttonFreeFlow[0])
    {
        projectManager->setLissajousParameter(UNIT_1_FREE_FLOW, 0);
    }
    else if (button == buttonFreeFlow[1])
    {
        projectManager->setLissajousParameter(UNIT_2_FREE_FLOW, 0);
    }
    else if (button == buttonFreeFlow[2])
    {
        projectManager->setLissajousParameter(UNIT_3_FREE_FLOW, 0);
    }
    else if (button == buttonSequencer[0])
    {
        projectManager->setLissajousParameter(UNIT_1_FREE_FLOW, 1);
    }
    else if (button == buttonSequencer[1])
    {
        projectManager->setLissajousParameter(UNIT_2_FREE_FLOW, 1);
    }
    else if (button == buttonSequencer[2])
    {
        projectManager->setLissajousParameter(UNIT_3_FREE_FLOW, 1);
    }
    
    else if (button == buttonChordOrFrequency[0])
    {
        projectManager->setLissajousParameter(UNIT_1_PROCESSOR_TYPE, !buttonChordOrFrequency[0]->getToggleState());
    }
    else if (button == buttonChordOrFrequency[1])
    {
        projectManager->setLissajousParameter(UNIT_2_PROCESSOR_TYPE, !buttonChordOrFrequency[1]->getToggleState());
    }
    else if (button == buttonChordOrFrequency[2])
    {
        projectManager->setLissajousParameter(UNIT_3_PROCESSOR_TYPE, !buttonChordOrFrequency[2]->getToggleState());
    }
    
    else if (button == buttonOpenSettings[0])
    {
        openSettingsView(0);
    }
    else if (button == buttonOpenSettings[1])
    {
        openSettingsView(1);
    }
    else if (button == buttonOpenSettings[2])
    {
        openSettingsView(2);
    }
    
    
    
}


void LissajousCurveComponent::sliderValueChanged (Slider* slider)
{
    if (slider == slider_X)
    {
        projectManager->setLissajousParameter(AMPLITUDE_X, slider_X->getValue());
    }
    else if (slider == slider_Y)
    {
        projectManager->setLissajousParameter(AMPLITUDE_Y, slider_Y->getValue());
    }
    else if (slider == slider_Z)
    {
        projectManager->setLissajousParameter(AMPLITUDE_Z, slider_Z->getValue());
    }
    
    else if (slider == slider_PhaseX)
    {
        projectManager->setLissajousParameter(UNIT_1_PHASE, slider_PhaseX->getValue());
    }
    else if (slider == slider_PhaseY)
    {
        projectManager->setLissajousParameter(UNIT_2_PHASE, slider_PhaseY->getValue());
    }
    else if (slider == slider_PhaseZ)
    {
        projectManager->setLissajousParameter(UNIT_3_PHASE, slider_PhaseZ->getValue());
    }
    
}

void LissajousCurveComponent::textEditorReturnKeyPressed (TextEditor&editor)
{
    if (&editor == textEditor_SliderX)
    {
        double value = editor.getText().getDoubleValue();

        // bounds check
        if (value >= 100) {  value = 100; } if (value <= 0) { value = 0; }
        
        projectManager->setLissajousParameter(AMPLITUDE_X, value);
    }
    else if (&editor == textEditor_SliderY)
    {
        double value = editor.getText().getDoubleValue();

        // bounds check
        if (value >= 100) {  value = 100; } if (value <= 0) { value = 0; }
        
        projectManager->setLissajousParameter(AMPLITUDE_Y, value);
    }
    else if (&editor == textEditor_SliderZ)
    {
        double value = editor.getText().getDoubleValue();

        // bounds check
        if (value >= 100) {  value = 100; } if (value <= 0) { value = 0; }
        
        projectManager->setLissajousParameter(AMPLITUDE_Z, value);
    }
    
    else if (&editor == textEditor_SliderPhaseX)
    {
        double value = editor.getText().getDoubleValue();

        // bounds check
        if (value >= 100) {  value = 100; } if (value <= -100) { value = -100; }
        
        projectManager->setLissajousParameter(UNIT_1_PHASE, value);
    }
    else if (&editor == textEditor_SliderPhaseY)
    {
        double value = editor.getText().getDoubleValue();

        // bounds check
        if (value >= 100) {  value = 100; } if (value <= -100) { value = -100; }
        
        projectManager->setLissajousParameter(UNIT_2_PHASE, value);
    }
    else if (&editor == textEditor_SliderPhaseZ)
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
