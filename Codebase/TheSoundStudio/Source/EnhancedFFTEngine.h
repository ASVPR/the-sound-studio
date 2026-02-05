/*
  ==============================================================================

    EnhancedFFTEngine.h
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <memory>
#include <atomic>
#include <array>

//==============================================================================
// Enhanced FFT Configuration
#define ENHANCED_FFT_ORDER 11        // 2048 samples for high resolution
#define ENHANCED_FFT_SIZE (1 << ENHANCED_FFT_ORDER)
#define MAX_NUM_BINS 1024
#define NUM_OCTAVE_BANDS 10
#define NUM_SPECTRUM_COLORS 256

//==============================================================================
// Peak Detection Structure
struct PeakInfo
{
    float frequency;
    float amplitude;
    float phase;
    int binIndex;
    
    PeakInfo() : frequency(0), amplitude(0), phase(0), binIndex(-1) {}
    PeakInfo(float freq, float amp, float ph, int bin) 
        : frequency(freq), amplitude(amp), phase(ph), binIndex(bin) {}
};

//==============================================================================
// Octave Band Data
struct OctaveBand  
{
    float centerFrequency;
    float amplitude;
    float peakFrequency;
    float bandwidth;
    int startBin;
    int endBin;
    
    OctaveBand() : centerFrequency(0), amplitude(0), peakFrequency(0), 
                   bandwidth(0), startBin(0), endBin(0) {}
};

//==============================================================================
// Color Spectrum Data
struct SpectrumColor
{
    float hue;          // 0-360 degrees
    float saturation;   // 0-1
    float brightness;   // 0-1
    Colour juceColor;
    
    SpectrumColor() : hue(0), saturation(0), brightness(0) {}
    
    void updateJuceColor()
    {
        juceColor = Colour::fromHSV(hue / 360.0f, saturation, brightness, 1.0f);
    }
};

//==============================================================================
/**
    Enhanced FFT Engine - Single shared instance for all modules
    
    This class implements the 4 key FFT improvements:
    1. Shared processing to reduce CPU load
    2. High-resolution bins with intelligent octave grouping
    3. Real-time peak detection and tracking
    4. Color spectrum mapping for visualization
*/
class EnhancedFFTEngine : public Timer
{
public:
    EnhancedFFTEngine(double sampleRate = 44100.0);
    ~EnhancedFFTEngine();
    
    //==============================================================================
    // Core FFT Processing (Improvement #1: Single shared object)
    void initialize(double sampleRate, int bufferSize = 512);
    void processBlock(const AudioBuffer<float>& buffer, int channelIndex = 0);
    void shutdown();
    
    //==============================================================================
    // High-Resolution Bins (Improvement #2: Better frequency resolution)
    void setFFTSize(int newOrder); // 10=1024, 11=2048, 12=4096, 13=8192
    int getFFTSize() const { return fftSize; }
    float getBinWidth() const { return sampleRate / fftSize; }
    int getNumBins() const { return fftSize / 2; }
    
    // Octave-based bin grouping for musical analysis
    void calculateOctaveBands();
    const std::array<OctaveBand, NUM_OCTAVE_BANDS>& getOctaveBands() const { return octaveBands; }
    
    //==============================================================================
    // Peak Detection (Improvement #3: Peak amplitude readout)
    void updatePeakDetection();
    const std::vector<PeakInfo>& getPeaks() const { return detectedPeaks; }
    PeakInfo getHighestPeak() const;
    PeakInfo getPeakInFrequencyRange(float minHz, float maxHz) const;
    
    // Peak detection settings
    void setPeakThreshold(float thresholdDB) { peakThreshold = thresholdDB; }
    void setMaxPeaks(int maxPeaks) { maxDetectedPeaks = maxPeaks; }
    
    //==============================================================================
    // Color Spectrum (Improvement #4: Color spectrum visualization)
    void calculateColorSpectrum();
    const std::array<SpectrumColor, NUM_SPECTRUM_COLORS>& getColorSpectrum() const { return colorSpectrum; }
    Colour getColorForFrequency(float frequency) const;
    Colour getColorForBin(int binIndex) const;
    
