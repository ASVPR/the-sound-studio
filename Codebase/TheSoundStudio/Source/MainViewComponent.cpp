/*
  ==============================================================================

    MainViewComponent.cpp
    Created: 13 Mar 2019 9:57:24pm
    Author:  Gary Jones

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "MainViewComponent.h"

//==============================================================================
MainViewComponent::MainViewComponent(ProjectManager * pm)
{
    projectManager = pm;

    // Image Cache
    // Use the TSS icon for the application logo
    Image tssLogo = ImageCache::getFromMemory(BinaryData::icon_128_png, BinaryData::icon_128_pngSize);
    imageLogoButton = tssLogo; // Use TSS icon instead of LogoButton_Normal
    imageMenuButtonNormal = ImageCache::getFromMemory(BinaryData::Sidebar_Button_Normal_png, BinaryData::Sidebar_Button_Normal_pngSize);
    imageMenuButtonSelected = ImageCache::getFromMemory(BinaryData::Sidebar_ButtonHighlighted_png, BinaryData::Sidebar_ButtonHighlighted_pngSize);
    imageSidebar = ImageCache::getFromMemory(BinaryData::SidebarNew_png, BinaryData::SidebarNew_pngSize);


    // sidebar component
    imageComponent_Sidebar = std::make_unique<ImageComponent>();
    imageComponent_Sidebar->setImage(imageSidebar);
    addAndMakeVisible(*imageComponent_Sidebar);

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

    auto* feedbackModule = projectManager->feedbackModuleProcessor.get();
    menuViews[(int)MenuItem::Feedback] = std::make_unique<FeedbackModuleComponent>(*feedbackModule);
    mainContainerComponent.addAndMakeVisible(*menuViews[(int)MenuItem::Feedback]);

    menuViews[(int)MenuItem::FeedbackHardware] = std::make_unique<FeedbackModuleComponent>(*feedbackModule);
    mainContainerComponent.addAndMakeVisible(*menuViews[(int)MenuItem::FeedbackHardware]);

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
    g.fillAll ({45,44,44});
}

void MainViewComponent::paintOverChildren(juce::Graphics& g)
{
    auto menuBounds = imageComponent_Sidebar->getBounds();

    menuBounds.removeFromRight(2);
    auto bottomBounds = juce::Rectangle<int>(0, 720, menuBounds.getWidth(), getHeight() - 720);
    auto logoBounds = menuBounds.removeFromTop(65);

    g.setColour({58, 61, 69});

    g.fillRect(logoBounds);
    g.fillRect(bottomBounds);
    g.setColour(juce::Colours::white);

    logoBounds.reduce(10, 10);
    g.drawImageWithin(imageLogoButton,
                      logoBounds.getX(),
                      logoBounds.getY(),
                      logoBounds.getWidth(),
                      logoBounds.getHeight(),
                      juce::RectanglePlacement::centred);

    auto local = getLocalBounds();

    g.drawText(labelProjectVersion.getText(),
               local.removeFromBottom(labelProjectVersion.getHeight()),
               juce::Justification::centredLeft);

//    logoBounds.reduced(<#int delta#>)
}

void MainViewComponent::resized()
{
    printf("MainView Resize called");
    
    imageComponent_Sidebar              ->setBounds(0*scaleFactor, 0*scaleFactor, 358*scaleFactor, 1440*scaleFactor);
    menuButton_ASVPRTool                ->setBounds(18*scaleFactor, 36*scaleFactor, logoButtonWidth*scaleFactor, logoButtonHeight*scaleFactor);

    for (auto i = 0; i < menuButtons.size(); i++) {
        menuButtons[i]->setBounds(0*scaleFactor, (buttonTopMargin + (buttonHeight * i))*scaleFactor, buttonWidth*scaleFactor, buttonHeight*scaleFactor);
    }
    
    mainContainerComponent.setBounds(menuSideBarWidth*scaleFactor, 0, containerWidth*scaleFactor, containerHeight + 200 *scaleFactor);
    for (auto& view : menuViews) {
        view->setBounds(0, 0, containerWidth*scaleFactor, containerHeight*scaleFactor);
    }

    float labelH = 40 * scaleFactor;
    labelProjectVersion.setBounds(0* scaleFactor, getHeight() - labelH, buttonWidth * scaleFactor, labelH);
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
    // Override auto-calculated scale if needed
    scaleFactor = jlimit(0.5f, 2.0f, factor);
    for (auto& view : menuViews) {
        view->setScale(scaleFactor);
    }
    lookAndFeel.setScale(scaleFactor);
    resized();  // Re-layout with new scale
}
