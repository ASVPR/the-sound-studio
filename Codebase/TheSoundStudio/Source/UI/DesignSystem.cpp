/*
  ==============================================================================

    DesignSystem.cpp

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#include "DesignSystem.h"

namespace TSS {
namespace Design {

//==============================================================================
// COLOR IMPLEMENTATIONS
//==============================================================================

// Primary Brand Colors - Modern blue with good contrast
const juce::Colour Colors::primary = juce::Colour(0xff3b82f6);        // Blue-500
const juce::Colour Colors::primaryDark = juce::Colour(0xff2563eb);    // Blue-600
const juce::Colour Colors::primaryLight = juce::Colour(0xff60a5fa);   // Blue-400

// Semantic Colors - Industry standard
const juce::Colour Colors::success = juce::Colour(0xff10b981);  // Green-500
const juce::Colour Colors::warning = juce::Colour(0xfff59e0b);  // Amber-500
const juce::Colour Colors::error = juce::Colour(0xffef4444);    // Red-500
const juce::Colour Colors::info = juce::Colour(0xff06b6d4);     // Cyan-500

// Light Theme Colors
const juce::Colour Colors::Light::background = juce::Colour(0xfffafafa);      // Gray-50
const juce::Colour Colors::Light::surface = juce::Colour(0xffffffff);         // White
const juce::Colour Colors::Light::surfaceHover = juce::Colour(0xfff5f5f5);    // Gray-100
const juce::Colour Colors::Light::border = juce::Colour(0xffe5e7eb);          // Gray-200
const juce::Colour Colors::Light::divider = juce::Colour(0xffd1d5db);         // Gray-300

const juce::Colour Colors::Light::text = juce::Colour(0xff111827);            // Gray-900
const juce::Colour Colors::Light::textSecondary = juce::Colour(0xff6b7280);   // Gray-500
const juce::Colour Colors::Light::textDisabled = juce::Colour(0xff9ca3af);    // Gray-400

// Dark Theme Colors - True dark with good contrast
const juce::Colour Colors::Dark::background = juce::Colour(0xff0f172a);       // Slate-900
const juce::Colour Colors::Dark::surface = juce::Colour(0xff1e293b);          // Slate-800
const juce::Colour Colors::Dark::surfaceHover = juce::Colour(0xff334155);     // Slate-700
const juce::Colour Colors::Dark::border = juce::Colour(0xff475569);           // Slate-600
const juce::Colour Colors::Dark::divider = juce::Colour(0xff64748b);          // Slate-500

const juce::Colour Colors::Dark::text = juce::Colour(0xfff8fafc);             // Slate-50
const juce::Colour Colors::Dark::textSecondary = juce::Colour(0xff94a3b8);    // Slate-400
const juce::Colour Colors::Dark::textDisabled = juce::Colour(0xff64748b);     // Slate-500

// Audio-specific colors
const juce::Colour Colors::waveform = juce::Colour(0xff3b82f6);       // Blue-500
const juce::Colour Colors::peakMeter = juce::Colour(0xff10b981);      // Green-500
const juce::Colour Colors::clipping = juce::Colour(0xffef4444);       // Red-500

//==============================================================================
// TYPOGRAPHY IMPLEMENTATIONS
//==============================================================================

juce::Font Typography::getFont(float size, Weight weight)
{
    // Use modern JUCE FontOptions constructor
    juce::Font font(juce::FontOptions().withHeight(size));

    // Apply weight using style flags
    switch (weight)
    {
        case Weight::Light:
            // Light style - no specific flag, use default
            break;
        case Weight::Regular:
            // Default
            break;
        case Weight::SemiBold:
            // SemiBold - use bold flag
            font = font.withStyle(juce::Font::bold);
            break;
        case Weight::Bold:
            font = font.withStyle(juce::Font::bold);
            break;
    }

    return font;
}

juce::Font Typography::getHeading1()
{
    return getFont(h1, Weight::Bold);
}

juce::Font Typography::getHeading2()
{
    return getFont(h2, Weight::SemiBold);
}

juce::Font Typography::getHeading3()
{
    return getFont(h3, Weight::SemiBold);
}

juce::Font Typography::getBody()
{
    return getFont(body1, Weight::Regular);
}

juce::Font Typography::getCaption()
{
    return getFont(caption, Weight::Regular);
}

//==============================================================================
// SHADOWS IMPLEMENTATIONS
//==============================================================================

const juce::DropShadow Shadows::none = juce::DropShadow(juce::Colours::transparentBlack, 0, {0, 0});

const juce::DropShadow Shadows::sm = juce::DropShadow(
    juce::Colours::black.withAlpha(0.1f), 4, {0, 1}
);

const juce::DropShadow Shadows::md = juce::DropShadow(
    juce::Colours::black.withAlpha(0.15f), 8, {0, 2}
);

const juce::DropShadow Shadows::lg = juce::DropShadow(
    juce::Colours::black.withAlpha(0.2f), 16, {0, 4}
);

const juce::DropShadow Shadows::xl = juce::DropShadow(
    juce::Colours::black.withAlpha(0.25f), 24, {0, 8}
);

//==============================================================================
// ANIMATION IMPLEMENTATIONS
//==============================================================================

float Animation::applyEasing(float t, Easing easing)
{
    // Clamp input to [0, 1]
    t = juce::jlimit(0.0f, 1.0f, t);

    switch (easing)
    {
        case Easing::Linear:
            return t;

        case Easing::EaseIn:
            return t * t;

        case Easing::EaseOut:
            return t * (2.0f - t);

        case Easing::EaseInOut:
            return t < 0.5f
                ? 2.0f * t * t
                : -1.0f + (4.0f - 2.0f * t) * t;

        case Easing::Spring:
        {
            // Simple spring approximation
            float s = 1.70158f;
            return (t - 1.0f) * (t - 1.0f) * ((s + 1.0f) * (t - 1.0f) + s) + 1.0f;
        }

        default:
            return t;
    }
}

//==============================================================================
// BREAKPOINTS IMPLEMENTATIONS
//==============================================================================

Breakpoints::Size Breakpoints::getBreakpoint(int width)
{
    if (width < xs) return Size::XS;
    if (width < sm) return Size::SM;
    if (width < md) return Size::MD;
    if (width < lg) return Size::LG;
    return Size::XL;
}

} // namespace Design
} // namespace TSS
