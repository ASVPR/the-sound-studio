/*
  ==============================================================================

    ChordScannerComponent.cpp
    Created: 13 Mar 2019 9:55:10pm
    Author:  Ziv Elovitch - The Sound Studio Team
    
    ChordScannerComponent provides chord scanning and analysis functionality.
    Uses modern C++ smart pointer management for UI component lifecycle.

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "ChordScannerComponent.h"

//==============================================================================
ChordScannerComponent::ChordScannerComponent(ProjectManager * pm)
{
    projectManager = pm;
    projectManager->addUIListener(this);
    
    setWantsKeyboardFocus(true);
    
    // Font
    Typeface::Ptr AssistantLight        = Typeface::createSystemTypefaceFor(BinaryData::AssistantLight_ttf, BinaryData::AssistantLight_ttfSize);
    Typeface::Ptr AssistantSemiBold     = Typeface::createSystemTypefaceFor(BinaryData::AssistantSemiBold_ttf, BinaryData::AssistantSemiBold_ttfSize);
    Font fontSemiBold(AssistantSemiBold);
    Font fontLight(AssistantLight);
    
    fontSemiBold.setHeight(33);
    fontLight.setHeight(33);
    
    // images

    imageMainBackground                 = ImageCache::getFromMemory(BinaryData::ChordScannerBackground2_png, BinaryData::ChordScannerBackground2_pngSize);

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
    

    
    
    slider_Amplitude    = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_AMPLITUDE);
    slider_Amplitude    ->setRange (0, 1.0, 0);
    slider_Amplitude    ->addListener (this);
    slider_Amplitude    ->setTextValueSuffix("%");
    slider_Amplitude    ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_Amplitude    ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_Amplitude    ->setBounds(knobL, knobY, 130, 158);
    slider_Amplitude    ->setNumDecimalPlacesToDisplay(1);
    slider_Amplitude    ->setLookAndFeel(&lookAndFeel);
    
    addAndMakeVisible(slider_Amplitude.get());
    
    int dif = 248;
    
    slider_Attack       = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_ATTACK);
    slider_Attack       ->setRange (0, 1.0, 0);
    slider_Attack       ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_Attack       ->addListener (this);
    slider_Attack       ->setTextValueSuffix("Ms");
    slider_Attack       ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_Attack       ->setBounds(knobL+(1 * dif), knobY, 130, 158);
    slider_Attack       ->setNumDecimalPlacesToDisplay(0);
    slider_Attack       ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_Attack.get());
    
    slider_Sustain      = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_SUSTAIN);
    slider_Sustain      ->setRange (0, 1.0, 0);
    slider_Sustain      ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_Sustain      ->addListener (this);
    slider_Sustain      ->setTextValueSuffix("%");
    slider_Sustain      ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_Sustain      ->setBounds(knobL+(2 * dif), knobY, 130, 158);
    slider_Sustain      ->setNumDecimalPlacesToDisplay(2);
    slider_Sustain        ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_Sustain.get());
    
    slider_Decay        = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_DECAY);
    slider_Decay        ->setRange (0, 1.0, 0);
    slider_Decay        ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_Decay        ->addListener (this);
    slider_Decay        ->setTextValueSuffix("Ms");
    slider_Decay        ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_Decay        ->setBounds(knobL+(3 * dif), knobY, 130, 158);
    slider_Decay        ->setNumDecimalPlacesToDisplay(0);
    slider_Decay        ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_Decay.get());
    
    slider_Release      = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_RELEASE);
    slider_Release      ->setRange (0, 1.0, 0);
    slider_Release      ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_Release      ->addListener (this);
    slider_Release      ->setTextValueSuffix("Ms");
    slider_Release      ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_Release      ->setBounds(knobL+(4 * dif), knobY, 130, 158);
    slider_Release      ->setNumDecimalPlacesToDisplay(0);
    slider_Release      ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_Release.get());
    
    // FFT

    // Initialize playing status label with proper smart pointer management
    label_Playing = std::make_unique<Label>();
    label_Playing->setText("Playing (00:40)", dontSendNotification);
    label_Playing->setJustificationType(Justification::left);
    fontLight.setHeight(33);
    label_Playing->setBounds(playingLeftMargin, playingTopMargin, 300, 40);
    label_Playing->setFont(fontLight);
    addAndMakeVisible(label_Playing.get());

    
    // Initialize play button with proper smart pointer management
    button_Play = std::make_unique<ImageButton>();
    button_Play->setTriggeredOnMouseDown(true);
    button_Play->setImages (false, true, true,
                            imagePlayButton, 0.999f, Colour (0x00000000),
                            Image(), 1.000f, Colour (0x00000000),
                            imagePlayButton, 0.6, Colour (0x00000000));
    button_Play->addListener(this);
    button_Play->setBounds(playLeftMargin, playTopMargin, playWidth, playHeight);
    addAndMakeVisible(button_Play.get());
    
    // Initialize stop button with proper smart pointer management
    button_Stop = std::make_unique<ImageButton>();
    button_Stop->setTriggeredOnMouseDown(true);
    button_Stop->setImages (false, true, true,
                            imageStopButton, 0.999f, Colour (0x00000000),
                            Image(), 1.000f, Colour (0x00000000),
                            imageStopButton, 0.6, Colour (0x00000000));
    button_Stop->addListener(this);
    button_Stop->setBounds(stopLeftMargin, playTopMargin, playWidth, playHeight);
    addAndMakeVisible(button_Stop.get());
    
    // Initialize panic button with proper smart pointer management
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
    // Initialize waveform selection buttons with proper smart pointer management
    button_Default = std::make_unique<ImageButton>();
    button_Default->setTriggeredOnMouseDown(true);
    button_Default->setImages (false, true, true,
                               imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                               Image(), 1.000f, Colour (0x00000000),
                               imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_Default->addListener(this);
    button_Default->setBounds(183, waveY, 38, 38);
    addAndMakeVisible(button_Default.get());
    
    button_Sine = std::make_unique<ImageButton>();
    button_Sine->setTriggeredOnMouseDown(true);
    button_Sine->setImages (false, true, true,
                            imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                            Image(), 1.000f, Colour (0x00000000),
                            imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_Sine->addListener(this);
    button_Sine->setBounds(330, waveY, 38, 38);
    addAndMakeVisible(button_Sine.get());
    
    button_Triangle = std::make_unique<ImageButton>();
    button_Triangle->setTriggeredOnMouseDown(true);
    button_Triangle->setImages (false, true, true,
                                imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_Triangle->addListener(this);
    button_Triangle->setBounds(639, waveY, 38, 38);
    addAndMakeVisible(button_Triangle.get());
    
    button_Square = std::make_unique<ImageButton>();
    button_Square->setTriggeredOnMouseDown(true);
    button_Square->setImages (false, true, true,
                              imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                              Image(), 1.000f, Colour (0x00000000),
                              imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_Square->addListener(this);
    button_Square->setBounds(924, waveY, 38, 38);
    addAndMakeVisible(button_Square.get());
    
    button_Sawtooth = std::make_unique<ImageButton>();
    button_Sawtooth->setTriggeredOnMouseDown(true);
    button_Sawtooth->setImages (false, true, true,
                                imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_Sawtooth->addListener(this);
    button_Sawtooth->setBounds(1240, waveY, 38, 38);
    addAndMakeVisible(button_Sawtooth.get());
    
    button_Wavetable = std::make_unique<ImageButton>();
    button_Wavetable->setTriggeredOnMouseDown(true);
    button_Wavetable->setImages (false, true, true,
                                                imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                                Image(), 1.000f, Colour (0x00000000),
                                                imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_Wavetable->addListener(this);
    addAndMakeVisible(button_Wavetable.get());

    
    //Chord Scan buttons
    int size    = 50;
    int scanY   = 462;

    
    // Initialize chord scanning buttons with proper smart pointer management
    button_ScanOnlyMainChords = std::make_unique<ImageButton>();
    button_ScanOnlyMainChords->setTriggeredOnMouseDown(true);
    button_ScanOnlyMainChords->setImages (false, true, true,
                                imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_ScanOnlyMainChords->addListener(this);
    button_ScanOnlyMainChords->setBounds(150, scanY, size, size);
    addAndMakeVisible(button_ScanOnlyMainChords.get());

    button_ScanAllChords = std::make_unique<ImageButton>();
    button_ScanAllChords->setTriggeredOnMouseDown(true);
    button_ScanAllChords->setImages (false, true, true,
                                          imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                          Image(), 1.000f, Colour (0x00000000),
                                          imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_ScanAllChords->addListener(this);
    button_ScanAllChords->setBounds(542, scanY, size, size);
    addAndMakeVisible(button_ScanAllChords.get());
    
    button_ScanSpecificRange = std::make_unique<ImageButton>();
    button_ScanSpecificRange->setTriggeredOnMouseDown(true);
    button_ScanSpecificRange->setImages (false, true, true,
                                          imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                          Image(), 1.000f, Colour (0x00000000),
                                          imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_ScanSpecificRange->addListener(this);
    button_ScanSpecificRange->setBounds(880, scanY, size, size);
    addAndMakeVisible(button_ScanSpecificRange.get());
    
    
    button_ScanByFrequency = std::make_unique<ImageButton>();
    button_ScanByFrequency->setTriggeredOnMouseDown(true);
    button_ScanByFrequency->setImages (false, true, true,
                                          imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                          Image(), 1.000f, Colour (0x00000000),
                                          imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_ScanByFrequency->addListener(this);
    button_ScanByFrequency->setBounds(1240, scanY, size, size);
    addAndMakeVisible(button_ScanByFrequency.get());
    
    imageBlueCheckButtonNormal  = ImageCache::getFromMemory(BinaryData::Button_Checkbox_Normal_Max_png, BinaryData::Button_Checkbox_Normal_Max_pngSize);
    imageBlueCheckButtonSelected= ImageCache::getFromMemory(BinaryData::Button_Checkbox_Selected_Max_png, BinaryData::Button_Checkbox_Selected_Max_pngSize);
    
    
    // Initialize range control button
    button_ExtendedRange = std::make_unique<ImageButton>();
    button_ExtendedRange->setTriggeredOnMouseDown(true);
    button_ExtendedRange->setImages (false, true, true,
                                          imageBlueCheckButtonNormal, 0.999f, Colour (0x00000000),
                                          Image(), 1.000f, Colour (0x00000000),
                                          imageBlueCheckButtonSelected, 1.0, Colour (0x00000000));
    button_ExtendedRange->addListener(this);
    addAndMakeVisible(button_ExtendedRange.get());
    
    
    
    // Text Entry for Repeater
    int playY = 1252;
    
    // Initialize text editors for timing controls
    textEditorRepeat = std::make_unique<TextEditor>("");
    textEditorRepeat->setReturnKeyStartsNewLine(false);
    textEditorRepeat->setInputRestrictions(5, "0123456789");
    textEditorRepeat->setMultiLine(false);
    textEditorRepeat->setText("1");
    textEditorRepeat->addListener(this);
    textEditorRepeat->setLookAndFeel(&lookAndFeel);
    textEditorRepeat->setJustification(Justification::centred);
    textEditorRepeat->setFont(fontSemiBold);
    textEditorRepeat->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditorRepeat->applyFontToAllText(fontSemiBold);
    textEditorRepeat->applyColourToAllText(Colours::lightgrey);
    textEditorRepeat->setBounds(500, playY, 111, 35);
    addAndMakeVisible(textEditorRepeat.get());

    
    textEditorPause = std::make_unique<TextEditor>("");
    textEditorPause->setReturnKeyStartsNewLine(false);
    textEditorPause->setInputRestrictions(5, "0123456789.");
    textEditorPause->setMultiLine(false);
    textEditorPause->setText("1000");
    textEditorPause->addListener(this);
    textEditorPause->setLookAndFeel(&lookAndFeel);
    textEditorPause->setJustification(Justification::centred);
    textEditorPause->setFont(fontSemiBold);
    textEditorPause->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditorPause->applyFontToAllText(fontSemiBold);
    textEditorPause->applyColourToAllText(Colours::lightgrey);
    textEditorPause->setBounds(1094, playY, 111, 35);
    addAndMakeVisible(textEditorPause.get());
    
    
    textEditorLength = std::make_unique<TextEditor>("");
    textEditorLength->setReturnKeyStartsNewLine(false);
    textEditorLength->setMultiLine(false);
    textEditorLength->setInputRestrictions(5, "0123456789.");
    textEditorLength->setText("1000");
    textEditorLength->addListener(this);
    textEditorLength->setLookAndFeel(&lookAndFeel);
    textEditorLength->setJustification(Justification::centred);
    textEditorLength->setFont(fontSemiBold);
    textEditorLength->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditorLength->applyFontToAllText(fontSemiBold);
    textEditorLength->applyColourToAllText(Colours::lightgrey);
    textEditorLength->setBounds(794, playY, 111, 35);
    addAndMakeVisible(textEditorLength.get());
    
    
    // visualiser component
    // Initialize visualizer container component
    visualiserContainerComponent = std::make_unique<VisualiserContainerComponent2>(projectManager, AUDIO_MODE::MODE_CHORD_SCANNER);
    visualiserContainerComponent->setBounds(350, 28, 1146, 320);
    addAndMakeVisible(visualiserContainerComponent.get());
    
    // Progress bar
    // Initialize progress bar component
    progressBar = std::make_unique<CustomProgressBar>();
    progressBar->setBounds(292, 372, 1226, 42);
    addAndMakeVisible(progressBar.get());
    
    
    // Load / Save Button
//    button_Load = new TextButton("Load");
//    button_Load->setBounds(1430, 1204, 60, 40);
//    button_Load->addListener(this);
//    addAndMakeVisible(button_Load);
//
//    button_Save = new TextButton("Save");
//    button_Save->setBounds(1360, 1204, 60, 40);
//    button_Save->addListener(this);
//    addAndMakeVisible(button_Save);

    
    // Initialize save/load buttons with proper smart pointer management
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
    
    // combobox playing instrument
    comboBoxPlayingInstrument = std::make_unique<ComboBox>();
    
    // generate
    PopupMenu * comboBoxMenu =  comboBoxPlayingInstrument->getRootMenu();
    
    // Use synthesis-based instrument list instead of file system scanning
    Array<String> synthInstruments = {"Grand Piano", "Electric Guitar", "Cello", "Flute", "Brass", "Harp", "Strings", "Woodwinds"};
    
    // Add synthesis instruments to combo box menu
    for (int i = 0; i < synthInstruments.size(); i++)
    {
        comboBoxMenu->addItem(i+1, synthInstruments[i]);
    }

    comboBoxPlayingInstrument->setLookAndFeel(&lookAndFeel);
    comboBoxPlayingInstrument->getRootMenu()->setLookAndFeel(&lookAndFeel);
    comboBoxPlayingInstrument->addListener(this);
    addAndMakeVisible(comboBoxPlayingInstrument.get());
    
    comboBoxOutputSelection = std::make_unique<ComboBox>();
    comboBoxOutputSelection->setLookAndFeel(&lookAndFeel);
    comboBoxOutputSelection->addListener(this);
    comboBoxOutputSelection->setSelectedId(0);
    projectManager->getOutputsPopupMenu(*comboBoxOutputSelection->getRootMenu());
    addAndMakeVisible(comboBoxOutputSelection.get());

    
    
    // Initialize information display labels with proper memory management
    label_CurrentPlayingChord = std::make_unique<Label>();
    label_CurrentPlayingChord->setText("F#Maj7", dontSendNotification);
    label_CurrentPlayingChord->setBounds(140, 310, 300, 40);
    fontLight.setHeight(33);
    label_CurrentPlayingChord->setJustificationType(Justification::left);
    label_CurrentPlayingChord->setFont(fontLight);
    label_CurrentPlayingChord->setColour(Label::textColourId , Colours::darkgrey);
    addAndMakeVisible(label_CurrentPlayingChord.get());
    
    label_Frequency = std::make_unique<Label>();
    label_Frequency->setText("440hz", dontSendNotification);
    label_Frequency->setBounds(250, 659, 300, 40);
    label_Frequency->setJustificationType(Justification::left);
    fontLight.setHeight(33);
    label_Frequency->setFont(fontLight);
    label_Frequency->setColour(Label::textColourId , Colours::darkgrey);
    addAndMakeVisible(label_Frequency.get());
    
    label_ClosestChord = std::make_unique<Label>();
    label_ClosestChord->setText("A Minor", dontSendNotification);
    label_ClosestChord->setBounds(1030, 659, 300, 40);
    label_ClosestChord->setJustificationType(Justification::left);
    fontLight.setHeight(33);
    label_ClosestChord->setFont(fontLight);
    label_ClosestChord->setColour(Label::textColourId , Colours::darkgrey);
    addAndMakeVisible(label_ClosestChord.get());
    
    
    // Initialize container components for UI organization
    component_KeynoteContainer = std::make_unique<Component>();
    component_KeynoteContainer->setBounds(0, 563, 600, 35);
    addAndMakeVisible(component_KeynoteContainer.get());
    
    //=============================================
    
    component_FrequencyContainer = std::make_unique<Component>();
    component_FrequencyContainer->setBounds(0, 563, 600, 35);
    addAndMakeVisible(component_FrequencyContainer.get());
    
    switchKeynoteFrequencyComponent(true);
    
    //=============================================
    
    int octaveY = 563;
    int keynoteToX = 420;
    int keynoteFromX =  176;
    int comboBoxWidth = 186;
    
    
    // Initialize octave selection combo boxes
    comboBoxOctaveFrom = std::make_unique<ComboBox>();
    comboBoxOctaveFrom->addItemList(ProjectStrings::getOctaveArray(), 1);
    comboBoxOctaveFrom->setSelectedId(1);
    comboBoxOctaveFrom->addListener(this);
    comboBoxOctaveFrom->setBounds(1174-250-40, octaveY, comboBoxWidth, 35);
    comboBoxOctaveFrom->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(comboBoxOctaveFrom.get());
    
    comboBoxOctaveTo = std::make_unique<ComboBox>();
    comboBoxOctaveTo->addItemList(ProjectStrings::getOctaveArray(), 1);
    comboBoxOctaveTo->setSelectedId(1);
    comboBoxOctaveTo->addListener(this);
    comboBoxOctaveTo->setBounds(1174-40, octaveY, comboBoxWidth, 35);
    comboBoxOctaveTo->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(comboBoxOctaveTo.get());
    
    // Initialize keynote selection combo boxes
    comboBoxKeynoteFrom = std::make_unique<ComboBox>();
    comboBoxKeynoteFrom->addItemList(ProjectStrings::getKeynoteArray(), 1);
    comboBoxKeynoteFrom->setSelectedId(1);
    comboBoxKeynoteFrom->addListener(this);
    comboBoxKeynoteFrom->setBounds(keynoteFromX, 0, comboBoxWidth, 35);
    comboBoxKeynoteFrom->setLookAndFeel(&lookAndFeel);
    component_KeynoteContainer->addAndMakeVisible(comboBoxKeynoteFrom.get());
    
    comboBoxKeynoteTo = std::make_unique<ComboBox>();
    comboBoxKeynoteTo->addItemList(ProjectStrings::getKeynoteArray(), 1);
    comboBoxKeynoteTo->setSelectedId(1);
    comboBoxKeynoteTo->addListener(this);
    comboBoxKeynoteTo->setBounds(keynoteToX, 0, comboBoxWidth, 35);
    comboBoxKeynoteTo->setLookAndFeel(&lookAndFeel);
    component_KeynoteContainer->addAndMakeVisible(comboBoxKeynoteTo.get());
    
    // Initialize frequency input text editors
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
    textEditorFrequencyFrom->setBounds(keynoteFromX, 0, comboBoxWidth, 35);
    component_FrequencyContainer->addAndMakeVisible(textEditorFrequencyFrom.get());
    
    textEditorFrequencyTo = std::make_unique<TextEditor>("");
    textEditorFrequencyTo->setReturnKeyStartsNewLine(false);
    textEditorFrequencyTo->setMultiLine(false);
    textEditorFrequencyTo->setInputRestrictions(12, "0123456789.");
    textEditorFrequencyTo->setText("432");
    textEditorFrequencyTo->addListener(this);
    textEditorFrequencyTo->setLookAndFeel(&lookAndFeel);
    textEditorFrequencyTo->setJustification(Justification::centred);
    textEditorFrequencyTo->setFont(fontSemiBold);
    textEditorFrequencyTo->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditorFrequencyTo->applyFontToAllText(fontSemiBold);
    textEditorFrequencyTo->setBounds(keynoteToX, 0, comboBoxWidth, 35);
    component_FrequencyContainer->addAndMakeVisible(textEditorFrequencyTo.get());
    

    //===========================================================
    // end and update params
    
    for (int i = AMPLITUDE_MIN; i <= RELEASE_MAX; i++)
    {
        updateMinMaxSettings(i);
    }
    
    // Initialize wavetable editor button
    button_WavetableEditor = std::make_unique<TextButton>("");
    button_WavetableEditor->setButtonText("WT Editor");
    button_WavetableEditor->setLookAndFeel(&lookAndFeel);
    button_WavetableEditor->addListener(this);
    addAndMakeVisible(button_WavetableEditor.get());

    // Initialize wavetable editor components (raw pointer for compatibility)
    wavetableEditorComponent = new WaveTableOscViewComponent(projectManager, AUDIO_MODE::MODE_CHORD_SCANNER, -1);
    wavetableEditorComponent->setBounds(0, 0, 600, 400);
    
    popupWavetableWindow = new PopupFFTWindow("Wavetable Editor", wavetableEditorComponent, Colours::black, DocumentWindow::allButtons, true);
    popupWavetableWindow ->centreWithSize(600, 400);
    popupWavetableWindow ->setVisible(false);
    popupWavetableWindow ->setResizable(true, true);
    
    
    startTimerHz(TIMER_UPDATE_RATE);
}

ChordScannerComponent::~ChordScannerComponent() { }

void ChordScannerComponent::resized()
{
    int knobY = 916 * scaleFactor;
    int knobL = 374 * scaleFactor;
    
    int dif = 248 * scaleFactor;
    
    int fontSize = 30;
    
    slider_Amplitude    ->setTextBoxStyle (Slider::TextBoxBelow, false, 78 * scaleFactor, 28 * scaleFactor);
    slider_Amplitude    ->setBounds(knobL, knobY, 130 * scaleFactor, 158 * scaleFactor);
    
    slider_Attack       ->setTextBoxStyle (Slider::TextBoxBelow, false, 78 * scaleFactor, 28 * scaleFactor);
    slider_Attack       ->setBounds(knobL+(1 * dif), knobY, 130 * scaleFactor, 158 * scaleFactor);
    
    slider_Sustain      ->setTextBoxStyle (Slider::TextBoxBelow, false, 78 * scaleFactor, 28 * scaleFactor);
    slider_Sustain      ->setBounds(knobL+(2 * dif), knobY, 130 * scaleFactor, 158 * scaleFactor);
    
    slider_Decay        ->setTextBoxStyle (Slider::TextBoxBelow, false, 78 * scaleFactor, 28 * scaleFactor);
    slider_Decay        ->setBounds(knobL+(3 * dif), knobY, 130 * scaleFactor, 158 * scaleFactor);
    
    slider_Release      ->setTextBoxStyle (Slider::TextBoxBelow, false, 78 * scaleFactor, 28 * scaleFactor);
    slider_Release      ->setBounds(knobL+(4 * dif), knobY, 130 * scaleFactor, 158 * scaleFactor);
    
    label_Playing->setBounds(playingLeftMargin * scaleFactor, playingTopMargin * scaleFactor, 300 * scaleFactor, 40 * scaleFactor);
    label_Playing->setFont(fontSize * scaleFactor);
    
    button_Play->setBounds(playLeftMargin * scaleFactor, playTopMargin * scaleFactor, playWidth * scaleFactor, playHeight * scaleFactor);
    button_Stop->setBounds(stopLeftMargin * scaleFactor, playTopMargin * scaleFactor, playWidth * scaleFactor, playHeight * scaleFactor);
    button_Panic->setBounds(panicLeftMargin * scaleFactor, panicTopMargin * scaleFactor, panicWidth * scaleFactor, panicHeight * scaleFactor);
    
    int shift = 90;
    button_WavetableEditor          ->setBounds((1465-shift) * scaleFactor, 760 * scaleFactor, 150 * scaleFactor, 43 * scaleFactor);
    
    int waveY = 842 * scaleFactor;

    button_Default      ->setBounds(147 * scaleFactor, waveY, 38 * scaleFactor, 38 * scaleFactor);
    button_Sine         ->setBounds(330 * scaleFactor, waveY, 38 * scaleFactor, 38 * scaleFactor);
    button_Triangle     ->setBounds(570 * scaleFactor, waveY, 38 * scaleFactor, 38 * scaleFactor);
    button_Square       ->setBounds(818 * scaleFactor, waveY, 38 * scaleFactor, 38 * scaleFactor);
    button_Sawtooth     ->setBounds(1098 * scaleFactor, waveY, 38 * scaleFactor, 38 * scaleFactor);
    button_Wavetable    ->setBounds(1370 * scaleFactor, waveY, 38 * scaleFactor, 38 * scaleFactor);
    
    int size    = 50 * scaleFactor;
    int scanY   = 462 * scaleFactor;
    button_ScanOnlyMainChords->setBounds(150 * scaleFactor, scanY, size, size);
    button_ScanAllChords->setBounds(542 * scaleFactor, scanY, size, size);
    button_ScanSpecificRange->setBounds(880 * scaleFactor, scanY, size, size);
    button_ScanByFrequency->setBounds(1240 * scaleFactor, scanY, size, size);\
    
    
    
    int playY = 1252 * scaleFactor;
    
    float xDif = 200;
    
    textEditorRepeat->setBounds((472 - xDif) * scaleFactor, playY, 120 * scaleFactor, 35 * scaleFactor);
    textEditorRepeat->applyFontToAllText(fontSize * scaleFactor);
    
    textEditorPause->setBounds((1100 - xDif) * scaleFactor, playY, 120 * scaleFactor, 35 * scaleFactor);
    textEditorPause->applyFontToAllText(fontSize * scaleFactor);
    
    textEditorLength->setBounds((794 - xDif) * scaleFactor, playY, 111 * scaleFactor, 35 * scaleFactor);
    textEditorLength->applyFontToAllText(fontSize * scaleFactor);
    
//    setBounds(350, 28, 1146, 320);
    visualiserContainerComponent->setBounds(350 * scaleFactor, 28 * scaleFactor, 1150 * scaleFactor, 324 * scaleFactor);
    
    
    progressBar->setBounds(292 * scaleFactor, 372 * scaleFactor, 1226 * scaleFactor, 42 * scaleFactor);
    
    button_Load->setBounds(1440 * scaleFactor, 1160 * scaleFactor, 100 * scaleFactor, 40 * scaleFactor);
    button_Save->setBounds(1330 * scaleFactor, 1160 * scaleFactor, 100 * scaleFactor, 40 * scaleFactor);
    
    comboBoxPlayingInstrument->setBounds(310 * scaleFactor, 760 * scaleFactor, 300 * scaleFactor, 43 * scaleFactor);
    
    label_CurrentPlayingChord->setBounds(140 * scaleFactor, 310 * scaleFactor, 300 * scaleFactor, 40 * scaleFactor);
    label_CurrentPlayingChord->setFont(fontSize * scaleFactor);
    
    label_Frequency->setBounds(250 * scaleFactor, 659 * scaleFactor, 300 * scaleFactor, 40 * scaleFactor);
    label_Frequency->setFont(fontSize * scaleFactor);
    
    label_ClosestChord->setBounds(1030 * scaleFactor, 659 * scaleFactor, 300 * scaleFactor, 40 * scaleFactor);
    label_ClosestChord->setFont(fontSize * scaleFactor);
    
    component_KeynoteContainer->setBounds(0, 563 * scaleFactor, 600 * scaleFactor, 35 * scaleFactor);
    component_FrequencyContainer->setBounds(0, 563 * scaleFactor, 600 * scaleFactor, 35 * scaleFactor);
    
    int octaveY = 563 * scaleFactor;
    int keynoteToX = 420 * scaleFactor;
    int keynoteFromX =  176 * scaleFactor;
    int comboBoxWidth = 186 * scaleFactor;
    
    comboBoxOctaveFrom->setBounds((1174-250-40) * scaleFactor, octaveY, comboBoxWidth, 35 * scaleFactor);
    comboBoxOctaveTo->setBounds((1174-40) * scaleFactor, octaveY, comboBoxWidth, 35 * scaleFactor);
    comboBoxKeynoteFrom->setBounds(keynoteFromX, 0, comboBoxWidth, 35 * scaleFactor);
    comboBoxKeynoteTo->setBounds(keynoteToX, 0, comboBoxWidth, 35 * scaleFactor);
    
    button_ExtendedRange->setBounds(1330 * scaleFactor, octaveY + (5 * scaleFactor), 30 * scaleFactor, 30 * scaleFactor);
    
    textEditorFrequencyFrom->setBounds(keynoteFromX, 0, comboBoxWidth, 35 * scaleFactor);
    textEditorFrequencyFrom->applyFontToAllText(fontSize * scaleFactor);
    
    textEditorFrequencyTo->setBounds(keynoteToX, 0, comboBoxWidth, 35 * scaleFactor);
    textEditorFrequencyTo->applyFontToAllText(fontSize * scaleFactor);
    
    wavetableEditorComponent->setShortcut(0);
    
    comboBoxOutputSelection         -> setBounds(1150 * scaleFactor, 1248 * scaleFactor, 250 * scaleFactor, 43 * scaleFactor);
    
    
}

void ChordScannerComponent::timerCallback()
{
    if (projectManager->chordScannerProcessor->getPlaystate() == PLAY_STATE::PLAYING)
    {
        MessageManagerLock lock;
        
        float val = projectManager->chordScannerProcessor->repeater->getProgressBarValue();
        
        progressBar->setValue(val);
        
        String text = projectManager->chordScannerProcessor->repeater->getTimeRemainingInSecondsString();
        
        label_Playing->setText(text, dontSendNotification);
        
        // Current Chord label
        int keynote = projectManager->chordScannerProcessor->repeater->keynoteIterator - 1;
        String keynoteS;
        if (keynote < 0 || keynote > 11) { /* do nothing */ }
        else
        {
            keynoteS = ProjectStrings::getKeynoteArray().getReference(keynote);
        }
        
         String keynoteString(keynoteS);
        
        int chordtype = projectManager->chordScannerProcessor->repeater->currentPlayingChord - 1;
        
        String chordtypeString(ProjectStrings::getChordTypeArray().getReference(chordtype));
        
        int octave = projectManager->chordScannerProcessor->repeater->octaveIterator - 1;
        String octaveString(octave);
        
        String currentChordString(keynoteString);
        currentChordString.append(octaveString, 2);
        currentChordString.append(" ", 2);
        currentChordString.append(chordtypeString, 10);
        
        label_CurrentPlayingChord->setText(currentChordString, dontSendNotification);
    }
    
    // visualiser