    // Color mapping modes
    enum ColorMode { RAINBOW, THERMAL, BLUE_TO_RED, CUSTOM };
    void setColorMode(ColorMode mode) { colorMode = mode; recalculateColors = true; }
    
    //==============================================================================
    // Data Access - Thread-safe
    const float* getMagnitudeData() const { return magnitudeBuffer.data(); }
    const float* getPhaseData() const { return phaseBuffer.data(); }
    float getMagnitudeAt(int binIndex) const;
    float getPhaseAt(int binIndex) const;
    float getFrequencyForBin(int binIndex) const { return binIndex * getBinWidth(); }
    int getBinForFrequency(float frequency) const;
    
    //==============================================================================
    // Windowing and Processing Options
    enum WindowType { HANNING, HAMMING, BLACKMAN, RECTANGULAR };
    void setWindowType(WindowType type) { windowType = type; recalculateWindow(); }
    
    void setOverlapFactor(float factor) { overlapFactor = jlimit(0.0f, 0.9f, factor); }
    void setUpdateRate(int hz) { updateRateHz = hz; }
    
    //==============================================================================
    // Registration system for modules to share this FFT
    class Listener
    {
    public:
        virtual ~Listener() = default;
        virtual void fftDataUpdated(const EnhancedFFTEngine& engine) = 0;
        virtual void peaksUpdated(const std::vector<PeakInfo>& peaks) = 0;
        virtual void octaveBandsUpdated(const std::array<OctaveBand, NUM_OCTAVE_BANDS>& bands) = 0;
    };
    
    void addListener(Listener* listener);
    void removeListener(Listener* listener);
    
    //==============================================================================
    // Timer callback for regular updates
    void timerCallback() override;
    
private:
    //==============================================================================
    // Core FFT Data
    std::unique_ptr<dsp::FFT> fft;
    int fftOrder = ENHANCED_FFT_ORDER;
    int fftSize = ENHANCED_FFT_SIZE;
    double sampleRate = 44100.0;
    
    //==============================================================================
    // Processing Buffers - Thread-safe
    std::atomic<bool> dataReady{false};
    std::array<float, ENHANCED_FFT_SIZE * 2> fftBuffer; // Complex data
    std::array<float, MAX_NUM_BINS> magnitudeBuffer;
    std::array<float, MAX_NUM_BINS> phaseBuffer;
    
    // Input buffering for overlap processing
    std::array<float, ENHANCED_FFT_SIZE> inputBuffer;
    std::array<float, ENHANCED_FFT_SIZE> windowBuffer;
    int bufferWritePos = 0;
    float overlapFactor = 0.5f;
    
    //==============================================================================
    // Peak Detection Data
    std::vector<PeakInfo> detectedPeaks;
    float peakThreshold = -60.0f; // dB
    int maxDetectedPeaks = 20;
    
    //==============================================================================
    // Octave Band Data
    std::array<OctaveBand, NUM_OCTAVE_BANDS> octaveBands;
    bool recalculateOctaves = true;
    
    //==============================================================================
    // Color Spectrum Data
    std::array<SpectrumColor, NUM_SPECTRUM_COLORS> colorSpectrum;
    ColorMode colorMode = RAINBOW;
    bool recalculateColors = true;
    
    //==============================================================================
    // Windowing
    WindowType windowType = HANNING;
    void recalculateWindow();
    
    //==============================================================================
    // Update Management
    int updateRateHz = 30;
    std::atomic<bool> needsUpdate{false};
    
    //==============================================================================
    // Listener Management
    std::vector<Listener*> listeners;
    mutable SpinLock listenersLock;
    
    void notifyListeners();
    
    //==============================================================================
    // Internal Processing
    void processFFTData();
    void applyWindow();
    float calculateMagnitude(int binIndex) const;
    float calculatePhase(int binIndex) const;
    
    //==============================================================================
    // Color Calculation
    void calculateRainbowSpectrum();
    void calculateThermalSpectrum();  
    void calculateBlueToRedSpectrum();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EnhancedFFTEngine)
};