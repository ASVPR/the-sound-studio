/*
  ==============================================================================

    VotanSynthProcessor.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/


#pragma once
#ifndef VOTANSYNTHPROCESSOR_H_INCLUDED
#define VOTANSYNTHPROCESSOR_H_INCLUDED

#include "math.h"
#include <stdio.h>
#include "JuceHeader.h"
#include "ADSR2.h"
#include "OSCManager.h"
#include "Parameters.h"
#include "VotanBuffer.h"
#include "ChordManager.h"
#include "FrequencyManager.h"



// New Juce Class
#define NUMVOICES 8


class ChordSynthSound : public SynthesiserSound
{
public:
    ChordSynthSound() {}
    
    bool appliesToNote (const int /*midiNoteNumber*/)           { return true; }
    bool appliesToChannel (const int /*midiChannel*/)           { return true; }
};

//==============================================================================

class ChordSynthVoice : public SynthesiserVoice
{
public:
    ChordSynthVoice(double sampleRate, FrequencyManager * fm);
    virtual ~ChordSynthVoice();
    
    void setCurrentPlaybackSampleRate (double newRate) override;
    bool canPlaySound (SynthesiserSound* sound)override;
    void startNote (int midiNoteNumber, float velocity, SynthesiserSound* sound,  int currentPitchWheelPosition) override;
    void stopNote (float velocity, bool allowTailOff) override;
    void pitchWheelMoved (int val) override {}
    void controllerMoved (int /*controllerNumber*/, int /*newValue*/) override{}
    void renderNextBlock (AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;
    void ProcessBuffer(float *bufferLeft, float * bufferRight, int buffersize); // internal proc
    void setParameter(int index, var newValue);
    
    void set_oversample_factor(int newFactor) { oversample_factor = newFactor;}
    void reset();
    void loadTableFromFile(File * f, int idx){}
    void setVoiceRef(int ref){voiceRef = ref;}

    void setFrequencyDirect(float newFreq)
    {
        currentFrequency = newFreq;
        oscillator[0]->set_frequency(newFreq);
    }
    
    void setPhase(double newPhase)
    {
        oscillator[0]->setPhase(newPhase);
    }

private:
    
    void process_oscs(float *bufferLeft, float * bufferRight, int numSamples);
    void process_adsr(float *bufferLeft, float * bufferRight, int numSamples);
    
    FrequencyManager * frequencyManager;
    OSCManager2 * oscillator[1];
    ADSR2 * env[1];

    CVotanBuffer m_bufferAlloc[12];
    CVotanBuffer m_bufferOversampling;
    
    int     voiceRef;
    int     oversample_factor = 1;
    double  sample_rate;
    
    // member parameters
    // osc
    float   osc_volume[1];
    
    // new params
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
    float octaveShift;
    float insertFrequency;
    bool manipulateChoseFrequency;
    bool multiplyOrDivision; // mult = 0, div = 1
    float multiplyValue;
    float divisionValue;
    
    float currentFrequency;
    
};

// this is the one i need to implement

class ChordSynthProcessor : public Synthesiser
{
public:
    ChordSynthProcessor(float hwsamplerate, FrequencyManager * fm)
    {
        internal_sample_rate    = hwsamplerate * OVERSAMPLE_FACTOR;
        frequencyManager        = fm;
        
        clearVoices();

        for (int i = 0; i < MAX_NUM_VOICES; i++)
        {
            cVoices[i] = new ChordSynthVoice(internal_sample_rate, frequencyManager);
            cVoices[i]->setVoiceRef(i);
            this->addVoice(cVoices[i]);
        }
        
        clearSounds();
        
        addSound (new ChordSynthSound());
        setNoteStealingEnabled(true);
        
    }
    ~ChordSynthProcessor() { }
    
    void prepareToPlay(float hwSampleRate, int blockSize)
    {
        internal_sample_rate = hwSampleRate * OVERSAMPLE_FACTOR;

        setCurrentPlaybackSampleRate(internal_sample_rate);
    }
    
