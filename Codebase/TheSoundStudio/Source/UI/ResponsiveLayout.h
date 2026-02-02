/*
  ==============================================================================

    ResponsiveLayout.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DesignSystem.h"

namespace TSS {
namespace UI {

//==============================================================================
/**
    Responsive container that automatically layouts children using flexbox-style rules.
    Supports horizontal/vertical direction, alignment, spacing, and wrapping.
*/
class ResponsiveContainer : public juce::Component
{
public:
    enum class Direction
    {
        Row,            // Left to right
        RowReverse,     // Right to left
        Column,         // Top to bottom
        ColumnReverse   // Bottom to top
    };

    enum class Justify
    {
        Start,          // Items at start
        Center,         // Items centered
        End,            // Items at end
        SpaceBetween,   // Equal space between
        SpaceAround,    // Equal space around
        SpaceEvenly     // Equal space evenly
    };

    enum class Align
    {
        Start,          // Cross-axis start
        Center,         // Cross-axis center
        End,            // Cross-axis end
        Stretch         // Stretch to fill
    };

    ResponsiveContainer()
    {
        setDirection(Direction::Row);
        setJustifyContent(Justify::Start);
        setAlignItems(Align::Start);
        setGap(Design::Spacing::md);
        setPadding(0);
        setWrap(false);
    }

    ~ResponsiveContainer() override = default;

    // Configuration
    void setDirection(Direction dir) { direction = dir; resized(); }
    void setJustifyContent(Justify j) { justify = j; resized(); }
    void setAlignItems(Align a) { align = a; resized(); }
    void setGap(int g) { gap = g; resized(); }
    void setPadding(int p) { padding = juce::BorderSize<int>(p); resized(); }
    void setPadding(int top, int right, int bottom, int left)
    {
        padding = juce::BorderSize<int>(top, left, bottom, right);
        resized();
    }
    void setWrap(bool w) { wrap = w; resized(); }

    // Override resized to perform layout
    void resized() override
    {
        layoutChildren();
    }

    // Add child with flex properties
    void addFlexChild(juce::Component* child, float flexGrow = 0.0f, float flexShrink = 1.0f, int flexBasis = -1)
    {
        FlexItem item;
        item.component = child;
        item.flexGrow = flexGrow;
        item.flexShrink = flexShrink;
        item.flexBasis = flexBasis;
        flexItems.add(item);
        addAndMakeVisible(child);
        resized();
    }

    void removeFlexChild(juce::Component* child)
    {
        for (int i = 0; i < flexItems.size(); ++i)
        {
            if (flexItems[i].component == child)
            {
                flexItems.remove(i);
                removeChildComponent(child);
                resized();
                return;
            }
        }
    }

    void clearChildren()
    {
        flexItems.clear();
        removeAllChildren();
    }

private:
    struct FlexItem
    {
        juce::Component* component = nullptr;
        float flexGrow = 0.0f;
        float flexShrink = 1.0f;
        int flexBasis = -1;  // -1 means auto
    };

