/*
  ==============================================================================

    PluginBrowser_Component.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/


#pragma once
#ifndef PLUGINBROWSER_COMPONENT_H_INCLUDED
#define PLUGINBROWSER_COMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include <memory>

//==============================================================================
/*
*/
class PluginBrowser_Component : public ListBoxModel, public Component
{
    
    
public:
    
    PluginBrowser_Component(KnownPluginList &list, AudioPluginFormatManager &formatManager);
    ~PluginBrowser_Component();
    
    // component
    void paint (Graphics& g);
    void resized();
    
    //listboxmodel
    virtual int getNumRows();
    virtual void paintListBoxItem(int rowNumber, Graphics &g, int width, int height, bool rowIsSelected);
    virtual var getDragSourceDescription(const SparseSet<int> &currentlySelectedRows);
    
    std::unique_ptr<ListBox> listBox;
    
    KnownPluginList &list;
    AudioPluginFormatManager &formatManager;
 
};

#endif  // PLUGINBROWSER_COMPONENT_H_INCLUDED
