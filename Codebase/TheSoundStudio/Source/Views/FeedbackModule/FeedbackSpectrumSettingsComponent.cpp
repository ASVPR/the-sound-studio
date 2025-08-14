#include "FeedbackSpectrumSettingsComponent.h"

FeedbackSpectrumSettingsComponent::FeedbackSpectrumSettingsComponent(ProjectManager &pm, FundamentalFrequencyProcessor& ffp) :
    projectManager(pm),
    frequencyProcessor(ffp)
{
    Image closeButtonImage = ImageCache::getFromMemory(BinaryData::CloseButton2x_png, BinaryData::CloseButton2x_pngSize);
    Image applyButtonImage = ImageCache::getFromMemory(BinaryData::ApplyButton2x_png, BinaryData::ApplyButton2x_pngSize);

    closeButton.setImages(false, true, true,
        closeButtonImage, 1.f, Colour{},
        Image{}, 1.f, Colour{},
        Image{}, 1.f, Colour{});
    closeButton.setMouseCursor(MouseCursor::PointingHandCursor);
    closeButton.addListener(this);
    addAndMakeVisible(closeButton);

    applyButton.setImages(false, true, true,
        applyButtonImage, 1.f, Colour{},
        Image{}, 1.f, Colour{},
        Image{}, 1.f, Colour{});
    applyButton.setMouseCursor(MouseCursor::PointingHandCursor);
    applyButton.addListener(this);
    addAndMakeVisible(applyButton);

    addAndMakeVisible(containers.createContainer("FFT Parameters", true));
    addAndMakeVisible(containers.createContainer("Threshold & Tolerance", true));
    addAndMakeVisible(containers.createContainer("Range", true));
    addAndMakeVisible(containers.createContainer("Process", true));

    projectManager.addUIListener(this);

    prepareFftParameters();
    prepareProcess();
    prepareRange();
    prepareThresholdAndTolerance();
}

FeedbackSpectrumSettingsComponent::~FeedbackSpectrumSettingsComponent()
{
    closeButton.removeListener(this);
    applyButton.removeListener(this);
    projectManager.removeUIListener(this);
}

void FeedbackSpectrumSettingsComponent::prepareFftParameters()
{
    auto doToBox = [this] (asvpr::ComboBox& c)
    {
        c.showLabel(true);
        c.addListener(this);
        c.setLookAndFeel(&lookAndFeel);
        c.getRootMenu()->setLookAndFeel(&lookAndFeel);
        containers.getContainer(0).addAndMakeVisible(c);
    };
    comboBoxFFTSize.addItem("2 ^ 10", 1024);
    comboBoxFFTSize.addItem("2 ^ 11", 2048);
    comboBoxFFTSize.addItem("2 ^ 12", 4096);
    comboBoxFFTSize.addItem("2 ^ 13", 8192);
    comboBoxFFTSize.addItem("2 ^ 14", 16384);
    comboBoxFFTSize.addItem("2 ^ 15", 32768);
    comboBoxFFTSize.addItem("2 ^ 16", 65536);
    comboBoxFFTSize.addItem("2 ^ 17", 131072);
    comboBoxFFTSize.addItem("2 ^ 18", 262144);
    comboBoxFFTSize.addItem("2 ^ 19", 524288);
    comboBoxFFTSize.addItem("2 ^ 20", 1048576);
    comboBoxFFTSize.setBounds(Bounds::get(Bounds::fftSizeCombobox));
    //comboBoxFFTSize.createAttachment(frequencyProcessor.getApvts(), FundamentalFrequencyProcessor::get_parameter_name(FundamentalFrequencyProcessor::Parameter::fft_size));
    doToBox(comboBoxFFTSize);

    comboBoxFFTWindow.addItem("Rectangular", 1);
    comboBoxFFTWindow.addItem("Triangular", 2);
    comboBoxFFTWindow.addItem("Hann", 3);
    comboBoxFFTWindow.addItem("Hamming", 4);
    comboBoxFFTWindow.addItem("Blackman", 5);
    comboBoxFFTWindow.addItem("BlackmanHarris", 6);
    comboBoxFFTWindow.addItem("Flat Top", 7);
    comboBoxFFTWindow.addItem("Kaiser", 8);
    comboBoxFFTWindow.setBounds(Bounds::get(Bounds::fftWindowCombobox));
    //comboBoxFFTWindow.createAttachment(frequencyProcessor.getApvts(), FundamentalFrequencyProcessor::get_parameter_name(FundamentalFrequencyProcessor::Parameter::fft_window));
    doToBox(comboBoxFFTWindow);

    comboBoxAlgorithm.addItemList(ProjectStrings::getFundamentalFrequencyAlgorithms(), 1);
    comboBoxAlgorithm.setBounds(Bounds::get(Bounds::fftAlgorithmCombobox));
    //comboBoxAlgorithm.createAttachment(frequencyProcessor.getApvts(), FundamentalFrequencyProcessor::get_parameter_name(FundamentalFrequencyProcessor::Parameter::algorithm));
    doToBox(comboBoxAlgorithm);

    for (auto harmonic = 1; harmonic <= 10; ++harmonic)
        comboBoxNumHarmonics.addItem(juce::String(harmonic), harmonic);
    comboBoxNumHarmonics.setBounds(Bounds::get(Bounds::fftNumHarmonicsCombobox));
    //comboBoxNumHarmonics->createAttachment(frequencyProcessor.getApvts(), FundamentalFrequencyProcessor::get_parameter_name(FundamentalFrequencyProcessor::Parameter::harmonics));
    doToBox(comboBoxNumHarmonics);

    containers.getContainer(0).paintInBackground = [this] (juce::Graphics& g, const auto& bounds) {};
}