//    visualiserContainerComponent->pushUpdate();
}

void ChordScannerComponent::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background
    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
    
    g.setOpacity(1.0);
    g.drawImage(imageMainBackground, 0, 0, 1562 * scaleFactor, 1440 * scaleFactor, 0, 0, 1562, 1440);
}


void ChordScannerComponent::buttonClicked (Button*button)
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
        projectManager->setChordScannerParameter(CHORD_SCANNER_WAVEFORM_TYPE, 0);
    }
    else if (button == button_Sine.get())
    {
        projectManager->setChordScannerParameter(CHORD_SCANNER_WAVEFORM_TYPE, 1);
    }
    else if (button == button_Triangle.get())
    {
        projectManager->setChordScannerParameter(CHORD_SCANNER_WAVEFORM_TYPE, 2);
    }
    else if (button == button_Square.get())
    {
        projectManager->setChordScannerParameter(CHORD_SCANNER_WAVEFORM_TYPE, 3);
    }
    else if (button == button_Sawtooth.get())
    {
        projectManager->setChordScannerParameter(CHORD_SCANNER_WAVEFORM_TYPE, 4);
    }
    else if (button == button_Wavetable.get())
    {
        projectManager->setChordScannerParameter(CHORD_SCANNER_WAVEFORM_TYPE, 5);
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
    
    else if (button == button_ScanOnlyMainChords.get())
    {
        projectManager->setChordScannerParameter(CHORD_SCANNER_MODE, 0);
        
        switchKeynoteFrequencyComponent(true);
    }
    else if (button == button_ScanAllChords.get())
    {
        projectManager->setChordScannerParameter(CHORD_SCANNER_MODE, 1);
        
        switchKeynoteFrequencyComponent(true);
    }
    else if (button == button_ScanSpecificRange.get())
    {
        projectManager->setChordScannerParameter(CHORD_SCANNER_MODE, 2);
        
        switchKeynoteFrequencyComponent(true);
    }
    else if (button == button_ScanByFrequency.get())
    {
        projectManager->setChordScannerParameter(CHORD_SCANNER_MODE, 3);
        
        switchKeynoteFrequencyComponent(false);
    }
    else if (button == button_ExtendedRange.get())
    {
        projectManager->setChordScannerParameter(CHORD_SCANNER_OCTAVE_EXTENDED, !button_ExtendedRange->getToggleState());
    }
    else if (button == button_Load.get())
    {
        projectManager->loadProfileForMode(AUDIO_MODE::MODE_CHORD_SCANNER);
    }
    else if (button == button_Save.get())
    {
        projectManager->saveProfileForMode(AUDIO_MODE::MODE_CHORD_SCANNER);
    }
}

