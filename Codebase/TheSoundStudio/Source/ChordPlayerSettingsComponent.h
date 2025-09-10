/*
  ==============================================================================

    ChordPlayerSettingsComponent.h
    Created: 14 Mar 2019 9:01:08pm
    Author:  Ziv Elovitch - The Sound Studio Team

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "CustomLookAndFeel.h"
#include "CustomChordComponent.h"
#include "ProjectManager.h"
#include "CustomRotarySlider.h"
#include "WaveTableOscViewComponent.h"
#include "PopupFFTWindow.h"
#include <memory>

//==============================================================================
/*
*/


class AddOnPopupComponent : public Component, public Button::Listener
{
public:

    AddOnPopupComponent(ProjectManager * pm);
    ~AddOnPopupComponent();
    
    void paint (Graphics&g) override;
    void buttonClicked (Button*button)override;
    void setShortcutRef(int newShortcutRef);
    void syncGUI();
    
    float scaleFactor = 0.f;
    void setScale(float factor) { scaleFactor = factor; }
    
    void resized() override;
    
private:
    ProjectManager * projectManager;
    int   shortcutRef;
    Image backgroundImage;
    Image imageBlueCheckButtonNormal;
    Image imageBlueCheckButtonSelected;
    std::unique_ptr<ImageButton> addonButton[NUM_ADDONS];
    std::unique_ptr<Label> label[NUM_ADDONS];
};


class ChordPlayerSettingsComponent    : public Component, public Button::Listener, public Slider::Listener, public ComboBox::Listener, public TextEditor::Listener, public ProjectManager::UIListener
{
public:
    ChordPlayerSettingsComponent(ProjectManager * pm);
    ~ChordPlayerSettingsComponent();

    void paint (Graphics&g) override;
    void resized() override;
    
    void buttonClicked (Button*button)override;
    void sliderValueChanged (Slider* slider)override;
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged)override;
    void textEditorReturnKeyPressed (TextEditor&editor)override;
    
    void openView(int shortcut);
    void closeView();
    
    void handleWaveformToggle() {}
    void handleChordOriginToggle() {}
    void handleManipulateFrequency() {}
    
    void syncUI();

    void updateMinMaxSettings(int paramIndex) override;
    
    void updateScalesComponents()
    {
        ScalesManager * sm = projectManager->frequencyManager->scalesManager;
        
        sm->getComboBoxPopupMenuForChords(*comboBoxChordtype->getRootMenu(), (SCALES_UNIT)shortcutRef);
        sm->getComboBoxPopupMenuForKeynotes(*comboBoxKeynote->getRootMenu(), (SCALES_UNIT)shortcutRef);
        
        comboBox_Scales->setSelectedId(projectManager->getChordPlayerParameter(shortcutRef, CHORDPLAYER_SCALE));
        
        // update for custom chord
        
        customChordPopupComponent->syncGUI();
    }
    
    float scaleFactor = 0.5;
    void setScale(float factor)
    {
        scaleFactor = factor;
        
        customChordPopupComponent   ->setScale(scaleFactor);
        addOnPopupComponent         ->setScale(scaleFactor);
        
        lookAndFeel.setScale(scaleFactor);
    }
    
private:
    
    ProjectManager * projectManager;
    
    bool isSynced;
    int shortcutRef;
    
    std::unique_ptr<CustomChordPopupComponent> customChordPopupComponent;
    std::unique_ptr<AddOnPopupComponent> addOnPopupComponent;

    std::unique_ptr<ImageButton> button_Close;
    
    // toggle between these two
    std::unique_ptr<ImageButton> button_ChooseChordFromList;
    std::unique_ptr<ImageButton> button_ChooseChordFromFrequency;

    std::unique_ptr<CustomRotarySlider> slider_Amplitude;
    std::unique_ptr<CustomRotarySlider> slider_Attack;
    std::unique_ptr<CustomRotarySlider> slider_Sustain;
    std::unique_ptr<CustomRotarySlider> slider_Decay;
    std::unique_ptr<CustomRotarySlider> slider_Release;
    
    // act as toggle between 5 buttons
    std::unique_ptr<ImageButton> button_Default;      // sampler
    std::unique_ptr<ImageButton> button_Sine;         // synth
    std::unique_ptr<ImageButton> button_Triangle;
    std::unique_ptr<ImageButton> button_Square;
    std::unique_ptr<ImageButton> button_Sawtooth;
    std::unique_ptr<ImageButton> button_Wavetable;    // wavetable
    
    std::unique_ptr<TextButton> button_AddCustomChord; // opens popup..
    std::unique_ptr<TextButton> button_Addons; // opens popup..
    
    std::unique_ptr<ImageButton> button_Add;
    
    std::unique_ptr<TextButton>   button_WavetableEditor;
    
    // Owned by PopupFFTWindow via setContentOwned; keep a non-owning pointer here
    WaveTableOscViewComponent* wavetableEditorComponent { nullptr };
    std::unique_ptr<PopupFFTWindow>            popupWavetableWindow;
    
    // Combobox
    std::unique_ptr<ComboBox> comboBoxKeynote;
    std::unique_ptr<ComboBox> comboBoxChordtype;
    
    std::unique_ptr<TextEditor> textEditor_Octave;
    
    std::unique_ptr<ComboBox> comboBoxPlayingInstrument;
    std::unique_ptr<ComboBox> comboBox_Scales;
    
    std::unique_ptr<ComboBox> comboBoxOutputSelection;
    
    // Text Entry for Repeater
    std::unique_ptr<TextEditor> textEditorRepeat;
    std::unique_ptr<TextEditor> textEditorPause;
    std::unique_ptr<TextEditor> textEditorLength;
    
    std::unique_ptr<ImageButton> button_ManipulateFreq;
    
    // toggle between these two
    std::unique_ptr<ImageButton> button_Multiplication;
    std::unique_ptr<ImageButton> button_Division;
    
    // TextEntryBoxes
    std::unique_ptr<TextEditor> textEditorInsertFreq;
    std::unique_ptr<TextEditor> textEditorMultiplication;
    std::unique_ptr<TextEditor> textEditorDivision;
    
    std::unique_ptr<ImageButton> button_Panic;
    Image imagePanicButton;
    
    // Labels
    Label labelInsertFrequency;
    Label labelClosestChord;
    Label labelEstimatedOctave;
    
    Label labelClosestChordValue;
    Label labelEstimatedOctaveValue;
    
    // Images
    Image mainBackgroundImage;
    Image sectionTopLeftBackground;
    Image section;
    
    Image imageCloseButton;
    Image imageBlueButtonNormal;
    Image imageBlueButtonSelected;
    Image imageAddButton;
    Image imageBlueCheckButtonNormal;
    Image imageBlueCheckButtonSelected;
    
    Image imageSliderKnob;
    Image imageSliderBackground;
    
    // LookAndFeel
    CustomLookAndFeel  lookAndFeel;
    
    Array<String> instrumentFilePath;
    String getInstrumentFilePath(int selectedID)
    {
        return instrumentFilePath[selectedID];
    }
    
    
    
    

    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChordPlayerSettingsComponent)
};
