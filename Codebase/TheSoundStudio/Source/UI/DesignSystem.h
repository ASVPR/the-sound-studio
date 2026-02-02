/*
  ==============================================================================

    DesignSystem.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

namespace TSS {
namespace Design {

//==============================================================================
// SPACING SYSTEM - Based on 8px grid for consistency
//==============================================================================
struct Spacing
{
    static constexpr int xxxs = 2;   // 2px  - Hairline
    static constexpr int xxs  = 4;   // 4px  - Tight
    static constexpr int xs   = 8;   // 8px  - Base unit
    static constexpr int sm   = 12;  // 12px - Small
    static constexpr int md   = 16;  // 16px - Medium
    static constexpr int lg   = 24;  // 24px - Large
    static constexpr int xl   = 32;  // 32px - Extra Large
    static constexpr int xxl  = 48;  // 48px - 2X Large
    static constexpr int xxxl = 64;  // 64px - 3X Large

    // Component-specific spacing
    static constexpr int buttonPadding = md;
    static constexpr int panelPadding = lg;
    static constexpr int sectionGap = xl;
    static constexpr int controlGap = sm;
};

//==============================================================================
// COLOR SYSTEM - Modern, accessible palette with dark/light themes
//==============================================================================
struct Colors
{
    // Primary Brand Colors
    static const juce::Colour primary;
    static const juce::Colour primaryDark;
    static const juce::Colour primaryLight;

    // Semantic Colors
    static const juce::Colour success;
    static const juce::Colour warning;
    static const juce::Colour error;
    static const juce::Colour info;

    // Neutral Colors (Light Theme)
    struct Light
    {
        static const juce::Colour background;
        static const juce::Colour surface;
        static const juce::Colour surfaceHover;
        static const juce::Colour border;
        static const juce::Colour divider;

        static const juce::Colour text;
        static const juce::Colour textSecondary;
        static const juce::Colour textDisabled;
    };

    // Neutral Colors (Dark Theme)
    struct Dark
    {
        static const juce::Colour background;
        static const juce::Colour surface;
        static const juce::Colour surfaceHover;
        static const juce::Colour border;
        static const juce::Colour divider;

        static const juce::Colour text;
        static const juce::Colour textSecondary;
        static const juce::Colour textDisabled;
    };

    // Audio-specific colors
    static const juce::Colour waveform;
    static const juce::Colour peakMeter;
    static const juce::Colour clipping;
};

//==============================================================================
// TYPOGRAPHY SYSTEM - Clear hierarchy with modern fonts
//==============================================================================
struct Typography
{
    // Font Sizes (based on type scale)
    static constexpr float h1 = 32.0f;    // Page title
    static constexpr float h2 = 24.0f;    // Section header
    static constexpr float h3 = 20.0f;    // Subsection
    static constexpr float h4 = 18.0f;    // Card header
    static constexpr float body1 = 16.0f; // Body text
    static constexpr float body2 = 14.0f; // Secondary text
    static constexpr float caption = 12.0f; // Labels, captions
    static constexpr float overline = 10.0f; // Tiny labels

    // Font Weights
    enum class Weight
    {
        Light = 300,
        Regular = 400,
        SemiBold = 600,
        Bold = 700
    };

    // Line Heights (multipliers)
    static constexpr float lineHeightTight = 1.2f;
    static constexpr float lineHeightNormal = 1.5f;
    static constexpr float lineHeightLoose = 1.8f;

    // Helper to create fonts with proper sizing
    static juce::Font getFont(float size, Weight weight = Weight::Regular);
    static juce::Font getHeading1();
    static juce::Font getHeading2();
    static juce::Font getHeading3();
    static juce::Font getBody();
    static juce::Font getCaption();
};

//==============================================================================
// BORDER RADIUS - Consistent corner styling
//==============================================================================
struct BorderRadius
{
    static constexpr float none = 0.0f;
    static constexpr float sm = 4.0f;
    static constexpr float md = 8.0f;
    static constexpr float lg = 12.0f;
    static constexpr float xl = 16.0f;
    static constexpr float full = 9999.0f; // Pill shape
};

//==============================================================================
// SHADOWS - Elevation system for depth perception
//==============================================================================
struct Shadows
{
    static const juce::DropShadow none;
    static const juce::DropShadow sm;  // Subtle elevation
    static const juce::DropShadow md;  // Card elevation
    static const juce::DropShadow lg;  // Modal elevation
    static const juce::DropShadow xl;  // Popover elevation
};

//==============================================================================
// ANIMATION TIMING - Smooth, natural motion
//==============================================================================
struct Animation
{
    // Duration in milliseconds
    static constexpr int instant = 0;
    static constexpr int fast = 150;
    static constexpr int normal = 250;
    static constexpr int slow = 400;

    // Easing curves
    enum class Easing
    {
        Linear,
        EaseIn,
        EaseOut,
        EaseInOut,
        Spring
    };

    static float applyEasing(float t, Easing easing);
};

//==============================================================================
// BREAKPOINTS - Responsive design breakpoints
//==============================================================================
struct Breakpoints
{
    static constexpr int xs = 480;   // Mobile
    static constexpr int sm = 768;   // Tablet
    static constexpr int md = 1024;  // Desktop
    static constexpr int lg = 1440;  // Large Desktop
    static constexpr int xl = 1920;  // HD

    enum class Size { XS, SM, MD, LG, XL };
    static Size getBreakpoint(int width);
};

//==============================================================================
// COMPONENT DIMENSIONS - Standard sizes for UI elements
//==============================================================================
struct Dimensions
{
    // Button heights
    static constexpr int buttonSm = 32;
    static constexpr int buttonMd = 40;
    static constexpr int buttonLg = 48;

    // Input fields
    static constexpr int inputHeight = 40;
    static constexpr int textAreaMinHeight = 80;

    // Knobs and sliders
    static constexpr int knobSm = 48;
    static constexpr int knobMd = 64;
    static constexpr int knobLg = 96;

    // Meters
    static constexpr int meterWidth = 20;
    static constexpr int meterMinHeight = 120;

    // Sidebar
    static constexpr int sidebarWidth = 280;
    static constexpr int sidebarMinWidth = 200;
    static constexpr int sidebarMaxWidth = 400;
};

//==============================================================================
// Z-INDEX - Layering system for stacking order
//==============================================================================
struct ZIndex
{
    static constexpr int background = 0;
    static constexpr int content = 1;
    static constexpr int header = 10;
    static constexpr int dropdown = 100;
    static constexpr int overlay = 500;
    static constexpr int modal = 1000;
    static constexpr int tooltip = 2000;
    static constexpr int notification = 3000;
};

} // namespace Design
} // namespace TSS
