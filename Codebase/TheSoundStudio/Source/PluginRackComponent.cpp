/*
  ==============================================================================

    PluginRackComponent.cpp

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PluginRackComponent.h"
#include "UI/PluginMenuBuilder.h"

//==============================================================================


PluginRackComponent::PluginRackComponent(ProjectManager * pm)
{
    projectManager = pm;

    
    Typeface::Ptr AssistantBold     = Typeface::createSystemTypefaceFor(BinaryData::AssistantBold_ttf, BinaryData::AssistantBold_ttfSize);
    Typeface::Ptr AssistantSemiBold   = Typeface::createSystemTypefaceFor(BinaryData::AssistantSemiBold_ttf, BinaryData::AssistantSemiBold_ttfSize);
    Font fontBold(AssistantBold);
    Font fontNormal(AssistantSemiBold);
    
    fontNormal.setHeight(33);
    fontBold.setHeight(33);
    
    
    imageAddPluginIcon      = ImageCache::getFromMemory(BinaryData::ShortcutAdd2x_png, BinaryData::ShortcutAdd2x_pngSize);
    removePluginIcon        = ImageCache::getFromMemory(BinaryData::CloseButton2x_png, BinaryData::CloseButton2x_pngSize);
    openWindowPluginIcon    = ImageCache::getFromMemory(BinaryData::Button_SpectrumOpen_png, BinaryData::Button_SpectrumOpen_pngSize);
    
    buttonAddPlugin = new ImageButton();
    buttonAddPlugin->addListener(this);
    buttonAddPlugin->setTriggeredOnMouseDown(true);
    buttonAddPlugin->setImages (false, true, true,
                                   imageAddPluginIcon, 0.999f, Colour (0x00000000),
                                   Image(), 1.000f, Colour (0x00000000),
                                   imageAddPluginIcon, 0.999f, Colour (0x00000000));
    addAndMakeVisible(buttonAddPlugin);
    
    buttonRemovePlugin = new ImageButton();
    buttonRemovePlugin->addListener(this);
    buttonRemovePlugin->setTriggeredOnMouseDown(true);
    buttonRemovePlugin->setImages (false, true, true,
                                   removePluginIcon, 0.999f, Colour (0x00000000),
                                   Image(), 1.000f, Colour (0x00000000),
                                   removePluginIcon, 0.999f, Colour (0x00000000));
    addAndMakeVisible(buttonRemovePlugin);
    
    buttonOpenPlugin = new ImageButton();
    buttonOpenPlugin->addListener(this);
    buttonOpenPlugin->setTriggeredOnMouseDown(true);
    buttonOpenPlugin->setImages (false, true, true,
                                   openWindowPluginIcon, 0.999f, Colour (0x00000000),
                                   Image(), 1.000f, Colour (0x00000000),
                                   openWindowPluginIcon, 0.999f, Colour (0x00000000));
    addAndMakeVisible(buttonOpenPlugin);
    
    comboBoxPluginSelector = new ComboBox();
    comboBoxPluginSelector->setLookAndFeel(&lookAndFeel);
    comboBoxPluginSelector->addListener(this);
    
    TSS::UI::populatePluginMenu(*comboBoxPluginSelector->getRootMenu(),
                                *projectManager->knownPluginList, &lookAndFeel);
    addAndMakeVisible(comboBoxPluginSelector);
    
    
    scanPluginsButton = std::make_unique<TextButton>("Scan for new plugins");
    scanPluginsButton->addListener(this);
    scanPluginsButton->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(scanPluginsButton.get());
    
    
}

void PluginRackComponent::resized()
{
    int buttonWidth = 200;
    int buttonHeight = 60;
    int centreX = ((getWidth() / 2) - (buttonWidth / 2)) * scaleFactor;
    
//    currentPluginLabel  ->setBounds(centreX, 200, buttonWidth, buttonHeight);
//    loadPluginButton    ->setBounds(centreX, 300, buttonWidth, buttonHeight);
//
//    openPluginButton    ->setBounds(centreX, 500, buttonWidth, buttonHeight);
    
    int y = 300;
    comboBoxPluginSelector->setBounds(centreX* scaleFactor, y* scaleFactor, 250* scaleFactor, 41* scaleFactor); //(242, 497, 125, 41);
    
    buttonRemovePlugin ->setBounds((centreX + 250 + 5)* scaleFactor, y* scaleFactor, 41* scaleFactor, 41* scaleFactor);
    buttonOpenPlugin    ->setBounds((centreX + 250 + 50 + 5)* scaleFactor, y* scaleFactor, 41* scaleFactor, 41* scaleFactor);
    scanPluginsButton   ->setBounds((centreX + 250 + 50 + 50 + 5 )* scaleFactor, y* scaleFactor, 200* scaleFactor, 41* scaleFactor);
    
}

void PluginRackComponent::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
//    if (comboBoxThatHasChanged == comboBoxPluginSelector)
//    {
//        PluginWindow::closeAllCurrentlyOpenWindows();
//
//        bool didLoad = projectManager->pluginAssignProcessor->loadPluginRef(comboBoxPluginSelector->getSelectedId()-1);
//    }

}

PluginRackComponent::~PluginRackComponent() { }

// 1. keep menu in memory for effective access
// 2. return Plugin Name string upon loadPluginRef

void PluginRackComponent::buttonClicked (Button*button)
{
    if (button == buttonRemovePlugin)
    {
//        PluginAssignProcessor * f = projectManager->pluginAssignProcessor;
        PluginWindow::closeAllCurrentlyOpenWindows();
        
//        if (f->clearPlugin()) {
//            comboBoxPluginSelector->setSelectedId(-1);
//        }
    }
    else if (button == buttonOpenPlugin)
    {
//        PluginAssignProcessor * f = projectManager->pluginAssignProcessor;
//        if (f->hasPlugin)
//        {
//            AudioPluginInstance * plugin = f->instance.get();
//            PluginWindow* const w = PluginWindow::openWindowFor (plugin, false);
//            w->toFront (true);
//        }
//        else
//        {
//            // could do popup alert msg
//        }
    }
    else if (button == scanPluginsButton.get())
    {
        rescanPlugins();
        
        TSS::UI::populatePluginMenu(*comboBoxPluginSelector->getRootMenu(),
                                    *projectManager->knownPluginList, &lookAndFeel);

        
    }
}

void PluginRackComponent::rescanPlugins()
{
    // only scan for audio units

    // only scan for FX , not instruments


    for (int i = 0; i < projectManager->pluginFormatManager.getNumFormats(); i++)
    {
        AudioPluginFormat* const format = projectManager->pluginFormatManager.getFormat(i);
        
        PluginDirectoryScanner * scanner = new PluginDirectoryScanner(*projectManager->knownPluginList, *format, format->getDefaultLocationsToSearch(), true, projectManager->deadMansPedalFile);
        String plugName;
        
        // Skip plugin scanning popup to prevent blocking main window visibility
        // Automatically proceed with scanning without user interaction
        bool shouldScan = true;
        
        if (shouldScan)
        {
            while (scanner->scanNextFile(true, plugName))
            { }
        }
    }
}

void PluginRackComponent::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (Colours::white);
    g.setFont (14.0f);
    g.drawText ("PluginRackComponent", getLocalBounds(),
                Justification::centred, true);   // draw some placeholder text
}


