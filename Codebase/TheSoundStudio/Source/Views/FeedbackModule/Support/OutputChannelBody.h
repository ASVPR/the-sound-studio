/*
  ==============================================================================

    OutputChannelBody.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "VisualiserContainerComponent.h"
#include "FrequencyPlayerSettingsComponent.h"
#include "ChordPlayerSettingsComponent.h"

namespace Feedback
{
    class OutputChannelBody : public Component, Button::Listener, Slider::Listener
    {
    private:
        struct Bounds {
            static inline float scale {0.5f};
            static inline Rectangle<int> container = Rectangle<int>{0, 0, 1500, 451};
            static inline Rectangle<int> visualizer = Rectangle<int>{22, 9, 1455, 310};
            static inline Rectangle<int> phaseText = Rectangle<int>{22, 341, 83, 40};

            static inline Rectangle<int> freeFlowText = Rectangle<int>{156, 342, 186, 40};
            static inline Rectangle<int> freeFlowButton = Rectangle<int>{112, 342, 38, 38};
            static inline Rectangle<int> oscillatorText = Rectangle<int>{397, 342, 108, 40};
            static inline Rectangle<int> oscillatorButton = Rectangle<int>{352, 342, 38, 38};

            static inline Rectangle<int> phaseMarkingLeft = Rectangle<int>{115, 393, 71, 40};
            static inline Rectangle<int> phaseMarkingRight = Rectangle<int>{471, 393, 71, 40};
            static inline Rectangle<int> phaseSlider = Rectangle<int>{189, 393, 280, 40};
            static inline Rectangle<int> phaseNumberbox = Rectangle<int>{560, 393, 81, 40};

            static inline Rectangle<int> measuredFrequencyText = Rectangle<int>{955, 342, 220, 40};
            static inline Rectangle<int> measuredChordText = Rectangle<int>{1204, 342, 220, 40};
            static inline Rectangle<int> frequencyLabel = Rectangle<int>{955, 382, 126, 40};
            static inline Rectangle<int> chordLabel = Rectangle<int>{1204, 382, 220, 40};

            static inline Rectangle<int> chordButton = Rectangle<int>{692, 345, 81, 36};
            static inline Rectangle<int> frequencyButton = Rectangle<int>{788, 345, 122, 36};
            static inline Rectangle<int> algorithmResultLabel = Rectangle<int>{758, 393, 81, 40};

            static inline Rectangle<int> divider1 = Rectangle<int>{665, 335, 3, 100};
            static inline Rectangle<int> divider2 = Rectangle<int>{933, 335, 1, 100};

            static Rectangle<int> get(Rectangle<int> rectangle) {
                return rectangle.transformedBy(AffineTransform::scale(scale));
            }
        };
    public:
        OutputChannelBody(ProjectManager &projectManager, int inputIndex, Component* parent) :
            parent(parent),
            visualizer(&projectManager, VisualiserSelectorComponent::VISUALISER_TYPE::SPECTROGRAM, AUDIO_MODE::MODE_FEEDBACK_MODULE),
            frequencyPlayerSettingsComponent(&projectManager),
            chordPlayerSettingsComponent(&projectManager)
        {
            Image blueButtonNormal = ImageCache::getFromMemory(BinaryData::BlueButton_Normal_png, BinaryData::BlueButton_Normal_pngSize);
            Image blueButtonSelected = ImageCache::getFromMemory(BinaryData::BlueButton_Selected_png, BinaryData::BlueButton_Selected_pngSize);
            
            visualizer.setProcessingActive(false);
            addAndMakeVisible(visualizer);
            visualizer.setNewVisualiserSource(static_cast<VISUALISER_SOURCE>(inputIndex + 5));
            visualizer.showInputMenu(false);

            freeFlowButton.setTriggeredOnMouseDown(true);
            freeFlowButton.setMouseCursor(MouseCursor::PointingHandCursor);
            freeFlowButton.setClickingTogglesState(true);
            freeFlowButton.setRadioGroupId(1, dontSendNotification);
            freeFlowButton.setToggleState(true, dontSendNotification);
            freeFlowButton.setImages(false, true, true,
                blueButtonNormal, 0.999f, Colour(0x00000000),
                Image(), 1.000f, Colour(0x00000000),
                blueButtonSelected, 1.0, Colour(0x00000000));
            freeFlowButton.addListener(this);
            addAndMakeVisible(freeFlowButton);

            oscillatorButton.setTriggeredOnMouseDown(true);
            oscillatorButton.setMouseCursor(MouseCursor::PointingHandCursor);
            oscillatorButton.setClickingTogglesState(true);
            oscillatorButton.setRadioGroupId(1, dontSendNotification);
            oscillatorButton.setImages (false, true, true,
                blueButtonNormal, 0.999f, Colour (0x00000000),
                Image(), 1.000f, Colour (0x00000000),
                blueButtonSelected, 1.0, Colour (0x00000000));
            oscillatorButton.addListener(this);
            addAndMakeVisible(oscillatorButton);

            phaseSlider.setMouseCursor(MouseCursor::PointingHandCursor);
            phaseSlider.setNormalisableRange(NormalisableRange<double>{-100, 100, 0.01});
            phaseSlider.setValue(0.0, dontSendNotification);
            phaseSlider.addListener(this);
            addAndMakeVisible(phaseSlider);

            phaseLabel.setColour(Label::ColourIds::textColourId, Colours::white);
            phaseLabel.setText(String{phaseSlider.getValue(), 1}, dontSendNotification);
            addAndMakeVisible(phaseLabel);

            chordButton.setButtonText("Chord");
            chordButton.setClickingTogglesState(true);
            chordButton.setRadioGroupId(2);
            chordButton.setMouseCursor(MouseCursor::PointingHandCursor);
            chordButton.addListener(this);
            addAndMakeVisible(chordButton);

            frequencyButton.setButtonText("Frequency");
            frequencyButton.setClickingTogglesState(true);
            frequencyButton.setRadioGroupId(2);
            frequencyButton.setMouseCursor(MouseCursor::PointingHandCursor);
            frequencyButton.addListener(this);
            addAndMakeVisible(frequencyButton);

            algorithmResultLabel.setColour(Label::ColourIds::textColourId, Colours::white);
            addAndMakeVisible(algorithmResultLabel);

            measuredFrequency.setJustificationType(Justification::centredLeft);
            measuredChord.setJustificationType(Justification::centredLeft);
            addAndMakeVisible(measuredFrequency);
            addAndMakeVisible(measuredChord);

            parent->addChildComponent(chordPlayerSettingsComponent);
            parent->addChildComponent(frequencyPlayerSettingsComponent);
            chordPlayerSettingsComponent.updateScalesComponents();

            setSize(Bounds::get(Bounds::container).getWidth(), Bounds::get(Bounds::container).getHeight());
        }

        ~OutputChannelBody()
        {
            freeFlowButton.removeListener(this);
            oscillatorButton.removeListener(this);
            chordButton.removeListener(this);
            frequencyButton.removeListener(this);
            phaseSlider.removeListener(this);

            parent->removeChildComponent(&chordPlayerSettingsComponent);
            parent->removeChildComponent(&frequencyPlayerSettingsComponent);
        }
    private:
        void buttonClicked(Button *button) override
        {
            if (button == &frequencyButton && frequencyButton.getToggleState()) {
                frequencyPlayerSettingsComponent.toFront(true);
                frequencyPlayerSettingsComponent.openView(0);
            }
            if (button == &chordButton && chordButton.getToggleState()){
                chordPlayerSettingsComponent.toFront(true);
                chordPlayerSettingsComponent.openView(0);
            }
        }

        void sliderValueChanged(Slider *slider) override
        {
            if (slider == &phaseSlider)
                phaseLabel.setText(String{phaseSlider.getValue(), 1}, dontSendNotification);
        }

        void resized() override
        {
            auto bounds = getLocalBounds();
            if (!bounds.getHeight() > 0)
                return;
            visualizer.setBounds(Bounds::get(Bounds::visualizer));
            visualizer.setNewVisualiserType(VisualiserSelectorComponent::VISUALISER_TYPE::SPECTROGRAM);

            freeFlowButton.setBounds(Bounds::get(Bounds::freeFlowButton));
            oscillatorButton.setBounds(Bounds::get(Bounds::oscillatorButton));
            phaseSlider.setBounds(Bounds::get(Bounds::phaseSlider));

            measuredFrequency.setText("152 Hz", dontSendNotification);
            measuredChord.setText("C-4", dontSendNotification);
            measuredChord.setBounds(Bounds::get(Bounds::chordLabel));
            measuredFrequency.setBounds(Bounds::get(Bounds::frequencyLabel));

            chordButton.setBounds(Bounds::get(Bounds::chordButton));
            frequencyButton.setBounds(Bounds::get(Bounds::frequencyButton));

            phaseLabel.setBounds(Bounds::get(Bounds::phaseNumberbox).translated(5, 0));
            algorithmResultLabel.setBounds(Bounds::get(Bounds::algorithmResultLabel).translated(5, 0));

            frequencyPlayerSettingsComponent.setBounds(parent->getLocalBounds());
            chordPlayerSettingsComponent.setBounds(parent->getLocalBounds());
        }

        void paint(Graphics &g) override
        {
            g.setColour(Colours::white);
            g.setFont(14.f);
            g.drawFittedText("Measured estimated chord", Bounds::get(Bounds::measuredChordText), Justification::centredLeft, 1);
            g.drawFittedText("Measured frequency", Bounds::get(Bounds::measuredFrequencyText), Justification::centredLeft, 1);
            g.drawFittedText("Phase", Bounds::get(Bounds::phaseText), Justification::centredLeft, 1);
            g.drawFittedText("Free flow (phase)", Bounds::get(Bounds::freeFlowText), Justification::centredLeft, 1);
            g.drawFittedText("Oscillator", Bounds::get(Bounds::oscillatorText), Justification::centredLeft, 1);

            g.setFont(12.f);
            g.drawFittedText("-100%", Bounds::get(Bounds::phaseMarkingLeft), Justification::centredLeft, 1);
            g.drawFittedText("+100%", Bounds::get(Bounds::phaseMarkingRight), Justification::centredLeft, 1);

            g.setColour(Colours::white.withAlpha(0.4f));
            g.fillRect(Bounds::get(Bounds::divider1));
            g.fillRect(Bounds::get(Bounds::divider2));

            g.setColour(Colour::fromString("ff626773"));
            g.fillRoundedRectangle(Bounds::get(Bounds::algorithmResultLabel).toFloat(), 5);
            g.fillRoundedRectangle(Bounds::get(Bounds::phaseNumberbox).toFloat(), 5);
        }

        Component* parent;
        VisualiserSelectorComponent visualizer;
        Label measuredFrequency, measuredChord;
        ImageButton freeFlowButton, oscillatorButton;
        TextButton chordButton, frequencyButton;
        Slider phaseSlider{Slider::SliderStyle::LinearHorizontal, Slider::NoTextBox};
        Label algorithmResultLabel, phaseLabel;

        FrequencyPlayerSettingsComponent frequencyPlayerSettingsComponent;
        ChordPlayerSettingsComponent chordPlayerSettingsComponent;
    };
}
