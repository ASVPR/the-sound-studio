/*
  ==============================================================================

    FrequencyToLightComponent.cpp
    Created: 13 Mar 2019 9:55:50pm
    Author:  Gary Jones

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "FrequencyToLightComponent.h"

//==============================================================================

FrequencyToLightComponent::FrequencyToLightComponent(ProjectManager * pm)
{
    projectManager = pm;
    projectManager->addUIListener(this);
    
    frequencyManager = projectManager->frequencyManager.get();
    
    chordManager = new ChordManager(frequencyManager);
    chordManager->setOctave(4);
    chordManager->setKeyNote(KEYNOTES::KEY_C);
    chordManager->setChordType(CHORD_TYPES::Major);

    
    // Font
    Typeface::Ptr AssistantLight        = Typeface::createSystemTypefaceFor(BinaryData::AssistantLight_ttf, BinaryData::AssistantLight_ttfSize);
    Typeface::Ptr AssistantSemiBold     = Typeface::createSystemTypefaceFor(BinaryData::AssistantSemiBold_ttf, BinaryData::AssistantSemiBold_ttfSize);
    Font fontSemiBold(AssistantSemiBold);
    Font fontLight(AssistantLight);
    
    fontSemiBold.setHeight(33);
    fontLight.setHeight(33);
    
    
    imageMainBackground             = ImageCache::getFromMemory(BinaryData::FrequencyToLightBackground2_png, BinaryData::FrequencyToLightBackground2_pngSize);
    
    imageStopButton                 = ImageCache::getFromMemory(BinaryData::Button_Stop_png, BinaryData::Button_Stop_pngSize);
    imageStartButton                = ImageCache::getFromMemory(BinaryData::Button_Start_png, BinaryData::Button_Start_pngSize);
    
    imageBlueButtonNormal           = ImageCache::getFromMemory(BinaryData::BlueButton_Normal_png, BinaryData::BlueButton_Normal_pngSize);
    imageBlueButtonSelected         = ImageCache::getFromMemory(BinaryData::BlueButton_Selected_png, BinaryData::BlueButton_Selected_pngSize);
    
    imageBlueCheckButtonNormal      = ImageCache::getFromMemory(BinaryData::Button_Checkbox_Normal_Max_png, BinaryData::Button_Checkbox_Normal_Max_pngSize);
    imageBlueCheckButtonSelected    = ImageCache::getFromMemory(BinaryData::Button_Checkbox_Selected_Max_png, BinaryData::Button_Checkbox_Selected_Max_pngSize);
    
    button_ChordConversion = new ImageButton();
    button_ChordConversion->setTriggeredOnMouseDown(true);
    button_ChordConversion->setImages (false, true, true,
                                imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_ChordConversion->addListener(this);
    button_ChordConversion->setBounds(323, 106, 31, 31);
    addAndMakeVisible(button_ChordConversion);
    
    button_FrequencyConversion = new ImageButton();
    button_FrequencyConversion->setTriggeredOnMouseDown(true);
    button_FrequencyConversion->setImages (false, true, true,
                                imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_FrequencyConversion->addListener(this);
    button_FrequencyConversion->setBounds(835, 106, 31, 31);
    addAndMakeVisible(button_FrequencyConversion);
    
    component_ColourOutputMain = new ColourOutputComponent();
    component_ColourOutputMain->setBounds(70, 453, 1418, 112);
    component_ColourOutputMain->setBackgroundColour(Colours::green);
    addAndMakeVisible(component_ColourOutputMain);
    
    label_ColourCode = new Label("","some colour code");
    addAndMakeVisible(label_ColourCode);
    label_ColourCode->setBounds(70,453,1418, 112);
    label_ColourCode->setJustificationType(Justification::centred);
    

    int labelWaveLengthX    = 369;
    int labelFrequencyX     = 726;
    int labelWaveLengthY[5] = { 955, 1079, 1155, 1232, 1307 };
    int roundedColourX      = 78;
    
    int shortcutWidth       = 180;
    int shortcutHeight      = 140;
    int shortcutX           = 70;
    int shortcutY           = 618;
    int shortcutSpace       = 126;
    
    for (int i = 0; i < 5; i++)
    {
        shortcutComponent[i] = new ShortcutColourComponent();
        shortcutComponent[i]->setBounds(shortcutX + ((shortcutWidth + shortcutSpace) * i), shortcutY, shortcutWidth, shortcutHeight);
        shortcutComponent[i]->button_Add->addListener(this);
        shortcutComponent[i]->button_OpenSettings->addListener(this);
        shortcutComponent[i]->button_Delete->addListener(this);
        addAndMakeVisible(shortcutComponent[i]);

        
        labelWavelength[i] = new Label();
        labelWavelength[i]->setText("1.0", dontSendNotification);
        labelWavelength[i]->setBounds(labelWaveLengthX, labelWaveLengthY[i], 233, 51);
        labelWavelength[i]->setJustificationType(Justification::centred);
        fontLight.setHeight(33);
        labelWavelength[i]->setFont(fontLight);
        labelWavelength[i]->setColour(Label::textColourId, Colours::darkgrey);  
        addAndMakeVisible(labelWavelength[i]);
        
        labelFrequency[i] = new Label();
        labelFrequency[i]->setText("1.0", dontSendNotification);
        labelFrequency[i]->setBounds(labelFrequencyX, labelWaveLengthY[i], 800, 51);
        labelFrequency[i]->setJustificationType(Justification::centred);
        fontLight.setHeight(33);
        labelFrequency[i]->setFont(fontLight);
        labelFrequency[i]->setColour(Label::textColourId, Colours::darkgrey);
        addAndMakeVisible(labelFrequency[i]);
        
        roundedColourOutput[i] = new RoundedColourOutputComponent();
        roundedColourOutput[i]->setBounds(roundedColourX, labelWaveLengthY[i]+10, 130, 24);
        addAndMakeVisible(roundedColourOutput[i]);
    }

    ScalesManager * sm = frequencyManager->scalesManager;
    
    // Combobox
    comboBoxKeynote     = new ComboBox();
    comboBoxKeynote->addListener(this);
    comboBoxKeynote->setBounds(420, 191, 149, 35);
    comboBoxKeynote->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(comboBoxKeynote);
    sm->getComboBoxPopupMenuForKeynotes(*comboBoxKeynote->getRootMenu(), SCALES_UNIT::MAIN_SCALE);

    comboBoxChordtype= new ComboBox();
    comboBoxChordtype->addListener(this);
    comboBoxChordtype->setBounds(420, 257, 149, 35);
    comboBoxChordtype->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(comboBoxChordtype);
    sm->getComboBoxPopupMenuForChords(*comboBoxChordtype->getRootMenu(), SCALES_UNIT::MAIN_SCALE);
    
    int lX = 1166;
    int lw = 104;
    
    comboBox_FrequencySuffix= new ComboBox();
    comboBox_FrequencySuffix->addItemList(StringArray("Hz", "KHz", "MHz", "GHz", "THz"), 1);
    comboBox_FrequencySuffix->addListener(this);
    comboBox_FrequencySuffix->setBounds(lX, 155, lw, 35);
    comboBox_FrequencySuffix->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(comboBox_FrequencySuffix);
    
    comboBox_WavelgnthSuffix = new ComboBox();
    comboBox_WavelgnthSuffix->addItemList(StringArray("nm", "mcm", "mm", "cm", "m"), 1);
    comboBox_WavelgnthSuffix->addListener(this);
    comboBox_WavelgnthSuffix->setBounds(lX, 220, lw, 35);
    comboBox_WavelgnthSuffix->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(comboBox_WavelgnthSuffix);
    
    comboBox_PhaseSpeed = new ComboBox();
    comboBox_PhaseSpeed->addItemList(StringArray("Light | Vacuum", "Light | Water", "Sound | Air", "Sound | Water"), 1);
    comboBox_PhaseSpeed->addListener(this);
    comboBox_PhaseSpeed->setBounds(1000, 285, 220, 35);
    comboBox_PhaseSpeed->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(comboBox_PhaseSpeed);
    
    textEditor_Frequency = new TextEditor("");
    textEditor_Frequency->setReturnKeyStartsNewLine(false);
    textEditor_Frequency->setInputRestrictions(10, "0123456789.");
    textEditor_Frequency->setMultiLine(false);
    textEditor_Frequency->setText("1");
    textEditor_Frequency->addListener(this);
    textEditor_Frequency->setLookAndFeel(&lookAndFeel);
    textEditor_Frequency->setJustification(Justification::centred);
    textEditor_Frequency->setFont(fontSemiBold);
    textEditor_Frequency->applyFontToAllText(fontSemiBold);
    textEditor_Frequency->applyColourToAllText(Colours::darkgrey);
    textEditor_Frequency->setBounds(1011, 159, 136, 35);
    addAndMakeVisible(textEditor_Frequency);
    
    
    textEditor_Wavelength = new TextEditor("");
    textEditor_Wavelength->setReturnKeyStartsNewLine(false);
    textEditor_Wavelength->setInputRestrictions(10, "0123456789.");
    textEditor_Wavelength->setMultiLine(false);
    textEditor_Wavelength->setText("1");
    textEditor_Wavelength->addListener(this);
    textEditor_Wavelength->setLookAndFeel(&lookAndFeel);
    textEditor_Wavelength->setJustification(Justification::centred);
    textEditor_Wavelength->setFont(fontSemiBold);
    textEditor_Wavelength->applyFontToAllText(fontSemiBold);
    textEditor_Wavelength->applyColourToAllText(Colours::darkgrey);
    textEditor_Wavelength->setBounds(1011, 221, 136, 35);
    addAndMakeVisible(textEditor_Wavelength);
    
    
    
    for (int i = 0; i < 5; i++)
    {
        manipulationPopupComponent[i] = new ManipulationPopupComponent();
        manipulationPopupComponent[i]->setBounds(0, 0, 1566, 1440);
        addAndMakeVisible(manipulationPopupComponent[i]);
        manipulationPopupComponent[i]->setVisible(false);
        
        manipulationPopupComponent[i]->button_Multiplication->addListener(this);
        manipulationPopupComponent[i]->button_Division->addListener(this);
        manipulationPopupComponent[i]->button_Close->addListener(this);
        manipulationPopupComponent[i]->button_Save->addListener(this);
        manipulationPopupComponent[i]->textEditorMultiplication->addListener(this);
        manipulationPopupComponent[i]->textEditorDivision->addListener(this);
        
        
    }
    
    // Colour Slider
    sliderColour = new Slider("");
    sliderColour->addListener(this);
    sliderColour->setSliderStyle(Slider::LinearHorizontal);
    sliderColour->setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    sliderColour->setRange(0.f, 1.f);
    sliderColour->setBounds(66, 882, 1430, 60);
    addAndMakeVisible(sliderColour);
    
    
    // internal params
    initParameters();
    calculateCoefficientsFromWavelength();
    setControls();
    
    // set Colur Slider default
    calculateValuesFromColourSlider(0.f);
}

FrequencyToLightComponent::~FrequencyToLightComponent() { }

void FrequencyToLightComponent::resized()
{
    button_ChordConversion->setBounds(323 * scaleFactor, 106 * scaleFactor, 31 * scaleFactor, 31 * scaleFactor);
    button_FrequencyConversion->setBounds(835 * scaleFactor, 106 * scaleFactor, 31 * scaleFactor, 31 * scaleFactor);
    
    component_ColourOutputMain->setBounds(70 * scaleFactor, 453 * scaleFactor, 1418 * scaleFactor, 112 * scaleFactor);
    
    label_ColourCode->setBounds(70 * scaleFactor,453 * scaleFactor,1418 * scaleFactor, 112 * scaleFactor);
    
    int labelWaveLengthX    = 369;
    int labelFrequencyX     = 726;
    int labelWaveLengthY[5] = { 955, 1079, 1155, 1232, 1307 };
    int roundedColourX      = 78;
    
    int shortcutWidth       = 180;
    int shortcutHeight      = 140;
    int shortcutX           = 70;
    int shortcutY           = 618;
    int shortcutSpace       = 126;
    
    for (int i = 0; i < 5; i++)
    {
        shortcutComponent[i]->setBounds((shortcutX + ((shortcutWidth + shortcutSpace) * i)) * scaleFactor, shortcutY * scaleFactor, shortcutWidth * scaleFactor, shortcutHeight * scaleFactor);
        labelWavelength[i]->setBounds(labelWaveLengthX * scaleFactor, labelWaveLengthY[i] * scaleFactor, 233 * scaleFactor, 51 * scaleFactor);
        labelWavelength[i]->setFont(33 * scaleFactor);
        
        labelFrequency[i]->setBounds(labelFrequencyX * scaleFactor, labelWaveLengthY[i] * scaleFactor, 800 * scaleFactor, 51 * scaleFactor);
        labelFrequency[i]->setFont(33 * scaleFactor);
        
        roundedColourOutput[i]->setBounds(roundedColourX * scaleFactor, (labelWaveLengthY[i]+10) * scaleFactor, 130 * scaleFactor, 24 * scaleFactor);
    }
    
    comboBoxKeynote->setBounds(420 * scaleFactor, 191 * scaleFactor, 149 * scaleFactor, 35 * scaleFactor);
    comboBoxChordtype->setBounds(420 * scaleFactor, 257 * scaleFactor, 149 * scaleFactor, 35 * scaleFactor);
    
    int lX = 1166;
    int lw = 104;
    
    comboBox_FrequencySuffix->setBounds(lX * scaleFactor, 155 * scaleFactor, lw * scaleFactor, 35 * scaleFactor);
    comboBox_WavelgnthSuffix->setBounds(lX * scaleFactor, 220 * scaleFactor, lw * scaleFactor, 35 * scaleFactor);
    comboBox_PhaseSpeed->setBounds(1000 * scaleFactor, 285 * scaleFactor, 220 * scaleFactor, 35 * scaleFactor);
    
    textEditor_Frequency->setBounds(1011 * scaleFactor, 159 * scaleFactor, 136 * scaleFactor, 35 * scaleFactor);
    textEditor_Frequency->setFont(30 * scaleFactor);
    textEditor_Frequency->applyFontToAllText(30 * scaleFactor);
    
    textEditor_Wavelength->setBounds(1011 * scaleFactor, 221 * scaleFactor, 136 * scaleFactor, 35 * scaleFactor);
    textEditor_Wavelength->setFont(30 * scaleFactor);
    textEditor_Wavelength->applyFontToAllText(30 * scaleFactor);
    
    
    for (int i = 0; i < 5; i++)
    {
        manipulationPopupComponent[i]->setBounds(0, 0, 1566 * scaleFactor, 1440 * scaleFactor);
    }
    
    sliderColour->setBounds(66 * scaleFactor, 882 * scaleFactor, 1430 * scaleFactor, 60 * scaleFactor);
    
}


void FrequencyToLightComponent::initParameters()
{
    // chord
    conversionSource        = true;
    keynote                 = KEYNOTES::KEY_C;
    chordType               = CHORD_TYPES::Major;
    chordManager            ->setKeyNote((enum KEYNOTES) keynote);
    chordManager            ->setChordType((enum CHORD_TYPES) chordType);

    frequencyInHz           = 333.3;
    wavelengthInNm          = 444;
    currentFrequencyUnit    = FREQUENCY_UNITS::HZ;
    currentWaveLengthUnit   = WAVELENGTH_UNITS::NANOMETER; // nm etc
    phaseSpeedUnit          = PHASE_SPEED_UNITS::SPEED_LIGHT_IN_VACUUM;

    mainOutputColour        = Colours::black;
    
    // manipulation
    for (int i = 0; i < 5; i++)
    {
        manipulateChosenFrequency[i]    = false; // bool
        multOrDivide[i]                 = false;
        multiplyValue[i]                = 1.f;
        divisionValue[i]                = 1.f;
        shortcutFrequency[i]            = frequencyInHz;
        shortcutWaveLength[i]           = wavelengthInNm;
        manipulatedColour[i]            = Colours::black;
    }
    
    // colur Slider
}

void FrequencyToLightComponent::setControls()
{
    button_ChordConversion      ->setToggleState(!conversionSource, dontSendNotification);
    button_FrequencyConversion  ->setToggleState(conversionSource, dontSendNotification);
    comboBoxKeynote             ->setSelectedId(keynote);
    comboBoxChordtype           ->setSelectedId(chordType);
    
    textEditor_Frequency        ->setText(getFrequencyStringForCurrentUnit());
    textEditor_Wavelength       ->setText(getWavelengthStringForCurrentUnit());
    comboBox_PhaseSpeed         ->setSelectedId(phaseSpeedUnit);
    comboBox_FrequencySuffix    ->setSelectedId((int)currentFrequencyUnit);
    comboBox_WavelgnthSuffix    ->setSelectedId((int)currentWaveLengthUnit);
    
    component_ColourOutputMain  ->setBackgroundColour(mainOutputColour);
    
    String t;
    
    if (conversionSource)
    {
        t.append(String(frequencyInHz * THZ_FACTOR, 3, false), 20);

    }
    else{
        t.append(String(chordBaseFreq * THZ_FACTOR, 3, false), 20);
    }
    
    t.append(" THz / ", 7);
    t.append(String(wavelengthInNm, 3, false), 20);
    t.append(" nm", 3);

    label_ColourCode->setText(t, dontSendNotification);
    
    StringArray manipulationStrings;
    
    // shortcut manipulations
    for (int i = 0; i < 5; i++)
    {
        manipulationPopupComponent[i]   ->textEditorDivision        ->setText(String(divisionValue[i]));
        manipulationPopupComponent[i]   ->textEditorMultiplication  ->setText(String(multiplyValue[i]));
        manipulationPopupComponent[i]   ->button_Multiplication     ->setToggleState(!multOrDivide[i], dontSendNotification);
        manipulationPopupComponent[i]   ->button_Division           ->setToggleState(multOrDivide[i], dontSendNotification);
        
        shortcutComponent[i]            ->setState(manipulateChosenFrequency[i]);
        
        if (manipulateChosenFrequency[i])
        {
            shortcutComponent[i]            ->colourOutputComponent->setBackgroundColour(manipulatedColour[i]);
            
            // set text to colour component
            String text;
            text.append(String(shortcutFrequency[i] * THZ_FACTOR, 3, false), 20);
            text.append(" THz / ", 7);
            text.append(String(shortcutWaveLength[i], 3, false), 20);
            text.append(" nm", 3);
            
            shortcutComponent[i]            ->label_ColourCode->setText(text, dontSendNotification);
            
            manipulationStrings.add(text);
        }
    }
    
    String conversionType;
    String base;
    if (conversionSource)
    {
        conversionType  = "Frequency";
        base            = getFrequencyStringForCurrentUnit();
    }
    else
    {
        conversionType = "Chord";
        base = getChordStringForCurrentUnit();
    }

    String colorHex(mainOutputColour.toDisplayString(false));
    
    projectManager->logFileWriter->processLog_FrequencyToLight(conversionType, base, getWavelengthStringForCurrentUnit(), colorHex, manipulationStrings);

}

void FrequencyToLightComponent::calculateValuesFromColourSlider(double val)
{
     // map value to wavelength range 380 781...
    colourSliderValue       = val;
    colourSliderWavelength  = (colourSliderValue * (maxWavelength - minWavelength)) + minWavelength;
    
    double harmonicInterval = 1.03; // Defines harmonic interval
    bool mapByWavelength    = true;
    
    for (int i = 0; i < 5; i++)
    {
        if (mapByWavelength)
        {
            // will shift harmonics over the wavelength, if false, will map by frequencies..
            
            // set to the parameters
            if (i == 0)
            {
                wavelengthColourSlider[i]   = colourSliderWavelength;
                frequencyColourSlider[i]    = SPEEDLIGHTINVACUUM / (wavelengthColourSlider[i] * METER_FACTOR);
            }
            else
            {
                wavelengthColourSlider[i]   = wavelengthColourSlider[i-1] * harmonicInterval;
                frequencyColourSlider[i]    = SPEEDLIGHTINVACUUM / (wavelengthColourSlider[i] * METER_FACTOR);
            }
        }
        else
        {
            // map by Frequency harmonics
        }
        
        // set to the labels etc
        colourSliderColour[i] = getColourFromWavelength(wavelengthColourSlider[i]);
        roundedColourOutput[i]->setColour(colourSliderColour[i]);
        
        String waveValString(wavelengthColourSlider[i], 3, false); waveValString.append(" Nm", 3);
        
        labelWavelength[i]  ->setText(waveValString, dontSendNotification);
        
        String freqValString(frequencyColourSlider[i], 3, false); freqValString.append(" Hz", 3);
        
        labelFrequency[i]   ->setText(freqValString, dontSendNotification);
    }
}

void FrequencyToLightComponent::sliderValueChanged (Slider* slider)
{
    if (slider == sliderColour)
    {
        calculateValuesFromColourSlider(slider->getValue());
    }
}

void FrequencyToLightComponent::buttonClicked (Button* button)
{
    if (button == button_ChordConversion)
    {
        projectManager->setFrequencyToLightParameter(FREQUENCY_LIGHT_CONVERSION_SOURCE, false);
        
        conversionSource = false;
        
        component_ColourOutputMain->setGradientBool(true);
        
        calculateManipulationsFromChord();
        
        setControls();

    }
    else if (button == button_FrequencyConversion)
    {
        projectManager->setFrequencyToLightParameter(FREQUENCY_LIGHT_CONVERSION_SOURCE, true);
        
        conversionSource = true;
        
        component_ColourOutputMain->setGradientBool(false);
        
        calculateManipulations();
        
        setControls();
    }
    else
    {
        for (int i = 0; i < 5; i++)
        {
            if (button == shortcutComponent[i]->button_Add)
            {
                projectManager->setFrequencyToLightParameter(FREQUENCY_LIGHT_MANIPULATE_CHOSEN_FREQUENCY1 + (i * 4), true);
                
                manipulateChosenFrequency[i] = true;
                
                setControls();
                
                openPopup(i);
            }
            else if (button == shortcutComponent[i]->button_Delete)
            {
                projectManager->setFrequencyToLightParameter(FREQUENCY_LIGHT_MANIPULATE_CHOSEN_FREQUENCY1 + (i * 4), false);
                
                manipulateChosenFrequency[i] = false;
                
                setControls();

            }
            else if (button == shortcutComponent[i]->button_OpenSettings)
            {
                setControls();
                
                openPopup(i);
            }
        }
        
        for (int i = 0; i < 5; i++)
        {
            if (button == manipulationPopupComponent[i]->button_Close)
            {
                manipulateChosenFrequency[i] = false;
                
                setControls();
                
                closePopup();
            }
        }
        
        for (int i = 0; i < 5; i++)
        {
            if (button == manipulationPopupComponent[i]->button_Multiplication)
            {
                multOrDivide[i] = false;
                
                projectManager->setFrequencyToLightParameter(FREQUENCY_LIGHT_MULTIPLY_OR_DIVISION1 + (i * 4), multOrDivide[i]);
                
                setControls();
            }
            else if  ( button == manipulationPopupComponent[i]->button_Division )
            {
                multOrDivide[i] = true;
                
                projectManager->setFrequencyToLightParameter(FREQUENCY_LIGHT_MULTIPLY_OR_DIVISION1 + (i * 4), multOrDivide[i]);
                
                setControls();
            }
        }
        
        for (int i = 0; i < 5; i++)
        {
            if (button == manipulationPopupComponent[i]->button_Save)
            {
                // need all the manipulation text, booleans etc to be sent here...
                float value =  manipulationPopupComponent[i]->textEditorDivision->getText().getFloatValue();
                if (value >= DIVISION_MAXIMUM) {  value = DIVISION_MAXIMUM; }
                if (value <= DIVISION_MINIMUM) {  value = DIVISION_MINIMUM; }
                projectManager->setFrequencyToLightParameter(FREQUENCY_LIGHT_DIVISION_VALUE1 + (i * 4), value);
                
                divisionValue[i] = value;
                
                float value2 =  manipulationPopupComponent[i]->textEditorMultiplication->getText().getFloatValue();
                if (value2 >= MULTIPLY_MAXIMUM) {  value2 = MULTIPLY_MAXIMUM; }
                if (value2 <= MULTIPLY_MINIMUM) {  value2 = MULTIPLY_MINIMUM; }
                projectManager->setFrequencyToLightParameter(FREQUENCY_LIGHT_MULTIPLY_VALUE1 + (i * 4), value);
                
                multiplyValue[i] = value2;
                
                projectManager->setFrequencyToLightParameter(FREQUENCY_LIGHT_MULTIPLY_OR_DIVISION1 + (i * 4), multOrDivide[i]);
                
                if (conversionSource) {
                    calculateManipulations();
                }
                else {
                    calculateManipulationsFromChord();
                }
                
                setControls();
                
                closePopup();
            }
        }
    }
}

void FrequencyToLightComponent::textEditorReturnKeyPressed (TextEditor&editor)
{
    if (&editor == textEditor_Frequency)
    {
        double value = editor.getText().getDoubleValue(); // could be in hz, mhz, hghz
        
        // convert text number to hz,
        setFrequencyInHzFromCurrentUnit(value);
        
        calculateCoefficientsFromFrequency();
        
        setControls();
        
        // then send to projectManager
        projectManager->setFrequencyToLightParameter(FREQUENCY_LIGHT_FREQUENCY, frequencyInHz);
        

        
    }
    else if (&editor == textEditor_Wavelength)
    {
        double value = editor.getText().getDoubleValue();
        
//        String newVal(value); editor.setText(newVal);
        
        setWavelengthInNmFromCurrentUnit(value);
        
        calculateCoefficientsFromWavelength();
        
        setControls();
        
        // send to projectManager
        projectManager->setFrequencyToLightParameter(FREQUENCY_LIGHT_WAVELENGTH, wavelengthInNm);

    }
    
    else if (&editor == manipulationPopupComponent[0]->textEditorDivision)
    {
        float value = editor.getText().getFloatValue();
        
        // bounds check
        if (value >= DIVISION_MAXIMUM) {  value = DIVISION_MAXIMUM; }
        if (value <= DIVISION_MINIMUM) {  value = DIVISION_MINIMUM; }
        
        String newVal(value); editor.setText(newVal);
        
        divisionValue[0] = value;
        
    }
    else if (&editor == manipulationPopupComponent[0]->textEditorMultiplication)
    {
        float value = editor.getText().getFloatValue();
        
        // bounds check
        if (value >= MULTIPLY_MAXIMUM) {  value = MULTIPLY_MAXIMUM; }
        if (value <= MULTIPLY_MINIMUM) {  value = MULTIPLY_MINIMUM; }
        
        String newVal(value); editor.setText(newVal);
        
        multiplyValue[0] = value; calculateManipulations(); setControls();
    }
    
    // 2
    else if (&editor == manipulationPopupComponent[1]->textEditorDivision)
    {
        float value = editor.getText().getFloatValue();
        
        // bounds check
        if (value >= DIVISION_MAXIMUM) {  value = DIVISION_MAXIMUM; }
        if (value <= DIVISION_MINIMUM) {  value = DIVISION_MINIMUM; }
        
        String newVal(value); editor.setText(newVal);
        
        divisionValue[1] = value; calculateManipulations(); setControls();
        

    }
    else if (&editor == manipulationPopupComponent[1]->textEditorMultiplication)
    {
        float value = editor.getText().getFloatValue();
        
        // bounds check
        if (value >= MULTIPLY_MAXIMUM) {  value = MULTIPLY_MAXIMUM; }
        if (value <= MULTIPLY_MINIMUM) {  value = MULTIPLY_MINIMUM; }
        
        String newVal(value); editor.setText(newVal);
        

    }
    
    // 3
    else if (&editor == manipulationPopupComponent[2]->textEditorDivision)
    {
        float value = editor.getText().getFloatValue();
        
        // bounds check
        if (value >= DIVISION_MAXIMUM) {  value = DIVISION_MAXIMUM; }
        if (value <= DIVISION_MINIMUM) {  value = DIVISION_MINIMUM; }
        
        String newVal(value); editor.setText(newVal);

    }
    else if (&editor == manipulationPopupComponent[2]->textEditorMultiplication)
    {
        float value = editor.getText().getFloatValue();
        
        // bounds check
        if (value >= MULTIPLY_MAXIMUM) {  value = MULTIPLY_MAXIMUM; }
        if (value <= MULTIPLY_MINIMUM) {  value = MULTIPLY_MINIMUM; }
        
        String newVal(value); editor.setText(newVal);

    }
    
    // 4
    else if (&editor == manipulationPopupComponent[3]->textEditorDivision)
    {
        float value = editor.getText().getFloatValue();
        
        // bounds check
        if (value >= DIVISION_MAXIMUM) {  value = DIVISION_MAXIMUM; }
        if (value <= DIVISION_MINIMUM) {  value = DIVISION_MINIMUM; }
        
        String newVal(value); editor.setText(newVal);

    }
    else if (&editor == manipulationPopupComponent[3]->textEditorMultiplication)
    {
        float value = editor.getText().getFloatValue();
        
        // bounds check
        if (value >= MULTIPLY_MAXIMUM) {  value = MULTIPLY_MAXIMUM; }
        if (value <= MULTIPLY_MINIMUM) {  value = MULTIPLY_MINIMUM; }
        
        String newVal(value); editor.setText(newVal);
        
    }
    
    // 5
    else if (&editor == manipulationPopupComponent[4]->textEditorDivision)
    {
        float value = editor.getText().getFloatValue();
        
        // bounds check
        if (value >= DIVISION_MAXIMUM) {  value = DIVISION_MAXIMUM; }
        if (value <= DIVISION_MINIMUM) {  value = DIVISION_MINIMUM; }
        
        String newVal(value); editor.setText(newVal);

    }
    else if (&editor == manipulationPopupComponent[4]->textEditorMultiplication)
    {
        float value = editor.getText().getFloatValue();
        
        // bounds check
        if (value >= MULTIPLY_MAXIMUM) {  value = MULTIPLY_MAXIMUM; }
        if (value <= MULTIPLY_MINIMUM) {  value = MULTIPLY_MINIMUM; }
        
        String newVal(value); editor.setText(newVal);
        
    }
    
}

void FrequencyToLightComponent::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == comboBoxKeynote)
    {
        projectManager->setFrequencyToLightParameter(FREQUENCY_LIGHT_KEYNOTE, comboBoxKeynote->getSelectedId());
        
        keynote = comboBoxKeynote->getSelectedId();
        
        chordManager->setKeyNote((enum KEYNOTES) keynote);
        
        calculateCoefficientsFromChord();
        
        setControls();
    }
    else if (comboBoxThatHasChanged == comboBoxChordtype)
    {
        projectManager->setFrequencyToLightParameter(FREQUENCY_LIGHT_CHORDTYPE, comboBoxChordtype->getSelectedId());
        
        chordType = comboBoxChordtype->getSelectedId();
        
        chordManager->setChordType((enum CHORD_TYPES) chordType);
        
        calculateCoefficientsFromChord();
        
        setControls();
    }
    
    
    // other drop downs should just change displayed value
    else if (comboBoxThatHasChanged == comboBox_FrequencySuffix)
    {
        projectManager->setFrequencyToLightParameter(FREQUENCY_LIGHT_FREQUENCY_UNIT, comboBox_FrequencySuffix->getSelectedId());
        
        currentFrequencyUnit = (FREQUENCY_UNITS)comboBox_FrequencySuffix->getSelectedId();
        
        setControls();
    }
    else if (comboBoxThatHasChanged == comboBox_WavelgnthSuffix)
    {
        projectManager->setFrequencyToLightParameter(FREQUENCY_LIGHT_WAVELENGTH_UNIT, comboBox_WavelgnthSuffix->getSelectedId());
        
        currentWaveLengthUnit = (WAVELENGTH_UNITS)comboBox_WavelgnthSuffix->getSelectedId();
        
        
        setControls();
    }
    else if (comboBoxThatHasChanged == comboBox_PhaseSpeed)
    {
        projectManager->setFrequencyToLightParameter(FREQUENCY_LIGHT_PHASESPEED, comboBox_PhaseSpeed->getSelectedId());
        
        phaseSpeedUnit = comboBox_PhaseSpeed->getSelectedId();
        
        calculateCoefficientsFromFrequency();
        
        setControls();
    }
}

void FrequencyToLightComponent::setFrequencyInHzToCurrentUnit()
{
    float newFreqTextValue = 0;
    switch (currentFrequencyUnit)
    {
        case HZ:  newFreqTextValue = frequencyInHz; break;
        case KHZ: newFreqTextValue = frequencyInHz * KHZ_FACTOR; break;
        case MHZ: newFreqTextValue = frequencyInHz * MHZ_FACTOR; break;
        case GHZ: newFreqTextValue = frequencyInHz * GHZ_FACTOR; break;
        case THZ: newFreqTextValue = frequencyInHz * THZ_FACTOR; break;
        default: break;
    }

    String freqString(newFreqTextValue, getNumDecimalsForFrequency(), false);
    textEditor_Frequency->setText(freqString);
}

void FrequencyToLightComponent::setFrequencyInHzFromCurrentUnit(float val)
{
    switch (currentFrequencyUnit)
    {
        case HZ:  frequencyInHz = val; break;
        case KHZ: frequencyInHz = val / KHZ_FACTOR; break;
        case MHZ: frequencyInHz = val / MHZ_FACTOR; break;
        case GHZ: frequencyInHz = val / GHZ_FACTOR; break;
        case THZ: frequencyInHz = val / THZ_FACTOR; break;
        default:  frequencyInHz = val; break;
    }
}

String FrequencyToLightComponent::getFrequencyStringForCurrentUnit()
{
    float newFreqTextValue = 0;
    switch (currentFrequencyUnit)
    {
        case HZ:  newFreqTextValue = frequencyInHz; break;
        case KHZ: newFreqTextValue = frequencyInHz * KHZ_FACTOR; break;
        case MHZ: newFreqTextValue = frequencyInHz * MHZ_FACTOR; break;
        case GHZ: newFreqTextValue = frequencyInHz * GHZ_FACTOR; break;
        case THZ: newFreqTextValue = frequencyInHz * THZ_FACTOR; break;
        default: break;
    }
    
    String freqString(newFreqTextValue, getNumDecimalsForFrequency(), false);
    return freqString;
}

String FrequencyToLightComponent::getChordStringForCurrentUnit()
{
    String chordString;

    String keynoteString(ProjectStrings::getKeynoteArray().getReference(keynote-1));
    String chordTypeString(ProjectStrings::getChordTypeArray().getReference(chordType-1));
    
    chordString.append(keynoteString, 3); chordString.append(" ", 1);
    chordString.append(chordTypeString, 20);

    return chordString;
}

int FrequencyToLightComponent::getNumDecimalsForFrequency()
{
    int numDecimals = 0;
    switch (currentFrequencyUnit)
    {
        case HZ:  numDecimals = 3; break;
        case KHZ: numDecimals = 3; break;
        case MHZ: numDecimals = 5; break;
        case GHZ: numDecimals = 7; break;
        case THZ: numDecimals = 10; break;
        default: break;
    }
    
    return numDecimals;
}

void FrequencyToLightComponent::setWavelengthInNmToCurrentUnit()
{
    float newWavelengthTextValue = 0;
    switch (currentWaveLengthUnit)
    {
        case NANOMETER:     newWavelengthTextValue = wavelengthInNm;                        break;
        case MICROMETER:    newWavelengthTextValue = wavelengthInNm * MICROMETER_FACTOR;    break;
        case MILLIMETER:    newWavelengthTextValue = wavelengthInNm * MILLIMETER_FACTOR;    break;
        case CENTIMETER:    newWavelengthTextValue = wavelengthInNm * CENTIMETER_FACTOR;    break;
        case METER:         newWavelengthTextValue = wavelengthInNm * METER_FACTOR;         break;
        default: break;
    }

    String wlString(newWavelengthTextValue, getNumDecimalsForWavelength(), false);
    textEditor_Wavelength->setText(wlString);
}

void FrequencyToLightComponent::setWavelengthInNmFromCurrentUnit(float val)
{
    switch (currentWaveLengthUnit)
    {
        case NANOMETER:     wavelengthInNm = val;  break;
        case MICROMETER:    wavelengthInNm = val / MICROMETER_FACTOR;   break;
        case MILLIMETER:    wavelengthInNm = val / MILLIMETER_FACTOR;   break;
        case CENTIMETER:    wavelengthInNm = val / MICROMETER_FACTOR;   break;
        case METER:         wavelengthInNm = val / METER_FACTOR;        break;
        default: break;
    }
}

String FrequencyToLightComponent::getWavelengthStringForCurrentUnit()
{
    float newWavelengthTextValue = 0;
    switch (currentWaveLengthUnit)
    {
        case NANOMETER:     newWavelengthTextValue = wavelengthInNm;                        break;
        case MICROMETER:    newWavelengthTextValue = wavelengthInNm * MICROMETER_FACTOR;    break;
        case MILLIMETER:    newWavelengthTextValue = wavelengthInNm * MILLIMETER_FACTOR;    break;
        case CENTIMETER:    newWavelengthTextValue = wavelengthInNm * CENTIMETER_FACTOR;    break;
        case METER:         newWavelengthTextValue = wavelengthInNm * METER_FACTOR;         break;
        default: break;
    }
    
    String wlString(newWavelengthTextValue, getNumDecimalsForWavelength(), false);
    return wlString;
}

int FrequencyToLightComponent::getNumDecimalsForWavelength()
{
    int numDecimals = 0;
    switch (currentWaveLengthUnit)
    {
        case NANOMETER:  numDecimals = 3; break;
        case MICROMETER: numDecimals = 3; break;
        case MILLIMETER: numDecimals = 5; break;
        case CENTIMETER: numDecimals = 7; break;
        case METER:      numDecimals = 10; break;
        default: break;
    }
    
    return numDecimals;
}

double FrequencyToLightComponent::getWavelengthFromFrequency(double f)
{
    float c = 0;
    if (phaseSpeedUnit == PHASE_SPEED_UNITS::SPEED_LIGHT_IN_VACUUM)
    { c = SPEEDLIGHTINVACUUM; }
    else if (phaseSpeedUnit == PHASE_SPEED_UNITS::SPEED_LIGHT_IN_WATER)
    { c = SPEEDLIGHTINWATER; }
    else if (phaseSpeedUnit == PHASE_SPEED_UNITS::SPEED_SOUND_IN_VACUUM)
    { c = SPEEDSOUNDINVACUUM; }
    else if (phaseSpeedUnit == PHASE_SPEED_UNITS::SPEED_SOUND_IN_WATER)
    { c = SPEEDSOUNDINWATER; }
        
    double wavelgth = c / (f * METER_FACTOR);
    return wavelgth;
}
    
double FrequencyToLightComponent::getFrequencyFromWavelength(double w)
{
    float c = 0;
    if (phaseSpeedUnit == PHASE_SPEED_UNITS::SPEED_LIGHT_IN_VACUUM)
    { c = SPEEDLIGHTINVACUUM; }
    else if (phaseSpeedUnit == PHASE_SPEED_UNITS::SPEED_LIGHT_IN_WATER)
    { c = SPEEDLIGHTINWATER; }
    else if (phaseSpeedUnit == PHASE_SPEED_UNITS::SPEED_SOUND_IN_VACUUM)
    { c = SPEEDSOUNDINVACUUM; }
    else if (phaseSpeedUnit == PHASE_SPEED_UNITS::SPEED_SOUND_IN_WATER)
    { c = SPEEDSOUNDINWATER; }
        
    return c / (w * METER_FACTOR);
}

Colour FrequencyToLightComponent::getColourFromWavelength(double Wavelength)
{
    double Gamma            = 0.80;
    double IntensityMax     = 255;

    double factor, Red,Green,Blue;

    if((Wavelength >= 380) && (Wavelength<440))
    {
        Red = -(Wavelength - 440) / (440 - 380);
        Green = 0.0;
        Blue = 1.0;
    }
    else if((Wavelength >= 440) && (Wavelength<490))
    {
        Red = 0.0;
        Green = (Wavelength - 440) / (490 - 440);
        Blue = 1.0;
    }
    else if((Wavelength >= 490) && (Wavelength<510))
    {
        Red = 0.0;
        Green = 1.0;
        Blue = -(Wavelength - 510) / (510 - 490);
    }
    else if((Wavelength >= 510) && (Wavelength<580))
    {
        Red = (Wavelength - 510) / (580 - 510);
        Green = 1.0;
        Blue = 0.0;
    }
    else if((Wavelength >= 580) && (Wavelength<645))
    {
        Red = 1.0;
        Green = -(Wavelength - 645) / (645 - 580);
        Blue = 0.0;
    }
    else if((Wavelength >= 645) && (Wavelength<781))
    {
        Red = 1.0;
        Green = 0.0;
        Blue = 0.0;
    }
    else
    {
        Red = 0.0;
        Green = 0.0;
        Blue = 0.0;
    }

    // Let the intensity fall off near the vision limits

    if((Wavelength >= 380) && (Wavelength<420))
    {
        factor = 0.3 + 0.7*(Wavelength - 380) / (420 - 380);
    }
    else if((Wavelength >= 420) && (Wavelength<701))
    {
        factor = 1.0;
    }
    else if((Wavelength >= 701) && (Wavelength<781))
    {
        factor = 0.3 + 0.7*(780 - Wavelength) / (780 - 700);
    }
    else
    {
        factor = 0.0;
    }

    int* rgb = new int[3];

    // Don't want 0^x = 1 for x <> 0
    rgb[0] = Red==0.0 ? 0 : (int) round(IntensityMax * pow(Red * factor, Gamma));
    rgb[1] = Green==0.0 ? 0 : (int) round(IntensityMax * pow(Green * factor, Gamma));
    rgb[2] = Blue==0.0 ? 0 : (int) round(IntensityMax * pow(Blue * factor, Gamma));

    
    return  Colour::fromRGB(rgb[0], rgb[1], rgb[2]);
}

       // called when chord is selected
void FrequencyToLightComponent::calculateCoefficientsFromChord()
{
    // get frequency of Base note
    Array<int> notes =  chordManager->getMIDIKeysForChord();
        
    int numNotes = chordManager->getNumNotesInSelectedChord();
        
    double frequencies[numNotes];
    double wavelengths[numNotes];
    Colour colourForNotes[numNotes];
        
    for (int i = 0; i < numNotes; i++)
    {
        // get frequency of note
        int note            = notes.getReference(i) + (12 * 5);
        frequencies[i]      = projectManager->frequencyManager->getFrequencyForMIDINote(note); // need to fix based on Scales
            
        // put frequency 40 octaves higher
        double factor       = pow(2.0, 40);
        double freq         = frequencies[i] * factor;//    1099511627776; //(2power of 40;)
        double wavelgth     = getWavelengthFromFrequency(freq);  //(SPEED_LIGHT_IN_VACUUM / freq) * METER_FACTOR;
//            float c            = SPEED_LIGHT_IN_VACUUM;
//            double wavelgth     = c / (freq * METER_FACTOR);
            
        wavelengths[i]      = wavelgth;

        colourForNotes[i]   = getColourFromWavelength(wavelengths[i]);
    }
        
        // set to chord base freq
    chordBaseFreq = frequencies[0];

    // create basic gradient
    ColourGradient *grad = new ColourGradient(Colours::cyan, 0, 0,
    Colours::magenta, component_ColourOutputMain->getWidth(), component_ColourOutputMain->getHeight(), false);
            
    // clear colours from grdient before adding custom gradient
    grad->clearColours();

    int numDivisions = numNotes + (numNotes-1);
    double proportions = (double) 1 / numDivisions;
    int count = numNotes-1;
    bool rev = false;
    for (int i = 0; i < numDivisions; i++)
    {
        grad->addColour(proportions * i, colourForNotes[count]);
            
        if (!rev)
        {
            count--;
            if (count == 0)
            {
                rev = true;
            }
        }
        else count++;
    }
    // set Gradient to
    component_ColourOutputMain->setGradient(*grad);

    // mmmm.......
    calculateManipulationsFromChord();
}
        
// called when frequency parameter is changed
void FrequencyToLightComponent::calculateCoefficientsFromFrequency()
{
    wavelengthInNm      = getWavelengthFromFrequency(frequencyInHz);
    mainOutputColour    = getColourFromWavelength(wavelengthInNm);
            
    calculateManipulations();
}
        
// callend when wavelength is changed
void FrequencyToLightComponent::calculateCoefficientsFromWavelength()
{
    frequencyInHz       = getFrequencyFromWavelength(wavelengthInNm);
    mainOutputColour    = getColourFromWavelength(wavelengthInNm);
        
    calculateManipulations();
}
            
// called by all the above when changed, called when manipulations are called
void FrequencyToLightComponent::calculateManipulations()
{
    // use manipulation values for create colours
    for (int i = 0; i < 5; i++)
    {
        if (manipulateChosenFrequency[i])
        {
            if (!multOrDivide[i]) // mult
            {
                shortcutFrequency[i]    = frequencyInHz * multiplyValue[i];
                shortcutWaveLength[i]   = getWavelengthFromFrequency(shortcutFrequency[i]);
            }
            else // divide
            {
                shortcutFrequency[i]    = frequencyInHz / divisionValue[i];
                shortcutWaveLength[i]   = getWavelengthFromFrequency(shortcutFrequency[i]);
            }
                
            manipulatedColour[i] = getColourFromWavelength(shortcutWaveLength[i]);
        }
        else
        {
            // set to white
            manipulatedColour[i] = Colours::white;
        }
    }
}
    
void FrequencyToLightComponent::calculateManipulationsFromChord()
{
    // use manipulation values for create colours
    for (int i = 0; i < 5; i++)
    {
        if (manipulateChosenFrequency[i])
        {
            if (!multOrDivide[i]) // mult
            {
                shortcutFrequency[i]    = chordBaseFreq * multiplyValue[i];
                shortcutWaveLength[i]   = getWavelengthFromFrequency(shortcutFrequency[i]);
            }
            else // divide
            {
                shortcutFrequency[i]    = chordBaseFreq / divisionValue[i];
                shortcutWaveLength[i]   = getWavelengthFromFrequency(shortcutFrequency[i]);
            }
                    
            manipulatedColour[i] = getColourFromWavelength(shortcutWaveLength[i]);
        }
        else
        {
            // set to white
            manipulatedColour[i] = Colours::white;
        }
    }
}



void FrequencyToLightComponent::openPopup(int shortcutRef)
{
    manipulationPopupComponent[shortcutRef]->setVisible(true);
}

void FrequencyToLightComponent::closePopup()
{
    for (int i = 0; i < 5; i++)
    {
        manipulationPopupComponent[i]->setVisible(false);
    }
    
//    updateColourAlgorithm();
}

void FrequencyToLightComponent::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setOpacity(1.0);
    g.drawImage(imageMainBackground, 0, 0, 1562 * scaleFactor, 1440 * scaleFactor, 0, 0, 1562, 1440);
}