    void processBlock(AudioBuffer<float>& buffer,
                      MidiBuffer& midiMessages)
    {
        renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    }

    void setParameter(int index, var newValue)
    {
        for (int i = 0; i < MAX_NUM_VOICES; i++) {
            cVoices[i]->setParameter(index, newValue);
        }
    }
    
    void setPhase(double newPhase)
    {
        for (int i = 0; i < 1; i++) {
            cVoices[i]->setPhase(newPhase);
        }
    }

    //===============================================================
    // in order for new chord system to work, we need to override this method pushing frequencies direct to the oscillator
    //---------------------------------------------------------------
    void noteOn (const int midiChannel, const int midiNoteNumber, const float frequencyDirect) override
    {
        const ScopedLock sl (lock);

        for (auto* sound : sounds)
        {
            if (sound->appliesToNote (midiNoteNumber) && sound->appliesToChannel (midiChannel))
            {
                // If hitting a note that's still ringing, stop it first (it could be
                // still playing because of the sustain or sostenuto pedal).
                for (auto* voice : voices)
                    if (voice->getCurrentlyPlayingNote() == midiNoteNumber && voice->isPlayingChannel (midiChannel))
                        stopVoice (voice, 1.0f, true);

                ChordSynthVoice * v = (ChordSynthVoice*)findFreeVoice (sound, midiChannel, midiNoteNumber, false);
                
                if (v != nullptr)
                {
                    v->setFrequencyDirect(frequencyDirect);
                    startVoice (v, sound, midiChannel, midiNoteNumber, 1.0);
                }
                
            }
        }
    }
    
private:
    FrequencyManager * frequencyManager;
    float internal_sample_rate;

    ChordSynthVoice * cVoices[MAX_NUM_VOICES];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChordSynthProcessor)
};


//=================================================================================
// Frequency Player Synth Voice
//=================================================================================

class FrequencySynthVoice : public SynthesiserVoice
{
public:
    FrequencySynthVoice(double sampleRate, FrequencyManager * fm);
    virtual ~FrequencySynthVoice();
    