void ChordScannerComponent::sliderValueChanged (Slider* slider)
{
    if (slider == slider_Amplitude.get())
    {
        projectManager->setChordScannerParameter(CHORD_SCANNER_ENV_AMPLITUDE, slider_Amplitude->getValue());
    }
    else if (slider == slider_Attack.get())
    {
        projectManager->setChordScannerParameter(CHORD_SCANNER_ENV_ATTACK, slider_Attack->getValue());
    }
    else if (slider == slider_Decay.get())
    {
        projectManager->setChordScannerParameter(CHORD_SCANNER_ENV_DECAY, slider_Decay->getValue());
    }
    else if (slider == slider_Sustain.get())
    {
        projectManager->setChordScannerParameter(CHORD_SCANNER_ENV_SUSTAIN, slider_Sustain->getValue());
    }
    else if (slider == slider_Release.get())
    {
        projectManager->setChordScannerParameter(CHORD_SCANNER_ENV_RELEASE, slider_Release->getValue());
    }
}

void ChordScannerComponent::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == comboBoxPlayingInstrument.get())
    {
        int selectedID = comboBoxPlayingInstrument->getSelectedId();
        projectManager->setChordScannerParameter(CHORD_SCANNER_INSTRUMENT_TYPE, selectedID);
    }
    else if (comboBoxThatHasChanged == comboBoxKeynoteTo.get())
    {
        int selectedID = comboBoxKeynoteTo->getSelectedId();
        projectManager->setChordScannerParameter(CHORD_SCANNER_KEYNOTE_TO, selectedID);
    }
    else if (comboBoxThatHasChanged == comboBoxKeynoteFrom.get())
    {
        int selectedID = comboBoxKeynoteFrom->getSelectedId();
        projectManager->setChordScannerParameter(CHORD_SCANNER_KEYNOTE_FROM, selectedID);
    }
    else if (comboBoxThatHasChanged == comboBoxOctaveTo.get())
    {
        int selectedID = comboBoxOctaveTo->getSelectedId() - 1;
        projectManager->setChordScannerParameter(CHORD_SCANNER_OCTAVE_TO, selectedID);
    }
    else if (comboBoxThatHasChanged == comboBoxOctaveFrom.get())
    {
        int selectedID = comboBoxOctaveFrom->getSelectedId() - 1;
        projectManager->setChordScannerParameter(CHORD_SCANNER_OCTAVE_FROM, selectedID);
    }
    else if (comboBoxThatHasChanged == comboBoxOutputSelection.get())
    {
        projectManager->setChordScannerParameter( CHORD_SCANNER_OUTPUT_SELECTION, comboBoxOutputSelection->getSelectedId());
    }
}


