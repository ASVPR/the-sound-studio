/*
  ==============================================================================

    ChordPlayerComponent.cpp

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "ChordPlayerComponent.h"
#include "TSSConstants.h"

using Layout = TSS::Design::Layout;

//==============================================================================
ChordPlayerComponent::ChordPlayerComponent(ProjectManager * pm)
{
    projectManager = pm;
    projectManager->addUIListener(this);

    setWantsKeyboardFocus(true);

    // fonts
    Typeface::Ptr AssistantBold     = Typeface::createSystemTypefaceFor(BinaryData::AssistantBold_ttf, BinaryData::AssistantBold_ttfSize);
    Typeface::Ptr AssistantSemiBold   = Typeface::createSystemTypefaceFor(BinaryData::AssistantSemiBold_ttf, BinaryData::AssistantSemiBold_ttfSize);
    Font fontNormal(AssistantSemiBold);
    fontNormal.setHeight(33);

    // Shortcut container
    containerView_Shortcut = std::make_unique<ShortcutContainerComponent>(projectManager);
    for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
    {
        containerView_Shortcut->shortcutComponent[i]->addShortcutListener(this);
    }
    addAndMakeVisible(containerView_Shortcut.get());

    // Main container (holds visualiser and toolbar)
    containerView_Main = std::make_unique<Component>();
    addAndMakeVisible(containerView_Main.get());

    // Per-note frequencies label
    label_NoteFrequencies = std::make_unique<Label>();
    label_NoteFrequencies->setText("", dontSendNotification);
    label_NoteFrequencies->setJustificationType(Justification::left);
    label_NoteFrequencies->setFont(fontNormal);
    label_NoteFrequencies->setMinimumHorizontalScale(0.7f);
    containerView_Main->addAndMakeVisible(label_NoteFrequencies.get());

    // Transport toolbar (replaces individual play/stop/record/panic/loop/simultaneous/save/load buttons)
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
    chordPlayerSettingsComponent = std::make_unique<ChordPlayerSettingsComponent>(projectManager);
    addAndMakeVisible(chordPlayerSettingsComponent.get());
    chordPlayerSettingsComponent->setVisible(false);

    // visualiser component
    visualiserContainerComponent = std::make_unique<VisualiserContainerComponent2>(projectManager, AUDIO_MODE::MODE_CHORD_PLAYER);
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

    clearHeldNotes();

    startTimerHz(TIMER_UPDATE_RATE);
}

ChordPlayerComponent::~ChordPlayerComponent() { }

void ChordPlayerComponent::resized()
{
    auto bounds = getLocalBounds();
    const int h = bounds.getHeight();
    const int w = bounds.getWidth();

    // Shortcut bar at top
    int shortcutH = (int)(h * Layout::kShortcutBarHeightRatio);
    containerView_Shortcut->setBounds(bounds.removeFromTop(shortcutH));

    // Toolbar at bottom
    int toolbarH = (int)(h * Layout::kToolbarHeightRatio);
    transportToolbar->setBounds(bounds.removeFromBottom(toolbarH));

    // Main container fills the remainder (visualiser area)
    containerView_Main->setBounds(bounds);

    // Visualiser fills the main container
    visualiserContainerComponent->setBounds(0, 0, bounds.getWidth(), bounds.getHeight());

    // Note frequencies label at top of main container area
    float localScale = w / Layout::kRefContentWidth;
    int labelH = (int)(33 * localScale);
    label_NoteFrequencies->setBounds(10, 0, bounds.getWidth() - 20, labelH);
    label_NoteFrequencies->setFont(jmax(10.0f, 33.0f * localScale));

    // Settings overlay fills entire component
    chordPlayerSettingsComponent->setBounds(0, 0, getWidth(), getHeight());
}


void ChordPlayerComponent::timerCallback()
{
    if (projectManager->chordPlayerProcessor->getPlaystate() == PLAY_STATE::PLAYING)
    {
        MessageManagerLock lock;

        float val = projectManager->chordPlayerProcessor->repeater->getProgressBarValue();
        transportToolbar->setProgressValue(val);

        String text = projectManager->chordPlayerProcessor->repeater->getTimeRemainingInSecondsString();
        transportToolbar->setPlayingText(text);
    }
    else
    {
        float val = projectManager->chordPlayerProcessor->repeater->getProgressBarValue();
        transportToolbar->setProgressValue(val);
    }
}

void ChordPlayerComponent::paint (Graphics& g)
{
    // Vector background — dark fill replacing MainContainerBackground2x_png
    g.fillAll(juce::Colour(45, 44, 44));
}


void ChordPlayerComponent::buttonClicked (Button*button)
{
    // No individual transport buttons anymore — handled by TransportToolbarComponent::Listener
}

// TransportToolbarComponent::Listener implementations
void ChordPlayerComponent::transportPlayClicked()
{
    projectManager->setPlayerCommand(PLAYER_COMMANDS::COMMAND_PLAYER_PLAYPAUSE);
}

void ChordPlayerComponent::transportStopClicked()
{
    projectManager->setPlayerCommand(PLAYER_COMMANDS::COMMAND_PLAYER_STOP);
}

void ChordPlayerComponent::transportRecordClicked()
{
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

void ChordPlayerComponent::transportPanicClicked()
{
    projectManager->setPanicButton();
}

void ChordPlayerComponent::transportLoopToggled(bool isOn)
{
    if (isOn)
        projectManager->setPlayerPlayMode(PLAY_MODE::LOOP);
    else
        projectManager->setPlayerPlayMode(PLAY_MODE::NORMAL);
}

void ChordPlayerComponent::transportSimultaneousToggled(bool isOn)
{
    projectManager->setProjectSettingsParameter(PLAYER_PLAY_SIMULTANEOUS, isOn ? 1.0 : 0.0);
}

void ChordPlayerComponent::transportSaveClicked()
{
    projectManager->saveProfileForMode(AUDIO_MODE::MODE_CHORD_PLAYER);
}

void ChordPlayerComponent::transportLoadClicked()
{
    projectManager->loadProfileForMode(AUDIO_MODE::MODE_CHORD_PLAYER);
}

void ChordPlayerComponent::openChordPlayerSettingsForShortcut(int shortcutRef)
{
    chordPlayerSettingsComponent->openView(shortcutRef);
}

void ChordPlayerComponent::updateChordPlayerUIParameter(int shortcutRef, int paramIndex)
{
    // shortcut units first
    if (paramIndex == SHORTCUT_IS_ACTIVE)
    {
        bool shouldBeActive = projectManager->getChordPlayerParameter(shortcutRef, SHORTCUT_IS_ACTIVE).operator bool();
        containerView_Shortcut->shortcutComponent[shortcutRef]->setState(shouldBeActive);
    }
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
        label_NoteFrequencies->setText(computeNoteFrequenciesStringForShortcut(shortcutRef), dontSendNotification);
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
        label_NoteFrequencies->setText(computeNoteFrequenciesStringForShortcut(shortcutRef), dontSendNotification);
    }
    else if (paramIndex == CHORD_TYPE)
    {
        int chordRef            = projectManager->getChordPlayerParameter(shortcutRef, CHORD_TYPE).operator int();
        String chordString      = ProjectStrings::getChordTypeArray().getReference(chordRef-1);
        String comboString(""); comboString.append(chordString, 20);
        containerView_Shortcut  ->shortcutComponent[shortcutRef]->setChordTypeString(comboString);
        label_NoteFrequencies->setText(computeNoteFrequenciesStringForShortcut(shortcutRef), dontSendNotification);
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
        label_NoteFrequencies->setText(computeNoteFrequenciesStringForShortcut(shortcutRef), dontSendNotification);
    }
    else if (paramIndex == INSTRUMENT_TYPE || paramIndex == WAVEFORM_TYPE)
    {
        String stringLabel;
        String stringWaveform;

        int waveformType = projectManager->getChordPlayerParameter(shortcutRef, WAVEFORM_TYPE).operator int();
        if (waveformType == 0)
        {
            const int instrumentType = projectManager->getChordPlayerParameter(shortcutRef, INSTRUMENT_TYPE).operator int();
            String instName = "Grand Piano";
            switch (instrumentType)
            {
                case 1: instName = "Grand Piano"; break;
                case 4: instName = "Acoustic Guitar"; break;
                case 5: instName = "Harp"; break;
                case 7: instName = "Strings"; break;
                case 3: instName = "Flute"; break;
                default: break;
            }

            stringLabel         = "Instrument :";
            stringWaveform      = instName;
        }
        else { stringWaveform = TSS::Waveforms::getName(waveformType);  stringLabel = "Type :"; }

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
        label_NoteFrequencies->setText(computeNoteFrequenciesStringForShortcut(shortcutRef), dontSendNotification);
    }

    chordPlayerSettingsComponent->syncUI();
}

juce::String ChordPlayerComponent::computeNoteFrequenciesStringForShortcut(int shortcutRef)
{
    if (!projectManager || !projectManager->chordPlayerProcessor || !projectManager->frequencyManager || !projectManager->frequencyManager->scalesManager)
        return {};

    auto cmgr = projectManager->chordPlayerProcessor->chordManager[shortcutRef];
    if (!cmgr) return {};

    juce::Array<int> notes = cmgr->getMIDIKeysForChord();
    if (notes.isEmpty()) return {};

    juce::String result;
    result << "Notes (Hz): ";

    bool first = true;
    for (int i = 0; i < notes.size(); ++i)
    {
        const int midiNote = notes.getReference(i);
        if (midiNote < 0 || midiNote > 127) continue;

        const double freq = projectManager->frequencyManager->scalesManager->getFrequencyForMIDINoteShortcut(midiNote, shortcutRef);
        if (freq <= 0.0) continue;

        const int key = midiNote % 12;
        const int oct = (midiNote / 12) - 1;
        juce::String noteName = ProjectStrings::getKeynoteArray().getReference(key);

        if (!first) result << ", ";
        first = false;
        result << noteName << oct << " " << juce::String(freq, 3, false) << "Hz";
    }

    return result;
}

void ChordPlayerComponent::updateSettingsUIParameter(int settingIndex)
{
    if (settingIndex == PLAYER_PLAY_IN_LOOP)
    {
        transportToolbar->setLoopState(projectManager->getProjectSettingsParameter(settingIndex));
    }
    else if (settingIndex == PLAYER_PLAY_SIMULTANEOUS)
    {
        transportToolbar->setSimultaneousState(projectManager->getProjectSettingsParameter(settingIndex));
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
