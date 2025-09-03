/*
  ==============================================================================

    SynthesisEngine.cpp
    Created: 14 Aug 2025
    Author:  The Sound Studio Team

  ==============================================================================
*/

#include "SynthesisEngine.h"
#include "PhysicalModelingSynth.h"
#include "KarplusStrongSynth.h"
#include "WavetableSynth.h"

SynthesisEngine::SynthesisEngine()
    : currentSampleRate(44100.0)
    , blockSize(512)
    , frequencyManager(nullptr)
    , currentTuningReference(432.0)
{
    // Initialize synthesis engines
    physicalModeling = std::make_unique<PhysicalModelingEngine>();
    karplusStrong = std::make_unique<KarplusStrongEngine>();
    wavetable = std::make_unique<WavetableEngine>();
    // hybrid = std::make_unique<HybridEngine>();
    
    // Initialize default preset
    currentPreset.name = "Default Piano";
    currentPreset.type = SynthesisType::PHYSICAL_MODELING_PIANO;
    currentPreset.category = InstrumentCategory::KEYS_AND_PIANOS;
    
    // Initialize parameters to default values
    for (int i = 0; i < 16; ++i)
        currentParameters[i] = 0.5f;
    
    initializePresets();
}

SynthesisEngine::~SynthesisEngine()
{
}

void SynthesisEngine::initialize(double sampleRate, FrequencyManager* freqManager)
{
    currentSampleRate = sampleRate;
    frequencyManager = freqManager;
    
    if (frequencyManager)
        currentTuningReference = frequencyManager->getBaseAFrequency();
    
    // Initialize all synthesis engines
    if (physicalModeling)
        physicalModeling->initialize(sampleRate, currentTuningReference);
    
    if (karplusStrong)
        karplusStrong->initialize(sampleRate, currentTuningReference);
    
    if (wavetable)
        wavetable->initialize(sampleRate, currentTuningReference);
}

void SynthesisEngine::prepareToPlay(int samplesPerBlockExpected)
{
    blockSize = samplesPerBlockExpected;
    synthBuffer.setSize(2, blockSize, false, false, true);
    
    // Prepare all engines
    if (physicalModeling)
        physicalModeling->prepareToPlay(blockSize);
    
    if (karplusStrong)
        karplusStrong->prepareToPlay(blockSize);
    
    if (wavetable)
        wavetable->prepareToPlay(blockSize);
}

void SynthesisEngine::releaseResources()
{
    synthBuffer.setSize(0, 0);
    
    if (physicalModeling)
        physicalModeling->releaseResources();
    
    if (karplusStrong)
        karplusStrong->releaseResources();
    
    if (wavetable)
        wavetable->releaseResources();
}

AudioBuffer<float> SynthesisEngine::generateInstrument(SynthesisType type, 
                                                      float frequency, 
                                                      float velocity, 
                                                      int numSamples)
{
    AudioBuffer<float> result(2, numSamples);
    result.clear();
    
    switch (type)
    {
        case SynthesisType::PHYSICAL_MODELING_PIANO:
            if (physicalModeling)
                physicalModeling->generatePiano(result, frequency, velocity);
            break;
            
        case SynthesisType::PHYSICAL_MODELING_STRINGS:
            if (physicalModeling)
                physicalModeling->generateStrings(result, frequency, velocity);
            break;
            
        case SynthesisType::KARPLUS_STRONG_GUITAR:
            if (karplusStrong)
                karplusStrong->generateGuitar(result, frequency, velocity);
            break;
            
        case SynthesisType::KARPLUS_STRONG_HARP:
            if (karplusStrong)
                karplusStrong->generateHarp(result, frequency, velocity);
            break;
            
        case SynthesisType::WAVETABLE_SYNTH:
            if (wavetable)
                wavetable->generateSynth(result, frequency, velocity);
            break;
            
        case SynthesisType::WAVETABLE_ELECTRONIC:
            if (wavetable)
                wavetable->generateOrgan(result, frequency, velocity);
            break;
            
        default:
            // Generate simple sine wave as fallback
            for (int channel = 0; channel < result.getNumChannels(); ++channel)
            {
                auto* channelData = result.getWritePointer(channel);
                for (int sample = 0; sample < numSamples; ++sample)
                {
                    channelData[sample] = std::sin(2.0f * juce::MathConstants<float>::pi * 
                                                  frequency * sample / currentSampleRate) * velocity;
                }
            }
            break;
    }
    
    return result;
}

void SynthesisEngine::loadPreset(const SynthesisPreset& preset)
{
    currentPreset = preset;
    
    // Copy parameters
    for (int i = 0; i < 16; ++i)
        currentParameters[i] = preset.parameters[i];
    
    // Update synthesis engines with new parameters
    updateEngineParameters();
}

SynthesisPreset SynthesisEngine::getCurrentPreset() const
{
    return currentPreset;
}

Array<SynthesisPreset> SynthesisEngine::getAvailablePresets(InstrumentCategory category) const
{
    Array<SynthesisPreset> filtered;
    
    for (const auto& preset : presetDatabase)
    {
        if (preset.category == category)
            filtered.add(preset);
    }
    
    return filtered;
}

void SynthesisEngine::setParameter(int parameterIndex, float value)
{
    if (parameterIndex >= 0 && parameterIndex < 16)
    {
        currentParameters[parameterIndex] = juce::jlimit(0.0f, 1.0f, value);
        updateEngineParameters();
    }
}

float SynthesisEngine::getParameter(int parameterIndex) const
{
    if (parameterIndex >= 0 && parameterIndex < 16)
        return currentParameters[parameterIndex];
    
    return 0.0f;
}

