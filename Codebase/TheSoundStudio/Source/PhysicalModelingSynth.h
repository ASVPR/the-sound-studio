/*
  ==============================================================================

    PhysicalModelingSynth.h
    Created: 14 Aug 2025
    Author:  The Sound Studio Team

    Physical modeling synthesis algorithms for acoustic instrument simulation.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class PhysicalModelingEngine
{
public:
    PhysicalModelingEngine();
    ~PhysicalModelingEngine();
    
    void initialize(double sampleRate, double tuningReference);
    void prepareToPlay(int blockSize);
    void releaseResources();
    
    // Instrument generation methods
    void generatePiano(AudioBuffer<float>& buffer, float frequency, float velocity);
    void generateStrings(AudioBuffer<float>& buffer, float frequency, float velocity);
    
    // Parameter control
    void setParameters(const float* parameters);
    void updateTuning(double tuningReference);
    
private:
    double sampleRate;
    double tuningReference;
    int blockSize;
    
    // Piano modeling parameters
    float stringTension;
    float hammerHardness;
    float damping;
    float resonance;
    
    // String modeling parameters
    float stringLength;
    float stringDensity;
    float bowPressure;
    
    // Per-voice state structure for polyphonic synthesis
    struct VoiceState
    {
        float phase = 0.0f;
        float amplitude = 1.0f;
        float resonancePhase = 0.0f;
        float sympatheticPhase = 0.0f;
        float bowNoise = 0.0f;
        float previousSample = 0.0f;
        float filterState[4] = {0};
        bool isActive = false;
        
        void reset()
        {
            phase = 0.0f;
            amplitude = 1.0f;
            resonancePhase = 0.0f;
            sympatheticPhase = 0.0f;
            bowNoise = 0.0f;
            previousSample = 0.0f;
            for (int i = 0; i < 4; ++i)
                filterState[i] = 0.0f;
            isActive = true;
        }
    };
    
    // Voice management
    static constexpr int maxVoices = 32;
    VoiceState voices[maxVoices];
    int currentVoiceIndex = 0;
    
    // Simple physical modeling implementation
    void generatePianoNote(float* output, int numSamples, float frequency, float velocity);
    void generateStringNote(float* output, int numSamples, float frequency, float velocity);
    
    // Helper functions
    float calculateStringLength(float frequency);
    float applyHammerModel(float input, float hardness);
    VoiceState* getNextAvailableVoice();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PhysicalModelingEngine)
};