/*
  ==============================================================================

    FeedbackModuleProcessor.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "FundamentalFrequencyProcessor.h"

class ProjectManager;

class FeedbackModuleProcessor : public AudioProcessor
{
public:
    FeedbackModuleProcessor(ProjectManager& pm, FrequencyManager& fm);
    
    void prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock) override;
    void setParameter(int index, var newValue);
    void processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override;
    
    void clearAlgorithms();
    
    double getTailLengthSeconds() const override { return 0.0; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    void releaseResources()override { }
    AudioProcessorEditor* createEditor() override {  return 0; }
    bool hasEditor() const override {  return false; }
    const String getName() const  override { return "FundamentalFrequencyProcessor"; }
    int getNumPrograms()override { return 0; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int index) override { }
    const String getProgramName (int index) override { return ""; }
    void changeProgramName (int index, const String& newName) override { }
    void getStateInformation (juce::MemoryBlock& destData) override {}
    void setStateInformation (const void* data, int sizeInBytes)override {}
    ProjectManager& getProjectManager() { return projectManager; }
    juce::StringArray getInputNames();

    juce::StringArray getOutputNames();
    FundamentalFrequencyProcessor& getFundamentalFrequencyProcessor(int index);
private:
    ProjectManager& projectManager;
    
    std::vector<std::unique_ptr<FundamentalFrequencyProcessor>> fundamentalFrequencyProcessor;
};
