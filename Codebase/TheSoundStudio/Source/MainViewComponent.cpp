/*
  ==============================================================================

    MainViewComponent.cpp

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "MainViewComponent.h"

using Layout = TSS::Design::Layout;

//==============================================================================
MainViewComponent::MainViewComponent(ProjectManager * pm)
{
    projectManager = pm;

    // Image Cache
    Image tssLogo = ImageCache::getFromMemory(BinaryData::icon_128_png, BinaryData::icon_128_pngSize);
    imageLogoButton = tssLogo;
    imageMenuButtonNormal = ImageCache::getFromMemory(BinaryData::Sidebar_Button_Normal_png, BinaryData::Sidebar_Button_Normal_pngSize);
    imageMenuButtonSelected = ImageCache::getFromMemory(BinaryData::Sidebar_ButtonHighlighted_png, BinaryData::Sidebar_ButtonHighlighted_pngSize);

    // Buttons - Use TSS logo for the main application button
    menuButton_ASVPRTool = std::make_unique<ImageButton>();
    menuButton_ASVPRTool->setTriggeredOnMouseDown(true);
    menuButton_ASVPRTool->setImages (false, true, true,
                            tssLogo, 0.999f, Colour (0x00000000),
                            Image(), 1.000f, Colour (0x00000000),
                            tssLogo, 1.000f, Colour (0x00000000));
    menuButton_ASVPRTool->addListener(this);
    addAndMakeVisible(*menuButton_ASVPRTool);

    for (auto i = 0; i < (int)MenuItem::NumItems; i++) {
        auto& menuButton = menuButtons.emplace_back(std::make_unique<ImageButton>());
        menuButton->setTriggeredOnMouseDown(true);
        menuButton->setImages (false, true, true,
            imageMenuButtonNormal, 0.999f, Colour (0x00000000),
            Image(), 1.000f, Colour (0x00000000),
            imageMenuButtonSelected, 1.000f, Colour (0x00000000));
        menuButton->addListener(this);
        addAndMakeVisible(*menuButton);
    }

    // container views
    addAndMakeVisible(mainContainerComponent);

    // menu views
    menuViews.resize((int)MenuItem::NumItems);

    menuViews[(int)MenuItem::ChordPlayer] = std::make_unique<ChordPlayerComponent>(projectManager);
    mainContainerComponent.addAndMakeVisible(*menuViews[(int)MenuItem::ChordPlayer]);

    menuViews[(int)MenuItem::ChordScanner] = std::make_unique<ChordScannerComponent>(projectManager);
    mainContainerComponent.addAndMakeVisible(*menuViews[(int)MenuItem::ChordScanner]);

    menuViews[(int)MenuItem::FrequencyScanner] = std::make_unique<FrequencyScannerComponent>(projectManager);
    mainContainerComponent.addAndMakeVisible(*menuViews[(int)MenuItem::FrequencyScanner]);

    menuViews[(int)MenuItem::FrequencyPlayer] = std::make_unique<FrequencyPlayerComponent>(projectManager);
    mainContainerComponent.addAndMakeVisible(*menuViews[(int)MenuItem::FrequencyPlayer]);

    menuViews[(int)MenuItem::FrequencyLight] = std::make_unique<FrequencyToLightComponent>(projectManager);
    mainContainerComponent.addAndMakeVisible(*menuViews[(int)MenuItem::FrequencyLight]);

    menuViews[(int)MenuItem::LissajousCurves] = std::make_unique<LissajousCurveComponent>(projectManager);
    mainContainerComponent.addAndMakeVisible(*menuViews[(int)MenuItem::LissajousCurves]);

    menuViews[(int)MenuItem::FundamentalFrequency] = std::make_unique<FundamentalFrequencyComponent>(*projectManager);;
    mainContainerComponent.addAndMakeVisible(*menuViews[(int)MenuItem::FundamentalFrequency]);

    menuViews[(int)MenuItem::RealTimeAnalysis] = std::make_unique<RealtimeAnalysisComponent>(projectManager);
    mainContainerComponent.addAndMakeVisible(*menuViews[(int)MenuItem::RealTimeAnalysis]);

    menuViews[(int)MenuItem::Settings] = std::make_unique<SettingsComponent>(projectManager);
    mainContainerComponent.addAndMakeVisible(*menuViews[(int)MenuItem::Settings]);

    currentMenuItem = MenuItem::ChordPlayer;
    switchMenuView();

    String stringProjectVersion(projectManager->getProjectName());
    stringProjectVersion.append(" v", 2);
    stringProjectVersion.append(projectManager->getProjectVersionString(), 10);
    labelProjectVersion.setText(stringProjectVersion, dontSendNotification);
    labelProjectVersion.setJustificationType(Justification::centredLeft);
    addAndMakeVisible(labelProjectVersion);
}

MainViewComponent::~MainViewComponent()
{
    for (auto& button : menuButtons) {
        button->removeListener(this);
    }
}

void MainViewComponent::paint (Graphics& g)
{
    auto bounds = getLocalBounds();
    const int w = bounds.getWidth();
    const int h = bounds.getHeight();

    // Compute sidebar width proportionally
    int sidebarW = jlimit((int)Layout::kSidebarMinWidth,
                          (int)Layout::kSidebarMaxWidth,
                          (int)(w * Layout::kSidebarWidthRatio));

    // Draw sidebar background (vector — replaces PNG)
    auto sidebarBounds = bounds.removeFromLeft(sidebarW);
    g.setColour(juce::Colour(58, 61, 69));
    g.fillRect(sidebarBounds);

    // Right-edge border line
    g.setColour(juce::Colour(40, 40, 42));
    g.fillRect(sidebarBounds.getRight() - (int)Layout::kSidebarBorderWidth,
               sidebarBounds.getY(),
               (int)Layout::kSidebarBorderWidth,
               sidebarBounds.getHeight());

    // Content area background
    g.setColour(juce::Colour(45, 44, 44));
    g.fillRect(bounds);
}

void MainViewComponent::paintOverChildren(juce::Graphics& g)
{
    auto totalBounds = getLocalBounds();
    const int w = totalBounds.getWidth();
    const int h = totalBounds.getHeight();

    int sidebarW = jlimit((int)Layout::kSidebarMinWidth,
                          (int)Layout::kSidebarMaxWidth,
                          (int)(w * Layout::kSidebarWidthRatio));

    int logoH = (int)(h * Layout::kLogoHeightRatio);

    // Draw logo in the logo area
    auto logoBounds = juce::Rectangle<int>(0, 0, sidebarW, logoH).reduced(10, 10);
    g.setColour(juce::Colours::white);
    g.drawImageWithin(imageLogoButton,
                      logoBounds.getX(),
                      logoBounds.getY(),
                      logoBounds.getWidth(),
                      logoBounds.getHeight(),
                      juce::RectanglePlacement::centred);

    // Version label at bottom of sidebar
    float labelH = Layout::kVersionLabelHeight;
    auto versionBounds = juce::Rectangle<int>(0, h - (int)labelH, sidebarW, (int)labelH);
    g.setColour(juce::Colours::white.withAlpha(0.7f));
    g.setFont(12.0f);
    g.drawText(labelProjectVersion.getText(), versionBounds, juce::Justification::centredLeft);
}

void MainViewComponent::resized()
{
    auto bounds = getLocalBounds();
    const int w = bounds.getWidth();
    const int h = bounds.getHeight();

    // Sidebar width — proportional with clamp
    int sidebarW = jlimit((int)Layout::kSidebarMinWidth,
                          (int)Layout::kSidebarMaxWidth,
                          (int)(w * Layout::kSidebarWidthRatio));

    // Logo button at top of sidebar
    int logoH = (int)(h * Layout::kLogoHeightRatio);
    menuButton_ASVPRTool->setBounds(10, 10, sidebarW - 20, logoH - 20);

    // Menu buttons stacked below logo
    int buttonH = (int)(h * Layout::kMenuButtonHeightRatio);
    int buttonTopY = (int)(h * Layout::kMenuButtonTopRatio);
    for (auto i = 0; i < (int)menuButtons.size(); i++) {
        menuButtons[i]->setBounds(0, buttonTopY + (buttonH * i), sidebarW, buttonH);
    }

    // Content area fills remaining space to the right of the sidebar
    auto contentBounds = bounds.withTrimmedLeft(sidebarW);
    mainContainerComponent.setBounds(contentBounds);

    // All menu views fill the content area
    for (auto& view : menuViews) {
        view->setBounds(0, 0, contentBounds.getWidth(), contentBounds.getHeight());
    }

    // Version label — hidden, we paint it ourselves in paintOverChildren
    labelProjectVersion.setBounds(0, 0, 0, 0);
}

void MainViewComponent::buttonClicked (Button* button)
{

    if (button == menuButton_ASVPRTool.get())
    {
        // call website or something...
    }
    currentSelectedButton->setToggleState(false, dontSendNotification);

    // close popups of currently open mode
    if (currentMenuItem == MenuItem::ChordPlayer)
    {
        if (auto* chordPlayerComponent = dynamic_cast<ChordPlayerComponent*>(menuViews[(int)MenuItem::ChordPlayer].get()))
            chordPlayerComponent->closePopups();
    }
    if (currentMenuItem == MenuItem::ChordScanner)
    {
        if (auto* chordScannerComponent = dynamic_cast<ChordScannerComponent*>(menuViews[(int)MenuItem::ChordScanner].get()))
            chordScannerComponent->closePopups();
    }
    if (currentMenuItem == MenuItem::FrequencyPlayer)
    {
        if (auto* frequencyPlayerComponent = dynamic_cast<FrequencyPlayerComponent*>(menuViews[(int)MenuItem::FrequencyPlayer].get()))
            frequencyPlayerComponent->closePopups();
    }
    if (currentMenuItem == MenuItem::FrequencyScanner)
    {
        if (auto* frequencyScannerComponent = dynamic_cast<FrequencyScannerComponent*>(menuViews[(int)MenuItem::FrequencyScanner].get()))
            frequencyScannerComponent->closePopups();
    }

    // then processbutton and changing of mode
    //-------------------------------------

    for (auto i= 0; i < (int)MenuItem::NumItems; i++) {
        if (button == menuButtons[i].get()) {
            currentMenuItem = (MenuItem)i;
            switchMenuView();
            if (menuItemToModeMap.find(currentMenuItem) != menuItemToModeMap.end()) {
                projectManager->setMode(menuItemToModeMap.at(currentMenuItem));
            }
        }
    }
}

void MainViewComponent::switchMenuView()
{
    for (auto& view : menuViews) {
        view->setVisible(false);
    }
    if (auto* view = menuViews[(int)currentMenuItem].get()) {
        view->setVisible(true);
    }
    currentSelectedButton = menuButtons[(int)currentMenuItem].get();
    currentSelectedButton->setToggleState(true, dontSendNotification);
}

void MainViewComponent::setScale(float factor)
{
    // Layout is now proportional via getLocalBounds() in resized().
    // Propagation kept only for sub-components not yet converted.
    scaleFactor = jlimit(0.5f, 2.0f, factor);
    for (auto& view : menuViews) {
        view->setScale(scaleFactor);
    }
    lookAndFeel.setScale(scaleFactor);
}
