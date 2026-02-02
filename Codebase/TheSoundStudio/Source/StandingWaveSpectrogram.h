/*
  ==============================================================================

    StandingWaveSpectrogram.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "ProjectManager.h"
#include "ResponsiveUIHelper.h"
#include <vector>
#include <deque>

class StandingWaveSpectrogram : public ResponsiveComponent,
                                 public juce::Timer
{
public:
    StandingWaveSpectrogram(ProjectManager* pm);
    ~StandingWaveSpectrogram() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void timerCallback() override;

    enum WaveMode
    {
        MODE_1D,        // 1D standing wave (string)
        MODE_2D_RECT,   // 2D rectangular membrane
        MODE_2D_CIRC,   // 2D circular membrane (drum)
        MODE_3D         // 3D cavity modes
    };

    void setWaveMode(WaveMode mode) { currentMode = mode; repaint(); }
    void setFrequency(float freq) { frequency = freq; }
    void setWaveSpeed(float speed) { waveSpeed = speed; }
    void setDamping(float damp) { damping = juce::jlimit(0.0f, 1.0f, damp); }

    void setAnimationSpeed(float speed) { animationSpeed = speed; }
    void toggleAnimation(bool enable) { animationEnabled = enable; }

protected:
    void updateScaledValues() override;

private:
    ProjectManager* projectManager;

    WaveMode currentMode = MODE_1D;
    float frequency = 440.0f;
    float waveSpeed = 343.0f; // speed of sound in air (m/s)
    float damping = 0.05f;

    float animationSpeed = 1.0f;
    bool animationEnabled = true;
    float phase = 0.0f;

    // Wave parameters
    int numNodes = 2;
    float amplitude = 1.0f;

    // Historical data for spectrogram
    static constexpr int historySize = 200;
    std::deque<std::vector<float>> waveHistory;

    // UI Components
    std::unique_ptr<juce::ComboBox> modeSelector;
    std::unique_ptr<juce::Slider> frequencySlider;
    std::unique_ptr<juce::Slider> dampingSlider;
    std::unique_ptr<juce::Slider> nodeSlider;
    std::unique_ptr<juce::ToggleButton> animationToggle;
    std::unique_ptr<juce::TextButton> clearButton;

    // Calculation methods
    void calculate1DWave(std::vector<float>& wave, float time);
    void calculate2DRectWave(std::vector<std::vector<float>>& wave, float time);
    void calculate2DCircWave(std::vector<std::vector<float>>& wave, float time);
    void updateWaveData();

    // Drawing methods
    void draw1DStandingWave(juce::Graphics& g);
    void draw2DRectWave(juce::Graphics& g);
    void draw2DCircWave(juce::Graphics& g);
    void drawWaveHistory(juce::Graphics& g);
    void drawNodes(juce::Graphics& g);
    void drawControls(juce::Graphics& g);
    void drawInfo(juce::Graphics& g);

    // Helper methods
    juce::Rectangle<float> getWaveArea() const;
    float calculateWavelength() const { return waveSpeed / frequency; }
    int calculateNumNodes() const;

    // Scaled values
    int scaledMargin = 10;
    float scaledFontSize = 12.0f;
    float scaledNodeRadius = 4.0f;

    // Colors
    juce::Colour waveColor = juce::Colours::cyan;
    juce::Colour nodeColor = juce::Colours::red;
    juce::Colour antinodeColor = juce::Colours::yellow;
    juce::Colour gridColor = juce::Colours::darkgrey.withAlpha(0.3f);
    juce::Colour textColor = juce::Colours::white;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StandingWaveSpectrogram)
};