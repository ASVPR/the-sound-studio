/*
  ==============================================================================

    ChordScannerComponent.h
    Created: 13 Mar 2019 9:55:10pm
    Author:  Ziv Elovitch - The Sound Studio Team
    
    ChordScannerComponent provides chord scanning and analysis functionality.
    Uses modern C++ smart pointer management for UI component lifecycle.

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
#include <memory>
#include "MenuViewInterface.h"

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
    
    
    std::unique_ptr<ImageButton> button_Record;
    std::unique_ptr<ImageButton> button_Play;
    std::unique_ptr<ImageButton> button_Stop;
    std::unique_ptr<ImageButton> button_Panic;
    std::unique_ptr<Label> label_Playing;
    
    // need checkbox

    std::unique_ptr<TextButton>   button_Save;
    std::unique_ptr<TextButton>   button_Load;
    
    std::unique_ptr<TextButton> button_WavetableEditor;
    WaveTableOscViewComponent * wavetableEditorComponent;
    PopupFFTWindow * popupWavetableWindow;
    
    // needs custom progress bar
    std::unique_ptr<CustomProgressBar> progressBar;
    
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
    Image imageCheckboxBackground;
    Image imageMainBackground;
    
    Image imageBlueCheckButtonNormal;
    Image imageBlueCheckButtonSelected;

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
    
    int playingLeftMargin   = 90;
    int playingTopMargin    = 374;
    int playingWidth        = 205;
    int playingHeight       = 33;
    
    int playLeftMargin      = 521;
    int playTopMargin       = 1352;
    int playWidth           = 249;
    int playHeight          = 61;
    
    int stopLeftMargin      = 802;
    int stopTopMargin       = 970;
    
    int panicLeftMargin     = 1390;
    int panicTopMargin      = 1280;
    int panicWidth          = 180;
    int panicHeight         = panicWidth;
    
    int progressLeftMargin  = 220;
    int progressTopMargin   = 856;
    int progressWidth       = 1128;
    int progressHeight      = 53;
    
    
    // LookAndFeel
    CustomLookAndFeel  lookAndFeel;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChordScannerComponent)
};
