/*
  ==============================================================================

    PhysicalModelingSynth.cpp
    Created: 14 Aug 2025
    Author:  The Sound Studio Team

  ==============================================================================
*/

#include "PhysicalModelingSynth.h"

PhysicalModelingEngine::PhysicalModelingEngine()
    : sampleRate(44100.0)
    , tuningReference(432.0)
    , blockSize(512)
    , stringTension(0.8f)
    , hammerHardness(0.6f)
    , damping(0.4f)
    , resonance(0.5f)
    , stringLength(1.0f)
    , stringDensity(0.5f)
    , bowPressure(0.5f)
{
}

PhysicalModelingEngine::~PhysicalModelingEngine()
{
}

void PhysicalModelingEngine::initialize(double newSampleRate, double newTuningReference)
{
    sampleRate = newSampleRate;
    tuningReference = newTuningReference;
}

void PhysicalModelingEngine::prepareToPlay(int newBlockSize)
{
    blockSize = newBlockSize;
}

void PhysicalModelingEngine::releaseResources()
{
    // Clean up any allocated resources
}

void PhysicalModelingEngine::generatePiano(AudioBuffer<float>& buffer, float frequency, float velocity)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    // Generate mono piano signal
    HeapBlock<float> monoBuffer(numSamples);
    generatePianoNote(monoBuffer, numSamples, frequency, velocity);
    
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

void PhysicalModelingEngine::generateStrings(AudioBuffer<float>& buffer, float frequency, float velocity)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    // Generate mono string signal
    HeapBlock<float> monoBuffer(numSamples);
    generateStringNote(monoBuffer, numSamples, frequency, velocity);
    
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

void PhysicalModelingEngine::setParameters(const float* parameters)
{
    if (parameters != nullptr)
    {
        stringTension = parameters[0];
        hammerHardness = parameters[1];
        damping = parameters[2];
        resonance = parameters[3];
        stringLength = parameters[4];
        stringDensity = parameters[5];
        bowPressure = parameters[6];
    }
}

void PhysicalModelingEngine::updateTuning(double newTuningReference)
{
    tuningReference = newTuningReference;
}

void PhysicalModelingEngine::generatePianoNote(float* output, int numSamples, float frequency, float velocity)
{
    // Simple physical modeling approximation for piano
    // This is a basic implementation - a full physical model would be much more complex
    
    const float phase_increment = frequency * 2.0f * juce::MathConstants<float>::pi / (float)sampleRate;
    const float decay_factor = 1.0f - (damping * 0.001f);
    
    static float phase = 0.0f;
    static float amplitude = 1.0f;
    
    for (int i = 0; i < numSamples; ++i)
    {
        // Generate basic waveform with harmonics
        float sample = std::sin(phase);
        sample += 0.5f * std::sin(phase * 2.0f);  // 2nd harmonic
        sample += 0.25f * std::sin(phase * 3.0f); // 3rd harmonic
        sample += 0.125f * std::sin(phase * 4.0f); // 4th harmonic
        
        // Apply hammer hardness
        sample = applyHammerModel(sample, hammerHardness);
        
        // Apply envelope and damping
        sample *= amplitude * velocity;
        amplitude *= decay_factor;
        
        // Add resonance
        sample += sample * resonance * 0.1f * std::sin(phase * 0.5f);
        
        output[i] = sample * 0.3f; // Scale down to prevent clipping
        
        // Update phase
        phase += phase_increment;
        if (phase >= 2.0f * juce::MathConstants<float>::pi)
            phase -= 2.0f * juce::MathConstants<float>::pi;
    }
}

void PhysicalModelingEngine::generateStringNote(float* output, int numSamples, float frequency, float velocity)
{
    // Simple bowed string physical modeling
    const float phase_increment = frequency * 2.0f * juce::MathConstants<float>::pi / (float)sampleRate;
    const float decay_factor = 1.0f - (damping * 0.0005f);
    
    static float phase = 0.0f;
    static float amplitude = 1.0f;
    
    for (int i = 0; i < numSamples; ++i)
    {
        // Generate sawtooth-like waveform for bowed string
        float sample = 2.0f * (phase / (2.0f * juce::MathConstants<float>::pi)) - 1.0f;
        
        // Add harmonics for richness
        sample += 0.3f * std::sin(phase * 2.0f);
        sample += 0.15f * std::sin(phase * 3.0f);
        
        // Apply bow pressure effects
        sample = std::tanh(sample * bowPressure * 3.0f);
        
        // Apply envelope
        sample *= amplitude * velocity;
        amplitude *= decay_factor;
        
        output[i] = sample * 0.4f;
        
        // Update phase
        phase += phase_increment;
        if (phase >= 2.0f * juce::MathConstants<float>::pi)
            phase -= 2.0f * juce::MathConstants<float>::pi;
    }
}

float PhysicalModelingEngine::calculateStringLength(float frequency)
{
    // Calculate string length based on frequency and tension
    return (float)tuningReference / frequency * stringTension;
}

float PhysicalModelingEngine::applyHammerModel(float input, float hardness)
{
    // Simple hammer hardness model
    const float softness = 1.0f - hardness;
    return std::tanh(input * (1.0f + hardness * 2.0f)) * (0.5f + softness * 0.5f);
}