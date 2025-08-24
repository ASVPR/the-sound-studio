/*
  ==============================================================================

    FrequencyScannerComponent.cpp
    Created: 29 Sep 2019 10:38:05am
    Author:  Gary Jones

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "FrequencyScannerComponent.h"

//==============================================================================
FrequencyScannerComponent::FrequencyScannerComponent(ProjectManager * pm)
{
    projectManager = pm;
    projectManager->addUIListener(this);
    
    setWantsKeyboardFocus(true);
    
    // Font
    Typeface::Ptr AssistantLight        = Typeface::createSystemTypefaceFor(BinaryData::AssistantLight_ttf, BinaryData::AssistantLight_ttfSize);
    Typeface::Ptr AssistantSemiBold     = Typeface::createSystemTypefaceFor(BinaryData::AssistantSemiBold_ttf, BinaryData::AssistantSemiBold_ttfSize);
    Font fontSemiBold(AssistantSemiBold);
    Font fontLight(AssistantLight);
    int fontSize = 35;
    fontSemiBold.setHeight(fontSize);
    fontLight.setHeight(fontSize);
    
    // images
    
    imageMainBackground                 = ImageCache::getFromMemory(BinaryData::FrequencyScannerBackground2_png, BinaryData::FrequencyScannerBackground2_pngSize);
    
    imagePanicButton                    = ImageCache::getFromMemory(BinaryData::PanicButton2x_png, BinaryData::PanicButton2x_pngSize);
    imagePlayButton                     = ImageCache::getFromMemory(BinaryData::playPause2x_png, BinaryData::playPause2x_pngSize);
    imageProgresBarFill                 = ImageCache::getFromMemory(BinaryData::ProgressBarFill2x_png, BinaryData::ProgressBarFill2x_pngSize);
    imageSettingsIcon                   = ImageCache::getFromMemory(BinaryData::settings2x_png, BinaryData::settings2x_pngSize);
    imageAddIcon                        = ImageCache::getFromMemory(BinaryData::ShortcutAdd2x_png, BinaryData::ShortcutAdd2x_pngSize);
    imageCloseIcon                      = ImageCache::getFromMemory(BinaryData::ShortcutClose2x_png, BinaryData::ShortcutClose2x_pngSize);
    imageLeftIcon                       = ImageCache::getFromMemory(BinaryData::ShortcutLeft2x_png, BinaryData::ShortcutLeft2x_pngSize);
    imageRightIcon                      = ImageCache::getFromMemory(BinaryData::ShortcutRight2x_png, BinaryData::ShortcutRight2x_pngSize);
    imageLoopIcon                       = ImageCache::getFromMemory(BinaryData::ShortcutLoop2x_png, BinaryData::ShortcutLoop2x_pngSize);
    imageMuteIcon                       = ImageCache::getFromMemory(BinaryData::ShortcutMute2x_png, BinaryData::ShortcutMute2x_pngSize);
    imageStopButton                     = ImageCache::getFromMemory(BinaryData::stop2x_png, BinaryData::stop2x_pngSize);
    imageRecordButton                   = ImageCache::getFromMemory(BinaryData::RecordButton_png, BinaryData::RecordButton_pngSize);
    imageFFTMockup                      = ImageCache::getFromMemory(BinaryData::FFTMockup_png, BinaryData::FFTMockup_pngSize);
    imageColorSpectrumMockup            = ImageCache::getFromMemory(BinaryData::ColorSpectrumMockup_png, BinaryData::ColorSpectrumMockup_pngSize);
    imageOctaveSpectrumMockup           = ImageCache::getFromMemory(BinaryData::OctaveSpectrumMockup_png, BinaryData::OctaveSpectrumMockup_pngSize);
    imageBlueButtonNormal               = ImageCache::getFromMemory(BinaryData::BlueButton_Normal_png, BinaryData::BlueButton_Normal_pngSize);
    imageBlueButtonSelected             = ImageCache::getFromMemory(BinaryData::BlueButton_Selected_png, BinaryData::BlueButton_Selected_pngSize);
    imageBlueCheckButtonNormal          = ImageCache::getFromMemory(BinaryData::Button_Checkbox_Normal_Max_png, BinaryData::Button_Checkbox_Normal_Max_pngSize);
    imageBlueCheckButtonSelected        = ImageCache::getFromMemory(BinaryData::Button_Checkbox_Selected_Max_png, BinaryData::Button_Checkbox_Selected_Max_pngSize);
    
    
    
    // Knobs
    int knobY = 916;
    int knobL = 374;
    int dif = 248;
    int moveY = 154;

    
    
    
    slider_Amplitude = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_AMPLITUDE);
    slider_Amplitude    ->setRange (0, 1.0, 0);
    slider_Amplitude    ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_Amplitude    ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_Amplitude    ->addListener (this);
    slider_Amplitude    ->setTextValueSuffix("%");
    slider_Amplitude    ->setBounds(knobL, knobY-moveY, 130, 158);
    slider_Amplitude    ->setNumDecimalPlacesToDisplay(1);
    slider_Amplitude    ->setLookAndFeel(&lookAndFeel);
    
    addAndMakeVisible(slider_Amplitude.get());
    
    
    slider_Attack = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_ATTACK);
    slider_Attack       ->setRange (0, 1.0, 0);
    slider_Attack       ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_Attack       ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_Attack       ->addListener (this);
    slider_Attack       ->setTextValueSuffix("Ms");
    slider_Attack       ->setBounds(knobL+(1 * dif), knobY-moveY, 130, 158);
    slider_Attack       ->setNumDecimalPlacesToDisplay(0);
    slider_Attack       ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_Attack.get());
    
    slider_Sustain = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_SUSTAIN);
    slider_Sustain      ->setRange (0, 1.0, 0);
    slider_Sustain      ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_Sustain      ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_Sustain      ->addListener (this);
    slider_Sustain      ->setTextValueSuffix("%");
    slider_Sustain      ->setBounds(knobL+(2 * dif), knobY-moveY, 130, 158);
    slider_Sustain      ->setNumDecimalPlacesToDisplay(2);
    slider_Sustain        ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_Sustain.get());
    
    slider_Decay = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_DECAY);
    slider_Decay        ->setRange (0, 1.0, 0);
    slider_Decay        ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_Decay        ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_Decay        ->addListener (this);
    slider_Decay        ->setTextValueSuffix("Ms");
    slider_Decay        ->setBounds(knobL+(3 * dif), knobY-moveY, 130, 158);
    slider_Decay        ->setNumDecimalPlacesToDisplay(0);
    slider_Decay        ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_Decay.get());
    
    slider_Release = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_RELEASE);
    slider_Release      ->setRange (0, 1.0, 0);
    slider_Release      ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_Release      ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_Release      ->addListener (this);
    slider_Release      ->setTextValueSuffix("Ms");
    slider_Release      ->setBounds(knobL+(4 * dif), knobY-moveY, 130, 158);
    slider_Release      ->setNumDecimalPlacesToDisplay(0);
    slider_Release      ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_Release.get());
    
    // FFT
    

    
    
    label_Playing = std::make_unique<Label>();
    label_Playing->setText("Playing (00:40)", dontSendNotification);
    label_Playing->setBounds(playingLeftMargin, playingTopMargin, 300, 40);
    label_Playing->setJustificationType(Justification::left);
    fontLight.setHeight(33);
    label_Playing->setFont(fontLight);
    addAndMakeVisible(label_Playing.get());
    
    
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
    button_Panic->setBounds(panicLeftMargin, panicTopMargin, panicWidth, panicHeight);
    addAndMakeVisible(button_Panic.get());
    
    
    
    // Waveform buttons
    int waveY = 842;
    // act as toggle between 5 buttons
    button_Default = std::make_unique<ImageButton>();
    button_Default->setTriggeredOnMouseDown(true);
    button_Default->setImages (false, true, true,
                               imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                               Image(), 1.000f, Colour (0x00000000),
                               imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_Default->addListener(this);
//    button_Default->setBounds(183, waveY-moveY, 38, 38);
//    addAndMakeVisible(button_Default.get());
    
    button_Sine = std::make_unique<ImageButton>();
    button_Sine->setTriggeredOnMouseDown(true);
    button_Sine->setImages (false, true, true,
                            imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                            Image(), 1.000f, Colour (0x00000000),
                            imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_Sine->addListener(this);
    button_Sine->setBounds(330, waveY-moveY, 38, 38);
    addAndMakeVisible(button_Sine.get());
    
    button_Triangle = std::make_unique<ImageButton>();
    button_Triangle->setTriggeredOnMouseDown(true);
    button_Triangle->setImages (false, true, true,
                                imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_Triangle->addListener(this);
    button_Triangle->setBounds(639, waveY-moveY, 38, 38);
    addAndMakeVisible(button_Triangle.get());
    
    button_Square = std::make_unique<ImageButton>();
    button_Square->setTriggeredOnMouseDown(true);
    button_Square->setImages (false, true, true,
                              imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                              Image(), 1.000f, Colour (0x00000000),
                              imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_Square->addListener(this);
    button_Square->setBounds(924, waveY-moveY, 38, 38);
    addAndMakeVisible(button_Square.get());
    
    button_Sawtooth = std::make_unique<ImageButton>();
    button_Sawtooth->setTriggeredOnMouseDown(true);
    button_Sawtooth->setImages (false, true, true,
                                imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_Sawtooth->addListener(this);
    button_Sawtooth->setBounds(1240, waveY-moveY, 38, 38);
    addAndMakeVisible(button_Sawtooth.get());
    
    button_Wavetable = std::make_unique<ImageButton>();
    button_Wavetable->setTriggeredOnMouseDown(true);
    button_Wavetable->setImages (false, true, true,
                                                imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                                Image(), 1.000f, Colour (0x00000000),
                                                imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_Wavetable->addListener(this);
    addAndMakeVisible(button_Wavetable.get());
    
    button_WavetableEditor = std::make_unique<TextButton>("");
    button_WavetableEditor->setButtonText("WT Editor");
    button_WavetableEditor->setLookAndFeel(&lookAndFeel);
    button_WavetableEditor->addListener(this);
    addAndMakeVisible(button_WavetableEditor.get());
    
    
    //Chord Scan buttons
    int size    = 50;
    int scanY   = 462;

    
    
    
    
    button_ScanAllFrequencies = std::make_unique<ImageButton>();
    button_ScanAllFrequencies->setTriggeredOnMouseDown(true);
    button_ScanAllFrequencies->setImages (false, true, true,
                                     imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                     Image(), 1.000f, Colour (0x00000000),
                                     imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_ScanAllFrequencies->addListener(this);
    button_ScanAllFrequencies->setBounds(214, scanY, size, size);
    addAndMakeVisible(button_ScanAllFrequencies.get());
    
    button_ScanSpecificRange = std::make_unique<ImageButton>();
    button_ScanSpecificRange->setTriggeredOnMouseDown(true);
    button_ScanSpecificRange->setImages (false, true, true,
                                         imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                         Image(), 1.000f, Colour (0x00000000),
                                         imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_ScanSpecificRange->addListener(this);
    button_ScanSpecificRange->setBounds(542, scanY, size, size);
    addAndMakeVisible(button_ScanSpecificRange.get());
    
    
    
    
    // Text Entry for Repeater
    int playY = 1162;
    int playXAdjust = 164;
    
    textEditorRepeat = std::make_unique<TextEditor>("");
    textEditorRepeat->setReturnKeyStartsNewLine(false);
    textEditorRepeat->setInputRestrictions(5, "0123456789");
    textEditorRepeat->setMultiLine(false);
    textEditorRepeat->setText("1");
    textEditorRepeat->addListener(this);
    textEditorRepeat->setLookAndFeel(&lookAndFeel);
    textEditorRepeat->setJustification(Justification::centred);
    textEditorRepeat->setFont(fontSemiBold);
    textEditorRepeat->setColour(TextEditor::textColourId, Colours::lightgrey);
    textEditorRepeat->applyFontToAllText(fontSemiBold);
//    textEditorPause->applyColourToAllText(Colours::lightgrey);
    textEditorRepeat->setBounds(760-playXAdjust-40, playY, 111, 35);
    addAndMakeVisible(textEditorRepeat.get());
    
    
    
    textEditorPause = std::make_unique<TextEditor>("");
    textEditorPause->setReturnKeyStartsNewLine(false);
    textEditorPause->setInputRestrictions(5, "0123456789");
    textEditorPause->setMultiLine(false);
    textEditorPause->setText("1000");
    textEditorPause->addListener(this);
    textEditorPause->setLookAndFeel(&lookAndFeel);
    textEditorPause->setJustification(Justification::centred);
    textEditorPause->setFont(fontSemiBold);
    textEditorPause->setColour(TextEditor::textColourId, Colours::lightgrey);
    textEditorPause->applyFontToAllText(fontSemiBold);
//    textEditorPause->applyColourToAllText(Colours::lightgrey);
    textEditorPause->setBounds(1094-playXAdjust-40, playY, 111, 35);
    addAndMakeVisible(textEditorPause.get());
    
    
    textEditorLength = std::make_unique<TextEditor>("");
    textEditorLength->setReturnKeyStartsNewLine(false);
    textEditorLength->setMultiLine(false);
    textEditorLength->setInputRestrictions(5, "0123456789");
    textEditorLength->setText("1000");
    textEditorLength->addListener(this);
    textEditorLength->setLookAndFeel(&lookAndFeel);
    textEditorLength->setJustification(Justification::centred);
    textEditorLength->setFont(fontSemiBold);
    textEditorLength->setColour(TextEditor::textColourId, Colours::lightgrey);
    textEditorLength->applyFontToAllText(fontSemiBold);
//    textEditorLength->applyColourToAllText(Colours::lightgrey);
    textEditorLength->setBounds(794-playXAdjust, playY, 111, 35);
    addAndMakeVisible(textEditorLength.get());
    
    
    // visualiser component
    visualiserContainerComponent = std::make_unique<VisualiserContainerComponent2>(projectManager, AUDIO_MODE::MODE_FREQUENCY_SCANNER);
    visualiserContainerComponent->setBounds(350, 28, 1146, 320);
    addAndMakeVisible(visualiserContainerComponent.get());
    
    // Progress bar
    progressBar = std::make_unique<CustomProgressBar>();
    progressBar->setBounds(292, 372, 1226, 42);
    addAndMakeVisible(progressBar.get());
    
    
    // Load / Save Button
    button_Load = std::make_unique<TextButton>("Load");
    button_Load->setBounds(1470, 1204, 100, 40);
    button_Load->addListener(this);
    button_Load->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(button_Load.get());
    
    button_Save = std::make_unique<TextButton>("Save");
    button_Save->setBounds(1360, 1204, 100, 40);
    button_Save->addListener(this);
    button_Save->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(button_Save.get());
    

  
    
    
    label_CurrentPlayingFrequency = std::make_unique<Label>();
    label_CurrentPlayingFrequency->setText("0.000 Hz", dontSendNotification);
    label_CurrentPlayingFrequency->setBounds(184, 49, 190, 40);
    label_CurrentPlayingFrequency->setJustificationType(Justification::centred);
    fontLight.setHeight(33);
    label_CurrentPlayingFrequency->setFont(fontLight);
    label_CurrentPlayingFrequency->setColour(Label::textColourId , Colours::darkgrey);
    addAndMakeVisible(label_CurrentPlayingFrequency.get());
    

    
    //=============================================
    
    component_FrequencyContainer = std::make_unique<Component>();
    component_FrequencyContainer->setBounds(0, 563-7.5, 1562, size);
    addAndMakeVisible(component_FrequencyContainer.get());
    
    //=============================================
    
    int octaveY = 563;
    int keynoteToX = 420;
    int keynoteFromX =  176;
    int comboBoxWidth = 190;
    
    
    textEditorFrequencyFrom = std::make_unique<TextEditor>("");
    textEditorFrequencyFrom->setReturnKeyStartsNewLine(false);
    textEditorFrequencyFrom->setMultiLine(false);
    textEditorFrequencyFrom->setInputRestrictions(12, "0123456789.");
    textEditorFrequencyFrom->setText("432");
    textEditorFrequencyFrom->addListener(this);
    textEditorFrequencyFrom->setLookAndFeel(&lookAndFeel);
    textEditorFrequencyFrom->setJustification(Justification::centred);
    textEditorFrequencyFrom->setFont(fontSemiBold);
    textEditorFrequencyFrom->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditorFrequencyFrom->applyFontToAllText(fontSemiBold);
    textEditorFrequencyFrom->setBounds(keynoteFromX+30, 7.5, comboBoxWidth, 35);
    component_FrequencyContainer->addAndMakeVisible(textEditorFrequencyFrom.get());
    
    textEditorFrequencyTo = std::make_unique<TextEditor>("");
    textEditorFrequencyTo->setReturnKeyStartsNewLine(false);
    textEditorFrequencyTo->setMultiLine(false);
    textEditorFrequencyTo->setInputRestrictions(12, "0123456789.");
    textEditorFrequencyTo->setText("0.000 Hz");
    textEditorFrequencyTo->addListener(this);
    textEditorFrequencyTo->setLookAndFeel(&lookAndFeel);
    textEditorFrequencyTo->setJustification(Justification::centred);
    textEditorFrequencyTo->setFont(fontSemiBold);
    textEditorFrequencyTo->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditorFrequencyTo->applyFontToAllText(fontSemiBold);
    textEditorFrequencyTo->setBounds(keynoteToX+40, 7.5, comboBoxWidth, 35);
    component_FrequencyContainer->addAndMakeVisible(textEditorFrequencyTo.get());
    
    
    int buttonY = 5;
    button_ExtendedRange = std::make_unique<ImageButton>();
    button_ExtendedRange->setTriggeredOnMouseDown(true);
    button_ExtendedRange->setImages (false, true, true,
                                imageBlueCheckButtonNormal, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageBlueCheckButtonSelected, 1.0, Colour (0x00000000));
    button_ExtendedRange->addListener(this);
    button_ExtendedRange->setBounds(666, 6, 38, 38);
    component_FrequencyContainer->addAndMakeVisible(button_ExtendedRange.get());
    
    //Log / Lin buttons
    button_Log = std::make_unique<ImageButton>();
    button_Log->setTriggeredOnMouseDown(true);
    button_Log->setImages (false, true, true,
                                imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_Log->addListener(this);
    button_Log->setBounds(864, 0, size, size);
    component_FrequencyContainer->addAndMakeVisible(button_Log.get());
    
    
    button_Lin = std::make_unique<ImageButton>();
    button_Lin ->setTriggeredOnMouseDown(true);
    button_Lin ->setImages (false, true, true,
                                imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_Lin ->addListener(this);
    button_Lin ->setBounds(1184, 0, size, size);
    component_FrequencyContainer->addAndMakeVisible(button_Lin.get());
    
    
    textEditor_LogValue = std::make_unique<TextEditor>("");
    textEditor_LogValue->setReturnKeyStartsNewLine(false);
    textEditor_LogValue->setMultiLine(false);
    textEditor_LogValue->setInputRestrictions(10, "0123456789.");
    textEditor_LogValue->setText("0.01 Oct");
    textEditor_LogValue->addListener(this);
    textEditor_LogValue->setLookAndFeel(&lookAndFeel);
    textEditor_LogValue->setJustification(Justification::centred);
    textEditor_LogValue->setFont(fontSemiBold);
    textEditor_LogValue->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditor_LogValue->applyFontToAllText(fontSemiBold);
    textEditor_LogValue->setBounds(972, 7.5, comboBoxWidth, 35);
    component_FrequencyContainer->addAndMakeVisible(textEditor_LogValue.get());
    
    textEditor_LinValue = std::make_unique<TextEditor>("");
    textEditor_LinValue->setReturnKeyStartsNewLine(false);
    textEditor_LinValue->setMultiLine(false);
    textEditor_LinValue->setInputRestrictions(10, "0123456789.");
    textEditor_LinValue->setText("0.0 Hz");
    textEditor_LinValue->addListener(this);
    textEditor_LinValue->setLookAndFeel(&lookAndFeel);
    textEditor_LinValue->setJustification(Justification::centred);
    textEditor_LinValue->setFont(fontSemiBold);
    textEditor_LinValue->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditor_LinValue->applyFontToAllText(fontSemiBold);
    textEditor_LinValue->setBounds(1290, 7.5, comboBoxWidth, 35);
    component_FrequencyContainer->addAndMakeVisible(textEditor_LinValue.get());
    
    comboBoxOutputSelection = std::make_unique<ComboBox>();
    comboBoxOutputSelection->setLookAndFeel(&lookAndFeel);
    comboBoxOutputSelection->addListener(this);
    comboBoxOutputSelection->setSelectedId(0);
    projectManager->getOutputsPopupMenu(*comboBoxOutputSelection->getRootMenu());
    addAndMakeVisible(comboBoxOutputSelection.get());
    
    //===========================================================
    // end and update params
    
    for (int i = AMPLITUDE_MIN; i <= RELEASE_MAX; i++)
    {
        updateMinMaxSettings(i);
    }
    
    for (int i = 0; i < TOTAL_NUM_CHORD_SCANNER_PARAMS; i++) {
        updateChordScannerUIParameter(i);
    }
    
    wavetableEditorComponent = std::make_unique<WaveTableOscViewComponent>(projectManager, AUDIO_MODE::MODE_FREQUENCY_SCANNER, -1);
    wavetableEditorComponent->setBounds(0, 0, 600, 400);
    
    popupWavetableWindow = std::make_unique<PopupFFTWindow>("Wavetable Editor - Frequency Scanner", wavetableEditorComponent.get(), Colours::black, DocumentWindow::allButtons, true);
    popupWavetableWindow ->centreWithSize(600, 400);
    popupWavetableWindow ->setVisible(false);
    popupWavetableWindow ->setResizable(true, true);
    
    startTimerHz(TIMER_UPDATE_RATE);
}

FrequencyScannerComponent::~FrequencyScannerComponent() { }

void FrequencyScannerComponent::resized()
{
    int knobY = 916;
    int knobL = 374;
    int dif = 248;
    int moveY = 154;
    
    int fontSize = 30;
    
    slider_Amplitude    ->setBounds(knobL * scaleFactor, (knobY-moveY) * scaleFactor, 130 * scaleFactor, 158 * scaleFactor);
    slider_Amplitude    ->setTextBoxStyle (Slider::TextBoxBelow, false, 78 * scaleFactor, 28 * scaleFactor);
    
    slider_Attack       ->setBounds((knobL+(1 * dif)) * scaleFactor, (knobY-moveY) * scaleFactor, 130 * scaleFactor, 158 * scaleFactor);
    slider_Attack       ->setTextBoxStyle (Slider::TextBoxBelow, false, 78 * scaleFactor, 28 * scaleFactor);
    
    slider_Sustain      ->setBounds((knobL+(2 * dif)) * scaleFactor, (knobY-moveY) * scaleFactor, 130 * scaleFactor, 158 * scaleFactor);
    slider_Sustain      ->setTextBoxStyle (Slider::TextBoxBelow, false, 78 * scaleFactor, 28 * scaleFactor);
    
    slider_Decay        ->setBounds((knobL+(3 * dif)) * scaleFactor, (knobY-moveY) * scaleFactor, 130 * scaleFactor, 158 * scaleFactor);
    slider_Decay        ->setTextBoxStyle (Slider::TextBoxBelow, false, 78 * scaleFactor, 28 * scaleFactor);
    
    slider_Release      ->setBounds((knobL+(4 * dif)) * scaleFactor, (knobY-moveY) * scaleFactor, 130 * scaleFactor, 158 * scaleFactor);
    slider_Release      ->setTextBoxStyle (Slider::TextBoxBelow, false, 78 * scaleFactor, 28 * scaleFactor);
    
    
    label_Playing->setBounds(playingLeftMargin * scaleFactor, playingTopMargin * scaleFactor, 300 * scaleFactor, 40 * scaleFactor);
    label_Playing->setFont(fontSize * scaleFactor);
    
    button_Play->setBounds(playLeftMargin * scaleFactor, playTopMargin * scaleFactor, playWidth * scaleFactor, playHeight * scaleFactor);
    button_Stop->setBounds(stopLeftMargin * scaleFactor, playTopMargin * scaleFactor, playWidth * scaleFactor, playHeight * scaleFactor);
    button_Panic->setBounds(panicLeftMargin * scaleFactor, panicTopMargin * scaleFactor, panicWidth * scaleFactor, panicHeight * scaleFactor);
    int waveY = 842;
    button_Sine->setBounds(260 * scaleFactor, (waveY-moveY) * scaleFactor, 38 * scaleFactor, 38 * scaleFactor);
    button_Triangle->setBounds(500 * scaleFactor, (waveY-moveY) * scaleFactor, 38 * scaleFactor, 38 * scaleFactor);
    button_Square->setBounds(756 * scaleFactor, (waveY-moveY) * scaleFactor, 38 * scaleFactor, 38 * scaleFactor);
    button_Sawtooth->setBounds(1020 * scaleFactor, (waveY-moveY) * scaleFactor, 38 * scaleFactor, 38 * scaleFactor);
    
    int shift = 162;
    button_Wavetable                ->setBounds((1465-shift) * scaleFactor, (waveY-moveY) * scaleFactor, 38 * scaleFactor, 38 * scaleFactor);
    
    button_WavetableEditor          ->setBounds((1465-shift) * scaleFactor, 640 * scaleFactor, 150 * scaleFactor, 38 * scaleFactor);
        
    int size    = 50;
    int scanY   = 462;
    button_ScanAllFrequencies->setBounds(214 * scaleFactor, scanY * scaleFactor, size * scaleFactor, size * scaleFactor);
    button_ScanSpecificRange->setBounds(542 * scaleFactor, scanY * scaleFactor, size * scaleFactor, size * scaleFactor);
    int playY = 1162;
    int playXAdjust = 164+200;
    int playWidth = 140;
    
    textEditorRepeat->setBounds((674-playXAdjust-40) * scaleFactor, playY * scaleFactor, playWidth * scaleFactor, 35 * scaleFactor);
    textEditorRepeat->applyFontToAllText(30 * scaleFactor);
    
    
    textEditorPause->setBounds((1288-playXAdjust-40) * scaleFactor, playY * scaleFactor, playWidth * scaleFactor, 35 * scaleFactor);
    textEditorPause->applyFontToAllText(30 * scaleFactor);
    
    textEditorLength->setBounds((948-playXAdjust) * scaleFactor, playY * scaleFactor, playWidth * scaleFactor, 35 * scaleFactor);
    textEditorLength->applyFontToAllText(30 * scaleFactor);
    
    visualiserContainerComponent->setBounds(350 * scaleFactor, 28 * scaleFactor, 1150 * scaleFactor, 324 * scaleFactor);
    progressBar->setBounds(292 * scaleFactor, 372 * scaleFactor, 1226 * scaleFactor, 42 * scaleFactor);
    button_Load->setBounds(1470 * scaleFactor, 1040 * scaleFactor, 100 * scaleFactor, 40 * scaleFactor);
    button_Save->setBounds(1360 * scaleFactor, 1040 * scaleFactor, 100 * scaleFactor, 40 * scaleFactor);
    label_CurrentPlayingFrequency->setBounds(190 * scaleFactor, 49 * scaleFactor, 190 * scaleFactor, 40 * scaleFactor);
    label_CurrentPlayingFrequency->setFont(fontSize * scaleFactor);
    component_FrequencyContainer->setBounds(0, (563-7.5) * scaleFactor, 1562 * scaleFactor, size * scaleFactor);
    
    int octaveY = 563;
    int keynoteToX = 440;
    int keynoteFromX =  176;
    int comboBoxWidth = 220;
    
    textEditorFrequencyFrom->applyFontToAllText(33 * scaleFactor);
    textEditorFrequencyFrom->setBounds((keynoteFromX+30) * scaleFactor, 7.5 * scaleFactor, comboBoxWidth * scaleFactor, 35 * scaleFactor);
    textEditorFrequencyTo->applyFontToAllText(33 * scaleFactor);
    textEditorFrequencyTo->setBounds((keynoteToX+40) * scaleFactor, 7.5 * scaleFactor, comboBoxWidth * scaleFactor, 35 * scaleFactor);
    int buttonY = 5;
    button_ExtendedRange->setBounds(704 * scaleFactor, 6 * scaleFactor, 38 * scaleFactor, 38 * scaleFactor);
    button_Log->setBounds(900 * scaleFactor, 0, size * scaleFactor, size * scaleFactor);
    button_Lin ->setBounds(1210 * scaleFactor, 0, size * scaleFactor, size * scaleFactor);
    textEditor_LogValue->setBounds(980 * scaleFactor, 7.5 * scaleFactor, comboBoxWidth * scaleFactor, 35 * scaleFactor);
    textEditor_LogValue->applyFontToAllText(33 * scaleFactor);
    textEditor_LinValue->setBounds(1292 * scaleFactor, 7.5 * scaleFactor, comboBoxWidth * scaleFactor, 35 * scaleFactor);
    textEditor_LinValue->applyFontToAllText(33 * scaleFactor);
    
    comboBoxOutputSelection         -> setBounds(1150 * scaleFactor, 1160 * scaleFactor, 250 * scaleFactor, 43 * scaleFactor);
    
}


void FrequencyScannerComponent::timerCallback()
{
    if (projectManager->frequencyScannerProcessor->getPlaystate() == PLAY_STATE::PLAYING)
    {
        MessageManagerLock lock;
        
        float val = projectManager->frequencyScannerProcessor->repeater->getProgressBarValue();
        
        progressBar->setValue(val);
        
        String text = projectManager->frequencyScannerProcessor->repeater->getTimeRemainingInSecondsString();
        
        label_Playing->setText(text, dontSendNotification);
        
        String freqText = projectManager->frequencyScannerProcessor->repeater->getCurrentFrequencyString();
        label_CurrentPlayingFrequency->setText(freqText, dontSendNotification);
    }
    else
    {
        float val = projectManager->frequencyScannerProcessor->repeater->getProgressBarValue();
        
        progressBar->setValue(val);
    }
    
}

void FrequencyScannerComponent::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background
    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
    
    g.setOpacity(1.0);
    g.drawImage(imageMainBackground, 0, 0, 1562 * scaleFactor, 1440 * scaleFactor, 0, 0, 1562, 1440);
}

void FrequencyScannerComponent::buttonClicked (Button*button)
{
    if (button == button_Record.get())
    {
        projectManager->setPlayerCommand(PLAYER_COMMANDS::COMMAND_PLAYER_RECORD);
    }
    else if (button == button_Play.get())
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
    
    else if (button == button_Default.get())
    {
        projectManager->setFrequencyScannerParameter(FREQUENCY_SCANNER_WAVEFORM_TYPE, 0);
    }
    else if (button == button_Sine.get())
    {
        projectManager->setFrequencyScannerParameter(FREQUENCY_SCANNER_WAVEFORM_TYPE, 1);
    }
    else if (button == button_Triangle.get())
    {
        projectManager->setFrequencyScannerParameter(FREQUENCY_SCANNER_WAVEFORM_TYPE, 2);
    }
    else if (button == button_Square.get())
    {
        projectManager->setFrequencyScannerParameter(FREQUENCY_SCANNER_WAVEFORM_TYPE, 3);
    }
    else if (button == button_Sawtooth.get())
    {
        projectManager->setFrequencyScannerParameter(FREQUENCY_SCANNER_WAVEFORM_TYPE, 4);
    }
    else if (button == button_Wavetable.get())
    {
        projectManager->setFrequencyScannerParameter(FREQUENCY_SCANNER_WAVEFORM_TYPE, 5);
    }
    else if (button == button_WavetableEditor.get())
    {
        if (!popupWavetableWindow ->isVisible())
        {
            popupWavetableWindow ->setVisible(true);
            wavetableEditorComponent->setShortcut(0);
        }
        else popupWavetableWindow ->setVisible(false);
    }
    
    else if (button == button_ScanAllFrequencies.get())
    {
        projectManager->setFrequencyScannerParameter(FREQUENCY_SCANNER_MODE, 0);
    }
    else if (button == button_ScanSpecificRange.get())
    {
        projectManager->setFrequencyScannerParameter(FREQUENCY_SCANNER_MODE, 1);
    }
    
    else if (button == button_ExtendedRange.get())
    {
        projectManager->setFrequencyScannerParameter(FREQUENCY_SCANNER_EXTENDED, !button_ExtendedRange->getToggleState());
    }
    
    else if (button == button_Log.get())
    {
        projectManager->setFrequencyScannerParameter(FREQUENCY_SCANNER_LOG_LIN, 0);
    }
    else if (button == button_Lin.get())
    {
        projectManager->setFrequencyScannerParameter(FREQUENCY_SCANNER_LOG_LIN, 1);
    }
    else if (button == button_Load.get())
    {
        projectManager->loadProfileForMode(AUDIO_MODE::MODE_FREQUENCY_SCANNER);
    }
    else if (button == button_Save.get())
    {
        projectManager->saveProfileForMode(AUDIO_MODE::MODE_FREQUENCY_SCANNER);
    }
}

void FrequencyScannerComponent::sliderValueChanged (Slider* slider)
{
    if (slider == slider_Amplitude.get())
    {
        projectManager->setFrequencyScannerParameter(FREQUENCY_SCANNER_ENV_AMPLITUDE, slider_Amplitude->getValue());
    }
    else if (slider == slider_Attack.get())
    {
        projectManager->setFrequencyScannerParameter(FREQUENCY_SCANNER_ENV_ATTACK, slider_Attack->getValue());
    }
    else if (slider == slider_Decay.get())
    {
        projectManager->setFrequencyScannerParameter(FREQUENCY_SCANNER_ENV_DECAY, slider_Decay->getValue());
    }
    else if (slider == slider_Sustain.get())
    {
        projectManager->setFrequencyScannerParameter(FREQUENCY_SCANNER_ENV_SUSTAIN, slider_Sustain->getValue());
    }
    else if (slider == slider_Release.get())
    {
        projectManager->setFrequencyScannerParameter(FREQUENCY_SCANNER_ENV_RELEASE, slider_Release->getValue());
    }
}

void FrequencyScannerComponent::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == comboBoxOutputSelection.get())
    {
        projectManager->setFrequencyScannerParameter( FREQUENCY_SCANNER_OUTPUT_SELECTION, comboBoxOutputSelection->getSelectedId());
    }
}


void FrequencyScannerComponent::updateFrequencyScannerUIParameter(int paramIndex)
{
    // act as toggle between 5 buttons WAVEFORM_TYPE
    int chordScanType =  projectManager->getFrequencyScannerParameter(FREQUENCY_SCANNER_MODE).operator int();
    switch (chordScanType) {
        case 0:
        {
            button_ScanAllFrequencies   ->setToggleState(true, dontSendNotification);
            button_ScanSpecificRange    ->setToggleState(false, dontSendNotification);
            
        }
            break;
        case 1:
        {
            button_ScanAllFrequencies   ->setToggleState(false, dontSendNotification);
            button_ScanSpecificRange    ->setToggleState(true, dontSendNotification);
        }
            break;
    }
    
    
    slider_Amplitude    ->setValue(projectManager->getFrequencyScannerParameter(FREQUENCY_SCANNER_ENV_AMPLITUDE).operator double());
    slider_Attack       ->setValue(projectManager->getFrequencyScannerParameter(FREQUENCY_SCANNER_ENV_ATTACK).operator double());
    slider_Sustain      ->setValue(projectManager->getFrequencyScannerParameter(FREQUENCY_SCANNER_ENV_SUSTAIN).operator double());
    slider_Decay        ->setValue(projectManager->getFrequencyScannerParameter(FREQUENCY_SCANNER_ENV_DECAY).operator double());
    slider_Release      ->setValue(projectManager->getFrequencyScannerParameter(FREQUENCY_SCANNER_ENV_RELEASE).operator double());
    
    // act as toggle between 5 buttons WAVEFORM_TYPE
    int waveformType =  projectManager->getFrequencyScannerParameter(FREQUENCY_SCANNER_WAVEFORM_TYPE).operator int();
    switch (waveformType) {
        case 0:
        {
            button_Default  ->setToggleState(true,  dontSendNotification);
            button_Sine     ->setToggleState(false, dontSendNotification);
            button_Triangle ->setToggleState(false, dontSendNotification);
            button_Square   ->setToggleState(false, dontSendNotification);
            button_Sawtooth ->setToggleState(false, dontSendNotification);
            button_Wavetable->setToggleState(false, dontSendNotification);
        }
            break;
        case 1:
        {
            button_Default  ->setToggleState(false, dontSendNotification);
            button_Sine     ->setToggleState(true,  dontSendNotification);
            button_Triangle ->setToggleState(false, dontSendNotification);
            button_Square   ->setToggleState(false, dontSendNotification);
            button_Sawtooth ->setToggleState(false, dontSendNotification);
            button_Wavetable->setToggleState(false, dontSendNotification);
        }
            break;
        case 2:
        {
            button_Default  ->setToggleState(false, dontSendNotification);
            button_Sine     ->setToggleState(false, dontSendNotification);
            button_Triangle ->setToggleState(true,  dontSendNotification);
            button_Square   ->setToggleState(false, dontSendNotification);
            button_Sawtooth ->setToggleState(false, dontSendNotification);
            button_Wavetable->setToggleState(false, dontSendNotification);
        }
            break;
        case 3:
        {
            button_Default  ->setToggleState(false, dontSendNotification);
            button_Sine     ->setToggleState(false, dontSendNotification);
            button_Triangle ->setToggleState(false, dontSendNotification);
            button_Square   ->setToggleState(true,  dontSendNotification);
            button_Sawtooth ->setToggleState(false, dontSendNotification);
            button_Wavetable->setToggleState(false, dontSendNotification);
        }
            break;
        case 4:
        {
            button_Default  ->setToggleState(false, dontSendNotification);
            button_Sine     ->setToggleState(false, dontSendNotification);
            button_Triangle ->setToggleState(false, dontSendNotification);
            button_Square   ->setToggleState(false, dontSendNotification);
            button_Sawtooth ->setToggleState(true,  dontSendNotification);
            button_Wavetable->setToggleState(false, dontSendNotification);
        }
            break;
        case 5:
        {
            button_Default  ->setToggleState(false, dontSendNotification);
            button_Sine     ->setToggleState(false, dontSendNotification);
            button_Triangle ->setToggleState(false, dontSendNotification);
            button_Square   ->setToggleState(false, dontSendNotification);
            button_Sawtooth ->setToggleState(false, dontSendNotification);
            button_Wavetable->setToggleState(true, dontSendNotification);
        }
            break;
            
        default:
        {
            button_Default  ->setToggleState(false, dontSendNotification);
            button_Sine     ->setToggleState(true,  dontSendNotification);
            button_Triangle ->setToggleState(false, dontSendNotification);
            button_Square   ->setToggleState(false, dontSendNotification);
            button_Sawtooth ->setToggleState(false, dontSendNotification);
            button_Wavetable->setToggleState(false, dontSendNotification);
        }
            
            break;
    }
    
    // Labels
    
    bool exVal = projectManager->getFrequencyScannerParameter(FREQUENCY_SCANNER_EXTENDED).operator bool();
    button_ExtendedRange->setToggleState(exVal, dontSendNotification);
    
    
    // text editors
    
    double freqToVal = projectManager->getFrequencyScannerParameter(FREQUENCY_SCANNER_FREQUENCY_TO).operator double();
    double freqFromVal = projectManager->getFrequencyScannerParameter(FREQUENCY_SCANNER_FREQUENCY_FROM).operator double();
    
    if (exVal) // if extended
    {
        if (freqToVal >= FREQUENCY_MAX) { freqToVal = FREQUENCY_MAX; }
        if (freqToVal <= FREQUENCY_MIN) { freqToVal = FREQUENCY_MIN; }
        
        if (freqFromVal >= FREQUENCY_MAX) { freqFromVal = FREQUENCY_MAX; }
        if (freqFromVal <= FREQUENCY_MIN) { freqFromVal = FREQUENCY_MIN; }
    }
    else
    {
        if (freqToVal >= 20000) { freqToVal = 20000; }
        if (freqToVal <= FREQUENCY_MIN) { freqToVal = FREQUENCY_MIN; }
        
        if (freqFromVal >= 20000) { freqFromVal = 20000; }
        if (freqFromVal <= FREQUENCY_MIN) { freqFromVal = FREQUENCY_MIN; }
    }
    
    
    
    String freqToString(freqToVal, 3, false);
    /*freqToString.append("hz", 2);*/
    textEditorFrequencyTo->setText(freqToString);

    String freqFromString(freqFromVal, 3 , false);
    /*freqFromString.append("hz", 2);*/
    textEditorFrequencyFrom->setText(freqFromString);
    
    int repeatVal = projectManager->getFrequencyScannerParameter(FREQUENCY_SCANNER_NUM_REPEATS).operator int();
    String repeatString(repeatVal);
    textEditorRepeat->setText(repeatString);
    
    int pauseVal = projectManager->getFrequencyScannerParameter(FREQUENCY_SCANNER_NUM_PAUSE).operator int();
    String pauseString(pauseVal);
    textEditorPause->setText(pauseString);
    
    int lengthVal = projectManager->getFrequencyScannerParameter(FREQUENCY_SCANNER_NUM_DURATION).operator int();
    String lengthString(lengthVal);
    textEditorLength->setText(lengthString);
    

    
    int logLin =  projectManager->getFrequencyScannerParameter(FREQUENCY_SCANNER_LOG_LIN).operator int();
    switch (logLin) {
        case 0:
        {
            button_Log   ->setToggleState(true, dontSendNotification);
            button_Lin    ->setToggleState(false, dontSendNotification);
            
        }
            break;
        case 1:
        {
            button_Log   ->setToggleState(false, dontSendNotification);
            button_Lin    ->setToggleState(true, dontSendNotification);
        }
            break;
    }
    
    double linVal = projectManager->getFrequencyScannerParameter(FREQUENCY_SCANNER_LIN_VALUE).operator double();
    String linString(linVal, 3, false);
    linString.append(" Hz", 4);
    textEditor_LinValue->setText(linString);
    
    double logVal = projectManager->getFrequencyScannerParameter(FREQUENCY_SCANNER_LOG_VALUE).operator double();
    String logString(logVal, 3, false);
    logString.append(" oct", 4);
    textEditor_LogValue->setText(logString);
    
    int output = projectManager->getFrequencyScannerParameter( FREQUENCY_SCANNER_OUTPUT_SELECTION).operator int();
    comboBoxOutputSelection ->setSelectedId(output);
    
    
}

