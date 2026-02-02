/*
  ==============================================================================

    Holder.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace Feedback {
    class Holder : public Component, ChangeListener, public ChangeBroadcaster {
    public:
        Holder(const String& name) : name(name) {}
        ~Holder() {
            for (auto* container : containers) {
                if (container) container->removeChangeListener(this);
            }
        }

        void addContainer(FoldableContainer* container) {
            addAndMakeVisible(container);
            containers.push_back(container);
            container->addChangeListener(this);
        }
        void paint(Graphics& g) override {
            g.setColour(Colour::fromString("ff464b55"));
            g.fillRoundedRectangle(getLocalBounds().toFloat(), 8.f);

            g.setColour(Colours::white);
            g.setFont(Font{15.f}.boldened());
            g.drawFittedText(name, getLocalBounds().removeFromTop(40).translated(10, 0), Justification::centredLeft, 1);
        }
        void resized() override  {
            auto bounds = getLocalBounds();
            bounds.removeFromTop(40);
            for (auto* container : containers) {
                container->setBounds(bounds.removeFromTop(container->getContainerHeight()));
            }
        }

        int getContainerHeight() const {
            int height = 40;
            for (auto* container : containers) {
                height += container->getContainerHeight();
            }
            return height + 20;
        }
        void changeListenerCallback(ChangeBroadcaster *source) override
        {
            sendChangeMessage();
        }

    private:
        String name;
        std::vector<FoldableContainer*> containers;
    };
}
