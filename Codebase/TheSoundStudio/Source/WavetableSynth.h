/*
  ==============================================================================

    WavetableSynth.h
    Created: 31 Aug 2025
    Author:  The Sound Studio Team

    Wavetable synthesis engine for electronic and organ sounds.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class WavetableEngine
{
public:
    WavetableEngine();
    ~WavetableEngine();
    
    void initialize(double sampleRate, double tuningReference);
    void prepareToPlay(int blockSize);
    void releaseResources();
    
    // Instrument generation methods
    void generateSynth(AudioBuffer<float>& buffer, float frequency, float velocity);
    void generateOrgan(AudioBuffer<float>& buffer, float frequency, float velocity);
    
    // Parameter control
    void setParameters(const float* parameters);
    void updateTuning(double tuningReference);
    
private:
    double sampleRate;
    double tuningReference;
    int blockSize;
    
    // Wavetable parameters
    float wavetablePosition;
    float filterCutoff;
    float filterResonance;
    float amplitude;
    
    // Filter state
    float lowpassState1, lowpassState2;
    float highpassState1, highpassState2;
    
    // Wavetables
    Array<float> sineWavetable;
    Array<float> sawWavetable;
    Array<float> squareWavetable;
    Array<float> organWavetable;
    
    // Wavetable methods
    void initializeWavetables();
    float getWavetableSample(const Array<float>& wavetable, float phase);
    float interpolateWavetables(float phase, float position);
    
    // Synthesis methods
    void generateWavetableNote(float* output, int numSamples, const Array<float>& wavetable, 
                              float frequency, float velocity);
    void generateOrganNote(float* output, int numSamples, float frequency, float velocity);
    
    // Filter methods
    float lowpassFilter(float input, float cutoff, float& state1, float& state2);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WavetableEngine)
};