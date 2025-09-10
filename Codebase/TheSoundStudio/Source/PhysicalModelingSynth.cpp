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
    for (int i = 0; i < maxVoices; ++i)
        voices[i].reset();
}

void PhysicalModelingEngine::generatePiano(AudioBuffer<float>& buffer, float frequency, float velocity)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    // Get an available voice for this note
    VoiceState* voice = getNextAvailableVoice();
    if (voice == nullptr) return;
    
    voice->reset();
    
    // Generate mono piano signal
    HeapBlock<float> monoBuffer;
    monoBuffer.allocate(numSamples, true);
    generatePianoNote(monoBuffer.getData(), numSamples, frequency, velocity);
    
    // Apply stereo widening for more realistic sound
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

void PhysicalModelingEngine::generateStrings(AudioBuffer<float>& buffer, float frequency, float velocity)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    // Get an available voice for this note
    VoiceState* voice = getNextAvailableVoice();
    if (voice == nullptr) return;
    
    voice->reset();
    
    // Generate mono string signal
    HeapBlock<float> monoBuffer;
    monoBuffer.allocate(numSamples, true);
    generateStringNote(monoBuffer.getData(), numSamples, frequency, velocity);
    
    // Apply stereo widening
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
    // Get current voice state
    VoiceState* voice = &voices[currentVoiceIndex];
    
    // Enhanced piano physical modeling with realistic harmonic structure
    const float phase_increment = frequency * 2.0f * juce::MathConstants<float>::pi / (float)sampleRate;
    const float decay_factor = 1.0f - (damping * 0.0008f);
    
    // Initialize voice if needed
    if (!voice->isActive)
    {
        voice->reset();
    }
    
    // Improved inharmonicity calculation based on string physics
    // Higher strings (treble) have more inharmonicity than bass strings
    const float B_coefficient = 0.0001f * std::pow(frequency / 261.63f, 1.8f); // C4 = 261.63 Hz reference
    
    for (int i = 0; i < numSamples; ++i)
    {
        // Generate fundamental
        float fundamental = std::sin(voice->phase) * 0.9f;
        
        // Enhanced harmonic series with proper inharmonicity based on Railsback curve
        float harmonics = 0.0f;
        for (int h = 2; h <= 12; ++h)
        {
            // Inharmonicity formula: f_h = h * f_0 * sqrt(1 + B * h^2)
            float harmonic_ratio = (float)h * std::sqrt(1.0f + B_coefficient * h * h);
            
            // Realistic harmonic amplitudes that decay with harmonic number
            float harmonic_amplitude = 1.0f / (float)h;
            
            // Add some randomness for more realistic timbre
            harmonic_amplitude *= (0.9f + 0.2f * std::sin(voice->phase * 0.01f * h));
            
            harmonics += harmonic_amplitude * std::sin(voice->phase * harmonic_ratio);
        }
        harmonics *= 0.6f; // Scale harmonics relative to fundamental
        
        float sample = fundamental + harmonics;
        
        // Apply enhanced hammer model with velocity-dependent hardness
        float dynamic_hardness = hammerHardness + (1.0f - velocity) * 0.3f; // Softer hammers for quieter notes
        sample = applyHammerModel(sample, dynamic_hardness);
        
        // Add duplex scaling (additional resonant frequencies)
        voice->resonancePhase += (frequency * 0.618f) * 2.0f * juce::MathConstants<float>::pi / (float)sampleRate; // Golden ratio
        float duplexResonance = std::sin(voice->resonancePhase) * resonance * 0.12f * voice->amplitude;
        
        // Add sympathetic string resonance with octave and fifth relationships
        voice->sympatheticPhase += (frequency * 2.0f) * 2.0f * juce::MathConstants<float>::pi / (float)sampleRate; // Octave
        float sympatheticOctave = std::sin(voice->sympatheticPhase) * resonance * 0.06f * voice->amplitude;
        
        float sympatheticFifth = std::sin(voice->sympatheticPhase * 1.5f) * resonance * 0.04f * voice->amplitude; // Fifth
        
        // Combine all resonances
        sample += duplexResonance + sympatheticOctave + sympatheticFifth;
        
        // Realistic ADSR envelope with exponential decay
        float time_ratio = (float)i / (float)numSamples;
        float envelope = 1.0f;
        
        if (time_ratio < 0.02f) // Attack: 2% of note length
        {
            envelope = time_ratio / 0.02f;
        }
        else // Exponential decay
        {
            float decay_time = time_ratio - 0.02f;
            envelope = std::exp(-decay_time * (2.0f + frequency * 0.001f)); // Higher frequencies decay faster
        }
        
        sample *= voice->amplitude * velocity * envelope;
        
        // Apply frequency-dependent damping
        float frequency_damping_factor = frequency > 1000.0f ? 1.2f : 1.0f;
        voice->amplitude *= (1.0f - damping * frequency_damping_factor * 0.0008f);
        
        // Enhanced soundboard resonance with multiple resonant modes
        float soundboard_freq1 = 200.0f + resonance * 150.0f;  // Primary mode
        float soundboard_freq2 = 400.0f + resonance * 200.0f;  // Secondary mode
        
        float soundboard_phase1 = i * soundboard_freq1 * 2.0f * juce::MathConstants<float>::pi / (float)sampleRate;
        float soundboard_phase2 = i * soundboard_freq2 * 2.0f * juce::MathConstants<float>::pi / (float)sampleRate;
        
        float soundboard_resonance = (std::sin(soundboard_phase1) * 0.04f + std::sin(soundboard_phase2) * 0.02f) 
                                    * resonance * voice->amplitude;
        
        sample += soundboard_resonance;
        
        output[i] = jlimit(-1.0f, 1.0f, sample * 0.25f);
        
        // Update phases
        voice->phase += phase_increment;
        if (voice->phase >= 2.0f * juce::MathConstants<float>::pi)
        {
            voice->phase -= 2.0f * juce::MathConstants<float>::pi;
        }
        
        // Wrap other phases
        if (voice->resonancePhase >= 2.0f * juce::MathConstants<float>::pi)
            voice->resonancePhase -= 2.0f * juce::MathConstants<float>::pi;
        if (voice->sympatheticPhase >= 2.0f * juce::MathConstants<float>::pi)
            voice->sympatheticPhase -= 2.0f * juce::MathConstants<float>::pi;
    }
}

