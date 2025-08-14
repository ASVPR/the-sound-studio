/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    projectManager = std::make_unique<ProjectManager>();
    
    projectManager->setDeviceManager(&deviceManager);
    

    if (RuntimePermissions::isRequired (RuntimePermissions::recordAudio)
        && ! RuntimePermissions::isGranted (RuntimePermissions::recordAudio))
    {
        RuntimePermissions::request (RuntimePermissions::recordAudio,
                                     [&] (bool granted) { if (granted)  setAudioChannels (4, 4); });
    }
    else
    {
        setAudioChannels (4, 4);
    }
    
    mainViewComponent = std::make_unique<MainViewComponent>(projectManager.get());
    
    addAndMakeVisible(mainViewComponent.get());

    projectManager->setMainComponent(this);
    
    scaleFactor = 0.5;

    setScale(scaleFactor);
    
    projectManager->initGUISync();
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    if (projectManager)
    {
        projectManager->prepareToPlay(samplesPerBlockExpected, sampleRate);
    }
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    projectManager->getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    projectManager->releaseResources();
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll ({45,44,44});

    // You can add your drawing code here!
}
