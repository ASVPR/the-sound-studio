/*
  ==============================================================================

    PlayRepeaterBase.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "Parameters.h"

class PlayRepeaterBase
{
public:
    PlayRepeaterBase(double sr)
    {
        sampleRate              = sr;
        sampleCounter           = 0;
        numSamplesPerMS         = static_cast<int>(sampleRate / 1000);
        playMode                = PLAY_MODE::NORMAL;
        playState               = PLAY_STATE::OFF;
        currentMS               = 0;
        totalMSOfLoop           = 0;
        totalNumSamplesOfLoop   = 0;
        playSimultaneous        = false;
        shouldProcess           = false;
        currentPlayingShortcut  = 0;

        for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
        {
            isActive[i]         = false;
            numRepeats[i]       = 0;
            lengthMS[i]         = 0;
            lengthInSamples[i]  = 0;
            pauseMS[i]          = 0;
            pauseInSamples[i]   = 0;
            nextNoteOnEvent[i]  = 0;
            nextNoteOffEvent[i] = 0;
            currentRepeat[i]    = 0;
            isPaused[i]         = false;
        }
    }

    virtual ~PlayRepeaterBase() = default;

    // =========================================================
    // Virtual hooks — subclasses implement these
    // =========================================================
    virtual void triggerOnEvent(int shortcutRef) = 0;
    virtual void triggerOffEvent(int shortcutRef) = 0;
    virtual void allNotesOff() = 0;
    virtual bool isShortcutMuted(int shortcutRef) const = 0;
    virtual PLAY_STATE& getPlayState() = 0;

    // =========================================================
    // Preparation
    // =========================================================
    void prepareToPlay(double newSampleRate)
    {
        shouldProcess = false;

        sampleRate = newSampleRate;
        numSamplesPerMS = static_cast<int>(sampleRate / 1000);

        float oneMSInSamples = static_cast<float>(sampleRate / 1000);

        for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
        {
            lengthInSamples[i] = static_cast<int>(lengthMS[i] * oneMSInSamples);
            pauseInSamples[i]  = static_cast<int>(pauseMS[i]  * oneMSInSamples);
        }

        calculatePlaybackTimers();
        shouldProcess = true;
    }

    // =========================================================
    // Tick & Event Sequencing
    // =========================================================
    void tickBuffer(int numSamples)
    {
        if (!shouldProcess) return;

        for (int i = 0; i < numSamples; i++)
        {
            processSecondsClock();

            if (playSimultaneous)
            {
                processSimultaneousShortcuts(currentPlayingShortcut, sampleCounter);
            }
            else
            {
                if (isActive[currentPlayingShortcut])
                {
                    processShortcut(currentPlayingShortcut, sampleCounter);
                }
            }

            sampleCounter++;

            if (playMode == PLAY_MODE::NORMAL)
            {
                if (sampleCounter >= totalNumSamplesOfLoop)
                {
                    currentMS = totalMSOfLoop;
                    stop();
                }
            }
            else if (playMode == PLAY_MODE::LOOP)
            {
                if (sampleCounter >= totalNumSamplesOfLoop)
                    resetTick();
            }
        }
    }

    // =========================================================
    // Transport
    // =========================================================
    void play()
    {
        PLAY_STATE& state = getPlayState();

        if (state == PLAY_STATE::PLAYING)
        {
            shouldProcess = false;
            state = PLAY_STATE::PAUSED;
        }
        else if (state == PLAY_STATE::PAUSED)
        {
            shouldProcess = true;
            state = PLAY_STATE::PLAYING;
        }
        else
        {
            resetTick();
            shouldProcess   = true;
            sampleCounter   = 0;
            state = PLAY_STATE::PLAYING;
        }
    }

    void stop()
    {
        shouldProcess = false;
        allNotesOff();
        getPlayState() = PLAY_STATE::OFF;
    }

    // =========================================================
    // Configuration Setters
    // =========================================================
    void setIsActive(int shortcutRef, bool should)
    {
        isActive[shortcutRef] = should;
        calculatePlaybackTimers();
    }

    void calculateLengths(int shortcutRef)
    {
        float oneMSInSamples = static_cast<float>(sampleRate / 1000);

        lengthInSamples[shortcutRef] = static_cast<int>(lengthMS[shortcutRef] * oneMSInSamples);
        pauseInSamples[shortcutRef]  = static_cast<int>(pauseMS[shortcutRef]  * oneMSInSamples);

        calculatePlaybackTimers();
    }

    void setNumRepeats(int shortcutRef, int num)
    {
        numRepeats[shortcutRef] = num;
        calculateLengths(shortcutRef);
    }

    void setPauseMS(int shortcutRef, int ms)
    {
        pauseMS[shortcutRef] = ms;
        calculateLengths(shortcutRef);
    }

    void setLengthMS(int shortcutRef, int ms)
    {
        lengthMS[shortcutRef] = ms;
        calculateLengths(shortcutRef);
    }

    void setPlayMode(PLAY_MODE mode)
    {
        playMode = mode;
    }

    void setPlaySimultaneous(bool should)
    {
        playSimultaneous = should;
        resetTick();
    }

    // =========================================================
    // Progress Queries
    // =========================================================
    int getTotalMSOfLoop() const { return totalMSOfLoop; }
    int getCurrentMSInLoop() const { return currentMS; }

    float getProgressBarValue() const
    {
        if (totalMSOfLoop == 0) return 0.0f;
        return static_cast<float>(currentMS) / totalMSOfLoop;
    }

    String getTimeRemainingInSecondsString() const
    {
        int totalMSRemaining = totalMSOfLoop - currentMS;
        int totalSeconds     = totalMSRemaining / 1000;
        int minutes          = totalSeconds / 60;
        int secs             = totalSeconds % 60;

        return String::formatted("Playing (%02i:%02i)", minutes, secs);
    }

private:
    // =========================================================
    // Internal Sequencing
    // =========================================================
    void processSecondsClock()
    {
        currentMS = static_cast<int>(sampleCounter / (sampleRate / 1000));
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

            if (currentRepeat[shortcutRef] < numRepeats[shortcutRef])
            {
                nextNoteOnEvent[shortcutRef] = sampleRef + pauseInSamples[shortcutRef];
            }
            else
            {
                for (int c = currentPlayingShortcut + 1; c < NUM_SHORTCUT_SYNTHS; c++)
                {
                    if (isActive[c] && !isShortcutMuted(c))
                    {
                        currentPlayingShortcut = c;
                        prepareNextShortcut();
                        break;
                    }
                }
            }
        }
    }

    void prepareNextShortcut()
    {
        nextNoteOnEvent[currentPlayingShortcut] =
            sampleCounter + pauseInSamples[currentPlayingShortcut - 1];
    }

    void processSimultaneousShortcuts(int /*firstShortcutRef*/, int sampleRef)
    {
        for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
        {
            if (isActive[i])
            {
                if (sampleRef == nextNoteOnEvent[i])
                {
                    triggerOnEvent(i);
                    nextNoteOffEvent[i] = sampleRef + lengthInSamples[i];
                    currentRepeat[i]++;
                }
                else if (sampleRef == nextNoteOffEvent[i])
                {
                    triggerOffEvent(i);

                    if (currentRepeat[i] < numRepeats[i])
                    {
                        nextNoteOnEvent[i] = sampleRef + pauseInSamples[i];
                    }
                }
            }
        }
    }

    void clearOpenRepeats()
    {
        for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
        {
            currentRepeat[i]       = 0;
            currentPlayingShortcut = 0;
            nextNoteOnEvent[i]     = 0;
        }
    }

    void resetTick()
    {
        calculatePlaybackTimers();

        sampleCounter = 0;
        clearOpenRepeats();

        if (playSimultaneous)
        {
            currentPlayingShortcut = 0;
        }
        else
        {
            currentPlayingShortcut = 0;

            for (int c = 0; c < NUM_SHORTCUT_SYNTHS; c++)
            {
                if (isActive[c] && !isShortcutMuted(c))
                {
                    currentPlayingShortcut = c;
                    break;
                }
            }
        }
    }

    void calculatePlaybackTimers()
    {
        totalNumSamplesOfLoop = 0;

        if (playSimultaneous)
        {
            int shortcutValInSamps = 0;

            for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
            {
                if (isActive[i] && !isShortcutMuted(i))
                {
                    shortcutValInSamps = numRepeats[i] * (lengthInSamples[i] + pauseInSamples[i]);
                }

                if (shortcutValInSamps > static_cast<int>(totalNumSamplesOfLoop))
                {
                    totalNumSamplesOfLoop = shortcutValInSamps;
                }
            }
        }
        else
        {
            for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
            {
                if (isActive[i] && !isShortcutMuted(i))
                {
                    int shortcutValInSamps = numRepeats[i] * (lengthInSamples[i] + pauseInSamples[i]);
                    totalNumSamplesOfLoop += shortcutValInSamps;
                }
            }
        }

        totalMSOfLoop = static_cast<int>(totalNumSamplesOfLoop / sampleRate * 1000);
    }

    // =========================================================
    // Member Data
    // =========================================================
    PLAY_MODE  playMode;
    PLAY_STATE playState;

    bool playSimultaneous;

    int    currentPlayingShortcut = 0;
    int    sampleCounter;
    int    numSamplesPerMS;
    double sampleRate;

    bool shouldProcess;

    // Per-shortcut parameters
    bool   isActive[NUM_SHORTCUT_SYNTHS];
    int    numRepeats[NUM_SHORTCUT_SYNTHS];
    int    lengthMS[NUM_SHORTCUT_SYNTHS];
    int    lengthInSamples[NUM_SHORTCUT_SYNTHS];
    int    pauseMS[NUM_SHORTCUT_SYNTHS];
    int    pauseInSamples[NUM_SHORTCUT_SYNTHS];
    int    totalMSOfLoop;
    uint64 totalNumSamplesOfLoop;
    int    currentMS;

    // Events
    int  nextNoteOnEvent[NUM_SHORTCUT_SYNTHS];
    int  nextNoteOffEvent[NUM_SHORTCUT_SYNTHS];
    int  currentRepeat[NUM_SHORTCUT_SYNTHS];
    bool isPaused[NUM_SHORTCUT_SYNTHS];
};
