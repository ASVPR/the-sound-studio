/*
  ==============================================================================

    ResponsiveLayout.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <unordered_map>
#include <functional>

//==============================================================================
/**
    Screen size breakpoints for responsive design.
    Defines standard breakpoints used across the application.
*/
enum class ScreenSize
{
    Mobile,     // 0-767px
    Tablet,     // 768-1023px  
    Desktop,    // 1024-1919px
    FourK       // 1920px+
};

//==============================================================================
/**
    Responsive breakpoint configuration structure.
    Defines properties for each screen size breakpoint.
*/
struct ResponsiveBreakpoint
{
    int minWidth;           // Minimum width for this breakpoint
    int maxWidth;           // Maximum width for this breakpoint (-1 for unlimited)
    float scaleFactor;      // Scale factor for elements at this breakpoint
    float baseFontSize;     // Base font size for this breakpoint
    float paddingFactor;    // Padding scale factor
    float marginFactor;     // Margin scale factor
    String name;            // Human-readable name
    
    ResponsiveBreakpoint(int min, int max, float scale, float font, float pad, float margin, const String& n)
        : minWidth(min), maxWidth(max), scaleFactor(scale), baseFontSize(font), 
          paddingFactor(pad), marginFactor(margin), name(n) {}
};

//==============================================================================
/**
    Static responsive layout utility class.
    Provides helper methods for calculating responsive dimensions, breakpoints,
    and scaling factors without requiring instantiation.
*/
class ResponsiveLayout
{
public:
    //==============================================================================
    // Breakpoint Management
    
    /** Get the current screen size category based on component dimensions. */
    static ScreenSize getScreenSize(int width);
    
    /** Get the current screen size category based on component bounds. */
    static ScreenSize getScreenSize(const Rectangle<int>& bounds);
    
    /** Get breakpoint configuration for a specific screen size. */
    static ResponsiveBreakpoint getBreakpoint(ScreenSize screenSize);
    
    /** Check if current dimensions match a specific screen size. */
    static bool isScreenSize(int width, ScreenSize targetSize);
    
    //==============================================================================
    // Scale Factor Calculations
    
    /** Get scale factor for current screen size. */
    static float getScaleFactor(ScreenSize screenSize);
    
    /** Get scale factor based on component width. */
    static float getScaleFactor(int width);
    
    /** Get scale factor based on component bounds. */
    static float getScaleFactor(const Rectangle<int>& bounds);
    
    /** Get adaptive scale factor that smoothly interpolates between breakpoints. */
    static float getAdaptiveScaleFactor(int width);
    
    //==============================================================================
    // Responsive Dimensions
    
    /** Calculate responsive width as percentage of parent. */
    static int getResponsiveWidth(int parentWidth, float percentage, ScreenSize screenSize);
    
    /** Calculate responsive height as percentage of parent. */
    static int getResponsiveHeight(int parentHeight, float percentage, ScreenSize screenSize);
    
    /** Calculate responsive dimensions maintaining aspect ratio. */
    static Rectangle<int> getResponsiveBounds(const Rectangle<int>& parentBounds, 
                                            float widthPercent, float heightPercent, 
                                            ScreenSize screenSize);
    
    /** Calculate responsive bounds with min/max constraints. */
    static Rectangle<int> getConstrainedBounds(const Rectangle<int>& parentBounds,
                                             float widthPercent, float heightPercent,
                                             int minWidth, int maxWidth,
                                             int minHeight, int maxHeight,
                                             ScreenSize screenSize);
    
    //==============================================================================
    // Utility Functions
    
    /** Convert fixed pixel values to responsive percentages. */
    static float pixelsToPercentage(int pixels, int parentDimension);
    
    /** Convert percentage back to pixels with responsive scaling. */
    static int percentageToPixels(float percentage, int parentDimension, ScreenSize screenSize);
    
    /** Get optimal font size for screen size. */
    static float getResponsiveFontSize(float baseFontSize, ScreenSize screenSize);
    
    /** Calculate responsive padding. */
    static int getResponsivePadding(int basePadding, ScreenSize screenSize);
    