void ChordScannerComponent::updateChordScannerUIParameter(int paramIndex)
{
    if (paramIndex == CHORD_SCANNER_MODE)
    {
        // act as toggle between 5 buttons WAVEFORM_TYPE
        int chordScanType =  projectManager->getChordScannerParameter(CHORD_SCANNER_MODE).operator int();
        switch (chordScanType) {
            case 0:
            {
                button_ScanOnlyMainChords   ->setToggleState(true,  dontSendNotification);
                button_ScanAllChords        ->setToggleState(false, dontSendNotification);
                button_ScanSpecificRange    ->setToggleState(false, dontSendNotification);
                button_ScanByFrequency      ->setToggleState(false, dontSendNotification);
                
                switchKeynoteFrequencyComponent(true);
        
            }
                break;
            case 1:
            {
                button_ScanOnlyMainChords   ->setToggleState(false, dontSendNotification);
                button_ScanAllChords        ->setToggleState(true,  dontSendNotification);
                button_ScanSpecificRange    ->setToggleState(false, dontSendNotification);
                button_ScanByFrequency      ->setToggleState(false, dontSendNotification);
                
                switchKeynoteFrequencyComponent(true);
            }
                break;
            case 2:
            {
                button_ScanOnlyMainChords   ->setToggleState(false,  dontSendNotification);
                button_ScanAllChords        ->setToggleState(false,  dontSendNotification);
                button_ScanSpecificRange    ->setToggleState(true,   dontSendNotification);
                button_ScanByFrequency      ->setToggleState(false,  dontSendNotification);
                
                switchKeynoteFrequencyComponent(true);
            }
                break;
            case 3:
            {
                button_ScanOnlyMainChords   ->setToggleState(false,  dontSendNotification);
                button_ScanAllChords        ->setToggleState(false,  dontSendNotification);
                button_ScanSpecificRange    ->setToggleState(false,  dontSendNotification);
                button_ScanByFrequency      ->setToggleState(true,   dontSendNotification);
                
                switchKeynoteFrequencyComponent(false);
            }
                break;
                
            default:
            {
                button_ScanOnlyMainChords   ->setToggleState(true,  dontSendNotification);
                button_ScanAllChords        ->setToggleState(false, dontSendNotification);
                button_ScanSpecificRange    ->setToggleState(false, dontSendNotification);
                button_ScanByFrequency      ->setToggleState(false, dontSendNotification);
                
                switchKeynoteFrequencyComponent(true);
            }
                
                break;
        }
    }
    else if (paramIndex == CHORD_SCANNER_WAVEFORM_TYPE)
    {
        // act as toggle between 5 buttons WAVEFORM_TYPE
        int waveformType =  projectManager->getChordScannerParameter(CHORD_SCANNER_WAVEFORM_TYPE).operator int();
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
    }   // comboboxes
    else if (paramIndex == CHORD_SCANNER_INSTRUMENT_TYPE)
    {
        int instrumentType = projectManager->getChordScannerParameter(CHORD_SCANNER_INSTRUMENT_TYPE).operator int();
        comboBoxPlayingInstrument->setSelectedId(instrumentType, dontSendNotification);
    }
    else if (paramIndex == CHORD_SCANNER_KEYNOTE_TO)
    {
        int keynoteTo = projectManager->getChordScannerParameter(CHORD_SCANNER_KEYNOTE_TO).operator int();
        comboBoxKeynoteTo->setSelectedId(keynoteTo, dontSendNotification);
    }
    else if (paramIndex == CHORD_SCANNER_KEYNOTE_FROM)
    {
        int keynoteFrom = projectManager->getChordScannerParameter(CHORD_SCANNER_KEYNOTE_FROM).operator int();
        comboBoxKeynoteFrom->setSelectedId(keynoteFrom, dontSendNotification);
    }
    else if (paramIndex == CHORD_SCANNER_OCTAVE_TO)
    {
        int octaveTo = projectManager->getChordScannerParameter(CHORD_SCANNER_OCTAVE_TO).operator int() + 1;
        comboBoxOctaveTo->setSelectedId(octaveTo, dontSendNotification);
    }
    else if (paramIndex == CHORD_SCANNER_OCTAVE_FROM)
    {
        int octaveFrom = projectManager->getChordScannerParameter(CHORD_SCANNER_OCTAVE_FROM).operator int() + 1;
        comboBoxOctaveFrom->setSelectedId(octaveFrom, dontSendNotification);
    }
    else if (paramIndex == CHORD_SCANNER_OCTAVE_EXTENDED)
    {
        bool ex = projectManager->getChordScannerParameter( CHORD_SCANNER_OCTAVE_EXTENDED).operator bool();
        button_ExtendedRange->setToggleState(ex, dontSendNotification);
        
        if (ex)
        {
            // set comboBox to limit at 16 octaves
            comboBoxOctaveTo->clear();
            comboBoxOctaveTo->addItemList(ProjectStrings::getOctaveArrayExtended(true), 1);
            
            comboBoxOctaveFrom->clear();
            comboBoxOctaveFrom->addItemList(ProjectStrings::getOctaveArrayExtended(true), 1);
            
            int octaveFrom = projectManager->getChordScannerParameter(CHORD_SCANNER_OCTAVE_FROM).operator int() + 1;
            comboBoxOctaveFrom->setSelectedId(octaveFrom, dontSendNotification);
            
            int octaveTo = projectManager->getChordScannerParameter(CHORD_SCANNER_OCTAVE_TO).operator int() + 1;
            comboBoxOctaveTo->setSelectedId(octaveTo, dontSendNotification);
            
            
        }
        else
        {
            // limit to 10
            comboBoxOctaveTo->clear();
            comboBoxOctaveTo->addItemList(ProjectStrings::getOctaveArrayExtended(false), 1);
            
            comboBoxOctaveFrom->clear();
            comboBoxOctaveFrom->addItemList(ProjectStrings::getOctaveArrayExtended(false), 1);
            
            int octaveFrom = projectManager->getChordScannerParameter(CHORD_SCANNER_OCTAVE_FROM).operator int() + 1;
            comboBoxOctaveFrom->setSelectedId(octaveFrom, dontSendNotification);
            
            int octaveTo = projectManager->getChordScannerParameter(CHORD_SCANNER_OCTAVE_TO).operator int() + 1;
            comboBoxOctaveTo->setSelectedId(octaveTo, dontSendNotification);
        }
    }
    else if (paramIndex == CHORD_SCANNER_NUM_REPEATS)
    {
        // text editors
        int repeatVal = projectManager->getChordScannerParameter(CHORD_SCANNER_NUM_REPEATS).operator int();
        String repeatString(repeatVal);
        textEditorRepeat->setText(repeatString);
    }
    else if (paramIndex == CHORD_SCANNER_NUM_PAUSE)
    {
        // text editors
        int pauseVal = projectManager->getChordScannerParameter(CHORD_SCANNER_NUM_PAUSE).operator int();
        String pauseString(pauseVal);
        pauseString.append("ms", 2);
        textEditorPause->setText(pauseString);
    }
    else if (paramIndex == CHORD_SCANNER_NUM_DURATION)
    {
        int lengthVal = projectManager->getChordScannerParameter(CHORD_SCANNER_NUM_DURATION).operator int();
        String lengthString(lengthVal);
        lengthString.append("ms", 2);
        textEditorLength->setText(lengthString);
    }
    else if (paramIndex == CHORD_SCANNER_FREQUENCY_TO)
    {
        double freqToVal = projectManager->getChordScannerParameter(CHORD_SCANNER_FREQUENCY_TO).operator double();
        String freqToString(freqToVal, 3, false);
        /*freqToString.append("hz", 2);*/
        textEditorFrequencyTo->setText(freqToString);
    }
    else if (paramIndex == CHORD_SCANNER_FREQUENCY_FROM)
    {
        double freqFromVal = projectManager->getChordScannerParameter(CHORD_SCANNER_FREQUENCY_FROM).operator double();
        String freqFromString(freqFromVal, 3, false);
        /*freqFromString.append("hz", 2);*/
        textEditorFrequencyFrom->setText(freqFromString);
    }
    else if (paramIndex == CHORD_SCANNER_ENV_AMPLITUDE)
    {
        slider_Amplitude    ->setValue(projectManager->getChordScannerParameter(CHORD_SCANNER_ENV_AMPLITUDE).operator double());
    }
    else if (paramIndex == CHORD_SCANNER_ENV_ATTACK)
    {
        slider_Attack       ->setValue(projectManager->getChordScannerParameter(CHORD_SCANNER_ENV_ATTACK).operator double());
    }
    else if (paramIndex == CHORD_SCANNER_ENV_SUSTAIN)
    {
        slider_Sustain      ->setValue(projectManager->getChordScannerParameter(CHORD_SCANNER_ENV_SUSTAIN).operator double());
    }
    else if (paramIndex == CHORD_SCANNER_ENV_DECAY)
    {
        slider_Decay        ->setValue(projectManager->getChordScannerParameter(CHORD_SCANNER_ENV_DECAY).operator double());
    }
    else if (paramIndex == CHORD_SCANNER_ENV_RELEASE)
    {
        slider_Release      ->setValue(projectManager->getChordScannerParameter(CHORD_SCANNER_ENV_RELEASE).operator double());
    }
    else if (paramIndex == CHORD_SCANNER_OUTPUT_SELECTION)
    {
        int output = projectManager->getChordScannerParameter( CHORD_SCANNER_OUTPUT_SELECTION).operator int();
        comboBoxOutputSelection ->setSelectedId(output);
    }
    
    
    

}

