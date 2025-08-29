/*
  ==============================================================================

    FundamentalFrequencyComponent.cpp
    Created: 17 Mar 2021 9:47:28pm
    Author:  Gary Jones

  ==============================================================================
*/

#include "FundamentalFrequencyComponent.h"


FundamentalFrequencyComponent::FundamentalFrequencyComponent(ProjectManager& pm) :
projectManager(pm),
frequencyProcessor(pm.fundamentalFrequencyProcessor ? *pm.fundamentalFrequencyProcessor : 
                   throw std::runtime_error("FundamentalFrequencyProcessor is null"))
{
    Image imagePlayButton                     = ImageCache::getFromMemory(BinaryData::playPause2x_png, BinaryData::playPause2x_pngSize);
    Image imageStopButton                     = ImageCache::getFromMemory(BinaryData::stop2x_png, BinaryData::stop2x_pngSize);
    Image imageNoiseButton = ImageCache::getFromMemory(BinaryData::PanicButton2x_png, BinaryData::PanicButton2x_pngSize);
    
    Image imageButtonStart;
    Image imageButtonStop;
    
    Image imageSliderKnob;
    Image imageSliderBackground;
    Image imageCloseButton;
    Image imageBlueButtonNormal = ImageCache::getFromMemory(BinaryData::BlueButton_Normal_png, BinaryData::BlueButton_Normal_pngSize);
    Image imageBlueButtonSelected = ImageCache::getFromMemory(BinaryData::BlueButton_Selected_png, BinaryData::BlueButton_Selected_pngSize);
    Image imageAddButton = ImageCache::getFromMemory(BinaryData::AddButton2x_png, BinaryData::AddButton2x_pngSize);
    
    // fonts
    Typeface::Ptr AssistantLight        = Typeface::createSystemTypefaceFor(BinaryData::AssistantLight_ttf, BinaryData::AssistantLight_ttfSize);
    Typeface::Ptr AssistantBold     = Typeface::createSystemTypefaceFor(BinaryData::AssistantBold_ttf, BinaryData::AssistantBold_ttfSize);
    Typeface::Ptr AssistantSemiBold   = Typeface::createSystemTypefaceFor(BinaryData::AssistantSemiBold_ttf, BinaryData::AssistantSemiBold_ttfSize);
    Font fontBold(AssistantBold);
    Font fontNormal(AssistantSemiBold);
    Font fontLight(AssistantLight);
    Font fontSemiBold(AssistantSemiBold);
    
    fontNormal.setHeight(33);
    fontBold.setHeight(38);
    fontSemiBold.setHeight(33 * scaleFactor);
    fontLight.setHeight(33 * scaleFactor);
    
    buttonStart = std::make_unique<asvpr::PlayerButton>(asvpr::PlayerButton::Type::PlayPause);
    buttonStart->setTriggeredOnMouseDown(true);
    buttonStart->addListener(this);
    addAndMakeVisible(*buttonStart);
    
    
    buttonStop = std::make_unique<asvpr::PlayerButton>(asvpr::PlayerButton::Type::Stop);
    buttonStop->setTriggeredOnMouseDown(true);
    buttonStop->addListener(this);
    addAndMakeVisible(*buttonStop);
    
    noiseButton = std::make_unique<ImageButton>();
    noiseButton->setTriggeredOnMouseDown(true);
    noiseButton->setImages (false, true, true,
                            imageNoiseButton, 0.999f, Colour (0x00000000),
                            Image(), 1.000f, Colour (0x00000000),
                            imageNoiseButton, 0.6, Colour (0x00000000));
    noiseButton->addListener(this);
    addAndMakeVisible(*noiseButton);

    visualiserSelectorComponent = std::make_unique<VisualiserSelectorComponent>(&projectManager, VisualiserSelectorComponent::VISUALISER_TYPE::SPECTROGRAM, AUDIO_MODE::MODE_FUNDAMENTAL_FREQUENCY);
    visualiserSelectorComponent->setBounds(fftLeftMargin * scaleFactor, fftTopMargin * scaleFactor, fftWidth * scaleFactor, fftHeight * scaleFactor);
    visualiserSelectorComponent->setNewVisualiserType(VisualiserSelectorComponent::VISUALISER_TYPE::SPECTROGRAM);
    visualiserSelectorComponent->setProcessingActive(false);
    
    addAndMakeVisible(*visualiserSelectorComponent);

    auto imageBlueCheckButtonNormal = ImageCache::getFromMemory(BinaryData::Button_Checkbox_Normal_Max_png,
                                                                BinaryData::Button_Checkbox_Normal_Max_pngSize);
    auto imageBlueCheckButtonSelected = ImageCache::getFromMemory(BinaryData::Button_Checkbox_Selected_Max_png,
                                                                  BinaryData::Button_Checkbox_Selected_Max_pngSize);

    // Harmonic Data Feedback

    addAndMakeVisible(containers.createContainer("FFT Parameters", true));

    addAndMakeVisible(containers.createContainer("Threshold & Tolerance", true));

    addAndMakeVisible(containers.createContainer("Range", true));

    addAndMakeVisible(containers.createContainer("Process", true));

    auto& outputContainer = containers.createContainer("Harmonics", false);

    prepare_fft_parameters();
    prepare_thresholdAndTolerance();
    prepare_range();
    prepare_process_and_synth();
    prepare_harmonics_and_results();


    labelFrequency = std::make_unique<Label>();
    labelFrequency->setText("0.0 hz", dontSendNotification);
    labelFrequency->setJustificationType(Justification::centred);
    labelFrequency->setFont(fontNormal);
    containers.getContainer(4).addAndMakeVisible(*labelFrequency);
    labelFrequency->setBounds(0, 15, 150, 40);

    labelChord = std::make_unique<Label>();
    labelChord->setText("-", dontSendNotification);
    labelChord->setJustificationType(Justification::centred);
    labelChord->setFont(fontNormal);
    containers.getContainer(4).addAndMakeVisible(*labelChord);
    labelChord->setBounds(0, 50, 150, 40);

    visualiserSelectorComponent->setNewVisualiserSource(static_cast<VISUALISER_SOURCE>(1));
    visualiserSelectorComponent->showInputMenu(false);

    outputContainer.disableTitle(true);
    addAndMakeVisible(outputContainer);

    frequencyProcessor.getAnalyser().add_listener(*this);
    projectManager.addUIListener(this);
    startTimer(100);
}