void SynthesisEngine::updateTuningReference(double baseFrequency)
{
    currentTuningReference = baseFrequency;
    
    // Update all synthesis engines
    if (physicalModeling)
        physicalModeling->updateTuning(baseFrequency);
    
    if (karplusStrong)
        karplusStrong->updateTuning(baseFrequency);
    
    if (wavetable)
        wavetable->updateTuning(baseFrequency);
}

void SynthesisEngine::setScale(int scaleType)
{
    // Forward to frequency manager if available
    if (frequencyManager)
        frequencyManager->setScale(scaleType);
}

void SynthesisEngine::initializePresets()
{
    // Piano presets
    SynthesisPreset grandPiano;
    grandPiano.name = "Grand Classic";
    grandPiano.type = SynthesisType::PHYSICAL_MODELING_PIANO;
    grandPiano.category = InstrumentCategory::KEYS_AND_PIANOS;
    // Set default piano parameters
    for (int i = 0; i < 16; ++i)
        grandPiano.parameters[i] = 0.5f;
    grandPiano.parameters[0] = 0.8f; // String tension
    grandPiano.parameters[1] = 0.6f; // Hammer hardness
    grandPiano.parameters[2] = 0.4f; // Damping
    presetDatabase.add(grandPiano);
    
    // Guitar presets
    SynthesisPreset acousticGuitar;
    acousticGuitar.name = "Steel Guitar";
    acousticGuitar.type = SynthesisType::KARPLUS_STRONG_GUITAR;
    acousticGuitar.category = InstrumentCategory::GUITARS;
    for (int i = 0; i < 16; ++i)
        acousticGuitar.parameters[i] = 0.5f;
    acousticGuitar.parameters[0] = 0.7f; // Body resonance
    acousticGuitar.parameters[1] = 0.3f; // String damping
    presetDatabase.add(acousticGuitar);
    
    // Add more presets for each category...
    // TODO: Expand preset database with all instrument types
}

void SynthesisEngine::updateEngineParameters()
{
    // Update synthesis engines with current parameters
    if (physicalModeling)
        physicalModeling->setParameters(currentParameters);
    
    if (karplusStrong)
        karplusStrong->setParameters(currentParameters);
    
    if (wavetable)
        wavetable->setParameters(currentParameters);
}/*
  ==============================================================================

    WavetableSynth.cpp
    Created: 31 Aug 2025
    Author:  The Sound Studio Team

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
    , lowpassState1(0.0f)
    , lowpassState2(0.0f)
    , highpassState1(0.0f)
    , highpassState2(0.0f)
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
    
    // Generate mono synth signal
    HeapBlock<float> monoBuffer;
    monoBuffer.allocate(numSamples, true);
    generateWavetableNote(monoBuffer.getData(), numSamples, sawWavetable, frequency, velocity);
    
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

void WavetableEngine::generateOrgan(AudioBuffer<float>& buffer, float frequency, float velocity)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    // Generate mono organ signal
    HeapBlock<float> monoBuffer;
    monoBuffer.allocate(numSamples, true);
    generateOrganNote(monoBuffer.getData(), numSamples, frequency, velocity);
    
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
    const float phase_increment = frequency * 2.0f * MathConstants<float>::pi / (float)sampleRate;
    static float phase = 0.0f;
    static float envelope = 1.0f;
    
    // ADSR envelope parameters
    const int attackSamples = (int)(sampleRate * 0.01f);  // 10ms attack
    const int decaySamples = (int)(sampleRate * 0.1f);    // 100ms decay
    const float sustainLevel = 0.7f;
    
    for (int i = 0; i < numSamples; ++i)
    {
        // Generate wavetable sample with morphing
        float sample = interpolateWavetables(phase, wavetablePosition);
        
        // Apply ADSR envelope
        if (i < attackSamples)
        {
            envelope = (float)i / attackSamples;
        }
        else if (i < attackSamples + decaySamples)
        {
            float decayProgress = (float)(i - attackSamples) / decaySamples;
            envelope = 1.0f - decayProgress * (1.0f - sustainLevel);
        }
        else
        {
            envelope = sustainLevel;
        }
        
        sample *= envelope * velocity * amplitude;
        
        // Apply filter
        sample = lowpassFilter(sample, filterCutoff, lowpassState1, lowpassState2);
        
        output[i] = jlimit(-1.0f, 1.0f, sample * 0.6f);
        
        // Update phase
        phase += phase_increment;
        if (phase >= 2.0f * MathConstants<float>::pi)
            phase -= 2.0f * MathConstants<float>::pi;
    }
}

void WavetableEngine::generateOrganNote(float* output, int numSamples, float frequency, float velocity)
{
    const float phase_increment = frequency * 2.0f * MathConstants<float>::pi / (float)sampleRate;
    static float phase = 0.0f;
    static float envelope = 1.0f;
    
    // Organ has quick attack and sustain
    const int attackSamples = (int)(sampleRate * 0.001f);  // 1ms attack
    
    for (int i = 0; i < numSamples; ++i)
    {
        // Generate organ sample
        float sample = getWavetableSample(organWavetable, phase);
        
        // Quick attack envelope for organ
        if (i < attackSamples)
        {
            envelope = (float)i / attackSamples;
        }
        else
        {
            envelope = 1.0f;
        }
        
        sample *= envelope * velocity * amplitude;
        
        // Light filtering for organ
        sample = lowpassFilter(sample, 0.8f, lowpassState1, lowpassState2);
        
        output[i] = jlimit(-1.0f, 1.0f, sample * 0.5f);
        
        // Update phase
        phase += phase_increment;
        if (phase >= 2.0f * MathConstants<float>::pi)
            phase -= 2.0f * MathConstants<float>::pi;
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