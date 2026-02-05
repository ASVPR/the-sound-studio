/*
  ==============================================================================

    MainComponent.cpp
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

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
    
    // Set initial scale factor based on screen size
    auto screenBounds = Desktop::getInstance().getDisplays().getPrimaryDisplay()->userArea;
    float screenScale = jmin(screenBounds.getWidth() / (float)mainWidth, 
                             screenBounds.getHeight() / (float)mainHeight);
    scaleFactor = jlimit(0.3f, 1.0f, screenScale * 0.8f); // Start at 80% of screen size
    
    setScale(scaleFactor);
    
    projectManager->initGUISync();
}

MainComponent::~MainComponent()
{
    // Ensure safe teardown order to avoid use-after-free on close
    // 1) Stop audio to prevent callbacks during destruction
    shutdownAudio();
    // 2) Destroy UI (and its children) BEFORE destroying the ProjectManager,
    //    because many UI components unregister from ProjectManager in their destructors.
    if (mainViewComponent)
        mainViewComponent.reset();
    // 3) Now it is safe to destroy the ProjectManager
    if (projectManager)
        projectManager.reset();
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