FundamentalFrequencyComponent::~FundamentalFrequencyComponent()
{
    projectManager.removeUIListener(this);
    frequencyProcessor.getAnalyser().remove_listener(*this);
    setLookAndFeel(nullptr);
}

void FundamentalFrequencyComponent::resized()
{
    visualiserSelectorComponent->setBounds(48 * scaleFactor,
                                           40 * scaleFactor,
                                           1476 * scaleFactor,
                                           340 * scaleFactor);

    auto localBounds = getLocalBounds();

    localBounds.removeFromTop(200);

    localBounds.removeFromLeft(20);

    localBounds.removeFromRight(20);

    auto x = localBounds.getX();
    auto y = localBounds.getY();
    auto width = localBounds.getWidth();

    auto setContainerBounds = [this] (int index, const juce::Rectangle<int>& rec)
    {
        containers.setContainerBounds(index, rec);

        return containers.getContainer(index).getBottom();
    };

    auto gap = 10;

    y = setContainerBounds(0, juce::Rectangle<int>(x, y, width, 90)) + gap;

    y = std::max(setContainerBounds(1, juce::Rectangle<int>(x, y, width * 0.6f, 140)),
                 setContainerBounds(2, juce::Rectangle<int>(x + width * 0.6f + gap,
                                                            y,
                                                            (width * 0.4f) - gap,
                                                            140))) + gap;

    y = setContainerBounds(3, juce::Rectangle<int>(x,
                                                   y,
                                                   width,
                                                   70)) + gap;

    y = setContainerBounds(4, juce::Rectangle<int>(x, y, width, 100)) + gap;

    buttonStart->setBounds(localBounds.getCentreX() - 10, 645, 90, 25);
    buttonStop->setBounds(localBounds.getCentreX() - 95, 645, 60, 25);
    noiseButton->setBounds(localBounds.getCentreX() + 300, 640, 75, 75);
}

void FundamentalFrequencyComponent::paint(Graphics&g)
{
    g.fillAll({45,44,44});
}

void FundamentalFrequencyComponent::updateFrequencyOutput()
{
    auto active = frequencyProcessor.is_fft_process_enable();

    auto data = frequencyProcessor.getAnalyser().getHarmonics().get_harmonics_array();

    auto i = 0;

    Array<String> harmonicsArray;
    for (auto& label : labelHarmonic)
    {
        auto& harmonic = data[i];

        auto stringFreq = harmonic.isActive && active ? juce::String(harmonic.freq) + juce::String("Hz"): juce::String("Inactive");
        harmonicsArray.add(harmonic.isActive && active ? juce::String(harmonic.freq) + juce::String("Hz"): juce::String("NA"));

        auto harmonicString = juce::String("H") +
                              juce::String(i + 1) +
                              juce::String(" : ") +
                              stringFreq;

        label.setText(harmonicString, dontSendNotification);

        ++i;
    }

    auto fundamentalFrequency = active ? juce::String(frequencyProcessor.getCurrentFrequency()) : juce::String("0");
    fundamentalFrequency.append("Hz", 2);

    const auto chordName = getChordNameFromHarmonicData(data);
    labelChord->setText(chordName, juce::dontSendNotification);

    labelFrequency->setText(fundamentalFrequency, juce::dontSendNotification);
    
    if (active && logToFile)
    {
        projectManager.logFileWriter->processLog_FundamentalFrequency_Sequencer(fundamentalFrequency, chordName, harmonicsArray);
        logToFile = false;
    }
}