    void layoutChildren()
    {
        auto bounds = getLocalBounds().reduced(padding);

        if (flexItems.isEmpty())
            return;

        bool isRow = (direction == Direction::Row || direction == Direction::RowReverse);
        int availableSpace = isRow ? bounds.getWidth() : bounds.getHeight();
        int crossAxisSize = isRow ? bounds.getHeight() : bounds.getWidth();

        // Calculate total gap space
        int totalGap = gap * (flexItems.size() - 1);
        availableSpace -= totalGap;

        // Calculate sizes
        juce::Array<int> sizes;
        int totalFixed = 0;
        float totalGrow = 0.0f;

        for (auto& item : flexItems)
        {
            if (item.flexBasis >= 0)
            {
                sizes.add(item.flexBasis);
                totalFixed += item.flexBasis;
            }
            else
            {
                // Auto: use preferred size
                int preferredSize = isRow
                    ? item.component->getWidth()
                    : item.component->getHeight();
                sizes.add(preferredSize);
                totalFixed += preferredSize;
            }
            totalGrow += item.flexGrow;
        }

        // Distribute remaining space
        int remainingSpace = availableSpace - totalFixed;
        if (remainingSpace > 0 && totalGrow > 0)
        {
            for (int i = 0; i < flexItems.size(); ++i)
            {
                if (flexItems[i].flexGrow > 0)
                {
                    int extra = static_cast<int>(remainingSpace * (flexItems[i].flexGrow / totalGrow));
                    sizes.set(i, sizes[i] + extra);
                }
            }
        }

        // Calculate positions based on justify content
        juce::Array<int> positions;
        int currentPos = 0;

        switch (justify)
        {
            case Justify::Start:
                for (int size : sizes)
                {
                    positions.add(currentPos);
                    currentPos += size + gap;
                }
                break;

            case Justify::Center:
            {
                int totalSize = totalFixed + totalGap;
                int offset = (availableSpace - totalSize) / 2;
                currentPos = offset;
                for (int size : sizes)
                {
                    positions.add(currentPos);
                    currentPos += size + gap;
                }
                break;
            }

            case Justify::End:
            {
                int totalSize = totalFixed + totalGap;
                int offset = availableSpace - totalSize;
                currentPos = offset;
                for (int size : sizes)
                {
                    positions.add(currentPos);
                    currentPos += size + gap;
                }
                break;
            }

            case Justify::SpaceBetween:
            {
                if (flexItems.size() > 1)
                {
                    int spacing = remainingSpace / (flexItems.size() - 1);
                    for (int size : sizes)
                    {
                        positions.add(currentPos);
                        currentPos += size + spacing;
                    }
                }
                else
                {
                    positions.add(0);
                }
                break;
            }

            case Justify::SpaceAround:
            case Justify::SpaceEvenly:
            {
                int spacing = remainingSpace / (flexItems.size() + 1);
                currentPos = spacing;
                for (int size : sizes)
                {
                    positions.add(currentPos);
                    currentPos += size + spacing;
                }
                break;
            }
        }

        // Position components
        for (int i = 0; i < flexItems.size(); ++i)
        {
            auto* comp = flexItems[i].component;
            int mainSize = sizes[i];
            int mainPos = positions[i];

            int crossPos = 0;
            int compCrossSize = crossAxisSize;

            // Handle cross-axis alignment
            switch (align)
            {
                case Align::Start:
                    crossPos = 0;
                    compCrossSize = isRow ? comp->getHeight() : comp->getWidth();
                    break;

                case Align::Center:
                {
                    int preferredCrossSize = isRow ? comp->getHeight() : comp->getWidth();
                    crossPos = (crossAxisSize - preferredCrossSize) / 2;
                    compCrossSize = preferredCrossSize;
                    break;
                }

                case Align::End:
                {
                    int preferredCrossSize = isRow ? comp->getHeight() : comp->getWidth();
                    crossPos = crossAxisSize - preferredCrossSize;
                    compCrossSize = preferredCrossSize;
                    break;
                }

                case Align::Stretch:
                    crossPos = 0;
                    compCrossSize = crossAxisSize;
                    break;
            }

            // Apply bounds based on direction
            juce::Rectangle<int> compBounds;
            if (isRow)
            {
                compBounds = juce::Rectangle<int>(
                    bounds.getX() + mainPos,
                    bounds.getY() + crossPos,
                    mainSize,
                    compCrossSize
                );
            }
            else
            {
                compBounds = juce::Rectangle<int>(
                    bounds.getX() + crossPos,
                    bounds.getY() + mainPos,
                    compCrossSize,
                    mainSize
                );
            }

            comp->setBounds(compBounds);
        }
    }

    Direction direction;
    Justify justify;
    Align align;
    int gap;
    juce::BorderSize<int> padding;
    bool wrap;

    juce::Array<FlexItem> flexItems;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ResponsiveContainer)
};

//==============================================================================
/**
    Grid layout container for complex 2D layouts.
*/
class GridContainer : public juce::Component
{
public:
    GridContainer(int cols = 12, int gap = Design::Spacing::md)
        : columns(cols), gridGap(gap)
    {
    }

    void setColumns(int cols) { columns = cols; resized(); }
    void setGap(int gap) { gridGap = gap; resized(); }

    void addGridChild(juce::Component* child, int col, int row, int colSpan = 1, int rowSpan = 1)
    {
        GridItem item;
        item.component = child;
        item.column = col;
        item.row = row;
        item.columnSpan = colSpan;
        item.rowSpan = rowSpan;
        gridItems.add(item);
        addAndMakeVisible(child);
        resized();
    }

    void resized() override
    {
        layoutGrid();
    }

private:
    struct GridItem
    {
        juce::Component* component = nullptr;
        int column = 0;
        int row = 0;
        int columnSpan = 1;
        int rowSpan = 1;
    };

    void layoutGrid()
    {
        auto bounds = getLocalBounds();
        int cellWidth = (bounds.getWidth() - (gridGap * (columns - 1))) / columns;

        for (auto& item : gridItems)
        {
            int x = item.column * (cellWidth + gridGap);
            int y = item.row * (cellWidth + gridGap);  // Using cellWidth as row height for now
            int width = (cellWidth * item.columnSpan) + (gridGap * (item.columnSpan - 1));
            int height = (cellWidth * item.rowSpan) + (gridGap * (item.rowSpan - 1));

            item.component->setBounds(x, y, width, height);
        }
    }

    int columns;
    int gridGap;
    juce::Array<GridItem> gridItems;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GridContainer)
};

} // namespace UI
} // namespace TSS
