#pragma once

#include <JuceHeader.h>
#include <ProjectManager.h>
#include <utility_components/Containers.h>
#include <CustomLookAndFeel.h>
#include "utility_components/ComboBox.h"
#include "CustomRotarySlider.h"

class FeedbackSpectrumSettingsComponent :
    public Component,
    ComboBox::Listener,
    Button::Listener,
    TextEditor::Listener,
    ProjectManager::UIListener
{
public:
    FeedbackSpectrumSettingsComponent(ProjectManager& pm, FundamentalFrequencyProcessor& ffp);
    ~FeedbackSpectrumSettingsComponent();
    void setScale(const float factor);
private:
    void paint(Graphics& g) override;
    void resized() override;
    void prepareFftParameters();
    void prepareThresholdAndTolerance();
    void prepareRange();
    void prepareProcess();

    void comboBoxChanged(ComboBox *comboBoxThatHasChanged) override;
    void textEditorReturnKeyPressed(TextEditor&editor) override;
    void buttonClicked(Button *button) override;
    void updateSettingsUIParameter(int settingIndex) override;

    ProjectManager& projectManager;
    FundamentalFrequencyProcessor& frequencyProcessor;
    CustomLookAndFeel lookAndFeel;
    asvpr::Containers containers;

    /* FFT PARAMETERS */
    asvpr::ComboBoxWithAttachment comboBoxFFTWindow{"FFT Window"};
    asvpr::ComboBoxWithAttachment comboBoxFFTSize{"FFT Size"};
    asvpr::ComboBoxWithAttachment comboBoxAlgorithm{"Algorithm"};
    asvpr::ComboBoxWithAttachment comboBoxNumHarmonics{"Num Harmonics"};

    /* Threshold and Tolerance Parameters */
    CustomRotarySlider sliderThresholdInputDetection{CustomRotarySlider::ROTARY_TYPE::ROTARY_RELEASE}; // db -80db..0db
    CustomRotarySlider sliderKeynoteTolerance{CustomRotarySlider::ROTARY_TYPE::ROTARY_RELEASE}; // +/- 0 - 50%
    CustomRotarySlider sliderMinIntervalSize{CustomRotarySlider::ROTARY_TYPE::ROTARY_RELEASE}; // semitones or % of octave
    CustomRotarySlider sliderMaxIntervalSize{CustomRotarySlider::ROTARY_TYPE::ROTARY_RELEASE}; // semitones or % of octave

    /* Range Parameters */
    ImageButton buttonCustomFrequencyRange;
    TextEditor textEditorMinFrequency;
    TextEditor textEditorMaxFrequency;

    /* Process and Synth Parameters */
    ImageButton buttonProcessFFT;
    TextEditor textEditorLength;
    TextEditor textEditorIteration;

    ImageButton closeButton, applyButton;

    struct Bounds {
        static inline float scale {0.5f};
        static inline Rectangle<int> container = Rectangle<int>{74, 49, 1416, 861};
        static inline Rectangle<int> closeButton = Rectangle<int>{1428, 80, 28, 28};
        static inline Rectangle<int> applyButton = Rectangle<int>{694, 822, 171, 51};

        static inline Rectangle<int> fftParameters = Rectangle<int>{174, 153, 1221, 186};
        static inline Rectangle<int> threshold = Rectangle<int>{174, 356, 833, 261};
        static inline Rectangle<int> range = Rectangle<int>{1024, 356, 371, 261};
        static inline Rectangle<int> process = Rectangle<int>{174, 640, 1221, 158};

        static inline Rectangle<int> fftSizeCombobox = Rectangle<int>{36, 88, 208, 76};
        static inline Rectangle<int> fftWindowCombobox = Rectangle<int>{334, 88, 208, 76};
        static inline Rectangle<int> fftAlgorithmCombobox = Rectangle<int>{656, 88, 208, 76};
        static inline Rectangle<int> fftNumHarmonicsCombobox = Rectangle<int>{967, 88, 208, 76};

        static inline Rectangle<int> inputThreshold = Rectangle<int>{36, 68, 161, 146};
        static inline Rectangle<int> keynoteTolerance = Rectangle<int>{231, 68, 161, 146};
        static inline Rectangle<int> minInterval = Rectangle<int>{433, 68, 161, 146};
        static inline Rectangle<int> maxInterval = Rectangle<int>{639, 68, 161, 146};

        static inline Rectangle<int> customRangeBox = Rectangle<int>{34, 77, 33, 33};
        static inline Rectangle<int> customRangeText = Rectangle<int>{72, 72, 165, 45};
        static inline Rectangle<int> minFrequency = Rectangle<int>{33, 191, 151, 41};
        static inline Rectangle<int> maxFrequency = Rectangle<int>{204, 191, 151, 41};

        static inline Rectangle<int> processFftBox = Rectangle<int>{38, 83, 33, 33};
        static inline Rectangle<int> processFftText = Rectangle<int>{77, 75, 212, 50};
        static inline Rectangle<int> numIterations = Rectangle<int>{609, 81, 113, 39};
        static inline Rectangle<int> numIterationsText = Rectangle<int>{384, 75, 212, 50};
        static inline Rectangle<int> iteratioLength = Rectangle<int>{1048, 81, 113, 39};
        static inline Rectangle<int> iterationLengthText = Rectangle<int>{821, 75, 212, 50};

        static Rectangle<int> get(Rectangle<int> rectangle)
        {
            return rectangle.transformedBy(AffineTransform::scale(scale));
        }
    };

};