void FundamentalFrequencyComponent::buttonClicked (Button * button)
{
    if (button == buttonStart.get())
    {
        if (frequencyProcessor.is_fft_process_enable())
        {
            frequencyProcessor.startAnalyser();

            if (frequencyProcessor.isPlaying())
            {
                visualiserSelectorComponent->setProcessingActive(true);
                buttonStart->setButtonText("Playing...");
                buttonStart->setButtonColour({129, 135, 140});
                buttonStart->setEnabled(false);
                logToFile = true;
            }
            else
            {
                visualiserSelectorComponent->setProcessingActive(false);
                buttonStart->resetButtonColour();
                buttonStart->resetButtonText();
                buttonStart->setEnabled(true);
            }
        }
    }
    else if (button == buttonStop.get())
    {
        frequencyProcessor.stopAnalyser();
        frequencyProcessor.clearAlgorithm();
        buttonStart->resetButtonColour();
        buttonStart->resetButtonText();
        buttonStart->setEnabled(true);
        visualiserSelectorComponent->setProcessingActive(false);
    }
    else if (button == noiseButton.get())
    {
        projectManager.setPanicButton();
    }
    else if (button ==  buttonProcessFFT.get())
    {
        frequencyProcessor.enable_fft_process(buttonProcessFFT->getToggleState());
    }
    else if (button == buttonCustomFrequencyRange.get())
    {
        frequencyProcessor.enable_custom_range(!buttonCustomFrequencyRange->getToggleState());
    }
}

void FundamentalFrequencyComponent::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == comboBoxInput.get())
    {
        visualiserSelectorComponent->setNewVisualiserSource(static_cast<VISUALISER_SOURCE>(comboBoxInput->getSelectedId()));
    }
}

void FundamentalFrequencyComponent::textEditorReturnKeyPressed (TextEditor&editor)
{
    if (&editor == textEditorLength.get())
    {
        auto opt = frequencyProcessor.set_iteration_length(editor.getText().getIntValue());

        if (opt.has_value())
        {
            editor.setText(juce::String(opt.value()));
        }

        buttonStart->resetButtonText();
        buttonStart->resetButtonColour();
    }
    else if (&editor == textEditorIteration.get())
    {
        frequencyProcessor.set_iterations(editor.getText().getIntValue());
        buttonStart->setToggleState(false, juce::dontSendNotification);
        buttonStart->resetButtonText();
        buttonStart->resetButtonColour();
    }
//    else if (&editor == textEditorMinFrequency.get())
//    {
//        int value = editor.getText().getIntValue();
//
//        // bounds check
//        if (value >= FREQUENCY_MAX) {  value = FREQUENCY_MAX; }
//        if (value <= FREQUENCY_MIN) { value = FREQUENCY_MIN; }
//
//        String newVal(value); editor.setText(newVal);
//
//        // send to projectManager
//        projectManager.setFundamentalFrequencyParameter( FUNDAMENTAL_FREQUENCY_MIN_FREQ, value);
//    }
//    else if (&editor == textEditorMaxFrequency.get())
//    {
//        int value = editor.getText().getIntValue();
//
//        // bounds check
//        if (value >= FREQUENCY_MAX) {  value = FREQUENCY_MAX; }
//        if (value <= FREQUENCY_MIN) { value = FREQUENCY_MIN; }
//
//        String newVal(value); editor.setText(newVal);
//
//        // send to projectManager
//        projectManager.setFundamentalFrequencyParameter( FUNDAMENTAL_FREQUENCY_MAX_FREQ, value);
//    }

    juce::Component::unfocusAllComponents();
}

