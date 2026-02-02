/*
  ==============================================================================

    ShortcutComponent.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "ProjectManager.h"
#include "CustomLookAndFeel.h"
#include <memory>

//==============================================================================
/*
*/
class ShortcutComponent : public Component, public Button::Listener
{
public:
    ShortcutComponent(int shortcut, ProjectManager * pm);
    ~ShortcutComponent(){}
    
    //-------------------------------------------------
    // Shortcut Listener Interface
    //=================================================
    class ShortcutListener
    {
    public:
        virtual ~ShortcutListener() = default;
        
        // called from settings button to open settings creen in chordplayercomponent
        virtual void openChordPlayerSettingsForShortcut(int shortcutRef) {}
    };
    
    void addShortcutListener(ShortcutListener* toAdd)
    {
        shortcutListeners.add(toAdd);
    }
    void removeShortcutListener(ShortcutListener* toRemove)
    {
        shortcutListeners.remove(toRemove);
    }
    
    ListenerList<ShortcutListener>         shortcutListeners;

    void paint (Graphics&) override{}
    void resized() override;
    void buttonClicked (Button*button)override;
    
    std::unique_ptr<Label> shortcutRefLabel;
    
    void setState(bool isActive);
    void setMute(bool isMuted);
    void setLoop(bool isLooped);
    
    void setChordString(String chord); // key
    void setChordTypeString(String chordType); // type and octave
    void setFrequency(float newFrequency);
    void setInstrumentString(String instString, String titleString);
    void setScaleString(String scaleString);
    
    float scaleFactor = 0.5;
    void setScale(float factor)
    {
        scaleFactor = factor;
        lookAndFeel.setScale(scaleFactor);
    }
    
private:
    ProjectManager * projectManager;
    
    int shortcutRef;
    
    int activeState = 0; // 0 = inactive - 1 = active
    bool muted;
    bool loop;
    
    String chord;
    String chordType;
    String frequencyHz;
    String scaleString;
    String instString;
    
    std::unique_ptr<ImageButton>button_Settings;
    std::unique_ptr<ImageButton>button_Mute;
    std::unique_ptr<ImageButton>button_Loop;
    
    std::unique_ptr<ImageButton>button_Delete;
    std::unique_ptr<ImageButton>button_Add;
    
    std::unique_ptr<Label>label_Chord;
    std::unique_ptr<Label>label_Chordtype; // & octave
    std::unique_ptr<Label>label_Scale;
    std::unique_ptr<Label>label_Instrument;
    std::unique_ptr<Label>label_Frequency; // should be chord frequency, but how ?

    std::unique_ptr<Label>label_ChordValue;
    std::unique_ptr<Label>label_ChordtypeValue;
    std::unique_ptr<Label>label_ScaleValue;
    std::unique_ptr<Label>label_InstrumentValue;
    std::unique_ptr<Label>label_FrequencyValue;
    
    std::unique_ptr<Component>containerView_Active;
    std::unique_ptr<Component>containerView_Inactive;
    
    // Image Chache
    Image imageSettingsIcon;
    Image imageMuteIcon;
    Image imageLoopIcon;
    Image imageBackground;
    Image imageDelete; // delete, not close i think !!!!
    Image imageAddIcon;
    
    // UI Layout Variables
    int insetDelete         = 16;
    int deleteTopMargin     = 21;
    int deleteSize          = 35;
    
    int backgroundTopMargin = 38;
    int backgroundLeftMargin= insetDelete;
    int backgroundWidth     = 303;
    int backgroundHeight    = 271;
    
    int insetLeftLabels     = 16 + 8;
    int insetRightLabels    = backgroundLeftMargin+backgroundWidth-130;
    int labelHeight         = 40;
    int labelWidth          = 120;
    int label1Top           = 54;
    int label2Top           = label1Top + labelHeight;
    int label3Top           = label2Top + labelHeight;
    int label4Top           = label3Top + labelHeight;
    int label5Top           = label4Top + labelHeight;
    
    int mainWidth           = backgroundWidth + (2* insetDelete);
    int mainHeight          = backgroundHeight + 73;
    
    int settingsLeftMargin  = 16 + backgroundLeftMargin;
    int settingsTopMargin   = 223 + backgroundTopMargin;
    int settingsSize        = 32;
    
