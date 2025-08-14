/*
  ==============================================================================

    CustomProgressBar.h
    Created: 13 Feb 2020 4:06:01pm
    Author:  Gary Jones

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"


class CustomProgressBar : public Component
{
public:
    CustomProgressBar() { value = 0.f; }
    ~CustomProgressBar() { }
    
    void paint(Graphics&g)
    {
        int yInset = 2;
        int xInset = 2;
        
        // simple bordered rectangle
        g.setColour(Colours::lightgrey);
        g.drawRoundedRectangle(0, 0, getWidth(), getHeight(), 10, 6);
        
        // need gradient fill from left to right, blue to red
        ColourGradient gradient(Colours::cyan, 0, 0,
                                Colours::magenta, getWidth(), getHeight(), false);
        
        g.setGradientFill(gradient);
        
        float fillToXpoint = (getWidth() - (2 * xInset)) * value;
        
        g.fillRoundedRectangle(xInset, yInset, fillToXpoint, getHeight() - (2 * yInset), 3);
        
    }
    
    void setValue(float newVal) { value = newVal; repaint(); }
    
private:
    float value; // 0-1
    
};
