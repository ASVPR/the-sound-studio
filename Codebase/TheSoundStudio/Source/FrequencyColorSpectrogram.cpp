/*
  ==============================================================================

    FrequencyColorSpectrogram.cpp
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#include "FrequencyColorSpectrogram.h"
#include <cmath>

FrequencyColorSpectrogram::FrequencyColorSpectrogram(ProjectManager* pm)
    : projectManager(pm)
{
    // Create phase speed selector
    phaseSpeedLabel = std::make_unique<Label>("", "Phase Speed:");
    phaseSpeedLabel->setJustificationType(Justification::right);
    addAndMakeVisible(phaseSpeedLabel.get());
    
    phaseSpeedCombo = std::make_unique<ComboBox>();
    phaseSpeedCombo->addItem("Sound/Air", 1);
    phaseSpeedCombo->addItem("Sound/Water", 2);
    phaseSpeedCombo->addItem("Light/Vacuum", 3);
    phaseSpeedCombo->addItem("Light/Air", 4);
    phaseSpeedCombo->setSelectedId(4); // Default to Light/Air
    phaseSpeedCombo->addListener(this);
    addAndMakeVisible(phaseSpeedCombo.get());
    
    startTimer(50); // 20 FPS update rate
}

FrequencyColorSpectrogram::~FrequencyColorSpectrogram()
{
    stopTimer();
}

void FrequencyColorSpectrogram::updateScaledValues()
{
    scaledMargin = scaled(10);
    scaledAxisLabelHeight = scaled(20);
    scaledFontSize = ResponsiveUIHelper::scaleFontSize(12.0f, scaleFactor);
}

void FrequencyColorSpectrogram::paint(Graphics& g)
{
    g.fillAll(Colours::black);
    
    drawBackground(g);
    drawFrequencyColumns(g);
    drawAxis(g);
    drawLegend(g);
}

void FrequencyColorSpectrogram::resized()
{
    auto bounds = getLocalBounds();
    auto topBar = bounds.removeFromTop(scaled(40));
    
    phaseSpeedLabel->setBounds(topBar.removeFromLeft(scaled(100)));
    phaseSpeedCombo->setBounds(topBar.removeFromLeft(scaled(150)));
}

void FrequencyColorSpectrogram::timerCallback()
{
    // Get FFT data from project manager if available
    // For now, just repaint
    repaint();
}

void FrequencyColorSpectrogram::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == phaseSpeedCombo.get())
    {
        int selection = phaseSpeedCombo->getSelectedId();
        switch (selection)
        {
            case 1: currentPhaseSpeed = SOUND_AIR; break;
            case 2: currentPhaseSpeed = SOUND_WATER; break;
            case 3: currentPhaseSpeed = LIGHT_VACUUM; break;
            case 4: currentPhaseSpeed = LIGHT_AIR; break;
        }
        repaint();
    }
}

void FrequencyColorSpectrogram::setFFTData(const float* fftData, int size)
{
    fftSize = size;
    magnitudes.clear();
    frequencies.clear();
    
    // Convert FFT bins to frequencies and magnitudes
    for (int i = 0; i < fftSize / 2; ++i)
    {
        float freq = (i * sampleRate) / fftSize;
        float mag = fftData[i];
        
        if (freq >= minFrequency && freq <= maxFrequency)
        {
            frequencies.add(freq);
            magnitudes.add(mag);
        }
    }
    
    repaint();
}

float FrequencyColorSpectrogram::getPhaseSpeedMetersPerSec()
{
    switch (currentPhaseSpeed)
    {
        case SOUND_AIR:     return 343.0f;
        case SOUND_WATER:   return 1480.0f;
        case LIGHT_VACUUM:  return 299792458.0f;
        case LIGHT_AIR:     return 299727738.0f;
    }
    return 299727738.0f;
}

float FrequencyColorSpectrogram::frequencyToWavelength(float frequency)
{
    // For sound to light conversion, we need to raise the frequency 40 octaves
    // frequency_light = frequency_sound * 2^40
    double octaveShift = 40.0;
    double lightFrequency = frequency * std::pow(2.0, octaveShift);
    
    // wavelength = speed / frequency
    double speedOfLight = getPhaseSpeedMetersPerSec();
    double wavelengthMeters = speedOfLight / lightFrequency;
    
    // Convert to nanometers
    double wavelengthNm = wavelengthMeters * 1e9;
    
    return static_cast<float>(wavelengthNm);
}

Colour FrequencyColorSpectrogram::wavelengthToColor(float wavelengthNm)
{
    // Visible spectrum: 380nm (violet) to 780nm (red)
    float r = 0, g = 0, b = 0;
    
    if (wavelengthNm < 380.0f)
    {
        // Ultraviolet - show as deep purple/black
        r = 0.2f; g = 0.0f; b = 0.3f;
    }
    else if (wavelengthNm > 780.0f)
    {
        // Infrared - show as deep red/black
        r = 0.3f; g = 0.0f; b = 0.0f;
    }
    else if (wavelengthNm >= 380.0f && wavelengthNm < 440.0f)
    {
        // Violet
        r = (440.0f - wavelengthNm) / 60.0f;
        g = 0;
        b = 1;
    }
    else if (wavelengthNm >= 440.0f && wavelengthNm < 490.0f)
    {
        // Blue
        r = 0;
        g = (wavelengthNm - 440.0f) / 50.0f;
        b = 1;
    }
    else if (wavelengthNm >= 490.0f && wavelengthNm < 510.0f)
    {
        // Cyan
        r = 0;
        g = 1;
        b = (510.0f - wavelengthNm) / 20.0f;
    }
    else if (wavelengthNm >= 510.0f && wavelengthNm < 580.0f)
    {
        // Green
        r = (wavelengthNm - 510.0f) / 70.0f;
        g = 1;
        b = 0;
    }
    else if (wavelengthNm >= 580.0f && wavelengthNm < 645.0f)
    {
        // Yellow to Orange
        r = 1;
        g = (645.0f - wavelengthNm) / 65.0f;
        b = 0;
    }
    else if (wavelengthNm >= 645.0f && wavelengthNm <= 780.0f)
    {
        // Red
        r = 1;
        g = 0;
        b = 0;
    }
    
    // Apply intensity factor for edge wavelengths
    float intensity = 1.0f;
    if (wavelengthNm > 700.0f)
        intensity = 0.3f + 0.7f * (780.0f - wavelengthNm) / 80.0f;
    else if (wavelengthNm < 420.0f)
        intensity = 0.3f + 0.7f * (wavelengthNm - 380.0f) / 40.0f;
    
    return Colour::fromFloatRGBA(r * intensity, g * intensity, b * intensity, 1.0f);
}

Colour FrequencyColorSpectrogram::frequencyToColor(float frequency)
{
    float wavelength = frequencyToWavelength(frequency);
    return wavelengthToColor(wavelength);
}

void FrequencyColorSpectrogram::findPeakFrequencies(Array<float>& peaks, Array<float>& peakMagnitudes)
{
    peaks.clear();
    peakMagnitudes.clear();
    
    if (frequencies.size() < 3) return;
    
    // Find local maxima in the magnitude spectrum
    for (int i = 1; i < frequencies.size() - 1; ++i)
    {
        float prevMag = magnitudes[i - 1];
        float currMag = magnitudes[i];
        float nextMag = magnitudes[i + 1];
        
        // Check if current point is a local maximum and above threshold
        if (currMag > prevMag && currMag > nextMag && currMag > minDb + 10.0f)
        {
            peaks.add(frequencies[i]);
            peakMagnitudes.add(currMag);
        }
    }
    
    // Sort by magnitude and keep top 10 peaks
    if (peaks.size() > 10)
    {
        // Simple bubble sort for top 10
        for (int i = 0; i < 10; ++i)
        {
            for (int j = i + 1; j < peaks.size(); ++j)
            {
                if (peakMagnitudes[j] > peakMagnitudes[i])
                {
                    std::swap(peakMagnitudes.getReference(i), peakMagnitudes.getReference(j));
                    std::swap(peaks.getReference(i), peaks.getReference(j));
                }
            }
        }
        peaks.resize(10);
        peakMagnitudes.resize(10);
    }
}

void FrequencyColorSpectrogram::drawBackground(Graphics& g)
{
    auto bounds = getLocalBounds().reduced(scaledMargin);
    bounds.removeFromTop(scaled(50)); // Space for controls
    
    // Draw grid
    g.setColour(Colours::darkgrey.withAlpha(0.3f));
    
    // Horizontal grid lines (dB levels)
    for (int db = -70; db <= 0; db += 10)
    {
        float y = jmap<float>(db, minDb, maxDb, bounds.getBottom(), bounds.getY());
        g.drawHorizontalLine(y, bounds.getX(), bounds.getRight());
    }
    
    // Vertical grid lines (frequency markers)
    float freqMarkers[] = { 100, 200, 500, 1000, 2000, 5000, 10000 };
    for (float freq : freqMarkers)
    {
        float x = jmap(std::log10(freq), std::log10(minFrequency), 
                      std::log10(maxFrequency), 
                      (float)bounds.getX(), (float)bounds.getRight());
        g.drawVerticalLine(x, bounds.getY(), bounds.getBottom());
    }
}

void FrequencyColorSpectrogram::drawFrequencyColumns(Graphics& g)
{
    auto bounds = getLocalBounds().reduced(scaledMargin);
    bounds.removeFromTop(scaled(50)); // Space for controls
    
    Array<float> peaks, peakMagnitudes;
    findPeakFrequencies(peaks, peakMagnitudes);
    
    // Draw columns for peak frequencies
    for (int i = 0; i < peaks.size(); ++i)
    {
        float freq = peaks[i];
        float mag = peakMagnitudes[i];
        
        // Calculate position and size
        float x = jmap(std::log10(freq), std::log10(minFrequency), 
                      std::log10(maxFrequency), 
                      (float)bounds.getX(), (float)bounds.getRight());
        float height = jmap(mag, minDb, maxDb, 0.0f, (float)bounds.getHeight());
        float width = bounds.getWidth() / 50.0f; // Column width
        
        // Get color for this frequency
        Colour color = frequencyToColor(freq);
        
        // Draw column with gradient
        Rectangle<float> column(x - width/2, bounds.getBottom() - height, width, height);
        g.setGradientFill(ColourGradient(color.brighter(0.5f), column.getX(), column.getY(),
                                        color.darker(0.3f), column.getX(), column.getBottom(),
                                        false));
        g.fillRect(column);
        
        // Draw frequency label
        g.setColour(Colours::white);
        g.setFont(Font(scaledFontSize));
        String freqText = (freq >= 1000) ? String(freq/1000, 1) + "k" : String((int)freq);
        g.drawText(freqText + "Hz", column.getX() - scaled(20), column.getY() - scaled(20), 
                  scaled(40), scaled(15), Justification::centred);
        
        // Draw wavelength info
        float wavelength = frequencyToWavelength(freq);
        String waveText = String((int)wavelength) + "nm";
        g.setFont(Font(scaledFontSize * 0.8f));
        g.drawText(waveText, column.getX() - scaled(20), column.getY() - scaled(35), 
                  scaled(40), scaled(12), Justification::centred);
    }
}

void FrequencyColorSpectrogram::drawAxis(Graphics& g)
{
    auto bounds = getLocalBounds().reduced(scaledMargin);
    bounds.removeFromTop(scaled(50)); // Space for controls
    
    g.setColour(Colours::white);
    g.setFont(Font(scaledFontSize));
    
    // X-axis (Frequency)
    g.drawText("Frequency (Hz)", bounds.getX(), bounds.getBottom() + scaled(5),
              bounds.getWidth(), scaledAxisLabelHeight, Justification::centred);
    
    // Y-axis (dB)
    Graphics::ScopedSaveState save(g);
    g.addTransform(AffineTransform::rotation(-juce::MathConstants<float>::halfPi, 
                                            bounds.getX() - scaled(30), 
                                            bounds.getCentreY()));
    g.drawText("Magnitude (dB)", bounds.getX() - scaled(50), bounds.getCentreY() - scaled(40),
              scaled(80), scaledAxisLabelHeight, Justification::centred);
}

void FrequencyColorSpectrogram::drawLegend(Graphics& g)
{
    auto bounds = getLocalBounds().reduced(scaledMargin);
    auto legendBounds = bounds.removeFromRight(scaled(150));
    legendBounds = legendBounds.removeFromTop(scaled(200));
    legendBounds.translate(0, scaled(60));
    
    g.setColour(Colours::white);
    g.setFont(Font(scaledFontSize));
    g.drawText("Color Legend", legendBounds.removeFromTop(scaled(20)), Justification::centred);
    
    // Draw spectrum gradient
    auto gradientBounds = legendBounds.removeFromTop(scaled(100));
    for (int y = 0; y < gradientBounds.getHeight(); ++y)
    {
        float wavelength = jmap<float>(y, 0, gradientBounds.getHeight(), 380, 780);
        Colour color = wavelengthToColor(wavelength);
        g.setColour(color);
        g.drawHorizontalLine(gradientBounds.getY() + y, 
                           gradientBounds.getX(), gradientBounds.getRight());
    }
    
    // Labels
    g.setColour(Colours::white);
    g.setFont(Font(scaledFontSize * 0.8f));
    g.drawText("380nm (UV)", gradientBounds.getX() - scaled(10), gradientBounds.getY() - scaled(5),
              scaled(80), scaled(15), Justification::left);
    g.drawText("780nm (IR)", gradientBounds.getX() - scaled(10), gradientBounds.getBottom() - scaled(10),
              scaled(80), scaled(15), Justification::left);
}