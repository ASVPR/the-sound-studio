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
    HeapBlock<float> monoBuffer;
    monoBuffer.allocate(numSamples, true);
    generatePianoNote(monoBuffer.getData(), numSamples, frequency, velocity);
    
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

void PhysicalModelingEngine::generateStrings(AudioBuffer<float>& buffer, float frequency, float velocity)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    // Generate mono string signal
    HeapBlock<float> monoBuffer;
    monoBuffer.allocate(numSamples, true);
    generateStringNote(monoBuffer.getData(), numSamples, frequency, velocity);
    
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
    // Enhanced piano physical modeling with realistic harmonic structure
    const float phase_increment = frequency * 2.0f * juce::MathConstants<float>::pi / (float)sampleRate;
    const float decay_factor = 1.0f - (damping * 0.0008f);
    
    // Use per-note state variables (in real implementation, these should be per-voice)
    static float phase = 0.0f;
    static float amplitude = 1.0f;
    static float resonancePhase = 0.0f;
    static float sympatheticPhase = 0.0f;
    
    // Reset amplitude for new notes
    if (amplitude < 0.01f) amplitude = 1.0f;
    
    // Piano-specific frequency ratios for realistic harmonics
    const float inharmonicity = 0.0001f * frequency; // Higher frequencies are more inharmonic
    
    for (int i = 0; i < numSamples; ++i)
    {
        // Generate fundamental with inharmonicity
        float fundamental = std::sin(phase);
        
        // Add realistic harmonic series with inharmonicity
        float harmonics = 0.0f;
        harmonics += 0.8f * std::sin(phase * 2.0f * (1.0f + inharmonicity));    // 2nd harmonic
        harmonics += 0.6f * std::sin(phase * 3.0f * (1.0f + inharmonicity * 2.0f)); // 3rd harmonic
        harmonics += 0.4f * std::sin(phase * 4.0f * (1.0f + inharmonicity * 3.0f)); // 4th harmonic
        harmonics += 0.3f * std::sin(phase * 5.0f * (1.0f + inharmonicity * 4.0f)); // 5th harmonic
        harmonics += 0.2f * std::sin(phase * 6.0f * (1.0f + inharmonicity * 5.0f)); // 6th harmonic
        
        float sample = fundamental + harmonics * 0.7f;
        
        // Apply enhanced hammer model with velocity dependence
        sample = applyHammerModel(sample, hammerHardness * (0.5f + velocity * 0.5f));
        
        // Add string resonance modeling
        resonancePhase += (frequency * 0.5f) * 2.0f * juce::MathConstants<float>::pi / (float)sampleRate;
        float stringResonance = std::sin(resonancePhase) * resonance * 0.15f * amplitude;
        
        // Add sympathetic string resonance (octave relationships)
        sympatheticPhase += (frequency * 2.0f) * 2.0f * juce::MathConstants<float>::pi / (float)sampleRate;
        float sympatheticResonance = std::sin(sympatheticPhase) * resonance * 0.08f * amplitude;
        
        // Combine all components
        sample += stringResonance + sympatheticResonance;
        
        // Apply realistic envelope with multiple decay stages
        float envelopeStage1 = std::exp(-i * 0.0001f); // Initial bright attack
        float envelopeStage2 = std::exp(-i * 0.00005f); // Sustained harmonics
        float envelope = envelopeStage1 * 0.3f + envelopeStage2 * 0.7f;
        
        sample *= amplitude * velocity * envelope;
        
        // Apply frequency-dependent damping
        float freqDamping = 1.0f - (frequency > 1000.0f ? damping * 1.5f : damping) * 0.0008f;
        amplitude *= freqDamping;
        
        // Add subtle soundboard resonance
        float soundboardFreq = 200.0f + resonance * 300.0f;
        float soundboardPhase = i * soundboardFreq * 2.0f * juce::MathConstants<float>::pi / (float)sampleRate;
        sample += std::sin(soundboardPhase) * resonance * 0.05f * amplitude;
        
        output[i] = jlimit(-1.0f, 1.0f, sample * 0.25f);
        
        // Update phases
        phase += phase_increment;
        if (phase >= 2.0f * juce::MathConstants<float>::pi)
        {
            phase -= 2.0f * juce::MathConstants<float>::pi;
            resonancePhase -= 2.0f * juce::MathConstants<float>::pi;
            sympatheticPhase -= 2.0f * juce::MathConstants<float>::pi;
        }
    }
}

