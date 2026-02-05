/*
  ==============================================================================

    FeedbackInputOutputComponent.h
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ProjectManager.h"
#include "Support/FoldableContainer.h"
#include "Support/ChannelHeader.h"
#include "Support/InputChannelBody.h"
#include "Support/OutputChannelBody.h"
#include "Support/Holder.h"

class FeedbackInputOutputComponent : public Component, ChangeListener {
public:
    FeedbackInputOutputComponent(ProjectManager& pm, FeedbackModuleProcessor& fmp, Component* parent);
    ~FeedbackInputOutputComponent() override;
    void resized() override;
    void changeListenerCallback(ChangeBroadcaster *source) override;
    int getContainerHeight() const;
    void setScale(float factor);
    void createContainers();

private:
    using FoldableContainer = Feedback::FoldableContainer;
    using ChannelHeader = Feedback::ChannelHeader;
    using InputChannelBody = Feedback::InputChannelBody;
    using OutputChannelBody = Feedback::OutputChannelBody;
    using Holder = Feedback::Holder;

    ProjectManager& projectManager;
    FeedbackModuleProcessor& processor;
    Component* parent;
    std::vector<std::unique_ptr<ChannelHeader>> channelHeaders;
    std::vector<std::unique_ptr<InputChannelBody>> inputChannelBodies;
    std::vector<std::unique_ptr<OutputChannelBody>> outputChannelBodies;
    std::vector<std::unique_ptr<FoldableContainer>> foldableContainers;
    Holder inputHolder{ "Inputs" }, outputHolder{ "Outputs" };
};
