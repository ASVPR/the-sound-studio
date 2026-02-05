/*
  ==============================================================================

    VectorGraphicsManager.cpp
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#include "VectorGraphicsManager.h"

VectorGraphicsManager::VectorGraphicsManager()
{
}

VectorGraphicsManager::~VectorGraphicsManager()
{
}

void VectorGraphicsManager::drawElement(Graphics& g, VectorElementType type, Rectangle<float> bounds, 
                                      Colour primaryColour, Colour secondaryColour, 
                                      float cornerRadius, float strokeWidth)
{
    switch (type)
    {
        case VectorElementType::BUTTON_NORMAL:
            drawModernButton(g, bounds, "", false, false, primaryColour);
            break;
            
        case VectorElementType::BUTTON_HIGHLIGHTED:
            drawModernButton(g, bounds, "", true, false, primaryColour);
            break;
            
        case VectorElementType::BUTTON_PRESSED:
            drawModernButton(g, bounds, "", false, true, primaryColour);
            break;
            
        case VectorElementType::BACKGROUND_PANEL:
            drawGlassPanel(g, bounds, primaryColour, cornerRadius);
            break;
            
        case VectorElementType::PROGRESS_BAR:
            drawProgressBar(g, bounds, 0.5f, primaryColour);
            break;
            
        case VectorElementType::ICON_PLAY:
            drawPlayIcon(g, bounds, primaryColour);
            break;
            
        case VectorElementType::ICON_STOP:
            drawStopIcon(g, bounds, primaryColour);
            break;
            
        case VectorElementType::ICON_SETTINGS:
            drawSettingsIcon(g, bounds, primaryColour);
            break;
            
        default:
            // Draw a simple rounded rectangle as fallback
            g.setColour(primaryColour);
            g.fillRoundedRectangle(bounds, cornerRadius);
            break;
    }
}

ScreenSize VectorGraphicsManager::getScreenSize(int width)
{
    if (width < 800)
        return ScreenSize::SMALL;
    else if (width < 1200)
        return ScreenSize::MEDIUM;
    else if (width < 1600)
        return ScreenSize::LARGE;
    else
        return ScreenSize::XLARGE;
}

float VectorGraphicsManager::getScaleFactor(ScreenSize size)
{
    switch (size)
    {
        case ScreenSize::SMALL:  return 0.8f;
        case ScreenSize::MEDIUM: return 1.0f;
        case ScreenSize::LARGE:  return 1.2f;
        case ScreenSize::XLARGE: return 1.4f;
        default: return 1.0f;
    }
}

Font VectorGraphicsManager::getResponsiveFont(ScreenSize size, float baseSize)
{
    float scaleFactor = getScaleFactor(size);
    return Font(baseSize * scaleFactor);
}

float VectorGraphicsManager::getResponsiveMargin(ScreenSize size, float baseMargin)
{
    float scaleFactor = getScaleFactor(size);
    return baseMargin * scaleFactor;
}

void VectorGraphicsManager::drawPlayIcon(Graphics& g, Rectangle<float> bounds, Colour colour)
{
    g.setColour(colour);
    
    Path playPath;
    float centerX = bounds.getCentreX();
    float centerY = bounds.getCentreY();
    float size = jmin(bounds.getWidth(), bounds.getHeight()) * 0.6f;
    
    // Create triangle pointing right
    playPath.addTriangle(centerX - size/3, centerY - size/2,
                        centerX - size/3, centerY + size/2,
                        centerX + size/2, centerY);
    
    g.fillPath(playPath);
}

void VectorGraphicsManager::drawStopIcon(Graphics& g, Rectangle<float> bounds, Colour colour)
{
    g.setColour(colour);
    
    float centerX = bounds.getCentreX();
    float centerY = bounds.getCentreY();
    float size = jmin(bounds.getWidth(), bounds.getHeight()) * 0.6f;
    
    Rectangle<float> stopRect(centerX - size/2, centerY - size/2, size, size);
    g.fillRoundedRectangle(stopRect, 2.0f);
}

void VectorGraphicsManager::drawPauseIcon(Graphics& g, Rectangle<float> bounds, Colour colour)
{
    g.setColour(colour);
    
    float centerX = bounds.getCentreX();
    float centerY = bounds.getCentreY();
    float size = jmin(bounds.getWidth(), bounds.getHeight()) * 0.6f;
    float barWidth = size * 0.25f;
    float spacing = size * 0.2f;
    
    Rectangle<float> leftBar(centerX - spacing/2 - barWidth, centerY - size/2, barWidth, size);
    Rectangle<float> rightBar(centerX + spacing/2, centerY - size/2, barWidth, size);
    
    g.fillRoundedRectangle(leftBar, 1.0f);
    g.fillRoundedRectangle(rightBar, 1.0f);
}

void VectorGraphicsManager::drawSettingsIcon(Graphics& g, Rectangle<float> bounds, Colour colour)
{
    g.setColour(colour);
    
    Path gearPath;
    float centerX = bounds.getCentreX();
    float centerY = bounds.getCentreY();
    float outerRadius = jmin(bounds.getWidth(), bounds.getHeight()) * 0.4f;
    float innerRadius = outerRadius * 0.6f;
    float toothHeight = outerRadius * 0.2f;
    
    // Create gear shape with 8 teeth
    for (int i = 0; i < 8; ++i)
    {
        float angle1 = i * juce::MathConstants<float>::twoPi / 8.0f - juce::MathConstants<float>::pi / 16.0f;
        float angle2 = angle1 + juce::MathConstants<float>::twoPi / 16.0f;
        float angle3 = angle2 + juce::MathConstants<float>::twoPi / 16.0f;
        
        if (i == 0)
        {
            gearPath.startNewSubPath(centerX + std::cos(angle1) * innerRadius, 
                                   centerY + std::sin(angle1) * innerRadius);
        }
        
        gearPath.lineTo(centerX + std::cos(angle1) * innerRadius, centerY + std::sin(angle1) * innerRadius);
        gearPath.lineTo(centerX + std::cos(angle1) * outerRadius, centerY + std::sin(angle1) * outerRadius);
        gearPath.lineTo(centerX + std::cos(angle3) * outerRadius, centerY + std::sin(angle3) * outerRadius);
        gearPath.lineTo(centerX + std::cos(angle3) * innerRadius, centerY + std::sin(angle3) * innerRadius);
    }
    
    gearPath.closeSubPath();
    
    // Add center hole
    gearPath.addEllipse(centerX - innerRadius * 0.4f, centerY - innerRadius * 0.4f, 
                       innerRadius * 0.8f, innerRadius * 0.8f);
    gearPath.setUsingNonZeroWinding(false);
    
    g.fillPath(gearPath);
}

void VectorGraphicsManager::drawCloseIcon(Graphics& g, Rectangle<float> bounds, Colour colour)
{
    g.setColour(colour);
    
    float centerX = bounds.getCentreX();
    float centerY = bounds.getCentreY();
    float size = jmin(bounds.getWidth(), bounds.getHeight()) * 0.6f;
    float thickness = size * 0.15f;
    
    Path closePath;
    
    // Draw X shape
    closePath.addRoundedRectangle(centerX - size/2, centerY - thickness/2, size, thickness, thickness/2);
    
    Path rotatedPath = closePath;
    rotatedPath.applyTransform(AffineTransform::rotation(juce::MathConstants<float>::pi / 4.0f, centerX, centerY));
    
    closePath.applyTransform(AffineTransform::rotation(-juce::MathConstants<float>::pi / 4.0f, centerX, centerY));
    
    g.fillPath(closePath);
    g.fillPath(rotatedPath);
}

void VectorGraphicsManager::drawModernButton(Graphics& g, Rectangle<float> bounds, 
                                           const String& text, bool isHighlighted, bool isPressed,
                                           Colour baseColour)
{
    float cornerRadius = bounds.getHeight() * 0.1f;
    
    // Create color variations based on state
    Colour topColour = baseColour;
    Colour bottomColour = baseColour.darker(0.2f);
    
    if (isHighlighted)
    {
        topColour = baseColour.brighter(0.1f);
        bottomColour = baseColour.darker(0.1f);
    }
    
    if (isPressed)
    {
        topColour = baseColour.darker(0.1f);
        bottomColour = baseColour.darker(0.3f);
        bounds = bounds.reduced(1.0f);
    }
    else if (!isPressed)
    {
        // Draw outer glow for unpressed state
        drawOuterGlow(g, bounds.expanded(2.0f), cornerRadius, baseColour.withAlpha(0.3f), 3.0f);
    }
    
    // Draw gradient background
    drawRoundedRectWithGradient(g, bounds, cornerRadius, topColour, bottomColour);
    
    // Draw inner highlight
    if (!isPressed)
    {
        ColourGradient highlight = ColourGradient::vertical(
            Colours::white.withAlpha(0.3f), bounds.getY(),
            Colours::transparentWhite, bounds.getY() + bounds.getHeight() * 0.5f);
        
        g.setGradientFill(highlight);
        g.fillRoundedRectangle(bounds.reduced(1.0f).withHeight(bounds.getHeight() * 0.5f), cornerRadius - 1.0f);
    }
    
    // Draw border
    g.setColour(baseColour.darker(0.4f));
    g.drawRoundedRectangle(bounds, cornerRadius, 1.0f);
    
    // Draw text if provided
    if (text.isNotEmpty())
    {
        g.setColour(Colours::white);
        g.setFont(Font(bounds.getHeight() * 0.4f, Font::bold));
        g.drawText(text, bounds, Justification::centred);
    }
}

void VectorGraphicsManager::drawGlassPanel(Graphics& g, Rectangle<float> bounds, 
                                         Colour baseColour, float cornerRadius)
{
    // Draw background with transparency
    g.setColour(baseColour);
    g.fillRoundedRectangle(bounds, cornerRadius);
    
    // Draw glass highlight
    ColourGradient glassGradient = ColourGradient::vertical(
        Colours::white.withAlpha(0.1f), bounds.getY(),
        Colours::transparentWhite, bounds.getY() + bounds.getHeight() * 0.7f);
    
    g.setGradientFill(glassGradient);
    g.fillRoundedRectangle(bounds.reduced(1.0f), cornerRadius - 1.0f);
    
    // Draw subtle border
    g.setColour(Colours::white.withAlpha(0.2f));
    g.drawRoundedRectangle(bounds, cornerRadius, 1.0f);
}

void VectorGraphicsManager::drawProgressBar(Graphics& g, Rectangle<float> bounds, 
                                          float progress, Colour fillColour)
{
    float cornerRadius = bounds.getHeight() * 0.5f;
    
    // Draw background track
    g.setColour(Colour(0x33ffffff));
    g.fillRoundedRectangle(bounds, cornerRadius);
    
    // Draw fill
    if (progress > 0.0f)
    {
        Rectangle<float> fillBounds = bounds.withWidth(bounds.getWidth() * jlimit(0.0f, 1.0f, progress));
        
        ColourGradient fillGradient = ColourGradient::vertical(
            fillColour.brighter(0.2f), fillBounds.getY(),
            fillColour.darker(0.2f), fillBounds.getBottom());
        
        g.setGradientFill(fillGradient);
        g.fillRoundedRectangle(fillBounds, cornerRadius);
        
        // Add highlight
        g.setColour(Colours::white.withAlpha(0.4f));
        g.fillRoundedRectangle(fillBounds.reduced(1.0f).withHeight(fillBounds.getHeight() * 0.3f), 
                             cornerRadius - 1.0f);
    }
}

void VectorGraphicsManager::drawRoundedRectWithGradient(Graphics& g, Rectangle<float> bounds, 
                                                      float cornerRadius, 
                                                      Colour topColour, Colour bottomColour)
{
    ColourGradient gradient = ColourGradient::vertical(topColour, bounds.getY(), 
                                                     bottomColour, bounds.getBottom());
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(bounds, cornerRadius);
}

void VectorGraphicsManager::drawOuterGlow(Graphics& g, Rectangle<float> bounds, 
                                        float cornerRadius, Colour glowColour, float glowSize)
{
    for (int i = 0; i < (int)glowSize; ++i)
    {
        float alpha = 0.15f * (1.0f - (float)i / glowSize);
        g.setColour(glowColour.withAlpha(alpha));
        g.drawRoundedRectangle(bounds.expanded((float)i), cornerRadius + i, 1.0f);
    }
}