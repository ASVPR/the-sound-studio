/*
  ==============================================================================

    EnhancedSpectrogramComponent.h
    Created: 3 Sep 2025 7:25:00pm
    Author:  The Sound Studio Team

    Enhanced Spectrogram Component demonstrating the 4 FFT improvements:
    - Uses shared EnhancedFFTEngine
    - Displays high-resolution frequency data
    - Shows real-time peak detection
    - Renders color spectrum visualization

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "EnhancedFFTEngine.h"
#include "ProjectManager.h"
#include <memory>

//==============================================================================
/**
    Enhanced Spectrogram Component - Modern FFT visualization with improvements
    
    This component replaces the old SpectrogramComponent with:
    1. Shared FFT processing (reduced CPU usage)
    2. High-resolution frequency bins (better detail)
    3. Real-time peak detection (measurement tools)
    4. Color spectrum display (enhanced visualization)
*/
class EnhancedSpectrogramComponent : public Component,
                                   public EnhancedFFTEngine::Listener,
                                   public Timer,
                                   public Button::Listener,
                                   public Slider::Listener
{
public:
    EnhancedSpectrogramComponent(ProjectManager* pm, std::shared_ptr<EnhancedFFTEngine> engine);
    ~EnhancedSpectrogramComponent() override;
    
    //==============================================================================
    // Component overrides
    void paint(Graphics& g) override;
    void resized() override;
    void timerCallback() override;
    
    //==============================================================================
    // Mouse interaction
    void mouseDown(const MouseEvent& event) override;
    void mouseDrag(const MouseEvent& event) override;
    void mouseWheelMove(const MouseEvent& event, const MouseWheelDetails& wheel) override;
    
    //==============================================================================
    // Enhanced FFT Engine callbacks
    void fftDataUpdated(const EnhancedFFTEngine& engine) override;
    void peaksUpdated(const std::vector<PeakInfo>& peaks) override;
    void octaveBandsUpdated(const std::array<OctaveBand, NUM_OCTAVE_BANDS>& bands) override;
    
    //==============================================================================
    // UI Controls
    void buttonClicked(Button* button) override;
    void sliderValueChanged(Slider* slider) override;
    
    //==============================================================================
    // Display modes
    enum DisplayMode
    {
        SPECTRUM,           // Traditional spectrum display
        OCTAVE_BANDS,       // Musical octave bands
        COLOR_SPECTRUM,     // Color-coded frequency display
        PEAKS_OVERLAY       // Show detected peaks
    };
    
    void setDisplayMode(DisplayMode mode) { displayMode = mode; repaint(); }
    DisplayMode getDisplayMode() const { return displayMode; }
    
    //==============================================================================
    // Visualization settings
    void setFrequencyRange(float minHz, float maxHz);
    void setAmplitudeRange(float minDB, float maxDB);
    void setColorMode(EnhancedFFTEngine::ColorMode mode);
    
    // Peak detection display
    void setShowPeaks(bool shouldShow) { showPeaks = shouldShow; repaint(); }
    void setPeakLabels(bool shouldShowLabels) { showPeakLabels = shouldShowLabels; repaint(); }
    
private:
    //==============================================================================
    // Core components
    ProjectManager* projectManager;
    std::shared_ptr<EnhancedFFTEngine> fftEngine;
    
    //==============================================================================
    // Display settings
    DisplayMode displayMode = SPECTRUM;
    
    // Frequency range
    float minFrequency = 20.0f;
    float maxFrequency = 20000.0f;
    
    // Amplitude range
    float minAmplitudeDB = -80.0f;
    float maxAmplitudeDB = 0.0f;
    
    // Peak display
    bool showPeaks = true;
    bool showPeakLabels = true;
    int maxDisplayedPeaks = 10;
    
    //==============================================================================
    // Cached data for rendering
    std::vector<PeakInfo> currentPeaks;
    std::array<OctaveBand, NUM_OCTAVE_BANDS> currentOctaveBands;
    bool dataChanged = false;
    
    //==============================================================================
    // UI Controls
    std::unique_ptr<TextButton> modeButton;
    std::unique_ptr<TextButton> colorButton;
    std::unique_ptr<TextButton> peaksButton;
    std::unique_ptr<Slider> frequencyRangeSlider;
    std::unique_ptr<Slider> amplitudeRangeSlider;
    
    //==============================================================================
    // Rendering methods
    void drawSpectrum(Graphics& g);
    void drawOctaveBands(Graphics& g);
    void drawColorSpectrum(Graphics& g);
    void drawPeaksOverlay(Graphics& g);
    
    void drawFrequencyGrid(Graphics& g);
    void drawAmplitudeGrid(Graphics& g);
    void drawFrequencyLabels(Graphics& g);
    void drawAmplitudeLabels(Graphics& g);
    
    //==============================================================================
    // Coordinate conversion
    float frequencyToX(float frequency) const;
    float amplitudeToY(float amplitudeDB) const;
    float xToFrequency(float x) const;
    float yToAmplitude(float y) const;
    
    //==============================================================================
    // Color utilities
    Colour getAmplitudeColor(float amplitudeDB) const;
    Colour getFrequencyColor(float frequency) const;
    
    //==============================================================================
    // Layout
    Rectangle<int> spectrumArea;
    Rectangle<int> controlsArea;
    
    void setupControls();
    void layoutControls();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EnhancedSpectrogramComponent)
};