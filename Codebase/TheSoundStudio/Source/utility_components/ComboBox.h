/*
  ==============================================================================

    ComboBox.h
    Created: 3 Dec 2021 2:15:39am
    Author:  Javier Andoni Aizpuru Rodríguez

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
