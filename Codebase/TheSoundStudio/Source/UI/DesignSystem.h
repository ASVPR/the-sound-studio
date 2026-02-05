/*
  ==============================================================================

    DesignSystem.h
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

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
// INLINE COLOR DEFINITIONS (header-only)
//==============================================================================

// Primary Brand Colors - Modern blue with good contrast
inline const juce::Colour Colors::primary = juce::Colour(0xff3b82f6);        // Blue-500
inline const juce::Colour Colors::primaryDark = juce::Colour(0xff2563eb);    // Blue-600
inline const juce::Colour Colors::primaryLight = juce::Colour(0xff60a5fa);   // Blue-400

// Semantic Colors - Industry standard
inline const juce::Colour Colors::success = juce::Colour(0xff10b981);  // Green-500
inline const juce::Colour Colors::warning = juce::Colour(0xfff59e0b);  // Amber-500
inline const juce::Colour Colors::error = juce::Colour(0xffef4444);    // Red-500
inline const juce::Colour Colors::info = juce::Colour(0xff06b6d4);     // Cyan-500

// Light Theme Colors
inline const juce::Colour Colors::Light::background = juce::Colour(0xfffafafa);      // Gray-50
inline const juce::Colour Colors::Light::surface = juce::Colour(0xffffffff);         // White
inline const juce::Colour Colors::Light::surfaceHover = juce::Colour(0xfff5f5f5);    // Gray-100
inline const juce::Colour Colors::Light::border = juce::Colour(0xffe5e7eb);          // Gray-200
inline const juce::Colour Colors::Light::divider = juce::Colour(0xffd1d5db);         // Gray-300

inline const juce::Colour Colors::Light::text = juce::Colour(0xff111827);            // Gray-900
inline const juce::Colour Colors::Light::textSecondary = juce::Colour(0xff6b7280);   // Gray-500
inline const juce::Colour Colors::Light::textDisabled = juce::Colour(0xff9ca3af);    // Gray-400

// Dark Theme Colors - True dark with good contrast
inline const juce::Colour Colors::Dark::background = juce::Colour(0xff0f172a);       // Slate-900
inline const juce::Colour Colors::Dark::surface = juce::Colour(0xff1e293b);          // Slate-800
inline const juce::Colour Colors::Dark::surfaceHover = juce::Colour(0xff334155);     // Slate-700
inline const juce::Colour Colors::Dark::border = juce::Colour(0xff475569);           // Slate-600
inline const juce::Colour Colors::Dark::divider = juce::Colour(0xff64748b);          // Slate-500

inline const juce::Colour Colors::Dark::text = juce::Colour(0xfff8fafc);             // Slate-50
inline const juce::Colour Colors::Dark::textSecondary = juce::Colour(0xff94a3b8);    // Slate-400
inline const juce::Colour Colors::Dark::textDisabled = juce::Colour(0xff64748b);     // Slate-500

// Audio-specific colors
inline const juce::Colour Colors::waveform = juce::Colour(0xff3b82f6);       // Blue-500
inline const juce::Colour Colors::peakMeter = juce::Colour(0xff10b981);      // Green-500
inline const juce::Colour Colors::clipping = juce::Colour(0xffef4444);       // Red-500

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
// USABILITY MINIMUMS - Human-friendly minimum sizes
//==============================================================================
struct Usability
{
    // Typography minimums
    static constexpr float sidebarMinFont = 18.0f;     // Large preset
    static constexpr float toolbarLabelMinFont = 20.0f;
    static constexpr float visualiserAxisMinFont = 18.0f;
    static constexpr float visualiserValueMinFont = 18.0f;
    static constexpr float visualiserLegendMinFont = 14.0f;

    // Control minimums
    static constexpr int sidebarButtonMinHeight = 50;  // Large preset
    static constexpr int transportControlMinSize = 56;
    static constexpr int progressMinHeight = 32;
    static constexpr int toolbarTopMinHeight = 72;
    static constexpr int toolbarBottomMinHeight = 64;
    static constexpr int toolbarRowMinHeight = 30;
    static constexpr int visualiserHeaderMinHeight = 48;
    static constexpr int visualiserControlMinHeight = 32;
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

//==============================================================================
// LAYOUT RATIOS - Proportional layout derived from reference 1566x1440 content area
// All ratios are relative to the parent component's bounds via getLocalBounds()
//==============================================================================
struct Layout
{
    // Reference resolution (content area, excluding sidebar)
    static constexpr float kRefContentWidth  = 1566.0f;
    static constexpr float kRefContentHeight = 1440.0f;
    static constexpr float kRefTotalWidth    = 1920.0f;  // content + sidebar
    static constexpr float kRefTotalHeight   = 1440.0f;

    // Sidebar (MainViewComponent)
    static constexpr float kSidebarWidthRatio   = 354.0f / kRefTotalWidth;   // ~0.184
    static constexpr float kSidebarMinWidth     = 200.0f;
    static constexpr float kSidebarMaxWidth     = 420.0f;
    static constexpr float kLogoHeightRatio     = 65.0f / kRefTotalHeight;   // ~0.045
    static constexpr float kMenuButtonHeightRatio = 70.0f / kRefTotalHeight; // ~0.049
    static constexpr float kMenuButtonTopRatio  = 134.0f / kRefTotalHeight;  // ~0.093
    static constexpr float kVersionLabelHeight  = 40.0f;

    // Sidebar border
    static constexpr float kSidebarBorderWidth  = 2.0f;

    // Shortcut bar (top of chord/frequency player views)
    static constexpr float kShortcutBarHeightRatio = 344.0f / kRefContentHeight; // ~0.239

    // Toolbar (transport controls at bottom of views)
    static constexpr float kToolbarHeightRatio = 200.0f / kRefContentHeight;     // ~0.139

    // Visualiser grid (inside main container, between shortcut bar and toolbar)
    // Top row: full width spectrogram
    static constexpr float kVisTopRowHeightRatio   = 340.0f / 896.0f;  // ratio of visualiser area
    // Bottom row: two panels side by side
    static constexpr float kVisBottomLeftWidthRatio  = 742.0f / kRefContentWidth;
    static constexpr float kVisBottomRightWidthRatio = 1.0f - kVisBottomLeftWidthRatio;
    static constexpr float kVisInsetRatio            = 48.0f / kRefContentWidth;
    static constexpr float kVisGapRatio              = 30.0f / kRefContentHeight;

    // Realtime analysis: 4-panel grid
    static constexpr float kRtTopRowHeightRatio      = 340.0f / 1300.0f;
    static constexpr float kRtMidRowHeightRatio      = 300.0f / 1300.0f;
    static constexpr float kRtBottomRowHeightRatio   = 350.0f / 1300.0f;

    // Scanner views: single visualiser
    static constexpr float kScannerVisHeightRatio    = 280.0f / kRefContentHeight;

    // Shortcut card sizing
    static constexpr int   kShortcutCardsVisible     = 4;
    static constexpr float kShortcutCardWidthRatio   = 1.0f / kShortcutCardsVisible;
    static constexpr float kShortcutCardAspect       = 344.0f / 327.0f; // height / width

    // Transport toolbar button sizing (relative to toolbar height)
    static constexpr float kTransportButtonSizeRatio = 0.45f;  // button height = 45% of toolbar height
    static constexpr float kTransportProgressHeight  = 0.28f;  // progress bar = 28% of toolbar height
    static constexpr float kTransportRecordSizeRatio = 0.50f;  // record button slightly larger

    // Transport toolbar layout zones (must sum to 1.0f)
    static constexpr float kTransportTopZoneRatio = 0.34f;
    static constexpr float kTransportProgressZoneRatio = 0.20f;
    static constexpr float kTransportBottomZoneRatio =
        1.0f - kTransportTopZoneRatio - kTransportProgressZoneRatio;

    // VisualiserSelector header
    static constexpr float kVisSelectorHeaderHeight  = 42.0f;

    // Font scaling: compute local scale as getWidth() / kRefContentWidth
    // then multiply by base font size
};

} // namespace Design
} // namespace TSS
