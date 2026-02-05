/*
  ==============================================================================

    WavetableSynth.cpp
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#include "WavetableSynth.h"

WavetableEngine::WavetableEngine()
    : sampleRate(44100.0)
    , tuningReference(432.0)
    , blockSize(512)
    , wavetablePosition(0.5f)
    , filterCutoff(0.7f)
    , filterResonance(0.2f)
    , amplitude(0.8f)
{
    initializeWavetables();
}

WavetableEngine::~WavetableEngine()
{
}

void WavetableEngine::initialize(double newSampleRate, double newTuningReference)
{
    sampleRate = newSampleRate;
    tuningReference = newTuningReference;
}

void WavetableEngine::prepareToPlay(int newBlockSize)
{
    blockSize = newBlockSize;
}

void WavetableEngine::releaseResources()
{
    // Clean up resources
}

void WavetableEngine::generateSynth(AudioBuffer<float>& buffer, float frequency, float velocity)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    // Get an available voice for this note
    VoiceState* voice = getNextAvailableVoice();
    if (voice == nullptr) return;
    
    voice->reset();
    
    // Generate mono synth signal
    HeapBlock<float> monoBuffer;
    monoBuffer.allocate(numSamples, true);
    generateWavetableNote(monoBuffer.getData(), numSamples, sawWavetable, frequency, velocity);
    
    // Copy to stereo channels with slight stereo widening
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        float stereoPan = (channel == 0) ? 0.98f : 1.02f;
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            channelData[sample] = monoBuffer.getData()[sample] * stereoPan;
        }
    }
}

void WavetableEngine::generateOrgan(AudioBuffer<float>& buffer, float frequency, float velocity)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    // Get an available voice for this note
    VoiceState* voice = getNextAvailableVoice();
    if (voice == nullptr) return;
    
    voice->reset();
    
    // Generate mono organ signal
    HeapBlock<float> monoBuffer;
    monoBuffer.allocate(numSamples, true);
    generateOrganNote(monoBuffer.getData(), numSamples, frequency, velocity);
    
    // Copy to stereo channels with organ-style stereo spread
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        float stereoPan = (channel == 0) ? 0.95f : 1.05f;
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            channelData[sample] = monoBuffer.getData()[sample] * stereoPan;
        }
    }
}

void WavetableEngine::setParameters(const float* parameters)
{
    if (parameters != nullptr)
    {
        wavetablePosition = parameters[0];
        filterCutoff = parameters[1];
        filterResonance = parameters[2];
        amplitude = parameters[3];
    }
}

void WavetableEngine::updateTuning(double newTuningReference)
{
    tuningReference = newTuningReference;
}

void WavetableEngine::initializeWavetables()
{
    const int wavetableSize = 2048;
    
    // Initialize sine wavetable
    sineWavetable.resize(wavetableSize);
    for (int i = 0; i < wavetableSize; ++i)
    {
        float phase = (float)i / wavetableSize * 2.0f * MathConstants<float>::pi;
        sineWavetable.setUnchecked(i, std::sin(phase));
    }
    
    // Initialize sawtooth wavetable
    sawWavetable.resize(wavetableSize);
    for (int i = 0; i < wavetableSize; ++i)
    {
        float t = (float)i / wavetableSize;
        sawWavetable.setUnchecked(i, 2.0f * t - 1.0f);
    }
    
    // Initialize square wavetable with band-limiting
    squareWavetable.resize(wavetableSize);
    for (int i = 0; i < wavetableSize; ++i)
    {
        float phase = (float)i / wavetableSize * 2.0f * MathConstants<float>::pi;
        float sample = 0.0f;
        
        // Add harmonics for band-limited square wave
        for (int harmonic = 1; harmonic <= 32; harmonic += 2)
        {
            sample += std::sin(phase * harmonic) / harmonic;
        }
        squareWavetable.setUnchecked(i, sample * 4.0f / MathConstants<float>::pi);
    }
    
    // Initialize organ wavetable (drawbar simulation)
    organWavetable.resize(wavetableSize);
    for (int i = 0; i < wavetableSize; ++i)
    {
        float phase = (float)i / wavetableSize * 2.0f * MathConstants<float>::pi;
        float sample = 0.0f;
        
        // Hammond organ drawbar ratios
        sample += 0.8f * std::sin(phase);       // 16' (fundamental)
        sample += 0.6f * std::sin(phase * 2.0f); // 8' (octave)
        sample += 0.4f * std::sin(phase * 3.0f); // 5 1/3' (quint)
        sample += 0.5f * std::sin(phase * 4.0f); // 4' (octave)
        sample += 0.3f * std::sin(phase * 6.0f); // 2 2/3' (nazard)
        sample += 0.4f * std::sin(phase * 8.0f); // 2' (octave)
        
        organWavetable.setUnchecked(i, sample * 0.3f);
    }
}

float WavetableEngine::getWavetableSample(const Array<float>& wavetable, float phase)
{
    const int wavetableSize = wavetable.size();
    const float index = phase * wavetableSize / (2.0f * MathConstants<float>::pi);
    const int index0 = ((int)index) % wavetableSize;
    const int index1 = (index0 + 1) % wavetableSize;
    const float fraction = index - (int)index;
    
    // Linear interpolation
    const float sample0 = wavetable.getUnchecked(index0);
    const float sample1 = wavetable.getUnchecked(index1);
    
    return sample0 * (1.0f - fraction) + sample1 * fraction;
}

float WavetableEngine::interpolateWavetables(float phase, float position)
{
    // Interpolate between sine and saw based on position
    if (position <= 0.5f)
    {
        float mix = position * 2.0f;
        return getWavetableSample(sineWavetable, phase) * (1.0f - mix) + 
               getWavetableSample(sawWavetable, phase) * mix;
    }
    else
    {
        float mix = (position - 0.5f) * 2.0f;
        return getWavetableSample(sawWavetable, phase) * (1.0f - mix) + 
               getWavetableSample(squareWavetable, phase) * mix;
    }
}

void WavetableEngine::generateWavetableNote(float* output, int numSamples, const Array<float>& wavetable, 
                                           float frequency, float velocity)
{
    // Get current voice
    VoiceState* voice = &voices[currentVoiceIndex];
    if (!voice->isActive)
        voice->reset();
    
    const float phase_increment = frequency * 2.0f * MathConstants<float>::pi / (float)sampleRate;
    
    // ADSR envelope parameters
    const int attackSamples = (int)(sampleRate * 0.01f);  // 10ms attack
    const int decaySamples = (int)(sampleRate * 0.1f);    // 100ms decay
    const float sustainLevel = 0.7f;
    
    for (int i = 0; i < numSamples; ++i)
    {
        // Generate wavetable sample with morphing
        float sample = interpolateWavetables(voice->phase, wavetablePosition);
        
        // Apply ADSR envelope
        if (voice->sampleIndex < attackSamples)
        {
            voice->envelope = (float)voice->sampleIndex / attackSamples;
        }
        else if (voice->sampleIndex < attackSamples + decaySamples)
        {
            float decayProgress = (float)(voice->sampleIndex - attackSamples) / decaySamples;
            voice->envelope = 1.0f - decayProgress * (1.0f - sustainLevel);
        }
        else
        {
            voice->envelope = sustainLevel;
        }
        
        sample *= voice->envelope * velocity * amplitude;
        
        // Apply filter
        sample = lowpassFilter(sample, filterCutoff, voice->lowpassState1, voice->lowpassState2);
        
        output[i] = jlimit(-1.0f, 1.0f, sample * 0.6f);
        
        // Update phase
        voice->phase += phase_increment;
        if (voice->phase >= 2.0f * MathConstants<float>::pi)
            voice->phase -= 2.0f * MathConstants<float>::pi;
        
        voice->sampleIndex++;
    }
}

void WavetableEngine::generateOrganNote(float* output, int numSamples, float frequency, float velocity)
{
    // Get current voice
    VoiceState* voice = &voices[currentVoiceIndex];
    if (!voice->isActive)
        voice->reset();
    
    const float phase_increment = frequency * 2.0f * MathConstants<float>::pi / (float)sampleRate;
    
    // Organ has quick attack and sustain
    const int attackSamples = (int)(sampleRate * 0.001f);  // 1ms attack
    
    for (int i = 0; i < numSamples; ++i)
    {
        // Generate organ sample
        float sample = getWavetableSample(organWavetable, voice->phase);
        
        // Quick attack envelope for organ
        if (voice->sampleIndex < attackSamples)
        {
            voice->envelope = (float)voice->sampleIndex / attackSamples;
        }
        else
        {
            voice->envelope = 1.0f;
        }
        
        sample *= voice->envelope * velocity * amplitude;
        
        // Light filtering for organ
        sample = lowpassFilter(sample, 0.8f, voice->lowpassState1, voice->lowpassState2);
        
        output[i] = jlimit(-1.0f, 1.0f, sample * 0.5f);
        
        // Update phase
        voice->phase += phase_increment;
        if (voice->phase >= 2.0f * MathConstants<float>::pi)
            voice->phase -= 2.0f * MathConstants<float>::pi;
        
        voice->sampleIndex++;
    }
}

float WavetableEngine::lowpassFilter(float input, float cutoff, float& state1, float& state2)
{
    // Simple 2-pole lowpass filter
    const float alpha = jlimit(0.01f, 0.99f, cutoff);
    
    state1 = state1 * (1.0f - alpha) + input * alpha;
    state2 = state2 * (1.0f - alpha) + state1 * alpha;
    
    return state2;
}

WavetableEngine::VoiceState* WavetableEngine::getNextAvailableVoice()
{
    // First, try to find an inactive voice
    for (int i = 0; i < maxVoices; ++i)
    {
        if (!voices[i].isActive || voices[i].envelope < 0.001f)
        {
            currentVoiceIndex = i;
            return &voices[i];
        }
    }
    
    // If all voices are active, use round-robin voice stealing
    currentVoiceIndex = (currentVoiceIndex + 1) % maxVoices;
    return &voices[currentVoiceIndex];
}