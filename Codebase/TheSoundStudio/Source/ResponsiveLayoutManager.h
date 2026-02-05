/*
  ==============================================================================

    ResponsiveLayoutManager.h
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "VectorGraphicsManager.h"

// Layout breakpoints and responsive behavior
struct ResponsiveBreakpoint
{
    int minWidth;
    int maxWidth;
    float scaleFactor;
    int columns;
    float spacing;
    String description;
};

class ResponsiveLayoutManager : public Component
{
public:
    ResponsiveLayoutManager();
    ~ResponsiveLayoutManager() override;
    
    // Component overrides
    void resized() override;
    void paint(Graphics& g) override;
    
    // Layout management
    void addResponsiveComponent(Component* component, int priority = 0, 
                              float minWidth = 0.0f, float maxWidth = 0.0f);
    void removeResponsiveComponent(Component* component);
    
    // Layout modes
    enum class LayoutMode
    {
        AUTO,           // Automatic layout based on screen size
        GRID,           // Grid-based layout
        FLEX,           // Flexbox-style layout
        STACK,          // Vertical stack layout
        SIDEBAR         // Sidebar + main content layout
    };
    
    void setLayoutMode(LayoutMode mode);
    LayoutMode getLayoutMode() const { return currentLayoutMode; }
    
    // Responsive properties
    void setResponsiveMargins(float small, float medium, float large, float xlarge);
    void setResponsiveSpacing(float small, float medium, float large, float xlarge);
    void setGridColumns(int small, int medium, int large, int xlarge);
    
    // Animation and transitions
    void setAnimationDuration(int milliseconds) { animationDuration = milliseconds; }
    void enableSmoothTransitions(bool enabled) { smoothTransitions = enabled; }
    
    // Orientation handling
    void onOrientationChanged();
    bool isLandscape() const;
    bool isPortrait() const;
    
    // Accessibility
    void setAccessibilityMode(bool enabled);
    bool isAccessibilityMode() const { return accessibilityMode; }
    
private:
    struct ResponsiveComponent
    {
        Component* component;
        int priority;
        float minWidth;
        float maxWidth;
        Rectangle<int> targetBounds;
        Rectangle<int> currentBounds;
        bool isVisible;
        
        ResponsiveComponent(Component* comp, int prio, float minW, float maxW)
            : component(comp), priority(prio), minWidth(minW), maxWidth(maxW), isVisible(true) {}
    };
    
    // Component management
    OwnedArray<ResponsiveComponent> responsiveComponents;
    LayoutMode currentLayoutMode = LayoutMode::AUTO;
    
    // Responsive properties
    ScreenSize currentScreenSize = ScreenSize::MEDIUM;
    int currentColumns = 3;
    float currentMargin = 16.0f;
    float currentSpacing = 8.0f;
    
    // Responsive settings per screen size
    float margins[4] = { 8.0f, 16.0f, 24.0f, 32.0f };    // small, medium, large, xlarge
    float spacing[4] = { 4.0f, 8.0f, 12.0f, 16.0f };     // small, medium, large, xlarge  
    int gridColumns[4] = { 1, 2, 3, 4 };                  // small, medium, large, xlarge
    
    // Animation
    int animationDuration = 200;
    bool smoothTransitions = true;
    ComponentAnimator animator;
    
    // Accessibility
    bool accessibilityMode = false;
    
    // Layout calculation methods
    void calculateAutoLayout();
    void calculateGridLayout();
    void calculateFlexLayout();
    void calculateStackLayout();
    void calculateSidebarLayout();
    
    // Helper methods
    void updateCurrentProperties();
    void animateToNewLayout();
    Rectangle<int> calculateComponentBounds(const ResponsiveComponent& comp, 
                                          Rectangle<int> availableArea,
                                          int index, int totalComponents);
    
    // Screen size detection
    void updateScreenSize();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ResponsiveLayoutManager)
};