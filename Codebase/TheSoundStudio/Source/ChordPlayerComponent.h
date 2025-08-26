/*
  ==============================================================================

    ChordPlayerComponent.h
    Created: 13 Mar 2019 9:54:54pm
    Author:  Ziv Elovitch - The Sound Studio Team

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
#include "MenuViewInterface.h"
#include <memory>

class ChordPlayerComponent :
        public MenuViewInterface,
        public Button::Listener,
        public ShortcutComponent::ShortcutListener,
        public ProjectManager::UIListener,
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
        
        chordPlayerSettingsComponent    ->setScale(scaleFactor);
        containerView_Shortcut          ->setScale(scaleFactor);
        visualiserContainerComponent    ->setScale(scaleFactor);
        
        lookAndFeel.setScale(scaleFactor);
    }
    
private:
    
    ProjectManager * projectManager;
    
    std::unique_ptr<ChordPlayerSettingsComponent> chordPlayerSettingsComponent;
    
    std::unique_ptr<ImageComponent> imageComp;
    
    std::unique_ptr<ShortcutContainerComponent> containerView_Shortcut;
    std::unique_ptr<Component> containerView_Main;
    
    std::unique_ptr<Component> containerView_Visualiser;
    
    std::unique_ptr<ImageButton> button_Record;
    std::unique_ptr<ImageButton> button_Play;
    std::unique_ptr<ImageButton> button_Stop;
    std::unique_ptr<ImageButton> button_Panic;
    std::unique_ptr<Label> label_Playing;
    
    
    std::unique_ptr<VisualiserContainerComponent2> visualiserContainerComponent;
    
    // need checkbox
    std::unique_ptr<ToggleButton> button_PlayInLoop;
    std::unique_ptr<ToggleButton> button_PlaySimultaneous;
    std::unique_ptr<TextButton>   button_Save;
    std::unique_ptr<TextButton>   button_Load;
    
    // needs custom progress bar
    std::unique_ptr<CustomProgressBar> progressBar;
    
    
    // Image Cache
    Image imageCheckboxBackground;
    Image imageMainContainerBackground;
    Image imageShortcutContainerBackground;
    Image imageShortcutBackground;
    Image imagePanicButton;
    Image imagePlayButton;
    Image imageProgresBarFill;
    Image imageSettingsIcon;
    Image imageAddIcon;
    Image imageCloseIcon;
    Image imageLeftIcon;
    Image imageRightIcon;
    Image imageLoopIcon;
    Image imageMuteIcon;
    Image imageStopButton;
    Image imageRecordButton;
    
    Image imageFFTMockup;
    Image imageColorSpectrumMockup;
    Image imageOctaveSpectrumMockup;
    
    
    // UI Layout Variables
    int mainContainerHeight = 1096;
    int fftLeftMargin       = 48;
    int fftTopMargin        = 10;
    int fftWidth            = 1476;
    int fftHeight           = 310;
    
    int colorLeftMargin     = fftLeftMargin;
    int colorTopMargin      = fftTopMargin + fftHeight + 67;
    int colorWidth          = 742;
    int colorHeight         = 271;
    
    int octaveLeftMergin    = 822;
    int octaveTopMargin     = colorTopMargin;
    int octaveWidth         = 699;
    int octaveHeight        = colorHeight;
    
    int shortcutHeight      = 344;
    int shortcutWidth       = 1566;
    
    int recordLeftMargin    = 102;
    int recordTopMargin     = 835;
    int recordWidth         = 95;
    int recordHeight        = 95;
    
    int playingLeftMargin  = 219;
    int playingTopMargin    = 799;
    int playingWidth        = 205;
    int playingHeight       = 33;
    
    int playLeftMargin      = 521;
    int playTopMargin       = 970;
    int playWidth           = 249;
    int playHeight          = 61;
    
    int stopLeftMargin      = 802;
    int stopTopMargin       = 970;
    
    int panicLeftMargin     = 1360;
    int panicTopMargin      = 910;
    int panicWidth          = 180;
    int panicHeight         = panicWidth;
    
    int progressLeftMargin  = 220;
    int progressTopMargin   = 856;
    int progressWidth       = 1128;
    int progressHeight      = 53;
    
    CustomLookAndFeel lookAndFeel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChordPlayerComponent)
};
