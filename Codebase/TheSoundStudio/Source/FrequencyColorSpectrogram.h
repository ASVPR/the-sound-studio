/*
  ==============================================================================

    FrequencyColorSpectrogram.h
    Created: 12 Sep 2025
    Author:  The Sound Studio Team
    
    Frequency Color Spectrogram - converts audio frequencies to visible light colors

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "ProjectManager.h"
#include "ResponsiveUIHelper.h"

class FrequencyColorSpectrogram : public ResponsiveComponent, 
                                  public Timer,
                                  public ComboBox::Listener
{
public:
    FrequencyColorSpectrogram(ProjectManager* pm);
    ~FrequencyColorSpectrogram() override;
    
    void paint(Graphics& g) override;
    void resized() override;
    
    void timerCallback() override;
    void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;
    
    // Set FFT data
    void setFFTData(const float* fftData, int fftSize);
    
    // Phase speed options for wavelength calculation
    enum PhaseSpeed
    {
        SOUND_AIR,      // 343 m/s (sound in air)
        SOUND_WATER,    // 1480 m/s (sound in water)  
        LIGHT_VACUUM,   // 299,792,458 m/s (light in vacuum)
        LIGHT_AIR       // 299,727,738 m/s (light at Earth's surface)
    };
    
    void setPhaseSpeed(PhaseSpeed speed) { currentPhaseSpeed = speed; }
    
protected:
    void updateScaledValues() override;
    
private:
    ProjectManager* projectManager;
    
    // FFT data
    Array<float> magnitudes;
    Array<float> frequencies;
    int fftSize = 0;
    float sampleRate = 44100.0f;
    
    // UI Components
    std::unique_ptr<ComboBox> phaseSpeedCombo;
    std::unique_ptr<Label> phaseSpeedLabel;
    
    // Display settings
    PhaseSpeed currentPhaseSpeed = LIGHT_AIR;
    float minFrequency = 20.0f;
    float maxFrequency = 20000.0f;
    float minDb = -80.0f;
    float maxDb = 0.0f;
    
    // Color conversion
    Colour frequencyToColor(float frequency);
    float frequencyToWavelength(float frequency);
    Colour wavelengthToColor(float wavelengthNm);
    
    // Constants for light speed (m/s)
    float getPhaseSpeedMetersPerSec();
    
    // Helper to find peak frequencies
    void findPeakFrequencies(Array<float>& peaks, Array<float>& peakMagnitudes);
    
    // Drawing helpers
    void drawBackground(Graphics& g);
    void drawFrequencyColumns(Graphics& g);
    void drawAxis(Graphics& g);
    void drawLegend(Graphics& g);
    
    // Scaled values
    int scaledMargin = 10;
    int scaledAxisLabelHeight = 20;
    float scaledFontSize = 12.0f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FrequencyColorSpectrogram)
};