/*
  ==============================================================================

    ChordScannerProcessor.h
    Created: 30 May 2019 2:46:40pm
    Author:  Gary Jones

  ==============================================================================
*/

#pragma once

#include "VotanSynthProcessor.h"
#include "SamplerProcessor.h"
#include "WavetableSynthProcessor.h"


// All the mode wrappers

// Handles all the Chord Player synths etc..

// Needs
// 1. Refactor synthpro Parameters to ChordScanner param enums
// 2. fix synth gui for playing instrument & other comboboxes..
// 2. work out chod note to trigger.
// 3. sequencer appears to be ok
// 4. GUI callback for progress bar
// 5.


// 1. clarify gui widgets
// - Chord -> textbox - displays result - same chord playing now..

// - Scan only main chords -> ** go through all the chord types - play until 10th octave - updates chord
// - plays c0 Major, c0 Minor, etc, D0 major D0 minor etc.....
// - iterate through Chordtype, keynote, octave, play for length and ciao
// playRepeater simlply iterates through above, grabbing chord notes from chordmanager
// remove keynote/octave from the gui


// a, Scan all chords ?
// -- keynote to from
// -  a chord is defined as : three or more musical tones sounded simultaneously
// need an algorithm that iterates through the possible combinations of harmonic notes
//

// b, Scan Specific Range
// from keynote/octave to keynote/octave


// Scan by Frequency
// - remove octave to / from ****
// frequency range, to - from
// based on central frequency (eg 432hz), from range pull known chordtypes from chordManager that exists between the range
// play chords in sequence, prob best to get number of chord that exists before starting

// 2. mass chord theory

// 3. progress bar, pause / stop / no loop

class ProjectManager;

class ChordScannerProcessor : public AudioProcessor
{
public:
    ChordScannerProcessor(FrequencyManager * fm, SynthesisLibraryManager * slm, ProjectManager * pm);
    ~ChordScannerProcessor();
    
    virtual void prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock) override;
    
    void setParameter(int index, var newValue);
    void setOversamplingFactor(int newFactor);
    
    virtual void processBlock (AudioBuffer<float>& buffer,
                               MidiBuffer& midiMessages) override;
    
    
    void triggerKeyDown(int shortcutRef);
    void triggerKeyUp(int shortcutRef);
    void triggerNoteOn(KEYNOTES keynote, CHORD_TYPES chordType, int octave);
    void triggerNoteOff(KEYNOTES keynote, CHORD_TYPES chordType, int octave);
    void triggerNoteOnDirect(KEYNOTES keynote, int octave);
    void triggerNoteOffDirect(KEYNOTES keynote, int octave);
    
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
    
    void setScanMode(SCANNER_MODE mode) {
        scanMode = mode; repeater->setScanMode(scanMode);
    }
    
    ChordSynthProcessor *  synth;
    SamplerProcessor *  sampler;
    WavetableSynthProcessor * wavetableSynth;
    ChordManager * chordManager;
    
    
    enum WAVEFORM_TYPES
    {
        SAMPLER = 0, SINE, TRI, SQR, SAW, WAVETABLE
    }
    waveformType;
    
    
    
    PLAY_STATE getPlaystate() { return playState; }
    
    AUDIO_OUTPUTS output;
    
    AudioBuffer<float> outputBuffer;
    
private:
    bool samplerOrSynth; // switched between processing of Sampler of Synth
    
    FrequencyManager * frequencyManager;
    SynthesisLibraryManager * sampleLibraryManager;
    
    bool isActive;
    bool shouldMute;
    
    PLAY_MODE  playMode;
    PLAY_STATE playState;
    bool       shouldRecord;
    
    SCANNER_MODE scanMode;
    
    //=============================================================
    // Play Repeater
    //=============================================================
public:
    class PlayRepeater
    {
        
    public:
        PlayRepeater(ChordScannerProcessor * cp, double sr);
        ~PlayRepeater();
        
        void prepareToPlay(double newSampleRate);
        void tickBuffer(int numSamples);
        void play();
        void stop();
        
    private:
        void calculatePlaybackTimers();
        
        void processShortcut(int shortcutRef, int sampleRef);
        void processSimultaneousShortcuts(int firstShortcutRef, int sampleRef);
        
        void triggerOnEvent(KEYNOTES keynote, CHORD_TYPES chordType, int octave);
        void triggerOffEvent(KEYNOTES keynote, CHORD_TYPES chordType, int octave);
        
        
        
        void allNotesOff();
        void clearOpenRepeats();
        void resetTick();
        
        void prepareNextShortcut();
        
    public:
        void setIsActive(bool should);
        void calculateLengths();
        void setNumRepeats(int num);
        void setPauseMS(int ms);
        void setLengthMS(int ms);
        void setPlayMode(PLAY_MODE mode);
        void setPlaySimultaneous(bool should);
        
        void setKeynoteFrom(int k)      { keynoteFrom = k; resetTick();}
        void setKeynoteTo(int k)        { keynoteTo = k; resetTick();}
        void setOctaveFrom(int o)       { octaveFrom = o; resetTick();}
        void setOctaveTo(int o)         { octaveTo = o; resetTick();}
        void setFrequencyFrom(float f)  { frequencyFrom = f; resetTick();}
        void setFrequencyTo(float f)    { frequencyTo = f; resetTick();}
        void setScanMode(SCANNER_MODE mode) { scanMode = mode; resetTick(); }
        
        void process_MainChords(int sampleRef);
        void process_AllChords(int sampleRef);
        void process_SpecificRange(int sampleRef);
        void process_ByFrequency(int sampleRef);
        
        // main chord variables
        int keynoteIterator;
        int chordTypeIterator;
        int octaveIterator;
        int currentPlayingChord;
        
        int getOctaveIterator()
        {
            return octaveIterator;
        }
        
        int maxOctaves = OCTAVE_MAX;
        
        int getTotalMSOfLoop();
        int getCurrentMSInLoop();
        float getProgressBarValue();
        String getTimeRemainingInSecondsString();
       
        // All Chords Variables
        // int keynoteIterator
        // int octaveIterator;
        int numNotesIterator; // starts at 2, increases to maxNumNotes before back
        #define MaxNumNotes 12
        #define MinNumNotes 2
        int keynoteIteratorAllChords[MaxNumNotes];
        
        Array<int> getNextMidiNoteOnsForAllChords()
        {
            Array<int> notes;
            
            if (numNotesIterator < MaxNumNotes)
            {

                
            }
            
            return notes;
        }
        
        
        
    private:
        
        ChordScannerProcessor * proc;
        
        PLAY_MODE playMode;
        PLAY_STATE playState;
        SCANNER_MODE scanMode;
        
        bool playSimultaneous;
        
        int currentPlayingShortcut = 0;
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
        
        int keynoteFrom;
        int keynoteTo;
        int octaveFrom;
        int octaveTo;
        float frequencyFrom;
        float frequencyTo;
        
        
        void processSecondsClock();
        
        // events
        int nextNoteOnEvent;  // sampleRef for next note on
        int nextNoteOffEvent; // sampleRef for next note off
        
        int currentRepeat;    // current playing repeat, increments for each
        bool isPaused;
    };
    
    PlayRepeater * repeater;
    ProjectManager * projectManager;
};
