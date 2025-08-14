/*
  ==============================================================================

    FrequencyPlayerProcessor.h
    Created: 30 May 2019 2:46:52pm
    Author:  Gary Jones

  ==============================================================================
*/

#pragma once

#include "VotanSynthProcessor.h"
#include "SamplerProcessor.h"
#include "WavetableSynthProcessor.h"

class ProjectManager;

class FrequencyPlayerProcessor : public AudioProcessor
{
public:
    FrequencyPlayerProcessor(FrequencyManager * fm, ProjectManager * pm);
    ~FrequencyPlayerProcessor();
    
    virtual void prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock) override;
    
    void setParameter(int synthRef, int index, var newValue);
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
    virtual const String getName() const  override { return "FrequencyPlayerProcessor"; }
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
    
    FrequencySynthProcessor *  synth[NUM_SHORTCUT_SYNTHS];
    FrequencyPlayerWavetableSynthProcessor * wavetableSynth[NUM_SHORTCUT_SYNTHS];
    
    enum WAVEFORM_TYPES { DEFAULT = 0, SINE, TRI, SQR, SAW, WAVETABLE } waveformType[NUM_SHORTCUT_SYNTHS];
    
    PLAY_STATE getPlaystate() { return playState; }
    
    AUDIO_OUTPUTS output[NUM_SHORTCUT_SYNTHS];
    
    AudioBuffer<float> outputBuffer;
    
private:
    bool samplerOrSynth; // switched between processing of Sampler of Synth
    
    FrequencyManager * frequencyManager;
    
    bool isActive[NUM_SHORTCUT_SYNTHS];
    bool shouldMute[NUM_SHORTCUT_SYNTHS];
    
    PLAY_MODE  playMode;
    PLAY_STATE playState;
    bool       shouldRecord;
    
    //=============================================================
    // Play Repeater
    //=============================================================
public:
    class PlayRepeater {
        
    public:
        PlayRepeater(FrequencyPlayerProcessor * cp, double sr);
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
        
        FrequencyPlayerProcessor * proc;
        
        PLAY_MODE playMode;
        PLAY_STATE playState;
        
        bool playSimultaneous;
        
        int currentPlayingShortcut = 0;
        int sampleCounter;
        int numSamplesPerMS;
        double sampleRate;
        
        bool shouldProcess;
        
        // params
        bool isActive[NUM_SHORTCUT_SYNTHS];
        int numRepeats[NUM_SHORTCUT_SYNTHS];
        int lengthMS[NUM_SHORTCUT_SYNTHS];
        int lengthInSamples[NUM_SHORTCUT_SYNTHS];
        int pauseMS[NUM_SHORTCUT_SYNTHS];
        int pauseInSamples[NUM_SHORTCUT_SYNTHS];
        int totalNumMsOfLoop[NUM_SHORTCUT_SYNTHS];
        int totalMSOfLoop;
        uint64 totalNumSamplesOfLoop;
        int currentMS;
        
        
        void processSecondsClock();
        
        // events
        int nextNoteOnEvent[NUM_SHORTCUT_SYNTHS];  // sampleRef for next note on
        int nextNoteOffEvent[NUM_SHORTCUT_SYNTHS]; // sampleRef for next note off
        
        int currentRepeat[NUM_SHORTCUT_SYNTHS];    // current playing repeat, increments for each
        bool isPaused[NUM_SHORTCUT_SYNTHS];
    } ;
    
    PlayRepeater * repeater;
    ProjectManager * projectManager;
};
