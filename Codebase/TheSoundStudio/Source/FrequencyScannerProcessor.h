/*
  ==============================================================================

    FrequencyScannerProcessor.h
    Created: 1 Oct 2019 9:06:35am
    Author:  Gary Jones

  ==============================================================================
*/

#pragma once

#include "VotanSynthProcessor.h"
#include "WavetableSynthProcessor.h"

class ProjectManager;

class FrequencyScannerProcessor : public AudioProcessor
{
public:
    FrequencyScannerProcessor(FrequencyManager * fm, ProjectManager * pm);
    ~FrequencyScannerProcessor();
    
    virtual void prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock) override;
    
    void setParameter(int index, var newValue);
    void setOversamplingFactor(int newFactor);
    
    virtual void processBlock (AudioBuffer<float>& buffer,
                               MidiBuffer& midiMessages) override;
    
    
    void triggerKeyDown();
    void triggerKeyUp();
    void triggerNoteOn(float freq);
    void triggerNoteOff();
    
    void panic();
    
    virtual double getTailLengthSeconds() const override { return 0.0; }
    virtual bool acceptsMidi() const override { return true; }
    virtual bool producesMidi() const override { return false; }
    virtual void releaseResources()override { }
    virtual AudioProcessorEditor* createEditor() override {  return 0; }
    virtual bool hasEditor() const override {  return false; }
    virtual const String getName() const  override { return "FrequencyScannerProcessor"; }
    virtual int getNumPrograms()override { return 0; }
    virtual int getCurrentProgram() override { return 0; }
    virtual void setCurrentProgram (int index) override { }
    virtual const String getProgramName (int index) override { return ""; }
    virtual void changeProgramName (int index, const String& newName) override { }
    virtual void getStateInformation (juce::MemoryBlock& destData) override {}
    virtual void setStateInformation (const void* data, int sizeInBytes)override {}
    
    void setActiveShortcutSynth(int synthRef, bool shouldBeActive);
    
    void setPlayerCommand(PLAYER_COMMANDS command);
    void setPlayerPlayMode(PLAY_MODE mode);
    
    FrequencyScannerSynthProcessor * synth;
    
    FrequencyScannerWavetableSynthProcessor * wavetableSynth;
    
    enum WAVEFORM_TYPES { DEFAULT = 0, SINE, TRI, SQR, SAW, WAVETABLE } waveformType;
    
    PLAY_STATE getPlaystate() { return playState; }
    
    AUDIO_OUTPUTS output;
    
    AudioBuffer<float> outputBuffer;
    
    
private:
    bool samplerOrSynth; // switched between processing of Sampler of Synth
    
    FrequencyManager * frequencyManager;
    
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
        PlayRepeater(FrequencyScannerProcessor * cp, double sr);
        ~PlayRepeater();
        
        void prepareToPlay(double newSampleRate);
        void tickBuffer(int numSamples);
        void play();
        void stop();
        
    private:
        void calculatePlaybackTimers();
        
        void processLog(int sampleRef);
        void processLin(int sampleRef);
        
        void triggerOnEvent(float freq);
        void triggerOffEvent();
        void allNotesOff();
        void clearOpenRepeats();
        void resetTick();
        
//        void prepareNextShortcut();
        
    public:
        void setIsActive( bool should);
        void calculateLengths();
        void setPauseMS(int ms);
        void setLengthMS(int ms);
        void setNumRepeats(int num);
        void setPlayMode(PLAY_MODE mode);
        
        int getTotalMSOfLoop();
        int getCurrentMSInLoop();
        float getProgressBarValue();
        String getTimeRemainingInSecondsString();
        
        String getCurrentFrequencyString()
        {
            String freqText(currentFrequency, 3, false); freqText.append(" Hz", 3);
            return freqText;
        }
        
    private:
        
        FrequencyScannerProcessor * proc;
        
        PLAY_MODE playMode;
        PLAY_STATE playState;

        int sampleCounter;
        int numSamplesPerMS;
        double sampleRate;
        
        bool shouldProcess;
        
        // params
        bool isActive;
        int numRepeats;
        int numRepeatIterator;
        int totalNumRepeats;
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
        
        //
        // parameters for sequencer
        int scanningMode; // all ferequency / selected range
        float frequencyFrom;
        float frequencyTo;
        float frequencyToExtended;
        bool extendedRange;
        int logOrLinear;
        float logIntervalInOctaves;
        float linIntervalInHz;
        float currentFrequency;
        float nextFrequency;
        
    public:
        void setScanningMode(int mode)          { scanningMode = mode; }
        void setFrequencyFrom(float f)          { frequencyFrom = f; }
        void setFrequencyTo(float f)
        {
            frequencyTo = f;
        }
        void setExtendedRange(bool should)
        {
            extendedRange = should;
        }
        void setLogOrLinear(int m)              { logOrLinear = m; }
        void setLogIntervalInOctaves(float i)   { logIntervalInOctaves = i; }
        void setLinIntervalInHz(float hz)       { linIntervalInHz = hz; }
        
    } ;
    
    PlayRepeater * repeater;
    ProjectManager * projectManager;
};
