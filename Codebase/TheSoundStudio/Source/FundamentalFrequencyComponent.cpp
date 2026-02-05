/*
  ==============================================================================

    FundamentalFrequencyComponent.cpp
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#include "FundamentalFrequencyComponent.h"
#include "ResponsiveUIHelper.h"
#include "UI/DesignSystem.h"


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
    
    // Standardize fonts using Design System
    const Font fontNormal = ProjectManager::getAssistantFont(ProjectManager::FontType::SemiBold).withHeight(33);
    const Font fontBold = ProjectManager::getAssistantFont(ProjectManager::FontType::SemiBold).withHeight(38);
    const Font fontLight = ProjectManager::getAssistantFont(ProjectManager::FontType::Light).withHeight(33);
    const Font fontSemiBold = ProjectManager::getAssistantFont(ProjectManager::FontType::SemiBold).withHeight(33);
    
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

    // IR Controls (in Process container)
    toggleIREnable = std::make_unique<ToggleButton>("Enable IR");
    toggleIREnable->onClick = [this]()
    {
        frequencyProcessor.setIREnabled(toggleIREnable->getToggleState());
    };
    containers.getContainer(3).addAndMakeVisible(*toggleIREnable);

    sliderIRWet = std::make_unique<Slider>(Slider::LinearHorizontal, Slider::TextBoxRight);
    sliderIRWet->setRange(0.0, 1.0, 0.01);
    sliderIRWet->onValueChange = [this]()
    {
        frequencyProcessor.setIRWet((float) sliderIRWet->getValue());
    };
    containers.getContainer(3).addAndMakeVisible(*sliderIRWet);

    buttonIRLoad = std::make_unique<TextButton>("Load IR (WAV)");
    buttonIRLoad->onClick = [this]()
    {
        FileChooser chooser("Select Impulse Response (WAV)", File{}, "*.wav");
        if (chooser.browseForFileToOpen())
        {
            auto file = chooser.getResult();
            auto res = frequencyProcessor.loadIRFromWav(file);
            if (!res.wasOk())
                AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "IR Load Failed", res.getErrorMessage());
        }
    };
    containers.getContainer(3).addAndMakeVisible(*buttonIRLoad);


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
    auto bounds = getLocalBounds();

    // Calculate scale based on available space
    const float layoutScale = jlimit(0.5f, 2.0f,
                                     jmin(bounds.getWidth() / 1600.0f,
                                          bounds.getHeight() / 1000.0f));

    lookAndFeel.setScale(layoutScale);
    visualiserSelectorComponent->setScale(layoutScale);

    const int padding = jmax(10, roundToInt(20 * layoutScale));
    const int gap = jmax(8, roundToInt(12 * layoutScale));

    auto contentBounds = bounds.reduced(padding);

    // Bottom button row
    const int buttonRowHeight = jmax(70, roundToInt(90 * layoutScale));
    auto buttonRow = contentBounds.removeFromBottom(buttonRowHeight);
    contentBounds.removeFromBottom(gap);

    // Visualiser at top (40% of content)
    int visualiserHeight = roundToInt(contentBounds.getHeight() * 0.40f);
    visualiserHeight = jmax(200, visualiserHeight);
    auto visualiserArea = contentBounds.removeFromTop(visualiserHeight);
    visualiserArea = visualiserArea.reduced(padding / 2);
    visualiserSelectorComponent->setBounds(visualiserArea);

    contentBounds.removeFromTop(gap);

    // Calculate row heights for containers - give each section adequate space
    const int totalRowsHeight = contentBounds.getHeight();
    const int row0Height = jmax(90, roundToInt(totalRowsHeight * 0.18f));   // FFT Parameters
    const int row1Height = jmax(160, roundToInt(totalRowsHeight * 0.32f));  // Threshold/Range side by side
    const int row2Height = jmax(130, roundToInt(totalRowsHeight * 0.26f));  // Process
    const int row3Height = jmax(90, roundToInt(totalRowsHeight * 0.18f));   // Harmonics

    // Row 0: FFT Parameters (full width)
    auto row0 = contentBounds.removeFromTop(row0Height);
    contentBounds.removeFromTop(gap);

    // Row 1: Threshold & Tolerance (left) + Range (right)
    auto row1 = contentBounds.removeFromTop(row1Height);
    contentBounds.removeFromTop(gap);

    // Row 2: Process (full width)
    auto row2 = contentBounds.removeFromTop(row2Height);
    contentBounds.removeFromTop(gap);

    // Row 3: Harmonics/Results (full width)
    auto row3 = contentBounds;

    // Set container bounds
    containers.setContainerBounds(0, row0);

    // Split row1 into left (Threshold) and right (Range)
    const int row1LeftWidth = roundToInt(row1.getWidth() * 0.65f);
    auto row1Left = row1.removeFromLeft(row1LeftWidth);
    row1.removeFromLeft(gap);
    auto row1Right = row1;
    containers.setContainerBounds(1, row1Left);
    containers.setContainerBounds(2, row1Right);

    containers.setContainerBounds(3, row2);
    containers.setContainerBounds(4, row3);

    // =========== Transport Buttons (bottom row) ===========
    const int playWidth = jmax(120, roundToInt(150 * layoutScale));
    const int playHeight = jmax(40, roundToInt(50 * layoutScale));
    const int stopWidth = jmax(90, roundToInt(110 * layoutScale));
    const int buttonSpacing = jmax(15, roundToInt(20 * layoutScale));

    auto centerX = buttonRow.getCentreX();
    auto playBounds = Rectangle<int>(centerX - (playWidth + stopWidth + buttonSpacing) / 2,
                                     buttonRow.getCentreY() - playHeight / 2,
                                     playWidth, playHeight);
    auto stopBounds = playBounds.withX(playBounds.getRight() + buttonSpacing).withWidth(stopWidth);
    stopBounds = stopBounds.withHeight(playHeight);

    buttonStart->setBounds(playBounds);
    buttonStop->setBounds(stopBounds);

    const int noiseSize = jmax(60, roundToInt(80 * layoutScale));
    auto noiseBounds = buttonRow.removeFromRight(noiseSize + padding);
    noiseBounds = noiseBounds.withSizeKeepingCentre(noiseSize, noiseSize);
    noiseButton->setBounds(noiseBounds);

    // =========== Container 0: FFT Parameters ===========
    auto cont0 = containers.getContainer(0).getLocalBounds().reduced(10);
    const int comboHeight = jmax(36, roundToInt(42 * layoutScale));
    const int comboTop = jmax(30, roundToInt(38 * layoutScale));
    const int comboGap = jmax(8, roundToInt(12 * layoutScale));
    const int numCombos = 5;
    int comboWidth = (cont0.getWidth() - (numCombos - 1) * comboGap - 20) / numCombos;
    comboWidth = jmax(100, comboWidth);

    int comboX = 10;
    comboBoxFFTSize->setBounds(comboX, comboTop, comboWidth, comboHeight);
    comboX += comboWidth + comboGap;
    comboBoxFFTWindow->setBounds(comboX, comboTop, comboWidth, comboHeight);
    comboX += comboWidth + comboGap;
    comboBoxInput->setBounds(comboX, comboTop, comboWidth, comboHeight);
    comboX += comboWidth + comboGap;
    comboBoxAlgorithm->setBounds(comboX, comboTop, comboWidth, comboHeight);
    comboX += comboWidth + comboGap;
    comboBoxNumHarmonics->setBounds(comboX, comboTop, comboWidth, comboHeight);

    // =========== Container 1: Threshold & Tolerance (4 units) ===========
    // Layout: Gauge (rotary) at top, TextBox below gauge, Label at bottom
    auto cont1 = containers.getContainer(1).getLocalBounds().reduced(10);
    const int labelReservedHeight = jmax(22, roundToInt(25 * layoutScale)); // Space for label at bottom
    const int availableSliderHeight = cont1.getHeight() - labelReservedHeight - 15; // Leave margin
    const int unitWidth = jmax(70, roundToInt(80 * layoutScale));
    const int unitHeight = jmax(100, availableSliderHeight); // Slider height (gauge + textbox)
    const int unitGap = jmax(15, roundToInt(30 * layoutScale));
    const int numUnits = 4;
    int totalUnitsWidth = numUnits * unitWidth + (numUnits - 1) * unitGap;
    int startX = (cont1.getWidth() - totalUnitsWidth) / 2;
    const int unitTop = 5; // Small top margin

    sliderThresholdInputDetection->setBounds(startX, unitTop, unitWidth, unitHeight);
    sliderKeynoteTolerance->setBounds(startX + (unitWidth + unitGap), unitTop, unitWidth, unitHeight);
    sliderMinIntervalSize->setBounds(startX + 2 * (unitWidth + unitGap), unitTop, unitWidth, unitHeight);
    sliderMaxIntervalSize->setBounds(startX + 3 * (unitWidth + unitGap), unitTop, unitWidth, unitHeight);

    // =========== Container 2: Range ===========
    auto cont2 = containers.getContainer(2).getLocalBounds().reduced(10);
    const int rangeEditorHeight = jmax(26, roundToInt(32 * layoutScale));
    const int rangeEditorWidth = jmin(cont2.getWidth() / 2 - 20, jmax(85, roundToInt(100 * layoutScale)));
    const int rangeCheckSize = jmax(16, roundToInt(20 * layoutScale));
    const int rangeGap = jmax(8, roundToInt(10 * layoutScale));

    // Checkbox for custom range - positioned near top
    buttonCustomFrequencyRange->setBounds(15, 28, rangeCheckSize, rangeCheckSize);

    // Min/Max frequency editors - centered horizontally, positioned in lower half
    const int editorsY = cont2.getHeight() - rangeEditorHeight - 35; // Leave space for labels above
    const int totalEditorsWidth = 2 * rangeEditorWidth + rangeGap;
    const int editorsStartX = (cont2.getWidth() - totalEditorsWidth) / 2;

    textEditorMinFrequency->setBounds(editorsStartX, editorsY, rangeEditorWidth, rangeEditorHeight);
    textEditorMaxFrequency->setBounds(editorsStartX + rangeEditorWidth + rangeGap, editorsY, rangeEditorWidth, rangeEditorHeight);

    // =========== Container 3: Process ===========
    // Layout: Two rows - top row for controls, bottom row for sliders/presets
    auto cont3 = containers.getContainer(3).getLocalBounds().reduced(10);
    const int procEditorWidth = jmax(65, roundToInt(80 * layoutScale));
    const int procEditorHeight = jmax(24, roundToInt(28 * layoutScale));
    const int procButtonWidth = jmax(70, roundToInt(85 * layoutScale));
    const int procButtonHeight = jmax(26, roundToInt(32 * layoutScale));
    const int procToggleWidth = jmax(90, roundToInt(100 * layoutScale));
    const int procGap = jmax(8, roundToInt(12 * layoutScale));

    // Calculate column positions based on container width
    const int colWidth = (cont3.getWidth() - 40) / 5; // 5 logical columns

    // Top Row Y position (below label area)
    const int topY3 = 32;

    // Column 0-1: IR controls (left section)
    int irX = 15;
    toggleIREnable->setBounds(irX, topY3, procToggleWidth, procEditorHeight);
    buttonIRLoad->setBounds(irX + procToggleWidth + procGap, topY3, jmax(100, roundToInt(115 * layoutScale)), procButtonHeight);

    // Column 2: Iterations (center-left)
    const int iterX = colWidth * 2;
    textEditorIteration->setBounds(iterX, topY3, procEditorWidth, procEditorHeight);

    // Column 3: Iteration length (center-right)
    const int lengthX = colWidth * 3;
    textEditorLength->setBounds(lengthX, topY3, procEditorWidth + 15, procEditorHeight);

    // Column 4: Auto A (right section)
    const int autoAX = cont3.getWidth() - jmax(200, roundToInt(210 * layoutScale)) - 10;
    toggleAutoA->setBounds(autoAX, topY3, jmax(190, roundToInt(200 * layoutScale)), procEditorHeight);

    // Bottom Row Y position
    const int bottomY3 = cont3.getHeight() - procButtonHeight - 10;

    // Left: IR wet slider
    sliderIRWet->setBounds(15, bottomY3, jmax(220, roundToInt(250 * layoutScale)), 25);

    // Right: Range preset buttons (FULL, LOW, VOCAL) - properly spaced
    const int totalPresetsWidth = 3 * procButtonWidth + 2 * procGap;
    const int presetsX = cont3.getWidth() - totalPresetsWidth - 15;
    buttonRangeFull->setBounds(presetsX, bottomY3, procButtonWidth, procButtonHeight);
    buttonRangeLow->setBounds(presetsX + procButtonWidth + procGap, bottomY3, procButtonWidth, procButtonHeight);
    buttonRangeVocal->setBounds(presetsX + 2 * (procButtonWidth + procGap), bottomY3, procButtonWidth, procButtonHeight);

    // =========== Container 4: Harmonics/Results ===========
    auto cont4 = containers.getContainer(4).getLocalBounds().reduced(10);
    const float resultFontSize = ResponsiveUIHelper::getReadableFontSize(
        24.0f, layoutScale, TSS::Design::Usability::toolbarLabelMinFont);
    const auto resultFont = projectManager.getAssistantFont(ProjectManager::FontType::SemiBold)
                                .withHeight(resultFontSize);

    labelFrequency->setFont(resultFont);
    labelChord->setFont(resultFont);
    labelFrequency->setBounds(10, 10, 120, 35);
    labelChord->setBounds(10, 45, 120, 35);

    // Harmonic labels positioned horizontally
    int harmonicX = 150;
    int harmonicW = 100;
    int harmonicH = 28;
    for (int i = 0; i < 5; ++i)
    {
        labelHarmonic[i].setBounds(harmonicX + i * (harmonicW + 8), 10, harmonicW, harmonicH);
    }
    for (int i = 5; i < 10; ++i)
    {
        labelHarmonic[i].setBounds(harmonicX + (i - 5) * (harmonicW + 8), 40, harmonicW, harmonicH);
    }

    // Update fonts for text editors
    const auto editorFont = projectManager.getAssistantFont(ProjectManager::FontType::SemiBold)
                                .withHeight(ResponsiveUIHelper::getReadableFontSize(
                                    18.0f, layoutScale, TSS::Design::Usability::toolbarLabelMinFont));
    textEditorMinFrequency->setFont(editorFont);
    textEditorMaxFrequency->setFont(editorFont);
    textEditorIteration->setFont(editorFont);
    textEditorLength->setFont(editorFont);
    textEditorMinFrequency->applyFontToAllText(editorFont);
    textEditorMaxFrequency->applyFontToAllText(editorFont);
    textEditorIteration->applyFontToAllText(editorFont);
    textEditorLength->applyFontToAllText(editorFont);
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
        // Start analyser regardless of Process FFT checkbox state
        // This fixes the bug where Play button appeared broken when checkbox was off
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
        slider->setNumDecimalPlacesToDisplay(decimals);
        slider->setLookAndFeel(&lookAndFeel);
        slider->setBounds(bounds);
        
        // TextBox at bottom of unit (managed by Slider component height)
        slider->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);

        containers.getContainer(1).addAndMakeVisible(*slider);
    };


    // Initial bounds - will be properly set in resized()
    create(sliderThresholdInputDetection, -80, 6.0, 0, {10, 5, 80, 120}, 1);
    create(sliderKeynoteTolerance, -100, 100, 0, {100, 5, 80, 120}, 0);
    create(sliderMinIntervalSize, 0.05, 4.0, 0, {190, 5, 80, 120}, 3);
    create(sliderMaxIntervalSize, 0.05, 4.0, 0, {280, 5, 80, 120}, 3);


    containers.getContainer(1).paintInBackground = [this] (juce::Graphics& g, const juce::Rectangle<int>& bounds)
    {
        // Labels are drawn BELOW the textboxes (which are below the rotary gauges)
        // Label Y is at the very bottom of the container
        const int labelHeight = 18;
        const int labelY = bounds.getHeight() - labelHeight - 5; // 5px bottom margin
        const int labelWidth = 95;

        g.setColour(juce::Colours::white);
        const Font labelFont = ProjectManager::getAssistantFont(ProjectManager::FontType::SemiBold).withHeight(11);
        g.setFont(labelFont);

        // Match the dynamic layout calculation from resized()
        const int unitWidth = 80;
        const int unitGap = 30;
        const int numUnits = 4;
        const int totalWidth = numUnits * unitWidth + (numUnits - 1) * unitGap;
        const int startX = (bounds.getWidth() - totalWidth) / 2;

        // Center labels under each slider unit
        const int labelOffset = (unitWidth - labelWidth) / 2;

        g.drawFittedText("Input Threshold",
                         juce::Rectangle<int>(startX + labelOffset, labelY, labelWidth, labelHeight),
                         juce::Justification::centred, 1);

        g.drawFittedText("Keynote Tol.",
                         juce::Rectangle<int>(startX + (unitWidth + unitGap) + labelOffset, labelY, labelWidth, labelHeight),
                         juce::Justification::centred, 1);

        g.drawFittedText("Min Interval",
                         juce::Rectangle<int>(startX + 2 * (unitWidth + unitGap) + labelOffset, labelY, labelWidth, labelHeight),
                         juce::Justification::centred, 1);

        g.drawFittedText("Max Interval",
                         juce::Rectangle<int>(startX + 3 * (unitWidth + unitGap) + labelOffset, labelY, labelWidth, labelHeight),
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

    buttonCustomFrequencyRange->setBounds(30, 35, 15, 15);

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
    textEditorMinFrequency->setBounds(30, 110, 110, 30);
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
    textEditorMaxFrequency->setBounds(150, 110, 110, 30);
    containers.getContainer(2).addAndMakeVisible(textEditorMaxFrequency.get());

    containers.getContainer(2).paintInBackground = [this] (juce::Graphics& g, const juce::Rectangle<int>& bounds)
    {
        g.setColour(juce::Colours::white);
        const Font labelFont = ProjectManager::getAssistantFont(ProjectManager::FontType::SemiBold).withHeight(12);
        g.setFont(labelFont);

        // "Custom Range" label next to checkbox
        auto checkboxBounds = buttonCustomFrequencyRange->getBounds();
        g.drawFittedText("Custom Range",
                         checkboxBounds.translated(checkboxBounds.getWidth() + 5, 0).withWidth(100),
                         juce::Justification::centredLeft, 1);

        // Get positions from the text editors
        auto minBounds = textEditorMinFrequency->getBounds();
        auto maxBounds = textEditorMaxFrequency->getBounds();

        // Labels ABOVE the text editors
        const int labelY = minBounds.getY() - 20;
        g.drawFittedText("Min Frequency",
                         juce::Rectangle<int>(minBounds.getX(), labelY, minBounds.getWidth(), 18),
                         juce::Justification::centred, 1);
        g.drawFittedText("Max Frequency",
                         juce::Rectangle<int>(maxBounds.getX(), labelY, maxBounds.getWidth(), 18),
                         juce::Justification::centred, 1);
    };
}

void FundamentalFrequencyComponent::prepare_process_and_synth()
{
    // FFT processing is always enabled - no toggle needed
    frequencyProcessor.enable_fft_process(true);

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

    // Range preset selector (under Iteration length)
    buttonRangeFull = std::make_unique<TextButton>("Full");
    buttonRangeFull->onClick = [this]()
    {
        frequencyProcessor.enable_custom_range(true);
        frequencyProcessor.setCustomRangeMinHz(20);
        frequencyProcessor.setCustomRangeMaxHz(20000);
        textEditorMinFrequency->setText("20", juce::dontSendNotification);
        textEditorMaxFrequency->setText("20000", juce::dontSendNotification);
    };
    containers.getContainer(3).addAndMakeVisible(*buttonRangeFull);

    buttonRangeLow = std::make_unique<TextButton>("Low");
    buttonRangeLow->onClick = [this]()
    {
        frequencyProcessor.enable_custom_range(true);
        frequencyProcessor.setCustomRangeMinHz(20);
        frequencyProcessor.setCustomRangeMaxHz(500);
        textEditorMinFrequency->setText("20", juce::dontSendNotification);
        textEditorMaxFrequency->setText("500", juce::dontSendNotification);
    };
    containers.getContainer(3).addAndMakeVisible(*buttonRangeLow);

    buttonRangeVocal = std::make_unique<TextButton>("Vocal");
    buttonRangeVocal->onClick = [this]()
    {
        frequencyProcessor.enable_custom_range(true);
        frequencyProcessor.setCustomRangeMinHz(80);
        frequencyProcessor.setCustomRangeMaxHz(1000);
        textEditorMinFrequency->setText("80", juce::dontSendNotification);
        textEditorMaxFrequency->setText("1000", juce::dontSendNotification);
    };
    containers.getContainer(3).addAndMakeVisible(*buttonRangeVocal);

    // Auto A toggle (set A4 from input)
    toggleAutoA = std::make_unique<ToggleButton>("Auto A (A4 from input)");
    toggleAutoA->onClick = [this]()
    {
        autoAEnabled = toggleAutoA->getToggleState();
        autoAConsistencyCount = 0;
        autoALastKeynote = -1;
    };
    containers.getContainer(3).addAndMakeVisible(*toggleAutoA);
    toggleAutoA->setBounds(700, 35, 220, 24);

    containers.getContainer(3).paintInBackground = [this] (juce::Graphics& g, const juce::Rectangle<int>& bounds)
    {
        g.setColour(juce::Colours::white);
        const Font labelFont = ProjectManager::getAssistantFont(ProjectManager::FontType::SemiBold).withHeight(12);
        g.setFont(labelFont);

        // Calculate column positions to match resized() layout
        const int colWidth = (bounds.getWidth() - 40) / 5;
        const int labelY = 10;
        const int labelH = 18;

        // IR section label (left)
        g.drawFittedText("Impulse Response",
                         juce::Rectangle<int>(15, labelY, 130, labelH),
                         juce::Justification::centredLeft, 1);

        // Iterations label (center-left)
        g.drawFittedText("Iterations",
                         juce::Rectangle<int>(colWidth * 2, labelY, 80, labelH),
                         juce::Justification::centred, 1);

        // Iteration length label (center-right)
        g.drawFittedText("Length (ms)",
                         juce::Rectangle<int>(colWidth * 3, labelY, 95, labelH),
                         juce::Justification::centred, 1);

        // Tuning/Auto A label (right)
        g.drawFittedText("Tuning",
                         juce::Rectangle<int>(bounds.getWidth() - 210, labelY, 200, labelH),
                         juce::Justification::centred, 1);
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