void FrequencyScannerComponent::textEditorReturnKeyPressed (TextEditor&editor)
{
    if (&editor == textEditorPause.get())
    {
        int value = editor.getText().getIntValue();
        
        // bounds check
        if (value >= PAUSE_MAX) {  value = PAUSE_MAX; }
        if (value <= PAUSE_MIN) {  value = PAUSE_MIN; }
        
        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setFrequencyScannerParameter(FREQUENCY_SCANNER_NUM_PAUSE, value);
    }
    else if (&editor == textEditorRepeat.get())
    {
        int value = editor.getText().getIntValue();
        
        // bounds check
        if (value >= REPEAT_MAX) {  value = REPEAT_MAX; }
        if (value <= REPEAT_MIN) {  value = REPEAT_MIN; }
        
        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setFrequencyScannerParameter(FREQUENCY_SCANNER_NUM_REPEATS, value);
    }
    
    else if (&editor == textEditorLength.get())
    {
        int value = editor.getText().getIntValue();
        
        // bounds check
        if (value >= LENGTH_MAX) {  value = LENGTH_MAX; }
        if (value <= LENGTH_MIN) {  value = LENGTH_MIN; }
        
        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setFrequencyScannerParameter(FREQUENCY_SCANNER_NUM_DURATION, value);
    }
    else if (&editor == textEditorFrequencyTo.get())
    {
        double value = editor.getText().getDoubleValue();
        
        // bounds check
        if (value >= FREQUENCY_MAX) {  value = FREQUENCY_MAX; }
        if (value <= FREQUENCY_MIN) { value = FREQUENCY_MIN; }
        
//        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setFrequencyScannerParameter(FREQUENCY_SCANNER_FREQUENCY_TO, value);
    }
    else if (&editor == textEditorFrequencyFrom.get())
    {
        double value = editor.getText().getDoubleValue();
        
        // bounds check
        if (value >= FREQUENCY_MAX) {  value = FREQUENCY_MAX; }
        if (value <= FREQUENCY_MIN) {  value = FREQUENCY_MIN; }
        
//        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setFrequencyScannerParameter(FREQUENCY_SCANNER_FREQUENCY_FROM, value);
    }
    
    else if (&editor == textEditor_LogValue.get())
    {
        double value = editor.getText().getDoubleValue();
        
        // bounds check
        if (value >= 1.0) {  value = 1.0; } // max increase 1 octave
        if (value <= 0.001) {  value = 0.001; } // min increase 0.001 octave
        
//        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setFrequencyScannerParameter(FREQUENCY_SCANNER_LOG_VALUE, value);
    }
    
    else if (&editor == textEditor_LinValue.get())
    {
        double value = editor.getText().getDoubleValue();
        
        // bounds check
        if (value >= 1000.0) {  value = 1000.0; } // max increase 1000 hz
        if (value <= 0.001) {  value = 0.001; } // min increase 0.001 Hz
        
//        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setFrequencyScannerParameter(FREQUENCY_SCANNER_LIN_VALUE, value);
    }
    
    Component::unfocusAllComponents();
}