//void FundamentalFrequencyComponent::updateFundamentalFrequencyUIParameter(int paramIndex)
//{
//
//    if (paramIndex == FUNDAMENTAL_FREQUENCY_FFT_WINDOW)
//    {
//        comboBoxFFTWindow->setSelectedId(projectManager.getFundamentalFrequencyParameter(FUNDAMENTAL_FREQUENCY_FFT_WINDOW).operator int(), dontSendNotification);
//    }
//    else if (paramIndex == FUNDAMENTAL_FREQUENCY_INPUT_CHANNEL)
//    {
//        comboBoxInput->setSelectedId(projectManager.getFundamentalFrequencyParameter(FUNDAMENTAL_FREQUENCY_INPUT_CHANNEL).operator int() + 1, dontSendNotification);
//    }
//    else if (paramIndex == FUNDAMENTAL_FREQUENCY_OUTPUT_CHANNEL)
//    {
//        comboBoxOutput->setSelectedId(projectManager.getFundamentalFrequencyParameter(FUNDAMENTAL_FREQUENCY_OUTPUT_CHANNEL).operator int() + 1, dontSendNotification);
//    }
//    else if (paramIndex == NUM_HARMONICS_TO_TRACK)
//    {
//        comboBoxNumHarmonics->setSelectedId(projectManager.getFundamentalFrequencyParameter(NUM_HARMONICS_TO_TRACK).operator int(), dontSendNotification);
//    }
//    else if (paramIndex == ALGORITHM)
//    {
//        comboBoxAlgorithm->setSelectedId(static_cast<int>(projectManager.getFundamentalFrequencyParameter(ALGORITHM)), dontSendNotification);
//    }
//    else if (paramIndex == FUNDAMENTAL_FREQUENCY_CUSTOM_RANGE)
//    {
//        buttonCustomFrequencyRange->setToggleState(projectManager.getFundamentalFrequencyParameter( FUNDAMENTAL_FREQUENCY_CUSTOM_RANGE ).operator bool(), dontSendNotification);
//    }
//
//    else if (paramIndex == FUNDAMENTAL_FREQUENCY_MIN_FREQ)
//    {
//        float freq = projectManager.getFundamentalFrequencyParameter(FUNDAMENTAL_FREQUENCY_MIN_FREQ).operator float();
//        String freqString(freq); freqString.append("Hz", 2);
//        textEditorMinFrequency->setText(freqString);
//    }
//
//    else if (paramIndex == FUNDAMENTAL_FREQUENCY_MAX_FREQ)
//    {
//        float freq = projectManager.getFundamentalFrequencyParameter(FUNDAMENTAL_FREQUENCY_MAX_FREQ).operator float();
//        String freqString(freq); freqString.append("Hz", 2);
//        textEditorMaxFrequency->setText(freqString);
//    }
//
//    else if (paramIndex == FUNDAMENTAL_FREQUENCY_INPUT_THRESHOLD)
//    {
//        sliderThresholdInputDetection->setValue(projectManager.getFundamentalFrequencyParameter( FUNDAMENTAL_FREQUENCY_INPUT_THRESHOLD ).operator double());
//    }
//
//    else if (paramIndex == FUNDAMENTAL_FREQUENCY_KEYNOTE_TOLERANCE)
//    {
//        sliderKeynoteTolerance      ->setValue(projectManager.getFundamentalFrequencyParameter( FUNDAMENTAL_FREQUENCY_KEYNOTE_TOLERANCE ).operator double());
//    }
//    else if (paramIndex == FUNDAMENTAL_FREQUENCY_MIN_INTERVAL)
//    {
//        sliderMinIntervalSize      ->setValue(projectManager.getFundamentalFrequencyParameter( FUNDAMENTAL_FREQUENCY_MIN_INTERVAL ).operator double());
//    }
//
//    else if (paramIndex == FUNDAMENTAL_FREQUENCY_MAX_INTERVAL)
//    {
//        sliderMaxIntervalSize      ->setValue(projectManager.getFundamentalFrequencyParameter( FUNDAMENTAL_FREQUENCY_MAX_INTERVAL ).operator double());
//    }
//}

