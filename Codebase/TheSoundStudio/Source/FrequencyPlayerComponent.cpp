/*
  ==============================================================================

    FrequencyPlayerComponent.cpp

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "FrequencyPlayerComponent.h"

using Layout = TSS::Design::Layout;

//==============================================================================
FrequencyPlayerComponent::FrequencyPlayerComponent(ProjectManager * pm)
{
    projectManager = pm;
    projectManager->addUIListener(this);

    setWantsKeyboardFocus(true);

    // Shortcut container
    containerView_Shortcut = std::make_unique<ShortcutFrequencyContainerComponent>(projectManager);
    for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
    {
        containerView_Shortcut->shortcutComponent[i]->addShortcutListener(this);
    }
    addAndMakeVisible(containerView_Shortcut.get());

    // Main container (holds visualiser)
    containerView_Main = std::make_unique<Component>();
    addAndMakeVisible(containerView_Main.get());

    // visualiser component
    visualiserContainerComponent = std::make_unique<VisualiserContainerComponent2>(projectManager, AUDIO_MODE::MODE_FREQUENCY_PLAYER);
    containerView_Main->addAndMakeVisible(visualiserContainerComponent.get());

    // Transport toolbar
    TransportToolbarComponent::ButtonVisibility toolbarVis;
    toolbarVis.play = true;
    toolbarVis.stop = true;
    toolbarVis.record = true;
    toolbarVis.panic = true;
    toolbarVis.progress = true;
    toolbarVis.loop = true;
    toolbarVis.simultaneous = true;
    toolbarVis.save = true;
    toolbarVis.load = true;
    toolbarVis.playingLabel = true;
    transportToolbar = std::make_unique<TransportToolbarComponent>(toolbarVis);
    transportToolbar->addTransportListener(this);
    addAndMakeVisible(transportToolbar.get());

    // settings component
    frequencyPlayerSettingsComponent = std::make_unique<FrequencyPlayerSettingsComponent>(projectManager);
    addAndMakeVisible(frequencyPlayerSettingsComponent.get());
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
    auto bounds = getLocalBounds();
    const int h = bounds.getHeight();

    // Shortcut bar at top
    int shortcutH = (int)(h * Layout::kShortcutBarHeightRatio);
    containerView_Shortcut->setBounds(bounds.removeFromTop(shortcutH));

    // Toolbar at bottom
    int toolbarH = (int)(h * Layout::kToolbarHeightRatio);
    transportToolbar->setBounds(bounds.removeFromBottom(toolbarH));

    // Main container fills the remainder (visualiser area)
    containerView_Main->setBounds(bounds);
    visualiserContainerComponent->setBounds(0, 0, bounds.getWidth(), bounds.getHeight());

    // Settings overlay
    frequencyPlayerSettingsComponent->setBounds(0, 0, getWidth(), getHeight());
}


void FrequencyPlayerComponent::timerCallback()
{
    if (projectManager->frequencyPlayerProcessor->getPlaystate() == PLAY_STATE::PLAYING)
    {
        MessageManagerLock lock;

        float val = projectManager->frequencyPlayerProcessor->repeater->getProgressBarValue();
        transportToolbar->setProgressValue(val);

        String text = projectManager->frequencyPlayerProcessor->repeater->getTimeRemainingInSecondsString();
        transportToolbar->setPlayingText(text);
    }
    else
    {
        float val = projectManager->frequencyPlayerProcessor->repeater->getProgressBarValue();
        transportToolbar->setProgressValue(val);
    }
}

void FrequencyPlayerComponent::paint (Graphics& g)
{
    g.fillAll(juce::Colour(45, 44, 44));
}

void FrequencyPlayerComponent::buttonClicked (Button*button)
{
    // Handled by TransportToolbarComponent::Listener
}

// TransportToolbarComponent::Listener implementations
void FrequencyPlayerComponent::transportPlayClicked()
{
    projectManager->setPlayerCommand(PLAYER_COMMANDS::COMMAND_PLAYER_PLAYPAUSE);
}

void FrequencyPlayerComponent::transportStopClicked()
{
    projectManager->setPlayerCommand(PLAYER_COMMANDS::COMMAND_PLAYER_STOP);
}

void FrequencyPlayerComponent::transportRecordClicked()
{
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

void FrequencyPlayerComponent::transportPanicClicked()
{
    projectManager->setPanicButton();
}

void FrequencyPlayerComponent::transportLoopToggled(bool isOn)
{
    if (isOn)
        projectManager->setPlayerPlayMode(PLAY_MODE::LOOP);
    else
        projectManager->setPlayerPlayMode(PLAY_MODE::NORMAL);
}

void FrequencyPlayerComponent::transportSimultaneousToggled(bool isOn)
{
    projectManager->setProjectSettingsParameter(PLAYER_PLAY_SIMULTANEOUS, isOn ? 1.0 : 0.0);
}

void FrequencyPlayerComponent::transportSaveClicked()
{
    projectManager->saveProfileForMode(AUDIO_MODE::MODE_FREQUENCY_PLAYER);
}

void FrequencyPlayerComponent::transportLoadClicked()
{
    projectManager->loadProfileForMode(AUDIO_MODE::MODE_FREQUENCY_PLAYER);
}

void FrequencyPlayerComponent::openFrequencyPlayerSettingsForShortcut(int shortcutRef)
{
    frequencyPlayerSettingsComponent->openView(shortcutRef);
}

void FrequencyPlayerComponent::updateFrequencyPlayerUIParameter(int shortcutRef, int paramIndex)
{
    if (paramIndex == FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE)
    {
        bool shouldBeActive = projectManager->getFrequencyPlayerParameter(shortcutRef, FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE).operator bool();
        containerView_Shortcut->shortcutComponent[shortcutRef]->setState(shouldBeActive);
    }
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
        transportToolbar->setLoopState(projectManager->getProjectSettingsParameter(settingIndex));
    }
    else if (settingIndex == PLAYER_PLAY_SIMULTANEOUS)
    {
        transportToolbar->setSimultaneousState(projectManager->getProjectSettingsParameter(settingIndex));
    }
}

// shortcut handlers
bool FrequencyPlayerComponent::keyPressed (const KeyPress& key)
{
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
