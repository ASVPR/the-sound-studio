/*
  ==============================================================================

    RealtimeAnalysisComponent.h
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "VisualiserContainerComponent.h"
#include "OscilloscopeVisualiserComponent.h"
#include "ProjectManager.h"
#include "MenuViewInterface.h"
#include <memory>

class RealtimeAnalysisComponent :
    public MenuViewInterface,
    /*public Timer,*/
    public Button::Listener,
    public ProjectManager::UIListener
{
public:
    RealtimeAnalysisComponent(ProjectManager * pm)
    {
        projectManager = pm;
        projectManager->addUIListener(this);
        
        // fonts
        Typeface::Ptr AssistantBold     = Typeface::createSystemTypefaceFor(BinaryData::AssistantBold_ttf, BinaryData::AssistantBold_ttfSize);
        Typeface::Ptr AssistantSemiBold   = Typeface::createSystemTypefaceFor(BinaryData::AssistantSemiBold_ttf, BinaryData::AssistantSemiBold_ttfSize);
        Font fontBold(AssistantBold);
        Font fontNormal(AssistantSemiBold);
        
        fontNormal.setHeight(33);
        fontBold.setHeight(38);
        
        imageBackground                     = ImageCache::getFromMemory(BinaryData::RealtimeAnalysisBackground_png, BinaryData::RealtimeAnalysisBackground_pngSize);
        imageStopButton                     = ImageCache::getFromMemory(BinaryData::stop2x_png, BinaryData::stop2x_pngSize);
        imagePlayButton                     = ImageCache::getFromMemory(BinaryData::playPause2x_png, BinaryData::playPause2x_pngSize);
        imageRecordButton                   = ImageCache::getFromMemory(BinaryData::RecordButton_png, BinaryData::RecordButton_pngSize);
        
        visualiserContainerComponent        = std::make_unique<VisualiserContainerComponent2>(projectManager, AUDIO_MODE::MODE_REALTIME_ANALYSIS);
        
        addAndMakeVisible(visualiserContainerComponent.get());
        
        button_Play = std::make_unique<ImageButton>();
        button_Play->setTriggeredOnMouseDown(true);
        button_Play->setImages (false, true, true,
                                  imagePlayButton, 0.999f, Colour (0x00000000),
                                  Image(), 1.000f, Colour (0x00000000),
                                  imagePlayButton, 0.6, Colour (0x00000000));
        button_Play->addListener(this);
        addAndMakeVisible(button_Play.get());
        
        button_Stop = std::make_unique<ImageButton>();
        button_Stop->setTriggeredOnMouseDown(true);
        button_Stop->setImages (false, true, true,
                                  imageStopButton, 0.999f, Colour (0x00000000),
                                  Image(), 1.000f, Colour (0x00000000),
                                  imageStopButton, 0.6, Colour (0x00000000));
        button_Stop->addListener(this);
        addAndMakeVisible(button_Stop.get());
        
        button_Record = std::make_unique<ImageButton>();
        button_Record->setTriggeredOnMouseDown(true);
        button_Record->setImages (false, true, true,
                                  imageRecordButton, 0.999f, Colour (0x00000000),
                                  Image(), 1.000f, Colour (0x00000000),
                                  imageRecordButton, 0.6, Colour (0x00000000));
        button_Record->addListener(this);
        addAndMakeVisible(button_Record.get());
        
        
//        startTimer(TIMER_UPDATE_RATE);
    }
    
    ~RealtimeAnalysisComponent()
    {
        
    }
    
    
    void paint (Graphics&) override;
    void resized() override
    {
        // Reserve space at the bottom for transport buttons
        const int buttonAreaHeight = roundToInt(80 * scaleFactor);
        const int buttonY = getHeight() - buttonAreaHeight + roundToInt(10 * scaleFactor);

        // Visualiser fills area above buttons
        visualiserContainerComponent->setBounds(0, 0, getWidth(), getHeight() - buttonAreaHeight);

        // Transport buttons centered at bottom
        const int buttonWidth = roundToInt(playWidth * scaleFactor);
        const int buttonHeight = roundToInt(playHeight * scaleFactor);
        const int recordSize = roundToInt(70 * scaleFactor);
        const int centerX = getWidth() / 2;
        const int buttonSpacing = roundToInt(20 * scaleFactor);

        // Record button on the left
        button_Record->setBounds(roundToInt(20 * scaleFactor), buttonY, recordSize, recordSize);

        // Play and Stop buttons centered
        button_Play->setBounds(centerX - buttonWidth - buttonSpacing/2, buttonY, buttonWidth, buttonHeight);
        button_Stop->setBounds(centerX + buttonSpacing/2, buttonY, buttonWidth, buttonHeight);
    }
    
    float scaleFactor = 0.5;
    void setScale(float factor) override
    {
        scaleFactor = factor;
        
        visualiserContainerComponent    ->setScale(scaleFactor);
//        oscilloscopeComponent       ->setScale(scaleFactor);
        
        lookAndFeel.setScale(scaleFactor);
    }
    
    void closePopups()
    {
        visualiserContainerComponent->setPopupsAreVisible(false);
    }
    
//    void timerCallback() override
//    {
//        if (projectManager->mode == AUDIO_MODE::MODE_REALTIME_ANALYSIS)
//        {
//            // visualiser
//            if (shouldPlayAnalysis && !shouldFreeze)
//            {
//                visualiserContainerComponent->pushUpdate();
//            }
//        }
//    }
    
    void buttonClicked (Button*button)override
    {
        if (button == button_Play.get())
        {
            if (shouldPlayAnalysis && shouldFreeze == false)
            {
                shouldFreeze = true;
                

            }
            else if (shouldPlayAnalysis && shouldFreeze == true)
            {
                shouldFreeze = false;
            }
            else
            {
                shouldFreeze = false;
                
                shouldPlayAnalysis = true;
                
                projectManager->setPlayerCommand(PLAYER_COMMANDS::COMMAND_PLAYER_PLAYPAUSE);
                projectManager->logFileWriter->createNewFileForRealtimeAnalysisLogging();
                projectManager->logFileWriter->startRecordingLog();
            }
            

            
        }
        else if (button == button_Stop.get())
        {
            shouldPlayAnalysis = false;
            
            projectManager->setPlayerCommand(PLAYER_COMMANDS::COMMAND_PLAYER_STOP);
            projectManager->logFileWriter->stopRecordingLog();
        }
        else if (button == button_Export.get())
        {
            
        }
        else if (button == button_Record.get())
        {
            isRecording = !isRecording;
            
            if (!isRecording)
            {
                // if its stopping recording, offer user to export data, or delete
                
                projectManager->stopRecording();
//                projectManager->logFileWriter->stopRecordingLog();
            }
            else
            {
                projectManager->createNewFileForRecordingRealtimeAnalysis();
                
                projectManager->startRecording();
                
            }
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
    
    std::unique_ptr<ComboBox> comboBoxInput;
    
    Label * labelKeynote { nullptr };
    Label * labelOctave { nullptr };
    Label * labelChord { nullptr };
    Label * labelFrequency { nullptr };
    
    std::unique_ptr<ImageButton> button_Play;
    std::unique_ptr<ImageButton> button_Stop;
    std::unique_ptr<ImageButton> button_Record;
    std::unique_ptr<ImageButton> button_Export;
    
    Image imageStopButton;
    Image imagePlayButton;
    Image imageRecordButton;
    Image imageExportButton;
    Image imageBackground;
    
    ImageComponent * imageComp { nullptr };
    
    
    
    int playLeftMargin      = 521;
    int playTopMargin       = 1320;
    int playWidth           = 249;
    int playHeight          = 61;
    
    int stopLeftMargin      = 802;
    int stopTopMargin       = playTopMargin;
    
    bool isRecording = false;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RealtimeAnalysisComponent)
};
