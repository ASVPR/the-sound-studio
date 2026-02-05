/*
  ==============================================================================

    EnhancedTooltip.h
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
    Enhanced tooltip with rich text, icons, and animations.
*/
class EnhancedTooltipWindow : public juce::Component,
                              public juce::Timer
{
public:
    EnhancedTooltipWindow()
        : opacity(0.0f)
        , targetOpacity(0.0f)
    {
        setAlwaysOnTop(true);
        setOpaque(false);
        startTimerHz(60);  // 60 FPS animation
    }

    ~EnhancedTooltipWindow() override
    {
        stopTimer();
    }

    void displayTip(const juce::Point<int>& position,
                   const juce::String& title,
                   const juce::String& description,
                   const juce::String& shortcut = juce::String())
    {
        tooltipTitle = title;
        tooltipDescription = description;
        tooltipShortcut = shortcut;

        // Calculate size
        auto titleFont = Design::Typography::getFont(Design::Typography::body1, Design::Typography::Weight::SemiBold);
        auto descFont = Design::Typography::getFont(Design::Typography::caption, Design::Typography::Weight::Regular);
        auto shortcutFont = Design::Typography::getFont(Design::Typography::caption, Design::Typography::Weight::Regular);

        int width = 250;
        int height = Design::Spacing::md;  // Top padding

        // Title height
        if (tooltipTitle.isNotEmpty())
        {
            height += static_cast<int>(titleFont.getHeight()) + Design::Spacing::xs;
        }

        // Description height (word-wrapped)
        if (tooltipDescription.isNotEmpty())
        {
            juce::TextLayout layout;
            layout.createLayout(tooltipDescription, descFont, static_cast<float>(width - Design::Spacing::md * 2));
            height += static_cast<int>(layout.getHeight()) + Design::Spacing::xs;
        }

        // Shortcut height
        if (tooltipShortcut.isNotEmpty())
        {
            height += static_cast<int>(shortcutFont.getHeight()) + Design::Spacing::sm;
        }

        height += Design::Spacing::md;  // Bottom padding

        // Position tooltip
        juce::Rectangle<int> bounds(position.x, position.y + 20, width, height);

        // Adjust if off-screen
        auto screenBounds = juce::Desktop::getInstance().getDisplays()
            .getPrimaryDisplay()->userArea;

        if (bounds.getRight() > screenBounds.getRight())
            bounds.setX(screenBounds.getRight() - width);

        if (bounds.getBottom() > screenBounds.getBottom())
            bounds.setY(position.y - height - 10);

        if (bounds.getX() < screenBounds.getX())
            bounds.setX(screenBounds.getX());

        if (bounds.getY() < screenBounds.getY())
            bounds.setY(screenBounds.getY());

        setBounds(bounds);
        setVisible(true);
        targetOpacity = 1.0f;
        addToDesktop(juce::ComponentPeer::windowIsTemporary |
                     juce::ComponentPeer::windowIgnoresKeyPresses);
    }

    void hideTip()
    {
        targetOpacity = 0.0f;
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();

        // Apply opacity
        if (opacity <= 0.0f)
            return;

        g.setOpacity(opacity);

        // Draw shadow
        juce::DropShadow shadow(
            juce::Colours::black.withAlpha(0.3f * opacity),
            12, {0, 4}
        );
        juce::Path path;
        path.addRoundedRectangle(bounds, Design::BorderRadius::md);
        shadow.drawForPath(g, path);

        // Draw background
        g.setColour(Design::Colors::Dark::surface);
        g.fillRoundedRectangle(bounds, Design::BorderRadius::md);

        // Draw border
        g.setColour(Design::Colors::Dark::border);
        g.drawRoundedRectangle(bounds.reduced(1.0f), Design::BorderRadius::md, 1.0f);

        // Draw content
        auto contentBounds = bounds.reduced(Design::Spacing::md, Design::Spacing::md);
        float y = contentBounds.getY();

        // Title
        if (tooltipTitle.isNotEmpty())
        {
            auto titleFont = Design::Typography::getFont(Design::Typography::body1,
                                                        Design::Typography::Weight::SemiBold);
            g.setColour(Design::Colors::Dark::text);
            g.setFont(titleFont);

            juce::Rectangle<float> titleBounds(
                contentBounds.getX(),
                y,
                contentBounds.getWidth(),
                titleFont.getHeight()
            );
            g.drawText(tooltipTitle, titleBounds, juce::Justification::topLeft);
            y += titleFont.getHeight() + Design::Spacing::xs;
        }

        // Description
        if (tooltipDescription.isNotEmpty())
        {
            auto descFont = Design::Typography::getFont(Design::Typography::caption,
                                                       Design::Typography::Weight::Regular);
            g.setColour(Design::Colors::Dark::textSecondary);
            g.setFont(descFont);

            juce::TextLayout layout;
            layout.createLayout(tooltipDescription, descFont, contentBounds.getWidth());

            juce::Rectangle<float> descBounds(
                contentBounds.getX(),
                y,
                contentBounds.getWidth(),
                layout.getHeight()
            );
            layout.draw(g, descBounds);
            y += layout.getHeight() + Design::Spacing::xs;
        }

        // Shortcut
        if (tooltipShortcut.isNotEmpty())
        {
            auto shortcutFont = Design::Typography::getFont(Design::Typography::caption,
                                                           Design::Typography::Weight::Regular);
            g.setColour(Design::Colors::primary.withAlpha(0.8f));
            g.setFont(shortcutFont);

            juce::Rectangle<float> shortcutBounds(
                contentBounds.getX(),
                y,
                contentBounds.getWidth(),
                shortcutFont.getHeight()
            );
            g.drawText(tooltipShortcut, shortcutBounds, juce::Justification::topRight);
        }
    }

    void timerCallback() override
    {
        bool needsRepaint = false;

        // Animate opacity
        if (std::abs(opacity - targetOpacity) > 0.01f)
        {
            opacity += (targetOpacity - opacity) * 0.2f;
            needsRepaint = true;
        }
        else if (targetOpacity == 0.0f && opacity < 0.01f)
        {
            setVisible(false);
            removeFromDesktop();
        }

        if (needsRepaint)
            repaint();
    }

private:
    juce::String tooltipTitle;
    juce::String tooltipDescription;
    juce::String tooltipShortcut;
    float opacity;
    float targetOpacity;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EnhancedTooltipWindow)
};