void FundamentalFrequencyComponent::prepare_fft_parameters()
{

    auto doToBox = [this] (asvpr::ComboBox& c)
    {
        c.showLabel(true);
        c.addListener(this);
        c.setLookAndFeel(&lookAndFeel);
        c.getRootMenu()->setLookAndFeel(&lookAndFeel);
        containers.getContainer(0).addAndMakeVisible(c);
    };

    comboBoxFFTSize = std::make_unique<asvpr::ComboBoxWithAttachment>("FFT Size");
    comboBoxFFTSize->addItem("2 ^ 10", 1024);
    comboBoxFFTSize->addItem("2 ^ 11", 2048);
    comboBoxFFTSize->addItem("2 ^ 12", 4096);
    comboBoxFFTSize->addItem("2 ^ 13", 8192);
    comboBoxFFTSize->addItem("2 ^ 14", 16384);
    comboBoxFFTSize->addItem("2 ^ 15", 32768);
    comboBoxFFTSize->addItem("2 ^ 16", 65536);
    comboBoxFFTSize->addItem("2 ^ 17", 131072);
    comboBoxFFTSize->addItem("2 ^ 18", 262144);
    comboBoxFFTSize->addItem("2 ^ 19", 524288);
    comboBoxFFTSize->addItem("2 ^ 20", 1048576);
    comboBoxFFTSize->setBounds(20, 40, 125, 40);
    comboBoxFFTSize->createAttachment(frequencyProcessor.getApvts(),
                                      FundamentalFrequencyProcessor::get_parameter_name(FundamentalFrequencyProcessor::Parameter::fft_size));


    doToBox(*comboBoxFFTSize);

    comboBoxFFTWindow = std::make_unique<asvpr::ComboBoxWithAttachment>("FFT Window");
    comboBoxFFTWindow->addItem("Rectangular", 1);
    comboBoxFFTWindow->addItem("Triangular", 2);
    comboBoxFFTWindow->addItem("Hann", 3);
    comboBoxFFTWindow->addItem("Hamming", 4);
    comboBoxFFTWindow->addItem("Blackman", 5);
    comboBoxFFTWindow->addItem("BlackmanHarris", 6);
    comboBoxFFTWindow->addItem("Flat Top", 7);
    comboBoxFFTWindow->addItem("Kaiser", 8);
    comboBoxFFTWindow->setBounds(160, 40, 125, 40);
    comboBoxFFTWindow->createAttachment(frequencyProcessor.getApvts(),
                                      FundamentalFrequencyProcessor::get_parameter_name(FundamentalFrequencyProcessor::Parameter::fft_window));
    doToBox(*comboBoxFFTWindow);

    comboBoxInput = std::make_unique<asvpr::ComboBoxWithAttachment>("Input Channel");

    auto inputDevices = frequencyProcessor.getInputNames();

    {
        auto deviceIndex = 1;
        for (auto& input : inputDevices)
        {
            comboBoxInput->addItem(input, deviceIndex);

            ++deviceIndex;
        }
    }
    
    comboBoxInput->setBounds(299, 40, 125, 40);
    comboBoxInput->createAttachment(frequencyProcessor.getApvts(),
                                    FundamentalFrequencyProcessor::get_parameter_name(FundamentalFrequencyProcessor::Parameter::input_channel));
    doToBox(*comboBoxInput);

    comboBoxAlgorithm = std::make_unique<asvpr::ComboBoxWithAttachment>("Algorithm");

    comboBoxAlgorithm->addItemList(ProjectStrings::getFundamentalFrequencyAlgorithms(), 1);
    comboBoxAlgorithm->setBounds(437, 40, 125, 40);
    comboBoxAlgorithm->createAttachment(frequencyProcessor.getApvts(),
                                     FundamentalFrequencyProcessor::get_parameter_name(FundamentalFrequencyProcessor::Parameter::algorithm));
    doToBox(*comboBoxAlgorithm);

    comboBoxNumHarmonics = std::make_unique<asvpr::ComboBoxWithAttachment>("Num Harmonics");

    for (auto harmonic = 1; harmonic <= 10; ++harmonic)
    {
        comboBoxNumHarmonics->addItem(juce::String(harmonic), harmonic);
    }

    comboBoxNumHarmonics->createAttachment(frequencyProcessor.getApvts(),
                                        FundamentalFrequencyProcessor::get_parameter_name(FundamentalFrequencyProcessor::Parameter::harmonics));

    comboBoxNumHarmonics->setBounds(576, 40, 125, 40);
    doToBox(*comboBoxNumHarmonics);

    containers.getContainer(0).paintInBackground = [this] (juce::Graphics& g, const auto& bounds)
    {

    };
}


void FundamentalFrequencyComponent::prepare_thresholdAndTolerance()
{
    auto create = [this] (std::unique_ptr<CustomRotarySlider>& slider,
                          double newMinimum,
                          double newMaximum,
                          double newInterval,
                          const juce::Rectangle<int>& bounds,
                          int decimals)
    {

        slider = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_RELEASE);
        slider->setRange (newMinimum, newMaximum, newInterval);
        slider->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
        slider->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
        slider->setNumDecimalPlacesToDisplay(decimals);
        slider->setLookAndFeel(&lookAndFeel);
        slider->setBounds(bounds);
        slider->setTextBoxStyle (Slider::TextBoxBelow, false, 78 * scaleFactor, 28 * scaleFactor);

        containers.getContainer(1).addAndMakeVisible(*slider);
    };


    create(sliderThresholdInputDetection, -80, 6.0, 0, {15, 35, 80, 80}, 1);
    create(sliderKeynoteTolerance, -100, 100, 0, {115, 35, 80, 80}, 0);
    create(sliderMinIntervalSize, 0.05, 4.0, 0, {235, 35, 80, 80}, 3);
    create(sliderMaxIntervalSize, 0.05, 4.0, 0,{340, 35, 80, 80}, 3);


    containers.getContainer(1).paintInBackground = [this] (juce::Graphics& g, const juce::Rectangle<int>& bounds)
    {
        auto rect = juce::Rectangle<int>(10, 102, 80, 50);

        g.setColour(juce::Colours::white);

        g.drawFittedText("Input Threshold",
                         rect,
                         juce::Justification::centred, 1);

        g.drawFittedText("Keynote Tolerance",
                         rect.translated(110, 0),
                         juce::Justification::centred, 1);

        g.drawFittedText("Min Interval (oct)",
                         rect.translated(230, 0),
                         juce::Justification::centred, 1);

        g.drawFittedText("Max Interval (oct)",
                         rect.translated(340, 0),
                         juce::Justification::centred, 1);

    };

}

