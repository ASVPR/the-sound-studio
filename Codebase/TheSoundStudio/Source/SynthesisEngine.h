/*
  ==============================================================================

    SynthesisEngine.h
    Created: 14 Aug 2025
    Author:  The Sound Studio Team

    Central synthesis engine for The Sound Studio - coordinates all synthesis
    algorithms and provides instrument generation capabilities.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Parameters.h"
#include "FrequencyManager.h"
#include "PhysicalModelingSynth.h"
#include "KarplusStrongSynth.h"

// Synthesis types supported by TSS
enum class SynthesisType {
    PHYSICAL_MODELING_PIANO,
    PHYSICAL_MODELING_STRINGS,
    KARPLUS_STRONG_GUITAR,
    KARPLUS_STRONG_HARP,
    WAVETABLE_SYNTH,
    WAVETABLE_ELECTRONIC,
    HYBRID_MULTISAMPLED,
    VA_ANALOG_MODELING
};

// Instrument categories matching ASVPR structure
enum class InstrumentCategory {
    BELLS_AND_VIBES,
    CINEMATIC,
    GUITARS,
    KEYS_AND_PIANOS,
    ETHNIC,
    SYNTHESIZER
};

struct SynthesisPreset {
    String name;
    SynthesisType type;
    InstrumentCategory category;
    float parameters[16];  // Synthesis-specific parameters
};

class SynthesisEngine
{
public:
    SynthesisEngine();
    ~SynthesisEngine();
    
    // Main synthesis interface
    void initialize(double sampleRate, FrequencyManager* frequencyManager);
    void prepareToPlay(int samplesPerBlockExpected);
    void releaseResources();
    
    // Instrument generation
    AudioBuffer<float> generateInstrument(SynthesisType type, 
                                         float frequency, 
                                         float velocity, 
                                         int numSamples);
    
    // Preset management
    void loadPreset(const SynthesisPreset& preset);
    SynthesisPreset getCurrentPreset() const;
    Array<SynthesisPreset> getAvailablePresets(InstrumentCategory category) const;
    
    // Real-time parameter control
    void setParameter(int parameterIndex, float value);
    float getParameter(int parameterIndex) const;
    
    // Frequency scaling for tuning systems
    void updateTuningReference(double baseFrequency);
    void setScale(int scaleType);
    
private:
    // Synthesis engines (TODO: Implement remaining engines)
    std::unique_ptr<class PhysicalModelingSynth> physicalModeling;
    std::unique_ptr<class KarplusStrongSynth> karplusStrong;
    // TODO: Implement remaining engines
    // std::unique_ptr<class WavetableEngine> wavetable;
    // std::unique_ptr<class HybridEngine> hybrid;
    
    // Audio processing
    double currentSampleRate;
    int blockSize;
    AudioBuffer<float> synthBuffer;
    
    // Frequency management
    FrequencyManager* frequencyManager;
    double currentTuningReference;
    
    // Current state
    SynthesisPreset currentPreset;
    float currentParameters[16];
    
    // Presets database
    void initializePresets();
    
    // Internal engine management
    void updateEngineParameters();
    Array<SynthesisPreset> presetDatabase;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthesisEngine)
};