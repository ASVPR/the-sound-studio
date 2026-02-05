/*
  ==============================================================================

    AnalyzerPool.h
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "AnalyzerNew.h"
#include "TSSConstants.h"
#include <array>

class AnalyzerPool
{
public:
    static constexpr int kMaxAnalyzers = TSS::Audio::kMaxAnalyzers;

    AnalyzerPool() = default;

    void stopAllThreads(int timeoutMs)
    {
        for (int i = 0; i < kMaxAnalyzers; ++i)
            analyzers[i].stopThread(timeoutMs);
    }

    void setupAll(int sampleRateInt, float sampleRateFloat)
    {
        for (int i = 0; i < kMaxAnalyzers; ++i)
            analyzers[i].setupAnalyser(sampleRateInt, sampleRateFloat);
    }

    void setNewFFTSizeAll(int fftSizeEnumValue)
    {
        for (int i = 0; i < kMaxAnalyzers; ++i)
            analyzers[i].setNewFFTSize(fftSizeEnumValue);
    }

    void setNewFFTWindowFunctionAll(int windowEnum)
    {
        for (int i = 0; i < kMaxAnalyzers; ++i)
            analyzers[i].setNewFFTWindowFunction(windowEnum);
    }

    void initFFTAll()
    {
        for (int i = 0; i < kMaxAnalyzers; ++i)
            analyzers[i].initFFT();
    }

    // Audio thread: push sample data via addAudioData
    void addAudioData(int channel, const juce::AudioBuffer<float>& buffer,
                      int startChannel, int numChannels)
    {
        if (channel >= 0 && channel < kMaxAnalyzers)
            analyzers[channel].addAudioData(buffer, startChannel, numChannels);
    }

    // UI thread: get the analyzer for a specific channel (for spectrum display)
    AnalyserNew<float>& getAnalyzer(int channel)
    {
        jassert(channel >= 0 && channel < kMaxAnalyzers);
        return analyzers[channel];
    }

    const AnalyserNew<float>& getAnalyzer(int channel) const
    {
        jassert(channel >= 0 && channel < kMaxAnalyzers);
        return analyzers[channel];
    }

private:
    std::array<AnalyserNew<float>, kMaxAnalyzers> analyzers;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalyzerPool)
};
