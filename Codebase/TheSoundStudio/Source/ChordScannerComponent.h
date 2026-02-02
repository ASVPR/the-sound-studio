/*
  ==============================================================================

    ChordScannerComponent.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "ProjectManager.h"
#include "CustomLookAndFeel.h"
#include "VisualiserContainerComponent.h"
#include "CustomRotarySlider.h"
#include "CustomProgressBar.h"
#include "WaveTableOscViewComponent.h"
#include "TransportToolbarComponent.h"
#include "MenuViewInterface.h"
#include "UI/DesignSystem.h"
#include <memory>

//==============================================================================
/*
*/
class ChordScannerComponent :
        public MenuViewInterface,
        public Button::Listener,
        public ProjectManager::UIListener,
        public Slider::Listener,
        public ComboBox::Listener,
        public TextEditor::Listener,
        public TransportToolbarComponent::Listener,
        public Timer
{
public:
    ChordScannerComponent(ProjectManager * pm);
    ~ChordScannerComponent();

    void paint (Graphics&) override;
    void resized() override;
    
    void buttonClicked (Button*button)override;
    void sliderValueChanged (Slider* slider)override;
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged)override;
    void textEditorReturnKeyPressed (TextEditor&editor)override;
    
    void syncUI();
    
    void timerCallback() override;
    
    void updateChordScannerUIParameter(int paramIndex)override;
    void updateMinMaxSettings(int paramIndex) override;

    // TransportToolbarComponent::Listener
    void transportPlayClicked() override;
    void transportStopClicked() override;
    void transportRecordClicked() override;
    void transportPanicClicked() override;
    void transportSaveClicked() override;
    void transportLoadClicked() override;

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
        
        sm->getComboBoxPopupMenuForKeynotes(*comboBoxKeynoteTo->getRootMenu(), SCALES_UNIT::MAIN_SCALE);
        sm->getComboBoxPopupMenuForKeynotes(*comboBoxKeynoteFrom->getRootMenu(), SCALES_UNIT::MAIN_SCALE);
    }
    
    
    void closePopups()
    {
        visualiserContainerComponent->setPopupsAreVisible(false);
    }
    
    float scaleFactor = 0.5;
    void setScale(float factor) override
    {
        if (scaleFactor != factor)
        {
            scaleFactor = factor;
            
            // Update all components with new scale
            lookAndFeel.setScale(scaleFactor);
            if (visualiserContainerComponent)
                visualiserContainerComponent->setScale(scaleFactor);
            
            // Trigger re-layout with new scale
            resized();
            repaint();
        }
    }
    
    
    
private:
    
    ProjectManager * projectManager;
    
    std::unique_ptr<CustomRotarySlider> slider_Amplitude;
    std::unique_ptr<CustomRotarySlider> slider_Attack;
    std::unique_ptr<CustomRotarySlider> slider_Sustain;
    std::unique_ptr<CustomRotarySlider> slider_Decay;
    std::unique_ptr<CustomRotarySlider> slider_Release;
    
    std::unique_ptr<Label> label_CurrentPlayingChord;
    std::unique_ptr<Label> label_Frequency;
    std::unique_ptr<Label> label_ClosestChord;
    
    std::unique_ptr<ComboBox> comboBoxKeynoteFrom;
    std::unique_ptr<ComboBox> comboBoxKeynoteTo;
    
    std::unique_ptr<ComboBox> comboBoxOutputSelection;
    
    std::unique_ptr<TextEditor> textEditorFrequencyFrom;
    std::unique_ptr<TextEditor> textEditorFrequencyTo;
    
    std::unique_ptr<ComboBox> comboBoxOctaveFrom;
    std::unique_ptr<ComboBox> comboBoxOctaveTo;
    
    std::unique_ptr<ImageButton> button_ExtendedRange;
    
    std::unique_ptr<Component> component_FrequencyContainer;
    std::unique_ptr<Component> component_KeynoteContainer;
    
    void switchKeynoteFrequencyComponent(bool keynote)
    {
        if (keynote)
        {
            component_FrequencyContainer->setVisible(false);
            component_KeynoteContainer  ->setVisible(true);
        }
        else
        {
            component_FrequencyContainer->setVisible(true);
            component_KeynoteContainer  ->setVisible(false);
        }
    }
    
    void setChordStringForCurrentChordLabel()
    {
        // get keynote,
//        int keynote = projectManager->chordScannerProcessor->chordManager->get
        // get octave
        // get chordtype
        
        // create string
        
        // set to label
    }
    
    std::unique_ptr<ComboBox> comboBoxPlayingInstrument;
    
    Array<String> instrumentFilePath;
    String getInstrumentFilePath(int selectedID)
    {
        return instrumentFilePath[selectedID];
    }
    
    
    std::unique_ptr<TransportToolbarComponent> transportToolbar;
    
    std::unique_ptr<TextButton> button_WavetableEditor;
    WaveTableOscViewComponent * wavetableEditorComponent;
    PopupFFTWindow * popupWavetableWindow;
    
    // Waveform buttons
    std::unique_ptr<ImageButton> button_Default;
    std::unique_ptr<ImageButton> button_Sine;
    std::unique_ptr<ImageButton> button_Triangle;
    std::unique_ptr<ImageButton> button_Square;
    std::unique_ptr<ImageButton> button_Sawtooth;
    std::unique_ptr<ImageButton> button_Wavetable;    // wavetable
    
    //Chord Scan buttons
    std::unique_ptr<ImageButton> button_ScanOnlyMainChords;
    std::unique_ptr<ImageButton> button_ScanAllChords;
    std::unique_ptr<ImageButton> button_ScanSpecificRange;
    std::unique_ptr<ImageButton> button_ScanByFrequency;
    
    // Text Entry for Repeater
    std::unique_ptr<TextEditor> textEditorRepeat;
    std::unique_ptr<TextEditor> textEditorPause;
    std::unique_ptr<TextEditor> textEditorLength;
    
    
    // needs custom progress bar
    
    
    std::unique_ptr<VisualiserContainerComponent2> visualiserContainerComponent;
    
    
    // Image Cache
    Image imageBlueCheckButtonNormal;
    Image imageBlueCheckButtonSelected;

    Image imageBlueButtonNormal;
    Image imageBlueButtonSelected;

    // LookAndFeel
    CustomLookAndFeel  lookAndFeel;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChordScannerComponent)
};
