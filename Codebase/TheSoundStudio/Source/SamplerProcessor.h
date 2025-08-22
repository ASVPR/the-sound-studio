/*
  ==============================================================================

    SamplerProcessor.h
    Created: 29 Mar 2019 12:58:59pm
    Author:  Gary Jones

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "Parameters.h"
#include "ChordManager.h"
#include "ADSR2.h"
#include "FrequencyManager.h"
#include "SynthesisLibraryManager.h"

// pure sampler, for playing pianos etc..
class SynthSamplerSound    : public SynthesiserSound
{
public:
//    SynthSamplerSound (const String& name,
//                       AudioFormatReader& source,
//                       const BigInteger& midiNotes,
//                       int midiNoteForNormalPitch,
//                       double attackTimeSecs,
//                       double releaseTimeSecs,
//                       double maxSampleLengthSeconds);
    
    SynthSamplerSound (const String& name, AudioFormatReader& source, const BigInteger& midiNotes, int midiNoteForNormalPitch, float centralPitch, Range<float> range, double maxSampleLengthSeconds);
    
    ~SynthSamplerSound();
    const String& getName() const noexcept;
    AudioBuffer<float>* getAudioData() const noexcept;
    //==============================================================================
    bool appliesToNote (int midiNoteNumber) override;
    bool appliesToFrequency(float frequency);
    bool appliesToChannel (int midiChannel) override;
    
    //==============================================================================
    friend class SamplerVoice;
    
    String name;
    std::unique_ptr<AudioBuffer<float>> data;
    double sourceSampleRate;
    BigInteger midiNotes;
    int length, attackSamples, releaseSamples;
    int midiRootNote;
    
    float noteFrequency; // fixed note frequency 440 / semitones
    
    Range<float> frequencyRange;

    JUCE_LEAK_DETECTOR (SynthSamplerSound)
};

class SamplerProcessorVoice : public SynthesiserVoice
{
public:
    SamplerProcessorVoice(FrequencyManager * fm, int ref, int shortcut);
    ~SamplerProcessorVoice();
    
    virtual void setCurrentPlaybackSampleRate (double newRate) override;
    //==============================================================================
    bool canPlaySound (SynthesiserSound*sound) override;
    void calculatePitchRatio(int midiNoteNumber, const SynthSamplerSound*  sound);
    void startNote (int midiNoteNumber, float velocity, SynthesiserSound*s, int pitchWheel) override;
    void stopNote (float velocity, bool allowTailOff)override;
    void pitchWheelMoved (int newValue)override;
    void controllerMoved (int controllerNumber, int newValue)override;
    void renderNextBlock (AudioBuffer<float>&outputBuffer, int startSample, int numSamples)override;
    void process_adsr(float *bufferLeft, float * bufferRight, int buffersize);
    void setParameter(int index, var newValue);
    
    void setFrequencyDirect(float newFreq) { currentFrequency = newFreq; }

private:
    FrequencyManager * frequencyManager;
    int shortcutRef;
    
    float currentFrequency;
    
    double pitchRatio;
    double sourceSamplePosition;
    float lgain, rgain, attackReleaseLevel, attackDelta, releaseDelta;
    bool isInAttack, isInRelease;
    
    bool isBassSpread;

    ADSR2 * envelope;
    
    float amplitude;

//    bool chordSource; // 0 = chordSelect // 1 = insertFrequency
    int oversampleFactor;
//    int octaveShift;
//    float insertFrequency;
//    bool manipulateChoseFrequency;
//    bool multiplyOrDivision; // mult = 0, div = 1
//    float multiplyValue;
//    float divisionValue;
    double centralFrequency = 432;
    double oscFrequency;
    double sampleRate;

    int voiceRef;
};

class SamplerProcessor: public Synthesiser
{
public:
    SamplerProcessor(FrequencyManager *fm, SynthesisLibraryManager * slm, double initialSampleRate, int shortcut);
    ~SamplerProcessor();
    
    void processBlock(AudioBuffer<float>& buffer,
                      MidiBuffer& midiMessages);
    
    void setParameter(int index, var newValue);
//    void loadNewSamplerInstrument(int midiNote);
    void removeCurrentInstrument();
    void setCentralFrequencyToSounds();

    int64 lengthInSamples;
    AudioFormatManager formatManager;
    
    enum
    {
        maxMidiChannel = 16,
        maxNumberOfVoices = MAX_NUM_VOICES
    };
    
    void loadDefaultPreset()
    {
        
    }
    
    //===============================================================
    // in order for new chord system to work, we need to override this method pushing frequencies direct to the oscillator
    //---------------------------------------------------------------
    void noteOn (const int midiChannel, const int midiNoteNumber, const float frequencyDirect) override;
    
    
//    void loadSampleIntoPlayerNew (String filePath);
//    void loadSampleIntoPlayer (int filePathIndex);
//
    void loadInstrument(int filePathIndex); // used for new playing instruments high res

private:
    int shortcutRef;
    FrequencyManager * frequencyManager;
    SynthesisLibraryManager * sampleLibraryManager;
};


