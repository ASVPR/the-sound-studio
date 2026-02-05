/*
  ==============================================================================

    ComboBox.h
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace asvpr
{
class ComboBox : public juce::ComboBox
{
public:
    ComboBox(const juce::String& name) : juce::ComboBox(name)
    {

    };
    
    void showLabel(bool show)
    {
        showingLabel = show;
    }

    bool isShowingLabel() const
    {
        return showingLabel;
    }

private:
    bool showingLabel { false };
};

} /* End of asvpr */



namespace asvpr
{
struct ComboBoxWithAttachment : asvpr::ComboBox
{
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    ComboBoxWithAttachment(const char * name) : asvpr::ComboBox(name)
    {

    }

    void createAttachment(juce::AudioProcessorValueTreeState& stateToUse, const juce::String& parameterId)
    {
        attachment = std::make_unique<ComboBoxAttachment>(stateToUse, parameterId, *this);
    }
    
    std::unique_ptr<ComboBoxAttachment> attachment;
};



} /* End of asvpr */
