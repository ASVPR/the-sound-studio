/*
  ==============================================================================

    VectorGraphicsManager.h
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

// UI Element Types
enum class VectorElementType
{
    BUTTON_NORMAL,
    BUTTON_HIGHLIGHTED,
    BUTTON_PRESSED,
    BACKGROUND_PANEL,
    PROGRESS_BAR,
    SLIDER_TRACK,
    SLIDER_THUMB,
    CHECKBOX_UNCHECKED,
    CHECKBOX_CHECKED,
    MENU_ITEM,
    ICON_PLAY,
    ICON_STOP,
    ICON_SETTINGS,
    LOGO
};

// Responsive breakpoints
enum class ScreenSize
{
    SMALL,   // < 800px width
    MEDIUM,  // 800-1200px width  
    LARGE,   // 1200-1600px width
    XLARGE   // > 1600px width
};

class VectorGraphicsManager
{
public:
    VectorGraphicsManager();
    ~VectorGraphicsManager();
    
    // Main drawing interface
    static void drawElement(Graphics& g, VectorElementType type, Rectangle<float> bounds, 
                          Colour primaryColour = Colours::grey, 
                          Colour secondaryColour = Colours::white,
                          float cornerRadius = 4.0f,
                          float strokeWidth = 1.0f);
    
    // Responsive utilities
    static ScreenSize getScreenSize(int width);
    static float getScaleFactor(ScreenSize size);
    static Font getResponsiveFont(ScreenSize size, float baseSize = 14.0f);
    static float getResponsiveMargin(ScreenSize size, float baseMargin = 8.0f);
    
    // Icon drawing (vector-based)
    static void drawPlayIcon(Graphics& g, Rectangle<float> bounds, Colour colour);
    static void drawStopIcon(Graphics& g, Rectangle<float> bounds, Colour colour);
    static void drawPauseIcon(Graphics& g, Rectangle<float> bounds, Colour colour);
    static void drawSettingsIcon(Graphics& g, Rectangle<float> bounds, Colour colour);
    static void drawCloseIcon(Graphics& g, Rectangle<float> bounds, Colour colour);
    static void drawChevronLeft(Graphics& g, Rectangle<float> bounds, Colour colour);
    static void drawChevronRight(Graphics& g, Rectangle<float> bounds, Colour colour);
    
    // Complex UI elements
    static void drawModernButton(Graphics& g, Rectangle<float> bounds, 
                               const String& text, bool isHighlighted, bool isPressed,
                               Colour baseColour = Colour(0xff4a90e2));
    
    static void drawGlassPanel(Graphics& g, Rectangle<float> bounds, 
                             Colour baseColour = Colour(0x88000000),
                             float cornerRadius = 8.0f);
    
    static void drawProgressBar(Graphics& g, Rectangle<float> bounds, 
                              float progress, Colour fillColour = Colour(0xff4a90e2));
    
    static void drawVolumeSlider(Graphics& g, Rectangle<float> bounds, 
                               float value, bool isVertical = false);
    
private:
    // Internal drawing helpers
    static void drawRoundedRectWithGradient(Graphics& g, Rectangle<float> bounds, 
                                          float cornerRadius, 
                                          Colour topColour, Colour bottomColour);
    
    static void drawInnerShadow(Graphics& g, Rectangle<float> bounds, 
                              float cornerRadius, float shadowSize = 2.0f);
    
    static void drawOuterGlow(Graphics& g, Rectangle<float> bounds, 
                            float cornerRadius, Colour glowColour, float glowSize = 4.0f);
    
    // Colour schemes
    static ColourGradient createMetallicGradient(Rectangle<float> bounds, Colour baseColour);
    static ColourGradient createGlassGradient(Rectangle<float> bounds, Colour baseColour);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VectorGraphicsManager)
};