/*
  ==============================================================================

    KarplusStrongSynth.h
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class KarplusStrongEngine
{
public:
    KarplusStrongEngine();
    ~KarplusStrongEngine();
    
    void initialize(double sampleRate, double tuningReference);
    void prepareToPlay(int blockSize);
    void releaseResources();
    
    // Instrument generation methods
    void generateGuitar(AudioBuffer<float>& buffer, float frequency, float velocity);
    void generateHarp(AudioBuffer<float>& buffer, float frequency, float velocity);
    
    // Parameter control
    void setParameters(const float* parameters);
    void updateTuning(double tuningReference);
    
private:
    double sampleRate;
    double tuningReference;
    int blockSize;
    
    // Karplus-Strong parameters
    float bodyResonance;
    float stringDamping;
    float pluckPosition;
    float feedback;
    
    // Delay line for Karplus-Strong algorithm
    struct DelayLine
    {
        Array<float> buffer;
        int writeIndex;
        int readIndex;
        int length;
        float lastOutput;
        
        DelayLine() : writeIndex(0), readIndex(0), length(0), lastOutput(0.0f) {}
        
        void resize(int newLength)
        {
            length = newLength;
            buffer.resize(length);
            buffer.fill(0.0f);
            writeIndex = 0;
            readIndex = 0;
        }
        
        void write(float input)
        {
            buffer.setUnchecked(writeIndex, input);
            writeIndex = (writeIndex + 1) % length;
        }
        
        float read()
        {
            float output = buffer.getUnchecked(readIndex);
            readIndex = (readIndex + 1) % length;
            return output;
        }
    };
    
    DelayLine delayLine;
    
    // Filters for tone shaping
    float lowpassState;
    float highpassState;
    
    // Generate Karplus-Strong signal
    void generateKSNote(float* output, int numSamples, float frequency, float velocity, float damping, float resonance);
    
    // Excitation functions
    void generateNoise(float* buffer, int numSamples, float amplitude);
    void generatePluck(float* buffer, int numSamples, float amplitude, float position);
    void generateRealisticPluck(float* buffer, int numSamples, float velocity, float position, float frequency);
    
    // Filter functions
    float lowpassFilter(float input, float cutoff);
    float highpassFilter(float input, float cutoff);
    float onePoleLP(float input, float cutoff);
    float allPassFilter(float input, float delay);
    float simulateGuitarBody(float input, float resonance, float frequency, int sampleIndex);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KarplusStrongEngine)
};