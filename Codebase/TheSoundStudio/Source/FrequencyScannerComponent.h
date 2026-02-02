/*
  ==============================================================================

    FrequencyScannerComponent.h

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
#include "PopupFFTWindow.h"
#include "TransportToolbarComponent.h"
#include "MenuViewInterface.h"
#include "UI/DesignSystem.h"
#include <memory>

class FrequencyScannerComponent :
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
    FrequencyScannerComponent(ProjectManager * pm);
    ~FrequencyScannerComponent();

    void paint (Graphics&) override;
    void resized() override;
    
    void buttonClicked (Button*button)override;
    void sliderValueChanged (Slider* slider)override;
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged)override;
    void textEditorReturnKeyPressed (TextEditor&editor)override;
    
    void syncUI();
    
    void timerCallback() override;
    
    void updateFrequencyScannerUIParameter(int paramIndex)override;
    
    void updateMinMaxSettings(int paramIndex) override;

    // TransportToolbarComponent::Listener
    void transportPlayClicked() override;
    void transportStopClicked() override;
    void transportPanicClicked() override;
    void transportSaveClicked() override;
    void transportLoadClicked() override;

    void closePopups()
    {
        visualiserContainerComponent->setPopupsAreVisible(false);
    }
    
    float scaleFactor = 0.5;
    void setScale(float factor) override
    {
        scaleFactor = factor;
        lookAndFeel.setScale(scaleFactor);
        visualiserContainerComponent->setScale(scaleFactor);
    }

private:
    
    ProjectManager * projectManager;
    
    std::unique_ptr<CustomRotarySlider> slider_Amplitude;
    std::unique_ptr<CustomRotarySlider> slider_Attack;
    std::unique_ptr<CustomRotarySlider> slider_Sustain;
    std::unique_ptr<CustomRotarySlider> slider_Decay;
    std::unique_ptr<CustomRotarySlider> slider_Release;
    
    std::unique_ptr<Label> label_CurrentPlayingFrequency;

    std::unique_ptr<TextEditor> textEditorFrequencyFrom;
    std::unique_ptr<TextEditor> textEditorFrequencyTo;

    std::unique_ptr<Component> component_FrequencyContainer;
    
    std::unique_ptr<ComboBox> comboBoxOutputSelection;

    
    void setChordStringForCurrentChordLabel()
    {
    }
    
    
    Array<String> instrumentFilePath;
    String getInstrumentFilePath(int selectedID)
    {
        return instrumentFilePath[selectedID];
    }
    
    
    std::unique_ptr<TransportToolbarComponent> transportToolbar;
    
    // Waveform buttons
    std::unique_ptr<ImageButton> button_Default;
    std::unique_ptr<ImageButton> button_Sine;
    std::unique_ptr<ImageButton> button_Triangle;
    std::unique_ptr<ImageButton> button_Square;
    std::unique_ptr<ImageButton> button_Sawtooth;
    std::unique_ptr<ImageButton> button_Wavetable;    // wavetable
    
    std::unique_ptr<TextButton>   button_WavetableEditor;
    // Owned by PopupFFTWindow via setContentOwned; keep a non-owning pointer here
    WaveTableOscViewComponent* wavetableEditorComponent { nullptr };
    std::unique_ptr<PopupFFTWindow> popupWavetableWindow;
    
    //Chord Scan buttons
    std::unique_ptr<ImageButton> button_ScanAllFrequencies;
    std::unique_ptr<ImageButton> button_ScanSpecificRange;
    
    std::unique_ptr<ImageButton> button_ExtendedRange;
    
    //Log / Lin buttons
    std::unique_ptr<ImageButton> button_Log;
    std::unique_ptr<ImageButton> button_Lin;
    std::unique_ptr<TextEditor> textEditor_LogValue;
    std::unique_ptr<TextEditor> textEditor_LinValue;
    
    // Text Entry for Repeater
    std::unique_ptr<TextEditor> textEditorRepeat;
    std::unique_ptr<TextEditor> textEditorPause;
    std::unique_ptr<TextEditor> textEditorLength;
    
    
    // needs custom progress bar
    std::unique_ptr<VisualiserContainerComponent2> visualiserContainerComponent;
    
    
    // Image Cache
    Image imageBlueButtonNormal;
    Image imageBlueButtonSelected;
    Image imageBlueCheckButtonNormal;
    Image imageBlueCheckButtonSelected;

    // LookAndFeel
    CustomLookAndFeel  lookAndFeel;
    
    
    
    
    
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FrequencyScannerComponent)
};
