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

SynthesisEngine::SynthesisEngine()
    : currentSampleRate(44100.0)
    , blockSize(512)
    , frequencyManager(nullptr)
    , currentTuningReference(432.0)
{
    // Initialize synthesis engines
    physicalModeling = std::make_unique<PhysicalModelingEngine>();
    karplusStrong = std::make_unique<KarplusStrongEngine>();
    // wavetable = std::make_unique<WavetableEngine>();
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
}

void SynthesisEngine::releaseResources()
{
    synthBuffer.setSize(0, 0);
    
    if (physicalModeling)
        physicalModeling->releaseResources();
    
    if (karplusStrong)
        karplusStrong->releaseResources();
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
            
        case SynthesisType::KARPLUS_STRONG_GUITAR:
            if (karplusStrong)
                karplusStrong->generateGuitar(result, frequency, velocity);
            break;
            
        case SynthesisType::KARPLUS_STRONG_HARP:
            if (karplusStrong)
                karplusStrong->generateHarp(result, frequency, velocity);
            break;
            
        case SynthesisType::WAVETABLE_SYNTH:
            // TODO: Implement wavetable synthesis
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
}