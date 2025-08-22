/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "MainViewComponent.h"
#include "ProjectManager.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public AudioAppComponent //public AudioSource, public Component
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override
    {
        mainViewComponent->setBounds(0, 0, mainWidth * scaleFactor, mainHeight * scaleFactor);
    }
    
    float scaleFactor = 0.5;
    void setScale(float factor)
    {
        scaleFactor = factor;
        
        mainViewComponent->setScale(scaleFactor);
        
        setSize(mainWidth * scaleFactor, mainHeight * scaleFactor);
    }
    
    MainComponent * getMainComponent() {return this; }

    int mainHeight          = 1440 + 300;
    int mainWidth           = 1566 + 354;
    
    std::unique_ptr<MainViewComponent> mainViewComponent;
    
    std::unique_ptr<ProjectManager> projectManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
