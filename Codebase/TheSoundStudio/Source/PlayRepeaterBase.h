/*
  ==============================================================================

    PlayRepeaterBase.h
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "Parameters.h"

//=============================================================
// Play Repeater
//=============================================================
class PlayRepeaterBase {
    
public:
    PlayRepeaterBase(double sr)
    {
//        sampleRate          = sr;
//        proc                = cp;
        sampleCounter       = 0;
        numSamplesPerMS     = sampleRate / 1000;
        
        playMode            = PLAY_MODE::NORMAL;
        playState           = PLAY_STATE::OFF;
        
        for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
        {
            numRepeats[i]           = 0;
            lengthMS[i]             = 0;
            lengthInSamples[i]      = 0;
            pauseMS[i]              = 0;
            pauseInSamples[i]       = 0;
            shouldLoop[i]           = false;
            nextNoteOnEvent[i]      = 0;
            nextNoteOffEvent[i]     = 0;
            currentRepeat[i]        = 0;
            isPaused[i]             = false;
        }
    }
    ~PlayRepeaterBase();
    
    // needs range of virtual functions hwich each class overrides
    // anything that pushes to proc..
    
    
    
    void tickBuffer(int numSamples)
    {
        if (shouldProcess)
        {
            for (int i = 0; i < numSamples; i++)
            {
                processShortcut(currentPlayingShortcut, sampleCounter);
                
                sampleCounter++;
            }
        }
    }
    
    void processShortcut(int shortcutRef, int sampleRef)
    {
        if (sampleRef == nextNoteOnEvent[shortcutRef])
        {
            triggerOnEvent(shortcutRef);
            
            nextNoteOffEvent[shortcutRef] = sampleRef + lengthInSamples[shortcutRef];
            
            currentRepeat[shortcutRef]++;
        }
        else if (sampleRef == nextNoteOffEvent[shortcutRef])
        {
            triggerOffEvent(shortcutRef);
            
            // set next note on event after paus
            if (currentRepeat[shortcutRef] < numRepeats[shortcutRef])
            {
                nextNoteOnEvent[shortcutRef] = sampleRef + pauseInSamples[shortcutRef];
            }
            else
            {
                if (shouldLoop[shortcutRef])
                {
                    currentRepeat[shortcutRef]      = 0;
                    nextNoteOnEvent[shortcutRef]    = sampleRef + pauseInSamples[shortcutRef];
                }
                else
                {
                    // exit
                    stop();
                }
            }
        }
    }
    
    virtual void triggerOnEvent(int shortcutRef)
    {
//        proc->triggerNoteOn(shortcutRef);
    }
    
    void triggerOffEvent(int shortcutRef)
    {
//        proc->triggerNoteOff(shortcutRef);
    }
    void allNotesOff()
    {
//        proc->panic();
    }
    void resetTick()
    {
        sampleCounter               = 0;
        currentPlayingShortcut      = 0;
    }
    
    void start()
    {
        shouldProcess = true;
        sampleCounter = 0;
    }
    void stop()
    {
//        proc->playState = PLAY_STATE::OFF;
        
        shouldProcess   = false;
        sampleCounter   = 0;
        
        allNotesOff();
        clearOpenRepeats();
    }
    void play()
    {
        shouldProcess = true;
    }
    void pause()
    {
        shouldProcess = false;
        allNotesOff();
    }
    
    void clearOpenRepeats()
    {
        for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
        {
            currentRepeat[i]        = 0;
            currentPlayingShortcut  = 0;
            nextNoteOnEvent[i]      = 0;
            nextNoteOffEvent[i]     = nextNoteOnEvent[i] + lengthInSamples[i];
        }
    }
    
    void calculateLengths(int shortcutRef)
    {
        float oneMSInSamples = sampleRate / 1000;
        
        lengthInSamples[shortcutRef] = (int)lengthMS[shortcutRef] * oneMSInSamples;
        pauseInSamples[shortcutRef]  = (int)pauseMS[shortcutRef]  * oneMSInSamples;
    }
    void setNumRepeats(int shortcutRef, int num)
    {
        numRepeats[shortcutRef] = num; calculateLengths(shortcutRef);
    }
    
    void setPauseMS(int shortcutRef, int ms)
    {
        pauseMS[shortcutRef] = ms; calculateLengths(shortcutRef);
    }
    
    void setLengthMS(int shortcutRef, int ms)
    {
        lengthMS[shortcutRef] = ms; calculateLengths(shortcutRef);
    }
    void setLoop(int shortcutRef, bool should)
    {
        shouldLoop[shortcutRef] = should;
    }
    void setPlayMode(PLAY_MODE mode)
    {
        playMode = mode;
    }
    
private:
    AudioProcessor * proc;
    
    PLAY_MODE playMode;
    
    PLAY_STATE playState;
    
    int currentPlayingShortcut = 0;
    int sampleCounter;
    int numSamplesPerMS;
    double sampleRate;
    
    bool shouldProcess;
    
    // params
    int numRepeats[NUM_SHORTCUT_SYNTHS];
    int lengthMS[NUM_SHORTCUT_SYNTHS];
    int lengthInSamples[NUM_SHORTCUT_SYNTHS];
    int pauseMS[NUM_SHORTCUT_SYNTHS];
    int pauseInSamples[NUM_SHORTCUT_SYNTHS];
    bool shouldLoop[NUM_SHORTCUT_SYNTHS];
    
    // events
    int nextNoteOnEvent[NUM_SHORTCUT_SYNTHS];  // sampleRef for next note on
    int nextNoteOffEvent[NUM_SHORTCUT_SYNTHS]; // sampleRef for next note off
    
    int currentRepeat[NUM_SHORTCUT_SYNTHS];    // current playing repeat, increments for each
    bool isPaused[NUM_SHORTCUT_SYNTHS];
    
};


