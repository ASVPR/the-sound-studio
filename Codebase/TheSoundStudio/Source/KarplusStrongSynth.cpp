/*
  ==============================================================================

    KarplusStrongSynth.cpp
    Created: 14 Aug 2025
    Author:  The Sound Studio Team

  ==============================================================================
*/

#include "KarplusStrongSynth.h"

KarplusStrongEngine::KarplusStrongEngine()
    : sampleRate(44100.0)
    , tuningReference(432.0)
    , blockSize(512)
    , bodyResonance(0.7f)
    , stringDamping(0.3f)
    , pluckPosition(0.5f)
    , feedback(0.95f)
    , lowpassState(0.0f)
    , highpassState(0.0f)
{
}

KarplusStrongEngine::~KarplusStrongEngine()
{
}

void KarplusStrongEngine::initialize(double newSampleRate, double newTuningReference)
{
    sampleRate = newSampleRate;
    tuningReference = newTuningReference;
}

void KarplusStrongEngine::prepareToPlay(int newBlockSize)
{
    blockSize = newBlockSize;
}

void KarplusStrongEngine::releaseResources()
{
    // Clean up resources
}

void KarplusStrongEngine::generateGuitar(AudioBuffer<float>& buffer, float frequency, float velocity)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    // Generate mono guitar signal with guitar-specific parameters
    HeapBlock<float> monoBuffer(numSamples);
    generateKSNote(monoBuffer, numSamples, frequency, velocity, stringDamping, bodyResonance);
    
    // Copy to stereo channels with slight variations for width
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        const float pan = (channel == 0) ? 0.9f : 1.1f; // Slight stereo width
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            channelData[sample] = monoBuffer[sample] * pan;
        }
    }
}

void KarplusStrongEngine::generateHarp(AudioBuffer<float>& buffer, float frequency, float velocity)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    // Generate mono harp signal with harp-specific parameters
    HeapBlock<float> monoBuffer(numSamples);
    const float harpDamping = stringDamping * 0.5f; // Less damping for harp
    const float harpResonance = bodyResonance * 1.2f; // More resonance for harp
    
    generateKSNote(monoBuffer, numSamples, frequency, velocity, harpDamping, harpResonance);
    
    // Copy to stereo channels
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < numSamples; ++sample)
        {
            channelData[sample] = monoBuffer[sample];
        }
    }
}

void KarplusStrongEngine::setParameters(const float* parameters)
{
    if (parameters != nullptr)
    {
        bodyResonance = parameters[0];
        stringDamping = parameters[1];
        pluckPosition = parameters[2];
        feedback = juce::jlimit(0.1f, 0.99f, parameters[3]);
    }
}

void KarplusStrongEngine::updateTuning(double newTuningReference)
{
    tuningReference = newTuningReference;
}

void KarplusStrongEngine::generateKSNote(float* output, int numSamples, float frequency, float velocity, float damping, float resonance)
{
    // Calculate delay line length based on frequency
    const int delayLength = (int)(sampleRate / frequency);
    if (delayLength < 2) return; // Frequency too high
    
    // Resize delay line if necessary
    if (delayLine.length != delayLength)
    {
        delayLine.resize(delayLength);
        
        // Initialize with pluck excitation
        HeapBlock<float> excitation(delayLength);
        generatePluck(excitation, delayLength, velocity, pluckPosition);
        
        // Fill delay line with excitation
        for (int i = 0; i < delayLength; ++i)
        {
            delayLine.write(excitation[i]);
        }
    }
    
    // Generate output using Karplus-Strong algorithm
    for (int i = 0; i < numSamples; ++i)
    {
        // Read from delay line
        float delayOutput = delayLine.read();
        
        // Apply lowpass filtering for damping
        delayOutput = lowpassFilter(delayOutput, 1.0f - damping);
        
        // Apply feedback
        delayOutput *= feedback;
        
        // Write back to delay line
        delayLine.write(delayOutput);
        
        // Apply body resonance
        float bodyResponse = highpassFilter(delayOutput, resonance * 0.1f);
        
        // Combine direct and body response
        output[i] = delayOutput + bodyResponse * resonance;
    }
}

void KarplusStrongEngine::generateNoise(float* buffer, int numSamples, float amplitude)
{
    Random random;
    for (int i = 0; i < numSamples; ++i)
    {
        buffer[i] = (random.nextFloat() * 2.0f - 1.0f) * amplitude;
    }
}

void KarplusStrongEngine::generatePluck(float* buffer, int numSamples, float amplitude, float position)
{
    // Generate pluck excitation signal
    Random random;
    const int pluckPoint = (int)(position * numSamples);
    
    for (int i = 0; i < numSamples; ++i)
    {
        // Create triangular pluck shape
        float sample = 0.0f;
        
        if (i <= pluckPoint)
        {
            sample = (float)i / pluckPoint;
        }
        else
        {
            sample = 1.0f - (float)(i - pluckPoint) / (numSamples - pluckPoint);
        }
        
        // Add some noise for realism
        sample += (random.nextFloat() * 2.0f - 1.0f) * 0.1f;
        
        buffer[i] = sample * amplitude;
    }
}

float KarplusStrongEngine::lowpassFilter(float input, float cutoff)
{
    // Simple one-pole lowpass filter
    const float alpha = cutoff;
    lowpassState = lowpassState * (1.0f - alpha) + input * alpha;
    return lowpassState;
}

float KarplusStrongEngine::highpassFilter(float input, float cutoff)
{
    // Simple one-pole highpass filter
    const float alpha = cutoff;
    const float output = input - highpassState;
    highpassState = highpassState * (1.0f - alpha) + input * alpha;
    return output;
}