void FeedbackSpectrumSettingsComponent::prepareThresholdAndTolerance()
{
    auto create = [this] (CustomRotarySlider& slider,
        double newMinimum,
        double newMaximum,
        double newInterval,
        const juce::Rectangle<int>& bounds,
        int decimals)
    {
        slider.setRange (newMinimum, newMaximum, newInterval);
        slider.setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
        slider.setNumDecimalPlacesToDisplay(decimals);
        slider.setLookAndFeel(&lookAndFeel);
        slider.setBounds(bounds);
        slider.setTextBoxStyle (Slider::TextBoxBelow, false, 78 * Bounds::scale, 28 * Bounds::scale);

        containers.getContainer(1).addAndMakeVisible(slider);
    };

    create(sliderThresholdInputDetection, -80, 6.0, 0, Bounds::get(Bounds::inputThreshold), 1);
    create(sliderKeynoteTolerance, -100, 100, 0, Bounds::get(Bounds::keynoteTolerance), 0);
    create(sliderMinIntervalSize, 0.05, 4.0, 0, Bounds::get(Bounds::minInterval), 3);
    create(sliderMaxIntervalSize, 0.05, 4.0, 0,Bounds::get(Bounds::maxInterval), 3);

    containers.getContainer(1).paintInBackground = [this] (Graphics& g, const Rectangle<int>& bounds)
    {
        g.setColour(juce::Colours::white);

        g.drawFittedText("Input Threshold",
            Bounds::get(Bounds::inputThreshold).translated(0, 20),
            juce::Justification::centredBottom, 1);

        g.drawFittedText("Keynote Tolerance",
            Bounds::get(Bounds::keynoteTolerance).translated(0, 20),
            juce::Justification::centredBottom, 1);

        g.drawFittedText("Min Interval (oct)",
            Bounds::get(Bounds::minInterval).translated(0, 20),
            juce::Justification::centredBottom, 1);

        g.drawFittedText("Max Interval (oct)",
            Bounds::get(Bounds::maxInterval).translated(0, 20),
            juce::Justification::centredBottom, 1);

    };
}

