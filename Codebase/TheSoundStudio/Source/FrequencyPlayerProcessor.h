/*
  ==============================================================================

    FrequencyPlayerProcessor.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once

#include "VotanSynthProcessor.h"
#include "SamplerProcessor.h"
#include "WavetableSynthProcessor.h"
#include "PlayRepeaterBase.h"

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
    class PlayRepeater : public PlayRepeaterBase
    {
    public:
        PlayRepeater(FrequencyPlayerProcessor* cp, double sr)
            : PlayRepeaterBase(sr), proc(cp) {}
        ~PlayRepeater() override = default;

        void triggerOnEvent(int shortcutRef) override  { proc->triggerNoteOn(shortcutRef); }
        void triggerOffEvent(int shortcutRef) override { proc->triggerNoteOff(shortcutRef); }
        void allNotesOff() override                    { proc->panic(); }
        bool isShortcutMuted(int shortcutRef) const override { return proc->shouldMute[shortcutRef]; }
        PLAY_STATE& getPlayState() override            { return proc->playState; }

    private:
        FrequencyPlayerProcessor* proc;
    };
    
    PlayRepeater * repeater;
    ProjectManager * projectManager;
};
