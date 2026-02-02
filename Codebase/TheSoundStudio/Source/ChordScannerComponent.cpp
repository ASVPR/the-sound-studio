/*
  ==============================================================================

    ChordScannerComponent.cpp

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

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
    imageBlueButtonNormal               = ImageCache::getFromMemory(BinaryData::BlueButton_Normal_png, BinaryData::BlueButton_Normal_pngSize);
    imageBlueButtonSelected             = ImageCache::getFromMemory(BinaryData::BlueButton_Selected_png, BinaryData::BlueButton_Selected_pngSize);
    imageBlueCheckButtonNormal          = ImageCache::getFromMemory(BinaryData::Button_Checkbox_Normal_Max_png, BinaryData::Button_Checkbox_Normal_Max_pngSize);
    imageBlueCheckButtonSelected        = ImageCache::getFromMemory(BinaryData::Button_Checkbox_Selected_Max_png, BinaryData::Button_Checkbox_Selected_Max_pngSize);


    // Knobs (positions set in resized())
    slider_Amplitude    = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_AMPLITUDE);
    slider_Amplitude    ->setRange (0, 1.0, 0);
    slider_Amplitude    ->addListener (this);
    slider_Amplitude    ->setTextValueSuffix("%");
    slider_Amplitude    ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_Amplitude    ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_Amplitude    ->setNumDecimalPlacesToDisplay(1);
    slider_Amplitude    ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_Amplitude.get());

    slider_Attack       = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_ATTACK);
    slider_Attack       ->setRange (0, 1.0, 0);
    slider_Attack       ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_Attack       ->addListener (this);
    slider_Attack       ->setTextValueSuffix("Ms");
    slider_Attack       ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_Attack       ->setNumDecimalPlacesToDisplay(0);
    slider_Attack       ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_Attack.get());

    slider_Sustain      = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_SUSTAIN);
    slider_Sustain      ->setRange (0, 1.0, 0);
    slider_Sustain      ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_Sustain      ->addListener (this);
    slider_Sustain      ->setTextValueSuffix("%");
    slider_Sustain      ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_Sustain      ->setNumDecimalPlacesToDisplay(2);
    slider_Sustain      ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_Sustain.get());

    slider_Decay        = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_DECAY);
    slider_Decay        ->setRange (0, 1.0, 0);
    slider_Decay        ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_Decay        ->addListener (this);
    slider_Decay        ->setTextValueSuffix("Ms");
    slider_Decay        ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_Decay        ->setNumDecimalPlacesToDisplay(0);
    slider_Decay        ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_Decay.get());

    slider_Release      = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_RELEASE);
    slider_Release      ->setRange (0, 1.0, 0);
    slider_Release      ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_Release      ->addListener (this);
    slider_Release      ->setTextValueSuffix("Ms");
    slider_Release      ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_Release      ->setNumDecimalPlacesToDisplay(0);
    slider_Release      ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_Release.get());
    
    // FFT

    // Transport toolbar with Play/Stop/Panic/Record/Progress/Save/Load
    TransportToolbarComponent::ButtonVisibility toolbarVis;
    toolbarVis.play = true;
    toolbarVis.stop = true;
    toolbarVis.record = false;
    toolbarVis.panic = true;
    toolbarVis.progress = true;
    toolbarVis.loop = false;
    toolbarVis.simultaneous = false;
    toolbarVis.save = true;
    toolbarVis.load = true;
    toolbarVis.playingLabel = true;
    transportToolbar = std::make_unique<TransportToolbarComponent>(toolbarVis);
    transportToolbar->addTransportListener(this);
    addAndMakeVisible(transportToolbar.get());


    // Waveform buttons (positions set in resized())
    button_Default = std::make_unique<ImageButton>();
    button_Default->setTriggeredOnMouseDown(true);
    button_Default->setImages (false, true, true,
                               imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                               Image(), 1.000f, Colour (0x00000000),
                               imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_Default->addListener(this);
    addAndMakeVisible(button_Default.get());

    button_Sine = std::make_unique<ImageButton>();
    button_Sine->setTriggeredOnMouseDown(true);
    button_Sine->setImages (false, true, true,
                            imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                            Image(), 1.000f, Colour (0x00000000),
                            imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_Sine->addListener(this);
    addAndMakeVisible(button_Sine.get());

    button_Triangle = std::make_unique<ImageButton>();
    button_Triangle->setTriggeredOnMouseDown(true);
    button_Triangle->setImages (false, true, true,
                                imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_Triangle->addListener(this);
    addAndMakeVisible(button_Triangle.get());

    button_Square = std::make_unique<ImageButton>();
    button_Square->setTriggeredOnMouseDown(true);
    button_Square->setImages (false, true, true,
                              imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                              Image(), 1.000f, Colour (0x00000000),
                              imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_Square->addListener(this);
    addAndMakeVisible(button_Square.get());

    button_Sawtooth = std::make_unique<ImageButton>();
    button_Sawtooth->setTriggeredOnMouseDown(true);
    button_Sawtooth->setImages (false, true, true,
                                imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_Sawtooth->addListener(this);
    addAndMakeVisible(button_Sawtooth.get());

    button_Wavetable = std::make_unique<ImageButton>();
    button_Wavetable->setTriggeredOnMouseDown(true);
    button_Wavetable->setImages (false, true, true,
                                imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_Wavetable->addListener(this);
    addAndMakeVisible(button_Wavetable.get());

    
    // Chord Scan buttons (positions set in resized())
    button_ScanOnlyMainChords = std::make_unique<ImageButton>();
    button_ScanOnlyMainChords->setTriggeredOnMouseDown(true);
    button_ScanOnlyMainChords->setImages (false, true, true,
                                imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_ScanOnlyMainChords->addListener(this);
    addAndMakeVisible(button_ScanOnlyMainChords.get());

    button_ScanAllChords = std::make_unique<ImageButton>();
    button_ScanAllChords->setTriggeredOnMouseDown(true);
    button_ScanAllChords->setImages (false, true, true,
                                          imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                          Image(), 1.000f, Colour (0x00000000),
                                          imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_ScanAllChords->addListener(this);
    addAndMakeVisible(button_ScanAllChords.get());

    button_ScanSpecificRange = std::make_unique<ImageButton>();
    button_ScanSpecificRange->setTriggeredOnMouseDown(true);
    button_ScanSpecificRange->setImages (false, true, true,
                                          imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                          Image(), 1.000f, Colour (0x00000000),
                                          imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_ScanSpecificRange->addListener(this);
    addAndMakeVisible(button_ScanSpecificRange.get());

    button_ScanByFrequency = std::make_unique<ImageButton>();
    button_ScanByFrequency->setTriggeredOnMouseDown(true);
    button_ScanByFrequency->setImages (false, true, true,
                                          imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                          Image(), 1.000f, Colour (0x00000000),
                                          imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_ScanByFrequency->addListener(this);
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
    
    
    
    // Text Entry for Repeater (positions set in resized())
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
    addAndMakeVisible(textEditorLength.get());

    // Visualiser component (position set in resized())
    visualiserContainerComponent = std::make_unique<VisualiserContainerComponent2>(projectManager, AUDIO_MODE::MODE_CHORD_SCANNER);
    addAndMakeVisible(visualiserContainerComponent.get());
    
    // combobox playing instrument
    comboBoxPlayingInstrument = std::make_unique<ComboBox>();
    
    // generate
    PopupMenu * comboBoxMenu =  comboBoxPlayingInstrument->getRootMenu();
    
    // Only include instruments that are implemented; IDs match INSTRUMENTS enum
    comboBoxMenu->addItem(1, "Grand Piano");     // PIANO
    comboBoxMenu->addItem(4, "Acoustic Guitar"); // GUITAR
    comboBoxMenu->addItem(5, "Harp");            // HARP
    comboBoxMenu->addItem(7, "Strings");         // STRINGS
    comboBoxMenu->addItem(3, "Flute");           // FLUTE

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

    
    
    // Information display labels (positions set in resized())
    label_CurrentPlayingChord = std::make_unique<Label>();
    label_CurrentPlayingChord->setText("F#Maj7", dontSendNotification);
    fontLight.setHeight(33);
    label_CurrentPlayingChord->setJustificationType(Justification::left);
    label_CurrentPlayingChord->setFont(fontLight);
    label_CurrentPlayingChord->setColour(Label::textColourId , Colours::darkgrey);
    addAndMakeVisible(label_CurrentPlayingChord.get());

    label_Frequency = std::make_unique<Label>();
    label_Frequency->setText("440hz", dontSendNotification);
    label_Frequency->setJustificationType(Justification::left);
    fontLight.setHeight(33);
    label_Frequency->setFont(fontLight);
    label_Frequency->setColour(Label::textColourId , Colours::darkgrey);
    addAndMakeVisible(label_Frequency.get());

    label_ClosestChord = std::make_unique<Label>();
    label_ClosestChord->setText("A Minor", dontSendNotification);
    label_ClosestChord->setJustificationType(Justification::left);
    fontLight.setHeight(33);
    label_ClosestChord->setFont(fontLight);
    label_ClosestChord->setColour(Label::textColourId , Colours::darkgrey);
    addAndMakeVisible(label_ClosestChord.get());

    // Container components for UI organization (positions set in resized())
    component_KeynoteContainer = std::make_unique<Component>();
    addAndMakeVisible(component_KeynoteContainer.get());

    component_FrequencyContainer = std::make_unique<Component>();
    addAndMakeVisible(component_FrequencyContainer.get());
    
    switchKeynoteFrequencyComponent(true);
    
    //=============================================
    
    // Octave selection combo boxes (positions set in resized())
    comboBoxOctaveFrom = std::make_unique<ComboBox>();
    comboBoxOctaveFrom->addItemList(ProjectStrings::getOctaveArray(), 1);
    comboBoxOctaveFrom->setSelectedId(1);
    comboBoxOctaveFrom->addListener(this);
    comboBoxOctaveFrom->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(comboBoxOctaveFrom.get());

    comboBoxOctaveTo = std::make_unique<ComboBox>();
    comboBoxOctaveTo->addItemList(ProjectStrings::getOctaveArray(), 1);
    comboBoxOctaveTo->setSelectedId(1);
    comboBoxOctaveTo->addListener(this);
    comboBoxOctaveTo->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(comboBoxOctaveTo.get());

    // Keynote selection combo boxes
    comboBoxKeynoteFrom = std::make_unique<ComboBox>();
    comboBoxKeynoteFrom->addItemList(ProjectStrings::getKeynoteArray(), 1);
    comboBoxKeynoteFrom->setSelectedId(1);
    comboBoxKeynoteFrom->addListener(this);
    comboBoxKeynoteFrom->setLookAndFeel(&lookAndFeel);
    component_KeynoteContainer->addAndMakeVisible(comboBoxKeynoteFrom.get());

    comboBoxKeynoteTo = std::make_unique<ComboBox>();
    comboBoxKeynoteTo->addItemList(ProjectStrings::getKeynoteArray(), 1);
    comboBoxKeynoteTo->setSelectedId(1);
    comboBoxKeynoteTo->addListener(this);
    comboBoxKeynoteTo->setLookAndFeel(&lookAndFeel);
    component_KeynoteContainer->addAndMakeVisible(comboBoxKeynoteTo.get());

    // Frequency input text editors
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
    using Layout = TSS::Design::Layout;
    auto bounds = getLocalBounds();
    const int w = bounds.getWidth();
    const int h = bounds.getHeight();
    const float s = w / Layout::kRefContentWidth; // proportional scale

    // Toolbar at bottom
    int toolbarH = (int)(h * Layout::kToolbarHeightRatio);
    transportToolbar->setBounds(bounds.removeFromBottom(toolbarH));

    // Visualiser at top-right (~23% from left, ~2% from top, ~73% width, ~22% height)
    int visX = (int)(w * 0.224f);
    int visY = (int)(h * 0.019f);
    int visW = (int)(w * 0.735f);
    int visH = (int)(h * 0.225f);
    visualiserContainerComponent->setBounds(visX, visY, visW, visH);

    // Current playing chord label
    int fontSize = (int)(30 * s);
    label_CurrentPlayingChord->setBounds((int)(w * 0.090f), (int)(h * 0.215f), (int)(w * 0.192f), (int)(h * 0.028f));
    label_CurrentPlayingChord->setFont(fontSize);

    // Scan mode buttons row (~32% down)
    int scanBtnSize = (int)(50 * s);
    int scanY = (int)(h * 0.321f);
    button_ScanOnlyMainChords->setBounds((int)(w * 0.096f), scanY, scanBtnSize, scanBtnSize);
    button_ScanAllChords->setBounds((int)(w * 0.346f), scanY, scanBtnSize, scanBtnSize);
    button_ScanSpecificRange->setBounds((int)(w * 0.562f), scanY, scanBtnSize, scanBtnSize);
    button_ScanByFrequency->setBounds((int)(w * 0.793f), scanY, scanBtnSize, scanBtnSize);

    // Keynote/Frequency containers (~39% down)
    int containerY = (int)(h * 0.391f);
    int containerH = (int)(35 * s);
    int containerW = (int)(w * 0.383f);
    component_KeynoteContainer->setBounds(0, containerY, containerW, containerH);
    component_FrequencyContainer->setBounds(0, containerY, containerW, containerH);

    int comboBoxWidth = (int)(186 * s);
    int keynoteFromX = (int)(176 * s);
    int keynoteToX = (int)(420 * s);
    comboBoxKeynoteFrom->setBounds(keynoteFromX, 0, comboBoxWidth, containerH);
    comboBoxKeynoteTo->setBounds(keynoteToX, 0, comboBoxWidth, containerH);
    textEditorFrequencyFrom->setBounds(keynoteFromX, 0, comboBoxWidth, containerH);
    textEditorFrequencyFrom->applyFontToAllText(fontSize);
    textEditorFrequencyTo->setBounds(keynoteToX, 0, comboBoxWidth, containerH);
    textEditorFrequencyTo->applyFontToAllText(fontSize);

    // Octave combo boxes
    int octaveY = containerY;
    comboBoxOctaveFrom->setBounds((int)(w * 0.565f), octaveY, comboBoxWidth, containerH);
    comboBoxOctaveTo->setBounds((int)(w * 0.725f), octaveY, comboBoxWidth, containerH);

    // Extended range checkbox
    button_ExtendedRange->setBounds((int)(w * 0.850f), octaveY + (int)(5 * s), (int)(30 * s), (int)(30 * s));

    // Frequency & closest chord labels (~46% down)
    label_Frequency->setBounds((int)(w * 0.160f), (int)(h * 0.458f), (int)(w * 0.192f), (int)(h * 0.028f));
    label_Frequency->setFont(fontSize);
    label_ClosestChord->setBounds((int)(w * 0.658f), (int)(h * 0.458f), (int)(w * 0.192f), (int)(h * 0.028f));
    label_ClosestChord->setFont(fontSize);

    // Instrument combo box (~53% down)
    comboBoxPlayingInstrument->setBounds((int)(w * 0.198f), (int)(h * 0.528f), (int)(w * 0.192f), (int)(43 * s));

    // WT Editor button
    button_WavetableEditor->setBounds((int)(w * 0.878f), (int)(h * 0.528f), (int)(150 * s), (int)(43 * s));

    // Waveform buttons row (~58% down)
    int waveBtnSize = (int)(38 * s);
    int waveY = (int)(h * 0.585f);
    button_Default->setBounds((int)(w * 0.094f), waveY, waveBtnSize, waveBtnSize);
    button_Sine->setBounds((int)(w * 0.211f), waveY, waveBtnSize, waveBtnSize);
    button_Triangle->setBounds((int)(w * 0.364f), waveY, waveBtnSize, waveBtnSize);
    button_Square->setBounds((int)(w * 0.523f), waveY, waveBtnSize, waveBtnSize);
    button_Sawtooth->setBounds((int)(w * 0.702f), waveY, waveBtnSize, waveBtnSize);
    button_Wavetable->setBounds((int)(w * 0.876f), waveY, waveBtnSize, waveBtnSize);

    // ADSR knobs (~64% down)
    int knobW = (int)(130 * s);
    int knobH = (int)(158 * s);
    int textBoxW = (int)(78 * s);
    int textBoxH = (int)(28 * s);
    int knobY = (int)(h * 0.636f);
    int knobSpacing = (int)(w * 0.155f);
    int knobLeft = (int)(w * 0.239f);

    slider_Amplitude->setTextBoxStyle(Slider::TextBoxBelow, false, textBoxW, textBoxH);
    slider_Amplitude->setBounds(knobLeft, knobY, knobW, knobH);
    slider_Attack->setTextBoxStyle(Slider::TextBoxBelow, false, textBoxW, textBoxH);
    slider_Attack->setBounds(knobLeft + knobSpacing, knobY, knobW, knobH);
    slider_Sustain->setTextBoxStyle(Slider::TextBoxBelow, false, textBoxW, textBoxH);
    slider_Sustain->setBounds(knobLeft + 2 * knobSpacing, knobY, knobW, knobH);
    slider_Decay->setTextBoxStyle(Slider::TextBoxBelow, false, textBoxW, textBoxH);
    slider_Decay->setBounds(knobLeft + 3 * knobSpacing, knobY, knobW, knobH);
    slider_Release->setTextBoxStyle(Slider::TextBoxBelow, false, textBoxW, textBoxH);
    slider_Release->setBounds(knobLeft + 4 * knobSpacing, knobY, knobW, knobH);

    // Repeater text editors (~87% down, relative to remaining area)
    int playY = (int)(h * 0.869f);
    int editorW = (int)(120 * s);
    int editorH = (int)(35 * s);
    textEditorRepeat->setBounds((int)(w * 0.174f), playY, editorW, editorH);
    textEditorRepeat->applyFontToAllText(fontSize);
    textEditorLength->setBounds((int)(w * 0.380f), playY, (int)(111 * s), editorH);
    textEditorLength->applyFontToAllText(fontSize);
    textEditorPause->setBounds((int)(w * 0.575f), playY, editorW, editorH);
    textEditorPause->applyFontToAllText(fontSize);

    // Output selection combo box
    comboBoxOutputSelection->setBounds((int)(w * 0.735f), (int)(h * 0.867f), (int)(250 * s), (int)(43 * s));

    wavetableEditorComponent->setShortcut(0);
}

void ChordScannerComponent::timerCallback()
{
    if (projectManager->chordScannerProcessor->getPlaystate() == PLAY_STATE::PLAYING)
    {
        MessageManagerLock lock;

        float val = projectManager->chordScannerProcessor->repeater->getProgressBarValue();

        transportToolbar->setProgressValue(val);

        String text = projectManager->chordScannerProcessor->repeater->getTimeRemainingInSecondsString();

        transportToolbar->setPlayingText(text);
        
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
    g.fillAll(juce::Colour(45, 44, 44));
}


void ChordScannerComponent::buttonClicked (Button*button)
{
    if (button == button_Default.get())
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
}

// TransportToolbarComponent::Listener implementations
void ChordScannerComponent::transportPlayClicked()
{
    projectManager->setPlayerCommand(PLAYER_COMMANDS::COMMAND_PLAYER_PLAYPAUSE);
}

void ChordScannerComponent::transportStopClicked()
{
    projectManager->setPlayerCommand(PLAYER_COMMANDS::COMMAND_PLAYER_STOP);
}

void ChordScannerComponent::transportRecordClicked()
{
    projectManager->setPlayerCommand(PLAYER_COMMANDS::COMMAND_PLAYER_RECORD);
}

void ChordScannerComponent::transportPanicClicked()
{
    projectManager->setPanicButton();
}

void ChordScannerComponent::transportSaveClicked()
{
    projectManager->saveProfileForMode(AUDIO_MODE::MODE_CHORD_SCANNER);
}

void ChordScannerComponent::transportLoadClicked()
{
    projectManager->loadProfileForMode(AUDIO_MODE::MODE_CHORD_SCANNER);
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