void PhysicalModelingEngine::generateStringNote(float* output, int numSamples, float frequency, float velocity)
{
    // Enhanced bowed string physical modeling
    const float phase_increment = frequency * 2.0f * juce::MathConstants<float>::pi / (float)sampleRate;
    const float decay_factor = 1.0f - (damping * 0.0003f); // Slower decay for strings
    
    static float phase = 0.0f;
    static float amplitude = 1.0f;
    static float bowNoise = 0.0f;
    
    // Reset amplitude for new notes
    if (amplitude < 0.01f) amplitude = 1.0f;
    
    Random random(Time::currentTimeMillis());
    
    for (int i = 0; i < numSamples; ++i)
    {
        // Generate complex bowed string waveform
        float fundamental = std::sin(phase);
        
        // Rich harmonic series for bowed strings
        float harmonics = 0.0f;
        harmonics += 0.7f * std::sin(phase * 2.0f);  // Strong 2nd harmonic
        harmonics += 0.5f * std::sin(phase * 3.0f);  // 3rd harmonic
        harmonics += 0.4f * std::sin(phase * 4.0f);  // 4th harmonic
        harmonics += 0.3f * std::sin(phase * 5.0f);  // 5th harmonic
        harmonics += 0.2f * std::sin(phase * 6.0f);  // 6th harmonic
        harmonics += 0.15f * std::sin(phase * 7.0f); // 7th harmonic
        
        float sample = fundamental + harmonics * 0.8f;
        
        // Apply bow pressure with dynamic variations
        float dynamicBowPressure = bowPressure + 0.1f * std::sin(phase * 0.1f);
        sample = std::tanh(sample * dynamicBowPressure * 2.5f) * 0.8f;
        
        // Add bow noise (rosin friction)
        bowNoise = bowNoise * 0.95f + (random.nextFloat() * 2.0f - 1.0f) * 0.05f;
        sample += bowNoise * bowPressure * 0.03f;
        
        // Add string body resonance
        float bodyResonanceFreq = frequency * 0.5f + stringDensity * 100.0f;
        float bodyPhase = i * bodyResonanceFreq * 2.0f * juce::MathConstants<float>::pi / (float)sampleRate;
        sample += std::sin(bodyPhase) * resonance * 0.1f * amplitude;
        
        // Apply realistic envelope with sustain
        float sustainLevel = 0.6f + velocity * 0.4f;
        float envelope = 1.0f;
        if (i < numSamples * 0.1f) // Attack phase
        {
            envelope = (float)i / (numSamples * 0.1f);
        }
        else // Sustain phase
        {
            envelope = sustainLevel;
        }
        
        sample *= amplitude * velocity * envelope;
        
        // Apply damping
        amplitude *= decay_factor;
        
        output[i] = jlimit(-1.0f, 1.0f, sample * 0.35f);
        
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
    // Enhanced hammer hardness model with velocity dependence
    const float softness = 1.0f - hardness;
    
    // Nonlinear compression based on hammer hardness
    float compressed = std::tanh(input * (0.5f + hardness * 2.5f));
    
    // Add harmonic content based on hammer hardness
    float harmonicContent = std::sin(input * 6.0f) * hardness * 0.1f;
    
    // Combine compressed signal with harmonic content
    float result = compressed * (0.4f + softness * 0.6f) + harmonicContent;
    
    return jlimit(-1.0f, 1.0f, result);
}