void FeedbackSpectrumSettingsComponent::prepareRange()
{
    auto imageBlueCheckButtonNormal = ImageCache::getFromMemory(BinaryData::Button_Checkbox_Normal_Max_png,
        BinaryData::Button_Checkbox_Normal_Max_pngSize);
    auto imageBlueCheckButtonSelected = ImageCache::getFromMemory(BinaryData::Button_Checkbox_Selected_Max_png,
        BinaryData::Button_Checkbox_Selected_Max_pngSize);


    buttonCustomFrequencyRange.setTriggeredOnMouseDown(true);
    buttonCustomFrequencyRange.setImages (false, true, true,
        imageBlueCheckButtonNormal, 0.999f, Colour (0x00000000),
        Image(), 1.000f, Colour (0x00000000),
        imageBlueCheckButtonSelected, 1.0, Colour (0x00000000));
    buttonCustomFrequencyRange.addListener(this);
    buttonCustomFrequencyRange.setLookAndFeel(&lookAndFeel);
    buttonCustomFrequencyRange.setBounds(Bounds::get(Bounds::customRangeBox));
    containers.getContainer(2).addAndMakeVisible(buttonCustomFrequencyRange);

    auto fontNormal = projectManager.getAssistantFont(ProjectManager::FontType::SemiBold).withHeight(20);
    textEditorMinFrequency.setReturnKeyStartsNewLine(false);
    textEditorMinFrequency.setInputRestrictions(6, "0123456789");
    textEditorMinFrequency.setMultiLine(false);
    textEditorMinFrequency.setText("1");
    textEditorMinFrequency.addListener(this);
    textEditorMinFrequency.setLookAndFeel(&lookAndFeel);
    textEditorMinFrequency.setJustification(Justification::centred);
    textEditorMinFrequency.setFont(fontNormal);
    textEditorMinFrequency.setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditorMinFrequency.applyFontToAllText(fontNormal);
    textEditorMinFrequency.applyColourToAllText(Colours::lightgrey);
    textEditorMinFrequency.setBounds(Bounds::get(Bounds::minFrequency));
    containers.getContainer(2).addAndMakeVisible(textEditorMinFrequency);

    textEditorMaxFrequency.setReturnKeyStartsNewLine(false);
    textEditorMaxFrequency.setInputRestrictions(6, "0123456789");
    textEditorMaxFrequency.setMultiLine(false);
    textEditorMaxFrequency.setText("1");
    textEditorMaxFrequency.addListener(this);
    textEditorMaxFrequency.setLookAndFeel(&lookAndFeel);
    textEditorMaxFrequency.setJustification(Justification::centred);
    textEditorMaxFrequency.setFont(fontNormal);
    textEditorMaxFrequency.setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditorMaxFrequency.applyFontToAllText(fontNormal);
    textEditorMaxFrequency.applyColourToAllText(Colours::lightgrey);
    textEditorMaxFrequency.setBounds(Bounds::get(Bounds::maxFrequency));
    containers.getContainer(2).addAndMakeVisible(textEditorMaxFrequency);

    containers.getContainer(2).paintInBackground = [this] (juce::Graphics& g, const juce::Rectangle<int>& bounds)
    {
        g.setColour(juce::Colours::white);
        auto b = buttonCustomFrequencyRange.getBounds().translated(10, 0).withWidth(100);

        g.drawFittedText("Custom Range",  Bounds::get(Bounds::customRangeText), juce::Justification::centredLeft, 1);

        b.translate(-25, 30);
        g.drawFittedText("Min Frequency",  Bounds::get(Bounds::minFrequency).translated(0, -30), juce::Justification::centredLeft, 1);

        b.translate(125, 0);
        g.drawFittedText("Max Frequency",  Bounds::get(Bounds::maxFrequency).translated(0, -30), juce::Justification::centredLeft, 1);
    };
}

