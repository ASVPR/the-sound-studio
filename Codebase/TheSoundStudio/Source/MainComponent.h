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
        // Use responsive layout for proper scaling
        auto bounds = getLocalBounds();
        
        // Calculate responsive scale factor based on window size
        float widthScale = bounds.getWidth() / (float)mainWidth;
        float heightScale = bounds.getHeight() / (float)mainHeight;
        scaleFactor = jmin(widthScale, heightScale);
        scaleFactor = jlimit(0.3f, 2.0f, scaleFactor); // Limit scale between 0.3x and 2x
        
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
