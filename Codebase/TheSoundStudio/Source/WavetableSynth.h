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
    
    // Per-voice state structure
    struct VoiceState
    {
        float phase = 0.0f;
        float envelope = 1.0f;
        float lowpassState1 = 0.0f;
        float lowpassState2 = 0.0f;
        float highpassState1 = 0.0f;
        float highpassState2 = 0.0f;
        float previousSample = 0.0f;
        bool isActive = false;
        int sampleIndex = 0;
        
        void reset()
        {
            phase = 0.0f;
            envelope = 1.0f;
            lowpassState1 = lowpassState2 = 0.0f;
            highpassState1 = highpassState2 = 0.0f;
            previousSample = 0.0f;
            sampleIndex = 0;
            isActive = true;
        }
    };
    
    // Voice management
    static constexpr int maxVoices = 32;
    VoiceState voices[maxVoices];
    int currentVoiceIndex = 0;
    
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
    
    // Voice management
    VoiceState* getNextAvailableVoice();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WavetableEngine)
};