void FundamentalFrequencyComponent::prepare_range()
{
    auto imageBlueCheckButtonNormal = ImageCache::getFromMemory(BinaryData::Button_Checkbox_Normal_Max_png,
                                                                BinaryData::Button_Checkbox_Normal_Max_pngSize);
    auto imageBlueCheckButtonSelected = ImageCache::getFromMemory(BinaryData::Button_Checkbox_Selected_Max_png,
                                                                  BinaryData::Button_Checkbox_Selected_Max_pngSize);

    buttonCustomFrequencyRange = std::make_unique<ImageButton>();

    buttonCustomFrequencyRange->setTriggeredOnMouseDown(true);

    buttonCustomFrequencyRange->setImages (false, true, true,
                                           imageBlueCheckButtonNormal, 0.999f, Colour (0x00000000),
                                           Image(), 1.000f, Colour (0x00000000),
                                           imageBlueCheckButtonSelected, 1.0, Colour (0x00000000));

    buttonCustomFrequencyRange->addListener(this);

    buttonCustomFrequencyRange->setLookAndFeel(&lookAndFeel);

    containers.getContainer(2).addAndMakeVisible(buttonCustomFrequencyRange.get());

    buttonCustomFrequencyRange->setBounds(30, 60, 15, 15);

    auto fontNormal = projectManager.getAssistantFont(ProjectManager::FontType::SemiBold).withHeight(20);

    textEditorMinFrequency = std::make_unique<juce::TextEditor>("");
    textEditorMinFrequency->setReturnKeyStartsNewLine(false);
    textEditorMinFrequency->setInputRestrictions(6, "0123456789");
    textEditorMinFrequency->setMultiLine(false);
    textEditorMinFrequency->setText("1");
    textEditorMinFrequency->addListener(this);
    textEditorMinFrequency->setLookAndFeel(&lookAndFeel);
    textEditorMinFrequency->setJustification(Justification::centred);
    textEditorMinFrequency->setFont(fontNormal);
    textEditorMinFrequency->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditorMinFrequency->applyFontToAllText(fontNormal);
    textEditorMinFrequency->applyColourToAllText(Colours::lightgrey);
    textEditorMinFrequency->setBounds(30, 110, 110, 20);
    containers.getContainer(2).addAndMakeVisible(textEditorMinFrequency.get());

    textEditorMaxFrequency = std::make_unique<juce::TextEditor>("");
    textEditorMaxFrequency->setReturnKeyStartsNewLine(false);
    textEditorMaxFrequency->setInputRestrictions(6, "0123456789");
    textEditorMaxFrequency->setMultiLine(false);
    textEditorMaxFrequency->setText("1");
    textEditorMaxFrequency->addListener(this);
    textEditorMaxFrequency->setLookAndFeel(&lookAndFeel);
    textEditorMaxFrequency->setJustification(Justification::centred);
    textEditorMaxFrequency->setFont(fontNormal);
    textEditorMaxFrequency->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditorMaxFrequency->applyFontToAllText(fontNormal);
    textEditorMaxFrequency->applyColourToAllText(Colours::lightgrey);
    textEditorMaxFrequency->setBounds(150, 110, 110, 20);
    containers.getContainer(2).addAndMakeVisible(textEditorMaxFrequency.get());

    containers.getContainer(2).paintInBackground = [this] (juce::Graphics& g, const juce::Rectangle<int>& bounds)
    {
        g.setColour(juce::Colours::white);
        auto b = buttonCustomFrequencyRange->getBounds().translated(10, 0).withWidth(100);

        g.drawFittedText("Custom Range",  b, juce::Justification::centredRight, 1);

        b.translate(-25, 30);
        g.drawFittedText("Min Frequency",  b, juce::Justification::centredRight, 1);

        b.translate(125, 0);
        g.drawFittedText("Max Frequency",  b, juce::Justification::centredRight, 1);
    };
}

