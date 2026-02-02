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
#include "MenuViewInterface.h"
#include <memory>

class FrequencyScannerComponent :
    public MenuViewInterface,
    public Button::Listener,
    public ProjectManager::UIListener,
    public Slider::Listener,
    public ComboBox::Listener,
    public TextEditor::Listener,
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
    
    
    std::unique_ptr<ImageButton> button_Record;
    std::unique_ptr<ImageButton> button_Play;
    std::unique_ptr<ImageButton> button_Stop;
    std::unique_ptr<ImageButton> button_Panic;
    std::unique_ptr<Label> label_Playing;
    
    // need checkbox
    
    std::unique_ptr<TextButton>   button_Save;
    std::unique_ptr<TextButton>   button_Load;
    
    // needs custom progress bar
    std::unique_ptr<CustomProgressBar> progressBar;
    
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
    Image imageCheckboxBackground;
    Image imageMainBackground;
    
    Image imagePanicButton;
    Image imagePlayButton;
    Image imageProgresBarFill;
    Image imageSettingsIcon;
    Image imageAddIcon;
    Image imageCloseIcon;
    Image imageLeftIcon;
    Image imageRightIcon;
    Image imageLoopIcon;
    Image imageMuteIcon;
    Image imageStopButton;
    Image imageRecordButton;
    
    Image imageBlueButtonNormal;
    Image imageBlueButtonSelected;
    Image imageAddButton;
    Image imageBlueCheckButtonNormal;
    Image imageBlueCheckButtonSelected;
    
    Image imageFFTMockup;
    Image imageColorSpectrumMockup;
    Image imageOctaveSpectrumMockup;
    
    
    // UI Layout Variables
    int mainContainerHeight = 1096;
    
    int fftLeftMargin       = 48;
    int fftTopMargin        = 64;
    int fftWidth            = 1476;
    int fftHeight           = 310;
    
    int colorLeftMargin     = fftLeftMargin;
    int colorTopMargin      = fftTopMargin + fftHeight + 67;
    int colorWidth          = 742;
    int colorHeight         = 271;
    
    int octaveLeftMergin    = 822;
    int octaveTopMargin     = colorTopMargin;
    int octaveWidth         = 699;
    int octaveHeight        = colorHeight;
    
    int shortcutHeight      = 344;
    int shortcutWidth       = 1566;
    
    int recordLeftMargin    = 102;
    int recordTopMargin     = 835;
    int recordWidth         = 95;
    int recordHeight        = 95;
    
    int playingLeftMargin   = 30;
    int playingTopMargin    = 374;
    int playingWidth        = 205;
    int playingHeight       = 33;
    
    int playLeftMargin      = 521;
    int playTopMargin       = 1352;
    int playWidth           = 249;
    int playHeight          = 61;
    
    int stopLeftMargin      = 802;
    int stopTopMargin       = 970;
    
    int panicLeftMargin     = 1360;
    int panicTopMargin      = 1280;
    int panicWidth          = 180;
    int panicHeight         = panicWidth;
    
    int progressLeftMargin  = 220;
    int progressTopMargin   = 856;
    int progressWidth       = 1128;
    int progressHeight      = 53;
    
    
    // LookAndFeel
    CustomLookAndFeel  lookAndFeel;
    
    
    
    
    
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FrequencyScannerComponent)
};