    int muteLeftMargin      = 136 + backgroundLeftMargin;
    int muteTopMargin       = settingsTopMargin;
    int muteSize            = settingsSize;
    
    int loopLeftMargin      = 247 + backgroundLeftMargin;
    int loopTopMargin       = 218 + backgroundTopMargin;
    int loopWidth           = 39;
    int loopHeight          = 35;
    
    ImageComponent * backgroundImageComp;
    
    CustomLookAndFeel lookAndFeel;
};


// contains shortbutton components and left right buttons scroll etc
class ShortcutContainerComponent : public Component, public Button::Listener
{
public:
    ShortcutContainerComponent(ProjectManager * pm);
    ~ShortcutContainerComponent(){}
    
    void paint (Graphics&) override{}
    void resized() override;
    void buttonClicked (Button*button) override;
    void moveViewport();
    
    float scaleFactor = 0.5;
    void setScale(float factor)
    {
        scaleFactor = factor;
        
        for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
        {
            shortcutComponent[i]->setScale(scaleFactor);
        }
        
        lookAndFeel.setScale(scaleFactor);
    }
    
public: std::unique_ptr<ShortcutComponent>shortcutComponent[NUM_SHORTCUT_SYNTHS];
    
private:
    
    ProjectManager * projectManager;
    
    int shiftToRight = 0; // number of moves right into list
    
    std::unique_ptr<ImageButton>button_Left;
    std::unique_ptr<ImageButton>button_Right;
    std::unique_ptr<Viewport>viewport;
    std::unique_ptr<Component>shortcutsContainer;
    
    Image imageLeftButton;
    Image imageRightButton;
    
    // UI Variables
    int leftButtonLeftMargin    = 48;
    int leftButtonTopMargin     = 153;
    
    int rightButtonLeftMargin   = 1475;
    int rightButtonTopMargin    = leftButtonTopMargin;
    
    int buttonWidth             = 27;
    int buttonHeight            = 47;
    
    int shortcutWidth           = 303 + 24;
    int shortcutSpace           = 10;
    int containerWidth          = (shortcutWidth + shortcutSpace) * (NUM_SHORTCUT_SYNTHS) - ((shortcutWidth + shortcutSpace)* 0.5);
    
    int viewportLeftMargin      = 107 - 10;
    int viewportTopMargin       = 0;
    int viewportWidth           = (shortcutWidth + shortcutSpace) * 4;
    
    CustomLookAndFeel lookAndFeel;
};


class ShortcutComponentFrequency : public Component, public Button::Listener
{
public:
    ShortcutComponentFrequency(int shortcut, ProjectManager * pm);
    ~ShortcutComponentFrequency(){}
    
    //-------------------------------------------------
    // Shortcut Listener Interface
    //=================================================
    class ShortcutFrequencyListener
    {
    public:
        virtual ~ShortcutFrequencyListener() = default;
        
        // called from settings button to open settings creen in chordplayercomponent
        virtual void openFrequencyPlayerSettingsForShortcut(int shortcutRef) {}
    };
    
    void addShortcutListener(ShortcutFrequencyListener* toAdd)
    {
        shortcutListeners.add(toAdd);
    }
    void removeShortcutListener(ShortcutFrequencyListener* toRemove)
    {
        shortcutListeners.remove(toRemove);
    }
    
    ListenerList<ShortcutFrequencyListener>         shortcutListeners;
    
    void paint (Graphics&) override{}
    void resized() override;
    void buttonClicked (Button*button)override;
    
    std::unique_ptr<Label> shortcutRefLabel;
    
    void setState(bool isActive);
    void setMute(bool isMuted);
    void setLoop(bool isLooped);
    
    void setWaveTypeString(String waveType);
    void setFrequency(float newFrequency);
    
    float scaleFactor = 0.5;
    void setScale(float factor)
    {
        scaleFactor = factor;
        lookAndFeel.setScale(scaleFactor);
    }
    
private:
    ProjectManager * projectManager;
    
    int shortcutRef;
    
    int activeState = 0; // 0 = inactive - 1 = active
    bool muted;
    bool loop;
    

    String waveType;
    String frequencyHz;
    
    std::unique_ptr<ImageButton>button_Settings;
    std::unique_ptr<ImageButton>button_Mute;
    std::unique_ptr<ImageButton>button_Loop;
    
