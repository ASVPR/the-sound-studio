/*
  ==============================================================================

    ChordPlayerComponent.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "ChordPlayerSettingsComponent.h"
#include "ShortcutComponent.h"
#include "ProjectManager.h"
#include "VisualiserContainerComponent.h"
#include "CustomLookAndFeel.h"
#include "CustomProgressBar.h"
#include "TransportToolbarComponent.h"
#include "MenuViewInterface.h"
#include "UI/DesignSystem.h"
#include <memory>

class ChordPlayerComponent :
        public MenuViewInterface,
        public Button::Listener,
        public ShortcutComponent::ShortcutListener,
        public ProjectManager::UIListener,
        public TransportToolbarComponent::Listener,
        public Timer
{
public:
    ChordPlayerComponent(ProjectManager * pm);
    ~ChordPlayerComponent();

    void paint (Graphics&) override;
    void resized() override;

    void buttonClicked (Button*button)override;
    void openChordPlayerSettingsForShortcut(int shortcutRef)override;

    void updateChordPlayerUIParameter(int shortcutRef, int paramIndex)override;

    void updateSettingsUIParameter(int settingIndex) override;

    bool keyPressed (const KeyPress& key)override;
    bool keyStateChanged (bool isKeyDown)override;

    bool shortcutKeyStateDown[NUM_CHORD_PLAYER_SHORTCUT_KEYS];
    juce_wchar shortcutKey[NUM_CHORD_PLAYER_SHORTCUT_KEYS];

    void clearHeldNotes();

    void timerCallback() override;

    void closePopups()
    {
        visualiserContainerComponent->setPopupsAreVisible(false);
    }

    float scaleFactor = 0.5;
    void setScale(float factor) override
    {
        scaleFactor = factor;

        if (chordPlayerSettingsComponent)
            chordPlayerSettingsComponent->setScale(scaleFactor);
        if (containerView_Shortcut)
            containerView_Shortcut->setScale(scaleFactor);
        if (visualiserContainerComponent)
            visualiserContainerComponent->setScale(scaleFactor);

        lookAndFeel.setScale(scaleFactor);
        resized();
    }

    juce::String computeNoteFrequenciesStringForShortcut(int shortcutRef);

    // TransportToolbarComponent::Listener
    void transportPlayClicked() override;
    void transportStopClicked() override;
    void transportRecordClicked() override;
    void transportPanicClicked() override;
    void transportLoopToggled(bool isOn) override;
    void transportSimultaneousToggled(bool isOn) override;
    void transportSaveClicked() override;
    void transportLoadClicked() override;

private:

    ProjectManager * projectManager;

    std::unique_ptr<ChordPlayerSettingsComponent> chordPlayerSettingsComponent;

    std::unique_ptr<ShortcutContainerComponent> containerView_Shortcut;
    std::unique_ptr<Component> containerView_Main;

    std::unique_ptr<VisualiserContainerComponent2> visualiserContainerComponent;

    std::unique_ptr<TransportToolbarComponent> transportToolbar;

    std::unique_ptr<Label> label_NoteFrequencies;

    CustomLookAndFeel lookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChordPlayerComponent)
};