    /** Calculate responsive margins. */
    static int getResponsiveMargin(int baseMargin, ScreenSize screenSize);
    
    /** Get responsive spacing between components. */
    static int getResponsiveSpacing(int baseSpacing, ScreenSize screenSize);
    
    /** Calculate grid columns for screen size. */
    static int getOptimalColumns(ScreenSize screenSize, int maxColumns = 4);
    
    /** Get responsive border width. */
    static float getResponsiveBorderWidth(float baseBorderWidth, ScreenSize screenSize);
    
    //==============================================================================
    // Advanced Layout Calculations
    
    /** Calculate responsive grid layout bounds. */
    static Array<Rectangle<int>> calculateGridLayout(const Rectangle<int>& container,
                                                   int itemCount, int columns,
                                                   int spacing, ScreenSize screenSize);
    
    /** Calculate responsive flexbox-style layout. */
    static Array<Rectangle<int>> calculateFlexLayout(const Rectangle<int>& container,
                                                   const Array<float>& flexWeights,
                                                   bool horizontal = true,
                                                   int spacing = 0, ScreenSize screenSize = ScreenSize::Desktop);
    
    /** Calculate responsive stack layout (vertical or horizontal). */
    static Array<Rectangle<int>> calculateStackLayout(const Rectangle<int>& container,
                                                    int itemCount, bool horizontal = false,
                                                    int spacing = 0, ScreenSize screenSize = ScreenSize::Desktop);
    
private:
    // Static breakpoint definitions
    static const std::vector<ResponsiveBreakpoint> breakpoints;
    
    // Private constructor to prevent instantiation
    ResponsiveLayout() = delete;
    
    JUCE_DECLARE_NON_COPYABLE(ResponsiveLayout)
};

//==============================================================================
/**
    Base component class with built-in responsive behavior.
    Components can inherit from this to automatically get responsive scaling,
    positioning, and layout management.
*/
class ResponsiveComponent : public Component
{
public:
    ResponsiveComponent();
    virtual ~ResponsiveComponent() override;
    
    //==============================================================================
    // Component overrides with responsive behavior
    void resized() override;
    void parentSizeChanged() override;
    
    //==============================================================================
    // Responsive Configuration
    
    /** Enable or disable responsive behavior. */
    void setResponsiveEnabled(bool enabled);
    bool isResponsiveEnabled() const { return responsiveEnabled; }
    
    /** Set responsive size as percentage of parent. */
    void setResponsiveSize(float widthPercent, float heightPercent);
    
    /** Set responsive position as percentage of parent. */
    void setResponsivePosition(float xPercent, float yPercent);
    
    /** Set minimum and maximum size constraints. */
    void setResponsiveSizeConstraints(int minWidth, int maxWidth, int minHeight, int maxHeight);
    
    /** Set responsive margins. */
    void setResponsiveMargins(int top, int right, int bottom, int left);
    
    /** Set responsive padding. */
    void setResponsivePadding(int top, int right, int bottom, int left);
    
    /** Set whether to maintain aspect ratio during resize. */
    void setMaintainAspectRatio(bool maintain, float ratio = 0.0f);
    
    //==============================================================================
    // Responsive State Access
    
    /** Get current screen size category. */
    ScreenSize getCurrentScreenSize() const;
    
    /** Get current scale factor. */
    float getCurrentScaleFactor() const;
    
    /** Get current responsive font size. */
    float getResponsiveFontSize(float baseFontSize) const;
    
    /** Get current responsive spacing. */
    int getResponsiveSpacing(int baseSpacing) const;
    
    //==============================================================================
    // Layout Helpers
    
    /** Position child component responsively. */
    void setChildBoundsResponsive(Component* child, float xPercent, float yPercent,
                                float widthPercent, float heightPercent);
    
    /** Create responsive grid layout for child components. */
    void layoutChildrenInGrid(const Array<Component*>& children, int columns,
                            int spacing = 0, bool fillHorizontally = true);
    
    /** Create responsive flex layout for child components. */
    void layoutChildrenFlex(const Array<Component*>& children, 
                          const Array<float>& weights,
                          bool horizontal = true, int spacing = 0);
    
