/*
  ==============================================================================

    KarplusStrongSynth.cpp
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

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
        
        // Initialize with enhanced pluck excitation based on velocity
        HeapBlock<float> excitation;
        excitation.allocate(delayLength, true);
        generateRealisticPluck(excitation.getData(), delayLength, velocity, pluckPosition, frequency);
        
        // Fill delay line with excitation
        for (int i = 0; i < delayLength; ++i)
        {
            delayLine.write(excitation.getData()[i]);
        }
    }
    
    // Generate output using Enhanced Karplus-Strong algorithm
    for (int i = 0; i < numSamples; ++i)
    {
        // Read from delay line with cubic interpolation for smoother sound
        float delayOutput = delayLine.read();
        
        // Implement all-pass interpolation for fractional delay
        if (fractionalPart > 0.01f)
        {
            // All-pass filter for fractional delay with better tuning accuracy
            delayOutput = allPassFilter(delayOutput, fractionalPart);
        }
        
        // Multi-stage loop filter for realistic string damping
        float filtered = delayOutput;
        
        // Stage 1: Frequency-dependent damping (higher frequencies decay faster)
        float cutoffFreq = 0.6f - damping * 0.4f; // Dynamic cutoff based on damping
        filtered = onePoleLP(filtered, cutoffFreq);
        
        // Stage 2: String stiffness simulation (slight all-pass filtering)
        float stiffness = frequency < 200.0f ? 0.02f : 0.001f; // Lower strings have more stiffness
        filtered = allPassFilter(filtered, stiffness);
        
        // Stage 3: Nonlinear processing for realistic string behavior
        float gain = 1.0f + velocity * 0.3f; // Harder plucks add slight nonlinearity
        filtered = std::tanh(filtered * gain) / gain;
        
        // Dynamic feedback based on signal amplitude and frequency
        float amplitudeFactor = 1.0f - std::abs(filtered) * 0.05f; // Less feedback for louder signals
        float frequencyFactor = frequency > 1000.0f ? 0.98f : 0.995f; // High frequencies decay faster
        float dynamicFeedback = feedback * amplitudeFactor * frequencyFactor;
        
        filtered *= dynamicFeedback;
        
        // Write back to delay line
        delayLine.write(filtered);
        
        // Enhanced body resonance with multiple resonant modes
        float bodyOutput = simulateGuitarBody(filtered, resonance, frequency, i);
        
        // Combine string output with body resonance
        float finalOutput = filtered * 0.7f + bodyOutput * 0.3f;
        
        // Add slight sympathetic resonance (octave and fifth)
        if (i % 4 == 0) // Reduce CPU load by calculating only every 4th sample
        {
            float octaveResonance = std::sin(2.0f * juce::MathConstants<float>::pi * frequency * 2.0f * i / sampleRate);
            float fifthResonance = std::sin(2.0f * juce::MathConstants<float>::pi * frequency * 1.5f * i / sampleRate);
            finalOutput += (octaveResonance * 0.02f + fifthResonance * 0.015f) * velocity * resonance;
        }
        
        output[i] = jlimit(-1.0f, 1.0f, finalOutput * 0.8f);
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

void KarplusStrongEngine::generateRealisticPluck(float* buffer, int numSamples, float velocity, float position, float frequency)
{
    // Generate more sophisticated pluck excitation based on actual guitar physics
    Random random(Time::currentTimeMillis());
    const int pluckPoint = jlimit(1, numSamples - 1, (int)(position * numSamples));
    
    // Create bandwidth-limited impulse based on frequency
    float bandwidth = frequency * 2.0f; // Higher frequencies have more bandwidth
    float cutoff = jmin(bandwidth / (float)sampleRate * 2.0f, 0.9f);
    
    for (int i = 0; i < numSamples; ++i)
    {
        float sample = 0.0f;
        
        // Generate triangular pluck shape with velocity-dependent sharpness
        float sharpness = 1.0f + velocity * 2.0f; // Harder plucks are sharper
        
        if (i <= pluckPoint)
        {
            float t = (float)i / pluckPoint;
            sample = std::pow(t, 1.0f / sharpness) * (2.0f - std::pow(t, 1.0f / sharpness));
        }
        else
        {
            float t = (float)(i - pluckPoint) / (numSamples - pluckPoint);
            sample = (1.0f - std::pow(t, sharpness)) * std::exp(-t * sharpness * 3.0f);
        }
        
        // Add controlled randomness for string irregularities
        sample += (random.nextFloat() * 2.0f - 1.0f) * velocity * 0.1f;
        
        // Apply low-pass filter to limit bandwidth
        sample = lowpassFilter(sample, cutoff);
        
        buffer[i] = sample * velocity;
    }
}

float KarplusStrongEngine::onePoleLP(float input, float cutoff)
{
    // Simple one-pole low-pass filter
    static float state = 0.0f;
    state = state * (1.0f - cutoff) + input * cutoff;
    return state;
}

float KarplusStrongEngine::allPassFilter(float input, float delay)
{
    // Simple all-pass filter for fractional delay
    static float state = 0.0f;
    float output = -input + state;
    state = input + delay * output;
    return output;
}

float KarplusStrongEngine::simulateGuitarBody(float input, float resonance, float frequency, int sampleIndex)
{
    // Simulate guitar body resonance with multiple resonant modes
    static float bodyState1 = 0.0f, bodyState2 = 0.0f, bodyState3 = 0.0f;
    
    // Primary resonant frequencies of a typical guitar body
    float freq1 = 100.0f + resonance * 50.0f;   // Main air resonance
    float freq2 = 200.0f + resonance * 80.0f;   // Top plate resonance
    float freq3 = 400.0f + resonance * 100.0f;  // Higher mode
    
    // Calculate phases for each resonant mode
    float phase1 = 2.0f * juce::MathConstants<float>::pi * freq1 * sampleIndex / (float)sampleRate;
    float phase2 = 2.0f * juce::MathConstants<float>::pi * freq2 * sampleIndex / (float)sampleRate;
    float phase3 = 2.0f * juce::MathConstants<float>::pi * freq3 * sampleIndex / (float)sampleRate;
    
    // Apply resonant filtering
    float resonance1 = std::sin(phase1) * input * 0.3f;
    float resonance2 = std::sin(phase2) * input * 0.2f;
    float resonance3 = std::sin(phase3) * input * 0.1f;
    
    // Combine resonant modes
    float bodyOutput = resonance1 + resonance2 + resonance3;
    
    // Apply frequency-dependent response (more resonance at lower frequencies)
    float freqResponse = frequency < 300.0f ? 1.0f : 300.0f / frequency;
    
    return bodyOutput * resonance * freqResponse * 0.5f;
}