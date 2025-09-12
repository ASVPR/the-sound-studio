/*
  ==============================================================================

    ResponsiveUIHelper.h
    Created: 12 Sep 2025
    Author:  The Sound Studio Team
    
    Centralized responsive UI helper for consistent scaling across all components

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class ResponsiveUIHelper
{
public:
    // Screen size breakpoints
    enum ScreenSize
    {
        SMALL,   // < 1280 width
        MEDIUM,  // 1280-1920 width  
        LARGE,   // 1920-2560 width
        XLARGE   // > 2560 width
    };
    
    // Calculate optimal scale factor based on window dimensions
    static float calculateOptimalScale(int windowWidth, int windowHeight, 
                                      int baseWidth = 1920, int baseHeight = 1740)
    {
        float widthScale = windowWidth / (float)baseWidth;
        float heightScale = windowHeight / (float)baseHeight;
        float optimalScale = jmin(widthScale, heightScale);
        
        // Apply limits based on screen size
        ScreenSize size = getScreenSize(windowWidth);
        switch (size)
        {
            case SMALL:
                return jlimit(0.5f, 0.8f, optimalScale);
            case MEDIUM:
                return jlimit(0.7f, 1.0f, optimalScale);
            case LARGE:
                return jlimit(0.9f, 1.3f, optimalScale);
            case XLARGE:
                return jlimit(1.0f, 2.0f, optimalScale);
        }
        return optimalScale;
    }
    
    // Get current screen size category
    static ScreenSize getScreenSize(int windowWidth)
    {
        if (windowWidth < 1280) return SMALL;
        if (windowWidth < 1920) return MEDIUM;
        if (windowWidth < 2560) return LARGE;
        return XLARGE;
    }
    
    // Helper to scale a rectangle
    static Rectangle<int> scaleRectangle(const Rectangle<int>& original, float scaleFactor)
    {
        return Rectangle<int>(
            roundToInt(original.getX() * scaleFactor),
            roundToInt(original.getY() * scaleFactor),
            roundToInt(original.getWidth() * scaleFactor),
            roundToInt(original.getHeight() * scaleFactor)
        );
    }
    
    // Helper to scale font size
    static float scaleFontSize(float baseSize, float scaleFactor)
    {
        // Non-linear scaling for better readability
        if (scaleFactor < 0.7f)
            return baseSize * (0.7f + (scaleFactor - 0.5f) * 0.5f);
        else if (scaleFactor > 1.3f)
            return baseSize * (1.3f + (scaleFactor - 1.3f) * 0.7f);
        else
            return baseSize * scaleFactor;
    }
    
    // Helper for responsive margins/padding
    static int getResponsiveMargin(float scaleFactor, int baseMargin = 10)
    {
        return jmax(2, roundToInt(baseMargin * scaleFactor));
    }
    
    // Helper for responsive spacing
    static int getResponsiveSpacing(float scaleFactor, int baseSpacing = 8)
    {
        return jmax(2, roundToInt(baseSpacing * scaleFactor));
    }
};

// Base class for responsive components
class ResponsiveComponent : public Component
{
public:
    ResponsiveComponent() {}
    virtual ~ResponsiveComponent() {}
    
    virtual void setScale(float factor)
    {
        if (scaleFactor != factor)
        {
            scaleFactor = factor;
            updateScaledValues();
            resized();
            repaint();
        }
    }
    
    float getScale() const { return scaleFactor; }
    
protected:
    float scaleFactor = 1.0f;
    
    // Override this to update any cached scaled values
    virtual void updateScaledValues() {}
    
    // Helper to scale a value
    int scaled(int value) const
    {
        return roundToInt(value * scaleFactor);
    }
    
    float scaled(float value) const
    {
        return value * scaleFactor;
    }
    
    // Helper to scale bounds
    Rectangle<int> scaledBounds(int x, int y, int w, int h) const
    {
        return Rectangle<int>(scaled(x), scaled(y), scaled(w), scaled(h));
    }
};