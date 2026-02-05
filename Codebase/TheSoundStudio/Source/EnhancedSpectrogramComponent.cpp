/*
  ==============================================================================

    EnhancedSpectrogramComponent.cpp
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#include "EnhancedSpectrogramComponent.h"

//==============================================================================
EnhancedSpectrogramComponent::EnhancedSpectrogramComponent(ProjectManager* pm, std::shared_ptr<EnhancedFFTEngine> engine)
    : projectManager(pm), fftEngine(engine)
{
    // Register as listener to receive FFT updates
    if (fftEngine)
        fftEngine->addListener(this);
    
    setupControls();
    
    // Start UI update timer
    startTimerHz(30);
}

EnhancedSpectrogramComponent::~EnhancedSpectrogramComponent()
{
    stopTimer();
    
    if (fftEngine)
        fftEngine->removeListener(this);
}

//==============================================================================
// Component overrides

void EnhancedSpectrogramComponent::paint(Graphics& g)
{
    // Clear background
    g.fillAll(Colour(0xff1a1a1a));
    
    // Draw border
    g.setColour(Colour(0xff404040));
    g.drawRect(getLocalBounds(), 1);
    
    // Draw grid
    drawFrequencyGrid(g);
    drawAmplitudeGrid(g);
    
    // Draw main content based on display mode
    switch (displayMode)
    {
        case SPECTRUM:
            drawSpectrum(g);
            break;
        case OCTAVE_BANDS:
            drawOctaveBands(g);
            break;
        case COLOR_SPECTRUM:
            drawColorSpectrum(g);
            break;
        case PEAKS_OVERLAY:
            drawSpectrum(g);
            drawPeaksOverlay(g);
            break;
    }
    
    // Always draw peaks if enabled
    if (showPeaks && displayMode != PEAKS_OVERLAY)
        drawPeaksOverlay(g);
    
    // Draw labels
    drawFrequencyLabels(g);
    drawAmplitudeLabels(g);
}

void EnhancedSpectrogramComponent::resized()
{
    auto bounds = getLocalBounds();
    
    // Reserve space for controls at the bottom
    controlsArea = bounds.removeFromBottom(40);
    spectrumArea = bounds.reduced(50, 20); // Leave margins for labels
    
    layoutControls();
}

void EnhancedSpectrogramComponent::timerCallback()
{
    if (dataChanged)
    {
        repaint();
        dataChanged = false;
    }
}

//==============================================================================
// Enhanced FFT Engine callbacks

void EnhancedSpectrogramComponent::fftDataUpdated(const EnhancedFFTEngine& engine)
{
    dataChanged = true;
}

void EnhancedSpectrogramComponent::peaksUpdated(const std::vector<PeakInfo>& peaks)
{
    currentPeaks = peaks;
    dataChanged = true;
}

void EnhancedSpectrogramComponent::octaveBandsUpdated(const std::array<OctaveBand, NUM_OCTAVE_BANDS>& bands)
{
    currentOctaveBands = bands;
    dataChanged = true;
}

//==============================================================================
// Mouse interaction

void EnhancedSpectrogramComponent::mouseDown(const MouseEvent& event)
{
    if (!spectrumArea.contains(event.getPosition()))
        return;
    
    // Convert mouse position to frequency and amplitude
    float frequency = xToFrequency((float)event.x);
    float amplitude = yToAmplitude((float)event.y);
    
    // Log the clicked position for debugging
    DBG("Clicked: " << frequency << " Hz, " << amplitude << " dB");
}

void EnhancedSpectrogramComponent::mouseDrag(const MouseEvent& event)
{
    // Implement panning/zooming if needed
}

void EnhancedSpectrogramComponent::mouseWheelMove(const MouseEvent& event, const MouseWheelDetails& wheel)
{
    // Implement zoom functionality
    if (spectrumArea.contains(event.getPosition()))
    {
        float zoomFactor = 1.0f + wheel.deltaY * 0.1f;
        
        // Zoom frequency range
        float centerFreq = xToFrequency((float)event.x);
        float range = maxFrequency - minFrequency;
        float newRange = range * zoomFactor;
        
        minFrequency = centerFreq - newRange * 0.5f;
        maxFrequency = centerFreq + newRange * 0.5f;
        
        // Clamp to reasonable limits
        minFrequency = jmax(1.0f, minFrequency);
        maxFrequency = jmin(22050.0f, maxFrequency);
        
        repaint();
    }
}

//==============================================================================
// UI Controls

void EnhancedSpectrogramComponent::buttonClicked(Button* button)
{
    if (button == modeButton.get())
    {
        // Cycle through display modes
        int nextMode = (int)displayMode + 1;
        if (nextMode >= 4)
            nextMode = 0;
        
        setDisplayMode((DisplayMode)nextMode);
        
        // Update button text
        String modeNames[] = {"Spectrum", "Octaves", "Color", "Peaks"};
        modeButton->setButtonText(modeNames[nextMode]);
    }
    else if (button == colorButton.get())
    {
        // Cycle through color modes
        if (fftEngine)
        {
            auto currentMode = EnhancedFFTEngine::RAINBOW;
            
            // Simple cycle through modes
            int nextColorMode = ((int)currentMode + 1) % 4;
            fftEngine->setColorMode((EnhancedFFTEngine::ColorMode)nextColorMode);
            
            String colorNames[] = {"Rainbow", "Thermal", "Blue-Red", "Custom"};
            colorButton->setButtonText(colorNames[nextColorMode]);
        }
    }
    else if (button == peaksButton.get())
    {
        setShowPeaks(!showPeaks);
        peaksButton->setButtonText(showPeaks ? "Hide Peaks" : "Show Peaks");
    }
}

void EnhancedSpectrogramComponent::sliderValueChanged(Slider* slider)
{
    // Handle slider changes for zoom/range adjustment
    repaint();
}

//==============================================================================
// Visualization settings

void EnhancedSpectrogramComponent::setFrequencyRange(float minHz, float maxHz)
{
    minFrequency = jmax(1.0f, minHz);
    maxFrequency = jmin(22050.0f, maxHz);
    repaint();
}

void EnhancedSpectrogramComponent::setAmplitudeRange(float minDB, float maxDB)
{
    minAmplitudeDB = minDB;
    maxAmplitudeDB = maxDB;
    repaint();
}

void EnhancedSpectrogramComponent::setColorMode(EnhancedFFTEngine::ColorMode mode)
{
    if (fftEngine)
        fftEngine->setColorMode(mode);
}

//==============================================================================
// Rendering methods

void EnhancedSpectrogramComponent::drawSpectrum(Graphics& g)
{
    if (!fftEngine)
        return;
    
    const int numBins = fftEngine->getNumBins();
    const float binWidth = fftEngine->getBinWidth();
    
    // Create path for spectrum curve
    Path spectrumPath;
    bool firstPoint = true;
    
    for (int bin = 0; bin < numBins; ++bin)
    {
        float frequency = bin * binWidth;
        
        // Only draw frequencies in our visible range
        if (frequency < minFrequency || frequency > maxFrequency)
            continue;
        
        float magnitude = fftEngine->getMagnitudeAt(bin);
        float amplitudeDB = juce::Decibels::gainToDecibels(magnitude);
        
        // Clamp to display range
        amplitudeDB = jlimit(minAmplitudeDB, maxAmplitudeDB, amplitudeDB);
        
        float x = frequencyToX(frequency);
        float y = amplitudeToY(amplitudeDB);
        
        if (firstPoint)
        {
            spectrumPath.startNewSubPath(x, y);
            firstPoint = false;
        }
        else
        {
            spectrumPath.lineTo(x, y);
        }
    }
    
    // Fill under the curve
    if (!firstPoint)
    {
        spectrumPath.lineTo(spectrumArea.getRight(), spectrumArea.getBottom());
        spectrumPath.lineTo(spectrumArea.getX(), spectrumArea.getBottom());
        spectrumPath.closeSubPath();
        
        g.setColour(Colour(0xff4a90e2).withAlpha(0.3f));
        g.fillPath(spectrumPath);
        
        // Draw outline
        g.setColour(Colour(0xff4a90e2));
        g.strokePath(spectrumPath, PathStrokeType(2.0f));
    }
}

void EnhancedSpectrogramComponent::drawOctaveBands(Graphics& g)
{
    if (!fftEngine)
        return;
    
    const auto& bands = currentOctaveBands;
    const float bandWidth = (float)spectrumArea.getWidth() / NUM_OCTAVE_BANDS;
    
    for (int i = 0; i < NUM_OCTAVE_BANDS; ++i)
    {
        const auto& band = bands[i];
        
        if (band.centerFrequency < minFrequency || band.centerFrequency > maxFrequency)
            continue;
        
        float amplitudeDB = juce::Decibels::gainToDecibels(band.amplitude);
        amplitudeDB = jlimit(minAmplitudeDB, maxAmplitudeDB, amplitudeDB);
        
        float x = i * bandWidth;
        float y = amplitudeToY(amplitudeDB);
        float height = spectrumArea.getBottom() - y;
        
        Rectangle<float> barRect(spectrumArea.getX() + x, y, bandWidth * 0.8f, height);
        
        // Color based on amplitude
        Colour barColor = getAmplitudeColor(amplitudeDB);
        g.setColour(barColor);
        g.fillRect(barRect);
        
        // Draw bar outline
        g.setColour(Colours::white.withAlpha(0.5f));
        g.drawRect(barRect, 1.0f);
        
        // Draw frequency label
        g.setColour(Colours::white);
        g.setFont(10.0f);
        String freqText = String((int)band.centerFrequency) + "Hz";
        g.drawText(freqText, barRect, Justification::centredBottom);
    }
}

void EnhancedSpectrogramComponent::drawColorSpectrum(Graphics& g)
{
    if (!fftEngine)
        return;
    
    const int numBins = fftEngine->getNumBins();
    const float binWidth = fftEngine->getBinWidth();
    const float pixelWidth = (float)spectrumArea.getWidth() / numBins;
    
    for (int bin = 0; bin < numBins; ++bin)
    {
        float frequency = bin * binWidth;
        
        if (frequency < minFrequency || frequency > maxFrequency)
            continue;
        
        float magnitude = fftEngine->getMagnitudeAt(bin);
        float amplitudeDB = juce::Decibels::gainToDecibels(magnitude);
        amplitudeDB = jlimit(minAmplitudeDB, maxAmplitudeDB, amplitudeDB);
        
        float x = frequencyToX(frequency);
        float y = amplitudeToY(amplitudeDB);
        float height = spectrumArea.getBottom() - y;
        
        // Get color for this frequency
        Colour binColor = fftEngine->getColorForBin(bin);
        
        // Modulate brightness based on amplitude
        float normalizedAmp = (amplitudeDB - minAmplitudeDB) / (maxAmplitudeDB - minAmplitudeDB);
        binColor = binColor.withBrightness(normalizedAmp);
        
        g.setColour(binColor);
        g.fillRect(x, y, pixelWidth, height);
    }
}

void EnhancedSpectrogramComponent::drawPeaksOverlay(Graphics& g)
{
    if (currentPeaks.empty())
        return;
    
    int peaksDrawn = 0;
    
    for (const auto& peak : currentPeaks)
    {
        if (peaksDrawn >= maxDisplayedPeaks)
            break;
        
        if (peak.frequency < minFrequency || peak.frequency > maxFrequency)
            continue;
        
        float x = frequencyToX(peak.frequency);
        float y = amplitudeToY(peak.amplitude);
        
        // Draw peak marker
        g.setColour(Colours::yellow);
        g.fillEllipse(x - 3, y - 3, 6, 6);
        
        g.setColour(Colours::white);
        g.drawEllipse(x - 3, y - 3, 6, 6, 1.0f);
        
        // Draw frequency label if enabled
        if (showPeakLabels)
        {
            g.setColour(Colours::white);
            g.setFont(9.0f);
            
            String peakText = String((int)peak.frequency) + "Hz";
            g.drawText(peakText, x - 20, y - 20, 40, 15, Justification::centred);
        }
        
        ++peaksDrawn;
    }
}

void EnhancedSpectrogramComponent::drawFrequencyGrid(Graphics& g)
{
    g.setColour(Colour(0xff303030));
    
    // Draw vertical grid lines at musical intervals
    std::vector<float> frequencies = {20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000};
    
    for (float freq : frequencies)
    {
        if (freq >= minFrequency && freq <= maxFrequency)
        {
            float x = frequencyToX(freq);
            g.drawVerticalLine((int)x, (float)spectrumArea.getY(), (float)spectrumArea.getBottom());
        }
    }
}

void EnhancedSpectrogramComponent::drawAmplitudeGrid(Graphics& g)
{
    g.setColour(Colour(0xff303030));
    
    // Draw horizontal grid lines at dB intervals
    for (float db = minAmplitudeDB; db <= maxAmplitudeDB; db += 10.0f)
    {
        float y = amplitudeToY(db);
        g.drawHorizontalLine((int)y, (float)spectrumArea.getX(), (float)spectrumArea.getRight());
    }
}

void EnhancedSpectrogramComponent::drawFrequencyLabels(Graphics& g)
{
    g.setColour(Colours::lightgrey);
    g.setFont(10.0f);
    
    std::vector<float> frequencies = {20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000};
    
    for (float freq : frequencies)
    {
        if (freq >= minFrequency && freq <= maxFrequency)
        {
            float x = frequencyToX(freq);
            String label = (freq >= 1000) ? String((int)(freq / 1000)) + "k" : String((int)freq);
            g.drawText(label, x - 15, getHeight() - 15, 30, 12, Justification::centred);
        }
    }
}

void EnhancedSpectrogramComponent::drawAmplitudeLabels(Graphics& g)
{
    g.setColour(Colours::lightgrey);
    g.setFont(10.0f);
    
    for (float db = minAmplitudeDB; db <= maxAmplitudeDB; db += 20.0f)
    {
        float y = amplitudeToY(db);
        String label = String((int)db) + "dB";
        g.drawText(label, 5, y - 6, 40, 12, Justification::centredLeft);
    }
}

//==============================================================================
// Coordinate conversion

float EnhancedSpectrogramComponent::frequencyToX(float frequency) const
{
    if (maxFrequency <= minFrequency)
        return (float)spectrumArea.getX();
    
    float logMin = std::log10(minFrequency);
    float logMax = std::log10(maxFrequency);
    float logFreq = std::log10(frequency);
    
    float ratio = (logFreq - logMin) / (logMax - logMin);
    return spectrumArea.getX() + ratio * spectrumArea.getWidth();
}

float EnhancedSpectrogramComponent::amplitudeToY(float amplitudeDB) const
{
    float ratio = (amplitudeDB - minAmplitudeDB) / (maxAmplitudeDB - minAmplitudeDB);
    ratio = jlimit(0.0f, 1.0f, ratio);
    
    return spectrumArea.getBottom() - ratio * spectrumArea.getHeight();
}

float EnhancedSpectrogramComponent::xToFrequency(float x) const
{
    float ratio = (x - spectrumArea.getX()) / spectrumArea.getWidth();
    ratio = jlimit(0.0f, 1.0f, ratio);
    
    float logMin = std::log10(minFrequency);
    float logMax = std::log10(maxFrequency);
    float logFreq = logMin + ratio * (logMax - logMin);
    
    return std::pow(10.0f, logFreq);
}

float EnhancedSpectrogramComponent::yToAmplitude(float y) const
{
    float ratio = (spectrumArea.getBottom() - y) / spectrumArea.getHeight();
    ratio = jlimit(0.0f, 1.0f, ratio);
    
    return minAmplitudeDB + ratio * (maxAmplitudeDB - minAmplitudeDB);
}

//==============================================================================
// Color utilities

Colour EnhancedSpectrogramComponent::getAmplitudeColor(float amplitudeDB) const
{
    float normalizedAmp = (amplitudeDB - minAmplitudeDB) / (maxAmplitudeDB - minAmplitudeDB);
    normalizedAmp = jlimit(0.0f, 1.0f, normalizedAmp);
    
    // Blue to red gradient
    return Colour::fromHSV(240.0f * (1.0f - normalizedAmp) / 360.0f, 0.8f, 0.8f, 1.0f);
}

Colour EnhancedSpectrogramComponent::getFrequencyColor(float frequency) const
{
    if (fftEngine)
        return fftEngine->getColorForFrequency(frequency);
    
    return Colours::white;
}

//==============================================================================
// UI Setup

void EnhancedSpectrogramComponent::setupControls()
{
    // Mode button
    modeButton = std::make_unique<TextButton>("Spectrum");
    modeButton->addListener(this);
    addAndMakeVisible(modeButton.get());
    
    // Color button
    colorButton = std::make_unique<TextButton>("Rainbow");
    colorButton->addListener(this);
    addAndMakeVisible(colorButton.get());
    
    // Peaks button
    peaksButton = std::make_unique<TextButton>("Hide Peaks");
    peaksButton->addListener(this);
    addAndMakeVisible(peaksButton.get());
}

void EnhancedSpectrogramComponent::layoutControls()
{
    auto bounds = controlsArea;
    const int buttonWidth = 80;
    const int buttonHeight = 25;
    const int spacing = 10;
    
    modeButton->setBounds(bounds.removeFromLeft(buttonWidth).withHeight(buttonHeight));
    bounds.removeFromLeft(spacing);
    
    colorButton->setBounds(bounds.removeFromLeft(buttonWidth).withHeight(buttonHeight));
    bounds.removeFromLeft(spacing);
    
    peaksButton->setBounds(bounds.removeFromLeft(buttonWidth).withHeight(buttonHeight));
}