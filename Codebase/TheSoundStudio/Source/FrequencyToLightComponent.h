/*
  ==============================================================================

    FrequencyToLightComponent.h
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "CustomLookAndFeel.h"
#include "ProjectManager.h"
#include "FrequencyToLightSubComponents.h"
#include "MenuViewInterface.h"
#include <memory>

// change param -> processor ->
// need to refactor,

class FrequencyToLightComponent :
    public MenuViewInterface,
    public Button::Listener,
    public TextEditor::Listener,
    public ComboBox::Listener,
    public Slider::Listener,
    public ProjectManager::UIListener
{
public:
    FrequencyToLightComponent(ProjectManager * pm);
    ~FrequencyToLightComponent();

    void paint (Graphics&) override;
    void resized() override;
    void buttonClicked (Button*button)override;
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged)override;
    void textEditorReturnKeyPressed (TextEditor&editor)override;
    void sliderValueChanged (Slider* slider) override;
    void openPopup(int shortcutRef);
    void closePopup();
    
    // Conversions
    void updateColourSliderAlgorithm(){}
    
    void setFrequencyInHzToCurrentUnit();
    void setFrequencyInHzFromCurrentUnit(float val);
    String getFrequencyStringForCurrentUnit();
    
    String getChordStringForCurrentUnit();
    
    int getNumDecimalsForFrequency();
    
    void setWavelengthInNmToCurrentUnit();
    void setWavelengthInNmFromCurrentUnit(float val);
    String getWavelengthStringForCurrentUnit();
    int getNumDecimalsForWavelength();
    
       // called when chord is selected
    void calculateCoefficientsFromChord();
        
    // called when frequency parameter is changed
    void calculateCoefficientsFromFrequency();
    // callend when wavelength is changed
    void calculateCoefficientsFromWavelength();
       
        // called by all the above when changed, called when manipulations are called
    void calculateManipulations();
    void calculateManipulationsFromChord();
        
    double getWavelengthFromFrequency(double f);
    double getFrequencyFromWavelength(double w);
        
    Colour getColourFromWavelength(double Wavelength);
    void calculateValuesFromColourSlider(double val);
    
    void initParameters();
    void setControls();
    
    void updateSettingsUIParameter(int settingIndex)override
    {
        if (settingIndex == DEFAULT_SCALE)
        {
            updateScalesComponents();
        }
    }
    
    void updateScalesComponents()
    {
        ScalesManager * sm = projectManager->frequencyManager->scalesManager;
        
        sm->getComboBoxPopupMenuForChords(*comboBoxChordtype->getRootMenu(), SCALES_UNIT::MAIN_SCALE);
        sm->getComboBoxPopupMenuForKeynotes(*comboBoxKeynote->getRootMenu(), SCALES_UNIT::MAIN_SCALE);
    }
    
    float scaleFactor = 0.5;
    void setScale(float factor) override
    {
        scaleFactor = factor;
        lookAndFeel.setScale(scaleFactor);
        
        for (int i = 0; i < 5; i++)
        {
            manipulationPopupComponent[i]->setScale(scaleFactor);
            shortcutComponent[i]->setScale(scaleFactor);
        }
        
    }
    
private:
    ProjectManager * projectManager;
    
    FrequencyManager * frequencyManager;
    std::unique_ptr<ChordManager> chordManager;
    
    std::unique_ptr<ImageButton> button_ChordConversion;
    std::unique_ptr<ImageButton> button_FrequencyConversion;
    std::unique_ptr<TextEditor> textEditor_Frequency;
    std::unique_ptr<TextEditor> textEditor_Wavelength;
    std::unique_ptr<ComboBox> comboBox_PhaseSpeed;
    std::unique_ptr<ComboBox> comboBox_FrequencySuffix;
    std::unique_ptr<ComboBox> comboBox_WavelgnthSuffix;
    
    std::unique_ptr<ColourOutputComponent> component_ColourOutputMain;
    std::unique_ptr<Label> label_ColourCode;
    
    
    // shortcut manipulations
    std::unique_ptr<ManipulationPopupComponent> manipulationPopupComponent[5];
    std::unique_ptr<ShortcutColourComponent> shortcutComponent[5];
    
    // Colour Slider
    std::unique_ptr<RoundedColourOutputComponent> roundedColourOutput[5];
    
    std::unique_ptr<Label> labelWavelength[5];
    std::unique_ptr<Label> labelFrequency[5];
    
    std::unique_ptr<Slider> sliderColour;
    
    std::unique_ptr<ComboBox> comboBoxKeynote;
    std::unique_ptr<ComboBox> comboBoxChordtype;
    
    Image imageMainBackground;
    Image imageStopButton;
    Image imageStartButton;
    Image imageBlueButtonNormal;
    Image imageBlueButtonSelected;
    Image imageBlueCheckButtonNormal;
    Image imageBlueCheckButtonSelected;
    
    CustomLookAndFeel lookAndFeel;
    
    // local parameter values before caluclating into different units
    double chordBaseFreq;
    double frequencyInHz;
    double wavelengthInNm;
    
    bool conversionSource = false;
    int keynote;
    int chordType;
//    int frequencyUnit; // hz etc
//    int wavelengthUnit; // nm etc
    int  phaseSpeedUnit;
    double phaseSpeedValue;
    
    // manipulation
    bool manipulateChosenFrequency[5]; // bool
    bool multOrDivide[5];
    float multiplyValue[5];
    float divisionValue[5];
    float shortcutFrequency[5];
    float shortcutWaveLength[5];
    
    Colour mainOutputColour;
    Colour manipulatedColour[5];
    
    // colour Slider Params
    double colourSliderValue; // 0..1
    double colourSliderWavelength;
    double minWavelength = 380.f;
    double maxWavelength = 781.f;
    
    double frequencyColourSlider[5];
    double wavelengthColourSlider[5];
    Colour colourSliderColour[5];
    
    FREQUENCY_UNITS currentFrequencyUnit;
    WAVELENGTH_UNITS currentWaveLengthUnit;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FrequencyToLightComponent)
};
