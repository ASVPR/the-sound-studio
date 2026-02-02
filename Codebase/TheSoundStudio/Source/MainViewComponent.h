/*
  ==============================================================================

    MainViewComponent.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "ProjectManager.h"
#include "ChordPlayerComponent.h"
#include "ChordScannerComponent.h"
#include "FrequencyPlayerComponent.h"
#include "FrequencyScannerComponent.h"
#include "FrequencyToLightComponent.h"
#include "RealtimeAnalysisComponent.h"
#include "LissajousCurveComponent.h"
#include "SettingsComponent.h"
#include "PluginRackComponent.h"
#include "FundamentalFrequencyComponent.h"
#include "FeedbackModuleComponent.h"
#include "CustomLookAndFeel.h"

// 1. menu system
// 2. buttons and container views
class MainViewComponent    : public Component, public Button::Listener
{
public:
    // Make sure that the enum is ordered according the way it is displayed in the UI
    enum class MenuItem
    {
        ChordPlayer = 0, ChordScanner, FundamentalFrequency, FrequencyPlayer,
        FrequencyScanner, RealTimeAnalysis, LissajousCurves, Feedback,
        FeedbackHardware, FrequencyLight, Settings, NumItems
    };

    MainViewComponent(ProjectManager * pm);
    ~MainViewComponent();

    void paint (Graphics&) override;
    void paintOverChildren(juce::Graphics& g) override;
    void resized() override;
    void buttonClicked (Button*button)override;

    void setScale(float factor);
    MenuItem currentMenuItem;
private:
    void switchMenuView();

    ProjectManager* projectManager;
    CustomLookAndFeel lookAndFeel;
    
    // Buttons
    std::unique_ptr<ImageButton>                  menuButton_ASVPRTool;

    std::unique_ptr<ImageComponent>               imageComponent_Sidebar;
    Image imageSidebar;
    ImageButton* currentSelectedButton;
    
    // container views
    Component mainContainerComponent;
    std::vector<std::unique_ptr<MenuViewInterface>> menuViews;

    // Menu buttons
    std::vector<std::unique_ptr<ImageButton>> menuButtons;

    const std::map<MenuItem, AUDIO_MODE> menuItemToModeMap {
        {MenuItem::ChordPlayer, AUDIO_MODE::MODE_CHORD_PLAYER},
        {MenuItem::ChordScanner, AUDIO_MODE::MODE_CHORD_SCANNER},
        {MenuItem::FundamentalFrequency, AUDIO_MODE::MODE_FUNDAMENTAL_FREQUENCY},
        {MenuItem::FrequencyPlayer, AUDIO_MODE::MODE_FREQUENCY_PLAYER},
        {MenuItem::FrequencyScanner, AUDIO_MODE::MODE_FREQUENCY_SCANNER},
        {MenuItem::RealTimeAnalysis, AUDIO_MODE::MODE_REALTIME_ANALYSIS},
        {MenuItem::LissajousCurves, AUDIO_MODE::MODE_LISSAJOUS_CURVES},
        {MenuItem::Feedback, AUDIO_MODE::MODE_FEEDBACK_MODULE},
        {MenuItem::FeedbackHardware, AUDIO_MODE::MODE_FEEDBACK_MODULE_HARDWARE},
        {MenuItem::FrequencyLight, AUDIO_MODE::MODE_FREQUENCY_TO_LIGHT}
    };

    // Responsive UI Layout Variables
    float menuSideBarWidthRatio = 0.18f;  // 18% of window width
    float minSidebarWidth = 280.0f;
    float maxSidebarWidth = 400.0f;
    
    float buttonHeightRatio = 0.05f;  // 5% of window height
    float minButtonHeight = 50.0f;
    float maxButtonHeight = 80.0f;
    
    float logoHeightRatio = 0.045f;  // 4.5% of window height
    
    // Calculated values (updated in resized())
    int menuSideBarWidth    = 354;
    int mainHeight          = 1440;
    int containerWidth      = 1566;
    int containerHeight     = 1440;
    int buttonWidth         = 354;
    int buttonHeight        = 70;
    int buttonTopMargin     = 134;
    int logoButtonWidth     = 298;
    int logoButtonHeight    = 65;
    
    // Image Cache
    Image imageMenuButtonNormal;
    Image imageMenuButtonSelected;
    Image imageLogoButton;
    
    Label labelProjectVersion;
    float scaleFactor = 1.0f;
    
    // Window size tracking for responsive behavior
    int lastWindowWidth = 0;
    int lastWindowHeight = 0;
    
    void updateLayoutForWindowSize();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainViewComponent)
};
