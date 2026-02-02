/*
  ==============================================================================

    LissajousCurveComponent.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "ProjectManager.h"
#include "CustomLookAndFeel.h"
#include "SpectrogramComponent.h"
#include "CustomRotarySlider.h"
#include "LissajousCurveAddChordComponent.h"
#include "SpectrogramComponent.h"
#include "CustomProgressBar.h"
#include "MenuViewInterface.h"
#include <memory>

class LissajousCurveComponent :
    public MenuViewInterface,
    public Button::Listener,
    public Slider::Listener,
    public TextEditor::Listener,
    public ProjectManager::UIListener
{
public:
    LissajousCurveComponent(ProjectManager * pm);
    ~LissajousCurveComponent();

    void paint (Graphics&) override;
    void resized() override;
    void buttonClicked (Button*button)override;
    void sliderValueChanged (Slider* slider)override;
    void textEditorReturnKeyPressed (TextEditor&editor)override;
    
    float scaleFactor = 0.5;
    void setScale(float factor) override
    {
        scaleFactor = factor;
        
        for (int i = 0; i < 3; i++)
        {
            settingsChordComponent[i]->setScale(scaleFactor);
            settingsFrequencyComponent[i]->setScale(scaleFactor);
        }
        
    }
    
    // Source container
    // need containers, one for chord, with original chord, keynote etc.. with close button, settings and mute
    // also need container for Wavefile, with close / delete file, load file & label for wavfile name
    void openSettingsView(int axis)
    {
        bool chordOrFreq = projectManager->getLissajousParameter(UNIT_1_PROCESSOR_TYPE + axis).operator bool();
        
        if (chordOrFreq)
        {
            settingsFrequencyComponent[axis]->openView(0);
        }
        else
        {
            settingsChordComponent[axis]->openView();
        }
        
        
    }
    void closeSettingsView()
    {
        
        settingsChordComponent[0]->setVisible(false);
        settingsChordComponent[1]->setVisible(false);
        settingsChordComponent[2]->setVisible(false);
        
        settingsFrequencyComponent[0]->setVisible(false);
        settingsFrequencyComponent[1]->setVisible(false);
        settingsFrequencyComponent[2]->setVisible(false);
    }
    
    void openLoadWaveFile()     { }
    void closeLoadWaveFile()    { }
    
    void updateChordParameters()
    {
//        int chordRef  = projectManager->getLissajousParameter(KEYNOTE).operator int()-1;
////        chord      = projectManager->getStringForStringArray(ProjectManager::STRING_ARRAY_KEYNOTE, chordRef);
//        chord = ProjectStrings::getKeynoteArray().getReference(chordRef);
//
//        label_ChordValue    ->setText(chord, dontSendNotification);
//
//        int chordt  = projectManager->getLissajousParameter(CHORD_TYPE).operator int()-1;
////        chordType   = projectManager->getStringForStringArray(ProjectManager::STRING_ARRAY_CHORDTYPE, chordt);
//        chordType   = ProjectStrings::getChordTypeArray().getReference(chordt);
//
//
//
//        label_ChordtypeValue->setText(chordType, dontSendNotification);

//        double freq
//
//        label_FrequencyValue->setText(frequencyHz, dontSendNotification);
    }

    
    void updateLissajousCurveUIParameter(int paramIndex) override;
    void updateSettingsUIParameter(int settingIndex)override;

private:
    ProjectManager * projectManager;
    
    Image background;
    Image imagePanicButton;
    Image imagePlayButton;
    Image imageStopButton;
    Image imageAddChordButton;
    Image imageAddWavefileButton;
    Image imageSettingsIcon;
    Image imageMuteIcon;
    Image imageDelete;
    
    Image imageBlueButtonNormal;
    Image imageBlueButtonSelected;
    
    Image imageChordFreqButtonNormal;
    Image imageChordFreqButtonSelected;
    
    Image imageSourceActive;
    Image imageSourceInActive;
    
    std::unique_ptr<ImageComponent> backgroundImageComponent;
    
    std::unique_ptr<ImageButton> button_Play;
    std::unique_ptr<ImageButton> button_Stop;
    std::unique_ptr<ImageButton> button_Panic;
    
    std::unique_ptr<ToggleButton> button_PlayInLoop;
    
    std::unique_ptr<Label> labelChordOrFrequency[3];
    std::unique_ptr<ImageButton> buttonChordOrFrequency[3];
    
    std::unique_ptr<ImageButton> buttonFreeFlow[3]; // free flow or sequencer
    std::unique_ptr<ImageButton> buttonSequencer[3]; // free flow or sequencer
    
    std::unique_ptr<CustomRotarySlider> slider_X;
    std::unique_ptr<CustomRotarySlider> slider_Y;
    std::unique_ptr<CustomRotarySlider> slider_Z;
    
    std::unique_ptr<TextEditor> textEditor_SliderX;
    std::unique_ptr<TextEditor> textEditor_SliderY;
    std::unique_ptr<TextEditor> textEditor_SliderZ;
    
    std::unique_ptr<Slider> slider_PhaseX;
    std::unique_ptr<Slider> slider_PhaseY;
    std::unique_ptr<Slider> slider_PhaseZ;
    
    std::unique_ptr<TextEditor> textEditor_SliderPhaseX;
    std::unique_ptr<TextEditor> textEditor_SliderPhaseY;
    std::unique_ptr<TextEditor> textEditor_SliderPhaseZ;
    
    std::unique_ptr<ImageButton> buttonOpenSettings[3]; // free flow or sequencer
    std::unique_ptr<ImageButton> buttonMute[3]; // free flow or sequencer
    
    std::unique_ptr<Label> labelFrequency[3];
    
    void updateLabelFrequency(int axis)
    {
        bool val = projectManager->getLissajousParameter(UNIT_1_PROCESSOR_TYPE+axis).operator bool();
        
        if (val)
        {
            int paramIndexBase =  UNIT_1_FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE + ((UNIT_2_FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE - UNIT_1_FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE) * axis);
            
            float freq = projectManager->getLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_CHOOSE_FREQ).operator float();
            
            String freqValString(freq, 3, false); freqValString.append("hz", 2);
            
            labelFrequency[axis]->setText(freqValString, dontSendNotification);
        }
        else
        {
            // chord
            int paramIndexBase =  UNIT_1_CHORD_PLAYER_SHORTCUT_IS_ACTIVE + ((UNIT_2_CHORD_PLAYER_SHORTCUT_IS_ACTIVE - UNIT_1_CHORD_PLAYER_SHORTCUT_IS_ACTIVE) * axis);
            
            int chord = projectManager->getLissajousParameter(paramIndexBase + CHORD_TYPE).operator int();
            
            int octave = projectManager->getLissajousParameter(paramIndexBase + OCTAVE).operator int();
            
            int keynote = projectManager->getLissajousParameter(paramIndexBase + KEYNOTE).operator int();
            
            String chordString(ProjectStrings::getChordTypeArray().getReference(chord-1));
            String octaveString(octave);
            String keynoteString(ProjectStrings::getKeynoteArray().getReference(keynote-1));
            
            String finalString(keynoteString); finalString.append(octaveString, 3); finalString.append(chordString ,15);
            
            labelFrequency[axis]->setText(finalString, dontSendNotification);
            
            
        }
    }
    
    std::unique_ptr<LissajousChordPlayerSettingsComponent> settingsChordComponent[3];
    std::unique_ptr<LissajousFrequencyPlayerSettingsComponent> settingsFrequencyComponent[3];
    
    std::unique_ptr<LissajousCurveViewerComponent> viewerComponent;

    // LookAndFeel
    CustomLookAndFeel  lookAndFeel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LissajousCurveComponent)
};
