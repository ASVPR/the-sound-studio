#include "FeedbackInputOutputComponent.h"

FeedbackInputOutputComponent::FeedbackInputOutputComponent(ProjectManager &projectManager, FeedbackModuleProcessor& fmp, Component* parent) :
    projectManager(projectManager),
    processor(fmp),
    parent(parent)
{
    createContainers();
    addAndMakeVisible(inputHolder);
    addAndMakeVisible(outputHolder);

    inputHolder.addChangeListener(this);
    outputHolder.addChangeListener(this);
}
FeedbackInputOutputComponent::~FeedbackInputOutputComponent()
{
    inputHolder.removeChangeListener(this);
    outputHolder.removeChangeListener(this);
}

void FeedbackInputOutputComponent::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop(10);
    bounds.removeFromLeft(15);
    bounds.removeFromRight(15);
    inputHolder.setBounds(bounds.removeFromTop(inputHolder.getContainerHeight()));
    bounds.removeFromTop(10);
    outputHolder.setBounds(bounds.removeFromTop(outputHolder.getContainerHeight()));
}

void FeedbackInputOutputComponent::changeListenerCallback(ChangeBroadcaster *source)
{
    if (parent)
        parent->resized();
}

int FeedbackInputOutputComponent::getContainerHeight() const
{
    return 20 + inputHolder.getContainerHeight() + 10 + outputHolder.getContainerHeight() + 40;
}

void FeedbackInputOutputComponent::setScale(float factor)
{

}

void FeedbackInputOutputComponent::createContainers()
{
    for (int i = 0; i < processor.getInputNames().size(); i++) {
        auto& channelHeader = channelHeaders.emplace_back(std::make_unique<ChannelHeader>(
            projectManager,
            processor.getFundamentalFrequencyProcessor(i),
            processor.getInputNames().getReference(i),
            parent));
        auto& body = inputChannelBodies.emplace_back(std::make_unique<InputChannelBody>(projectManager, i));
        auto& container = foldableContainers.emplace_back(std::make_unique<FoldableContainer>(*channelHeader, *body));
        inputHolder.addContainer(container.get());
    }

    for (int i = 0; i < processor.getOutputNames().size(); i++) {
        auto& channelHeader = channelHeaders.emplace_back(std::make_unique<ChannelHeader>(
            projectManager,
            processor.getFundamentalFrequencyProcessor(i + processor.getInputNames().size()),
            processor.getOutputNames().getReference(i),
            parent));
        auto& body = outputChannelBodies.emplace_back(std::make_unique<OutputChannelBody>(projectManager, i, parent));
        auto& container = foldableContainers.emplace_back(std::make_unique<FoldableContainer>(*channelHeader, *body));
        outputHolder.addContainer(container.get());
    }
}
