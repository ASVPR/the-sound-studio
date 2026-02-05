/*
  ==============================================================================

    SynthesisProcessor.h
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "Oscillators.h"
#include "Parameters.h"
#include "ChordManager.h"
#include "ADSR2.h"
#include "VAOscillator.h"
#include "FrequencyManager.h"


// New Rewrite of the Synth Voice



//class SynthesisProcessorVoice2 : public SynthesiserVoice
//{
//public:
//    SynthesisProcessorVoice2(FrequencyManager * fm, int ref);
//    virtual ~SynthesisProcessorVoice2();
//    
//    //    void setCurrentPlaybackSampleRate (double newRate) override;
//    
//    void setOversamplingFactor(int newFactor);
//    
//    void renderNextBlock (AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;
//    
//    float process_adsr();
//    
//    void startNote (int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition)override;
//    void stopNote (float velocity, bool allowTailOff) override;
//    bool canPlaySound (SynthesiserSound*sound)override;
//    void pitchWheelMoved (int newPitchWheelValue) override;
//    void controllerMoved (int controllerNumber, int newControllerValue)override;
//    
//    void setParameter(int index, var newValue);
//    
//    void debugParameters();
//    
//private:
//    FrequencyManager * frequencyManager;
//    
////    DigitalVAOSC * osc;
//    ADSR2 * envNew;
//    
//    
//    
//    
//    float amplitude;
//    
//    enum WAVEFORM_TYPES
//    {
//        DEFAULT = 0, SINE,
//        TRIANGLE, SQUARE, SAWTOOTH,
//        TOTAL_NUM_WAVEFORM_TYPES
//    }
//    waveformType;
//    
//    bool chordSource; // 0 = chordSelect / 1 = insertFrequency
//    int oversampleFactor;
//    int octaveShift;
//    float insertFrequency;
//    bool manipulateChoseFrequency;
//    bool multiplyOrDivision; // mult = 0, div = 1
//    float multiplyValue;
//    float divisionValue;
//    
//    int voiceRef;
//};


class SynthSound : public SynthesiserSound
{
public:
    SynthSound(){}
    
    bool appliesToNote (const int midiNoteNumber ) { return true; }
    bool appliesToChannel (const int midiChannel ) { return true; }
};


class SynthesisProcessorVoice : public SynthesiserVoice
{
public:
    SynthesisProcessorVoice(FrequencyManager * fm, int ref);
    virtual ~SynthesisProcessorVoice();
    
//    void setCurrentPlaybackSampleRate (double newRate) override;
    
    void setOversamplingFactor(int newFactor);
    
    void renderNextBlock (AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;
    
    float process_adsr();
    
    void startNote (int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition)override;
    
    void stopNote (float velocity, bool allowTailOff) override;
    
    bool canPlaySound (SynthesiserSound*sound)override;
    
    void pitchWheelMoved (int newPitchWheelValue) override;
    void controllerMoved (int controllerNumber, int newControllerValue)override;
    
    void setParameter(int index, var newValue);
    
    void debugParameters();
    
private:
    FrequencyManager * frequencyManager;
    
    Osc  * osc;
    
//    CQBLimitedOscillator * newOsc;
    
    juce::ADSR * envelope;
    juce::ADSR::Parameters envParams; // local copy of env
    
    ADSR2 * envNew;
    
    
    
    
    float amplitude;
    
    enum WAVEFORM_TYPES
    {
        DEFAULT = 0, SINE,
        TRIANGLE, SQUARE, SAWTOOTH,
        TOTAL_NUM_WAVEFORM_TYPES
    }
    waveformType;

    bool chordSource; // 0 = chordSelect / 1 = insertFrequency
    
    int oversampleFactor;
    int octaveShift;
    float insertFrequency;
    bool manipulateChoseFrequency;
    bool multiplyOrDivision; // mult = 0, div = 1
    float multiplyValue;
    float divisionValue;
    
    int voiceRef;
};

class SynthesisProcessor : public Synthesiser
{
public:
    SynthesisProcessor(FrequencyManager * fm, double initialSampleRate);
    
    ~SynthesisProcessor();
    
    void processBlock(AudioBuffer<float>& buffer,
                      MidiBuffer& midiMessages);
    
//    void prepareToPlay(float sampleR, int blockSize);
    
    void setOversamplingFactor(int newFactor);
    
    void setParameter(int index, var newValue);
    
private:
    
    bool mute = false;
    
    SynthesisProcessorVoice * voices[MAX_NUM_VOICES];
    
    FrequencyManager * frequencyManager;
    
    
    // need overrcing central pitch to calculate all frequencies from..
};

// Fix long note bug
// check envelope parameters
// fix central pitch which is close
// check pitch along side sinewave