    /** Create responsive stack layout for child components. */
    void layoutChildrenStack(const Array<Component*>& children, 
                           bool horizontal = false, int spacing = 0);
    
    //==============================================================================
    // Animation and Transitions
    
    /** Set animation duration for responsive transitions. */
    void setAnimationDuration(int milliseconds) { animationDuration = milliseconds; }
    
    /** Enable smooth transitions between responsive states. */
    void setSmoothTransitions(bool enabled) { smoothTransitions = enabled; }
    
protected:
    //==============================================================================
    // Virtual methods for subclasses
    
    /** Called when screen size changes. Subclasses can override for custom behavior. */
    virtual void onScreenSizeChanged(ScreenSize oldSize, ScreenSize newSize);
    
    /** Called when scale factor changes. Subclasses can override for custom behavior. */
    virtual void onScaleFactorChanged(float newScaleFactor);
    
    /** Calculate responsive bounds. Can be overridden for custom responsive behavior. */
    virtual Rectangle<int> calculateResponsiveBounds(const Rectangle<int>& parentBounds);
    
private:
    //==============================================================================
    // Internal state
    bool responsiveEnabled = true;
    float widthPercent = 100.0f;
    float heightPercent = 100.0f;
    float xPercent = 0.0f;
    float yPercent = 0.0f;
    
    int minWidth = 0, maxWidth = -1;
    int minHeight = 0, maxHeight = -1;
    
    BorderSize<int> responsiveMargins;
    BorderSize<int> responsivePadding;
    
    bool maintainAspectRatio = false;
    float aspectRatio = 1.0f;
    
    ScreenSize lastScreenSize = ScreenSize::Desktop;
    float lastScaleFactor = 1.0f;
    
    // Animation
    int animationDuration = 150;
    bool smoothTransitions = true;
    ComponentAnimator animator;
    
    //==============================================================================
    // Internal methods
    void updateResponsiveLayout();
    void triggerResponsiveUpdate();
    bool hasScreenSizeChanged() const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ResponsiveComponent)
};

//==============================================================================
/**
    Responsive layout constraints for advanced positioning.
    Used for complex responsive layouts with multiple constraint conditions.
*/
struct ResponsiveConstraints
{
    struct SizeConstraint
    {
        float percentage = 100.0f;      // Percentage of parent
        int minPixels = 0;              // Minimum size in pixels
        int maxPixels = -1;             // Maximum size in pixels (-1 = unlimited)
        bool useAspectRatio = false;    // Whether to maintain aspect ratio
        float aspectRatio = 1.0f;       // Aspect ratio if maintained
        
        SizeConstraint() = default;
        SizeConstraint(float perc) : percentage(perc) {}
        SizeConstraint(float perc, int minPx, int maxPx) 
            : percentage(perc), minPixels(minPx), maxPixels(maxPx) {}
    };
    
    struct PositionConstraint  
    {
        float percentage = 0.0f;        // Percentage of parent
        int pixelOffset = 0;            // Additional pixel offset
        bool centerAlign = false;       // Whether to center align
        
        PositionConstraint() = default;
        PositionConstraint(float perc) : percentage(perc) {}
        PositionConstraint(float perc, int offset) : percentage(perc), pixelOffset(offset) {}
    };
    
    SizeConstraint width;
    SizeConstraint height;
    PositionConstraint x;
    PositionConstraint y;
    
    BorderSize<int> margins;
    BorderSize<int> padding;
    
    // Screen-size specific overrides
    std::unordered_map<ScreenSize, ResponsiveConstraints> screenOverrides;
    
    /** Apply constraints to calculate actual bounds. */
    Rectangle<int> apply(const Rectangle<int>& parentBounds, ScreenSize screenSize) const;
    
    /** Set screen-specific override constraints. */
    void setScreenOverride(ScreenSize screenSize, const ResponsiveConstraints& constraints);
    
    /** Check if has override for specific screen size. */
    bool hasScreenOverride(ScreenSize screenSize) const;
    
    /** Get constraints for specific screen size (with overrides applied). */
    ResponsiveConstraints getForScreen(ScreenSize screenSize) const;
};