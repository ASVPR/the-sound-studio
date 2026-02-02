/*
  ==============================================================================

    Containers.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once

#include "ContentContainer.h"

namespace asvpr
{

class Containers
{
public:
    ContentContainer& createContainer(const juce::String& name, bool collapsable)
    {
        containers.emplace_back(std::make_unique<ContentContainer>(name, collapsable));

        return *containers.back();
    }

    ContentContainer * getContainer(const juce::String& name)
    {
        for (auto& container : containers)
        {
            if (container->getName() == name)
            {
                return container.get();
            }
        }

        return nullptr;
    };

    ContentContainer& getContainer(int index)
    {
        return *containers[index];
    }

    void setContainerBounds(int index, const juce::Rectangle<int>& bounds)
    {
        auto& container = getContainer(index);

        auto b = container.isShowingContent() ? bounds : bounds.withHeight(40);

        container.setBounds(b);
    };

    std::optional<juce::Rectangle<int>> setContainerBounds(const juce::String& name,
                                            const juce::Rectangle<int>& bounds)
    {
        if (auto container = getContainer(name))
        {
            auto b = container->isShowingContent() ? bounds : bounds.withHeight(30);

            container->setBounds(b);

            return b;
        }

        return std::nullopt;
    }

    std::vector<std::unique_ptr<ContentContainer>>& getContainers()
    {
        return containers;
    }

private:
    std::vector<std::unique_ptr<ContentContainer>> containers;
};

}