    void setCurrentPlaybackSampleRate (double newRate) override;
    bool canPlaySound (SynthesiserSound* sound)override;
    void startNote (int midiNoteNumber, float velocity, SynthesiserSound* sound,  int currentPitchWheelPosition) override;
    void stopNote (float velocity, bool allowTailOff) override;
    void pitchWheelMoved (int val) override {}
    void controllerMoved (int /*controllerNumber*/, int /*newValue*/) override{}
    void renderNextBlock (AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;
    void ProcessBuffer(float *bufferLeft, float * bufferRight, int buffersize); // internal proc
    void setParameter(int index, var newValue);
    
    void set_oversample_factor(int newFactor) { oversample_factor = newFactor;}
    void reset();
    void loadTableFromFile(File * f, int idx){}
    void setVoiceRef(int ref){voiceRef = ref;}
    
    float getChoseFrequency()
    {
        return c_chosenFrequency;
    }
    
    void setPhase(double newPhase)
    {
        oscillator[0]->setPhase(newPhase);
    }
    
private:
    
    void process_oscs(float *bufferLeft, float * bufferRight, int numSamples);
    void process_adsr(float *bufferLeft, float * bufferRight, int numSamples);
    
    // params
    bool frequencySource; // 0 = frequency / 1 = range
    int oversampleFactor;
    int octaveShift;
    
    // params
    bool manipulateChoseFrequency;
    bool multiplyOrDivision; // mult = 0, div = 1
    float multiplyValue;
    float divisionValue;
    
    float p_chosenFrequency;
    float p_rangeMin;
    float p_rangeMax;
    
    int rangeLengthMS;
    bool rangeLog;
    
    // recalculated parameters after manipulate frequencies
    float c_chosenFrequency;
    float c_rangeMin;
    float c_rangeMax;
    
    // internal variables
    int rangeLengthInSamples;
    bool shouldProcessRangeSweep;
    float currentFrequency;
    float frequencyIncrementPerSample;
    float minMaxDifference;
    int sampleInc;

    // sweep functions
    void calcRangeSweep();
    void processRangeSweep(int bufferLength);
    void calcManipulateFrequencies();
    
private:
    FrequencyManager * frequencyManager;
    OSCManager2 * oscillator[1];
    ADSR2 * env[1];
    
    CVotanBuffer m_bufferAlloc[12];
    CVotanBuffer m_bufferOversampling;
    
    int     voiceRef;
    int     oversample_factor = 1;
    double  sample_rate;
    
    // member parameters
    // osc
    float   osc_volume[1];
    
    // new params
    float amplitude;
    
    enum WAVEFORM_TYPES
    {
        DEFAULT = 0, SINE,
        TRIANGLE, SQUARE, SAWTOOTH,
        TOTAL_NUM_WAVEFORM_TYPES
    }
    waveformType;

};

//=================================================================================
// Frequency Synth Proc
//=================================================================================
class FrequencySynthProcessor : public Synthesiser
{
public:
    // Mono synth...
    FrequencySynthProcessor(float hwsamplerate, FrequencyManager * fm)
    {
        internal_sample_rate    = hwsamplerate * OVERSAMPLE_FACTOR;
        frequencyManager        = fm;
        
        clearVoices();
        
        for (int i = 0; i < 1; i++) {
            cVoices[i] = new FrequencySynthVoice(internal_sample_rate, frequencyManager);
            cVoices[i]->setVoiceRef(i);
            this->addVoice(cVoices[i]);
        }
        
        clearSounds();
        
        addSound (new ChordSynthSound());
        setNoteStealingEnabled(true);
    }
    
    ~FrequencySynthProcessor() { }
    
    void prepareToPlay(float hwSampleRate, int blockSize)
    {
        internal_sample_rate = hwSampleRate * OVERSAMPLE_FACTOR;

        setCurrentPlaybackSampleRate(internal_sample_rate);
    }
    
    void processBlock(AudioBuffer<float>& buffer,
                      MidiBuffer& midiMessages)
    {
        renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    }
    
    void setParameter(int index, var newValue)
    {
        for (int i = 0; i < 1; i++) {
            cVoices[i]->setParameter(index, newValue);
        }
    }
    
    void setPhase(double newPhase)
    {
        for (int i = 0; i < 1; i++) {
            cVoices[i]->setPhase(newPhase);
        }
    }
    
    double getChosenFrequency()
    {
        return cVoices[0]->getChoseFrequency();
    }
    
    //===============================================================
    
private:
    FrequencyManager * frequencyManager;
    float internal_sample_rate;
    
    FrequencySynthVoice * cVoices[1];
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FrequencySynthProcessor)
};


//=================================================================================
// Frequency Scanner Synth Voice
//=================================================================================

class FrequencyScannerSynthVoice : public SynthesiserVoice
{
public:
    FrequencyScannerSynthVoice(double sampleRate, FrequencyManager * fm);
    virtual ~FrequencyScannerSynthVoice();
    
