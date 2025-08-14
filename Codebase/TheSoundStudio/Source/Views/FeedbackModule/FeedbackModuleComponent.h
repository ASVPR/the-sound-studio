/*
  ==============================================================================

    FeedbackModuleComponent.h
    Created: 27 Feb 2021 7:38:30pm
    Author:  Gary Jones

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ProjectManager.h"
#include "FeedbackModuleComponent.h"
#include "VisualiserContainerComponent.h"
#include "utility_components/Containers.h"
#include "MenuViewInterface.h"
#include "FrequencyPlayerSettingsComponent.h"
#include "ChordPlayerSettingsComponent.h"
#include "FeedbackModuleParameterSettingsComponent.h"
#include "FeedbackInputOutputComponent.h"
#include "Support/FoldableContainer.h"
#include "Support/ChannelHeader.h"
#include "Support/InputChannelBody.h"
#include "Support/Holder.h"

class FeedbackModuleComponent :
    public MenuViewInterface,
    public Button::Listener,
    public ProjectManager::UIListener,
    public ComboBox::Listener
{
public:
    FeedbackModuleComponent(FeedbackModuleProcessor &feedbackModule);
    ~FeedbackModuleComponent() override;
    void paint(juce::Graphics &g) override;
    void resized() override;
    void buttonClicked(Button *button) override;
    void comboBoxChanged(ComboBox *comboBoxThatHasChanged) override;
    void createContainers();
    void updateFrequencyPlayerUIParameter(int shortcutRef, int paramIndex) override;
    void updateChordPlayerUIParameter(int shortcutRef, int paramIndex) override;
    void setScale(float scaleFactor) override;

private:
    FeedbackModuleProcessor &processor;
    ProjectManager& projectManager;
    FeedbackInputOutputComponent inputOutputComponent;
    Viewport viewport;

    TextButton setParametersButton;
    ImageButton semiAutomaticModeButton, automaticModeButton;
    TextButton chordButton, frequencyButton;
    Label algorithmResultLabel;
    asvpr::PlayerButton playPauseButton{asvpr::PlayerButton::Type::PlayPause}, stopButton{asvpr::PlayerButton::Type::Stop};
    ImageButton noiseButton;

    FrequencyPlayerSettingsComponent frequencyPlayerSettingsComponent;
    ChordPlayerSettingsComponent chordPlayerSettingsComponent;
    FeedbackModuleParameterSettingsComponent parameterSettingsComponent;

    struct Bounds {
        static inline float scale {0.5f};
        static inline Rectangle<int> setParameterContainer = Rectangle<int>{366, 32, 354, 194};
        static inline Rectangle<int> setParametersButton = Rectangle<int>{430, 105, 225, 51};

        static inline Rectangle<int> algorithmContainer = Rectangle<int>{736, 32, 482, 194};
        static inline Rectangle<int> semiAutomaticButton = Rectangle<int>{761, 57, 39, 39};
        static inline Rectangle<int> semiAutomaticText = Rectangle<int>{803, 57, 180, 39};
        static inline Rectangle<int> automaticButton = Rectangle<int>{1014, 57, 39, 39};
        static inline Rectangle<int> automaticText = Rectangle<int>{1055, 57, 130, 39};
        static inline Rectangle<int> chordButton = Rectangle<int>{1014, 112, 71, 33};
        static inline Rectangle<int> frequencyButton = Rectangle<int>{1093, 112, 107, 33};
        static inline Rectangle<int> algorithmResultLabel = Rectangle<int>{1013, 160, 186, 48};

        static inline Rectangle<int> resultsText = Rectangle<int>{722, 268, 135, 45};

        static inline Rectangle<int> playPauseButton = Rectangle<int>{753, 1345, 186, 50};
        static inline Rectangle<int> stopButton = Rectangle<int>{626, 1345, 114, 50};
        static inline Rectangle<int> noiseButton = Rectangle<int>{1397, 1292, 160, 160};

        static Rectangle<int> get(Rectangle<int> rectangle);
    };
};
