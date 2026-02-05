/*
  ==============================================================================

    ResponsiveUIHelper.h
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

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

    // Helper to scale font size with a readable minimum
    static float getReadableFontSize(float baseSize, float scaleFactor, float minSize)
    {
        return jmax(minSize, scaleFontSize(baseSize, scaleFactor));
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

    struct LayoutMetrics
    {
        float scale = 1.0f;
        Rectangle<int> bounds;
    };

    // Calculate a centered, letterboxed layout for a fixed reference size
    static LayoutMetrics getLetterboxLayout(const Rectangle<int>& availableBounds,
                                            float referenceWidth,
                                            float referenceHeight)
    {
        LayoutMetrics metrics;
        if (referenceWidth <= 0.0f || referenceHeight <= 0.0f || availableBounds.isEmpty())
        {
            metrics.scale = 1.0f;
            metrics.bounds = availableBounds;
            return metrics;
        }

        const float scaleX = availableBounds.getWidth() / referenceWidth;
        const float scaleY = availableBounds.getHeight() / referenceHeight;
        metrics.scale = jmin(scaleX, scaleY);

        const int width = roundToInt(referenceWidth * metrics.scale);
        const int height = roundToInt(referenceHeight * metrics.scale);
        const int x = availableBounds.getX() + (availableBounds.getWidth() - width) / 2;
        const int y = availableBounds.getY() + (availableBounds.getHeight() - height) / 2;

        metrics.bounds = { x, y, width, height };
        return metrics;
    }

    // Scale a rectangle from reference coordinates into the letterboxed layout
    static Rectangle<int> scaleRect(const LayoutMetrics& metrics,
                                    float x, float y, float w, float h)
    {
        return Rectangle<int>(roundToInt(metrics.bounds.getX() + x * metrics.scale),
                              roundToInt(metrics.bounds.getY() + y * metrics.scale),
                              roundToInt(w * metrics.scale),
                              roundToInt(h * metrics.scale));
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