void FundamentalFrequencyComponent::prepare_process_and_synth()
{

    auto imageBlueCheckButtonNormal = ImageCache::getFromMemory(BinaryData::Button_Checkbox_Normal_Max_png,
                                                                BinaryData::Button_Checkbox_Normal_Max_pngSize);
    auto imageBlueCheckButtonSelected = ImageCache::getFromMemory(BinaryData::Button_Checkbox_Selected_Max_png,
                                                                  BinaryData::Button_Checkbox_Selected_Max_pngSize);

    buttonProcessFFT = std::make_unique<ImageButton>();
    buttonProcessFFT->setClickingTogglesState(true);
    buttonProcessFFT->setImages (false, true, true,
                                 imageBlueCheckButtonNormal, 0.999f, Colour (0x00000000),
                                 Image(), 1.000f, Colour (0x00000000),
                                 imageBlueCheckButtonSelected, 1.0, Colour (0x00000000));
    buttonProcessFFT->addListener(this);
    buttonProcessFFT->setLookAndFeel(&lookAndFeel);
    buttonProcessFFT->setToggleState(frequencyProcessor.is_fft_process_enable(), juce::dontSendNotification);
    containers.getContainer(3).addAndMakeVisible(*buttonProcessFFT);
    buttonProcessFFT->setBounds(55, 40, 15, 15);

    auto fontSemiBold = projectManager.getAssistantFont(ProjectManager::FontType::SemiBold);

    textEditorIteration = std::make_unique<TextEditor>("");
    textEditorIteration->setReturnKeyStartsNewLine(false);
    textEditorIteration->setMultiLine(false);
    textEditorIteration->setInputRestrictions(2, "0123456789");
    textEditorIteration->setText("3");
    textEditorIteration->addListener(this);
    textEditorIteration->setLookAndFeel(&lookAndFeel);
    textEditorIteration->setJustification(Justification::centred);
    textEditorIteration->setFont(fontSemiBold);
    textEditorIteration->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditorIteration->applyFontToAllText(fontSemiBold);
    textEditorIteration->applyColourToAllText(Colours::lightgrey);
    textEditorIteration->setBounds(300, 37, 80, 20);
    containers.getContainer(3).addAndMakeVisible(*textEditorIteration);

    textEditorLength = std::make_unique<TextEditor>("");
    textEditorLength->setReturnKeyStartsNewLine(false);
    textEditorLength->setMultiLine(false);
    textEditorLength->setInputRestrictions(5, "0123456789");
    textEditorLength->addListener(this);
    textEditorLength->setLookAndFeel(&lookAndFeel);
    textEditorLength->setJustification(Justification::centred);
    textEditorLength->setFont(fontSemiBold);
    textEditorLength->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditorLength->applyFontToAllText(fontSemiBold);
    textEditorLength->applyColourToAllText(Colours::lightgrey);
    textEditorLength->setBounds(600, 37, 80, 20);

    updateIterationUI();

    containers.getContainer(3).addAndMakeVisible(*textEditorLength);

    containers.getContainer(3).paintInBackground = [this] (juce::Graphics& g, const juce::Rectangle<int>& bounds)
    {
        g.setColour(juce::Colours::white);
        g.drawFittedText("Process FFT",
                         juce::Rectangle<int>(80, 23, 200, 50),
                         juce::Justification::centredLeft, 1);

        g.drawFittedText("Iterations",
                         juce::Rectangle<int>(230, 23, 200, 50),
                         juce::Justification::centredLeft, 1);

        g.drawFittedText("Iteration length (ms)",
                         juce::Rectangle<int>(450, 23, 200, 50),
                         juce::Justification::centredLeft, 1);
    };

}

void FundamentalFrequencyComponent::updateIterationUI()
{
    textEditorLength->setText(juce::String(frequencyProcessor.get_iteration_length()));
};

void FundamentalFrequencyComponent::prepare_harmonics_and_results()
{
    auto i = 0;
    auto x = 140;
    auto y = 20;
    auto w = 100;

    for (auto& l : labelHarmonic)
    {
        l.setText("000.000 hz", dontSendNotification);
        l.setJustificationType(Justification::centred);
        containers.getContainer(4).addAndMakeVisible(labelHarmonic[i]);
        l.setBounds(x, y, w, 30);

        x += w + 15;

        if (i == 4)
        {
            x = 140;
            y = y + 30;
        }

        ++i;
    }
}

void FundamentalFrequencyComponent::fft_updated()
{
    auto text = juce::String(frequencyProcessor.get_iteration_length());
    textEditorLength->setText(juce::String(text), juce::dontSendNotification);
}

