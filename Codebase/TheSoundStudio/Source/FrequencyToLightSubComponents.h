/*
  ==============================================================================

    FrequencyToLightSubComponents.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "CustomLookAndFeel.h"
#include "ProjectManager.h"
#include <memory>

//==============================================================================
/*

 */

class RoundedColourOutputComponent : public Component
{
public:
    RoundedColourOutputComponent() { mainColour = Colours::green; }
    ~RoundedColourOutputComponent() { }
    
    void paint(Graphics&g)
    {
        int yInset = 10;
        int xInset = 10;
        
        // simple bordered rectangle
//        g.setColour(Colours::lightgrey);
//        g.drawRoundedRectangle(0, 0, getWidth(), getHeight(), 10, 6);
        
        // need gradient fill from left to right, blue to red
        ColourGradient gradient(mainColour, 0, 0, mainColour, getWidth(), getHeight(), false);
        
        g.setGradientFill(gradient);
        
//        float fillToXpoint = (getWidth() - (2 * xInset)) * value;
        
        g.fillRoundedRectangle(0, 0, getWidth(), getHeight(), 12);
        
    }
    
    void setColour(Colour newColour) { mainColour = newColour; repaint(); }
    
private:
    Colour mainColour;
    float value = 1.f;
};


class ColourOutputComponent : public Component
{
public:
    ColourOutputComponent();
    ~ColourOutputComponent();
    void paint (Graphics&g) override;
    void setBackgroundColour(Colour newColour);
    
    void setGradientBool(bool should)
    {
        isGradient = should;
    }
    
    void setGradient(ColourGradient grad)
    {
        chordGradient = grad;
        repaint();
    }
    
private:
    bool isGradient;
    ColourGradient  chordGradient;
    Colour backgroundColour;
};

class ShortcutColourComponent : public Component
{
public:
    ShortcutColourComponent();
    ~ShortcutColourComponent();
    void paint (Graphics&g) override
    {
        g.fillAll(juce::Colour(45, 44, 44));
    }
    
    void setState(bool shouldBeActive)
    {
        isActive = shouldBeActive;
        if (isActive)
        {
            button_Add              ->setVisible(false);
            colourOutputComponent   ->setVisible(true);
            label_ColourCode        ->setVisible(true);
            
            button_Delete           ->setVisible(true);
            button_OpenSettings     ->setVisible(true);
        }
        else
        {
            button_Add              ->setVisible(true);
            colourOutputComponent   ->setVisible(false);
            label_ColourCode        ->setVisible(false);
            
            button_Delete           ->setVisible(false);
            button_OpenSettings     ->setVisible(false);
        }
    }
    
    
    std::unique_ptr<ImageButton> button_Add;
    std::unique_ptr<ColourOutputComponent> colourOutputComponent;
    std::unique_ptr<Label> label_ColourCode;
    std::unique_ptr<ImageButton> button_OpenSettings;
    std::unique_ptr<ImageButton> button_Delete;
    
    float scaleFactor = 0.5;
    void setScale(float factor)
    {
        scaleFactor = factor;
    }
    
    void resized() override;
    
private:
    bool isActive;

    
    Image imageAddIcon;
    Image imageSettings;
    Image imageDelete;
    
};

// option are 5 of these, each with shortcutRef, or reset each time

class ManipulationPopupComponent : public Component
{
public:
    ManipulationPopupComponent();
    ~ManipulationPopupComponent();
    
    void paint (Graphics&g) override
    {
        g.fillAll(juce::Colour(45, 44, 44));
    }
  
    std::unique_ptr<ImageButton> button_Multiplication;
    std::unique_ptr<ImageButton> button_Division;
    std::unique_ptr<TextEditor>  textEditorMultiplication;
    std::unique_ptr<TextEditor>  textEditorDivision;
    std::unique_ptr<ImageButton> button_Save;
    std::unique_ptr<ImageButton> button_Close;
    
    float scaleFactor = 0.5;
    void setScale(float factor)
    {
        scaleFactor = factor;
        lookAndFeel.setScale(scaleFactor);
    }
    
    void resized() override;
    
private:
    
    int shortcutRef;
    
    Image imageStopButton;
    Image imageStartButton;
    Image imageBlueButtonNormal;
    Image imageBlueButtonSelected;
    Image imageBlueCheckButtonNormal;
    Image imageBlueCheckButtonSelected;
    Image imageCloseButton;
    Image imageSaveButton;
    
    CustomLookAndFeel lookAndFeel;
};
