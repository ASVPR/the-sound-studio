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
#include "CustomLookAndFeel.h"
#include "UI/DesignSystem.h"

// 1. menu system
// 2. buttons and container views
class MainViewComponent    : public Component, public Button::Listener
{
public:
    // Make sure that the enum is ordered according the way it is displayed in the UI
    enum class MenuItem
    {
        ChordPlayer = 0, ChordScanner, FundamentalFrequency, FrequencyPlayer,
        FrequencyScanner, RealTimeAnalysis, LissajousCurves,
        FrequencyLight, Settings, NumItems
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
        {MenuItem::FrequencyLight, AUDIO_MODE::MODE_FREQUENCY_TO_LIGHT}
    };

    // Image Cache
    Image imageMenuButtonNormal;
    Image imageMenuButtonSelected;
    Image imageLogoButton;

    Label labelProjectVersion;
    float scaleFactor = 1.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainViewComponent)
};
