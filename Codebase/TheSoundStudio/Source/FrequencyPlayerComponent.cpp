/*
  ==============================================================================

    FrequencyPlayerComponent.cpp
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "FrequencyPlayerComponent.h"

//==============================================================================//==============================================================================
//==============================================================================
FrequencyPlayerComponent::FrequencyPlayerComponent(ProjectManager * pm)
{
    projectManager = pm;
    projectManager->addUIListener(this);
    
    setWantsKeyboardFocus(true);
    
    // fonts
    Typeface::Ptr AssistantBold     = Typeface::createSystemTypefaceFor(BinaryData::AssistantBold_ttf, BinaryData::AssistantBold_ttfSize);
    Typeface::Ptr AssistantSemiBold   = Typeface::createSystemTypefaceFor(BinaryData::AssistantSemiBold_ttf, BinaryData::AssistantSemiBold_ttfSize);
    Font fontBold(AssistantBold);
    Font fontNormal(AssistantSemiBold);
    
    fontNormal.setHeight(33);
    fontBold.setHeight(33);
    
    // images
    imageCheckboxBackground             = ImageCache::getFromMemory(BinaryData::CheckboxBackground2x_png, BinaryData::CheckboxBackground2x_pngSize);
    imageMainContainerBackground        = ImageCache::getFromMemory(BinaryData::MainContainerBackground2x_png, BinaryData::MainContainerBackground2x_pngSize);
    imageShortcutContainerBackground    = ImageCache::getFromMemory(BinaryData::ShortcutContainerBackground2x_png, BinaryData::ShortcutContainerBackground2x_pngSize);
    imageShortcutBackground             = ImageCache::getFromMemory(BinaryData::ShortcutBackground2x_png, BinaryData::ShortcutBackground2x_pngSize);
    imagePanicButton                    = ImageCache::getFromMemory(BinaryData::PanicButton2x_png, BinaryData::PanicButton2x_pngSize);
    imagePlayButton                     = ImageCache::getFromMemory(BinaryData::playPause2x_png, BinaryData::playPause2x_pngSize);
    imageProgresBarFill                 = ImageCache::getFromMemory(BinaryData::ProgressBarFill2x_png, BinaryData::ProgressBarFill2x_pngSize);
    imageSettingsIcon                   = ImageCache::getFromMemory(BinaryData::settings2x_png, BinaryData::settings2x_pngSize);
    imageAddIcon                        = ImageCache::getFromMemory(BinaryData::ShortcutAdd2x_png, BinaryData::ShortcutAdd2x_pngSize);
    imageCloseIcon                      = ImageCache::getFromMemory(BinaryData::ShortcutClose2x_png, BinaryData::ShortcutClose2x_pngSize);
    imageLeftIcon                       = ImageCache::getFromMemory(BinaryData::ShortcutLeft2x_png, BinaryData::ShortcutLeft2x_pngSize);
    imageRightIcon                      = ImageCache::getFromMemory(BinaryData::ShortcutRight2x_png, BinaryData::ShortcutRight2x_pngSize);
    imageLoopIcon                       = ImageCache::getFromMemory(BinaryData::ShortcutLoop2x_png, BinaryData::ShortcutLoop2x_pngSize);
    imageMuteIcon                       = ImageCache::getFromMemory(BinaryData::ShortcutMute2x_png, BinaryData::ShortcutMute2x_pngSize);
    imageStopButton                     = ImageCache::getFromMemory(BinaryData::stop2x_png, BinaryData::stop2x_pngSize);
    imageRecordButton                   = ImageCache::getFromMemory(BinaryData::RecordButton_png, BinaryData::RecordButton_pngSize);
    imageFFTMockup                      = ImageCache::getFromMemory(BinaryData::FFTMockup_png, BinaryData::FFTMockup_pngSize);
    imageColorSpectrumMockup            = ImageCache::getFromMemory(BinaryData::ColorSpectrumMockup_png, BinaryData::ColorSpectrumMockup_pngSize);
    imageOctaveSpectrumMockup           = ImageCache::getFromMemory(BinaryData::OctaveSpectrumMockup_png, BinaryData::OctaveSpectrumMockup_pngSize);
    
    
    containerView_Shortcut = std::make_unique<ShortcutFrequencyContainerComponent>(projectManager);
    for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
    {
        // add ShortcutListener to open settings view for selected shortcut
        containerView_Shortcut->shortcutComponent[i]->addShortcutListener(this);
    }
    
    containerView_Shortcut->setBounds(0, 0, shortcutWidth, shortcutHeight);
    addAndMakeVisible(containerView_Shortcut.get());
    
    
    containerView_Main = std::make_unique<Component>();
    containerView_Main->setBounds(0, shortcutHeight, shortcutWidth, 1096);
    
    imageComp = std::make_unique<ImageComponent>(); imageComp->setImage(imageMainContainerBackground);
    imageComp->setBounds(0, 0, containerView_Main->getWidth(), containerView_Main->getHeight());
    containerView_Main->addAndMakeVisible(imageComp.get());
    addAndMakeVisible(containerView_Main.get());
    
    // visualiser component
    visualiserContainerComponent = std::make_unique<VisualiserContainerComponent2>(projectManager, AUDIO_MODE::MODE_FREQUENCY_PLAYER);
    visualiserContainerComponent->setBounds(0, 0, containerView_Main->getWidth(), containerView_Main->getHeight()-300);
    containerView_Main->addAndMakeVisible(visualiserContainerComponent.get());
    

    
    label_Playing = std::make_unique<Label>();
    label_Playing->setText("Playing (00:40)", dontSendNotification);
    label_Playing->setJustificationType(Justification::left);
    fontNormal.setHeight(33);
    label_Playing->setBounds(playingLeftMargin, playingTopMargin, 300, 40);
    label_Playing->setFont(fontNormal);
    containerView_Main->addAndMakeVisible(label_Playing.get());
    
    
    button_Record = std::make_unique<ImageButton>();
    button_Record->setTriggeredOnMouseDown(true);
    button_Record->setImages (false, true, true,
                              imageRecordButton, 0.999f, Colour (0x00000000),
                              Image(), 1.000f, Colour (0x00000000),
                              imageRecordButton, 0.6, Colour (0x00000000));
    button_Record->addListener(this);
    button_Record->setBounds(recordLeftMargin, recordTopMargin, recordWidth, recordHeight);
    containerView_Main->addAndMakeVisible(button_Record.get());
    
    button_Play = std::make_unique<ImageButton>();
    button_Play->setTriggeredOnMouseDown(true);
    button_Play->setImages (false, true, true,
                            imagePlayButton, 0.999f, Colour (0x00000000),
                            Image(), 1.000f, Colour (0x00000000),
                            imagePlayButton, 0.6, Colour (0x00000000));
    button_Play->addListener(this);
    button_Play->setBounds(playLeftMargin, playTopMargin, playWidth, playHeight);
    containerView_Main->addAndMakeVisible(button_Play.get());
    
    button_Stop = std::make_unique<ImageButton>();
    button_Stop->setTriggeredOnMouseDown(true);
    button_Stop->setImages (false, true, true,
                            imageStopButton, 0.999f, Colour (0x00000000),
                            Image(), 1.000f, Colour (0x00000000),
                            imageStopButton, 0.6, Colour (0x00000000));
    button_Stop->addListener(this);
    button_Stop->setBounds(stopLeftMargin, stopTopMargin, playWidth, playHeight);
    containerView_Main->addAndMakeVisible(button_Stop.get());
    
    button_Panic = std::make_unique<ImageButton>();
    button_Panic->setTriggeredOnMouseDown(true);
    button_Panic->setImages (false, true, true,
                             imagePanicButton, 0.999f, Colour (0x00000000),
                             Image(), 1.000f, Colour (0x00000000),
                             imagePanicButton, 0.6, Colour (0x00000000));
    button_Panic->addListener(this);
    button_Panic->setBounds(panicLeftMargin, panicTopMargin, panicWidth, panicHeight);
    containerView_Main->addAndMakeVisible(button_Panic.get());
    
    // Progress bar
    progressBar = std::make_unique<CustomProgressBar>();
    progressBar->setBounds(214, 1196, 1126, 56);
    addAndMakeVisible(progressBar.get());
    
    // Play In Loop Button
    button_PlayInLoop = std::make_unique<ToggleButton>("Loop");
    button_PlayInLoop->setBounds(1173, 1100, 200, 100);
    button_PlayInLoop->setLookAndFeel(&lookAndFeel);
    button_PlayInLoop->addListener(this);
    addAndMakeVisible(button_PlayInLoop.get());
    // needs resiszing
    
    button_PlaySimultaneous = std::make_unique<ToggleButton>("Simultaneous");
    button_PlaySimultaneous->setBounds(1173-340, 1100, 300, 100);
    button_PlaySimultaneous->setLookAndFeel(&lookAndFeel);
    button_PlaySimultaneous->addListener(this);
    addAndMakeVisible(button_PlaySimultaneous.get());
    
    
    // Load / Save Button
    button_Load = std::make_unique<TextButton>("Load");
    button_Load->setBounds(1470, 1204, 100, 40);
    button_Load->addListener(this);
    button_Load->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(button_Load.get());
    
    button_Save = std::make_unique<TextButton>("Save");
    button_Save->setBounds(1360, 1204, 100, 40);
    button_Save->addListener(this);
    button_Save->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(button_Save.get());
    
    
    // settings component
    frequencyPlayerSettingsComponent = std::make_unique<FrequencyPlayerSettingsComponent>(projectManager);
    addAndMakeVisible(frequencyPlayerSettingsComponent.get());
    int w = shortcutWidth; int h =mainContainerHeight + shortcutHeight;
    frequencyPlayerSettingsComponent->setBounds(0, 0, w, h);
    frequencyPlayerSettingsComponent->setVisible(false);
    
    
    // shortcut Keys
    shortcutKey[SHORTCUT_A] = 'a';
    shortcutKey[SHORTCUT_S] = 's';
    shortcutKey[SHORTCUT_D] = 'd';
    shortcutKey[SHORTCUT_F] = 'f';
    shortcutKey[SHORTCUT_G] = 'g';
    shortcutKey[SHORTCUT_H] = 'h';
    shortcutKey[SHORTCUT_J] = 'j';
    shortcutKey[SHORTCUT_K] = 'k';
    shortcutKey[SHORTCUT_L] = 'l';
    shortcutKey[SHORTCUT_Z] = 'z';
    shortcutKey[SHORTCUT_X] = 'x';
    shortcutKey[SHORTCUT_C] = 'c';
    shortcutKey[SHORTCUT_V] = 'v';
    shortcutKey[SHORTCUT_B] = 'b';
    shortcutKey[SHORTCUT_N] = 'n';
    shortcutKey[SHORTCUT_M] = 'm';
    shortcutKey[SHORTCUT_Q] = 'q';
    shortcutKey[SHORTCUT_W] = 'w';
    shortcutKey[SHORTCUT_E] = 'e';
    shortcutKey[SHORTCUT_R] = 'r';
    
    clearHeldNotes();
    
    startTimerHz(TIMER_UPDATE_RATE);
}

FrequencyPlayerComponent::~FrequencyPlayerComponent() { }

void FrequencyPlayerComponent::resized()
{
    containerView_Shortcut->setBounds(0, 0, shortcutWidth * scaleFactor, shortcutHeight * scaleFactor);
    containerView_Main->setBounds(0, shortcutHeight * scaleFactor, shortcutWidth * scaleFactor, 1096 * scaleFactor);
    imageComp->setBounds(0, 0, containerView_Main->getWidth(), containerView_Main->getHeight());
    
//    label_FFT->setBounds(fftLeftMargin * scaleFactor, 20 * scaleFactor, 300 * scaleFactor, 40 * scaleFactor);
//    label_FFT->setFont(33 * scaleFactor);
//
//    label_ColorSpectrum->setBounds(colorLeftMargin * scaleFactor, (colorTopMargin-45) * scaleFactor, 300 * scaleFactor, 40 * scaleFactor);
//    label_ColorSpectrum->setFont(33 * scaleFactor);
//
//    label_OctaveSpectrum->setBounds(octaveLeftMergin * scaleFactor, (octaveTopMargin-45) * scaleFactor, 300 * scaleFactor, 40 * scaleFactor);
//    label_OctaveSpectrum->setFont(33 * scaleFactor);
    
    label_Playing->setBounds(playingLeftMargin * scaleFactor, playingTopMargin * scaleFactor, 300 * scaleFactor, 40 * scaleFactor);
    label_Playing->setFont(33 * scaleFactor);
    
    button_Record->setBounds(recordLeftMargin * scaleFactor, recordTopMargin * scaleFactor, recordWidth * scaleFactor, recordHeight * scaleFactor);
    button_Play->setBounds(playLeftMargin * scaleFactor, playTopMargin * scaleFactor, playWidth * scaleFactor, playHeight * scaleFactor);
    button_Stop->setBounds(stopLeftMargin * scaleFactor, stopTopMargin * scaleFactor, playWidth * scaleFactor, playHeight * scaleFactor);
    button_Panic->setBounds(panicLeftMargin * scaleFactor, panicTopMargin * scaleFactor, panicWidth * scaleFactor, panicHeight * scaleFactor);
    
    progressBar->setBounds(214 * scaleFactor, 1196 * scaleFactor, 1126 * scaleFactor, 56 * scaleFactor);
    
    button_PlayInLoop->setBounds(1173 * scaleFactor, 1100 * scaleFactor, 200 * scaleFactor, 100 * scaleFactor);
    button_PlaySimultaneous->setBounds((1173-340) * scaleFactor, 1100 * scaleFactor, 300 * scaleFactor, 100 * scaleFactor);
    button_Load->setBounds(1470 * scaleFactor, 1204 * scaleFactor, 100 * scaleFactor, 40 * scaleFactor);
    button_Save->setBounds(1360 * scaleFactor, 1204 * scaleFactor, 100 * scaleFactor, 40 * scaleFactor);
    
    int w = shortcutWidth * scaleFactor; int h = (mainContainerHeight + shortcutHeight) * scaleFactor;
    frequencyPlayerSettingsComponent->setBounds(0, 0, w, h);
    
    visualiserContainerComponent->setBounds(0, 0, containerView_Main->getWidth(), containerView_Main->getHeight() - (300 * scaleFactor));
    
}


void FrequencyPlayerComponent::timerCallback()
{
    if (projectManager->frequencyPlayerProcessor->getPlaystate() == PLAY_STATE::PLAYING)
    {
        MessageManagerLock lock;
        
        float val = projectManager->frequencyPlayerProcessor->repeater->getProgressBarValue();
        
        progressBar->setValue(val);
        
        String text = projectManager->frequencyPlayerProcessor->repeater->getTimeRemainingInSecondsString();
        
        label_Playing->setText(text, dontSendNotification);
    }
    else
    {
        float val = projectManager->frequencyPlayerProcessor->repeater->getProgressBarValue();
        
        progressBar->setValue(val);
    }
}

void FrequencyPlayerComponent::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background
    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
}



void FrequencyPlayerComponent::buttonClicked (Button*button)
{
    if (button == button_Record.get())
    {
//        projectManager->setPlayerCommand(PLAYER_COMMANDS::COMMAND_PLAYER_RECORD);
        
        if (projectManager->isRecording())
        {
            projectManager->stopRecording();
        }
        else
        {
            projectManager->createNewFileForRecordingFrequencyPlayer();
            projectManager->startRecording();
        }
    }
    else if (button == button_Play.get())
    {
        projectManager->setPlayerCommand(PLAYER_COMMANDS::COMMAND_PLAYER_PLAYPAUSE);
    }
    else if (button == button_Stop.get())
    {
        projectManager->setPlayerCommand(PLAYER_COMMANDS::COMMAND_PLAYER_STOP);
    }
    else if (button == button_Panic.get())
    {
        projectManager->setPanicButton();
    }
    else if (button == button_PlayInLoop.get())
    {
        if (button_PlayInLoop->getToggleState())
        {
            projectManager->setPlayerPlayMode(PLAY_MODE::LOOP);
        }
        else
        {
            projectManager->setPlayerPlayMode(PLAY_MODE::NORMAL); // might be trigger for shortcuts..
        }
    }
    else if (button == button_PlaySimultaneous.get())
    {
        if (button_PlaySimultaneous->getToggleState())
        {
            projectManager->setProjectSettingsParameter(PLAYER_PLAY_SIMULTANEOUS, 1.0);
        }
        else
        {
            projectManager->setProjectSettingsParameter(PLAYER_PLAY_SIMULTANEOUS, 0.0);
        }
    }
    else if (button == button_Load.get())
    {
        projectManager->loadProfileForMode(AUDIO_MODE::MODE_FREQUENCY_PLAYER);
    }
    else if (button == button_Save.get())
    {
        projectManager->saveProfileForMode(AUDIO_MODE::MODE_FREQUENCY_PLAYER);
    }
}

void FrequencyPlayerComponent::openFrequencyPlayerSettingsForShortcut(int shortcutRef)
{
    frequencyPlayerSettingsComponent->openView(shortcutRef);
}

//
void FrequencyPlayerComponent::updateFrequencyPlayerUIParameter(int shortcutRef, int paramIndex)
{
    // shortcut units first
    if (paramIndex == FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE)
    {
        bool shouldBeActive = projectManager->getFrequencyPlayerParameter(shortcutRef, FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE).operator bool();
        
        containerView_Shortcut->shortcutComponent[shortcutRef]->setState(shouldBeActive);
    }
    // need to split between component and settings ui
    else if (paramIndex == FREQUENCY_PLAYER_SHORTCUT_MUTE)
    {
        bool shouldMute = projectManager->getFrequencyPlayerParameter(shortcutRef, FREQUENCY_PLAYER_SHORTCUT_MUTE).operator bool();
        
        containerView_Shortcut->shortcutComponent[shortcutRef]->setMute(shouldMute);
    }
    else if (paramIndex == FREQUENCY_PLAYER_SHORTCUT_PLAY_AT_SAME_TIME)
    {
        bool shouldLoop = projectManager->getFrequencyPlayerParameter(shortcutRef, FREQUENCY_PLAYER_SHORTCUT_PLAY_AT_SAME_TIME).operator bool();
        
        containerView_Shortcut->shortcutComponent[shortcutRef]->setLoop(shouldLoop);
    }
    else if (paramIndex == FREQUENCY_PLAYER_WAVEFORM_TYPE)
    {
        int wavet            = projectManager->getFrequencyPlayerParameter(shortcutRef, FREQUENCY_PLAYER_WAVEFORM_TYPE).operator int();
        String waveString      = ProjectStrings::getWavetypes().getReference(wavet);
        containerView_Shortcut  ->shortcutComponent[shortcutRef]->setWaveTypeString(waveString);
    }
    else if (paramIndex == FREQUENCY_PLAYER_CHOOSE_FREQ)
    {
        float wavet            = projectManager->getFrequencyPlayerParameter(shortcutRef, FREQUENCY_PLAYER_CHOOSE_FREQ).operator float();
        containerView_Shortcut  ->shortcutComponent[shortcutRef]->setFrequency(wavet);
    }
    
    frequencyPlayerSettingsComponent->syncUI();
}

void FrequencyPlayerComponent::updateSettingsUIParameter(int settingIndex)
{
    if (settingIndex == PLAYER_PLAY_IN_LOOP)
    {
        button_PlayInLoop->setToggleState(projectManager->getProjectSettingsParameter(settingIndex), dontSendNotification);
    }
    else if (settingIndex == PLAYER_PLAY_SIMULTANEOUS)
    {
        button_PlaySimultaneous->setToggleState(projectManager->getProjectSettingsParameter(settingIndex), dontSendNotification);
    }
}

// shortcut handlers
bool FrequencyPlayerComponent::keyPressed (const KeyPress& key)
{
    //    // parse shortcut keys
        juce_wchar textCharacter = key.getTextCharacter();
    
        if      (textCharacter == 'a') projectManager->shortcutKeyPressed(CHORD_PLAYER_SHORTCUT_KEY::SHORTCUT_A);
        else if (textCharacter == 's') projectManager->shortcutKeyPressed(CHORD_PLAYER_SHORTCUT_KEY::SHORTCUT_S);
        else if (textCharacter == 'd') projectManager->shortcutKeyPressed(CHORD_PLAYER_SHORTCUT_KEY::SHORTCUT_D);
        else if (textCharacter == 'f') projectManager->shortcutKeyPressed(CHORD_PLAYER_SHORTCUT_KEY::SHORTCUT_F);
        else if (textCharacter == 'g') projectManager->shortcutKeyPressed(CHORD_PLAYER_SHORTCUT_KEY::SHORTCUT_G);
        else if (textCharacter == 'h') projectManager->shortcutKeyPressed(CHORD_PLAYER_SHORTCUT_KEY::SHORTCUT_H);
        else if (textCharacter == 'j') projectManager->shortcutKeyPressed(CHORD_PLAYER_SHORTCUT_KEY::SHORTCUT_J);
        else if (textCharacter == 'k') projectManager->shortcutKeyPressed(CHORD_PLAYER_SHORTCUT_KEY::SHORTCUT_K);
        else if (textCharacter == 'l') projectManager->shortcutKeyPressed(CHORD_PLAYER_SHORTCUT_KEY::SHORTCUT_L);
        else if (textCharacter == 'z') projectManager->shortcutKeyPressed(CHORD_PLAYER_SHORTCUT_KEY::SHORTCUT_Z);
        else if (textCharacter == 'x') projectManager->shortcutKeyPressed(CHORD_PLAYER_SHORTCUT_KEY::SHORTCUT_X);
        else if (textCharacter == 'c') projectManager->shortcutKeyPressed(CHORD_PLAYER_SHORTCUT_KEY::SHORTCUT_C);
        else if (textCharacter == 'v') projectManager->shortcutKeyPressed(CHORD_PLAYER_SHORTCUT_KEY::SHORTCUT_V);
        else if (textCharacter == 'b') projectManager->shortcutKeyPressed(CHORD_PLAYER_SHORTCUT_KEY::SHORTCUT_B);
        else if (textCharacter == 'n') projectManager->shortcutKeyPressed(CHORD_PLAYER_SHORTCUT_KEY::SHORTCUT_N);
        else if (textCharacter == 'm') projectManager->shortcutKeyPressed(CHORD_PLAYER_SHORTCUT_KEY::SHORTCUT_M);
        else if (textCharacter == 'q') projectManager->shortcutKeyPressed(CHORD_PLAYER_SHORTCUT_KEY::SHORTCUT_Q);
        else if (textCharacter == 'w') projectManager->shortcutKeyPressed(CHORD_PLAYER_SHORTCUT_KEY::SHORTCUT_W);
        else if (textCharacter == 'e') projectManager->shortcutKeyPressed(CHORD_PLAYER_SHORTCUT_KEY::SHORTCUT_E);
        else if (textCharacter == 'r') projectManager->shortcutKeyPressed(CHORD_PLAYER_SHORTCUT_KEY::SHORTCUT_R);
    
    return true;
}

bool FrequencyPlayerComponent::keyStateChanged (bool isKeyDown)
{
    // set to num shortcut synth to avoid calling out of bounds indexes
    for (int s = 0; s < NUM_SHORTCUT_SYNTHS; s++)
    {
        if (KeyPress::isKeyCurrentlyDown(shortcutKey[s]))
        {
            if (!shortcutKeyStateDown[s])
            {
                shortcutKeyStateDown[s] = true;
                projectManager->shortcutKeyDown(s);
            }
        }
        
        if (shortcutKeyStateDown[s] && !KeyPress::isKeyCurrentlyDown(shortcutKey[s]))
        {
            shortcutKeyStateDown[s] = false;
            projectManager->shortcutKeyUp(s);
        }
    }
    return false;
    
}

void FrequencyPlayerComponent::clearHeldNotes()
{
    for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
    {
        shortcutKeyStateDown[i] = false;
    }
}