template <typename HarmonicData>
juce::String FundamentalFrequencyComponent::getChordNameFromHarmonicData(const HarmonicData& data)
{
    auto root = frequencyProcessor.getCurrentFrequency();

    auto harmonics = frequencyProcessor.get_num_of_harmonics();

    juce::String chordString;

    if (root > 0)
    {
        int midiNote;
        int keynote;
        int octave;
        float frequencyDifference;

        projectManager.frequencyManager->getMIDINoteForFrequency(root, midiNote, keynote, octave, frequencyDifference);

        chordString = getKeyNoteString(keynote);

        juce::String sum;

        auto currentNote = keynote;

        int previousInterval = 0;

        auto i = 0;

        for (auto& d : data)
        {
            if (i == 0)
            {
                ++i;
                continue;
            }

            if (i >= harmonics)
            {
                break;
            }

            if (d.freq == 0)
            {
                continue;
            }

            projectManager.frequencyManager->getMIDINoteForFrequency(d.freq,
                                                                     midiNote,
                                                                     keynote,
                                                                     octave,
                                                                     frequencyDifference);

            if (keynote == currentNote)
            {
                continue;
            }

            int interval = 0;

            if (currentNote > keynote)
            {
                interval = std::abs(currentNote - (keynote + 12));
            }

            if (i == 1)
            {
                if (interval == 2) /* Suspended 2th */
                {
                    sum = "sus2";
                }
                else if (interval == 3) /* Minor */
                {
                    sum = "min";
                }

                else if (interval == 4) /* Major */
                {
                    sum = "maj";
                }
                else if (interval == 5) /* Suspended 4th */
                {
                    sum = "sus5";
                }
                else if (interval == 7) /* 5th */
                {
                    sum = "5th";
                }
            }

            else if (i == 2)
            {
                if (sum == "maj")
                {
                    if (interval == 8)
                    {
                        sum = "aug";
                    }
                }
                else if (sum == "min")
                {
                    if (interval == 6)
                    {
                        sum = "dim";
                    }
                }
            }

            else if (i == 3)
            {
                if (sum == "maj")
                {
                    if (interval == 9)
                    {
                        sum = "6th";
                    }
                    else if (interval == 10)
                    {
                        sum = "7th";
                    }
                    else if (interval == 11)
                    {
                        sum = "maj7";
                    }
                    else if (interval == 14)
                    {
                        sum = "add9";
                    }
                }
                else if (sum == "min")
                {
                    if (interval == 9)
                    {
                        sum = "min6";
                    }
                    else if (interval == 10)
                    {
                        sum = "min7";
                    }
                    else if (interval == 14)
                    {
                        sum = "minAdd9";
                    }
                }
                else if (sum == "dim")
                {
                    if (interval == 9)
                    {
                        sum = "dim7";
                    }
                    else if (interval == 10)
                    {
                        sum = "m7b5";
                    }
                }
            }

            else if (i == 4)
            {
                if (sum == "7th")
                {
                    if (interval == 14)
                    {
                        sum = "9th";
                    }
                    else if (interval == 15)
                    {
                        sum = "7#9";
                    }
                }
                else if (sum == "maj7")
                {
                    if (interval == 14)
                    {
                        sum = "maj9";
                    }
                }
                else if (sum == "min7")
                {
                    if (interval == 14)
                    {
                        sum = "min9";
                    }
                }
            }
            else if (i == 5)
            {
                if (sum == "9th")
                {
                    if (interval == 17)
                    {
                        sum = "11th";
                    }
                    else if (interval == 18)
                    {
                        sum = "7#11";
                    }
                }
                else if (sum == "maj9")
                {
                    if (interval == 17)
                    {
                        sum = "maj13";
                    }
                }
                else if (sum == "min9")
                {
                    if (interval == 17)
                    {
                        sum = "min11";
                    }
                }
            }
            else if (i == 6)
            {
                if (sum == "maj13")
                {
                    if (interval == 21)
                    {
                        sum = "13";
                    }
                }
            }

            previousInterval = interval;

            ++i;
        }

        chordString = chordString + sum;
    }

    if (chordString.isEmpty())
    {
        return "-";
    }
    else if (chordString.length() < 3)
    {
        chordString += "maj";
    }

    return chordString;
}

juce::String FundamentalFrequencyComponent::getKeyNoteString(int keynote)
{
    if (keynote == 0)
    {
        return { "C" };
    }
    else if (keynote == 1)
    {
        return { "C#" };
    }
    else if (keynote == 2)
    {
        return { "D" };
    }
    else if (keynote == 3)
    {
        return { "D#" };
    }
    else if (keynote == 4)
    {
        return { "E" };
    }
    else if (keynote == 5)
    {
        return { "F" };
    }
    else if (keynote == 6)
    {
        return { "F#" };
    }
    if (keynote == 7)
    {
        return { "G" };
    }
    else if (keynote == 8)
    {
        return { "G#" };
    }
    else if (keynote == 9)
    {
        return { "A" };
    }
    else if (keynote == 10)
    {
        return { "A#" };
    }
    else if (keynote == 11)
    {
        return { "B" };
    }

    return {""};
}

void FundamentalFrequencyComponent::updateSettingsUIParameter(int settingIndex)
{
    if (settingIndex == TSS_SETTINGS::FUNDAMENTAL_FREQUENCY_ALGORITHM)
    {
        comboBoxAlgorithm->setSelectedItemIndex((int)projectManager.getProjectSettingsParameter(settingIndex));
    }
}
