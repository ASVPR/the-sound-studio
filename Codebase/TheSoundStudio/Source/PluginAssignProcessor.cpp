/*
  ==============================================================================

    PluginAssignProcessor.cpp

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#include "PluginAssignProcessor.h"
//#include "PluginProcessor.h"

PluginAssignProcessor::PluginAssignProcessor(AudioPluginFormatManager & _formatManager, KnownPluginList * pluginList):
formatManager(_formatManager),
pluginList(pluginList)
{
    hasPlugin = false;
}

const String PluginAssignProcessor::getName() const
{
   if (hasPlugin)
   {
        return instance->getName();
   }
   else return "no plugin";
}

int PluginAssignProcessor::getNumParameters()
{
    return 0;
}

float PluginAssignProcessor::getParameter (int index)
{
    return 0; // should prob return 0 in this case
}

void PluginAssignProcessor::setParameter (int index, float newValue)
{
}

const String PluginAssignProcessor::getParameterName (int index)
{
    return "no param";
}

const String PluginAssignProcessor::getParameterText (int index)
{
    return "no text";
}

const String PluginAssignProcessor::getInputChannelName (const int channelIndex) const
{
    return String (channelIndex + 1);
}

const String PluginAssignProcessor::getOutputChannelName (const int channelIndex) const
{
    return String (channelIndex + 1);
}

bool PluginAssignProcessor::isInputChannelStereoPair (int index) const
{
    return false;
}

bool PluginAssignProcessor::isOutputChannelStereoPair (int index) const
{
    return false;
}

bool PluginAssignProcessor::acceptsMidi() const
{
    return true;
}

bool PluginAssignProcessor::producesMidi() const
{
    return true;
}

//==============================================================================
void PluginAssignProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    
    this->setPlayConfigDetails(getNumInputChannels(), getNumOutputChannels(), sampleRate, samplesPerBlock);

    if (hasPlugin) {
        instance->prepareToPlay(sampleRate, samplesPerBlock);
    }

}

bool PluginAssignProcessor::pluginIsLoaded()
{
    return hasPlugin;
}

void PluginAssignProcessor::processBlock (AudioBuffer<float>&buffer, MidiBuffer&midiBuffer)
{
    int numChannels = buffer.getNumChannels();
    {
        if (hasPlugin && instance)
        {
            int bufNumChan = buffer.getNumChannels();
            if (bufNumChan == 1)
            {
                // check if is stereo plugin
                int numChan = instance->getNumInputChannels();
                if (numChan > 1)
                {
                    AudioBuffer<float> stereoBuffer;
                    stereoBuffer.setSize(2, buffer.getNumSamples());
                    stereoBuffer.clear();
                    stereoBuffer.copyFrom(0, 0, buffer, 0, 0, buffer.getNumSamples());
                    stereoBuffer.copyFrom(1, 0, buffer, 0, 0, buffer.getNumSamples());

                    instance->processBlock(stereoBuffer, midiBuffer);
                    
                    // add single channel back to main buffer
                    buffer.copyFrom(0, 0, stereoBuffer, 0, 0, buffer.getNumSamples());

                }
                else
                {
                    instance->processBlock(buffer, midiBuffer);
                }
            }
            else
            {
                if (buffer.getNumChannels() == 2)
                {
                    // check stereo
                    instance->processBlock(buffer, midiBuffer);
                }
                else
                {

                }
                
            }
        }
    }
}

bool PluginAssignProcessor::clearPlugin()
{
    suspendProcessing(true);
    
//    delete instance;
    instance = nullptr;
    hasPlugin = false;
    
    suspendProcessing(false);
    
    if (instance == nullptr)
    {
        return true;
    } else  return false;
}


bool PluginAssignProcessor::loadNewPlugin(const PluginDescription* desc)
{
    // FIXED: Comprehensive error handling and input validation
    if (desc == nullptr) {
        DBG("PluginAssignProcessor::loadNewPlugin - null description provided");
        return false;
    }
    
    try {
        suspendProcessing(true);
        
        // Clear existing plugin safely
        if (hasPlugin && instance != nullptr) {
            instance = nullptr;
            hasPlugin = false;
        }
        
        // Validate channel configuration before creating plugin
        int numInputChans = desc->numInputChannels;
        int numOutputChans = desc->numOutputChannels;
        
        if (numInputChans > 2 || numOutputChans > 2) {
            DBG("Plugin rejected: too many channels (In:" << numInputChans << ", Out:" << numOutputChans << ")");
            suspendProcessing(false);
            return false;
        }
        
        // Attempt to create plugin instance with error handling
        String errorMessage;
        instance = formatManager.createPluginInstance(*desc, getSampleRate(), getBlockSize(), errorMessage);
        
        if (instance == nullptr) {
            DBG("Failed to create plugin instance: " << errorMessage);
            hasPlugin = false;
            suspendProcessing(false);
            return false;
        }
        
        // Configure and initialize plugin safely
        try {
            instance->setPlayConfigDetails(numInputChans, numOutputChans, getSampleRate(), getBlockSize());
            instance->prepareToPlay(getSampleRate(), getBlockSize());
            hasPlugin = true;
            
            DBG("Successfully loaded plugin: " << desc->name);
        }
        catch (const std::exception& e) {
            DBG("Failed to initialize plugin: " << e.what());
            instance = nullptr;
            hasPlugin = false;
            suspendProcessing(false);
            return false;
        }
        
        suspendProcessing(false);
        return true;
    }
    catch (const std::exception& e) {
        DBG("Exception in loadNewPlugin: " << e.what());
        instance = nullptr;
        hasPlugin = false;
        suspendProcessing(false);
        return false;
    }
}

void callToAddFilter()
{}

void PluginAssignProcessor::releaseResources()
{
    if (hasPlugin)
    {
       // instance->releaseResources();
    }
}

//==============================================================================
void PluginAssignProcessor::getStateInformation (MemoryBlock& destData)
{
    if (hasPlugin) {
      //  instance->getStateInformation(destData);
    }
}

void PluginAssignProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (hasPlugin) {
      //  instance->setStateInformation(data, sizeInBytes);
    }
}
double PluginAssignProcessor::getTailLengthSeconds() const
{
    return 0.0;
}
