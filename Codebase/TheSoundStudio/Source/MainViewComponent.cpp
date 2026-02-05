/*
  ==============================================================================

    MainViewComponent.cpp
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "MainViewComponent.h"
#include "ResponsiveUIHelper.h"
#include "UI/DesignSystem.h"

class SidebarMenuButton final : public Button
{
public:
    explicit SidebarMenuButton(const String& labelText)
        : Button(labelText),
          label(labelText)
    {
        setClickingTogglesState(true);
        setTriggeredOnMouseDown(true);
        updateFont();
    }

    void setScale(float newScale)
    {
        scaleFactor = newScale;
        updateFont();
        repaint();
    }

private:
    void updateFont()
    {
        const float baseSize = TSS::Design::Typography::h4;
        const float scaledSize = ResponsiveUIHelper::scaleFontSize(baseSize, scaleFactor);
        const float minSize = TSS::Design::Usability::sidebarMinFont;
        const float fontSize = jmax(minSize, scaledSize);
        font = ProjectManager::getAssistantFont(ProjectManager::FontType::SemiBold).withHeight(fontSize);
    }

    void paintButton(Graphics& g, bool isHighlighted, bool isDown) override
    {
        auto bounds = getLocalBounds();

        auto background = TSS::Design::Colors::Dark::surface;
        auto hover = TSS::Design::Colors::Dark::surfaceHover;

        if (getToggleState())
        {
            g.setColour(hover);
            g.fillRect(bounds);

            const int accentMinWidth = roundToInt(TSS::Design::Spacing::xxxs * scaleFactor);
            const int accentWidth = jmax(accentMinWidth, roundToInt(TSS::Design::Spacing::xxs * scaleFactor));
            g.setColour(TSS::Design::Colors::primary);
            g.fillRect(bounds.removeFromLeft(accentWidth));
        }
        else if (isHighlighted || isDown)
        {
            g.setColour(hover);
            g.fillRect(bounds);
        }
        else
        {
            g.setColour(background);
            g.fillRect(bounds);
        }

        const int padding = roundToInt(TSS::Design::Spacing::sm * scaleFactor);
        auto textBounds = getLocalBounds().reduced(padding, 0);

        g.setColour(getToggleState() ? TSS::Design::Colors::Dark::text
                                     : TSS::Design::Colors::Dark::textSecondary);
        g.setFont(font);
        g.drawText(label, textBounds, Justification::centredLeft, true);

        g.setColour(TSS::Design::Colors::Dark::divider);
        g.drawLine((float)getLocalBounds().getX(),
                   (float)getLocalBounds().getBottom(),
                   (float)getLocalBounds().getRight(),
                   (float)getLocalBounds().getBottom());
    }

    String label;
    float scaleFactor = 1.0f;
    Font font;
};

const std::array<MainViewComponent::MenuEntry,
    static_cast<size_t>(MainViewComponent::MenuItem::NumItems)> MainViewComponent::menuEntries = {{
    {MenuItem::ChordPlayer, "Chord Player", true, AUDIO_MODE::MODE_CHORD_PLAYER},
    {MenuItem::ChordScanner, "Chord Scanner", true, AUDIO_MODE::MODE_CHORD_SCANNER},
    {MenuItem::FundamentalFrequency, "Fundamental Frequency", true, AUDIO_MODE::MODE_FUNDAMENTAL_FREQUENCY},
    {MenuItem::FrequencyPlayer, "Frequency Player", true, AUDIO_MODE::MODE_FREQUENCY_PLAYER},
    {MenuItem::FrequencyScanner, "Frequency Scanner", true, AUDIO_MODE::MODE_FREQUENCY_SCANNER},
    {MenuItem::RealTimeAnalysis, "Realtime Analysis", true, AUDIO_MODE::MODE_REALTIME_ANALYSIS},
    {MenuItem::LissajousCurves, "Lissajous Curves", true, AUDIO_MODE::MODE_LISSAJOUS_CURVES},
    {MenuItem::Feedback, "Feedback", true, AUDIO_MODE::MODE_FEEDBACK_MODULE},
    {MenuItem::FrequencyLight, "Frequency To Light", true, AUDIO_MODE::MODE_FREQUENCY_TO_LIGHT},
    {MenuItem::Settings, "Settings", false, AUDIO_MODE::MODE_NONE}
}};

//==============================================================================
MainViewComponent::MainViewComponent(ProjectManager * pm)
{
    projectManager = pm;

    // Image Cache
    // Use the TSS icon for the application logo
    Image tssLogo = ImageCache::getFromMemory(BinaryData::icon_128_png, BinaryData::icon_128_pngSize);

    // Buttons - Use TSS logo for the main application button
    menuButton_ASVPRTool = std::make_unique<ImageButton>();
    menuButton_ASVPRTool->setTriggeredOnMouseDown(true);
    menuButton_ASVPRTool->setImages (false, true, true,
                            tssLogo, 0.999f, Colour (0x00000000),
                            Image(), 1.000f, Colour (0x00000000),
                            tssLogo, 1.000f, Colour (0x00000000));
    menuButton_ASVPRTool->addListener(this);
    addAndMakeVisible(*menuButton_ASVPRTool);

    for (const auto& entry : menuEntries)
    {
        auto& menuButton = menuButtons.emplace_back(std::make_unique<SidebarMenuButton>(entry.label));
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
    labelProjectVersion.setColour(Label::textColourId, TSS::Design::Colors::Dark::textSecondary);
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
    g.fillAll(TSS::Design::Colors::Dark::background);

    if (!sidebarBounds.isEmpty())
    {
        g.setColour(TSS::Design::Colors::Dark::surface);
        g.fillRect(sidebarBounds);

        g.setColour(TSS::Design::Colors::Dark::divider);
        const float borderWidth = TSS::Design::Layout::kSidebarBorderWidth * scaleFactor;
        g.drawLine((float)sidebarBounds.getRight(), 0.0f,
                   (float)sidebarBounds.getRight(), (float)getHeight(),
                   borderWidth);
    }
}

void MainViewComponent::resized()
{
    updateLayoutForWindowSize();

    mainContainerComponent.setBounds(contentBounds);
    for (auto& view : menuViews)
    {
        view->setBounds(0, 0, contentBounds.getWidth(), contentBounds.getHeight());
    }
}

void MainViewComponent::updateLayoutForWindowSize()
{
    const auto totalBounds = getLocalBounds();

    lastWindowWidth = totalBounds.getWidth();
    lastWindowHeight = totalBounds.getHeight();

    const int desiredSidebarWidth = roundToInt(totalBounds.getWidth() * TSS::Design::Layout::kSidebarWidthRatio);
    const int sidebarWidth = jlimit((int)TSS::Design::Layout::kSidebarMinWidth,
                                    (int)TSS::Design::Layout::kSidebarMaxWidth,
                                    desiredSidebarWidth);

    sidebarBounds = totalBounds.withWidth(sidebarWidth);
    contentBounds = totalBounds.withTrimmedLeft(sidebarWidth);

    // Increase logo height ratio locally for a larger icon without affecting global constants
    const float localLogoHeightRatio = TSS::Design::Layout::kLogoHeightRatio * 2.2f;
    const int logoHeight = roundToInt((float)totalBounds.getHeight() * localLogoHeightRatio);
    
    // Adjust menu top ratio to account for larger logo
    const float localMenuTopRatio = TSS::Design::Layout::kMenuButtonTopRatio * 1.5f;
    const int menuTopRatio = roundToInt((float)totalBounds.getHeight() * localMenuTopRatio);
    
    int menuButtonHeight = roundToInt((float)totalBounds.getHeight() * TSS::Design::Layout::kMenuButtonHeightRatio);
    const int minMenuButtonHeight = TSS::Design::Usability::sidebarButtonMinHeight;
    menuButtonHeight = jmax(menuButtonHeight, minMenuButtonHeight);

    const int spacing = ResponsiveUIHelper::getResponsiveSpacing(scaleFactor, TSS::Design::Spacing::xs);
    const int footerHeight = jmax(roundToInt(TSS::Design::Spacing::md * scaleFactor),
                                  roundToInt(TSS::Design::Layout::kVersionLabelHeight * scaleFactor));

    const int menuCount = (int)menuButtons.size();
    const int menuTop = jmax(menuTopRatio, logoHeight + spacing);
    const int maxHeightForButtons = sidebarBounds.getHeight() - menuTop - footerHeight -
        (menuCount > 0 ? (menuCount - 1) * spacing : 0);
    if (menuCount > 0)
    {
        menuButtonHeight = jmin(menuButtonHeight, maxHeightForButtons / menuCount);
    }

    const int logoPadding = ResponsiveUIHelper::getResponsiveMargin(scaleFactor, TSS::Design::Spacing::sm);
    if (menuButton_ASVPRTool)
    {
        auto logoBounds = sidebarBounds.withHeight(logoHeight).reduced(logoPadding);
        menuButton_ASVPRTool->setBounds(logoBounds);
    }

    int currentY = sidebarBounds.getY() + menuTop;
    const int buttonWidth = sidebarBounds.getWidth();

    for (auto& button : menuButtons)
    {
        button->setBounds(sidebarBounds.getX(), currentY, buttonWidth, menuButtonHeight);
        button->setScale(scaleFactor);
        currentY += menuButtonHeight + spacing;
    }

    const float captionScaled = ResponsiveUIHelper::scaleFontSize(TSS::Design::Typography::caption, scaleFactor);
    const float captionSize = jmax(TSS::Design::Typography::caption, captionScaled);
    labelProjectVersion.setFont(ProjectManager::getAssistantFont(ProjectManager::FontType::Light)
                                    .withHeight(captionSize));
    labelProjectVersion.setBounds(sidebarBounds.getX() + logoPadding,
                                  sidebarBounds.getBottom() - footerHeight - logoPadding / 2,
                                  sidebarBounds.getWidth() - logoPadding * 2,
                                  footerHeight);
}

void MainViewComponent::buttonClicked (Button* button)
{
    
    if (button == menuButton_ASVPRTool.get())
    {
        // call website or something...
    }
    if (currentSelectedButton)
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

    for (auto i = 0; i < menuButtons.size(); i++)
    {
        if (button == menuButtons[i].get())
        {
            currentMenuItem = menuEntries[i].item;
            switchMenuView();
            if (menuEntries[i].hasMode)
                projectManager->setMode(menuEntries[i].mode);
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
    if (!menuButtons.empty())
    {
        currentSelectedButton = menuButtons[(int)currentMenuItem].get();
        if (currentSelectedButton)
            currentSelectedButton->setToggleState(true, dontSendNotification);
    }
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
