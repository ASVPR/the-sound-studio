/*
  ==============================================================================

    FeedbackModuleComponent.h
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

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
        // Reference dimensions for ratio calculation
        static constexpr float kRefW = 1566.0f;
        static constexpr float kRefH = 1440.0f;

        static Rectangle<int> get(Rectangle<int> rectangle);

        static Rectangle<int> scaled(float x, float y, float w, float h, int parentW, int parentH)
        {
            return Rectangle<int>(
                (int)(x / kRefW * parentW),
                (int)(y / kRefH * parentH),
                (int)(w / kRefW * parentW),
                (int)(h / kRefH * parentH)
            );
        }
    };
};