void ChordScannerComponent::textEditorReturnKeyPressed (TextEditor&editor)
{
    if (&editor == textEditorPause.get())
    {
        int value = editor.getText().getIntValue();
        
        // bounds check
        if (value >= PAUSE_MAX) {  value = PAUSE_MAX; }
        if (value <= PAUSE_MIN) {  value = PAUSE_MIN; }
        
        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setChordScannerParameter(CHORD_SCANNER_NUM_PAUSE, value);
    }
    else if (&editor == textEditorRepeat.get())
    {
        int value = editor.getText().getIntValue();
        
        // bounds check
        if (value >= REPEAT_MAX) {  value = REPEAT_MAX; }
        if (value <= REPEAT_MIN) {  value = REPEAT_MIN; }
        
        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setChordScannerParameter(CHORD_SCANNER_NUM_REPEATS, value);
    }
    else if (&editor == textEditorLength.get())
    {
        int value = editor.getText().getIntValue();
        
        // bounds check
        if (value >= LENGTH_MAX) {  value = LENGTH_MAX; }
        if (value <= LENGTH_MIN) {  value = LENGTH_MIN; }
        
        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setChordScannerParameter(CHORD_SCANNER_NUM_DURATION, value);
    }
    else if (&editor == textEditorFrequencyTo.get())
    {
        double value = editor.getText().getDoubleValue();
        
        // bounds check
        if (value >= FREQUENCY_MAX) {  value = FREQUENCY_MAX; }
        if (value <= FREQUENCY_MIN) { value = FREQUENCY_MIN; }
        
        String newVal(value, 3, false); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setChordScannerParameter(CHORD_SCANNER_FREQUENCY_TO, value);
    }
    else if (&editor == textEditorFrequencyFrom.get())
    {
        double value = editor.getText().getDoubleValue();
        
        // bounds check
        if (value >= FREQUENCY_MAX) {  value = FREQUENCY_MAX; }
        if (value <= FREQUENCY_MIN) {  value = FREQUENCY_MIN; }
        
        String newVal(value, 3, false); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setChordScannerParameter(CHORD_SCANNER_FREQUENCY_FROM, value);
    }
    
    Component::unfocusAllComponents();
}

void ChordScannerComponent::updateMinMaxSettings(int paramIndex) 
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
