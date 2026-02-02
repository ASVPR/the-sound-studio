/*
  ==============================================================================

    TSSProcessorBase.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Parameters.h"
#include "AudioRouting.h"
#include "TSSConstants.h"

class TSSProcessorBase
{
public:
    TSSProcessorBase() = default;
    virtual ~TSSProcessorBase() = default;

    virtual void prepareToPlay(double sampleRate, int maxBlockSize)
    {
        currentSampleRate = sampleRate;
        currentBlockSize  = maxBlockSize;
    }

    virtual void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) = 0;

    virtual void triggerNoteOn(int shortcutRef) = 0;
    virtual void triggerNoteOff(int shortcutRef) = 0;
    virtual void panic() = 0;

    double getSampleRate() const { return currentSampleRate; }
    int    getBlockSize()  const { return currentBlockSize; }

protected:
    void routeOutput(juce::AudioBuffer<float>& destination,
                     const juce::AudioBuffer<float>& source,
                     AUDIO_OUTPUTS output)
    {
        TSS::Audio::routeToOutput(destination, source, output);
    }

    double currentSampleRate = TSS::Audio::kDefaultSampleRate;
    int    currentBlockSize  = 512;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TSSProcessorBase)
};
