/*
  ==============================================================================

    PluginRackComponent.h
    Created: 14 Feb 2020 9:06:04am
    Author:  Gary Jones

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ProjectManager.h"
#include "PluginWindow.h"
#include "CustomLookAndFeel.h"
#include <memory>

//==============================================================================
/*
*/
class PluginRackComponent    : public Component, public Button::Listener, public ComboBox::Listener
{
public:
    PluginRackComponent(ProjectManager * pm);

    ~PluginRackComponent();

    void buttonClicked (Button*button)override;
    
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged)override;
    
    void rescanPlugins();

    void paint (Graphics& g) override;

    void resized() override;
    
    float scaleFactor = 0.5;
    void setScale(float factor)
    {
        scaleFactor = factor;
        
        lookAndFeel.setScale(scaleFactor);
    }

private:
    
    ProjectManager * projectManager;
    
    std::unique_ptr<TextButton>   scanPluginsButton;
    
    CustomLookAndFeel lookAndFeel;
    
    Image imageAddPluginIcon;
    Image removePluginIcon;
    Image openWindowPluginIcon;
    
    ImageButton * buttonAddPlugin;
    ImageButton * buttonRemovePlugin;
    ImageButton * buttonOpenPlugin;
    
    ComboBox * comboBoxPluginSelector;
    

    
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginRackComponent)
};
