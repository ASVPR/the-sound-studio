/*
  ==============================================================================

    PluginMenuBuilder.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once

#include "../../JuceLibraryCode/JuceHeader.h"

namespace TSS { namespace UI {

inline void populatePluginMenu(juce::PopupMenu& rootMenu,
                               const juce::KnownPluginList& pluginList,
                               juce::LookAndFeel* laf = nullptr)
{
    rootMenu.clear();

    juce::PopupMenu auMenu;
    juce::PopupMenu vstMenu;
    juce::PopupMenu vst3Menu;

    if (laf != nullptr)
    {
        auMenu.setLookAndFeel(laf);
        vstMenu.setLookAndFeel(laf);
        vst3Menu.setLookAndFeel(laf);
    }

    for (int i = 0; i < pluginList.getNumTypes(); i++)
    {
        auto* desc = pluginList.getType(i);

        if (desc != nullptr && !desc->isInstrument)
        {
            if (desc->pluginFormatName == "AudioUnit")
                auMenu.addItem(i + 1, desc->name);
            else if (desc->pluginFormatName == "VST")
                vstMenu.addItem(i + 1, desc->name);
            else if (desc->pluginFormatName == "VST3")
                vst3Menu.addItem(i + 1, desc->name);
        }
    }

    rootMenu.addSubMenu("AU", auMenu);
    rootMenu.addSubMenu("VST", vstMenu);
    rootMenu.addSubMenu("VST3", vst3Menu);
}

}} // namespace TSS::UI
