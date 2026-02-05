/*
  ==============================================================================

    DesignSystem.cpp
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#include "DesignSystem.h"

namespace TSS {
namespace Design {

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

const juce::DropShadow Shadows::none = juce::DropShadow(juce::Colours::transparentBlack, 1, {0, 0});

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
