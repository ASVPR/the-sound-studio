/*
  ==============================================================================

    FeedbackModuleProcessor.cpp

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#include "FeedbackModuleProcessor.h"
#include "ProjectManager.h"


FeedbackModuleProcessor::FeedbackModuleProcessor(ProjectManager& pm, FrequencyManager& fm) :
projectManager(pm)
{
    for (auto i = 0; i < 4; i++)
    {
        fundamentalFrequencyProcessor.push_back(std::make_unique<FundamentalFrequencyProcessor>(pm, fm));
    }
}

void FeedbackModuleProcessor::prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock)
{
    for (auto& p : fundamentalFrequencyProcessor)
    {
        p->prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);
    }
}

void FeedbackModuleProcessor::setParameter(int index, var newValue)
{
    
}

void FeedbackModuleProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
//    for (auto& p : frequencyProcessor)
//    {
//        p->processBlock(buffer, midiMessages);
//    }
}

void FeedbackModuleProcessor::clearAlgorithms()
{
    
}

juce::StringArray FeedbackModuleProcessor::getInputNames()
{
    auto device = projectManager.getDeviceManager()->getCurrentAudioDevice();

    return device->getInputChannelNames();
}

juce::StringArray FeedbackModuleProcessor::getOutputNames()
{
    auto device = projectManager.getDeviceManager()->getCurrentAudioDevice();

    return device->getOutputChannelNames();
}

FundamentalFrequencyProcessor &FeedbackModuleProcessor::getFundamentalFrequencyProcessor(int index)
{
    jassert(index >= 0 && index < fundamentalFrequencyProcessor.size());
    return *fundamentalFrequencyProcessor.at(index);
}

