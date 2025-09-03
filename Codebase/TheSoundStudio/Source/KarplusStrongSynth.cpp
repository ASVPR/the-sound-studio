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
    HeapBlock<float> monoBuffer;
    monoBuffer.allocate(numSamples, true);
    generateKSNote(monoBuffer.getData(), numSamples, frequency, velocity, stringDamping, bodyResonance);
    
    // Copy to stereo channels with slight variations for width
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        const float pan = (channel == 0) ? 0.9f : 1.1f; // Slight stereo width
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            channelData[sample] = monoBuffer.getData()[sample] * pan;
        }
    }
}

void KarplusStrongEngine::generateHarp(AudioBuffer<float>& buffer, float frequency, float velocity)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    // Generate mono harp signal with harp-specific parameters
    HeapBlock<float> monoBuffer;
    monoBuffer.allocate(numSamples, true);
    const float harpDamping = stringDamping * 0.5f; // Less damping for harp
    const float harpResonance = bodyResonance * 1.2f; // More resonance for harp
    
    generateKSNote(monoBuffer.getData(), numSamples, frequency, velocity, harpDamping, harpResonance);
    
    // Copy to stereo channels
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < numSamples; ++sample)
        {
            channelData[sample] = monoBuffer.getData()[sample];
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
        feedback = jlimit(0.1f, 0.99f, parameters[3]);
    }
}

void KarplusStrongEngine::updateTuning(double newTuningReference)
{
    tuningReference = newTuningReference;
}

void KarplusStrongEngine::generateKSNote(float* output, int numSamples, float frequency, float velocity, float damping, float resonance)
{
    // Calculate delay line length with fractional delay support
    const float exactDelayLength = (float)sampleRate / frequency;
    const int delayLength = (int)exactDelayLength;
    const float fractionalPart = exactDelayLength - delayLength;
    
    if (delayLength < 2) return; // Frequency too high
    
    // Resize delay line if necessary
    if (delayLine.length != delayLength)
    {
        delayLine.resize(delayLength + 1); // +1 for fractional delay interpolation
        
        // Initialize with more realistic pluck excitation
        HeapBlock<float> excitation;
        excitation.allocate(delayLength, true);
        generatePluck(excitation.getData(), delayLength, velocity, pluckPosition);
        
        // Fill delay line with excitation
        for (int i = 0; i < delayLength; ++i)
        {
            delayLine.write(excitation.getData()[i]);
        }
    }
    
    // Generate output using Enhanced Karplus-Strong algorithm
    for (int i = 0; i < numSamples; ++i)
    {
        // Read from delay line with fractional delay interpolation
        float delayOutput = delayLine.read();
        float nextSample = (delayLine.readIndex < delayLine.length - 1) ? 
            delayLine.buffer.getUnchecked(delayLine.readIndex) : 
            delayLine.buffer.getUnchecked(0);
        
        // Linear interpolation for fractional delay
        delayOutput = delayOutput * (1.0f - fractionalPart) + nextSample * fractionalPart;
        
        // Enhanced damping filter (more realistic frequency response)
        float dampedOutput = lowpassFilter(delayOutput, 0.5f + (1.0f - damping) * 0.5f);
        dampedOutput = highpassFilter(dampedOutput, damping * 0.1f); // Remove DC
        
        // Nonlinear saturation for more realistic pluck behavior
        dampedOutput = std::tanh(dampedOutput * 1.5f) * 0.7f;
        
        // Apply dynamic feedback based on frequency content
        float dynamicFeedback = feedback * (0.99f - std::abs(dampedOutput) * 0.1f);
        dampedOutput *= dynamicFeedback;
        
        // Write back to delay line
        delayLine.write(dampedOutput);
        
        // Enhanced body resonance modeling
        float bodyResponse = highpassFilter(dampedOutput * 0.3f, resonance * 0.05f);
        bodyResponse += lowpassFilter(dampedOutput * 0.2f, resonance * 0.3f);
        
        // Combine direct signal with body resonance
        output[i] = dampedOutput * 0.8f + bodyResponse * resonance * 0.4f;
        
        // Add slight harmonic content for realism
        output[i] += std::sin(2.0f * juce::MathConstants<float>::pi * frequency * 2.0f * i / sampleRate) * velocity * 0.05f;
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
    // Generate more realistic pluck excitation signal
    Random random(Time::currentTimeMillis());
    const int pluckPoint = jlimit(1, numSamples - 1, (int)(position * numSamples));
    
    for (int i = 0; i < numSamples; ++i)
    {
        float sample = 0.0f;
        
        // Create more realistic pluck shape with exponential curves
        if (i <= pluckPoint)
        {
            float t = (float)i / pluckPoint;
            sample = t * (2.0f - t); // Quadratic rise
        }
        else
        {
            float t = (float)(i - pluckPoint) / (numSamples - pluckPoint);
            sample = std::exp(-3.0f * t); // Exponential decay
        }
        
        // Add controlled noise for finger/pick variation
        float noiseAmount = 0.15f * (1.0f - std::abs(sample));
        sample += (random.nextFloat() * 2.0f - 1.0f) * noiseAmount;
        
        // Add slight harmonic content to initial pluck
        sample += 0.1f * std::sin(2.0f * juce::MathConstants<float>::pi * i * 3.0f / numSamples);
        
        buffer[i] = jlimit(-1.0f, 1.0f, sample * amplitude);
    }
}

float KarplusStrongEngine::lowpassFilter(float input, float cutoff)
{
    // Enhanced one-pole lowpass filter with better frequency response
    const float alpha = jlimit(0.0f, 0.99f, cutoff);
    lowpassState = lowpassState * (1.0f - alpha) + input * alpha;
    
    // Add slight resonance for more character
    lowpassState += (input - lowpassState) * 0.05f;
    
    return lowpassState;
}

float KarplusStrongEngine::highpassFilter(float input, float cutoff)
{
    // Enhanced one-pole highpass filter
    const float alpha = jlimit(0.0f, 0.99f, cutoff);
    const float output = input - highpassState;
    highpassState = highpassState * (1.0f - alpha) + input * alpha;
    
    // Prevent DC buildup
    return jlimit(-1.0f, 1.0f, output);
}