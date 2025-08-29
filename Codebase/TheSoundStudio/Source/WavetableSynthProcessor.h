/*
  ==============================================================================

    WavetableSynthProcessor.h
    Created: 31 Mar 2020 9:27:07pm
    Author:  Gary Jones

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "Parameters.h"
#include "ChordManager.h"
#include "ADSR2.h"
#include "VotanBuffer.h"
#include "FrequencyManager.h"
#include "SynthesisLibraryManager.h"
#include "WaveStackOsc.h"

#define NUMVOICES 8


class WavetableSynthSound : public SynthesiserSound
{
public:
    WavetableSynthSound() {}
    
    bool appliesToNote (const int /*midiNoteNumber*/)           { return true; }
    bool appliesToChannel (const int /*midiChannel*/)           { return true; }
};

//==============================================================================

class WavetableSynthVoice : public SynthesiserVoice
{
public:
    WavetableSynthVoice(double sampleRate, FrequencyManager * fm);
    virtual ~WavetableSynthVoice();
    
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
        oscillator->setFrequency(currentFrequency / sample_rate);
    }

    WaveTableOsc * oscillator;
    
private:
    
    void process_oscs(float *bufferLeft, float * bufferRight, int numSamples);
    
    void process_adsr(float *bufferLeft, float * bufferRight, int numSamples);
    
    // Instrument-specific synthesis methods
    void synthesizePiano(float* bufferLeft, float* bufferRight, int numSamples);
    void synthesizeFlute(float* bufferLeft, float* bufferRight, int numSamples);
    void synthesizeGuitar(float* bufferLeft, float* bufferRight, int numSamples);
    void synthesizeStrings(float* bufferLeft, float* bufferRight, int numSamples);
    void synthesizeHarp(float* bufferLeft, float* bufferRight, int numSamples);
    
    FrequencyManager * frequencyManager;
    
    
    
    ADSR2 * env[1];

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
    
    // Instrument selection
    int instrumentType; // Based on INSTRUMENTS enum
    
};

// this is the one i need to implement

class WavetableSynthProcessor : public Synthesiser
{
public:
    WavetableSynthProcessor(float hwsamplerate, FrequencyManager * fm);
    
    ~WavetableSynthProcessor();
    
    void prepareToPlay(float hwSampleRate, int blockSize);
    
    void processBlock(AudioBuffer<float>& buffer,
                      MidiBuffer& midiMessages);

    void setParameter(int index, var newValue);
    
    void initBaseWaveType(int type);
    
    double * getWavetableBuffer(int type, int length);
    
    double * getBaseWavetableBuffer();
    
    void setWavetableBuffer(double * table);
    
    void noteOn (const int midiChannel, const int midiNoteNumber, const float frequencyDirect) override;
    
private:
    FrequencyManager * frequencyManager;
    float internal_sample_rate;

    WavetableSynthVoice * cVoices[MAX_NUM_VOICES];
    
    double * baseWavetable;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WavetableSynthProcessor)
};

//=================================================================================
// Frequency Player Wavetable Synth Voice
//=================================================================================

class FrequencyPlayerWavetableSynthVoice : public SynthesiserVoice
{
public:
    FrequencyPlayerWavetableSynthVoice(double sampleRate, FrequencyManager * fm);
    virtual ~FrequencyPlayerWavetableSynthVoice();
    
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
        oscillator->setFrequency(currentFrequency / sample_rate);
    }
    
    float getChoseFrequency()
    {
        return c_chosenFrequency;
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

public: WaveTableOsc * oscillator;
private:
    
    FrequencyManager * frequencyManager;
    
    ADSR2 * env[1];

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
    float insertFrequency;
};

// this is the one i need to implement

class FrequencyPlayerWavetableSynthProcessor : public Synthesiser
{
public:
    FrequencyPlayerWavetableSynthProcessor(float hwsamplerate, FrequencyManager * fm);
    ~FrequencyPlayerWavetableSynthProcessor();
    
    void prepareToPlay(float hwSampleRate, int blockSize);
    void processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages);
    void setParameter(int index, var newValue);
    
    void initBaseWaveType(int type);
    double * getWavetableBuffer(int type, int length);
    double * getBaseWavetableBuffer();
    void setWavetableBuffer(double * table);
    
    double getChosenFrequency()
    {
        return cVoices[0]->getChoseFrequency();
    }
    
private:
    FrequencyManager * frequencyManager;
    float internal_sample_rate;
    FrequencyPlayerWavetableSynthVoice * cVoices[MAX_NUM_VOICES];
    double * baseWavetable;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FrequencyPlayerWavetableSynthProcessor)
};


// Frequency Sanner Wavetable Synth

class FrequencyScannerWavetableSynthVoice : public SynthesiserVoice
{
public:
    FrequencyScannerWavetableSynthVoice(double sampleRate, FrequencyManager * fm);
    virtual ~FrequencyScannerWavetableSynthVoice();
    
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
        oscillator->setFrequency(currentFrequency / sample_rate);
    }

    WaveTableOsc * oscillator;
    
private:
    
    void process_oscs(float *bufferLeft, float * bufferRight, int numSamples);
    
    void process_adsr(float *bufferLeft, float * bufferRight, int numSamples);
    
    FrequencyManager * frequencyManager;
    
    
    
    ADSR2 * env[1];

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

class FrequencyScannerWavetableSynthProcessor : public Synthesiser
{
public:
    FrequencyScannerWavetableSynthProcessor(float hwsamplerate, FrequencyManager * fm);
    
    ~FrequencyScannerWavetableSynthProcessor();
    
    void prepareToPlay(float hwSampleRate, int blockSize);
    void processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages);
    void setParameter(int index, var newValue);
    
    void initBaseWaveType(int type);
    double * getWavetableBuffer(int type, int length);
    double * getBaseWavetableBuffer();
    void setWavetableBuffer(double * table);
    
    void noteOn (const int midiChannel, const int midiNoteNumber, const float frequencyDirect) override;
    
private:
    FrequencyManager * frequencyManager;
    float internal_sample_rate;

    FrequencyScannerWavetableSynthVoice * cVoices[MAX_NUM_VOICES];
    
    double * baseWavetable;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FrequencyScannerWavetableSynthProcessor)
};
