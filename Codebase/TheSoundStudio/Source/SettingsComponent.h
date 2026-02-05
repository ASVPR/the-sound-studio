/*
  ==============================================================================

    SettingsComponent.h
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "CustomLookAndFeel.h"
#include "ProjectManager.h"
#include "HighQualityMeter.h"
#include "MenuViewInterface.h"
#include <memory>

class AudioMixerComponent : public Component, public Slider::Listener, public Button::Listener, public Timer, public ComboBox::Listener
{
public:
    AudioMixerComponent(ProjectManager * pm)
    {
        projectManager = pm;
        
        setLookAndFeel(&lookAndFeel);
        
        // Start timer for meter animations
        startTimer(30); // Faster refresh for smoother animations
        
        // Initialize preset system
        initializePresets();

        // Preset selector
        presetComboBox = std::make_unique<ComboBox>("Mixer Presets");
        presetComboBox->addItem("Default", 1);
        presetComboBox->addItem("Recording", 2);
        presetComboBox->addItem("Mixing", 3);
        presetComboBox->addItem("Mastering", 4);
        presetComboBox->addItem("Live", 5);
        presetComboBox->addItem("Custom 1", 6);
        presetComboBox->addItem("Custom 2", 7);
        presetComboBox->setSelectedId(1);
        presetComboBox->addListener(this);
        addAndMakeVisible(presetComboBox.get());
        
        // Save/Load preset buttons
        savePresetButton = std::make_unique<TextButton>("Save");
        savePresetButton->addListener(this);
        savePresetButton->setColour(TextButton::buttonColourId, Colour(0xFF5E81AC));
        savePresetButton->setColour(TextButton::textColourOffId, Colour(0xFFECEFF4));
        addAndMakeVisible(savePresetButton.get());
        
        loadPresetButton = std::make_unique<TextButton>("Load");
        loadPresetButton->addListener(this);
        loadPresetButton->setColour(TextButton::buttonColourId, Colour(0xFF5E81AC));
        loadPresetButton->setColour(TextButton::textColourOffId, Colour(0xFFECEFF4));
        addAndMakeVisible(loadPresetButton.get());
        
        // Master section controls
        masterGainSlider = std::make_unique<Slider>();
        masterGainSlider->setRange(-60, 12, 0.1);
        masterGainSlider->setSliderStyle(Slider::LinearVertical);
        masterGainSlider->setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
        masterGainSlider->setColour(Slider::trackColourId, Colour(0xFF5E81AC));
        masterGainSlider->setColour(Slider::backgroundColourId, Colour(0xFF1A1E24));
        masterGainSlider->setColour(Slider::thumbColourId, Colour(0xFFD8DEE9));
        masterGainSlider->setColour(Slider::textBoxTextColourId, Colour(0xFFD8DEE9));
        masterGainSlider->setColour(Slider::textBoxBackgroundColourId, Colour(0xFF2E3440));
        masterGainSlider->addListener(this);
        masterGainSlider->setValue(0.0);
        masterGainSlider->setTextValueSuffix(" dB");
        addAndMakeVisible(masterGainSlider.get());
        
        masterLabel = std::make_unique<Label>("", "MASTER");
        masterLabel->setJustificationType(Justification::centred);
        masterLabel->setFont(Font(16.0f, Font::bold));
        masterLabel->setColour(Label::textColourId, Colour(0xFFECEFF4));
        addAndMakeVisible(masterLabel.get());
        
        masterMeter = std::make_unique<HighQualityMeter>(2); // Stereo meter
        addAndMakeVisible(masterMeter.get());

        for (int i = 0 ; i < 4; i++)
        {
            sliderInputChannel[i]           = std::make_unique<Slider>();
            sliderInputChannel[i]           ->setRange (-60, 12, 0.1);
            sliderInputChannel[i]           ->setSliderStyle (Slider::LinearVertical);
            sliderInputChannel[i]           ->setTextBoxStyle(Slider::TextBoxBelow, false, 50, 18);
            sliderInputChannel[i]           ->setColour(Slider::trackColourId, Colour(0xFF88C0D0));
            sliderInputChannel[i]           ->setColour(Slider::backgroundColourId, Colour(0xFF1A1E24));
            sliderInputChannel[i]           ->setColour(Slider::thumbColourId, Colour(0xFFD8DEE9));
            sliderInputChannel[i]           ->setColour(Slider::textBoxTextColourId, Colour(0xFF88C0D0));
            sliderInputChannel[i]           ->setColour(Slider::textBoxBackgroundColourId, Colour(0xFF2E3440));
            sliderInputChannel[i]           ->addListener (this);
            sliderInputChannel[i]           ->setValue(0.0);
            sliderInputChannel[i]           ->setTextValueSuffix(" dB");
            sliderInputChannel[i]           ->setDoubleClickReturnValue(true, 0.0);
            addAndMakeVisible(sliderInputChannel[i].get());
            
            
            sliderOutputChannel[i]          = std::make_unique<Slider>();
            sliderOutputChannel[i]          ->setRange (-60, 12, 0.1);
            sliderOutputChannel[i]          ->setSliderStyle (Slider::LinearVertical);
            sliderOutputChannel[i]          ->setTextBoxStyle(Slider::TextBoxBelow, false, 50, 18);
            sliderOutputChannel[i]          ->setColour(Slider::trackColourId, Colour(0xFFBF616A));
            sliderOutputChannel[i]          ->setColour(Slider::backgroundColourId, Colour(0xFF1A1E24));
            sliderOutputChannel[i]          ->setColour(Slider::thumbColourId, Colour(0xFFD8DEE9));
            sliderOutputChannel[i]          ->setColour(Slider::textBoxTextColourId, Colour(0xFFBF616A));
            sliderOutputChannel[i]          ->setColour(Slider::textBoxBackgroundColourId, Colour(0xFF2E3440));
            sliderOutputChannel[i]          ->addListener (this);
            sliderOutputChannel[i]          ->setValue(0.0);
            sliderOutputChannel[i]          ->setTextValueSuffix(" dB");
            sliderOutputChannel[i]          ->setDoubleClickReturnValue(true, 0.0);
            addAndMakeVisible(sliderOutputChannel[i].get());
            
            // EQ controls (3-band: Low, Mid, High)
            for (int band = 0; band < 3; band++)
            {
                eqInputChannel[i][band] = std::make_unique<Slider>();
                eqInputChannel[i][band]->setRange(-12, 12, 0.1);
                eqInputChannel[i][band]->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
                eqInputChannel[i][band]->setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
                eqInputChannel[i][band]->setColour(Slider::rotarySliderFillColourId, Colour(0xFF81A1C1));
                eqInputChannel[i][band]->setColour(Slider::rotarySliderOutlineColourId, Colour(0xFF3B4252));
                eqInputChannel[i][band]->setColour(Slider::thumbColourId, Colour(0xFFD8DEE9));
                eqInputChannel[i][band]->addListener(this);
                eqInputChannel[i][band]->setValue(0.0);
                eqInputChannel[i][band]->setDoubleClickReturnValue(true, 0.0);
                addAndMakeVisible(eqInputChannel[i][band].get());
                
                eqOutputChannel[i][band] = std::make_unique<Slider>();
                eqOutputChannel[i][band]->setRange(-12, 12, 0.1);
                eqOutputChannel[i][band]->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
                eqOutputChannel[i][band]->setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
                eqOutputChannel[i][band]->setColour(Slider::rotarySliderFillColourId, Colour(0xFFD08770));
                eqOutputChannel[i][band]->setColour(Slider::rotarySliderOutlineColourId, Colour(0xFF3B4252));
                eqOutputChannel[i][band]->setColour(Slider::thumbColourId, Colour(0xFFD8DEE9));
                eqOutputChannel[i][band]->addListener(this);
                eqOutputChannel[i][band]->setValue(0.0);
                eqOutputChannel[i][band]->setDoubleClickReturnValue(true, 0.0);
                addAndMakeVisible(eqOutputChannel[i][band].get());
            }
            
            // Pan controls
            panInputChannel[i]              = std::make_unique<Slider>();
            panInputChannel[i]              ->setRange(-100, 100, 1);
            panInputChannel[i]              ->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
            panInputChannel[i]              ->setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
            panInputChannel[i]              ->setColour(Slider::rotarySliderFillColourId, Colour(0xFF88C0D0));
            panInputChannel[i]              ->setColour(Slider::rotarySliderOutlineColourId, Colour(0xFF2E3440));
            panInputChannel[i]              ->setColour(Slider::thumbColourId, Colour(0xFFD8DEE9));
            panInputChannel[i]              ->addListener(this);
            panInputChannel[i]              ->setValue(0.0);
            panInputChannel[i]              ->setDoubleClickReturnValue(true, 0.0);
            addAndMakeVisible(panInputChannel[i].get());
            
            panOutputChannel[i]             = std::make_unique<Slider>();
            panOutputChannel[i]             ->setRange(-100, 100, 1);
            panOutputChannel[i]             ->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
            panOutputChannel[i]             ->setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
            panOutputChannel[i]             ->setColour(Slider::rotarySliderFillColourId, Colour(0xFFBF616A));
            panOutputChannel[i]             ->setColour(Slider::rotarySliderOutlineColourId, Colour(0xFF2E3440));
            panOutputChannel[i]             ->setColour(Slider::thumbColourId, Colour(0xFFD8DEE9));
            panOutputChannel[i]             ->addListener(this);
            panOutputChannel[i]             ->setValue(0.0);
            panOutputChannel[i]             ->setDoubleClickReturnValue(true, 0.0);
            addAndMakeVisible(panOutputChannel[i].get());
            
            labelInput[i]                   = std::make_unique<Label>("","IN " + String(i + 1));
            labelInput[i]                   ->setJustificationType(Justification::centred);
            labelInput[i]                   ->setFont(Font(13.0f, Font::bold));
            labelInput[i]                   ->setColour(Label::textColourId, Colour(0xFFECEFF4));
            addAndMakeVisible(labelInput[i].get());
            
            // Add peak indicators
            peakLabelInput[i]               = std::make_unique<Label>("", "PEAK");
            peakLabelInput[i]               ->setJustificationType(Justification::centred);
            peakLabelInput[i]               ->setFont(Font(9.0f));
            peakLabelInput[i]               ->setColour(Label::textColourId, Colour(0xFF88C0D0).withAlpha(0.0f));
            peakLabelInput[i]               ->setColour(Label::backgroundColourId, Colour(0xFFBF616A));
            addAndMakeVisible(peakLabelInput[i].get());

            labelOutput[i]                  = std::make_unique<Label>("","OUT " + String(i + 1));
            labelOutput[i]                  ->setJustificationType(Justification::centred);
            labelOutput[i]                  ->setFont(Font(13.0f, Font::bold));
            labelOutput[i]                  ->setColour(Label::textColourId, Colour(0xFFECEFF4));
            addAndMakeVisible(labelOutput[i].get());
            
            // Add peak indicators
            peakLabelOutput[i]              = std::make_unique<Label>("", "PEAK");
            peakLabelOutput[i]              ->setJustificationType(Justification::centred);
            peakLabelOutput[i]              ->setFont(Font(9.0f));
            peakLabelOutput[i]              ->setColour(Label::textColourId, Colour(0xFFBF616A).withAlpha(0.0f));
            peakLabelOutput[i]              ->setColour(Label::backgroundColourId, Colour(0xFFBF616A));
            addAndMakeVisible(peakLabelOutput[i].get());

            meterInput[i]                   = std::make_unique<HighQualityMeter>(1);
            meterInput[i]                   ->setColour(HighQualityMeter::level10dBColourId, Colour(0xFF88C0D0));
            meterInput[i]                   ->setColour(HighQualityMeter::level6dBColourId, Colour(0xFF5E81AC));
            meterInput[i]                   ->setColour(HighQualityMeter::level3dBColourId, Colour(0xFFEBCB8B));
            meterInput[i]                   ->setColour(HighQualityMeter::level0dBColourId, Colour(0xFFD08770));
            meterInput[i]                   ->setColour(HighQualityMeter::levelOverColourId, Colour(0xFFBF616A));
            addAndMakeVisible(meterInput[i].get());

            meterOutput[i]                  = std::make_unique<HighQualityMeter>(1);
            meterOutput[i]                  ->setColour(HighQualityMeter::level10dBColourId, Colour(0xFFBF616A));
            meterOutput[i]                  ->setColour(HighQualityMeter::level6dBColourId, Colour(0xFFD08770));
            meterOutput[i]                  ->setColour(HighQualityMeter::level3dBColourId, Colour(0xFFEBCB8B));
            meterOutput[i]                  ->setColour(HighQualityMeter::level0dBColourId, Colour(0xFFD08770));
            meterOutput[i]                  ->setColour(HighQualityMeter::levelOverColourId, Colour(0xFFBF616A));
            addAndMakeVisible(meterOutput[i].get());

            buttonFFTInputChannel[i]        = std::make_unique<TextButton>("FFT");
            buttonFFTInputChannel[i]        ->addListener(this);
            buttonFFTInputChannel[i]        ->setColour(TextButton::buttonColourId, Colour(0xFF3B4252));
            buttonFFTInputChannel[i]        ->setColour(TextButton::buttonOnColourId, Colour(0xFF88C0D0));
            buttonFFTInputChannel[i]        ->setColour(TextButton::textColourOffId, Colour(0xFFD8DEE9));
            buttonFFTInputChannel[i]        ->setColour(TextButton::textColourOnId, Colour(0xFF2E3440));
            addAndMakeVisible(buttonFFTInputChannel[i].get());
            
            buttonFFTOutputChannel[i]       = std::make_unique<TextButton>("FFT");
            buttonFFTOutputChannel[i]       ->addListener(this);
            buttonFFTOutputChannel[i]       ->setColour(TextButton::buttonColourId, Colour(0xFF3B4252));
            buttonFFTOutputChannel[i]       ->setColour(TextButton::buttonOnColourId, Colour(0xFFBF616A));
            buttonFFTOutputChannel[i]       ->setColour(TextButton::textColourOffId, Colour(0xFFD8DEE9));
            buttonFFTOutputChannel[i]       ->setColour(TextButton::textColourOnId, Colour(0xFF2E3440));
            addAndMakeVisible(buttonFFTOutputChannel[i].get());
            
            // Solo and Mute buttons with better styling
            soloInputChannel[i]             = std::make_unique<TextButton>("S");
            soloInputChannel[i]             ->addListener(this);
            soloInputChannel[i]             ->setColour(TextButton::buttonColourId, Colour(0xFF3B4252));
            soloInputChannel[i]             ->setColour(TextButton::buttonOnColourId, Colour(0xFFEBCB8B));
            soloInputChannel[i]             ->setColour(TextButton::textColourOffId, Colour(0xFF81A1C1));
            soloInputChannel[i]             ->setColour(TextButton::textColourOnId, Colour(0xFF2E3440));
            addAndMakeVisible(soloInputChannel[i].get());
            
            muteInputChannel[i]             = std::make_unique<TextButton>("M");
            muteInputChannel[i]             ->addListener(this);
            muteInputChannel[i]             ->setColour(TextButton::buttonColourId, Colour(0xFF3B4252));
            muteInputChannel[i]             ->setColour(TextButton::buttonOnColourId, Colour(0xFFBF616A));
            muteInputChannel[i]             ->setColour(TextButton::textColourOffId, Colour(0xFF81A1C1));
            muteInputChannel[i]             ->setColour(TextButton::textColourOnId, Colour(0xFFECEFF4));
            addAndMakeVisible(muteInputChannel[i].get());
            
            soloOutputChannel[i]            = std::make_unique<TextButton>("S");
            soloOutputChannel[i]            ->addListener(this);
            soloOutputChannel[i]            ->setColour(TextButton::buttonColourId, Colour(0xFF3B4252));
            soloOutputChannel[i]            ->setColour(TextButton::buttonOnColourId, Colour(0xFFEBCB8B));
            soloOutputChannel[i]            ->setColour(TextButton::textColourOffId, Colour(0xFF81A1C1));
            soloOutputChannel[i]            ->setColour(TextButton::textColourOnId, Colour(0xFF2E3440));
            addAndMakeVisible(soloOutputChannel[i].get());
            
            muteOutputChannel[i]            = std::make_unique<TextButton>("M");
            muteOutputChannel[i]            ->addListener(this);
            muteOutputChannel[i]            ->setColour(TextButton::buttonColourId, Colour(0xFF3B4252));
            muteOutputChannel[i]            ->setColour(TextButton::buttonOnColourId, Colour(0xFFBF616A));
            muteOutputChannel[i]            ->setColour(TextButton::textColourOffId, Colour(0xFF81A1C1));
            muteOutputChannel[i]            ->setColour(TextButton::textColourOnId, Colour(0xFFECEFF4));
            addAndMakeVisible(muteOutputChannel[i].get());
            
            // Record arm buttons
            recInputChannel[i]              = std::make_unique<TextButton>("R");
            recInputChannel[i]              ->addListener(this);
            recInputChannel[i]              ->setColour(TextButton::buttonColourId, Colour(0xFF3B4252));
            recInputChannel[i]              ->setColour(TextButton::buttonOnColourId, Colour(0xFFBF616A));
            recInputChannel[i]              ->setColour(TextButton::textColourOffId, Colour(0xFF81A1C1));
            recInputChannel[i]              ->setColour(TextButton::textColourOnId, Colour(0xFFECEFF4));
            addAndMakeVisible(recInputChannel[i].get());
        }
        
        syncUI();
    }
    
    
    ~AudioMixerComponent()
    {
        stopTimer();
        setLookAndFeel(nullptr);
    }
    
    void timerCallback() override
    {
        // Animate meters with smooth decay and peak detection
        for (int i = 0; i < 4; i++)
        {
            if (meterInput[i] != nullptr)
            {
                // Get current gain value
                float linearGain = (float)projectManager->getProjectSettingsParameter(MIXER_INPUT_GAIN_1 + i);
                float targetValue = jmap(linearGain, 0.0f, 2.0f, 0.0f, 1.0f);  // Map gain to meter range
                
                // Peak detection
                if (targetValue > 0.95f) // Near 0dB
                {
                    peakLabelInput[i]->setColour(Label::textColourId, Colour(0xFFBF616A));
                    peakLabelInput[i]->setAlpha(1.0f);
                    peakHoldInput[i] = 60; // Hold for 2 seconds at 30fps
                }
                else if (peakHoldInput[i] > 0)
                {
                    peakHoldInput[i]--;
                    if (peakHoldInput[i] == 0)
                        peakLabelInput[i]->setAlpha(0.0f);
                }
                
                meterInput[i]->setValue(0, targetValue);
            }
            
            if (meterOutput[i] != nullptr)
            {
                // Get current gain value
                float linearGain = (float)projectManager->getProjectSettingsParameter(MIXER_OUTPUT_GAIN_1 + i);
                float targetValue = jmap(linearGain, 0.0f, 2.0f, 0.0f, 1.0f);  // Map gain to meter range
                
                // Peak detection
                if (targetValue > 0.95f) // Near 0dB
                {
                    peakLabelOutput[i]->setColour(Label::textColourId, Colour(0xFFBF616A));
                    peakLabelOutput[i]->setAlpha(1.0f);
                    peakHoldOutput[i] = 60; // Hold for 2 seconds at 30fps
                }
                else if (peakHoldOutput[i] > 0)
                {
                    peakHoldOutput[i]--;
                    if (peakHoldOutput[i] == 0)
                        peakLabelOutput[i]->setAlpha(0.0f);
                }
                
                meterOutput[i]->setValue(0, targetValue);
            }
        }
        
        // Update master meter
        if (masterMeter != nullptr)
        {
            float masterDb = (float)masterGainSlider->getValue();
            float masterValue = jmap(masterDb, -60.0f, 12.0f, 0.0f, 1.0f);
            masterMeter->setValue(0, masterValue);
            masterMeter->setValue(1, masterValue); // Stereo
        }
        
        repaint();
    }
    
    void paint(Graphics& g) override
    {
        // Professional studio-style dark gradient background
        ColourGradient bgGradient(Colour(0xFF1C1F26), 0, 0,
                                 Colour(0xFF0D0E11), getWidth(), getHeight(), false);
        g.setGradientFill(bgGradient);
        g.fillAll();

        // Add subtle texture overlay for depth
        g.setColour(Colour(0xFF000000).withAlpha(0.1f));
        for (int y = 0; y < getHeight(); y += 2)
        {
            g.drawHorizontalLine(y, 0, getWidth());
        }
        
        auto area = getLocalBounds();
        
        // Reserve space for master section
        auto masterArea = area.removeFromRight(120);
        
        auto channelsArea = area;
        auto inputsArea = channelsArea.removeFromLeft(channelsArea.getWidth() / 2);
        auto outputsArea = channelsArea;
        
        // Draw professional section headers
        auto drawSectionHeader = [&g](juce::Rectangle<int> r, const String& title, Colour accent)
        {
            // Sleek header with metallic gradient
            ColourGradient headerGradient(Colour(0xFF2A2E38), r.getX(), r.getY(),
                                         Colour(0xFF1F2229), r.getRight(), r.getY(), false);
            g.setGradientFill(headerGradient);
            auto headerRect = r.removeFromTop(45);
            g.fillRoundedRectangle(headerRect.toFloat(), 8.f);

            // Professional accent bar with glow
            Path accentPath;
            auto accentRect = headerRect.removeFromLeft(5).reduced(0, 10);
            accentPath.addRoundedRectangle(accentRect.toFloat(), 2.f);
            g.setColour(accent.withAlpha(0.2f));
            g.fillPath(accentPath);
            g.setColour(accent.withAlpha(0.8f));
            g.strokePath(accentPath, PathStrokeType(2.0f));

            // Title with better typography
            g.setColour(Colours::black.withAlpha(0.8f));
            g.setFont(Font("Helvetica Neue", 14.0f, Font::bold));
            g.drawText(title, headerRect.reduced(15, 2), Justification::centredLeft);

            g.setColour(Colour(0xFFE8EBF0));
            g.setFont(Font("Helvetica Neue", 14.0f, Font::bold));
            g.drawText(title, headerRect.reduced(14, 1), Justification::centredLeft);
        };
        
        drawSectionHeader(inputsArea.reduced(10), "INPUT CHANNELS", Colour(0xFF88C0D0));
        drawSectionHeader(outputsArea.reduced(10), "OUTPUT CHANNELS", Colour(0xFFBF616A));
        
        // Draw master section header
        auto masterHeaderArea = masterArea.reduced(10);
        drawSectionHeader(masterHeaderArea, "MASTER", Colour(0xFF5E81AC));
        
        // Draw professional channel strips
        for (int i = 0; i < 4; i++)
        {
            const int stripWidth = inputsArea.getWidth() / 4;

            // Input channel strip with professional console look
            auto inStripRect = juce::Rectangle<int>(inputsArea.getX() + stripWidth * i + 8,
                                                    55, stripWidth - 16, getHeight() - 70);

            // Multi-layer shadow for depth
            for (int s = 3; s > 0; s--)
            {
                g.setColour(Colours::black.withAlpha(0.15f / s));
                g.fillRoundedRectangle(inStripRect.translated(s, s).toFloat(), 6.f);
            }

            // Professional channel strip background
            ColourGradient stripGradient(Colour(0xFF252830),
                                        inStripRect.getX(), inStripRect.getY(),
                                        Colour(0xFF1A1D23),
                                        inStripRect.getX(), inStripRect.getBottom(), false);
            g.setGradientFill(stripGradient);
            g.fillRoundedRectangle(inStripRect.toFloat(), 6.f);

            // Metallic border with inner glow
            g.setColour(Colour(0xFF3A3F4A));
            g.drawRoundedRectangle(inStripRect.toFloat(), 6.f, 1.5f);
            g.setColour(Colour(0xFF88C0D0).withAlpha(0.15f));
            g.drawRoundedRectangle(inStripRect.reduced(1).toFloat(), 6.f, 0.5f);
            
            // Output channel strip with glass effect
            auto outStripRect = juce::Rectangle<int>(outputsArea.getX() + stripWidth * i + 6,
                                                     50, stripWidth - 12, getHeight() - 60);
            
            // Drop shadow
            g.setColour(Colours::black.withAlpha(0.3f));
            g.fillRoundedRectangle(outStripRect.translated(2, 2).toFloat(), 8.f);
            
            // Glass background
            stripGradient = ColourGradient(Colour(0xFF2E3440).withAlpha(0.7f), 
                                          outStripRect.getX(), outStripRect.getY(),
                                          Colour(0xFF3B4252).withAlpha(0.5f), 
                                          outStripRect.getX(), outStripRect.getBottom(), false);
            g.setGradientFill(stripGradient);
            g.fillRoundedRectangle(outStripRect.toFloat(), 8.f);
            
            // Border highlight
            g.setColour(Colour(0xFFBF616A).withAlpha(0.2f));
            g.drawRoundedRectangle(outStripRect.toFloat(), 8.f, 1.0f);
        }
        
        // Draw master strip
        auto masterStripRect = masterArea.reduced(10, 0);
        masterStripRect.removeFromTop(50);
        masterStripRect = masterStripRect.reduced(5, 10);
        
        // Drop shadow
        g.setColour(Colours::black.withAlpha(0.4f));
        g.fillRoundedRectangle(masterStripRect.translated(3, 3).toFloat(), 10.f);
        
        // Master strip gradient background
        ColourGradient masterGradient(Colour(0xFF3B4252), 
                                     masterStripRect.getX(), masterStripRect.getY(),
                                     Colour(0xFF434C5E), 
                                     masterStripRect.getX(), masterStripRect.getBottom(), false);
        g.setGradientFill(masterGradient);
        g.fillRoundedRectangle(masterStripRect.toFloat(), 10.f);
        
        // Master strip border
        g.setColour(Colour(0xFF5E81AC).withAlpha(0.3f));
        g.drawRoundedRectangle(masterStripRect.toFloat(), 10.f, 1.5f);
        
        // Draw dB scale markings
        g.setColour(Colour(0xFF81A1C1).withAlpha(0.5f));
        g.setFont(Font(9.0f));
        
        const float dbMarks[] = { 12, 6, 0, -6, -12, -24, -48 };
        for (auto db : dbMarks)
        {
            String text = (db > 0 ? "+" : "") + String(int(db));
            // Position calculation would go here based on fader positions
        }
    }

    void resized() override
    {
        auto area = getLocalBounds();

        // Preset controls at top
        auto presetArea = area.removeFromTop(45);
        presetArea = presetArea.reduced(10, 5);
        auto presetLabelWidth = 60;
        presetArea.removeFromLeft(presetLabelWidth); // Space for "Presets:" label
        presetComboBox->setBounds(presetArea.removeFromLeft(180));
        presetArea.removeFromLeft(10);
        savePresetButton->setBounds(presetArea.removeFromLeft(60));
        presetArea.removeFromLeft(5);
        loadPresetButton->setBounds(presetArea.removeFromLeft(60));

        // Reserve space for master section (wider for better layout)
        auto masterArea = area.removeFromRight(150);

        auto channelsArea = area;
        auto inputsArea = channelsArea.removeFromLeft(channelsArea.getWidth() / 2);
        auto outputsArea = channelsArea;

        // Skip header area
        inputsArea.removeFromTop(55);
        outputsArea.removeFromTop(55);
        masterArea.removeFromTop(55);

        // Master section layout
        auto masterStrip = masterArea.reduced(15, 10);
        masterLabel->setBounds(masterStrip.removeFromTop(25));

        // Master meter
        auto masterMeterArea = masterStrip.removeFromTop(140);
        masterMeter->setBounds(masterMeterArea.reduced(25, 0));

        // Master fader with text box
        masterGainSlider->setBounds(masterStrip.reduced(20, 10));
        
        for (int i = 0 ; i < 4; i++)
        {
            const int stripWidth = inputsArea.getWidth() / 4;
            const int stripPadding = 10;
            const int channelWidth = stripWidth - stripPadding * 2;
            
            // Input channel layout
            auto inStrip = inputsArea.removeFromLeft(stripWidth).reduced(stripPadding);
            
            labelInput[i]->setBounds(inStrip.removeFromTop(18));
            peakLabelInput[i]->setBounds(inStrip.removeFromTop(14));
            
            // EQ controls (3-band)
            auto eqArea = inStrip.removeFromTop(75);
            auto eqKnobWidth = eqArea.getWidth() / 3;
            for (int band = 0; band < 3; band++)
            {
                auto knobArea = eqArea.removeFromLeft(eqKnobWidth);
                eqInputChannel[i][band]->setBounds(knobArea.reduced(3));
            }

            // Pan control
            auto panArea = inStrip.removeFromTop(50);
            panInputChannel[i]->setBounds(panArea.reduced(channelWidth/4, 5));

            // Meter area
            auto meterArea = inStrip.removeFromTop(120);
            meterInput[i]->setBounds(meterArea.reduced(channelWidth/3, 0));
            
            // Solo/Mute/Rec buttons
            auto buttonArea = inStrip.removeFromBottom(22);
            auto thirdWidth = buttonArea.getWidth() / 3;
            soloInputChannel[i]->setBounds(buttonArea.removeFromLeft(thirdWidth - 2));
            muteInputChannel[i]->setBounds(buttonArea.removeFromLeft(thirdWidth));
            recInputChannel[i]->setBounds(buttonArea.removeFromLeft(thirdWidth + 2));
            
            // FFT button
            buttonFFTInputChannel[i]->setBounds(inStrip.removeFromBottom(20).reduced(6, 0));
            
            // Fader with text box
            sliderInputChannel[i]->setBounds(inStrip.reduced(channelWidth/5, 5));
            
            // Output channel layout
            auto outStrip = outputsArea.removeFromLeft(stripWidth).reduced(stripPadding);
            
            labelOutput[i]->setBounds(outStrip.removeFromTop(18));
            peakLabelOutput[i]->setBounds(outStrip.removeFromTop(14));
            
            // EQ controls (3-band) for output
            eqArea = outStrip.removeFromTop(75);
            eqKnobWidth = eqArea.getWidth() / 3;
            for (int band = 0; band < 3; band++)
            {
                auto knobArea = eqArea.removeFromLeft(eqKnobWidth);
                eqOutputChannel[i][band]->setBounds(knobArea.reduced(3));
            }

            // Pan control
            panArea = outStrip.removeFromTop(50);
            panOutputChannel[i]->setBounds(panArea.reduced(channelWidth/4, 5));

            // Meter area
            meterArea = outStrip.removeFromTop(120);
            meterOutput[i]->setBounds(meterArea.reduced(channelWidth/3, 0));
            
            // Solo/Mute buttons
            buttonArea = outStrip.removeFromBottom(22);
            auto halfWidth = buttonArea.getWidth() / 2;
            soloOutputChannel[i]->setBounds(buttonArea.removeFromLeft(halfWidth - 2));
            muteOutputChannel[i]->setBounds(buttonArea.removeFromLeft(halfWidth + 2));
            
            // FFT button
            buttonFFTOutputChannel[i]->setBounds(outStrip.removeFromBottom(20).reduced(6, 0));
            
            // Fader with text box
            sliderOutputChannel[i]->setBounds(outStrip.reduced(channelWidth/5, 5));
        }
    }
    
    void sliderValueChanged (Slider* slider) override
    {
        // Convert dB to linear gain for storage
        for (int i = 0; i < 4; i++)
        {
            if (slider == sliderInputChannel[i].get())
            {
                float dbValue = (float)sliderInputChannel[i]->getValue();
                float linearGain = Decibels::decibelsToGain(dbValue);
                projectManager->setProjectSettingsParameter(MIXER_INPUT_GAIN_1 + i, linearGain);
            }
            else if (slider == sliderOutputChannel[i].get())
            {
                float dbValue = (float)sliderOutputChannel[i]->getValue();
                float linearGain = Decibels::decibelsToGain(dbValue);
                projectManager->setProjectSettingsParameter(MIXER_OUTPUT_GAIN_1 + i, linearGain);
            }
            else if (slider == panInputChannel[i].get())
            {
                // Store pan value (-100 to 100)
                // You may need to add PAN parameters to your project settings
            }
            else if (slider == panOutputChannel[i].get())
            {
                // Store pan value (-100 to 100)
                // You may need to add PAN parameters to your project settings
            }
        }
        
        if (slider == masterGainSlider.get())
        {
            // Store master gain
            // You may need to add MASTER_GAIN parameter to your project settings
        }
        
        syncUI();
    }
    
    void comboBoxChanged(ComboBox* comboBox) override
    {
        if (comboBox == presetComboBox.get())
        {
            loadPreset(comboBox->getSelectedId());
        }
    }
    
    void buttonClicked (Button*button) override
    {
        if (button == savePresetButton.get())
        {
            saveCurrentPreset();
        }
        else if (button == loadPresetButton.get())
        {
            loadPreset(presetComboBox->getSelectedId());
        }
        else if (button == buttonFFTInputChannel[0].get())
        {
            projectManager->setProjectSettingsParameter(MIXER_INPUT_FFT_1, !buttonFFTInputChannel[0]->getToggleState());
        }
        else if (button == buttonFFTInputChannel[1].get())
        {
            projectManager->setProjectSettingsParameter(MIXER_INPUT_FFT_2, !buttonFFTInputChannel[1]->getToggleState());
        }
        else if (button == buttonFFTInputChannel[2].get())
        {
            projectManager->setProjectSettingsParameter(MIXER_INPUT_FFT_3, !buttonFFTInputChannel[2]->getToggleState());
        }
        else if (button == buttonFFTInputChannel[3].get())
        {
            projectManager->setProjectSettingsParameter(MIXER_INPUT_FFT_4, !buttonFFTInputChannel[3]->getToggleState());
        }
        else if (button == buttonFFTOutputChannel[0].get())
        {
            projectManager->setProjectSettingsParameter(MIXER_OUTPUT_FFT_1, !buttonFFTOutputChannel[0]->getToggleState());
        }
        else if (button == buttonFFTOutputChannel[1].get())
        {
            projectManager->setProjectSettingsParameter(MIXER_OUTPUT_FFT_2, !buttonFFTOutputChannel[1]->getToggleState());
        }
        else if (button == buttonFFTOutputChannel[2].get())
        {
            projectManager->setProjectSettingsParameter(MIXER_OUTPUT_FFT_3, !buttonFFTOutputChannel[2]->getToggleState());
        }
        else if (button == buttonFFTOutputChannel[3].get())
        {
            projectManager->setProjectSettingsParameter(MIXER_OUTPUT_FFT_4, !buttonFFTOutputChannel[3]->getToggleState());
        }
        
        syncUI();
    }
    
    void syncUI()
    {
        for (int i = 0 ; i < 4; i++)
        {
            // Convert linear gain to dB for display
            float linearGainIn = (float)projectManager->getProjectSettingsParameter(MIXER_INPUT_GAIN_1 + i);
            float dbValueIn = linearGainIn > 0.0f ? Decibels::gainToDecibels(linearGainIn) : -60.0f;
            sliderInputChannel[i]->setValue(dbValueIn, dontSendNotification);
            
            float linearGainOut = (float)projectManager->getProjectSettingsParameter(MIXER_OUTPUT_GAIN_1 + i);
            float dbValueOut = linearGainOut > 0.0f ? Decibels::gainToDecibels(linearGainOut) : -60.0f;
            sliderOutputChannel[i]->setValue(dbValueOut, dontSendNotification);
            
            buttonFFTInputChannel[i] ->setToggleState((bool)projectManager->getProjectSettingsParameter(MIXER_INPUT_FFT_1 + i), dontSendNotification);
            buttonFFTOutputChannel[i]->setToggleState((bool)projectManager->getProjectSettingsParameter(MIXER_OUTPUT_FFT_1 + i), dontSendNotification);
        }
    }
    
    void initializePresets()
    {
        // Initialize default presets
        // This would typically load from a file or database
    }
    
    void saveCurrentPreset()
    {
        // Save current mixer state to selected preset slot
        int presetId = presetComboBox->getSelectedId();
        // Implementation would save all mixer parameters
    }
    
    void loadPreset(int presetId)
    {
        // Load preset values based on ID
        switch(presetId)
        {
            case 1: // Default
                for (int i = 0; i < 4; i++)
                {
                    sliderInputChannel[i]->setValue(0.0);
                    sliderOutputChannel[i]->setValue(0.0);
                    panInputChannel[i]->setValue(0.0);
                    panOutputChannel[i]->setValue(0.0);
                    for (int band = 0; band < 3; band++)
                    {
                        eqInputChannel[i][band]->setValue(0.0);
                        eqOutputChannel[i][band]->setValue(0.0);
                    }
                }
                masterGainSlider->setValue(0.0);
                break;
            
            case 2: // Recording
                // Set recording-optimized levels
                for (int i = 0; i < 4; i++)
                {
                    sliderInputChannel[i]->setValue(-12.0);
                    sliderOutputChannel[i]->setValue(-6.0);
                }
                masterGainSlider->setValue(-3.0);
                break;
                
            // Add more preset configurations as needed
        }
    }
    
    
private:
    ProjectManager * projectManager;
    
    CustomLookAndFeel lookAndFeel;
    
    // Channel strips
    std::unique_ptr<Slider> sliderInputChannel[4];
    std::unique_ptr<Slider> sliderOutputChannel[4];
    std::unique_ptr<Slider> panInputChannel[4];
    std::unique_ptr<Slider> panOutputChannel[4];
    
    // EQ controls (3-band per channel)
    std::unique_ptr<Slider> eqInputChannel[4][3];  // [channel][band]
    std::unique_ptr<Slider> eqOutputChannel[4][3]; // [channel][band]
    
    std::unique_ptr<TextButton> buttonFFTInputChannel[4];
    std::unique_ptr<TextButton> buttonFFTOutputChannel[4];
    std::unique_ptr<TextButton> soloInputChannel[4];
    std::unique_ptr<TextButton> muteInputChannel[4];
    std::unique_ptr<TextButton> soloOutputChannel[4];
    std::unique_ptr<TextButton> muteOutputChannel[4];
    std::unique_ptr<TextButton> recInputChannel[4];

    std::unique_ptr<Label> labelInput[4];
    std::unique_ptr<Label> labelOutput[4];
    std::unique_ptr<Label> peakLabelInput[4];
    std::unique_ptr<Label> peakLabelOutput[4];
    
    std::unique_ptr<HighQualityMeter> meterInput[4];
    std::unique_ptr<HighQualityMeter> meterOutput[4];
    
    // Master section
    std::unique_ptr<Slider> masterGainSlider;
    std::unique_ptr<Label> masterLabel;
    std::unique_ptr<HighQualityMeter> masterMeter;
    
    // Preset system
    std::unique_ptr<ComboBox> presetComboBox;
    std::unique_ptr<TextButton> savePresetButton;
    std::unique_ptr<TextButton> loadPresetButton;
    
    // Peak hold counters
    int peakHoldInput[4] = {0, 0, 0, 0};
    int peakHoldOutput[4] = {0, 0, 0, 0};

};


class RoundedColourOutputComponentWithSelector : public Component, public ChangeListener
{
public:
    RoundedColourOutputComponentWithSelector(ProjectManager * pm, int i)
    {
        projectManager = pm;
        
        index = i;
        
        mainColour = Colours::green;

    }
    
    ~RoundedColourOutputComponentWithSelector() { }
    
    void paint(Graphics&g) override
    {
        ColourGradient gradient(mainColour, 0, 0, mainColour, getWidth(), getHeight(), false);
        
        g.setGradientFill(gradient);
        
        g.fillRoundedRectangle(0, 0, getWidth(), getHeight(), 6);
    }
    
    void mouseDown(const MouseEvent& event) override
    {

        auto colourSelector = std::make_unique<ColourSelector>();
        colourSelector->setName ("background");
        colourSelector->setCurrentColour (mainColour);
        colourSelector->setSize (300, 400);
        
        colourSelector->addChangeListener(this);

        CallOutBox::launchAsynchronously (std::unique_ptr<juce::Component>(colourSelector.release()), this->getScreenBounds(), nullptr);
        
    }
    
    void changeListenerCallback (ChangeBroadcaster* source) override
    {
        if (ColourSelector* cs = dynamic_cast <ColourSelector*> (source))
        {
            projectManager->setSettingsColorParameter(index, cs->getCurrentColour().getARGB());
        }
        
    }

    void setColour(Colour newColour)
    {
        mainColour = newColour;
        
        repaint();
    }
    
    void setColour(int agrb)
    {
        mainColour = Colour(agrb);
        
        repaint();
        
    }
    
private:
    ProjectManager * projectManager;
    Colour mainColour;
    
    float value = 1.f;
    int index = 0;
};


class SpectrogramSettingsPopup : public Component, public TextEditor::Listener, public Button::Listener, public ProjectManager::UIListener, public ComboBox::Listener
{
public:
    SpectrogramSettingsPopup(ProjectManager * pm)
    {
        projectManager = pm;
        projectManager->addUIListener(this);
        
        imageBackground = ImageCache::getFromMemory(BinaryData::SettingsSpectrogramBackground, BinaryData::SettingsSpectrogramBackgroundSize);
        imageCloseButton            = ImageCache::getFromMemory(BinaryData::CloseButton2x_png, BinaryData::CloseButton2x_pngSize);
        imageBlueButtonNormal       = ImageCache::getFromMemory(BinaryData::BlueButton_Normal_png, BinaryData::BlueButton_Normal_pngSize);
        imageBlueButtonSelected     = ImageCache::getFromMemory(BinaryData::BlueButton_Selected_png, BinaryData::BlueButton_Selected_pngSize);
        imageAddButton              = ImageCache::getFromMemory(BinaryData::ApplyButton2x_png, BinaryData::ApplyButton2x_pngSize);
        
        imageBlueCheckButtonNormal  = ImageCache::getFromMemory(BinaryData::Button_Checkbox_Normal_Max_png, BinaryData::Button_Checkbox_Normal_Max_pngSize);
        imageBlueCheckButtonSelected= ImageCache::getFromMemory(BinaryData::Button_Checkbox_Selected_Max_png, BinaryData::Button_Checkbox_Selected_Max_pngSize);
        
        // fonts
        Typeface::Ptr AssistantBold     = Typeface::createSystemTypefaceFor(BinaryData::AssistantBold_ttf, BinaryData::AssistantBold_ttfSize);
        Typeface::Ptr AssistantSemiBold   = Typeface::createSystemTypefaceFor(BinaryData::AssistantSemiBold_ttf, BinaryData::AssistantSemiBold_ttfSize);
        Font fontBold(AssistantBold);
        Font fontNormal(AssistantSemiBold);
        
        fontNormal.setHeight(40);
        fontBold.setHeight(30);
        
        button_Close = std::make_unique<ImageButton>();
        button_Close->setTriggeredOnMouseDown(true);
        button_Close->setImages (false, true, true,
                               imageCloseButton, 0.999f, Colour (0x00000000),
                               Image(), 1.000f, Colour (0x00000000),
                               imageCloseButton, 0.75, Colour (0x00000000));
        button_Close->addListener(this);
        addAndMakeVisible(button_Close.get());
        
        button_Add = std::make_unique<ImageButton>();
        button_Add->setTriggeredOnMouseDown(true);
        button_Add->setImages (false, true, true,
                                    imageAddButton, 0.999f, Colour (0x00000000),
                                    Image(), 1.000f, Colour (0x00000000),
                                    imageAddButton, 0.888, Colour (0x00000000));
        button_Add->addListener(this);
        addAndMakeVisible(button_Add.get());
        
        
        comboBox_FFTSize = std::make_unique<ComboBox>();
        comboBox_FFTSize->setLookAndFeel(&lookAndFeel);
        comboBox_FFTSize->addListener(this);
        PopupMenu * fftSizeMenu =  comboBox_FFTSize->getRootMenu();
        fftSizeMenu ->setLookAndFeel(&lookAndFeel);

        fftSizeMenu->addItem(1, "1024 Samples");
        fftSizeMenu->addItem(2, "2048 Samples");
        fftSizeMenu->addItem(3, "4096 Samples");
        fftSizeMenu->addItem(4, "8192 Samples");
        fftSizeMenu->addItem(5, "16384 Samples");
        fftSizeMenu->addItem(6, "32768 Samples");
        
        
        addAndMakeVisible(comboBox_FFTSize.get());
        comboBox_FFTSize->setBounds(1000, 880 + 46, 250, 41); //(242, 497, 125, 41);
        
        comboBox_FFTWindow = std::make_unique<ComboBox>();
        comboBox_FFTWindow->setLookAndFeel(&lookAndFeel);
        comboBox_FFTWindow->addListener(this);
        PopupMenu * fftWindowMenu =  comboBox_FFTWindow->getRootMenu();
        fftWindowMenu ->setLookAndFeel(&lookAndFeel);

        fftWindowMenu->addItem(1, "Rectangular");
        fftWindowMenu->addItem(2, "Triangular");
        fftWindowMenu->addItem(3, "Hann");
        fftWindowMenu->addItem(4, "Hamming");
        fftWindowMenu->addItem(5, "Blackman");
        fftWindowMenu->addItem(6, "BlackmanHarris");
        fftWindowMenu->addItem(7, "Flat Top");
        fftWindowMenu->addItem(8, "Kaiser");
        
        addAndMakeVisible(comboBox_FFTWindow.get());
        comboBox_FFTWindow->setBounds(1000, 980 + 46, 250, 41); //(242, 497, 125, 41);
        
        
        // Bool button
        button_HighestPeakFreqBool = std::make_unique<ImageButton>();
        button_HighestPeakFreqBool->setTriggeredOnMouseDown(true);
        button_HighestPeakFreqBool->setImages (false, true, true,
                                    imageBlueCheckButtonNormal, 0.999f, Colour (0x00000000),
                                    Image(), 1.000f, Colour (0x00000000),
                                    imageBlueCheckButtonSelected, 1.0, Colour (0x00000000));
        button_HighestPeakFreqBool->addListener(this);
        button_HighestPeakFreqBool->setBounds(666, 6, 38, 38);
        addAndMakeVisible(button_HighestPeakFreqBool.get());
        
        button_HighestPeakOctaveBool = std::make_unique<ImageButton>();
        button_HighestPeakOctaveBool->setTriggeredOnMouseDown(true);
        button_HighestPeakOctaveBool->setImages (false, true, true,
                                    imageBlueCheckButtonNormal, 0.999f, Colour (0x00000000),
                                    Image(), 1.000f, Colour (0x00000000),
                                    imageBlueCheckButtonSelected, 1.0, Colour (0x00000000));
        button_HighestPeakOctaveBool->addListener(this);
        button_HighestPeakOctaveBool->setBounds(666, 6, 38, 38);
        addAndMakeVisible(button_HighestPeakOctaveBool.get());
        
        textEditor_NumHighestPeakFreqs = std::make_unique<TextEditor>();
        textEditor_NumHighestPeakFreqs->setInputRestrictions(1, "0123456789");
        textEditor_NumHighestPeakFreqs->setBounds(432, 787, 125, 41);
        textEditor_NumHighestPeakFreqs->setText("5 peaks");
        textEditor_NumHighestPeakFreqs->addListener(this);
        textEditor_NumHighestPeakFreqs->setLookAndFeel(&lookAndFeel);
        textEditor_NumHighestPeakFreqs->setJustification(Justification::centred);
        textEditor_NumHighestPeakFreqs->setFont(fontNormal);
        textEditor_NumHighestPeakFreqs->setColour(TextEditor::textColourId, Colours::darkgrey);
        textEditor_NumHighestPeakFreqs->applyFontToAllText(fontNormal);
        textEditor_NumHighestPeakFreqs->applyColourToAllText(Colours::darkgrey);
        addAndMakeVisible(textEditor_NumHighestPeakFreqs.get());
        
        textEditor_NumHighestPeakOctaves = std::make_unique<TextEditor>();
        textEditor_NumHighestPeakOctaves->setInputRestrictions(1, "0123456789");
        textEditor_NumHighestPeakOctaves->setBounds(432, 787, 125, 41);
        textEditor_NumHighestPeakOctaves->setText("5 octaves");
        textEditor_NumHighestPeakOctaves->addListener(this);
        textEditor_NumHighestPeakOctaves->setLookAndFeel(&lookAndFeel);
        textEditor_NumHighestPeakOctaves->setJustification(Justification::centred);
        textEditor_NumHighestPeakOctaves->setFont(fontNormal);
        textEditor_NumHighestPeakOctaves->setColour(TextEditor::textColourId, Colours::darkgrey);
        textEditor_NumHighestPeakOctaves->applyFontToAllText(fontNormal);
        textEditor_NumHighestPeakOctaves->applyColourToAllText(Colours::darkgrey);
        addAndMakeVisible(textEditor_NumHighestPeakOctaves.get());
        
        textEditor_FFTDelay = std::make_unique<TextEditor>();
        textEditor_FFTDelay->setInputRestrictions(5, "0123456789");
        textEditor_FFTDelay->setBounds(432, 787, 125, 41);
        textEditor_FFTDelay->setText("1000ms");
        textEditor_FFTDelay->addListener(this);
        textEditor_FFTDelay->setLookAndFeel(&lookAndFeel);
        textEditor_FFTDelay->setJustification(Justification::centred);
        textEditor_FFTDelay->setFont(fontNormal);
        textEditor_FFTDelay->setColour(TextEditor::textColourId, Colours::darkgrey);
        textEditor_FFTDelay->applyFontToAllText(fontNormal);
        textEditor_FFTDelay->applyColourToAllText(Colours::darkgrey);
        addAndMakeVisible(textEditor_FFTDelay.get());
        
        colourSelectorFFTMain = new RoundedColourOutputComponentWithSelector(projectManager, 0);
        addAndMakeVisible(colourSelectorFFTMain);
        
        colourSelectorFFTSec = new RoundedColourOutputComponentWithSelector(projectManager, 1);
        addAndMakeVisible(colourSelectorFFTSec);
        
        colourSelectorOctaveMain = new RoundedColourOutputComponentWithSelector(projectManager, 2);
        addAndMakeVisible(colourSelectorOctaveMain);
        
        colourSelectorOctaveSec = new RoundedColourOutputComponentWithSelector(projectManager, 3);
        addAndMakeVisible(colourSelectorOctaveSec);
        
        colourSelectorColourMain = new RoundedColourOutputComponentWithSelector(projectManager, 4);
        addAndMakeVisible(colourSelectorColourMain);
        
        colourSelectorColourSec = new RoundedColourOutputComponentWithSelector(projectManager, 5);
        addAndMakeVisible(colourSelectorColourSec);
        
        colourSelector3DMain = new RoundedColourOutputComponentWithSelector(projectManager, 6);
        addAndMakeVisible(colourSelector3DMain);
        
        colourSelector3DSec = new RoundedColourOutputComponentWithSelector(projectManager, 7);
        addAndMakeVisible(colourSelector3DSec);
        
        colourSelectorFrequencyMain = new RoundedColourOutputComponentWithSelector(projectManager, 8);
        addAndMakeVisible(colourSelectorFrequencyMain);
        
        colourSelectorFrequencySec = new RoundedColourOutputComponentWithSelector(projectManager, 9);
        addAndMakeVisible(colourSelectorFrequencySec);
        
        colourSelectorLissajousMain = new RoundedColourOutputComponentWithSelector(projectManager, 10);
        addAndMakeVisible(colourSelectorLissajousMain);
        
        colourSelectorLissajousSec = new RoundedColourOutputComponentWithSelector(projectManager, 11);
        addAndMakeVisible(colourSelectorLissajousSec);
        
        
    }
    
    ~SpectrogramSettingsPopup()
    {
        
    }
    
    void paint (Graphics&g) override
    {
        g.drawImage(imageBackground, 0, 0, 1560 * scaleFactor, 1440 * scaleFactor, 0, 0, 1560, 1440);
    }
    
    void resized() override
    {
        button_Close                    ->setBounds(1380 * scaleFactor, 224 * scaleFactor, 150 * scaleFactor, 28 * scaleFactor);
        
        button_Add                      ->setBounds(609 * scaleFactor, 1040 * scaleFactor, 341 * scaleFactor, 84 * scaleFactor);
        
        comboBox_FFTSize                ->setBounds(156 * scaleFactor, 872 * scaleFactor, 200 * scaleFactor, 41 * scaleFactor);
        
        comboBox_FFTWindow              ->setBounds(420 * scaleFactor, 872 * scaleFactor, 200 * scaleFactor, 41 * scaleFactor);
        
        textEditor_NumHighestPeakFreqs  ->setBounds(600 * scaleFactor, 523 * scaleFactor, 125 * scaleFactor, 41 * scaleFactor);
        textEditor_NumHighestPeakFreqs  ->applyFontToAllText(33 * scaleFactor);
        
        textEditor_NumHighestPeakOctaves->setBounds(550 * scaleFactor, 662 * scaleFactor, 125 * scaleFactor, 41 * scaleFactor);
        textEditor_NumHighestPeakOctaves->applyFontToAllText(33 * scaleFactor);
        
        textEditor_FFTDelay             ->setBounds(954 * scaleFactor, 406 * scaleFactor, 125 * scaleFactor, 41 * scaleFactor);
        textEditor_FFTDelay             ->applyFontToAllText(33 * scaleFactor);
        
        button_HighestPeakFreqBool      ->setBounds(155 * scaleFactor, 472 * scaleFactor, 38 * scaleFactor, 38 * scaleFactor);
        button_HighestPeakOctaveBool    ->setBounds(155 * scaleFactor, 610 * scaleFactor, 38 * scaleFactor, 38 * scaleFactor);
        
        float mainX = 1227 * scaleFactor;
        float secX  = 1316 * scaleFactor;
        float fftY  = 595 * scaleFactor;
        float ySpace = 65 * scaleFactor;
        float colourW = 64 * scaleFactor;
        float colourH = 27 * scaleFactor;

        colourSelectorFFTMain   ->setBounds(mainX, fftY + (ySpace * 0), colourW, colourH);
        colourSelectorFFTSec    ->setBounds(secX, fftY + (ySpace * 0), colourW, colourH);
        
        colourSelectorOctaveMain->setBounds(mainX, fftY + (ySpace * 1), colourW, colourH);
        colourSelectorOctaveSec->setBounds(secX, fftY + (ySpace * 1), colourW, colourH);
        
        colourSelectorColourMain->setBounds(mainX, fftY + (ySpace * 2), colourW, colourH);
        colourSelectorColourSec->setBounds(secX, fftY + (ySpace * 2), colourW, colourH);
        
        colourSelector3DMain->setBounds(mainX, fftY + (ySpace * 3), colourW, colourH);
        colourSelector3DSec->setBounds(secX, fftY + (ySpace * 3), colourW, colourH);
        
        colourSelectorFrequencyMain->setBounds(mainX, fftY + (ySpace * 4), colourW, colourH);
        colourSelectorFrequencySec->setBounds(secX, fftY + (ySpace * 4), colourW, colourH);
        
        colourSelectorLissajousMain->setBounds(mainX, fftY + (ySpace * 5), colourW, colourH);
        colourSelectorLissajousSec->setBounds(secX, fftY + (ySpace * 5), colourW, colourH);


    }
    
    // Text editor listener overrides
    void textEditorTextChanged (TextEditor&editor)override {}
    void textEditorReturnKeyPressed (TextEditor&editor)override
    {
        if (&editor == textEditor_NumHighestPeakFreqs.get())
        {
            // add hz to the string
            int value = editor.getText().getIntValue();
                
            // send to projectManager
            projectManager->setProjectSettingsParameter(NUMBER_HIGHEST_PEAK_FREQUENCIES, value);
        }
        else if (&editor == textEditor_NumHighestPeakOctaves.get())
        {
            // add hz to the string
            int value = editor.getText().getIntValue();
                
            // send to projectManager
            projectManager->setProjectSettingsParameter(NUMBER_HIGHEST_PEAK_OCTAVES, value);
        }
        else if (&editor == textEditor_FFTDelay.get())
        {
            // add hz to the string
            int value = editor.getText().getIntValue();
                
            // send to projectManager
            projectManager->setProjectSettingsParameter(FFT_DELAY_MS, value);
        }
    }
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged)override
    {
        if (comboBoxThatHasChanged == comboBox_FFTSize.get())
        {
            projectManager->setProjectSettingsParameter(FFT_SIZE, comboBox_FFTSize->getSelectedId());
        }
        else if (comboBoxThatHasChanged == comboBox_FFTWindow.get())
        {
            projectManager->setProjectSettingsParameter(FFT_WINDOW, comboBox_FFTWindow->getSelectedId());
        }
    }
    
    void buttonClicked (Button*button) override
    {
        if (button == button_HighestPeakFreqBool.get())
        {
            projectManager->setProjectSettingsParameter(SHOW_HIGHEST_PEAK_FREQUENCY, !button->getToggleState());
        }
        else if (button == button_HighestPeakOctaveBool.get())
        {
            projectManager->setProjectSettingsParameter(SHOW_HIGHEST_PEAK_OCTAVES, !button->getToggleState());
        }
        else if (button == button_Close.get())
        {
            setVisible(false);
        }
        else if (button == button_Add.get())
        {
            setVisible(false);
        }
    }
    
    void updateUI(int settingIndex)
    {
        if (settingIndex == FFT_SIZE)
        {
            int size = (int)projectManager->getProjectSettingsParameter(FFT_SIZE);
            comboBox_FFTSize->setSelectedId(size, dontSendNotification);
        }
        else if (settingIndex == FFT_WINDOW)
        {
            int size = (int)projectManager->getProjectSettingsParameter(FFT_WINDOW);
            comboBox_FFTWindow->setSelectedId(size, dontSendNotification);
        }
        else if (settingIndex == SHOW_HIGHEST_PEAK_FREQUENCY)
        {
            bool b = projectManager->getProjectSettingsParameter(SHOW_HIGHEST_PEAK_FREQUENCY);
            button_HighestPeakFreqBool->setToggleState(b, dontSendNotification);
        }
        else if (settingIndex == NUMBER_HIGHEST_PEAK_FREQUENCIES)
        {
            int val = projectManager->getProjectSettingsParameter(NUMBER_HIGHEST_PEAK_FREQUENCIES);
            
            String valString(val); textEditor_NumHighestPeakFreqs->setText(valString);
        }
        else if (settingIndex == SHOW_HIGHEST_PEAK_OCTAVES)
        {
            bool b = projectManager->getProjectSettingsParameter(SHOW_HIGHEST_PEAK_OCTAVES);
            button_HighestPeakOctaveBool->setToggleState(b, dontSendNotification);
        }
        else if (settingIndex == NUMBER_HIGHEST_PEAK_OCTAVES)
        {
            int val = projectManager->getProjectSettingsParameter(NUMBER_HIGHEST_PEAK_OCTAVES);
            
            String valString(val); textEditor_NumHighestPeakOctaves->setText(valString);
        }
        else if (settingIndex == FFT_DELAY_MS)
        {
            int val = projectManager->getProjectSettingsParameter(FFT_DELAY_MS);
            
            String valString(val); textEditor_FFTDelay->setText(valString);
        }
        else if (settingIndex == FFT_COLOR_SPEC_MAIN)
        {
            uint32 col = projectManager->getSettingsColorParameter(0);
            colourSelectorFFTMain->setColour(col);
        }
        else if (settingIndex == FFT_COLOR_SPEC_SEC)
        {
            uint32 col = projectManager->getSettingsColorParameter(1);
            colourSelectorFFTSec->setColour(col);
        }
        
        else if (settingIndex == FFT_COLOR_OCTAVE_MAIN)
        {
            uint32 col = projectManager->getSettingsColorParameter(2);
            colourSelectorOctaveMain->setColour(col);
        }
        else if (settingIndex == FFT_COLOR_OCTAVE_SEC)
        {
            uint32 col = projectManager->getSettingsColorParameter(3);
            colourSelectorOctaveSec->setColour(col);
        }
        
        else if (settingIndex == FFT_COLOR_COLOR_MAIN)
        {
            uint32 col = projectManager->getSettingsColorParameter(4);
            colourSelectorColourMain->setColour(col);
        }
        else if (settingIndex == FFT_COLOR_COLOR_SEC)
        {
            uint32 col = projectManager->getSettingsColorParameter(5);
            colourSelectorColourSec->setColour(col);
        }
        
        else if (settingIndex == FFT_COLOR_3D_MAIN)
        {
            uint32 col = projectManager->getSettingsColorParameter(6);
            colourSelector3DMain->setColour(col);
        }
        else if (settingIndex == FFT_COLOR_3D_SEC)
        {
            uint32 col = projectManager->getSettingsColorParameter(7);
            colourSelector3DSec->setColour(col);
        }
        
        else if (settingIndex == FFT_COLOR_FREQUENCY_MAIN)
        {
            uint32 col = projectManager->getSettingsColorParameter(8);
            colourSelectorFrequencyMain->setColour(col);
        }
        else if (settingIndex == FFT_COLOR_FREQUENCY_SEC)
        {
            uint32 col = projectManager->getSettingsColorParameter(9);
            colourSelectorFrequencySec->setColour(col);
        }
        
        else if (settingIndex == LISSAJOUS_COLOR_MAIN)
        {
            uint32 col = projectManager->getSettingsColorParameter(10);
            colourSelectorLissajousMain->setColour(col);
        }
        else if (settingIndex == LISSAJOUS_COLOR_SEC)
        {
            uint32 col = projectManager->getSettingsColorParameter(11);
            colourSelectorLissajousSec->setColour(col);
        }
    }
    
    // listener override
    void updateSettingsUIParameter(int settingIndex)override
    {
        updateUI(settingIndex);
    }
    
    float scaleFactor = 0.5;
    void setScale(float factor)
    {
        scaleFactor = factor;
        lookAndFeel.setScale(scaleFactor);
    }

private:
    
    CustomLookAndFeel lookAndFeel;
    
    ProjectManager * projectManager;
    
    Image imageBackground;
    
    std::unique_ptr<ImageButton> button_Add;
    std::unique_ptr<ImageButton> button_Close;
    
    std::unique_ptr<ImageButton>  button_HighestPeakFreqBool;
    std::unique_ptr<ImageButton>  button_HighestPeakOctaveBool;
    
    std::unique_ptr<TextEditor> textEditor_FFTDelay;
    std::unique_ptr<TextEditor>   textEditor_NumHighestPeakFreqs;
    std::unique_ptr<TextEditor>   textEditor_NumHighestPeakOctaves;
    
    
    std::unique_ptr<ComboBox> comboBox_FFTSize;
    std::unique_ptr<ComboBox> comboBox_FFTWindow;
    
    Image imageAddButton;
    Image imageCloseButton;
    Image imageBlueCheckButtonNormal;
    Image imageBlueCheckButtonSelected;
    Image imageBlueButtonNormal;
    Image imageBlueButtonSelected;
    
    RoundedColourOutputComponentWithSelector *colourSelectorFFTMain;
    RoundedColourOutputComponentWithSelector *colourSelectorFFTSec;
    
    RoundedColourOutputComponentWithSelector *colourSelectorOctaveMain;
    RoundedColourOutputComponentWithSelector *colourSelectorOctaveSec;
    
    RoundedColourOutputComponentWithSelector *colourSelectorColourMain;
    RoundedColourOutputComponentWithSelector *colourSelectorColourSec;
    
    RoundedColourOutputComponentWithSelector *colourSelector3DMain;
    RoundedColourOutputComponentWithSelector *colourSelector3DSec;
    
    RoundedColourOutputComponentWithSelector *colourSelectorFrequencyMain;
    RoundedColourOutputComponentWithSelector *colourSelectorFrequencySec;
    
    RoundedColourOutputComponentWithSelector *colourSelectorLissajousMain;
    RoundedColourOutputComponentWithSelector *colourSelectorLissajousSec;
    
    
    
};


class SettingsComponent :
    public MenuViewInterface,
    public TextEditor::Listener,
    public Button::Listener,
    public ProjectManager::UIListener,
    public ComboBox::Listener
{
public:
    SettingsComponent(ProjectManager * pm);
    ~SettingsComponent();

    void paint (Graphics&g) override;
    void resized() override;
    
    // Text editor listener overrides
    void textEditorTextChanged (TextEditor&editor)override;
    void textEditorReturnKeyPressed (TextEditor&editor)override;
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged)override;
    void buttonClicked (Button*button) override;
    
    void updateUI(int settingIndex);
    
    // listener override
    void updateSettingsUIParameter(int settingIndex)override;
    
    float scaleFactor = 0.5;
    void setScale(float factor) override
    {
        scaleFactor = factor;
        lookAndFeel.setScale(scaleFactor);
        component_AudioSettings->setScale(scaleFactor);
        component_AudioSettings->resized();
    }

private:
    
    CustomLookAndFeel lookAndFeel;
    
    ProjectManager * projectManager;
    
    SpectrogramSettingsPopup * spectrogramPopupComponent;
    
    AudioDeviceSelectorComponent * audioSetupComponent;
    std::unique_ptr<ImageButton> textButton_OpenAudioSettings;
    
    class AudioSettingsContainer : public Component, public Button::Listener
    { public: // simple container class to open and close the audio settings
        AudioSettingsContainer(){
            mainBackgroundImage         = ImageCache::getFromMemory(BinaryData::AudioSettingsBackground_png, BinaryData::AudioSettingsBackground_pngSize); imageCloseButton            = ImageCache::getFromMemory(BinaryData::CloseButton2x_png, BinaryData::CloseButton2x_pngSize);
            
            button_Close = std::make_unique<ImageButton>(); button_Close->setTriggeredOnMouseDown(true);
            button_Close->setImages (false, true, true,
                                     imageCloseButton, 0.999f, Colour (0x00000000),
                                     Image(), 1.000f, Colour (0x00000000),
                                     imageCloseButton, 0.75, Colour (0x00000000));
            button_Close->addListener(this); button_Close->setBounds(1407, 130, 150, 28);
            addAndMakeVisible(button_Close.get());
        }
        ~AudioSettingsContainer(){}
        
        void paint (Graphics&g) override
        {
            g.setColour(Colours::black);
            g.setOpacity(0.88);
            g.fillAll(); g.setOpacity(1.0);
            g.drawImage(mainBackgroundImage, 24 * scaleFactor, 94 * scaleFactor, 1502*scaleFactor, 1242*scaleFactor, 0, 0, 1502, 1242);
            
        }
        
        void resized() override
        {
            button_Close->setBounds(1407 * scaleFactor, 130 * scaleFactor, 150 * scaleFactor, 28 * scaleFactor);
        }
        void buttonClicked (Button*button) override { setVisible(false); }
        void setOpen(bool shouldOpen) { setVisible(true); }
        
        float scaleFactor = 0.5;
        void setScale(float factor) { scaleFactor = factor; }
        
        
    private:
        std::unique_ptr<ImageButton> button_Close; Image mainBackgroundImage; Image imageCloseButton;
    };
    
    AudioSettingsContainer* component_AudioSettings;

    Image imageBackground;
    
    std::unique_ptr<TextEditor> textEditor_AmplitudeMin;
    std::unique_ptr<TextEditor> textEditor_AttackMin;
    std::unique_ptr<TextEditor> textEditor_DecayMin;
    std::unique_ptr<TextEditor> textEditor_SustainMin;
    std::unique_ptr<TextEditor> textEditor_ReleaseMin;
    
    std::unique_ptr<TextEditor> textEditor_AmplitudeMax;
    std::unique_ptr<TextEditor> textEditor_AttackMax;
    std::unique_ptr<TextEditor> textEditor_DecayMax;
    std::unique_ptr<TextEditor> textEditor_SustainMax;
    std::unique_ptr<TextEditor> textEditor_ReleaseMax;
    
    std::unique_ptr<Label>  label_LogFileLocation;
    std::unique_ptr<Label>  label_RecordFileLocation;
    
    std::unique_ptr<ImageButton>  button_LogFileLocation;
    std::unique_ptr<ImageButton>  button_RecordFileLocation;
    
    std::unique_ptr<TextEditor> textEditor_NotesFrequency;
    std::unique_ptr<Label> label_B_Frequency;
    std::unique_ptr<Label> label_C_Frequency;
    std::unique_ptr<Label> label_D_Frequency;
    std::unique_ptr<Label> label_E_Frequency;
    std::unique_ptr<Label> label_F_Frequency;
    std::unique_ptr<Label> label_G_Frequency;
    
    std::unique_ptr<ImageButton> textButton_ResetAmp;
    std::unique_ptr<ImageButton> textButton_ResetAttack;
    std::unique_ptr<ImageButton> textButton_ResetDecay;
    std::unique_ptr<ImageButton> textButton_ResetSustain;
    std::unique_ptr<ImageButton> textButton_ResetRelease;
    
    std::unique_ptr<ImageButton> textButton_ChangeLogFile;
    std::unique_ptr<ImageButton> textButton_ChangeRecordFile;
    
    std::unique_ptr<ImageButton> textButton_SpectrogramSetttingsPopup;
    
    std::unique_ptr<ImageButton> buttonScale25;
    std::unique_ptr<ImageButton> buttonScale50;
    std::unique_ptr<ImageButton> buttonScale75;
    std::unique_ptr<ImageButton> buttonScale100;
    
    std::unique_ptr<TextButton> buttonMixer;
    
    Image imageButtonScale25Normal;
    Image imageButtonScale25Selected;
    
    Image imageButtonScale50Normal;
    Image imageButtonScale50Selected;
    
    Image imageButtonScale75Normal;
    Image imageButtonScale75Selected;
    
    Image imageButtonScale100Normal;
    Image imageButtonScale100Selected;
    
    Image imageButtonSave;
    Image imageButtonLoad;
    Image imageButtonAudio;
    Image imageButtonChange;
    Image imageButtonReset;
    Image imageButtonScalesAdd;
    Image imageButtonSpectrogram;
    Image imageButtonScan;
    
    std::unique_ptr<ComboBox> comboBox_Scales;
    std::unique_ptr<ComboBox> comboBox_GUIScale;
    std::unique_ptr<ComboBox> comboBox_RecordFormat;
    
    std::unique_ptr<ComboBox> comboBox_NoiseType;
    
//    Label * fftWindowLabel;
//    Label * fftLabel;
//    Label * scalesLabel;
//    Label * guiScaleLabel;
//    
//    Label * noiseTypeLabel;
    
    
    // Plugin Slot code
    
    void rescanPlugins();
    std::unique_ptr<ImageButton>   scanPluginsButton;
    
    Image imageAddPluginIcon;
    Image removePluginIcon;
    Image openWindowPluginIcon;
    
    Image imageBlueButtonNormal;
    Image imageBlueButtonSelected;
    Image imageAddButton;
    Image imageBlueCheckButtonNormal;
    Image imageBlueCheckButtonSelected;
    Image imagePluginPlayButton;
    
    Label *         labelPluginSlot[NUM_PLUGIN_SLOTS];
    ImageButton *   buttonAddPlugin[NUM_PLUGIN_SLOTS];
    ImageButton *   buttonRemovePlugin[NUM_PLUGIN_SLOTS];
    ImageButton *   buttonOpenPlugin[NUM_PLUGIN_SLOTS];
    ComboBox *      comboBoxPluginSelector[NUM_PLUGIN_SLOTS];
    
    std::unique_ptr<ImageButton>   button_Save;
    std::unique_ptr<ImageButton>   button_Load;
    
    std::unique_ptr<ImageButton>  button_FreqToChordMainHarmonics;
    std::unique_ptr<ImageButton>  button_FreqToChordAverage;
    std::unique_ptr<ImageButton>  button_FreqToChordEMA;

    ComboBox fundamentalFrequencyAlgorithmChooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettingsComponent)
};
