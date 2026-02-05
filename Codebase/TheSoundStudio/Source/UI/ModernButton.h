/*
  ==============================================================================

    ModernButton.h
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DesignSystem.h"

namespace TSS {
namespace UI {

//==============================================================================
/**
    Modern button with animations, states, and accessibility.
*/
class ModernButton : public juce::Button
{
public:
    enum class Variant
    {
        Primary,    // Filled, prominent
        Secondary,  // Outlined
        Ghost,      // Transparent with hover
        Danger      // Red/warning style
    };

    enum class Size
    {
        Small,
        Medium,
        Large
    };

    ModernButton(const juce::String& buttonText = juce::String())
        : juce::Button(buttonText)
        , variant(Variant::Primary)
        , size(Size::Medium)
        , hoverAmount(0.0f)
        , pressAmount(0.0f)
    {
        setWantsKeyboardFocus(true);
        setMouseCursor(juce::MouseCursor::PointingHandCursor);

        // Start animation timer
        startTimerHz(60);  // 60 FPS
    }

    ~ModernButton() override
    {
        stopTimer();
    }

    // Configuration
    void setVariant(Variant v) { variant = v; repaint(); }
    void setSize(Size s) { size = s; repaint(); }
    void setIcon(const juce::Path& iconPath) { icon = iconPath; hasIcon = true; repaint(); }

    // Button overrides
    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto bounds = getLocalBounds().toFloat();

        // Get colors based on variant and state
        juce::Colour bgColor, textColor, borderColor;
        getColors(bgColor, textColor, borderColor);

        // Apply hover and press animations
        if (shouldDrawButtonAsHighlighted || isMouseOver())
        {
            bgColor = bgColor.brighter(0.1f * hoverAmount);
        }

        if (shouldDrawButtonAsDown)
        {
            bgColor = bgColor.darker(0.05f * pressAmount);
        }

        // Draw shadow for elevated variants
        if (variant == Variant::Primary)
        {
            juce::DropShadow shadow(
                juce::Colours::black.withAlpha(0.15f * hoverAmount),
                8, {0, 2}
            );
            juce::Path buttonPath;
            buttonPath.addRoundedRectangle(bounds, getCornerRadius());
            shadow.drawForPath(g, buttonPath);
        }

        // Draw background
        if (variant != Variant::Ghost)
        {
            g.setColour(bgColor);
            g.fillRoundedRectangle(bounds, getCornerRadius());
        }

        // Draw border for outlined variants
        if (variant == Variant::Secondary || variant == Variant::Ghost)
        {
            g.setColour(borderColor);
            g.drawRoundedRectangle(bounds.reduced(1.0f), getCornerRadius(), 2.0f);
        }

        // Draw content (icon + text)
        auto contentBounds = bounds.reduced(getHorizontalPadding(), getVerticalPadding());

        if (hasIcon && !getButtonText().isEmpty())
        {
            // Icon + Text layout
            float iconSize = getIconSize();
            float gap = Design::Spacing::xs;

            auto iconBounds = contentBounds.removeFromLeft(iconSize);
            iconBounds = iconBounds.withSizeKeepingCentre(iconSize, iconSize);

            g.setColour(textColor);
            g.fillPath(icon, icon.getTransformToScaleToFit(iconBounds, true));

            contentBounds.removeFromLeft(gap);
            g.setColour(textColor);
            g.setFont(getFont());
            g.drawText(getButtonText(), contentBounds, juce::Justification::centredLeft);
        }
        else if (hasIcon)
        {
            // Icon only
            float iconSize = getIconSize();
            auto iconBounds = contentBounds.withSizeKeepingCentre(iconSize, iconSize);

            g.setColour(textColor);
            g.fillPath(icon, icon.getTransformToScaleToFit(iconBounds, true));
        }
        else
        {
            // Text only
            g.setColour(textColor);
            g.setFont(getFont());
            g.drawText(getButtonText(), contentBounds, juce::Justification::centred);
        }

