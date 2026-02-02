/*
  ==============================================================================

    CustomLookAndFeel.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"


// CustomComboBox
// need to be subclasses and override multiple functions
// also needs CustomLookAndFeel




class CustomLookAndFeel : public LookAndFeel_V4
{
public:
    CustomLookAndFeel();
    
    ~CustomLookAndFeel() { }
    

    
    // Buttons
    //==============================================================================
    
    void drawButtonBackground (Graphics&g, Button&button, const Colour& backgroundColour,
                               bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)override;

    /** Draws the contents of a standard ToggleButton. */
    virtual void drawToggleButton (Graphics&g, ToggleButton&button,
                                   bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)override;
    
    void drawTickBox (Graphics& g, Component& component,
                                      float x, float y, float w, float h,
                                      const bool ticked,
                                      const bool isEnabled,
                                      const bool shouldDrawButtonAsHighlighted,
                      const bool shouldDrawButtonAsDown) override;
    
    void drawButtonText (Graphics& g, TextButton& button, bool /*shouldDrawButtonAsHighlighted*/, bool /*shouldDrawButtonAsDown*/)override;
    
    Font getTextButtonFont (TextButton&button, int buttonHeight) override;
    
    
    // Progress Bar
    //==============================================================================
    void drawProgressBar (Graphics&, ProgressBar&, int width, int height, double progress, const String& textToShow) override;
    bool isProgressBarOpaque (ProgressBar&) override;
    
    
    // ComboBox & popup
    //==============================================================================
    void drawComboBox (Graphics&g, int width, int height, bool isButtonDown,
                       int buttonX, int buttonY, int buttonW, int buttonH,
                       ComboBox&box) override;
    
    Label* createComboBoxTextBox (ComboBox&box) override;

    void positionComboBoxText (ComboBox&box, Label& label) override;

    Font getComboBoxFont (ComboBox&c) override;
    
    Font getPopupMenuFont() override;
    
    /** Fills the background of a popup menu component. */
    void drawPopupMenuBackground (Graphics&g, int width, int height)override;
    
    virtual void drawPopupMenuItem (Graphics&g, const Rectangle<int>& area,
                                    bool isSeparator, bool isActive, bool isHighlighted,
                                    bool isTicked, bool hasSubMenu,
                                    const String& text,
                                    const String& shortcutKeyText,
                                    const Drawable* icon,
                                    const Colour* textColourToUse) override;
    
    
    
    // Rotary Slider
    //==============================================================================
    void drawRotarySlider (Graphics&, int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle,
                           float rotaryEndAngle, Slider&) override;
    

    Label* createSliderTextBox (Slider& slider) override;


    
    // Tooltip
    //==============================================================================
    void drawTooltip (Graphics&g, const String& text, int width, int height) override;
    
    void fillTextEditorBackground (Graphics&, int width, int height, TextEditor&) override;
    void drawTextEditorOutline (Graphics&, int width, int height, TextEditor&) override;

    
    float scaleFactor = 0.5;
    void setScale(float factor)
    {
        scaleFactor = factor;
    }
    
private:

    std::unique_ptr<Font> fontLight;
    
    
};