void FeedbackSpectrumSettingsComponent::prepareProcess()
{
    auto imageBlueCheckButtonNormal = ImageCache::getFromMemory(BinaryData::Button_Checkbox_Normal_Max_png,
        BinaryData::Button_Checkbox_Normal_Max_pngSize);
    auto imageBlueCheckButtonSelected = ImageCache::getFromMemory(BinaryData::Button_Checkbox_Selected_Max_png,
        BinaryData::Button_Checkbox_Selected_Max_pngSize);

    buttonProcessFFT.setClickingTogglesState(true);
    buttonProcessFFT.setImages (false, true, true,
        imageBlueCheckButtonNormal, 0.999f, Colour (0x00000000),
        Image(), 1.000f, Colour (0x00000000),
        imageBlueCheckButtonSelected, 1.0, Colour (0x00000000));
    buttonProcessFFT.addListener(this);
    buttonProcessFFT.setLookAndFeel(&lookAndFeel);
    containers.getContainer(3).addAndMakeVisible(buttonProcessFFT);
    buttonProcessFFT.setBounds(Bounds::get(Bounds::processFftBox));

    auto fontSemiBold = projectManager.getAssistantFont(ProjectManager::FontType::SemiBold);

    textEditorIteration.setReturnKeyStartsNewLine(false);
    textEditorIteration.setMultiLine(false);
    textEditorIteration.setInputRestrictions(2, "0123456789");
    textEditorIteration.setText("3");
    textEditorIteration.addListener(this);
    textEditorIteration.setLookAndFeel(&lookAndFeel);
    textEditorIteration.setJustification(Justification::centred);
    textEditorIteration.setFont(fontSemiBold);
    textEditorIteration.setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditorIteration.applyFontToAllText(fontSemiBold);
    textEditorIteration.applyColourToAllText(Colours::lightgrey);
    textEditorIteration.setBounds(Bounds::get(Bounds::numIterations));
    containers.getContainer(3).addAndMakeVisible(textEditorIteration);

    textEditorLength.setReturnKeyStartsNewLine(false);
    textEditorLength.setMultiLine(false);
    textEditorLength.setInputRestrictions(5, "0123456789");
    textEditorLength.addListener(this);
    textEditorLength.setLookAndFeel(&lookAndFeel);
    textEditorLength.setJustification(Justification::centred);
    textEditorLength.setFont(fontSemiBold);
    textEditorLength.setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditorLength.applyFontToAllText(fontSemiBold);
    textEditorLength.applyColourToAllText(Colours::lightgrey);
    textEditorLength.setBounds(Bounds::get(Bounds::iteratioLength));
    containers.getContainer(3).addAndMakeVisible(textEditorLength);
    textEditorLength.setText(juce::String(frequencyProcessor.get_iteration_length()));

    containers.getContainer(3).paintInBackground = [this] (juce::Graphics& g, const juce::Rectangle<int>& bounds)
    {
        g.setColour(juce::Colours::white);
        g.drawFittedText("Process FFT",
            Bounds::get(Bounds::processFftText),
            juce::Justification::centredRight, 1);

        g.drawFittedText("Iterations",
            Bounds::get(Bounds::numIterationsText),
            juce::Justification::centredRight, 1);

        g.drawFittedText("Iteration length (ms)",
            Bounds::get(Bounds::iterationLengthText),
            juce::Justification::centredRight, 1);
    };
}

void FeedbackSpectrumSettingsComponent::comboBoxChanged(ComboBox *comboBoxThatHasChanged)
{

}

void FeedbackSpectrumSettingsComponent::buttonClicked(Button *button)
{
    if (button == &buttonProcessFFT)
    {
        frequencyProcessor.enable_fft_process(buttonProcessFFT.getToggleState());
    }
    else if (button == &buttonCustomFrequencyRange)
    {
        frequencyProcessor.enable_custom_range(!buttonCustomFrequencyRange.getToggleState());
    }
    else if (button == &applyButton)
    {
        setVisible(false);
    }
    else if (button == &closeButton)
    {
        setVisible(false);
    }
}

void FeedbackSpectrumSettingsComponent::textEditorReturnKeyPressed(TextEditor &editor)
{
    if (&editor == &textEditorLength)
    {
        auto opt = frequencyProcessor.set_iteration_length(editor.getText().getIntValue());
        if (opt.has_value())
        {
            editor.setText(juce::String(opt.value()));
        }
    }
    else if (&editor == &textEditorIteration)
    {
        frequencyProcessor.set_iterations(editor.getText().getIntValue());
    }
}

void FeedbackSpectrumSettingsComponent::paint(Graphics &g)
{
    g.setColour(Colours::black);
    g.setOpacity(0.88);
    g.fillAll();

    g.setOpacity(1.0);
    g.setColour(Colour::fromString("ff2c2c2c"));
    g.fillRoundedRectangle(Bounds::get(Bounds::container).toFloat(), 5);
}

void FeedbackSpectrumSettingsComponent::resized()
{
    applyButton.setBounds(Bounds::get(Bounds::applyButton));
    closeButton.setBounds(Bounds::get(Bounds::closeButton));
    applyButton.setBounds(Bounds::get(Bounds::applyButton));

    containers.setContainerBounds(0, Bounds::get(Bounds::fftParameters));
    containers.setContainerBounds(1, Bounds::get(Bounds::threshold));
    containers.setContainerBounds(2, Bounds::get(Bounds::range));
    containers.setContainerBounds(3, Bounds::get(Bounds::process));
}

void FeedbackSpectrumSettingsComponent::setScale(const float factor)
{
    Bounds::scale = factor;
    resized();
    repaint();
}

void FeedbackSpectrumSettingsComponent::updateSettingsUIParameter(int settingIndex)
{
    if (settingIndex == ASVPR_SETTINGS::FUNDAMENTAL_FREQUENCY_ALGORITHM)
        comboBoxAlgorithm.setSelectedItemIndex((int)projectManager.getProjectSettingsParameter(FUNDAMENTAL_FREQUENCY_ALGORITHM));
}
