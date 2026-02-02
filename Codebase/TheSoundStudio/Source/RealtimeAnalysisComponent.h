/*
  ==============================================================================

    RealtimeAnalysisComponent.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "VisualiserContainerComponent.h"
#include "OscilloscopeVisualiserComponent.h"
#include "ProjectManager.h"
#include "TransportToolbarComponent.h"
#include "MenuViewInterface.h"
#include "UI/DesignSystem.h"
#include <memory>

class RealtimeAnalysisComponent :
    public MenuViewInterface,
    public Button::Listener,
    public ProjectManager::UIListener,
    public TransportToolbarComponent::Listener
{
public:
    RealtimeAnalysisComponent(ProjectManager * pm)
    {
        projectManager = pm;
        projectManager->addUIListener(this);

        visualiserContainerComponent = std::make_unique<VisualiserContainerComponent2>(projectManager, AUDIO_MODE::MODE_REALTIME_ANALYSIS);
        addAndMakeVisible(visualiserContainerComponent.get());

        // Transport toolbar with Play/Stop/Record only (no panic, loop, simultaneous, save, load)
        TransportToolbarComponent::ButtonVisibility toolbarVis;
        toolbarVis.play = true;
        toolbarVis.stop = true;
        toolbarVis.record = true;
        toolbarVis.panic = false;
        toolbarVis.progress = false;
        toolbarVis.loop = false;
        toolbarVis.simultaneous = false;
        toolbarVis.save = false;
        toolbarVis.load = false;
        toolbarVis.playingLabel = false;
        transportToolbar = std::make_unique<TransportToolbarComponent>(toolbarVis);
        transportToolbar->addTransportListener(this);
        addAndMakeVisible(transportToolbar.get());
    }

    ~RealtimeAnalysisComponent() { }

    void paint (Graphics&) override;
    void resized() override
    {
        auto bounds = getLocalBounds();
        const int h = bounds.getHeight();

        // Toolbar at bottom
        int toolbarH = jmax(50, (int)(h * TSS::Design::Layout::kToolbarHeightRatio * 0.5f));
        transportToolbar->setBounds(bounds.removeFromBottom(toolbarH));

        // Visualiser fills remainder
        visualiserContainerComponent->setBounds(bounds);
    }

    float scaleFactor = 0.5;
    void setScale(float factor) override
    {
        scaleFactor = factor;

        visualiserContainerComponent->setScale(scaleFactor);
        lookAndFeel.setScale(scaleFactor);
    }

    void closePopups()
    {
        visualiserContainerComponent->setPopupsAreVisible(false);
    }

    void buttonClicked (Button*button) override { }

    // TransportToolbarComponent::Listener
    void transportPlayClicked() override
    {
        if (!shouldPlayAnalysis)
        {
            shouldPlayAnalysis = true;
            shouldFreeze = false;
            projectManager->logFileWriter->createNewFileForRealtimeAnalysisLogging();
            projectManager->logFileWriter->startRecordingLog();
        }
        else
        {
            shouldFreeze = !shouldFreeze;
        }
    }

    void transportStopClicked() override
    {
        shouldPlayAnalysis = false;
        shouldFreeze = false;
        projectManager->logFileWriter->stopRecordingLog();
    }

    void transportRecordClicked() override
    {
        isRecording = !isRecording;
        if (!isRecording)
        {
            projectManager->stopRecording();
        }
        else
        {
            projectManager->createNewFileForRecordingRealtimeAnalysis();
            projectManager->startRecording();
        }
    }

private:

    bool shouldPlayAnalysis = false;
    bool shouldFreeze = false;

    ProjectManager *        projectManager;
    AudioDeviceManager *    deviceManager;
    AudioIODeviceType *     type;

    CustomLookAndFeel lookAndFeel;

    std::unique_ptr<VisualiserContainerComponent2> visualiserContainerComponent;
    std::unique_ptr<TransportToolbarComponent> transportToolbar;

    bool isRecording = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RealtimeAnalysisComponent)
};