//==============================================================================
/**
    Tooltip manager for the application.
    Handles showing/hiding tooltips with proper delays.
*/
class TooltipManager : public juce::Timer
{
public:
    static TooltipManager& getInstance()
    {
        static TooltipManager instance;
        return instance;
    }

    void showTooltip(const juce::Component* component,
                    const juce::String& title,
                    const juce::String& description,
                    const juce::String& shortcut = juce::String())
    {
        if (!component || !component->isVisible())
            return;

        currentComponent = component;
        pendingTitle = title;
        pendingDescription = description;
        pendingShortcut = shortcut;

        // Start timer for delay
        startTimer(showDelay);
    }

    void hideTooltip()
    {
        stopTimer();
        currentComponent = nullptr;

        if (tooltipWindow != nullptr)
        {
            tooltipWindow->hideTip();
        }
    }

    void setShowDelay(int milliseconds)
    {
        showDelay = milliseconds;
    }

    void timerCallback() override
    {
        stopTimer();

        if (currentComponent == nullptr || !currentComponent->isVisible())
            return;

        // Get mouse position
        auto mousePos = juce::Desktop::getInstance().getMainMouseSource().getScreenPosition().toInt();

        // Create tooltip window if needed
        if (tooltipWindow == nullptr)
        {
            tooltipWindow = std::make_unique<EnhancedTooltipWindow>();
        }

        // Display tooltip
        tooltipWindow->displayTip(mousePos, pendingTitle, pendingDescription, pendingShortcut);
    }

private:
    TooltipManager()
        : showDelay(700)  // 700ms default delay
    {
    }

    ~TooltipManager()
    {
        tooltipWindow = nullptr;
    }

    const juce::Component* currentComponent = nullptr;
    juce::String pendingTitle;
    juce::String pendingDescription;
    juce::String pendingShortcut;
    int showDelay;
    std::unique_ptr<EnhancedTooltipWindow> tooltipWindow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TooltipManager)
};

//==============================================================================
/**
    Component that provides enhanced tooltip support.
    Inherit from this to add rich tooltips to your components.
*/
class TooltipProvider
{
public:
    virtual ~TooltipProvider() = default;

    void setTooltip(const juce::String& title,
                   const juce::String& description,
                   const juce::String& shortcut = juce::String())
    {
        tooltipTitle = title;
        tooltipDescription = description;
        tooltipShortcut = shortcut;
    }

    const juce::String& getTooltipTitle() const { return tooltipTitle; }
    const juce::String& getTooltipDescription() const { return tooltipDescription; }
    const juce::String& getTooltipShortcut() const { return tooltipShortcut; }

protected:
    void handleMouseEnter(const juce::Component* comp)
    {
        if (tooltipTitle.isNotEmpty() || tooltipDescription.isNotEmpty())
        {
            TooltipManager::getInstance().showTooltip(
                comp,
                tooltipTitle,
                tooltipDescription,
                tooltipShortcut
            );
        }
    }

    void handleMouseExit()
    {
        TooltipManager::getInstance().hideTooltip();
    }

private:
    juce::String tooltipTitle;
    juce::String tooltipDescription;
    juce::String tooltipShortcut;
};

//==============================================================================
/**
    Helper mixin to add tooltip support to existing components.
*/
template<typename BaseComponentType>
class WithTooltip : public BaseComponentType, public TooltipProvider
{
public:
    template<typename... Args>
    WithTooltip(Args&&... args) : BaseComponentType(std::forward<Args>(args)...)
    {
    }

    void mouseEnter(const juce::MouseEvent& e) override
    {
        handleMouseEnter(this);
        BaseComponentType::mouseEnter(e);
    }

    void mouseExit(const juce::MouseEvent& e) override
    {
        handleMouseExit();
        BaseComponentType::mouseExit(e);
    }
};

} // namespace UI
} // namespace TSS
