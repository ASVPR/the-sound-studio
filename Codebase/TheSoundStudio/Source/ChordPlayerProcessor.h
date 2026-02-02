/*
  ==============================================================================

    ChordPlayerProcessor.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "SamplerProcessor.h"
#include "VotanSynthProcessor.h"
#include "SynthesisLibraryManager.h"
#include "SynthesisEngine.h"
#include "WavetableSynthProcessor.h"
#include "PlayRepeaterBase.h"

class ProjectManager;

class ChordPlayerProcessor : public AudioProcessor
{
public:
    ChordPlayerProcessor(FrequencyManager * fm, SynthesisLibraryManager * slm, SynthesisEngine * se, ProjectManager * pm);
    ~ChordPlayerProcessor();
    
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
    virtual const String getName() const  override { return "ChordPlayerProcessor"; }
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
    
    ChordSynthProcessor *   synth[NUM_SHORTCUT_SYNTHS];
    SamplerProcessor *      sampler[NUM_SHORTCUT_SYNTHS];
    WavetableSynthProcessor * wavetableSynth[NUM_SHORTCUT_SYNTHS];
    
    ChordManager *          chordManager[NUM_SHORTCUT_SYNTHS];
    
    enum WAVEFORM_TYPES
    {
        SAMPLER = 0, SINE, TRI, SQR, SAW, WAVETABLE
    }
    waveformType [NUM_SHORTCUT_SYNTHS];
    
    PLAY_STATE getPlaystate() { return playState; }
    
private:
    bool samplerOrSynth; // switched between processing of Sampler of Synth
    
    bool chordSource[NUM_SHORTCUT_SYNTHS];
    int octaveShift[NUM_SHORTCUT_SYNTHS];
    float insertFrequency[NUM_SHORTCUT_SYNTHS];
    bool manipulateChoseFrequency[NUM_SHORTCUT_SYNTHS];
    bool multiplyOrDivision[NUM_SHORTCUT_SYNTHS]; // mult = 0, div = 1
    float multiplyValue[NUM_SHORTCUT_SYNTHS];
    float divisionValue[NUM_SHORTCUT_SYNTHS];
    bool hasCustomChord[NUM_SHORTCUT_SYNTHS];
    
    
    FrequencyManager * frequencyManager;
    SynthesisLibraryManager * sampleLibraryManager;
    SynthesisEngine * synthesisEngine;

    bool isActive[NUM_SHORTCUT_SYNTHS];
    bool shouldMute[NUM_SHORTCUT_SYNTHS];
    
    PLAY_MODE  playMode;
    PLAY_STATE playState;
    bool       shouldRecord;
    
    AUDIO_OUTPUTS output[NUM_SHORTCUT_SYNTHS];
    
    AudioBuffer<float> outputBuffer;
    
    //=============================================================
    // Play Repeater
    //=============================================================
public:
    class PlayRepeater : public PlayRepeaterBase
    {
    public:
        PlayRepeater(ChordPlayerProcessor* cp, double sr)
            : PlayRepeaterBase(sr), proc(cp) {}
        ~PlayRepeater() override = default;

        void triggerOnEvent(int shortcutRef) override  { proc->triggerNoteOn(shortcutRef); }
        void triggerOffEvent(int shortcutRef) override { proc->triggerNoteOff(shortcutRef); }
        void allNotesOff() override                    { proc->panic(); }
        bool isShortcutMuted(int shortcutRef) const override { return proc->shouldMute[shortcutRef]; }
        PLAY_STATE& getPlayState() override            { return proc->playState; }

    private:
        ChordPlayerProcessor* proc;
    };
    
    PlayRepeater * repeater;
    
    ProjectManager * projectManager;
    
    // Helper methods for synthesis engine integration
    String getCurrentInstrumentName(int shortcutRef);
    SynthesisType getSynthesisTypeForInstrument(const String& instrumentName);
    void generateSynthesisAudio(AudioBuffer<float>& buffer, int shortcutRef, SynthesisType synthType);
};


