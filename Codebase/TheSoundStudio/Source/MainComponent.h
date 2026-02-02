/*
  ==============================================================================

    MainComponent.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "MainViewComponent.h"
#include "ProjectManager.h"
#include "ResponsiveUIHelper.h"

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
        // Use responsive layout helper for optimal scaling
        auto bounds = getLocalBounds();
        
        // Calculate optimal scale using responsive helper
        scaleFactor = ResponsiveUIHelper::calculateOptimalScale(
            bounds.getWidth(), bounds.getHeight(), mainWidth, mainHeight);
        
        if (mainViewComponent)
        {
            mainViewComponent->setBounds(bounds);
            mainViewComponent->setScale(scaleFactor);
        }
    }
    
    float scaleFactor = 1.0f;
    void setScale(float factor)
    {
        scaleFactor = factor;
        
        if (mainViewComponent)
            mainViewComponent->setScale(scaleFactor);
        
        // Set window size based on scale factor
        setSize(mainWidth * scaleFactor, mainHeight * scaleFactor);
    }
    
    MainComponent * getMainComponent() {return this; }

    int mainHeight          = 1440 + 300;
    int mainWidth           = 1566 + 354;
    
    std::unique_ptr<MainViewComponent> mainViewComponent;
    
    std::unique_ptr<ProjectManager> projectManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
