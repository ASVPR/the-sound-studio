/*
  ==============================================================================

    TransportToolbarComponent.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "CustomProgressBar.h"
#include "CustomLookAndFeel.h"
#include "UI/DesignSystem.h"

struct TransportButtonVisibility
{
    bool play          = true;
    bool stop          = true;
    bool record        = true;
    bool panic         = true;
    bool progress      = true;
    bool loop          = true;
    bool simultaneous  = true;
    bool save          = true;
    bool load          = true;
    bool playingLabel  = true;
};

class TransportToolbarComponent : public Component, public Button::Listener
{
public:
    struct Listener
    {
        virtual ~Listener() = default;
        virtual void transportPlayClicked() {}
        virtual void transportStopClicked() {}
        virtual void transportRecordClicked() {}
        virtual void transportPanicClicked() {}
        virtual void transportLoopToggled(bool isOn) {}
        virtual void transportSimultaneousToggled(bool isOn) {}
        virtual void transportSaveClicked() {}
        virtual void transportLoadClicked() {}
    };

    using ButtonVisibility = TransportButtonVisibility;

    TransportToolbarComponent(ButtonVisibility visibility = {});
    ~TransportToolbarComponent() override;

    void paint(Graphics& g) override;
    void resized() override;
    void buttonClicked(Button* button) override;

    void addTransportListener(Listener* l)    { listeners.add(l); }
    void removeTransportListener(Listener* l) { listeners.remove(l); }

    void setProgressValue(float value);
    void setPlayingText(const juce::String& text);
    void setLoopState(bool isOn);
    void setSimultaneousState(bool isOn);

    CustomProgressBar* getProgressBar() { return progressBar.get(); }

private:
    ButtonVisibility vis;
    ListenerList<Listener> listeners;
    CustomLookAndFeel lookAndFeel;

    std::unique_ptr<DrawableButton> button_Play;
    std::unique_ptr<DrawableButton> button_Stop;
    std::unique_ptr<DrawableButton> button_Record;
    std::unique_ptr<DrawableButton> button_Panic;

    std::unique_ptr<ToggleButton> button_Loop;
    std::unique_ptr<ToggleButton> button_Simultaneous;
    std::unique_ptr<TextButton>   button_Save;
    std::unique_ptr<TextButton>   button_Load;

    std::unique_ptr<CustomProgressBar> progressBar;
    std::unique_ptr<Label> label_Playing;

    // Vector icon helpers
    static DrawablePath createPlayIcon(float size, juce::Colour colour);
    static DrawablePath createStopIcon(float size, juce::Colour colour);
    static DrawablePath createRecordIcon(float size, juce::Colour colour);
    static DrawablePath createPanicIcon(float size, juce::Colour colour);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportToolbarComponent)
};
