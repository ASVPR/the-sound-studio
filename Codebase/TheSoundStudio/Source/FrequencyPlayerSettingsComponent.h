/*
  ==============================================================================

    FrequencyPlayerSettingsComponent.h
    Created: 30 May 2019 2:47:58pm
    Author:  Gary Jones

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "CustomLookAndFeel.h"
#include "ProjectManager.h"
#include "CustomRotarySlider.h"
#include "WaveTableOscViewComponent.h"
#include "PopupFFTWindow.h"
#include <memory>

//==============================================================================
/*
*/
class FrequencyPlayerSettingsComponent    : public Component, public Button::Listener, public TextEditor::Listener, public ProjectManager::UIListener, public Slider::Listener, public ComboBox::Listener
{
public:
    FrequencyPlayerSettingsComponent(ProjectManager * pm);
    ~FrequencyPlayerSettingsComponent();

    void paint (Graphics&g) override;
    void resized() override;
    
    void buttonClicked (Button*button)override;
    void textEditorReturnKeyPressed (TextEditor&editor)override;
    void sliderValueChanged (Slider* slider) override;
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged)override;
    
    void openView(int shortcut);
    void closeView();
    
    void handleWaveformToggle() {}
    void handleChordOriginToggle() {}
    void handleManipulateFrequency() {}
    
    void syncUI();
    
    void updateMinMaxSettings(int paramIndex) override;
    
    float scaleFactor = 0.5;
    void setScale(float factor)
    {
        scaleFactor = factor;
        lookAndFeel.setScale(scaleFactor);
    }
    
private:
    
    ProjectManager * projectManager;
    bool isSynced;
    int shortcutRef;
    
    std::unique_ptr<ImageButton> button_Close;
    
    // toggle between these two
    std::unique_ptr<ImageButton> button_ChooseSpecificFrequency;
    std::unique_ptr<ImageButton> button_ChooseRangeOfFrequencies;
    
    std::unique_ptr<ComboBox> comboBoxOutputSelection;

    
    // act as toggle between 5 buttons
    std::unique_ptr<ImageButton> button_Default;
    std::unique_ptr<ImageButton> button_Sine;
    std::unique_ptr<ImageButton> button_Triangle;
    std::unique_ptr<ImageButton> button_Square;
    std::unique_ptr<ImageButton> button_Sawtooth;
    std::unique_ptr<ImageButton> button_Wavetable;    // wavetable
    
    std::unique_ptr<ImageButton> button_Add;
    
    std::unique_ptr<TextButton>   button_WavetableEditor;
    // Owned by PopupFFTWindow via setContentOwned; keep a non-owning pointer here
    WaveTableOscViewComponent* wavetableEditorComponent { nullptr };
    std::unique_ptr<PopupFFTWindow> popupWavetableWindow;
    
    // Text Entry for Repeater
    std::unique_ptr<TextEditor> textEditorRepeat;
    std::unique_ptr<TextEditor> textEditorPause;
    std::unique_ptr<TextEditor> textEditorLength;
    
    std::unique_ptr<ImageButton> button_ManipulateFreq;
    
    // new stuff
    std::unique_ptr<ImageButton> button_Log;
    
    std::unique_ptr<TextEditor> textEditorRangeLength;
    
    std::unique_ptr<CustomRotarySlider> slider_Amplitude;
    std::unique_ptr<CustomRotarySlider> slider_Attack;
    std::unique_ptr<CustomRotarySlider> slider_Sustain;
    std::unique_ptr<CustomRotarySlider> slider_Decay;
    std::unique_ptr<CustomRotarySlider> slider_Release;
    
    // toggle between these two
    std::unique_ptr<ImageButton> button_Multiplication;
    std::unique_ptr<ImageButton> button_Division;
    
    
    // TextEntryBoxes
    std::unique_ptr<TextEditor> textEditorInsertFreq;
    std::unique_ptr<TextEditor> textEditorFreqFrom;
    std::unique_ptr<TextEditor> textEditorFreqTo;
    std::unique_ptr<TextEditor> textEditorMultiplication;
    std::unique_ptr<TextEditor> textEditorDivision;

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

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FrequencyPlayerSettingsComponent)
};
