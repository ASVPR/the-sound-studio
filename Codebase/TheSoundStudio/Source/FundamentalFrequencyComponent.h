/*
  ==============================================================================

    FundamentalFrequencyComponent.h
    Created: 17 Mar 2021 9:47:28pm
    Author:  Gary Jones

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "ProjectManager.h"
#include "VisualiserContainerComponent.h"
#include "utility_components/Containers.h"
#include "utility_components/PlayerButton.h"
#include "utility_components/ComboBox.h"
#include "Analyzer.h"
#include "FundamentalFrequencyProcessor.h"
#include "MenuViewInterface.h"

class FundamentalFrequencyComponent :
    public MenuViewInterface,
    public ProjectManager::UIListener,
    public juce::Button::Listener,
    public juce::ComboBox::Listener,
    public juce::TextEditor::Listener,
    public juce::Timer,
    public Analyser::Listener
{
public:

    FundamentalFrequencyComponent(ProjectManager& pm);
    
    ~FundamentalFrequencyComponent() override;

    void resized() override;
    
    void paint(Graphics&g)override;

    void recalcCoordinates();

    void updateFrequencyOutput();

    void buttonClicked (Button*button) override;
    
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged) override;

    void textEditorReturnKeyPressed (TextEditor&editor) override;
    
    void timerCallback() override
    {
        auto it = frequencyProcessor.get_iterations() == 0;
        if (!frequencyProcessor.is_doing_analysis() || it)
        {
            if (!it)
            {
                buttonStart->resetButtonText();
                buttonStart->resetButtonColour();
                buttonStart->setEnabled(true);
                visualiserSelectorComponent->setProcessingActive(false);
            }

            if (frequencyProcessor.should_wait_for_harmonics())
            {
                return;
            }
            
            updateFrequencyOutput();
        }

        // Auto A logic: if enabled, infer A4 from current input
        if (autoAEnabled)
        {
            const double f = frequencyProcessor.getCurrentFrequency();
            if (f >= autoASetMinHz && f <= autoASetMaxHz)
            {
                int midiNote = -1;
                int keynote = -1;
                int octave = -1;
                float freqDiff = 0.0f;
                projectManager.frequencyManager->getMIDINoteForFrequency((float) f, midiNote, keynote, octave, freqDiff);

                // Expect keynote 9 to correspond to 'A'
                if (keynote == 9)
                {
                    if (autoALastKeynote == 9)
                        ++autoAConsistencyCount;
                    else
                        autoAConsistencyCount = 1;

                    autoALastKeynote = 9;

                    if (autoAConsistencyCount >= autoARequiredCount)
                    {
                        projectManager.setProjectSettingsParameter(BASE_FREQUENCY_A, f);
                        autoAConsistencyCount = 0;
                    }
                }
                else
                {
                    autoALastKeynote = keynote;
                    autoAConsistencyCount = 0;
                }
            }
            else
            {
                autoAConsistencyCount = 0;
            }
        }
    }
    
    float scaleFactor = 0.5;

    void setScale(float factor) override
    {
        scaleFactor = factor;
        lookAndFeel.setScale(scaleFactor);

    }

    void prepare_fft_parameters();

    void prepare_thresholdAndTolerance();

    void prepare_range();

    void prepare_process_and_synth();

    void prepare_harmonics_and_results();

    template <typename HarmonicData>
    juce::String getChordNameFromHarmonicData(const HarmonicData& data);

    static juce::String getKeyNoteString(int keynote);

    void updateIterationUI();

    void fft_updated() override;

    void updateSettingsUIParameter(int settingIndex) override;

private:

    ProjectManager& projectManager;

    FundamentalFrequencyProcessor& frequencyProcessor;

    CustomLookAndFeel lookAndFeel;

    std::unique_ptr<VisualiserSelectorComponent> visualiserSelectorComponent;
    
    std::unique_ptr<asvpr::PlayerButton> buttonStart;
    std::unique_ptr<asvpr::PlayerButton> buttonStop;
    std::unique_ptr<ImageButton> noiseButton;

    /* FFT PARAMETERS */
    std::unique_ptr<asvpr::ComboBoxWithAttachment> comboBoxFFTSize;
    std::unique_ptr<asvpr::ComboBoxWithAttachment> comboBoxFFTWindow;
    std::unique_ptr<asvpr::ComboBoxWithAttachment> comboBoxInput;
    std::unique_ptr<asvpr::ComboBoxWithAttachment> comboBoxAlgorithm;
    std::unique_ptr<asvpr::ComboBoxWithAttachment> comboBoxNumHarmonics;
    /* ************** */

    /* Threshold and Tolerance Parameters */
    std::unique_ptr<CustomRotarySlider>       sliderThresholdInputDetection; // db -80db..0db
    std::unique_ptr<CustomRotarySlider>       sliderKeynoteTolerance; // +/- 0 - 50%
    std::unique_ptr<CustomRotarySlider>       sliderMinIntervalSize; // semitones or % of octave
    std::unique_ptr<CustomRotarySlider>       sliderMaxIntervalSize; // semitones or % of octave
    /* ************** */

    /* Range Parameters */
    std::unique_ptr<ImageButton>   buttonCustomFrequencyRange;
    std::unique_ptr<juce::TextEditor>   textEditorMinFrequency;
    std::unique_ptr<juce::TextEditor>   textEditorMaxFrequency;
    /* ************** */

    /* Process and Synth Parameters */
    std::unique_ptr<ImageButton>   buttonProcessFFT;
    std::unique_ptr<juce::TextEditor> textEditorLength;
    std::unique_ptr<juce::TextEditor> textEditorIteration;
    /* ************** */

    // Auto A (set A4 from input) toggle and helpers
    std::unique_ptr<ToggleButton> toggleAutoA;
    bool autoAEnabled { false };
    int  autoAConsistencyCount { 0 };
    int  autoARequiredCount { 6 }; // require consecutive confirmations
    int  autoALastKeynote { -1 };
    double autoASetMinHz { 380.0 };
    double autoASetMaxHz { 500.0 };

    // Range preset buttons
    std::unique_ptr<TextButton> buttonRangeFull;
    std::unique_ptr<TextButton> buttonRangeLow;
    std::unique_ptr<TextButton> buttonRangeVocal;

    // IR Convolution Controls (WAV only)
    std::unique_ptr<ToggleButton> toggleIREnable;
    std::unique_ptr<Slider>       sliderIRWet;
    std::unique_ptr<TextButton>   buttonIRLoad;

    std::unique_ptr<Label> labelFrequency;
    std::unique_ptr<Label> labelChord;

    std::unique_ptr<Component> containerHarmonicData;
    // Harmonic Data Feedback
    std::array<Label, 10> labelHarmonic;

    bool logToFile {false};

    int fftLeftMargin       = 48;
    int fftTopMargin        = 210;
    int fftWidth            = 1476;
    int fftShortWidth       = 1100;
    int fftHeight           = 340;
    int fftShortHeight      = 254;

    asvpr::Containers containers;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FundamentalFrequencyComponent)

};
