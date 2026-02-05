/*
  ==============================================================================

    FeedbackModuleParameterSettingsComponent.h
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ProjectManager.h"

class FeedbackModuleParameterSettingsComponent : public Component, Button::Listener
{
public:
    FeedbackModuleParameterSettingsComponent(ProjectManager& pm);
    ~FeedbackModuleParameterSettingsComponent() override;
    void paint(Graphics &g) override;
    void resized() override;
    void setScale(float scaleFactor);

private:
    void buttonClicked(Button *button) override;

private:
    ProjectManager& projectManager;
    ImageButton closeButton, applyButton, simpleFrequencyButton, phaseShiftingButton;

    struct Bounds {
        static inline float scale {0.5f};
        static inline Rectangle<int> container = Rectangle<int>{185, 203, 1183, 578};
        static inline Rectangle<int> setFeedbackParametersText = Rectangle<int>{534, 252, 485, 71};
        static inline Rectangle<int> closeButton = Rectangle<int>{1285, 238, 49, 49};
        static inline Rectangle<int> applyButton = Rectangle<int>{671, 669, 212, 68};
        static inline Rectangle<int> tuningMethodsContainer = Rectangle<int>{567, 382, 419, 198};
        static inline Rectangle<int> tuningMethodsText = Rectangle<int>{589, 406, 190, 35};
        static inline Rectangle<int> simpleFrequencyText = Rectangle<int>{642, 456, 327, 35};
        static inline Rectangle<int> phaseShiftingText = Rectangle<int>{642, 517, 203, 35};
        static inline Rectangle<int> simpleFrequencyButton = Rectangle<int>{600, 454, 37, 37};
        static inline Rectangle<int> phaseShiftingButton = Rectangle<int>{600, 514, 37, 37};

        static Rectangle<int> get(Rectangle<int> rectangle);
    };
};
