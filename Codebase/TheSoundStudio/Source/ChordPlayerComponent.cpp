/*
  ==============================================================================

    ChordPlayerComponent.cpp
    Created: 13 Mar 2019 9:54:54pm
    Author:  Ziv Elovitch - The Sound Studio Team

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "ChordPlayerComponent.h"

//==============================================================================


//==============================================================================
//==============================================================================
ChordPlayerComponent::ChordPlayerComponent(ProjectManager * pm)
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
    fontBold.setHeight(38);
    
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
    
    
    containerView_Shortcut = std::make_unique<ShortcutContainerComponent>(projectManager);
    for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
    {
        // add ShortcutListener to open settings view for selected shortcut
        containerView_Shortcut->shortcutComponent[i]->addShortcutListener(this);
    }
    
    addAndMakeVisible(containerView_Shortcut.get());
    
    
    containerView_Main = std::make_unique<Component>();
    
    imageComp = std::make_unique<ImageComponent>(); 
    imageComp->setImage(imageMainContainerBackground);
    containerView_Main->addAndMakeVisible(imageComp.get());
    addAndMakeVisible(containerView_Main.get());

    // Labels


    label_Playing = std::make_unique<Label>();
    label_Playing->setText("Playing (00:00)", dontSendNotification);
//    label_Playing->setBounds(playingLeftMargin, playingTopMargin, 300, 40);
    label_Playing->setJustificationType(Justification::left);
    fontNormal.setHeight(33);
    label_Playing->setFont(fontNormal);
    containerView_Main->addAndMakeVisible(label_Playing.get());
    
    
    button_Record = std::make_unique<ImageButton>();
    button_Record->setTriggeredOnMouseDown(true);
    button_Record->setImages (false, true, true,
                                       imageRecordButton, 0.999f, Colour (0x00000000),
                                       Image(), 1.000f, Colour (0x00000000),
                                       imageRecordButton, 0.6, Colour (0x00000000));
    button_Record->addListener(this);
    containerView_Main->addAndMakeVisible(button_Record.get());
    
    button_Play = std::make_unique<ImageButton>();
    button_Play->setTriggeredOnMouseDown(true);
    button_Play->setImages (false, true, true,
                              imagePlayButton, 0.999f, Colour (0x00000000),
                              Image(), 1.000f, Colour (0x00000000),
                              imagePlayButton, 0.6, Colour (0x00000000));
    button_Play->addListener(this);
    containerView_Main->addAndMakeVisible(button_Play.get());
    
    button_Stop = std::make_unique<ImageButton>();
    button_Stop->setTriggeredOnMouseDown(true);
    button_Stop->setImages (false, true, true,
                              imageStopButton, 0.999f, Colour (0x00000000),
                              Image(), 1.000f, Colour (0x00000000),
                              imageStopButton, 0.6, Colour (0x00000000));
    button_Stop->addListener(this);
    containerView_Main->addAndMakeVisible(button_Stop.get());
    
    button_Panic = std::make_unique<ImageButton>();
    button_Panic->setTriggeredOnMouseDown(true);
    button_Panic->setImages (false, true, true,
                              imagePanicButton, 0.999f, Colour (0x00000000),
                              Image(), 1.000f, Colour (0x00000000),
                              imagePanicButton, 0.6, Colour (0x00000000));
    button_Panic->addListener(this);
    containerView_Main->addAndMakeVisible(button_Panic.get());
    
    // Progress bar
    progressBar = std::make_unique<CustomProgressBar>();
    addAndMakeVisible(progressBar.get());
    
    // Play In Loop Button
    button_PlayInLoop = std::make_unique<ToggleButton>("Loop");
    button_PlayInLoop->setLookAndFeel(&lookAndFeel);
    button_PlayInLoop->addListener(this);
    addAndMakeVisible(button_PlayInLoop.get());
    // needs resiszing
    
    button_PlaySimultaneous = std::make_unique<ToggleButton>("Simultaneous");
    button_PlaySimultaneous->setLookAndFeel(&lookAndFeel);
    button_PlaySimultaneous->addListener(this);
    addAndMakeVisible(button_PlaySimultaneous.get());
    
    
    // Load / Save Button
    button_Load = std::make_unique<TextButton>("Load");
    button_Load->addListener(this);
    button_Load->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(button_Load.get());
    
    button_Save = std::make_unique<TextButton>("Save");
    button_Save->addListener(this);
    button_Save->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(button_Save.get());
    
    
    
    // settings component
    chordPlayerSettingsComponent = std::make_unique<ChordPlayerSettingsComponent>(projectManager);
    addAndMakeVisible(chordPlayerSettingsComponent.get());
    chordPlayerSettingsComponent->setVisible(false);
    
    
    // visualiser component
    visualiserContainerComponent = std::make_unique<VisualiserContainerComponent2>(projectManager, AUDIO_MODE::MODE_CHORD_PLAYER);
    visualiserContainerComponent->setBounds(0, 0, containerView_Main->getWidth(), containerView_Main->getHeight()-300);
    containerView_Main->addAndMakeVisible(visualiserContainerComponent.get());
    

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
    
    clearHeldNotes(); // 
    
    startTimerHz(TIMER_UPDATE_RATE);
}

ChordPlayerComponent::~ChordPlayerComponent() { }

void ChordPlayerComponent::resized()
{
    containerView_Shortcut->setBounds(0, 0, shortcutWidth * scaleFactor, shortcutHeight * scaleFactor);
    containerView_Main->setBounds(0, shortcutHeight * scaleFactor, shortcutWidth * scaleFactor, 1096 * scaleFactor);
    
    imageComp->setBounds(0, 0, containerView_Main->getWidth(), containerView_Main->getHeight());
    
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
    
    int w = shortcutWidth * scaleFactor; int h =(mainContainerHeight + shortcutHeight) * scaleFactor;
    
    chordPlayerSettingsComponent->setBounds(0, 0, w, h);
    
    visualiserContainerComponent->setBounds(0, 0, containerView_Main->getWidth(), containerView_Main->getHeight() - (300 * scaleFactor));
}


void ChordPlayerComponent::timerCallback()
{
    if (projectManager->chordPlayerProcessor->getPlaystate() == PLAY_STATE::PLAYING)
    {
        MessageManagerLock lock;

        float val = projectManager->chordPlayerProcessor->repeater->getProgressBarValue();

        progressBar->setValue(val);

        String text = projectManager->chordPlayerProcessor->repeater->getTimeRemainingInSecondsString();

        label_Playing->setText(text, dontSendNotification);
    }
    else
    {
        float val = projectManager->chordPlayerProcessor->repeater->getProgressBarValue();

        progressBar->setValue(val);
    }

    // visualiser

}

void ChordPlayerComponent::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background
    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
}


void ChordPlayerComponent::buttonClicked (Button*button)
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
            projectManager->createNewFileForRecordingChordPlayer();
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
//        projectManager->setPlayerCommand(PLAYER_COMMANDS::COMMAND_PLAYER_PANIC);
        
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
        projectManager->loadProfileForMode(AUDIO_MODE::MODE_CHORD_PLAYER);
    }
    else if (button == button_Save.get())
    {
        projectManager->saveProfileForMode(AUDIO_MODE::MODE_CHORD_PLAYER);
        
    }
}

void ChordPlayerComponent::openChordPlayerSettingsForShortcut(int shortcutRef)
{
    chordPlayerSettingsComponent->openView(shortcutRef);
    
}

//
void ChordPlayerComponent::updateChordPlayerUIParameter(int shortcutRef, int paramIndex)
{
    // shortcut units first
    if (paramIndex == SHORTCUT_IS_ACTIVE)
    {
        bool shouldBeActive = projectManager->getChordPlayerParameter(shortcutRef, SHORTCUT_IS_ACTIVE).operator bool();
        
        containerView_Shortcut->shortcutComponent[shortcutRef]->setState(shouldBeActive);
        
//        int keynote = projectManager->getChordPlayerParameter(shortcutRef, KEYNOTE).operator int();
//        int oct = projectManager->getChordPlayerParameter(shortcutRef, OCTAVE).operator int();
//        
//        int midiNote = keynote + (oct * 12) - 1;
//        
//        float baseFreq = projectManager->frequencyManager->getFrequencyForMIDINote(midiNote);
//        containerView_Shortcut  ->shortcutComponent[shortcutRef]->setFrequency(baseFreq);
    }
    // need to split between component and settings ui
    else if (paramIndex == SHORTCUT_MUTE)
    {
        bool shouldMute = projectManager->getChordPlayerParameter(shortcutRef, SHORTCUT_MUTE).operator bool();
        
        containerView_Shortcut->shortcutComponent[shortcutRef]->setMute(shouldMute);
    }
    else if (paramIndex == SHORTCUT_PLAY_AT_SAME_TIME)
    {
        bool shouldLoop = projectManager->getChordPlayerParameter(shortcutRef, SHORTCUT_PLAY_AT_SAME_TIME).operator bool();
        
        containerView_Shortcut->shortcutComponent[shortcutRef]->setLoop(shouldLoop);
    }
    else if (paramIndex == KEYNOTE)
    {
        int keynote             = projectManager->getChordPlayerParameter(shortcutRef, KEYNOTE).operator int();
        int oct                 = projectManager->getChordPlayerParameter(shortcutRef, OCTAVE).operator int() + 1;
        int chordRef            = projectManager->getChordPlayerParameter(shortcutRef, KEYNOTE).operator int();
        
        String chordString(ProjectStrings::getKeynoteArray().getReference(chordRef-1));
        chordString.append("-", 3); String octString(oct-1); chordString.append(octString, 3);
        containerView_Shortcut  ->shortcutComponent[shortcutRef]->setChordString(chordString);
        
        int midiNote = keynote + (oct * 12) - 1;

        float baseFreq = projectManager->frequencyManager->getFrequencyForMIDINote(midiNote);
        containerView_Shortcut  ->shortcutComponent[shortcutRef]->setFrequency(baseFreq);
    }
    else if (paramIndex == OCTAVE)
    {
        int keynote             = projectManager->getChordPlayerParameter(shortcutRef, KEYNOTE).operator int();
        int oct                 = projectManager->getChordPlayerParameter(shortcutRef, OCTAVE).operator int() + 1;
        int chordRef            = projectManager->getChordPlayerParameter(shortcutRef, KEYNOTE).operator int();
        
        String chordString(ProjectStrings::getKeynoteArray().getReference(chordRef-1));
        chordString.append("-", 3); String octString(oct-1); chordString.append(octString, 3);
        containerView_Shortcut  ->shortcutComponent[shortcutRef]->setChordString(chordString);
        

        int midiNote = keynote + (oct * 12) - 1;

        float baseFreq = projectManager->frequencyManager->getFrequencyForMIDINote(midiNote);
        containerView_Shortcut  ->shortcutComponent[shortcutRef]->setFrequency(baseFreq);
        
//        // get chord type and octave
//        int chordRef            = projectManager->getChordPlayerParameter(shortcutRef, CHORD_TYPE).operator int();
//        String chordString      = ProjectStrings::getChordTypeArray().getReference(chordRef-1);
//        oct-=1;
//
//        String comboString(oct); comboString.append(chordString, 20);
//        containerView_Shortcut  ->shortcutComponent[shortcutRef]->setChordTypeString(comboString);
        
    }
    else if (paramIndex == CHORD_TYPE)
    {
        int chordRef            = projectManager->getChordPlayerParameter(shortcutRef, CHORD_TYPE).operator int();
//        int oct                 = projectManager->getChordPlayerParameter(shortcutRef, OCTAVE).operator int();
        String chordString      = ProjectStrings::getChordTypeArray().getReference(chordRef-1);
        String comboString(""); comboString.append(chordString, 20);
        containerView_Shortcut  ->shortcutComponent[shortcutRef]->setChordTypeString(comboString);
    }
    else if (paramIndex == CHORDPLAYER_SCALE)
    {
        chordPlayerSettingsComponent->updateScalesComponents();

        int scale = projectManager->getChordPlayerParameter(shortcutRef, CHORDPLAYER_SCALE).operator int();
        String scaleString;
        switch (scale) {
            case 1: scaleString = "Di-Py"; break;
            case 2: scaleString = "Di-IF"; break;
            case 3: scaleString = "Di-JT"; break;
            case 4: scaleString = "CH-Py"; break;
            case 5: scaleString = "CH-JI"; break;
            case 6: scaleString = "CH-ET"; break;
            case 7: scaleString = "Ha-Si"; break;
            case 8: scaleString = "Enharm"; break;
            case 9: scaleString = "Solf"; break;
   
            default: break;
        }
        
        containerView_Shortcut  ->shortcutComponent[shortcutRef]->setScaleString(scaleString);
    }
    else if (paramIndex == INSTRUMENT_TYPE || paramIndex == WAVEFORM_TYPE)
    {
        // Waveform Type
        String stringLabel;
        String stringWaveform;
        
        int waveformType = projectManager->getChordPlayerParameter(shortcutRef, WAVEFORM_TYPE).operator int();
        if (waveformType == 0) // playing instrument
        {
            // PLAYING_INSTRUMENT
            int instrumentType = projectManager->getChordPlayerParameter(shortcutRef, INSTRUMENT_TYPE).operator int() - 1;
            
            // get string for type
            String instString;
            
            File dir = File::getSpecialLocation (File::userDocumentsDirectory).getChildFile ("ASVPR/SampleLibrary/Playing Instruments");
            
            Array<juce::File> results; dir.findChildFiles(results, juce::File::TypesOfFileToFind::findDirectories , false);
            
            File instrumentDir(results[instrumentType]); String instName(instrumentDir.getFileName());
            
            stringLabel         = "Instrument :";
            stringWaveform      = instName;
        }
        else if (waveformType == 1) { stringWaveform = "Sinewave";  stringLabel = "Type :"; }
        else if (waveformType == 2) { stringWaveform = "Triangle";  stringLabel = "Type :"; }
        else if (waveformType == 3) { stringWaveform = "Square";    stringLabel = "Type :"; }
        else if (waveformType == 4) { stringWaveform = "Sawtooth";  stringLabel = "Type :"; }
        else if (waveformType == 5) { stringWaveform = "Custom";    stringLabel = "Type :"; }

        containerView_Shortcut  ->shortcutComponent[shortcutRef]->setInstrumentString(stringWaveform, stringLabel);
    }
    else if (paramIndex == CUSTOM_CHORD)
    {
        const auto isCustomChordActive = (bool)projectManager->getChordPlayerParameter(shortcutRef, CUSTOM_CHORD);
        if (isCustomChordActive) {
            containerView_Shortcut->shortcutComponent[shortcutRef]->setChordTypeString("Custom");

            const auto isFirstNoteActive = (bool)projectManager->getChordPlayerParameter(shortcutRef, CUSTOM_CHORD_ACTIVE_1);
            if (isFirstNoteActive) {
                const auto keyNote = (int)projectManager->getChordPlayerParameter(shortcutRef, CUSTOM_CHORD_NOTE_1) - 1;
                const auto octave = (int)projectManager->getChordPlayerParameter(shortcutRef, CUSTOM_CHORD_OCTAVE_1) - 1;

                const auto keyNoteString = ProjectStrings::getKeynoteArray().getReference(keyNote);
                const String chordString { keyNoteString + "-" + String{octave} };
                containerView_Shortcut->shortcutComponent[shortcutRef]->setChordString(chordString);

                int midiNote = keyNote + ((octave + 1) * 12);
                const auto baseFreq = projectManager->frequencyManager->getFrequencyForMIDINote(midiNote);
                containerView_Shortcut ->shortcutComponent[shortcutRef]->setFrequency(baseFreq);
            }
        } else {
            ChordPlayerComponent::updateChordPlayerUIParameter(shortcutRef,CHORD_TYPE);
            ChordPlayerComponent::updateChordPlayerUIParameter(shortcutRef,KEYNOTE);
        }
    }
    
    chordPlayerSettingsComponent->syncUI();
}

void ChordPlayerComponent::updateSettingsUIParameter(int settingIndex)
{
    if (settingIndex == PLAYER_PLAY_IN_LOOP)
    {
        button_PlayInLoop->setToggleState(projectManager->getProjectSettingsParameter(settingIndex), dontSendNotification);
    }
    else if (settingIndex == PLAYER_PLAY_SIMULTANEOUS)
    {
        button_PlaySimultaneous->setToggleState(projectManager->getProjectSettingsParameter(settingIndex), dontSendNotification);
    }
    else if (settingIndex == DEFAULT_SCALE)
    {
        chordPlayerSettingsComponent->updateScalesComponents();
    }
}

// shortcut handlers
bool ChordPlayerComponent::keyPressed (const KeyPress& key)
{
    return true;
}

bool ChordPlayerComponent::keyStateChanged (bool isKeyDown)
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

void ChordPlayerComponent::clearHeldNotes()
{
    for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
    {
        shortcutKeyStateDown[i] = false;
    }
}