void FrequencyScannerComponent::updateMinMaxSettings(int paramIndex)
{
    switch (paramIndex) {
        case AMPLITUDE_MIN:
        {
            double min = projectManager->getProjectSettingsParameter(AMPLITUDE_MIN);
            double max = projectManager->getProjectSettingsParameter(AMPLITUDE_MAX);
            
            slider_Amplitude->setRange(min, max, 0.1);
        }
            break;
            
        case AMPLITUDE_MAX:
        {
            double min = projectManager->getProjectSettingsParameter(AMPLITUDE_MIN);
            double max = projectManager->getProjectSettingsParameter(AMPLITUDE_MAX);
            
            slider_Amplitude->setRange(min, max, 0.1);
        }
            break;
        case ATTACK_MIN:
        {
            double min = projectManager->getProjectSettingsParameter(ATTACK_MIN);
            double max = projectManager->getProjectSettingsParameter(ATTACK_MAX);
            
            slider_Attack->setRange(min, max, 1);
        }
            break;
        case ATTACK_MAX:
        {
            double min = projectManager->getProjectSettingsParameter(ATTACK_MIN);
            double max = projectManager->getProjectSettingsParameter(ATTACK_MAX);
            
            slider_Attack->setRange(min, max, 1);
        }
            break;
        case DECAY_MIN:
        {
            double min = projectManager->getProjectSettingsParameter(DECAY_MIN);
            double max = projectManager->getProjectSettingsParameter(DECAY_MAX);
            
            slider_Decay->setRange(min, max, 1);
        }
            break;
        case DECAY_MAX:
        {
            double min = projectManager->getProjectSettingsParameter(DECAY_MIN);
            double max = projectManager->getProjectSettingsParameter(DECAY_MAX);
            
            slider_Decay->setRange(min, max, 1);
        }
            break;
        case SUSTAIN_MIN:
        {
            double min = projectManager->getProjectSettingsParameter(SUSTAIN_MIN);
            double max = projectManager->getProjectSettingsParameter(SUSTAIN_MAX);
            
            slider_Sustain->setRange(min, max, 0.1);
        }
            
        case SUSTAIN_MAX:
        {
            double min = projectManager->getProjectSettingsParameter(SUSTAIN_MIN);
            double max = projectManager->getProjectSettingsParameter(SUSTAIN_MAX);
            
            slider_Sustain->setRange(min, max, 0.1);
        }
            break;
        case RELEASE_MIN:
        {
            double min = projectManager->getProjectSettingsParameter(RELEASE_MIN);
            double max = projectManager->getProjectSettingsParameter(RELEASE_MAX);
            
            slider_Release->setRange(min, max, 1);
        }
            break;
        case RELEASE_MAX:
        {
            double min = projectManager->getProjectSettingsParameter(RELEASE_MIN);
            double max = projectManager->getProjectSettingsParameter(RELEASE_MAX);
            
            slider_Release->setRange(min, max, 1);
        }
            break;
            
        default: break;
    }
}
