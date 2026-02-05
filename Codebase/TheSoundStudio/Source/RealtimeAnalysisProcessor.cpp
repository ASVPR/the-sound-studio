/*
  ==============================================================================

    RealtimeAnalysisProcessor.cpp
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#include "RealtimeAnalysisProcessor.h"
#include "ProjectManager.h"

RealtimeAnalysisProcessor::RealtimeAnalysisProcessor(ProjectManager& pm)
    : projectManager(pm)
{
}

RealtimeAnalysisProcessor::~RealtimeAnalysisProcessor()
{
}

void RealtimeAnalysisProcessor::prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock)
{
}

void RealtimeAnalysisProcessor::releaseResources()
{
}

void RealtimeAnalysisProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    if (!playing.load())
        return;

    // Data routing is handled in ProjectManager::processRealtimeAnalysis
}

void RealtimeAnalysisProcessor::setPlayerCommand(PLAYER_COMMANDS command)
{
    switch (command)
    {
        case COMMAND_PLAYER_PLAYPAUSE:
            playing.store(!playing.load());
            break;
        case COMMAND_PLAYER_STOP:
            playing.store(false);
            break;
        default:
            break;
    }
}