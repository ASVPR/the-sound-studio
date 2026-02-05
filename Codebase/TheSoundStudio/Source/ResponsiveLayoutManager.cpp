/*
  ==============================================================================

    ResponsiveLayoutManager.cpp
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#include "ResponsiveLayoutManager.h"

ResponsiveLayoutManager::ResponsiveLayoutManager()
{
    updateScreenSize();
    updateCurrentProperties();
}

ResponsiveLayoutManager::~ResponsiveLayoutManager()
{
}

void ResponsiveLayoutManager::resized()
{
    updateScreenSize();
    updateCurrentProperties();
    
    switch (currentLayoutMode)
    {
        case LayoutMode::AUTO:
            calculateAutoLayout();
            break;
        case LayoutMode::GRID:
            calculateGridLayout();
            break;
        case LayoutMode::FLEX:
            calculateFlexLayout();
            break;
        case LayoutMode::STACK:
            calculateStackLayout();
            break;
        case LayoutMode::SIDEBAR:
            calculateSidebarLayout();
            break;
    }
    
    if (smoothTransitions)
        animateToNewLayout();
    else
    {
        // Apply bounds immediately
        for (auto* comp : responsiveComponents)
        {
            if (comp->component && comp->isVisible)
            {
                comp->component->setBounds(comp->targetBounds);
                comp->currentBounds = comp->targetBounds;
            }
        }
    }
}

void ResponsiveLayoutManager::paint(Graphics& g)
{
    // Optional: Draw layout debugging information
    #ifdef DEBUG_RESPONSIVE_LAYOUT
    g.setColour(Colours::red.withAlpha(0.1f));
    for (auto* comp : responsiveComponents)
    {
        g.drawRect(comp->targetBounds, 1);
    }
    #endif
}

void ResponsiveLayoutManager::addResponsiveComponent(Component* component, int priority, 
                                                   float minWidth, float maxWidth)
{
    if (component == nullptr) return;
    
    auto* responsiveComp = new ResponsiveComponent(component, priority, minWidth, maxWidth);
    responsiveComponents.add(responsiveComp);
    addAndMakeVisible(component);
    
    // Sort by priority (higher priority first)
    responsiveComponents.sort([](const ResponsiveComponent* a, const ResponsiveComponent* b) 
    {
        return a->priority > b->priority;
    });
    
    resized(); // Recalculate layout
}

void ResponsiveLayoutManager::removeResponsiveComponent(Component* component)
{
    for (int i = responsiveComponents.size() - 1; i >= 0; --i)
    {
        if (responsiveComponents[i]->component == component)
        {
            removeChildComponent(component);
            responsiveComponents.remove(i);
            resized(); // Recalculate layout
            break;
        }
    }
}

void ResponsiveLayoutManager::setLayoutMode(LayoutMode mode)
{
    if (currentLayoutMode != mode)
    {
        currentLayoutMode = mode;
        resized();
    }
}

void ResponsiveLayoutManager::setResponsiveMargins(float small, float medium, float large, float xlarge)
{
    margins[0] = small;
    margins[1] = medium;
    margins[2] = large;
    margins[3] = xlarge;
    updateCurrentProperties();
    resized();
}

void ResponsiveLayoutManager::setResponsiveSpacing(float small, float medium, float large, float xlarge)
{
    spacing[0] = small;
    spacing[1] = medium;
    spacing[2] = large;
    spacing[3] = xlarge;
    updateCurrentProperties();
    resized();
}

void ResponsiveLayoutManager::setGridColumns(int small, int medium, int large, int xlarge)
{
    gridColumns[0] = jmax(1, small);
    gridColumns[1] = jmax(1, medium);
    gridColumns[2] = jmax(1, large);
    gridColumns[3] = jmax(1, xlarge);
    updateCurrentProperties();
    resized();
}

void ResponsiveLayoutManager::onOrientationChanged()
{
    // Handle orientation changes with a slight delay to avoid layout thrashing
    Timer::callAfterDelay(100, [this]() 
    {
        updateScreenSize();
        resized();
    });
}

bool ResponsiveLayoutManager::isLandscape() const
{
    return getWidth() > getHeight();
}

bool ResponsiveLayoutManager::isPortrait() const
{
    return getHeight() > getWidth();
}

void ResponsiveLayoutManager::setAccessibilityMode(bool enabled)
{
    if (accessibilityMode != enabled)
    {
        accessibilityMode = enabled;
        
        // Adjust spacing and sizing for accessibility
        if (enabled)
        {
            for (int i = 0; i < 4; ++i)
            {
                margins[i] *= 1.2f;
                spacing[i] *= 1.3f;
            }
        }
        
        resized();
    }
}

void ResponsiveLayoutManager::calculateAutoLayout()
{
    // Auto layout chooses the best layout mode based on screen size and aspect ratio
    if (currentScreenSize == ScreenSize::SMALL)
    {
        calculateStackLayout(); // Stack on small screens
    }
    else if (isPortrait() && currentScreenSize <= ScreenSize::MEDIUM)
    {
        calculateStackLayout(); // Stack in portrait mode on medium screens
    }
    else
    {
        calculateGridLayout(); // Grid for larger screens
    }
}

void ResponsiveLayoutManager::calculateGridLayout()
{
    Rectangle<int> area = getLocalBounds().reduced((int)currentMargin);
    
    if (responsiveComponents.isEmpty()) return;
    
    int cols = currentColumns;
    int rows = (responsiveComponents.size() + cols - 1) / cols;
    
    float cellWidth = (area.getWidth() - (cols - 1) * currentSpacing) / (float)cols;
    float cellHeight = (area.getHeight() - (rows - 1) * currentSpacing) / (float)rows;
    
    for (int i = 0; i < responsiveComponents.size(); ++i)
    {
        auto* comp = responsiveComponents[i];
        
        int col = i % cols;
        int row = i / cols;
        
        float x = area.getX() + col * (cellWidth + currentSpacing);
        float y = area.getY() + row * (cellHeight + currentSpacing);
        
        // Check component constraints
        float finalWidth = cellWidth;
        float finalHeight = cellHeight;
        
        if (comp->minWidth > 0 && finalWidth < comp->minWidth)
            finalWidth = comp->minWidth;
        if (comp->maxWidth > 0 && finalWidth > comp->maxWidth)
            finalWidth = comp->maxWidth;
        
        // Hide component if it doesn't fit
        comp->isVisible = (finalWidth <= area.getWidth() && finalHeight <= area.getHeight());
        
        comp->targetBounds = Rectangle<int>((int)x, (int)y, (int)finalWidth, (int)finalHeight);
        
        if (comp->component)
            comp->component->setVisible(comp->isVisible);
    }
}

void ResponsiveLayoutManager::calculateFlexLayout()
{
    Rectangle<int> area = getLocalBounds().reduced((int)currentMargin);
    
    if (responsiveComponents.isEmpty()) return;
    
    // Calculate total weight and available space
    float totalWeight = 0.0f;
    for (auto* comp : responsiveComponents)
    {
        if (comp->isVisible)
            totalWeight += 1.0f; // Equal weight for now - could be made configurable
    }
    
    float availableWidth = (float)area.getWidth() - (responsiveComponents.size() - 1) * currentSpacing;
    float itemWidth = availableWidth / totalWeight;
    
    float currentX = (float)area.getX();
    
    for (auto* comp : responsiveComponents)
    {
        if (!comp->isVisible) continue;
        
        float finalWidth = itemWidth;
        
        // Apply constraints
        if (comp->minWidth > 0 && finalWidth < comp->minWidth)
            finalWidth = comp->minWidth;
        if (comp->maxWidth > 0 && finalWidth > comp->maxWidth)
            finalWidth = comp->maxWidth;
        
        comp->targetBounds = Rectangle<int>((int)currentX, area.getY(), 
                                          (int)finalWidth, area.getHeight());
        
        currentX += finalWidth + currentSpacing;
    }
}

void ResponsiveLayoutManager::calculateStackLayout()
{
    Rectangle<int> area = getLocalBounds().reduced((int)currentMargin);
    
    if (responsiveComponents.isEmpty()) return;
    
    int visibleCount = 0;
    for (auto* comp : responsiveComponents)
        if (comp->isVisible) visibleCount++;
    
    float availableHeight = (float)area.getHeight() - (visibleCount - 1) * currentSpacing;
    float itemHeight = availableHeight / visibleCount;
    
    float currentY = (float)area.getY();
    
    for (auto* comp : responsiveComponents)
    {
        if (!comp->isVisible) continue;
        
        comp->targetBounds = Rectangle<int>(area.getX(), (int)currentY, 
                                          area.getWidth(), (int)itemHeight);
        
        currentY += itemHeight + currentSpacing;
    }
}

void ResponsiveLayoutManager::calculateSidebarLayout()
{
    Rectangle<int> area = getLocalBounds().reduced((int)currentMargin);
    
    if (responsiveComponents.isEmpty()) return;
    
    // First component becomes sidebar, rest fill main area
    float sidebarWidth = area.getWidth() * 0.25f; // 25% for sidebar
    
    if (responsiveComponents.size() > 0)
    {
        // Sidebar
        auto* sidebarComp = responsiveComponents[0];
        sidebarComp->targetBounds = Rectangle<int>(area.getX(), area.getY(), 
                                                 (int)sidebarWidth, area.getHeight());
        
        // Main content area
        Rectangle<int> mainArea(area.getX() + (int)sidebarWidth + (int)currentSpacing, area.getY(),
                               area.getWidth() - (int)sidebarWidth - (int)currentSpacing, area.getHeight());
        
        // Layout remaining components in main area using grid
        int remainingCount = responsiveComponents.size() - 1;
        if (remainingCount > 0)
        {
            int cols = jmin(currentColumns, remainingCount);
            int rows = (remainingCount + cols - 1) / cols;
            
            float cellWidth = (mainArea.getWidth() - (cols - 1) * currentSpacing) / (float)cols;
            float cellHeight = (mainArea.getHeight() - (rows - 1) * currentSpacing) / (float)rows;
            
            for (int i = 1; i < responsiveComponents.size(); ++i)
            {
                auto* comp = responsiveComponents[i];
                int index = i - 1;
                
                int col = index % cols;
                int row = index / cols;
                
                float x = mainArea.getX() + col * (cellWidth + currentSpacing);
                float y = mainArea.getY() + row * (cellHeight + currentSpacing);
                
                comp->targetBounds = Rectangle<int>((int)x, (int)y, (int)cellWidth, (int)cellHeight);
            }
        }
    }
}

void ResponsiveLayoutManager::updateCurrentProperties()
{
    int sizeIndex = (int)currentScreenSize;
    currentMargin = margins[sizeIndex];
    currentSpacing = spacing[sizeIndex];
    currentColumns = gridColumns[sizeIndex];
}

void ResponsiveLayoutManager::animateToNewLayout()
{
    for (auto* comp : responsiveComponents)
    {
        if (comp->component && comp->isVisible)
        {
            animator.animateComponent(comp->component, comp->targetBounds, 
                                    1.0f, animationDuration, false, 0.8, 0.2);
            comp->currentBounds = comp->targetBounds;
        }
    }
}

void ResponsiveLayoutManager::updateScreenSize()
{
    ScreenSize newSize = VectorGraphicsManager::getScreenSize(getWidth());
    if (newSize != currentScreenSize)
    {
        currentScreenSize = newSize;
        updateCurrentProperties();
    }
}