    std::unique_ptr<ImageButton>button_Delete;
    std::unique_ptr<ImageButton>button_Add;
    
    std::unique_ptr<Label>label_Wavetype;
    std::unique_ptr<Label>label_Frequency;

    std::unique_ptr<Label>label_WavetypeValue;
    std::unique_ptr<Label>label_FrequencyValue;
    
    std::unique_ptr<Component>containerView_Active;
    std::unique_ptr<Component>containerView_Inactive;
    
    ImageComponent * backgroundImageComp;
    
    // Image Chache
    Image imageSettingsIcon;
    Image imageMuteIcon;
    Image imageLoopIcon;
    Image imageBackground;
    Image imageDelete; // delete, not close i think !!!!
    Image imageAddIcon;
    
    // UI Layout Variables
    int insetDelete         = 16;
    int deleteTopMargin     = 21;
    int deleteSize          = 35;
    
    int backgroundTopMargin = 38;
    int backgroundLeftMargin= insetDelete;
    int backgroundWidth     = 303;
    int backgroundHeight    = 271;
    
    int insetLeftLabels     = 16 + 8;
    int insetRightLabels    = backgroundLeftMargin+backgroundWidth-130;
    int labelHeight         = 40;
    int labelWidth          = 120;
    int label1Top           = 54;
    int label2Top           = label1Top + labelHeight;
    int label3Top           = label2Top + labelHeight;
    
    int mainWidth           = backgroundWidth + (2* insetDelete);
    int mainHeight          = backgroundHeight + 73;
    
    int settingsLeftMargin  = 16 + backgroundLeftMargin;
    int settingsTopMargin   = 223 + backgroundTopMargin;
    int settingsSize        = 32;
    
    int muteLeftMargin      = 136 + backgroundLeftMargin;
    int muteTopMargin       = settingsTopMargin;
    int muteSize            = settingsSize;
    
    int loopLeftMargin      = 247 + backgroundLeftMargin;
    int loopTopMargin       = 218 + backgroundTopMargin;
    int loopWidth           = 39;
    int loopHeight          = 35;
    
    CustomLookAndFeel lookAndFeel;
};


// contains shortbutton components and left right buttons scroll etc
class ShortcutFrequencyContainerComponent : public Component, public Button::Listener
{
public:
    ShortcutFrequencyContainerComponent(ProjectManager * pm);
    ~ShortcutFrequencyContainerComponent()
    {
        
    }
    
    void paint (Graphics&) override{}
    void resized() override;
    void buttonClicked (Button*button) override;
    void moveViewport();
    
    float scaleFactor = 0.5;
    void setScale(float factor)
    {
        scaleFactor = factor;
        
        for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
        {
            shortcutComponent[i]->setScale(scaleFactor);
        }
            
        lookAndFeel.setScale(scaleFactor);
    }
    
public: std::unique_ptr<ShortcutComponentFrequency>shortcutComponent[NUM_SHORTCUT_SYNTHS];
    
private:
    
    ProjectManager * projectManager;
    
    int shiftToRight = 0; // number of moves right into list
    
    std::unique_ptr<ImageButton>button_Left;
    std::unique_ptr<ImageButton>button_Right;
    std::unique_ptr<Viewport>viewport;
    std::unique_ptr<Component>shortcutsContainer;
    
    Image imageLeftButton;
    Image imageRightButton;
    
    
    // UI Variables
    int leftButtonLeftMargin    = 48;
    int leftButtonTopMargin     = 153;
    
    int rightButtonLeftMargin   = 1475;
    int rightButtonTopMargin    = leftButtonTopMargin;
    
    int buttonWidth             = 27;
    int buttonHeight            = 47;
    
    int shortcutWidth           = 303 + 24;
    int shortcutSpace           = 10;
    int containerWidth          = (shortcutWidth + shortcutSpace) * (NUM_SHORTCUT_SYNTHS) - ((shortcutWidth + shortcutSpace)* 0.5);
    
    int viewportLeftMargin      = 107 - 10;
    int viewportTopMargin       = 0;
    int viewportWidth           = (shortcutWidth + shortcutSpace) * 4;
    
    CustomLookAndFeel lookAndFeel;
};
