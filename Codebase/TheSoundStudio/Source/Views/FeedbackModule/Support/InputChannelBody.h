/*
  ==============================================================================

    InputChannelBody.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "VisualiserContainerComponent.h"

namespace Feedback {
    class InputChannelBody : public Component {
    private:
        struct Bounds {
            static inline float scale {0.5f};
            static inline Rectangle<int> container = Rectangle<int>{0, 0, 1500, 426};
            static inline Rectangle<int> visualizer = Rectangle<int>{22, 9, 1455, 310};
            static inline Rectangle<int> phaseText = Rectangle<int>{22, 341, 83, 40};

            static inline Rectangle<int> measuredFrequencyText = Rectangle<int>{26, 333, 220, 40};
            static inline Rectangle<int> measuredChordText = Rectangle<int>{284, 333, 220, 40};
            static inline Rectangle<int> frequencyLabel = Rectangle<int>{26, 373, 126, 40};
            static inline Rectangle<int> chordLabel = Rectangle<int>{286, 373, 220, 40};

            static Rectangle<int> get(Rectangle<int> rectangle) {
                return rectangle.transformedBy(AffineTransform::scale(scale));
            }
        };
    public:
        InputChannelBody(ProjectManager& projectManager, int inputIndex) :
            visualizer(&projectManager, VisualiserSelectorComponent::VISUALISER_TYPE::SPECTROGRAM, AUDIO_MODE::MODE_FEEDBACK_MODULE)
        {
            visualizer.setProcessingActive(false);
            addAndMakeVisible(visualizer);
            visualizer.setNewVisualiserSource(static_cast<VISUALISER_SOURCE>(inputIndex + 1));
            visualizer.showInputMenu(false);

            measuredFrequency.setJustificationType(Justification::centredLeft);
            measuredChord.setJustificationType(Justification::centredLeft);
            addAndMakeVisible(measuredFrequency);
            addAndMakeVisible(measuredChord);

            setSize(Bounds::get(Bounds::container).getWidth(), Bounds::get(Bounds::container).getHeight());
        }

        void resized() override {
            auto bounds = getLocalBounds();
            if (!bounds.getHeight() > 0)
                return;
            visualizer.setBounds(Bounds::get(Bounds::visualizer));
            visualizer.setNewVisualiserType(VisualiserSelectorComponent::VISUALISER_TYPE::SPECTROGRAM);

            measuredFrequency.setText("152 Hz", dontSendNotification);
            measuredChord.setText("C-4", dontSendNotification);
            measuredFrequency.setBounds(Bounds::get(Bounds::frequencyLabel));
            measuredChord.setBounds(Bounds::get(Bounds::chordLabel));
        }
    private:
        void paint(Graphics& g) override {
            g.setColour(Colours::white);
            g.setFont(14.f);
            g.drawFittedText("Measured frequency", Bounds::get(Bounds::measuredFrequencyText), Justification::centredLeft, 1);
            g.drawFittedText("Measured estimated chord", Bounds::get(Bounds::measuredChordText), Justification::centredLeft, 1);
        }

        VisualiserSelectorComponent visualizer;
        Label measuredFrequency, measuredChord;
    };
}
