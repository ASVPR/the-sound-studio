/*
  ==============================================================================

    OscilloscopeVisualiserComponent.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "ProjectManager.h"
#include "CustomLookAndFeel.h"

//class OscilloscopeComponent : public Component, public Timer
//{
//public:
//    OscilloscopeComponent(ProjectManager * pm)
//    {
//        projectManager = pm;
//
//        startTimerHz(33);
//    }
//
//    ~OscilloscopeComponent(){}
//    
//    void paint (Graphics&g) override
//    {
//        g.fillAll(Colours::black);
//
//        if (shouldDrawBackground)
//        {
//            drawBackgroundGrid();
//
//            shouldDrawBackground = false;
//        }
//
//        g.drawImage(*imageBackground, xInset, yInset, sizeHW, sizeHW, 0, 0, imageBackground->getWidth(), imageBackground->getHeight());
//
//        drawOscilloscope(g);
//    }
//
//    void resized() override
//    {
//        yInset                  = 20 * scaleFactor;
//        sizeHW                  = getHeight() - (2 * yInset);
//        xInset                  = (getWidth() - sizeHW) / 2.f;
//        radius                  = (sizeHW / 2.f);
//
//        imageBackground         = new Image(Image::RGB, sizeHW, sizeHW, true);
//        imageOscilloscope       = new Image(Image::RGB, sizeHW, sizeHW, true);
//
//        shouldDrawBackground    = true;
//        shouldDrawOscilloscope  = true;
//    }
//
//    void timerCallback() override
//    {
//        repaint();
//    }
//
//
//    float yInset;
//    float sizeHW;
//    float xInset;
//
//    Image *imageBackground;
//    bool shouldDrawBackground = true;
//
//    void drawBackgroundGrid()
//    {
//        Graphics g_Background(*imageBackground);
//
//        g_Background.fillAll(Colours::black);
//
//        g_Background.setColour(Colours::lightgrey);
//
//        // draw horizontal line with tabs and labels
//        g_Background.drawLine(0.f, sizeHW / 2.f , sizeHW, sizeHW / 2.f, 1.f * scaleFactor);
//
//        for (int i = 0; i <= 20; i++)
//        {
//            // draw tabs
//            float tabSpace = sizeHW / 20;
//
//            float y = sizeHW / 2.f;
//            float tabSize = 5 * scaleFactor;
//
//            if (i == 0 || i == 5  ||  i == 15 || i == 20)
//            {
//                tabSize = 13 * scaleFactor;
//            }
//
//            g_Background.drawLine(tabSpace * i, y , tabSpace * i, y - tabSize, 1.f * scaleFactor);
//            g_Background.drawLine(tabSpace * i, y , tabSpace * i, y + tabSize, 1.f * scaleFactor);
//        }
//
//        // draw vertical line with tabs and labels
//        g_Background.drawLine(sizeHW / 2.f, 0.f , sizeHW/2.f, sizeHW, 1.f * scaleFactor);
//
//        for (int i = 0; i <= 20; i++)
//        {
//            // draw tabs
//            float tabSpace = sizeHW / 20;
//
//            float x = sizeHW / 2.f;
//            float tabSize = 5 * scaleFactor;
//
//            if (i == 0 || i == 5 ||  i == 15 || i == 20)
//            {
//                tabSize = 13 * scaleFactor;
//            }
//
//            g_Background.drawLine(x, tabSpace * i , x - tabSize, tabSpace * i, 1.f * scaleFactor);
//            g_Background.drawLine(x, tabSpace * i , x + tabSize, tabSpace * i, 1.f * scaleFactor);
//        }
//    }
//
//    void drawOscilloscope(Graphics &g)
//    {
//        AudioBuffer<float> oscilloscopeBuffer(projectManager->getOscillscopeBuffer());
//
//        oscilloscopePath.clear();
//        oscilloscopePath.preallocateSpace (oscilloscopeBuffer.getNumSamples());
//
//        circlePointCentre.x = 0;
//        circlePointCentre.y = yInset + radius;
//
//        oscilloscopePath.startNewSubPath(circlePointCentre);
//
//        float xFactor = (float)getWidth() / (oscilloscopeBuffer.getNumSamples()-1);
//
//        for (int s = 1; s < oscilloscopeBuffer.getNumSamples()-1; s++)
//        {
//            circlePointCentre.x = (xFactor * s);
//            circlePointCentre.y = (yInset + radius) + (radius * oscilloscopeBuffer.getSample(0, s));
//
//            oscilloscopePath.lineTo(circlePointCentre);
//        }
//
//        circlePointCentre.x = getWidth();
//        circlePointCentre.y = yInset + radius;
//
//        oscilloscopePath.lineTo(circlePointCentre);
//
//        oscilloscopePath.closeSubPath();
//
//
//        g.setColour(Colour::fromRGBA(70, 240, 70, 200));
//        g.strokePath(oscilloscopePath, PathStrokeType(1.f * scaleFactor));
//
//    }
//
//private:
//    ProjectManager * projectManager; // or maybe just direct processing oboject...
//
//    Point<float>circlePointCentre;
//    float radius;
//    float scaleFactor = 1.f;
//
//    Image *imageOscilloscope;
//    bool shouldDrawOscilloscope = true;
//    Path oscilloscopePath;
//
//};
