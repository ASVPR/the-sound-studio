/*
  ==============================================================================

    MainViewComponent.h
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <array>
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

class SidebarMenuButton;

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
        FrequencyLight, Settings, NumItems
    };

    MainViewComponent(ProjectManager * pm);
    ~MainViewComponent();

    void paint (Graphics&) override;
    void resized() override;
    void buttonClicked (Button*button)override;

    void setScale(float factor);
    MenuItem currentMenuItem;
private:
    void switchMenuView();
    void updateLayoutForWindowSize();

    struct MenuEntry
    {
        MenuItem item;
        const char* label;
        bool hasMode;
        AUDIO_MODE mode;
    };

    static const std::array<MenuEntry, static_cast<size_t>(MenuItem::NumItems)> menuEntries;

    ProjectManager* projectManager;
    CustomLookAndFeel lookAndFeel;
    
    // Buttons
    std::unique_ptr<ImageButton>                  menuButton_ASVPRTool;

    SidebarMenuButton* currentSelectedButton = nullptr;
    
    // container views
    Component mainContainerComponent;
    std::vector<std::unique_ptr<MenuViewInterface>> menuViews;

    // Menu buttons
    std::vector<std::unique_ptr<SidebarMenuButton>> menuButtons;

    // Image Cache
    
    Label labelProjectVersion;
    float scaleFactor = 1.0f;
    
    // Window size tracking for responsive behavior
    int lastWindowWidth = 0;
    int lastWindowHeight = 0;

    Rectangle<int> sidebarBounds;
    Rectangle<int> contentBounds;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainViewComponent)
};
