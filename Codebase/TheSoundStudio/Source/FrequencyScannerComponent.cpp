/*
  ==============================================================================

    FrequencyScannerComponent.cpp

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

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
    imageBlueButtonNormal               = ImageCache::getFromMemory(BinaryData::BlueButton_Normal_png, BinaryData::BlueButton_Normal_pngSize);
    imageBlueButtonSelected             = ImageCache::getFromMemory(BinaryData::BlueButton_Selected_png, BinaryData::BlueButton_Selected_pngSize);
    imageBlueCheckButtonNormal          = ImageCache::getFromMemory(BinaryData::Button_Checkbox_Normal_Max_png, BinaryData::Button_Checkbox_Normal_Max_pngSize);
    imageBlueCheckButtonSelected        = ImageCache::getFromMemory(BinaryData::Button_Checkbox_Selected_Max_png, BinaryData::Button_Checkbox_Selected_Max_pngSize);



    // Knobs

    slider_Amplitude = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_AMPLITUDE);
    slider_Amplitude    ->setRange (0, 1.0, 0);
    slider_Amplitude    ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_Amplitude    ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_Amplitude    ->addListener (this);
    slider_Amplitude    ->setTextValueSuffix("%");
    slider_Amplitude    ->setNumDecimalPlacesToDisplay(1);
    slider_Amplitude    ->setLookAndFeel(&lookAndFeel);

    addAndMakeVisible(slider_Amplitude.get());


    slider_Attack = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_ATTACK);
    slider_Attack       ->setRange (0, 1.0, 0);
    slider_Attack       ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_Attack       ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_Attack       ->addListener (this);
    slider_Attack       ->setTextValueSuffix("Ms");
    slider_Attack       ->setNumDecimalPlacesToDisplay(0);
    slider_Attack       ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_Attack.get());

    slider_Sustain = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_SUSTAIN);
    slider_Sustain      ->setRange (0, 1.0, 0);
    slider_Sustain      ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_Sustain      ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_Sustain      ->addListener (this);
    slider_Sustain      ->setTextValueSuffix("%");
    slider_Sustain      ->setNumDecimalPlacesToDisplay(2);
    slider_Sustain        ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_Sustain.get());

    slider_Decay = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_DECAY);
    slider_Decay        ->setRange (0, 1.0, 0);
    slider_Decay        ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_Decay        ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_Decay        ->addListener (this);
    slider_Decay        ->setTextValueSuffix("Ms");
    slider_Decay        ->setNumDecimalPlacesToDisplay(0);
    slider_Decay        ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_Decay.get());

    slider_Release = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_RELEASE);
    slider_Release      ->setRange (0, 1.0, 0);
    slider_Release      ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_Release      ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_Release      ->addListener (this);
    slider_Release      ->setTextValueSuffix("Ms");
    slider_Release      ->setNumDecimalPlacesToDisplay(0);
    slider_Release      ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_Release.get());

    // FFT



    // Transport toolbar with Play/Stop/Panic/Progress/Save/Load
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


    // Waveform buttons
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

    button_WavetableEditor = std::make_unique<TextButton>("");
    button_WavetableEditor->setButtonText("WT Editor");
    button_WavetableEditor->setLookAndFeel(&lookAndFeel);
    button_WavetableEditor->addListener(this);
    addAndMakeVisible(button_WavetableEditor.get());


    //Chord Scan buttons

    button_ScanAllFrequencies = std::make_unique<ImageButton>();
    button_ScanAllFrequencies->setTriggeredOnMouseDown(true);
    button_ScanAllFrequencies->setImages (false, true, true,
                                     imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                     Image(), 1.000f, Colour (0x00000000),
                                     imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_ScanAllFrequencies->addListener(this);
    addAndMakeVisible(button_ScanAllFrequencies.get());

    button_ScanSpecificRange = std::make_unique<ImageButton>();
    button_ScanSpecificRange->setTriggeredOnMouseDown(true);
    button_ScanSpecificRange->setImages (false, true, true,
                                         imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                         Image(), 1.000f, Colour (0x00000000),
                                         imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_ScanSpecificRange->addListener(this);
    addAndMakeVisible(button_ScanSpecificRange.get());




    // Text Entry for Repeater

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
    addAndMakeVisible(textEditorLength.get());


    // visualiser component
    visualiserContainerComponent = std::make_unique<VisualiserContainerComponent2>(projectManager, AUDIO_MODE::MODE_FREQUENCY_SCANNER);
    addAndMakeVisible(visualiserContainerComponent.get());







    label_CurrentPlayingFrequency = std::make_unique<Label>();
    label_CurrentPlayingFrequency->setText("0.000 Hz", dontSendNotification);
    label_CurrentPlayingFrequency->setJustificationType(Justification::centred);
    fontLight.setHeight(33);
    label_CurrentPlayingFrequency->setFont(fontLight);
    label_CurrentPlayingFrequency->setColour(Label::textColourId , Colours::darkgrey);
    addAndMakeVisible(label_CurrentPlayingFrequency.get());



    //=============================================

    component_FrequencyContainer = std::make_unique<Component>();
    addAndMakeVisible(component_FrequencyContainer.get());

    //=============================================


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
    textEditorFrequencyTo->setText("0.000 Hz");
    textEditorFrequencyTo->addListener(this);
    textEditorFrequencyTo->setLookAndFeel(&lookAndFeel);
    textEditorFrequencyTo->setJustification(Justification::centred);
    textEditorFrequencyTo->setFont(fontSemiBold);
    textEditorFrequencyTo->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditorFrequencyTo->applyFontToAllText(fontSemiBold);
    component_FrequencyContainer->addAndMakeVisible(textEditorFrequencyTo.get());


    button_ExtendedRange = std::make_unique<ImageButton>();
    button_ExtendedRange->setTriggeredOnMouseDown(true);
    button_ExtendedRange->setImages (false, true, true,
                                imageBlueCheckButtonNormal, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageBlueCheckButtonSelected, 1.0, Colour (0x00000000));
    button_ExtendedRange->addListener(this);
    component_FrequencyContainer->addAndMakeVisible(button_ExtendedRange.get());

    //Log / Lin buttons
    button_Log = std::make_unique<ImageButton>();
    button_Log->setTriggeredOnMouseDown(true);
    button_Log->setImages (false, true, true,
                                imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_Log->addListener(this);
    component_FrequencyContainer->addAndMakeVisible(button_Log.get());


    button_Lin = std::make_unique<ImageButton>();
    button_Lin ->setTriggeredOnMouseDown(true);
    button_Lin ->setImages (false, true, true,
                                imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_Lin ->addListener(this);
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

    wavetableEditorComponent = new WaveTableOscViewComponent(projectManager, AUDIO_MODE::MODE_FREQUENCY_SCANNER, -1);
    wavetableEditorComponent->setBounds(0, 0, 600, 400);

    // Hand ownership of the editor to the popup window to avoid double-deletion
    popupWavetableWindow = std::make_unique<PopupFFTWindow>("Wavetable Editor - Frequency Scanner", wavetableEditorComponent, Colours::black, DocumentWindow::allButtons, true);
    popupWavetableWindow ->centreWithSize(600, 400);
    popupWavetableWindow ->setVisible(false);
    popupWavetableWindow ->setResizable(true, true);

    startTimerHz(TIMER_UPDATE_RATE);
}

FrequencyScannerComponent::~FrequencyScannerComponent() { }

void FrequencyScannerComponent::resized()
{
    using Layout = TSS::Design::Layout;
    auto bounds = getLocalBounds();
    const int w = bounds.getWidth();
    const int h = bounds.getHeight();
    const float s = w / Layout::kRefContentWidth;

    // Toolbar at bottom
    int toolbarH = (int)(h * Layout::kToolbarHeightRatio);
    transportToolbar->setBounds(bounds.removeFromBottom(toolbarH));

    // Visualiser at top-right
    int visX = (int)(w * 0.224f);
    int visY = (int)(h * 0.019f);
    int visW = (int)(w * 0.735f);
    int visH = (int)(h * 0.225f);
    visualiserContainerComponent->setBounds(visX, visY, visW, visH);

    int fontSize = (int)(30 * s);

    // Current playing frequency label
    label_CurrentPlayingFrequency->setBounds((int)(w * 0.121f), (int)(h * 0.034f), (int)(w * 0.121f), (int)(h * 0.028f));
    label_CurrentPlayingFrequency->setFont(fontSize);

    // Scan mode buttons
    int scanBtnSize = (int)(50 * s);
    int scanY = (int)(h * 0.321f);
    button_ScanAllFrequencies->setBounds((int)(w * 0.137f), scanY, scanBtnSize, scanBtnSize);
    button_ScanSpecificRange->setBounds((int)(w * 0.346f), scanY, scanBtnSize, scanBtnSize);

    // Frequency container
    int containerY = (int)(h * 0.386f);
    int containerH = (int)(50 * s);
    component_FrequencyContainer->setBounds(0, containerY, w, containerH);

    int comboBoxWidth = (int)(220 * s);
    int keynoteFromX = (int)(206 * s);
    int keynoteToX = (int)(480 * s);
    textEditorFrequencyFrom->setBounds(keynoteFromX, (int)(7.5f * s), comboBoxWidth, (int)(35 * s));
    textEditorFrequencyFrom->applyFontToAllText((int)(33 * s));
    textEditorFrequencyTo->setBounds(keynoteToX, (int)(7.5f * s), comboBoxWidth, (int)(35 * s));
    textEditorFrequencyTo->applyFontToAllText((int)(33 * s));
    button_ExtendedRange->setBounds((int)(704 * s), (int)(6 * s), (int)(38 * s), (int)(38 * s));
    button_Log->setBounds((int)(900 * s), 0, scanBtnSize, scanBtnSize);
    button_Lin->setBounds((int)(1210 * s), 0, scanBtnSize, scanBtnSize);
    textEditor_LogValue->setBounds((int)(980 * s), (int)(7.5f * s), comboBoxWidth, (int)(35 * s));
    textEditor_LogValue->applyFontToAllText((int)(33 * s));
    textEditor_LinValue->setBounds((int)(1292 * s), (int)(7.5f * s), comboBoxWidth, (int)(35 * s));
    textEditor_LinValue->applyFontToAllText((int)(33 * s));

    // Waveform buttons
    int waveBtnSize = (int)(38 * s);
    int waveY = (int)(h * 0.478f);
    button_Sine->setBounds((int)(w * 0.166f), waveY, waveBtnSize, waveBtnSize);
    button_Triangle->setBounds((int)(w * 0.320f), waveY, waveBtnSize, waveBtnSize);
    button_Square->setBounds((int)(w * 0.483f), waveY, waveBtnSize, waveBtnSize);
    button_Sawtooth->setBounds((int)(w * 0.652f), waveY, waveBtnSize, waveBtnSize);
    button_Wavetable->setBounds((int)(w * 0.833f), waveY, waveBtnSize, waveBtnSize);
    button_WavetableEditor->setBounds((int)(w * 0.833f), (int)(h * 0.444f), (int)(150 * s), (int)(38 * s));

    // ADSR knobs
    int knobW = (int)(130 * s);
    int knobH = (int)(158 * s);
    int textBoxW = (int)(78 * s);
    int textBoxH = (int)(28 * s);
    int knobY = (int)(h * 0.529f);
    int knobLeft = (int)(w * 0.239f);
    int knobSpacing = (int)(w * 0.155f);

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

    // Repeater text editors
    int playY = (int)(h * 0.806f);
    int editorW = (int)(140 * s);
    int editorH = (int)(35 * s);
    textEditorRepeat->setBounds((int)(w * 0.174f), playY, editorW, editorH);
    textEditorRepeat->applyFontToAllText((int)(30 * s));
    textEditorLength->setBounds((int)(w * 0.380f), playY, editorW, editorH);
    textEditorLength->applyFontToAllText((int)(30 * s));
    textEditorPause->setBounds((int)(w * 0.575f), playY, editorW, editorH);
    textEditorPause->applyFontToAllText((int)(30 * s));

    // Output selection
    comboBoxOutputSelection->setBounds((int)(w * 0.735f), (int)(h * 0.806f), (int)(250 * s), (int)(43 * s));
}


void FrequencyScannerComponent::timerCallback()
{
    if (projectManager->frequencyScannerProcessor->getPlaystate() == PLAY_STATE::PLAYING)
    {
        MessageManagerLock lock;

        float val = projectManager->frequencyScannerProcessor->repeater->getProgressBarValue();

        transportToolbar->setProgressValue(val);

        String text = projectManager->frequencyScannerProcessor->repeater->getTimeRemainingInSecondsString();

        transportToolbar->setPlayingText(text);

        String freqText = projectManager->frequencyScannerProcessor->repeater->getCurrentFrequencyString();
        label_CurrentPlayingFrequency->setText(freqText, dontSendNotification);
    }
    else
    {
        float val = projectManager->frequencyScannerProcessor->repeater->getProgressBarValue();

        transportToolbar->setProgressValue(val);
    }

}

void FrequencyScannerComponent::paint (Graphics& g)
{
    g.fillAll(juce::Colour(45, 44, 44));
}

void FrequencyScannerComponent::buttonClicked (Button*button)
{
    if (button == button_Default.get())
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
}

void FrequencyScannerComponent::transportPlayClicked()
{
    projectManager->setPlayerCommand(PLAYER_COMMANDS::COMMAND_PLAYER_PLAYPAUSE);
}

void FrequencyScannerComponent::transportStopClicked()
{
    projectManager->setPlayerCommand(PLAYER_COMMANDS::COMMAND_PLAYER_STOP);
}

void FrequencyScannerComponent::transportPanicClicked()
{
    projectManager->setPanicButton();
}

void FrequencyScannerComponent::transportSaveClicked()
{
    projectManager->saveProfileForMode(AUDIO_MODE::MODE_FREQUENCY_SCANNER);
}

void FrequencyScannerComponent::transportLoadClicked()
{
    projectManager->loadProfileForMode(AUDIO_MODE::MODE_FREQUENCY_SCANNER);
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