    void setCurrentPlaybackSampleRate (double newRate) override;
    bool canPlaySound (SynthesiserSound* sound)override;
    void startNote (int midiNoteNumber, float velocity, SynthesiserSound* sound,  int currentPitchWheelPosition) override;
    void stopNote (float velocity, bool allowTailOff) override;
    void pitchWheelMoved (int val) override {}
    void controllerMoved (int /*controllerNumber*/, int /*newValue*/) override{}
    void renderNextBlock (AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;
    void ProcessBuffer(float *bufferLeft, float * bufferRight, int buffersize); // internal proc
    void setParameter(int index, var newValue);
    
    void set_oversample_factor(int newFactor) { oversample_factor = newFactor;}
    void reset();
    void loadTableFromFile(File * f, int idx){}
    void setVoiceRef(int ref){voiceRef = ref;}
    
    void setFrequencyDirect(float newFreq)
    {
        currentFrequency = newFreq;
        oscillator[0]->set_frequency(newFreq);
    }
    
private:
    
    void process_oscs(float *bufferLeft, float * bufferRight, int numSamples);
    void process_adsr(float *bufferLeft, float * bufferRight, int numSamples);
    
    float currentFrequency;
    
private:
    FrequencyManager * frequencyManager;
    OSCManager2 * oscillator[1];
    ADSR2 * env[1];
    
    CVotanBuffer m_bufferAlloc[12];
    CVotanBuffer m_bufferOversampling;
    
    int     voiceRef;
    int     oversample_factor = 1;
    double  sample_rate;
    
    // member parameters
    // osc
    float   osc_volume[1];
    
    // new params
    float amplitude;
    
    enum WAVEFORM_TYPES
    {
        DEFAULT = 0, SINE,
        TRIANGLE, SQUARE, SAWTOOTH,
        TOTAL_NUM_WAVEFORM_TYPES
    }
    waveformType;

};

//=================================================================================
// Frequency Scanner Synth Proc
//=================================================================================
class FrequencyScannerSynthProcessor : public Synthesiser
{
public:
    // Mono synth...
    FrequencyScannerSynthProcessor(float hwsamplerate, FrequencyManager * fm)
    {
        internal_sample_rate    = hwsamplerate * OVERSAMPLE_FACTOR;
        frequencyManager        = fm;
        
        clearVoices();
        
        for (int i = 0; i < MAX_NUM_VOICES; i++) {
            cVoices[i] = new FrequencyScannerSynthVoice(internal_sample_rate, frequencyManager);
            cVoices[i]->setVoiceRef(i);
            this->addVoice(cVoices[i]);
        }
        
        clearSounds();
        
        addSound (new ChordSynthSound());
        setNoteStealingEnabled(true);
    }
    
    ~FrequencyScannerSynthProcessor() { }
    
    void prepareToPlay(float hwSampleRate, int blockSize)
    {
        internal_sample_rate = hwSampleRate * OVERSAMPLE_FACTOR;

        setCurrentPlaybackSampleRate(internal_sample_rate);
    }
    
    void processBlock(AudioBuffer<float>& buffer,
                      MidiBuffer& midiMessages)
    {
        renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    }
    
    void setParameter(int index, var newValue)
    {
        for (int i = 0; i < MAX_NUM_VOICES; i++) {
            cVoices[i]->setParameter(index, newValue);
        }
    }
    
    void noteOn (const int midiChannel, const int midiNoteNumber, const float frequencyDirect) override
    {
        const ScopedLock sl (lock);

        for (auto* sound : sounds)
        {
            if (sound->appliesToNote (midiNoteNumber) && sound->appliesToChannel (midiChannel))
            {
                // If hitting a note that's still ringing, stop it first (it could be
                // still playing because of the sustain or sostenuto pedal).
                for (auto* voice : voices)
                    if (voice->getCurrentlyPlayingNote() == midiNoteNumber && voice->isPlayingChannel (midiChannel))
                        stopVoice (voice, 1.0f, true);

                FrequencyScannerSynthVoice * v = (FrequencyScannerSynthVoice*)findFreeVoice (sound, midiChannel, midiNoteNumber, false);
                v->setFrequencyDirect(frequencyDirect);
                startVoice (v, sound, midiChannel, midiNoteNumber, 1.0);
            }
        }
    }
    
    //===============================================================
    
private:
    FrequencyManager * frequencyManager;
    float internal_sample_rate;
    
    FrequencyScannerSynthVoice * cVoices[MAX_NUM_VOICES];
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FrequencyScannerSynthProcessor)
};

#endif  // VOTANSYNTHPROCESSOR_H_INCLUDED
