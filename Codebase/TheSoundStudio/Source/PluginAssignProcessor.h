/*
  ==============================================================================

    PluginAssignProcessor.h
    Created: 25 Aug 2015 9:57:39pm
    Author:  Gary Jones

  ==============================================================================
*/

/*
 ==============================================================================
 
 PluginAssignProcessor.h
 Created: 21 Jan 2014 4:53:52pm
 Author:  Gary Jones
 
 ==============================================================================
 */

#ifndef PluginAssignProcessor_H_INCLUDED
#define PluginAssignProcessor_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Delay.h"

class PluginAssignProcessor : public AudioProcessor //public AudioProcessorGraph
{
public:
    
    PluginAssignProcessor(AudioPluginFormatManager & _formatManager, KnownPluginList * pluginList);
    ~PluginAssignProcessor(){ }

    const String getName() const;
    void processBlock (AudioBuffer<float>&, MidiBuffer&);
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void releaseResources();
    
    bool loadPluginRef(int newValue)
    {
        bool didLoad = false;
        if (newValue != -1)
        {
            PluginDescription * plugResult = pluginList->getType((int)newValue);
            if (plugResult)
            {
                didLoad = loadNewPlugin(plugResult);
            }
        }

        return didLoad;
    }
    
    bool loadPluginFromIdentifierString(String identifier)
    {
        bool didLoad = false;
        std::unique_ptr<PluginDescription> plugResult = pluginList->getTypeForIdentifierString(identifier);
        
        if (plugResult)
        {
            didLoad = loadNewPluginNew(*plugResult);

        }
        
        return didLoad;
    }
    
    bool loadNewPluginNew(PluginDescription desc)
    {
        suspendProcessing(true);
        
        if (pluginIsLoaded())
        {
            instance = nullptr;
            
            //recreate it
            String errorMessage;
            instance = formatManager.createPluginInstance(desc, getSampleRate(), getBlockSize(), errorMessage);

            int numInputChans = desc.numInputChannels;
            int numOutputChans = desc.numOutputChannels;
            
            if (numInputChans > 2 || numOutputChans > 2)
            {
                return false;
            }
            else
            {
                instance->setPlayConfigDetails(numInputChans, numOutputChans, getSampleRate(), getBlockSize());
            }

            instance->prepareToPlay(getSampleRate(), getBlockSize());
            hasPlugin = true;
        }
        else
        {
            //recreate it
            String errorMessage;
            instance = formatManager.createPluginInstance(desc, getSampleRate(), getBlockSize(), errorMessage);
            
            int numInputChans = desc.numInputChannels;
            int numOutputChans = desc.numOutputChannels;
            
            if (numInputChans > 2 || numOutputChans > 2)
            {
                return false;
            }
            else
            {
                instance->setPlayConfigDetails(numInputChans, numOutputChans, getSampleRate(), getBlockSize());
            }

            if (instance != nullptr)
            {
                instance->setPlayConfigDetails(numInputChans, numOutputChans, getSampleRate(), getBlockSize());
                instance->prepareToPlay(getSampleRate(), getBlockSize());
                hasPlugin = true;
            }
            else
            {
              //  AlertWindow::showMessageBox (AlertWindow::WarningIcon, TRANS("Couldn't create filter"), errorMessage);
                hasPlugin = false;
            }
        }
        
        suspendProcessing(false);
        
        if (hasPlugin) return true;
        else return false;
    }
    
    bool loadNewPlugin(const PluginDescription*desc);
    bool clearPlugin();
    
    AudioProcessorEditor* createEditor() { return 0; };
    
    int getNumParameters();
    float getParameter (int index);
    void setParameter (int index, float newValue);
    const String getParameterName (int index);
    const String getParameterText (int index);
    const String getInputChannelName (const int channelIndex) const;
    const String getOutputChannelName (const int channelIndex) const;
    bool isInputChannelStereoPair (int index) const;
    bool isOutputChannelStereoPair (int index) const;
    bool acceptsMidi() const;
    bool producesMidi() const;
    bool silenceInProducesSilenceOut() const {return false;}
    double getTailLengthSeconds() const;
    bool hasEditor() const { return true; };
    int getNumPrograms()                                        { return 0; }
    int getCurrentProgram()                                     { return 0; }
    void setCurrentProgram (int index)                          { }
    const String getProgramName (int index)                     { return ""; }
    void changeProgramName (int index, const String& newName)   { }
    void getStateInformation (MemoryBlock& destData);
    void setStateInformation (const void* data, int sizeInBytes);
    
    bool hasPlugin = false;
    bool pluginIsLoaded();
    
    AudioPluginFormatManager & formatManager;
    KnownPluginList * pluginList;
    std::unique_ptr<AudioPluginInstance> instance;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginAssignProcessor);
};




#endif  // PLUGINASSIGNPROCESSOR_H_INCLUDED
