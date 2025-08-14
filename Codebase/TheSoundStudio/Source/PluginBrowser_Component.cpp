/*
  ==============================================================================

    PluginBrowser_Component.cpp
    Created: 26 Aug 2015 1:46:44am
    Author:  Gary Jones

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginBrowser_Component.h"

//==============================================================================
PluginBrowser_Component::PluginBrowser_Component(KnownPluginList &list, AudioPluginFormatManager &formatManager) : formatManager(formatManager), list(list)
{
    addAndMakeVisible(listBox = new ListBox("listbox"));
    
    listBox->setModel(this);
    listBox->updateContent();
    
    setSize(210, 600);
}

PluginBrowser_Component::~PluginBrowser_Component()
{
    
}

void PluginBrowser_Component::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]
    
    g.fillAll (Colours::white);
    
    g.setColour (Colours::black);
    
    
    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void PluginBrowser_Component::resized()
{
    listBox->setBounds (0, 0, 210, 600);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

int PluginBrowser_Component::getNumRows()
{
    return list.getNumTypes() + list.getBlacklistedFiles().size();
    
}

void PluginBrowser_Component::paintListBoxItem(int rowNumber, Graphics &g, int width, int height, bool rowIsSelected)
{
    String text;
    bool isBlacklisted = rowNumber >= list.getNumTypes();
    
    if (isBlacklisted)
    {
 
            text = list.getBlacklistedFiles() [rowNumber - list.getNumTypes()];

    }
    else if (const PluginDescription* const desc = list.getType (rowNumber))
    {
        text = desc->name;
      //  text = desc->pluginFormatName;
      //  text = desc->category.isNotEmpty() ? desc->category : "-";
      //  text = desc->manufacturerName;
     //   text = getPluginDescription (*desc);

    }
    
    if (text.isNotEmpty())
    {
        g.setColour (Colours::black);
     //   g.setColour (isBlacklisted ? Colours::red
          //           : columnId == nameCol ? Colours::black
           //          : Colours::grey);
        g.setFont (Font (height * 0.7f, Font::bold));
        g.drawFittedText (text, 4, 0, width - 6, height, Justification::centredLeft, 1, 0.9f);
    }
}

var PluginBrowser_Component::getDragSourceDescription(const SparseSet<int> &currentlySelectedRows)
{
    // for our drag description, we'll just make a comma-separated list of the selected row
    // numbers - this will be picked up by the drag target and displayed in its box.
    String text;
    
   // for (int i = 0; i < currentlySelectedRows.size(); i++) {
       const PluginDescription* const desc = list.getType (currentlySelectedRows[0]);
       // text = desc->name;
  //  }
    int index = currentlySelectedRows[0];
    
    return index;
    
}