        // Draw focus ring for accessibility
        if (hasKeyboardFocus(true))
        {
            g.setColour(Design::Colors::primary.withAlpha(0.5f));
            g.drawRoundedRectangle(bounds.reduced(2.0f), getCornerRadius(), 2.0f);
        }
    }

    // Mouse events for animations
    void mouseEnter(const juce::MouseEvent&) override
    {
        targetHoverAmount = 1.0f;
    }

    void mouseExit(const juce::MouseEvent&) override
    {
        targetHoverAmount = 0.0f;
    }

    void mouseDown(const juce::MouseEvent& e) override
    {
        targetPressAmount = 1.0f;
        juce::Button::mouseDown(e);
    }

    void mouseUp(const juce::MouseEvent& e) override
    {
        targetPressAmount = 0.0f;
        juce::Button::mouseUp(e);
    }

    // Timer for smooth animations
    void timerCallback() override
    {
        bool needsRepaint = false;

        // Animate hover
        if (std::abs(hoverAmount - targetHoverAmount) > 0.01f)
        {
            hoverAmount += (targetHoverAmount - hoverAmount) * 0.15f;
            needsRepaint = true;
        }

        // Animate press
        if (std::abs(pressAmount - targetPressAmount) > 0.01f)
        {
            pressAmount += (targetPressAmount - pressAmount) * 0.3f;
            needsRepaint = true;
        }

        if (needsRepaint)
            repaint();
    }

    // Keyboard accessibility
    bool keyPressed(const juce::KeyPress& key) override
    {
        if (key == juce::KeyPress::spaceKey || key == juce::KeyPress::returnKey)
        {
            triggerClick();
            return true;
        }
        return juce::Button::keyPressed(key);
    }

private:
    void getColors(juce::Colour& bg, juce::Colour& text, juce::Colour& border)
    {
        bool isDark = false;  // TODO: Get from theme system

        switch (variant)
        {
            case Variant::Primary:
                bg = Design::Colors::primary;
                text = juce::Colours::white;
                border = Design::Colors::primary;
                break;

            case Variant::Secondary:
                bg = juce::Colours::transparentBlack;
                text = isDark ? Design::Colors::Dark::text : Design::Colors::Light::text;
                border = isDark ? Design::Colors::Dark::border : Design::Colors::Light::border;
                break;

            case Variant::Ghost:
                bg = juce::Colours::transparentBlack;
                text = isDark ? Design::Colors::Dark::text : Design::Colors::Light::text;
                border = juce::Colours::transparentBlack;
                break;

            case Variant::Danger:
                bg = Design::Colors::error;
                text = juce::Colours::white;
                border = Design::Colors::error;
                break;
        }

        if (!isEnabled())
        {
            bg = bg.withAlpha(0.5f);
            text = text.withAlpha(0.5f);
            border = border.withAlpha(0.5f);
        }
    }

    float getCornerRadius() const
    {
        switch (size)
        {
            case Size::Small:  return Design::BorderRadius::sm;
            case Size::Medium: return Design::BorderRadius::md;
            case Size::Large:  return Design::BorderRadius::lg;
            default:           return Design::BorderRadius::md;
        }
    }

    int getHorizontalPadding() const
    {
        switch (size)
        {
            case Size::Small:  return Design::Spacing::sm;
            case Size::Medium: return Design::Spacing::md;
            case Size::Large:  return Design::Spacing::lg;
            default:           return Design::Spacing::md;
        }
    }

    int getVerticalPadding() const
    {
        switch (size)
        {
            case Size::Small:  return Design::Spacing::xs;
            case Size::Medium: return Design::Spacing::sm;
            case Size::Large:  return Design::Spacing::md;
            default:           return Design::Spacing::sm;
        }
    }

    juce::Font getFont() const
    {
        switch (size)
        {
            case Size::Small:  return Design::Typography::getFont(Design::Typography::body2);
            case Size::Medium: return Design::Typography::getFont(Design::Typography::body1);
            case Size::Large:  return Design::Typography::getFont(Design::Typography::h4);
            default:           return Design::Typography::getBody();
        }
    }

    float getIconSize() const
    {
        switch (size)
        {
            case Size::Small:  return 16.0f;
            case Size::Medium: return 20.0f;
            case Size::Large:  return 24.0f;
            default:           return 20.0f;
        }
    }

    Variant variant;
    Size size;
    juce::Path icon;
    bool hasIcon = false;

    // Animation state
    float hoverAmount;
    float targetHoverAmount = 0.0f;
    float pressAmount;
    float targetPressAmount = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModernButton)
};

} // namespace UI
} // namespace TSS
