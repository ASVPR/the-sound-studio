/*
  ==============================================================================

    LissajousCurveProcessor.h
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#pragma once

// needs to rebuild from chordplayer processor code
// adding callbacks form the audio buffer
// new parameter structure
// what about the axis parameters ? in component or in processing class ??
// prob best to keep all i nthe
// or place chordplayer inside this, and act as an interface..
// chirdplayer has shortcut, so would only use one !


#include "SamplerProcessor.h"
#include "VotanSynthProcessor.h"
#include "WavetableSynthProcessor.h"

class LissajousFrequencyPlayerProcessor : public AudioProcessor
{
public:
    LissajousFrequencyPlayerProcessor(FrequencyManager * fm, SynthesisLibraryManager * slm);
    ~LissajousFrequencyPlayerProcessor();
    
    virtual void prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock) override;
    
    void setParameter(int synthRef, int index, var newValue);
    
    void setPhase(double newPhase)
    {
        synth->setPhase(newPhase);
    }
    
    virtual void processBlock (AudioBuffer<float>& buffer,
                               MidiBuffer& midiMessages) override;
    
    void triggerKeyDown();
    void triggerKeyUp();
    void triggerNoteOn();
    void triggerNoteOff();
    
    void panic();
    
    virtual double getTailLengthSeconds() const override { return 0.0; }
    virtual bool acceptsMidi() const override { return true; }
    virtual bool producesMidi() const override { return false; }
    virtual void releaseResources()override { }
    virtual AudioProcessorEditor* createEditor() override {  return 0; }
    virtual bool hasEditor() const override {  return false; }
    virtual const String getName() const  override { return "FrequencyPlayerProcessor"; }
    virtual int getNumPrograms()override { return 0; }
    virtual int getCurrentProgram() override { return 0; }
    virtual void setCurrentProgram (int index) override { }
    virtual const String getProgramName (int index) override { return ""; }
    virtual void changeProgramName (int index, const String& newName) override { }
    virtual void getStateInformation (juce::MemoryBlock& destData) override {}
    virtual void setStateInformation (const void* data, int sizeInBytes)override {}
    
    
    void setPlayerCommand(PLAYER_COMMANDS command, bool freeFlow);
    void setPlayerPlayMode(PLAY_MODE mode);
    
    FrequencySynthProcessor *  synth;
    FrequencyPlayerWavetableSynthProcessor * wavetableSynth;
    
    enum WAVEFORM_TYPES { DEFAULT = 0, SINE, TRI, SQR, SAW, WAVETABLE } waveformType;
    
    PLAY_STATE getPlaystate() { return playState; }
    
private:
    bool samplerOrSynth; // switched between processing of Sampler of Synth
    
    FrequencyManager * frequencyManager;
    SynthesisLibraryManager * sampleLibraryManager;
    
    bool isActive;
    bool shouldMute;
    
    PLAY_MODE  playMode;
    PLAY_STATE playState;
    bool       shouldRecord;
    
    //=============================================================
    // Play Repeater
    //=============================================================
public:
    class PlayRepeater {
        
    public:
        PlayRepeater(LissajousFrequencyPlayerProcessor * cp, double sr);
        ~PlayRepeater();
        
        void prepareToPlay(double newSampleRate);
        void tickBuffer(int numSamples);
        void play();
        void stop();
        
    private:
        void calculatePlaybackTimers();
        
        void processShortcut(int shortcutRef, int sampleRef);
        void processSimultaneousShortcuts(int firstShortcutRef, int sampleRef);
        
        void triggerOnEvent(int shortcutRef);
        void triggerOffEvent(int shortcutRef);
        void allNotesOff();
        void clearOpenRepeats();
        void resetTick();
        
        void prepareNextShortcut();
        
    public:
        void setIsActive(int shortcutRef, bool should);
        void calculateLengths(int shortcutRef);
        void setNumRepeats(int shortcutRef, int num);
        void setPauseMS(int shortcutRef, int ms);
        void setLengthMS(int shortcutRef, int ms);
        void setPlayMode(PLAY_MODE mode);
        void setPlaySimultaneous(bool should);
        
        int getTotalMSOfLoop();
        int getCurrentMSInLoop();
        float getProgressBarValue();
        String getTimeRemainingInSecondsString();
        
    private:
        
        LissajousFrequencyPlayerProcessor * proc;
        
        PLAY_MODE playMode;
        PLAY_STATE playState;
        
        bool playSimultaneous;
        
        int currentPlayingShortcut = 0;
        int sampleCounter;
        int numSamplesPerMS;
        double sampleRate;
        
        bool shouldProcess;
        
        // params
        bool isActive;
        int numRepeats;
        int lengthMS;
        int lengthInSamples;
        int pauseMS;
        int pauseInSamples;
        int totalNumMsOfLoop;
        int totalMSOfLoop;
        uint64 totalNumSamplesOfLoop;
        int currentMS;
        
        
        void processSecondsClock();
        
        // events
        int nextNoteOnEvent;  // sampleRef for next note on
        int nextNoteOffEvent; // sampleRef for next note off
        
        int currentRepeat;    // current playing repeat, increments for each
        bool isPaused;
    } ;
    
    PlayRepeater * repeater;
};

class LissajousChordPlayerProcessor : public AudioProcessor
{
public:
    LissajousChordPlayerProcessor(FrequencyManager * fm, SynthesisLibraryManager * slm);
    ~LissajousChordPlayerProcessor();
    
    virtual void prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock) override;
    
    void setParameter(int synthRef, int index, var newValue);
    
    void setPhase(double newPhase)
    {
        synth->setPhase(newPhase);
    }
    
    void setOversamplingFactor(int newFactor);
    
    virtual void processBlock (AudioBuffer<float>& buffer,
                               MidiBuffer& midiMessages) override;

    
    void triggerKeyDown(int shortcutRef);
    void triggerKeyUp(int shortcutRef);
    void triggerNoteOn(int shortcutRef);
    void triggerNoteOff(int shortcutRef);
    
    void panic();
    
    virtual double getTailLengthSeconds() const override { return 0.0; }
    virtual bool acceptsMidi() const override { return true; }
    virtual bool producesMidi() const override { return false; }
    virtual void releaseResources()override { }
    virtual AudioProcessorEditor* createEditor() override {  return 0; }
    virtual bool hasEditor() const override {  return false; }
    virtual const String getName() const  override { return "ChordPlayerProcessor"; }
    virtual int getNumPrograms()override { return 0; }
    virtual int getCurrentProgram() override { return 0; }
    virtual void setCurrentProgram (int index) override { }
    virtual const String getProgramName (int index) override { return ""; }
    virtual void changeProgramName (int index, const String& newName) override { }
    virtual void getStateInformation (juce::MemoryBlock& destData) override {}
    virtual void setStateInformation (const void* data, int sizeInBytes)override {}

    void setActiveShortcutSynth(int synthRef, bool shouldBeActive);
    
    void setPlayerCommand(PLAYER_COMMANDS command, bool freeFlow);
    void setPlayerPlayMode(PLAY_MODE mode);
    
    ChordSynthProcessor *   synth;
    SamplerProcessor *      sampler;
    WavetableSynthProcessor * wavetableSynth;
    
    ChordManager *          chordManager;
    
    enum WAVEFORM_TYPES
    {
        SAMPLER = 0, SINE, TRI, SQR, SAW, WAVETABLE
    }
    waveformType ;
    
    PLAY_STATE getPlaystate() { return playState; }
    
private:
    bool samplerOrSynth; // switched between processing of Sampler of Synth
    
    bool chordSource;
    int octaveShift;
    float insertFrequency;
    bool manipulateChoseFrequency;
    bool multiplyOrDivision; // mult = 0, div = 1
    float multiplyValue;
    float divisionValue;

    
    FrequencyManager * frequencyManager;
    SynthesisLibraryManager * sampleLibraryManager;

    bool isActive;
    bool shouldMute;
    
    PLAY_MODE  playMode;
    PLAY_STATE playState;
    bool       shouldRecord;
    
    //=============================================================
    // Play Repeater
    //=============================================================
public:
    class PlayRepeater {
        
    public:
        PlayRepeater(LissajousChordPlayerProcessor * cp, double sr);
        ~PlayRepeater();
        
        void prepareToPlay(double newSampleRate);
        void tickBuffer(int numSamples);
        void play();
        void stop();
        
    private:
        void calculatePlaybackTimers();
        
        void processShortcut(int shortcutRef, int sampleRef);
        void processSimultaneousShortcuts(int firstShortcutRef, int sampleRef);
        
        void triggerOnEvent(int shortcutRef);
        void triggerOffEvent(int shortcutRef);
        void allNotesOff();
        void clearOpenRepeats();
        void resetTick();
        
        void prepareNextShortcut();
        
    public:
        void setIsActive(int shortcutRef, bool should);
        void calculateLengths(int shortcutRef);
        void setNumRepeats(int shortcutRef, int num);
        void setPauseMS(int shortcutRef, int ms);
        void setLengthMS(int shortcutRef, int ms);
        void setPlayMode(PLAY_MODE mode);
        void setPlaySimultaneous(bool should);
        
        int getTotalMSOfLoop();
        int getCurrentMSInLoop();
        float getProgressBarValue();
        String getTimeRemainingInSecondsString();
        
    private:
        
        LissajousChordPlayerProcessor * proc;
        
        PLAY_MODE playMode;
        PLAY_STATE playState;
        
        bool playSimultaneous;
        
        int currentPlayingShortcut = 0;
        int sampleCounter;
        int numSamplesPerMS;
        double sampleRate;
        
        bool shouldProcess;
        
        // params
        bool isActive;
        int numRepeats;
        int lengthMS;
        int lengthInSamples;
        int pauseMS;
        int pauseInSamples;
        int totalNumMsOfLoop;
        int totalMSOfLoop;
        uint64 totalNumSamplesOfLoop;
        int currentMS;
        
        
        void processSecondsClock();
        
        // events
        int nextNoteOnEvent;  // sampleRef for next note on
        int nextNoteOffEvent; // sampleRef for next note off
        
        int currentRepeat;    // current playing repeat, increments for each
        bool isPaused;
    } ;
    
    PlayRepeater * repeater;
    
};


class LissajousProcessor
{
public:
    LissajousProcessor(FrequencyManager * fm, SynthesisLibraryManager * slm)
    {
        frequencyManager        = fm;
        sampleLibraryManager    = slm;
        sample_rate             = 44100;
        
        for (int i = 0; i < 3; i++)
        {
            freqPlayer[i]               = new LissajousFrequencyPlayerProcessor(frequencyManager, sampleLibraryManager); // x
            chordPlayer[i]              = new LissajousChordPlayerProcessor(frequencyManager, sampleLibraryManager); // x
            
            freqOrChord[i]              = false;
            freeFlowMode[i]             = false;
            
            phase[i]                    = 0.f;
            amplitude[i]                = 1.f;
        }

        setRefreshRate(33);
    }
    
    virtual ~LissajousProcessor() { }
    
    void prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock)
    {
        sample_rate = sampleRate;
        
        for (int axis = 0; axis < 3; axis++)
        {
            freqPlayer[axis]->prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);
            chordPlayer[axis]->prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);
            
            processorOutputBuffers[axis].clear();
            processorOutputBuffers[axis].setSize(1, maximumExpectedSamplesPerBlock);
            
        }
        
        // ring buffer
        
        visualiserBufferSize = (int)sample_rate / refreshRate;
        
        visualiserRingBuffer.clear();
        visualiserRingBuffer.setSize(3, visualiserBufferSize);
        
        visualiserBufferCounter = 0;
    }
    
    void setParameter(int index, var newValue)
    {
        if      (index == UNIT_1_PROCESSOR_TYPE)    { freqOrChord[0] = newValue.operator bool(); }
        else if (index == UNIT_2_PROCESSOR_TYPE)    { freqOrChord[1] = newValue.operator bool(); }
        else if (index == UNIT_3_PROCESSOR_TYPE)    { freqOrChord[2] = newValue.operator bool(); }
        
        else if (index == UNIT_1_FREE_FLOW)         { freeFlowMode[0] = newValue.operator bool(); }
        else if (index == UNIT_2_FREE_FLOW)         { freeFlowMode[1] = newValue.operator bool(); }
        else if (index == UNIT_3_FREE_FLOW)         { freeFlowMode[2] = newValue.operator bool(); }
        
        else if (index == UNIT_1_PHASE)
        {
            phase[0] = newValue.operator double() / 100.f;
            
            freqPlayer[0]->setPhase(phase[0]);
            chordPlayer[0]->setPhase(phase[0]);
        }
        else if (index == UNIT_2_PHASE)
        {
            phase[1] = newValue.operator double() / 100.f;
            freqPlayer[1]->setPhase(phase[1]);
            chordPlayer[1]->setPhase(phase[1]);
        }
        else if (index == UNIT_3_PHASE)
        {
            phase[2] = newValue.operator double() / 100.f;
            freqPlayer[2]->setPhase(phase[2]);
            chordPlayer[2]->setPhase(phase[2]);
    
        }
        
        else if (index == AMPLITUDE_X)              { amplitude[0] = newValue.operator double(); }
        else if (index == AMPLITUDE_Y)              { amplitude[1] = newValue.operator double(); }
        else if (index == AMPLITUDE_Z)              { amplitude[2] = newValue.operator double(); }

        else if (index >= UNIT_1_FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE && index <= UNIT_1_FREQUENCY_PLAYER_NUM_DURATION)
        {
            // freq player unit 1
            
            int indexShift = index - UNIT_1_FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE;
            
            freqPlayer[0]->setParameter(0, indexShift, newValue);

        }
        
        else if (index >= UNIT_2_FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE && index <= UNIT_2_FREQUENCY_PLAYER_NUM_DURATION)
        {
            // freq player unit 2
            
            int indexShift = index - UNIT_2_FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE;
            
            freqPlayer[1]->setParameter(0, indexShift, newValue);
        }
        
        else if (index >= UNIT_3_FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE && index <= UNIT_3_FREQUENCY_PLAYER_NUM_DURATION)
        {
            // freq player unit 2
            
            int indexShift = index - UNIT_3_FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE;
            
            freqPlayer[2]->setParameter(0, indexShift, newValue);
        }
        
        else if (index >= UNIT_1_CHORD_PLAYER_SHORTCUT_IS_ACTIVE && index <= UNIT_1_CHORD_PLAYER_CHORDPLAYER_SCALE)
        {
            // chord player unit 1
            
            int indexShift = index - UNIT_1_CHORD_PLAYER_SHORTCUT_IS_ACTIVE;
            
            chordPlayer[0]->setParameter(0, indexShift, newValue);
        }
        
        else if (index >= UNIT_2_CHORD_PLAYER_SHORTCUT_IS_ACTIVE && index <= UNIT_2_CHORD_PLAYER_CHORDPLAYER_SCALE)
        {
            // chord player unit 2
            
            int indexShift = index - UNIT_2_CHORD_PLAYER_SHORTCUT_IS_ACTIVE;
            
            chordPlayer[1]->setParameter(0, indexShift, newValue);
        }
        
        else if (index >= UNIT_3_CHORD_PLAYER_SHORTCUT_IS_ACTIVE && index <= UNIT_3_CHORD_PLAYER_CHORDPLAYER_SCALE)
        {
            // chord player unit 3
            
            int indexShift = index - UNIT_3_CHORD_PLAYER_SHORTCUT_IS_ACTIVE;
            
            chordPlayer[2]->setParameter(0, indexShift, newValue);
        }
 
    }
    
    
    virtual void processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
    {
        // process x processor left
        
        for (int axis = 0; axis < 3; axis++)
        {
            processorOutputBuffers[axis].clear();
            
            if (freqOrChord[axis])
            {
                freqPlayer[axis]->processBlock(processorOutputBuffers[axis], midiMessages);
            }
            else
            {
                chordPlayer[axis]->processBlock(processorOutputBuffers[axis], midiMessages);
            }
            
            processorOutputBuffers[axis].applyGain(amplitude[axis]/100.f);
            
        }
        
        /* sum || process dual mono-binaural || frequency modulation || amplitude modulation  */
        sumOutputBuffers(buffer);
        
        // add outputbuffers to ringbuffer
        
        for (int i = 0; i< buffer.getNumSamples(); i++)
        {
            for (int axis = 0; axis < 3; axis++)
            {
                visualiserRingBuffer.setSample(axis, visualiserBufferCounter, processorOutputBuffers[axis].getSample(0, i));
            }
            
            visualiserBufferCounter++;
            
            if (visualiserBufferCounter >= visualiserBufferSize) visualiserBufferCounter = 0;
        }
    }

    void sumOutputBuffers(AudioBuffer<float> & buffer)
    {
        buffer.clear();
        
        // binaural output temporary
        buffer.addFrom(0, 0, processorOutputBuffers[0], 0, 0, buffer.getNumSamples()); // left x
        buffer.addFrom(1, 0, processorOutputBuffers[1], 0, 0, buffer.getNumSamples()); // right Y
    }

    
    void panic()
    {
        freqPlayer[0]->panic();
        freqPlayer[1]->panic();
        freqPlayer[2]->panic();
        
        chordPlayer[0]->panic();
        chordPlayer[1]->panic();
        chordPlayer[2]->panic();
    }
    
    void setPlayerCommand(PLAYER_COMMANDS command)
    {
        freqPlayer[0]->setPlayerCommand(command, freeFlowMode[0]);
        freqPlayer[1]->setPlayerCommand(command, freeFlowMode[1]);
        freqPlayer[2]->setPlayerCommand(command, freeFlowMode[2]);
        
        chordPlayer[0]->setPlayerCommand(command, freeFlowMode[0]);
        chordPlayer[1]->setPlayerCommand(command, freeFlowMode[1]);
        chordPlayer[2]->setPlayerCommand(command, freeFlowMode[2]);
    }
    void setPlayerPlayMode(PLAY_MODE mode)
    {
        freqPlayer[0]->setPlayerPlayMode(mode);
        freqPlayer[1]->setPlayerPlayMode(mode);
        freqPlayer[2]->setPlayerPlayMode(mode);
        
        chordPlayer[0]->setPlayerPlayMode(mode);
        chordPlayer[1]->setPlayerPlayMode(mode);
        chordPlayer[2]->setPlayerPlayMode(mode);
    }
    

    // lissajous buffer needs to return values for all three axises, x, y, z

    int refreshRate = 1;
    int visualiserBufferSize = 44100 + 1;
    int visualiserBufferCounter = 0;
    
    void setRefreshRate(int newRate)
    {
        refreshRate = newRate;
        
        visualiserBufferSize = (int)(sample_rate / refreshRate) + 1;
        
        visualiserRingBuffer.clear();
        visualiserRingBuffer.setSize(3, visualiserBufferSize);
        
        visualiserBufferCounter = 0;
    }
    

    // need refresh rate, slider on visualiser.. integer numbers 1hz-60hz.. sets timer callback, and also ring buffer size..
    // could pull plot path directly from processor..
    
    AudioBuffer<float> getLissajousBuffer()
    {
        return AudioBuffer<float>(visualiserRingBuffer);
    }
    
    // need a ring buffer, set max buffer ms * sample rate..
    // add function to prepareToPlay to redeclare buffer
    // could
    
    AudioBuffer<float> visualiserRingBuffer;
    AudioBuffer<float> processorOutputBuffers[3];
    
private:
    double sample_rate;
    
    
    FrequencyManager * frequencyManager;
    SynthesisLibraryManager * sampleLibraryManager;
    
    LissajousFrequencyPlayerProcessor * freqPlayer[3]; // x and y
    LissajousChordPlayerProcessor * chordPlayer[3]; // x and y
    
    bool freqOrChord[3];
    bool freeFlowMode[3];
    
    double phase[3];
    double amplitude[3];
};
