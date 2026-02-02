/*
  ==============================================================================

    FrequencyPlayerComponent.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "FrequencyPlayerSettingsComponent.h"
#include "ShortcutComponent.h"
#include "ProjectManager.h"
#include "VisualiserContainerComponent.h"
#include "CustomProgressBar.h"
#include "TransportToolbarComponent.h"
#include "MenuViewInterface.h"
#include "UI/DesignSystem.h"
#include <memory>

class FrequencyPlayerComponent :
    public MenuViewInterface,
    public Button::Listener,
    public ShortcutComponentFrequency::ShortcutFrequencyListener,
    public ProjectManager::UIListener,
    public TransportToolbarComponent::Listener,
    public Timer
{
public:
    FrequencyPlayerComponent(ProjectManager * pm);
    ~FrequencyPlayerComponent();

    void paint (Graphics&) override;
    void resized() override;

    void buttonClicked (Button*button)override;
    void openFrequencyPlayerSettingsForShortcut(int shortcutRef)override;

    void updateFrequencyPlayerUIParameter(int shortcutRef, int paramIndex)override;

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
        if (scaleFactor != factor)
        {
            scaleFactor = factor;
            lookAndFeel.setScale(scaleFactor);

            if (frequencyPlayerSettingsComponent)
                frequencyPlayerSettingsComponent->setScale(scaleFactor);
            if (containerView_Shortcut)
                containerView_Shortcut->setScale(scaleFactor);
            if (visualiserContainerComponent)
                visualiserContainerComponent->setScale(scaleFactor);

            resized();
            repaint();
        }
    }

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

    std::unique_ptr<FrequencyPlayerSettingsComponent> frequencyPlayerSettingsComponent;

    std::unique_ptr<ShortcutFrequencyContainerComponent> containerView_Shortcut;
    std::unique_ptr<Component> containerView_Main;

    std::unique_ptr<VisualiserContainerComponent2> visualiserContainerComponent;

    std::unique_ptr<TransportToolbarComponent> transportToolbar;

    CustomLookAndFeel lookAndFeel;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FrequencyPlayerComponent)
};
