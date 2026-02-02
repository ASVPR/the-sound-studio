/*
  ==============================================================================

    EnhancedFFTEngine.cpp

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#include "EnhancedFFTEngine.h"

//==============================================================================
EnhancedFFTEngine::EnhancedFFTEngine(double sampleRate)
    : sampleRate(sampleRate)
{
    initialize(sampleRate);
}

EnhancedFFTEngine::~EnhancedFFTEngine()
{
    shutdown();
}

//==============================================================================
// Core FFT Processing (Improvement #1: Single shared object)

void EnhancedFFTEngine::initialize(double newSampleRate, int bufferSize)
{
    sampleRate = newSampleRate;
    
    // Create FFT object
    fft = std::make_unique<dsp::FFT>(fftOrder);
    fftSize = 1 << fftOrder;
    
    // Clear all buffers
    std::fill(fftBuffer.begin(), fftBuffer.end(), 0.0f);
    std::fill(magnitudeBuffer.begin(), magnitudeBuffer.end(), 0.0f);
    std::fill(phaseBuffer.begin(), phaseBuffer.end(), 0.0f);
    std::fill(inputBuffer.begin(), inputBuffer.end(), 0.0f);
    
    bufferWritePos = 0;
    
    // Calculate window function
    recalculateWindow();
    
    // Setup octave bands
    calculateOctaveBands();
    
    // Setup color spectrum
    calculateColorSpectrum();
    
    // Start timer for updates
    startTimer(1000 / updateRateHz);
}

void EnhancedFFTEngine::shutdown()
{
    stopTimer();
    fft.reset();
}

void EnhancedFFTEngine::processBlock(const AudioBuffer<float>& buffer, int channelIndex)
{
    if (!fft || channelIndex >= buffer.getNumChannels())
        return;
    
    const float* channelData = buffer.getReadPointer(channelIndex);
    int numSamples = buffer.getNumSamples();
    
    // Process samples into circular buffer with overlap
    for (int i = 0; i < numSamples; ++i)
    {
        inputBuffer[bufferWritePos] = channelData[i];
        bufferWritePos = (bufferWritePos + 1) % fftSize;
        
        // Trigger FFT when buffer is full
        if (bufferWritePos == 0)
        {
            processFFTData();
            needsUpdate = true;
        }
    }
}

//==============================================================================
// High-Resolution Bins (Improvement #2: Better frequency resolution)

void EnhancedFFTEngine::setFFTSize(int newOrder)
{
    if (newOrder < 10 || newOrder > 13) // Limit to reasonable sizes
        return;
        
    stopTimer();
    
    fftOrder = newOrder;
    fftSize = 1 << fftOrder;
    
    // Recreate FFT
    fft = std::make_unique<dsp::FFT>(fftOrder);
    
    // Recalculate dependent data
    recalculateWindow();
    calculateOctaveBands();
    recalculateColors = true;
    
    startTimer(1000 / updateRateHz);
}

void EnhancedFFTEngine::calculateOctaveBands()
{
    // Musical octave bands from 20Hz to 20kHz
    const float baseFreq = 20.0f;
    const float maxFreq = jmin(20000.0f, (float)sampleRate / 2.0f);
    
    for (int i = 0; i < NUM_OCTAVE_BANDS; ++i)
    {
        float centerFreq = baseFreq * std::pow(2.0f, i);
        if (centerFreq > maxFreq)
        {
            centerFreq = maxFreq;
        }
        
        octaveBands[i].centerFrequency = centerFreq;
        octaveBands[i].bandwidth = centerFreq * 0.707f; // ~1/sqrt(2) for octave band
        
        // Calculate bin range for this octave
        float startFreq = centerFreq / std::sqrt(2.0f);
        float endFreq = centerFreq * std::sqrt(2.0f);
        
        octaveBands[i].startBin = getBinForFrequency(startFreq);
        octaveBands[i].endBin = getBinForFrequency(endFreq);
        
        // Ensure we don't exceed array bounds
        octaveBands[i].startBin = jlimit(0, getNumBins() - 1, octaveBands[i].startBin);
        octaveBands[i].endBin = jlimit(octaveBands[i].startBin, getNumBins() - 1, octaveBands[i].endBin);
    }
    
    recalculateOctaves = false;
}

//==============================================================================
// Peak Detection (Improvement #3: Peak amplitude readout)

void EnhancedFFTEngine::updatePeakDetection()
{
    detectedPeaks.clear();
    
    const int numBins = getNumBins();
    const float threshold = juce::Decibels::decibelsToGain(peakThreshold);
    
    // Simple peak detection - look for local maxima above threshold
    for (int bin = 1; bin < numBins - 1; ++bin)
    {
        float magnitude = getMagnitudeAt(bin);
        
        if (magnitude > threshold &&
            magnitude > getMagnitudeAt(bin - 1) &&
            magnitude > getMagnitudeAt(bin + 1))
        {
            float frequency = getFrequencyForBin(bin);
            float phase = getPhaseAt(bin);
            float amplitudeDB = juce::Decibels::gainToDecibels(magnitude);
            
            detectedPeaks.emplace_back(frequency, amplitudeDB, phase, bin);
            
            if ((int)detectedPeaks.size() >= maxDetectedPeaks)
                break;
        }
    }
    
    // Sort peaks by amplitude (highest first)
    std::sort(detectedPeaks.begin(), detectedPeaks.end(),
              [](const PeakInfo& a, const PeakInfo& b) {
                  return a.amplitude > b.amplitude;
              });
}

PeakInfo EnhancedFFTEngine::getHighestPeak() const
{
    if (detectedPeaks.empty())
        return PeakInfo();
    
    return detectedPeaks[0];
}

PeakInfo EnhancedFFTEngine::getPeakInFrequencyRange(float minHz, float maxHz) const
{
    for (const auto& peak : detectedPeaks)
    {
        if (peak.frequency >= minHz && peak.frequency <= maxHz)
            return peak;
    }
    
    return PeakInfo();
}

//==============================================================================
// Color Spectrum (Improvement #4: Color spectrum visualization)

void EnhancedFFTEngine::calculateColorSpectrum()
{
    switch (colorMode)
    {
        case RAINBOW:
            calculateRainbowSpectrum();
            break;
        case THERMAL:
            calculateThermalSpectrum();
            break;
        case BLUE_TO_RED:
            calculateBlueToRedSpectrum();
            break;
        case CUSTOM:
            // User-defined color mapping would go here
            calculateRainbowSpectrum();
            break;
    }
    
    recalculateColors = false;
}

void EnhancedFFTEngine::calculateRainbowSpectrum()
{
    for (int i = 0; i < NUM_SPECTRUM_COLORS; ++i)
    {
        float ratio = (float)i / (float)(NUM_SPECTRUM_COLORS - 1);
        
        colorSpectrum[i].hue = ratio * 360.0f;        // Full hue range
        colorSpectrum[i].saturation = 0.9f;           // High saturation
        colorSpectrum[i].brightness = 0.8f;           // Bright but not blinding
        colorSpectrum[i].updateJuceColor();
    }
}

void EnhancedFFTEngine::calculateThermalSpectrum()
{
    for (int i = 0; i < NUM_SPECTRUM_COLORS; ++i)
    {
        float ratio = (float)i / (float)(NUM_SPECTRUM_COLORS - 1);
        
        if (ratio < 0.5f)
        {
            // Black to red
            colorSpectrum[i].hue = 0.0f;
            colorSpectrum[i].saturation = 1.0f;
            colorSpectrum[i].brightness = ratio * 2.0f;
        }
        else
        {
            // Red to yellow to white
            float adjusted = (ratio - 0.5f) * 2.0f;
            colorSpectrum[i].hue = adjusted * 60.0f; // 0-60 degrees (red to yellow)
            colorSpectrum[i].saturation = 1.0f - adjusted * 0.5f;
            colorSpectrum[i].brightness = 1.0f;
        }
        
        colorSpectrum[i].updateJuceColor();
    }
}

void EnhancedFFTEngine::calculateBlueToRedSpectrum()
{
    for (int i = 0; i < NUM_SPECTRUM_COLORS; ++i)
    {
        float ratio = (float)i / (float)(NUM_SPECTRUM_COLORS - 1);
        
        colorSpectrum[i].hue = 240.0f - (ratio * 240.0f); // 240° (blue) to 0° (red)
        colorSpectrum[i].saturation = 0.8f;
        colorSpectrum[i].brightness = 0.9f;
        colorSpectrum[i].updateJuceColor();
    }
}

Colour EnhancedFFTEngine::getColorForFrequency(float frequency) const
{
    int binIndex = getBinForFrequency(frequency);
    return getColorForBin(binIndex);
}

Colour EnhancedFFTEngine::getColorForBin(int binIndex) const
{
    if (binIndex < 0 || binIndex >= getNumBins())
        return Colours::black;
    
    // Map bin to color spectrum
    int colorIndex = jlimit(0, NUM_SPECTRUM_COLORS - 1,
                           (int)((float)binIndex / (float)getNumBins() * NUM_SPECTRUM_COLORS));
    
    return colorSpectrum[colorIndex].juceColor;
}

//==============================================================================
// Data Access

float EnhancedFFTEngine::getMagnitudeAt(int binIndex) const
{
    if (binIndex < 0 || binIndex >= (int)magnitudeBuffer.size())
        return 0.0f;
    
    return magnitudeBuffer[binIndex];
}

float EnhancedFFTEngine::getPhaseAt(int binIndex) const
{
    if (binIndex < 0 || binIndex >= (int)phaseBuffer.size())
        return 0.0f;
    
    return phaseBuffer[binIndex];
}

int EnhancedFFTEngine::getBinForFrequency(float frequency) const
{
    if (frequency <= 0 || getBinWidth() <= 0)
        return 0;
    
    return jlimit(0, getNumBins() - 1, (int)(frequency / getBinWidth()));
}

//==============================================================================
// Windowing

void EnhancedFFTEngine::recalculateWindow()
{
    for (int i = 0; i < fftSize; ++i)
    {
        float sample = (float)i / (float)fftSize;
        
        switch (windowType)
        {
            case HANNING:
                windowBuffer[i] = 0.5f * (1.0f - std::cos(2.0f * MathConstants<float>::pi * sample));
                break;
            case HAMMING:
                windowBuffer[i] = 0.54f - 0.46f * std::cos(2.0f * MathConstants<float>::pi * sample);
                break;
            case BLACKMAN:
                windowBuffer[i] = 0.42f - 0.5f * std::cos(2.0f * MathConstants<float>::pi * sample)
                                 + 0.08f * std::cos(4.0f * MathConstants<float>::pi * sample);
                break;
            case RECTANGULAR:
            default:
                windowBuffer[i] = 1.0f;
                break;
        }
    }
}

//==============================================================================
// Processing

void EnhancedFFTEngine::processFFTData()
{
    if (!fft)
        return;
    
    // Copy input buffer to FFT buffer and apply window
    for (int i = 0; i < fftSize; ++i)
    {
        int readIndex = (bufferWritePos + i) % fftSize;
        fftBuffer[i * 2] = inputBuffer[readIndex] * windowBuffer[i];     // Real part
        fftBuffer[i * 2 + 1] = 0.0f;                                    // Imaginary part
    }
    
    // Perform FFT
    fft->performFrequencyOnlyForwardTransform(fftBuffer.data());
    
    // Calculate magnitude and phase for each bin
    const int numBins = getNumBins();
    for (int bin = 0; bin < numBins; ++bin)
    {
        magnitudeBuffer[bin] = calculateMagnitude(bin);
        phaseBuffer[bin] = calculatePhase(bin);
    }
    
    // Update octave bands
    for (auto& band : octaveBands)
    {
        band.amplitude = 0.0f;
        band.peakFrequency = band.centerFrequency;
        float maxAmp = 0.0f;
        
        for (int bin = band.startBin; bin <= band.endBin; ++bin)
        {
            float amp = getMagnitudeAt(bin);
            band.amplitude += amp;
            
            if (amp > maxAmp)
            {
                maxAmp = amp;
                band.peakFrequency = getFrequencyForBin(bin);
            }
        }
        
        // Average amplitude across the band
        int bandWidth = band.endBin - band.startBin + 1;
        if (bandWidth > 0)
            band.amplitude /= bandWidth;
    }
    
    dataReady = true;
}

float EnhancedFFTEngine::calculateMagnitude(int binIndex) const
{
    if (binIndex < 0 || binIndex >= getNumBins())
        return 0.0f;
    
    float real = fftBuffer[binIndex * 2];
    float imag = fftBuffer[binIndex * 2 + 1];
    
    return std::sqrt(real * real + imag * imag);
}

float EnhancedFFTEngine::calculatePhase(int binIndex) const
{
    if (binIndex < 0 || binIndex >= getNumBins())
        return 0.0f;
    
    float real = fftBuffer[binIndex * 2];
    float imag = fftBuffer[binIndex * 2 + 1];
    
    return std::atan2(imag, real);
}

//==============================================================================
// Listener Management

void EnhancedFFTEngine::addListener(Listener* listener)
{
    const SpinLock::ScopedLockType lock(listenersLock);
    listeners.push_back(listener);
}

void EnhancedFFTEngine::removeListener(Listener* listener)
{
    const SpinLock::ScopedLockType lock(listenersLock);
    listeners.erase(std::remove(listeners.begin(), listeners.end(), listener), listeners.end());
}

void EnhancedFFTEngine::notifyListeners()
{
    const SpinLock::ScopedLockType lock(listenersLock);
    
    for (auto* listener : listeners)
    {
        listener->fftDataUpdated(*this);
        listener->peaksUpdated(detectedPeaks);
        listener->octaveBandsUpdated(octaveBands);
    }
}

//==============================================================================
// Timer Callback

void EnhancedFFTEngine::timerCallback()
{
    if (needsUpdate.exchange(false))
    {
        updatePeakDetection();
        
        if (recalculateOctaves)
            calculateOctaveBands();
            
        if (recalculateColors)
            calculateColorSpectrum();
        
        notifyListeners();
    }
}