void PhysicalModelingEngine::generateStringNote(float* output, int numSamples, float frequency, float velocity)
{
    // Get current voice state
    VoiceState* voice = &voices[currentVoiceIndex];
    
    // Enhanced bowed string physical modeling
    const float phase_increment = frequency * 2.0f * juce::MathConstants<float>::pi / (float)sampleRate;
    const float decay_factor = 1.0f - (damping * 0.0003f); // Slower decay for strings
    
    // Initialize voice if needed
    if (!voice->isActive)
    {
        voice->reset();
    }
    
    Random random(Time::currentTimeMillis());
    
    for (int i = 0; i < numSamples; ++i)
    {
        // Generate complex bowed string waveform
        float fundamental = std::sin(voice->phase);
        
        // Rich harmonic series for bowed strings with improved ratios
        float harmonics = 0.0f;
        harmonics += 0.75f * std::sin(voice->phase * 2.0f);  // Strong 2nd harmonic
        harmonics += 0.55f * std::sin(voice->phase * 3.0f);  // 3rd harmonic
        harmonics += 0.42f * std::sin(voice->phase * 4.0f);  // 4th harmonic
        harmonics += 0.32f * std::sin(voice->phase * 5.0f);  // 5th harmonic
        harmonics += 0.24f * std::sin(voice->phase * 6.0f);  // 6th harmonic
        harmonics += 0.18f * std::sin(voice->phase * 7.0f);  // 7th harmonic
        harmonics += 0.14f * std::sin(voice->phase * 8.0f);  // 8th harmonic
        harmonics += 0.10f * std::sin(voice->phase * 9.0f);  // 9th harmonic
        
        float sample = fundamental + harmonics * 0.8f;
        
        // Apply bow pressure with dynamic variations
        float dynamicBowPressure = bowPressure + 0.1f * std::sin(voice->phase * 0.1f);
        sample = std::tanh(sample * dynamicBowPressure * 2.5f) * 0.8f;
        
        // Add bow noise (rosin friction)
        voice->bowNoise = voice->bowNoise * 0.95f + (random.nextFloat() * 2.0f - 1.0f) * 0.05f;
        sample += voice->bowNoise * bowPressure * 0.03f;
        
        // Add string body resonance with improved modeling
        float bodyResonanceFreq = frequency * 0.5f + stringDensity * 100.0f;
        float bodyPhase = i * bodyResonanceFreq * 2.0f * juce::MathConstants<float>::pi / (float)sampleRate;
        sample += std::sin(bodyPhase) * resonance * 0.1f * voice->amplitude;
        
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
        
        sample *= voice->amplitude * velocity * envelope;
        
        // Apply damping
        voice->amplitude *= decay_factor;
        
        output[i] = jlimit(-1.0f, 1.0f, sample * 0.35f);
        
        // Update phase
        voice->phase += phase_increment;
        if (voice->phase >= 2.0f * juce::MathConstants<float>::pi)
            voice->phase -= 2.0f * juce::MathConstants<float>::pi;
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

PhysicalModelingEngine::VoiceState* PhysicalModelingEngine::getNextAvailableVoice()
{
    // First, try to find an inactive voice
    for (int i = 0; i < maxVoices; ++i)
    {
        if (!voices[i].isActive || voices[i].amplitude < 0.001f)
        {
            currentVoiceIndex = i;
            return &voices[i];
        }
    }
    
    // If all voices are active, use round-robin voice stealing
    currentVoiceIndex = (currentVoiceIndex + 1) % maxVoices;
    return &voices[currentVoiceIndex];
}