/*
  ==============================================================================

    SpectraHarmonicsChart.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "ProjectManager.h"
#include "ResponsiveUIHelper.h"

class SpectraHarmonicsChart : public ResponsiveComponent,
                               public Timer
{
public:
    SpectraHarmonicsChart(ProjectManager* pm);
    ~SpectraHarmonicsChart() override;
    
    void paint(Graphics& g) override;
    void resized() override;
    
    void timerCallback() override;
    
    // Set FFT data for analysis
    void setFFTData(const float* fftData, int fftSize, float sampleRate);
    
    // Set fundamental frequency for harmonic detection
    void setFundamentalFrequency(float freq) { fundamentalFreq = freq; }
    
    // Export chart as image
    void exportSnapshot();
    
    // Display modes
    enum DisplayMode
    {
        BARS,           // Bar chart
        LINES,          // Line chart
        STEM_PLOT,      // Stem plot
        WATERFALL       // 3D waterfall
    };
    
    void setDisplayMode(DisplayMode mode) { currentMode = mode; repaint(); }
    
protected:
    void updateScaledValues() override;
    
private:
    ProjectManager* projectManager;
    
    // Data
    Array<float> harmonicFrequencies;
    Array<float> harmonicAmplitudes;
    float fundamentalFreq = 440.0f;
    float sampleRate = 44100.0f;
    int maxHarmonics = 16;
    
    // Display settings
    DisplayMode currentMode = BARS;
    float minDb = -80.0f;
    float maxDb = 0.0f;
    bool showPeakLabels = true;
    bool showGridLines = true;
    
    // Analysis methods
    void detectHarmonics(const float* fftData, int fftSize);
    float getAmplitudeAtFrequency(const float* fftData, int fftSize, float frequency);
    float interpolateAmplitude(const float* fftData, int fftSize, float binIndex);
    
    // Drawing methods
    void drawBackground(Graphics& g);
    void drawGridLines(Graphics& g);
    void drawBars(Graphics& g);
    void drawLines(Graphics& g);
    void drawStemPlot(Graphics& g);
    void drawAxis(Graphics& g);
    void drawLegend(Graphics& g);
    void drawPeakLabels(Graphics& g);
    
    // Helper methods
    Rectangle<float> getChartArea() const;
    float frequencyToX(float freq, const Rectangle<float>& area) const;
    float amplitudeToY(float amp, const Rectangle<float>& area) const;
    
    // UI Components
    std::unique_ptr<ComboBox> modeSelector;
    std::unique_ptr<Slider> harmonicsSlider;
    std::unique_ptr<TextButton> exportButton;
    std::unique_ptr<ToggleButton> peakLabelsToggle;
    std::unique_ptr<ToggleButton> gridToggle;
    
    // Scaled values
    int scaledMargin = 10;
    int scaledAxisLabelHeight = 20;
    float scaledFontSize = 12.0f;
    float scaledBarWidth = 20.0f;
    
    // Colors
    Colour barColor = Colours::cyan;
    Colour lineColor = Colours::orange;
    Colour gridColor = Colours::darkgrey.withAlpha(0.3f);
    Colour textColor = Colours::white;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectraHarmonicsChart)
};