/*
  ==============================================================================

    HarmonicsSurface3D.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once

#include "OpenGLVisualizerBase.h"
#include <deque>

class HarmonicsSurface3D : public OpenGLVisualizerBase
{
public:
    HarmonicsSurface3D(ProjectManager* pm);
    ~HarmonicsSurface3D() override;

    enum DisplayMode
    {
        SURFACE,
        WIREFRAME,
        POINTS,
        FILLED_WIREFRAME
    };

    void setDisplayMode(DisplayMode mode) { displayMode = mode; }
    void setMaxHarmonics(int num) { maxHarmonics = juce::jlimit(1, 32, num); }
    void setHistorySize(int size) { historySize = juce::jlimit(10, 500, size); }
    void setAmplitudeScale(float scale) { amplitudeScale = scale; }

    void resized() override;

protected:
    void initializeGL() override;
    void renderScene() override;
    void cleanupGL() override;
    void updateVisualizationData() override;

private:
    void buildSurfaceMesh();
    void calculateNormals();

    juce::Vector3D<float> calculateNormal(int x, int z);

    juce::Colour getFrequencyColor(float frequency) const;
    juce::Colour getAmplitudeColor(float amplitude) const;

    DisplayMode displayMode = SURFACE;

    int maxHarmonics = 16;
    int historySize = 100;
    float amplitudeScale = 1.0f;

    float fundamentalFreq = 440.0f;
    float sampleRate = 44100.0f;

    std::deque<std::vector<float>> harmonicHistory;

    std::unique_ptr<juce::ComboBox> modeSelector;
    std::unique_ptr<juce::Slider> harmonicsSlider;
    std::unique_ptr<juce::Slider> historySlider;
    std::unique_ptr<juce::Slider> scaleSlider;
    std::unique_ptr<juce::ToggleButton> gridToggle;
    std::unique_ptr<juce::ToggleButton> axesToggle;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HarmonicsSurface3D)
};