/*
  ==============================================================================

    SpectrogramComponent.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "HighQualityMeter.h"
#include "CustomLookAndFeel.h"
#include "ProjectManager.h"
#include <memory>

//==============================================================================
/*
*/

#define kDefaultMinHertz    11.7185
#define kDefaultMaxHertz    22050
#define kNumFreqLines        10

#define kDefaultMinDbFS     -80.f
#define kDefaultMaxDbFS     12.f
#define kNumDBLines         8
#define kDBLogFactor        2.4

#define kDBAxisGap            40
#define kFreqAxisGap        17
#define kRightMargin        10
#define kTopMargin            14

#define POINT_DIFF          1



/*
// New FFt Visualiser Prototyping

 
 1. Single object shared between all modules ?
 2. Fix octave for higher res bins, needs resets create bands renewing.
 3. Peak amplitude readout.
 4. Colour spectrum.
 5.
 
 
 
 1. Import new OpenGL Oscilloscope
 2. Derive Lissajous from this
 3. Make 6 equal size visualisers on normal display, do popups later (Oscilloscope, FFT, Octave, Colour, Harmonograph, Lissaajous)
 4. Check Ringbuffer class to process audio from main proc object, instead of push model we currently use
 5. Check benefits of FFT in OpenGL, or standard 3D plotting
 6. Derive Harmonograph and Lissajous from oscilloscope
 7. For 3D harmonograph, add rotate with mouse option
 

 
 // Linux bug on drawing rendered images.. doesnt work on pointers.. need to create Image's on the stack and manage delete, rescale
 
 //
 
 
*/


class OscilloscopeComponent : public Component, public Timer
{
public:
    
    VISUALISER_SOURCE   visualiserSource;
    
    OscilloscopeComponent(ProjectManager * pm)
    {
        projectManager = pm;
        
        startTimerHz(33);
    }
    
    ~OscilloscopeComponent(){}
    
    void paint (Graphics&g) override
    {
        g.fillAll(Colours::black);
        
        g.setColour(Colours::lightgrey);
    //    g.setOpacity (0.7);
        g.drawRoundedRectangle(0, 0, getWidth(), getHeight(), 0, 4 * scaleFactor);
        
        if (shouldDrawBackground)
        {
            drawBackgroundGrid();
            
            shouldDrawBackground = false;
        }

        g.drawImage(*imageBackground, xInset, yInset, sizeHW, sizeHW, 0, 0, imageBackground->getWidth(), imageBackground->getHeight());
    
        drawOscilloscope(g);
    }

    void resized() override
    {
        yInset                  = 20 * scaleFactor;
        sizeHW                  = getHeight() - (2 * yInset);
        xInset                  = (getWidth() - sizeHW) / 2.f;
        radius                  = (sizeHW / 2.f);
        
        imageBackground         = new Image(Image::RGB, sizeHW, sizeHW, true);
        imageOscilloscope       = new Image(Image::RGB, sizeHW, sizeHW, true);
        
        shouldDrawBackground    = true;
        shouldDrawOscilloscope  = true;
    }
    
    void timerCallback() override
    {
        repaint();
    }
    
    
    float yInset;
    float sizeHW;
    float xInset;
    
    Image *imageBackground;
    bool shouldDrawBackground = true;
    
    void drawBackgroundGrid()
    {
        Graphics g_Background(*imageBackground);
        
        g_Background.fillAll(Colours::black);
    
        g_Background.setColour(Colours::lightgrey);
        
        // draw horizontal line with tabs and labels
        g_Background.drawLine(0.f, sizeHW / 2.f , sizeHW, sizeHW / 2.f, 1.f * scaleFactor);
        
        for (int i = 0; i <= 20; i++)
        {
            // draw tabs
            float tabSpace = sizeHW / 20;
            
            float y = sizeHW / 2.f;
            float tabSize = 5 * scaleFactor;
            
            if (i == 0 || i == 5  ||  i == 15 || i == 20)
            {
                tabSize = 13 * scaleFactor;
            }
            
            g_Background.drawLine(tabSpace * i, y , tabSpace * i, y - tabSize, 1.f * scaleFactor);
            g_Background.drawLine(tabSpace * i, y , tabSpace * i, y + tabSize, 1.f * scaleFactor);
        }
        
        // draw vertical line with tabs and labels
        g_Background.drawLine(sizeHW / 2.f, 0.f , sizeHW/2.f, sizeHW, 1.f * scaleFactor);
        
        for (int i = 0; i <= 20; i++)
        {
            // draw tabs
            float tabSpace = sizeHW / 20;
            
            float x = sizeHW / 2.f;
            float tabSize = 5 * scaleFactor;
            
            if (i == 0 || i == 5 ||  i == 15 || i == 20)
            {
                tabSize = 13 * scaleFactor;
            }
            
            g_Background.drawLine(x, tabSpace * i , x - tabSize, tabSpace * i, 1.f * scaleFactor);
            g_Background.drawLine(x, tabSpace * i , x + tabSize, tabSpace * i, 1.f * scaleFactor);
        }
    }
    
    void drawOscilloscope(Graphics &g)
    {
        AudioBuffer<float> oscilloscopeBuffer(projectManager->getOscillscopeBuffer());
        
        oscilloscopePath.clear();
        oscilloscopePath.preallocateSpace (oscilloscopeBuffer.getNumSamples());
        
        circlePointCentre.x = 0;
        circlePointCentre.y = yInset + radius;

        oscilloscopePath.startNewSubPath(circlePointCentre);
        
        float xFactor = (float)getWidth() / (oscilloscopeBuffer.getNumSamples()-1);
        
        for (int s = 1; s < oscilloscopeBuffer.getNumSamples()-1; s++)
        {
            circlePointCentre.x = (xFactor * s);
            circlePointCentre.y = (yInset + radius) + (radius * oscilloscopeBuffer.getSample(0, s));
            
            oscilloscopePath.lineTo(circlePointCentre);
        }
        
        circlePointCentre.x = getWidth();
        circlePointCentre.y = yInset + radius;
        
        oscilloscopePath.lineTo(circlePointCentre);
        
        oscilloscopePath.closeSubPath();
        

        g.setColour(Colour::fromRGBA(70, 240, 70, 200));
        g.strokePath(oscilloscopePath, PathStrokeType(1.f * scaleFactor));

    }
    
    bool shouldUpdate = false;
    void setShouldUpdate(bool should)
    {
        shouldUpdate = should;
        
        if (shouldUpdate)
        {
            startTimer(TIMER_UPDATE_RATE);
        }
        else
        {
            stopTimer();
        }
    }
    
    void setScale(float factor) { scaleFactor = factor; }

private:
    ProjectManager * projectManager; // or maybe just direct processing oboject...
    
    Point<float>circlePointCentre;
    float radius;
    float scaleFactor = 1.f;
    
    Image *imageOscilloscope;
    bool shouldDrawOscilloscope = true;
    Path oscilloscopePath;

};


class LissajousCurveViewerComponent : public Component, public Timer
{
public:
    
    VISUALISER_SOURCE   visualiserSource;
    
    LissajousCurveViewerComponent(ProjectManager * pm, bool isLissajous)
    {
        projectManager = pm;
        isForLissajous = isLissajous;
    }
    
    ~LissajousCurveViewerComponent() { }
    
    void paint (Graphics&g) override
    {
        if (isForLissajous)
        {
            if (shouldDrawBackground)
            {
                drawBackgroundGrid();
                
                shouldDrawBackground = false;
            }
            
            g.drawImage(*imageBackground, xInset, yInset, sizeHW, sizeHW, 0, 0, imageBackground->getWidth(), imageBackground->getHeight());
        }
        else
        {
            g.setColour(Colours::black);
            g.fillAll();
            
            g.setOpacity (1.0f);
        }
        

        
    
        drawLissajous(g);
        
        if (!isForLissajous)
        {
            g.setColour(Colours::lightgrey);
            g.setOpacity (1.0);
            g.drawRoundedRectangle(0, 0, getWidth(), getHeight(), 0, 4 * scaleFactor);
        }

    }

    void resized() override
    {
        yInset                  = 20 * scaleFactor;
        sizeHW                  = getHeight() - (2 * yInset);
        xInset                  = (getWidth() - sizeHW) / 2.f;
        radius                  = (sizeHW / 2.f);
        
        imageBackground         = new Image(Image::RGB, sizeHW, sizeHW, true);
        imageLissajous          = new Image(Image::RGB, sizeHW, sizeHW, true);
        
        shouldDrawBackground    = true;
        shouldDrawLissajous     = true;
    }
    
    void timerCallback() override
    {
        repaint();
    }
    
    bool shouldUpdate = false;
    void setShouldUpdate(bool should)
    {
        shouldUpdate = should;
        
        if (shouldUpdate)
        {
            startTimer(TIMER_UPDATE_RATE);
        }
        else
        {
            stopTimer();
        }
    }
    
    
    float yInset;
    float sizeHW;
    float xInset;
    
    Image *imageBackground;
    bool shouldDrawBackground = true;
    
    void drawBackgroundGrid()
    {
        if (isForLissajous)
        {
            Graphics g_Background(*imageBackground);
            
            g_Background.fillAll(Colours::black);
        
            g_Background.setColour(Colours::lightgrey);
            
            // draw horizontal line with tabs and labels
            g_Background.drawLine(0.f, sizeHW / 2.f , sizeHW, sizeHW / 2.f, 1.f * scaleFactor);
            
            for (int i = 0; i <= 20; i++)
            {
                // draw tabs
                float tabSpace = sizeHW / 20;
                
                float y = sizeHW / 2.f;
                float tabSize = 5 * scaleFactor;
                
                if (i == 0 || i == 5  ||  i == 15 || i == 20)
                {
                    tabSize = 13 * scaleFactor;
                }
                
                g_Background.drawLine(tabSpace * i, y , tabSpace * i, y - tabSize, 1.f * scaleFactor);
                g_Background.drawLine(tabSpace * i, y , tabSpace * i, y + tabSize, 1.f * scaleFactor);
            }
            
            // draw vertical line with tabs and labels
            g_Background.drawLine(sizeHW / 2.f, 0.f , sizeHW/2.f, sizeHW, 1.f * scaleFactor);
            
            for (int i = 0; i <= 20; i++)
            {
                // draw tabs
                float tabSpace = sizeHW / 20;
                
                float x = sizeHW / 2.f;
                float tabSize = 5 * scaleFactor;
                
                if (i == 0 || i == 5 ||  i == 15 || i == 20)
                {
                    tabSize = 13 * scaleFactor;
                }
                
                g_Background.drawLine(x, tabSpace * i , x - tabSize, tabSpace * i, 1.f * scaleFactor);
                g_Background.drawLine(x, tabSpace * i , x + tabSize, tabSpace * i, 1.f * scaleFactor);
            }
        }
        else
        {
            Graphics g_Background(*imageBackground);
            
            g_Background.fillAll(Colours::black);
        
            g_Background.setColour(Colours::lightgrey);
        }
        
        
       
    }
    
    // z axis = 45
    
    
    void drawLissajous(Graphics &g)
    {
        
        if (isForLissajous)
        {
            AudioBuffer<float> lissajousBuffer(projectManager->lissajousProcessor->getLissajousBuffer());
            
            lissajousPath.clear();
            lissajousPath.preallocateSpace (lissajousBuffer.getNumSamples()+1);
            
            circlePointCentre.x = (xInset + radius) + (radius * lissajousBuffer.getSample(0, 0));
            circlePointCentre.y = (yInset + radius) + (radius * lissajousBuffer.getSample(1, 0));

            lissajousPath.startNewSubPath(circlePointCentre);
            
            for (int s = 0; s < lissajousBuffer.getNumSamples(); s++)
            {
                circlePointCentre.x = (xInset + radius) + (radius * lissajousBuffer.getSample(0, s));
                circlePointCentre.y = (yInset + radius) + (radius * lissajousBuffer.getSample(1, s));
                
                lissajousPath.lineTo(circlePointCentre);
            }
        }
        else
        {
            AudioBuffer<float> lissajousBuffer(projectManager->getOscillscopeBuffer());
            
            lissajousPath.clear();
            lissajousPath.preallocateSpace (lissajousBuffer.getNumSamples()+1);
            
            circlePointCentre.x = (xInset + radius) + (radius * lissajousBuffer.getSample(0, 0));
            circlePointCentre.y = (yInset + radius) + (radius * lissajousBuffer.getSample(1, 0));

            lissajousPath.startNewSubPath(circlePointCentre);
            
            for (int s = 0; s < lissajousBuffer.getNumSamples(); s++)
            {
                circlePointCentre.x = (xInset + radius) + (radius * lissajousBuffer.getSample(0, s));
                circlePointCentre.y = (yInset + radius) + (radius * lissajousBuffer.getSample(1, s));
                
                lissajousPath.lineTo(circlePointCentre);
            }
        }

        
//        lissajousPath.closeSubPath();
        
        Colour lissColour(projectManager->getSettingsColorParameter(10));
        g.setColour(lissColour);
        g.strokePath(lissajousPath, PathStrokeType(1.f * scaleFactor));

    }
    
    void setScale(float factor) { scaleFactor = factor; }

private:
    ProjectManager * projectManager; // or maybe just direct processing oboject...
    
    Point<float>circlePointCentre;
    float radius;
    float scaleFactor = 1.f;
    
    Image *imageLissajous;
    bool shouldDrawLissajous = true;
    Path lissajousPath;
    
    bool isForLissajous;
};



class FrequencyDataComponent : public Component, public Timer
{
public:
    
    VISUALISER_SOURCE   visualiserSource;
    
    // Contains Funademtnal Harmonics & peak frequ3encies etc
    FrequencyDataComponent(ProjectManager * pm);
    ~FrequencyDataComponent();
    void resized() override;
    void setScale(float factor) { scaleFactor = factor; }
    void paint(Graphics &g) override;
    void pushUpdate();
    
    void timerCallback() override
    {
        pushUpdate();
    }
    
    void setShouldUpdate(bool should)
    {
        shouldUpdate = should;
        
        if (shouldUpdate)
        {
            startTimer(TIMER_UPDATE_RATE);
        }
        else
        {
            stopTimer();
        }
    }
    
private:
    
    ProjectManager * projectManager;
    float sampleRate;
    int fftSize;
    
    int     keynote;
    double   peakFrequency;
    double   peakDB;
    Array<float>   * upperHarmonics;
    Array<float>   * intervals;
    int     octave;
    int     chord;
    
    std::unique_ptr<Label> labelKeynote;
    std::unique_ptr<Label> labelPeakFrequency;
    std::unique_ptr<Label> labelPeakDB;
    std::unique_ptr<Label> labelEMA;
    std::unique_ptr<Label> labelUpperHarmonics;
    std::unique_ptr<Label> labelLowerHarmonics;
    std::unique_ptr<Label> labelOctave;
    
    std::unique_ptr<Label> labelKeynoteResult;
    std::unique_ptr<Label> labelPeakFrequencyResult;
    std::unique_ptr<Label> labelPeakDBResult;
    std::unique_ptr<Label> labelEMAResult;
    std::unique_ptr<Label> labelUpperHarmonicsResult;
    std::unique_ptr<Label> labelLowerHarmonicsResult;
    std::unique_ptr<Label> labelOctaveResult;
    
    float scaleFactor = 0.5;
    
    bool shouldUpdate = false;

};

class OctaveVisualiserComponent2 : public Component, public Timer
{
public:
    
    VISUALISER_SOURCE   visualiserSource;
    
    OctaveVisualiserComponent2(ProjectManager * pm);
    ~OctaveVisualiserComponent2();
    
    void mouseDown (const MouseEvent& event) override;
    void mouseDrag (const MouseEvent& event) override;
    void mouseUp (const MouseEvent& event) override;
    
    // FFT Improvements: Add mouse wheel zoom support
    void mouseWheelMove (const MouseEvent&event, const MouseWheelDetails&wheel)override
    {
        didSwipe(wheel.deltaX, wheel.deltaY, event.position);
    }

    void didSwipe (float deltaX, float deltaY, Point<float> position);
    
    void paint (Graphics&g) override;
    void drawDBLinesNew(Graphics&g);
    void drawDataDisplay(Graphics &g);
    
    inline float magnitudeToY (float magnitude) const;
    float getDBForY(float yPos, float top, float bottom);
    
    void resized() override;
    
    void timerCallback() override
    {
        pushUpdate();
    }
    
    void pushUpdate();
    
    void setScale(float factor) { scaleFactor = factor; }
    
    // FFT Improvements: Add zoom range support
    double zoomRange_FreqLow        = kDefaultMinHertz;
    double zoomRange_FreqHigh       = kDefaultMaxHertz;
    double zoomRange_AmplitudeLow   = kDefaultMinDbFS;
    double zoomRange_AmplitudeHigh  = kDefaultMaxDbFS;
    
    float range_X_Min_Factor        = 0.f;
    float range_X_Max_Factor        = 1.f;
    float range_Y_Min_Factor        = 0.f;
    float range_Y_Max_Factor        = 1.f;
    
    bool shouldUpdate = false;
    void setShouldUpdate(bool should)
    {
        shouldUpdate =should;
        
        if (shouldUpdate)
        {
            startTimer(TIMER_UPDATE_RATE);
        }
        else
        {
            stopTimer();
        }
    }
        
private:
    Image * imageBackground;
    Image * imageMainSpectrumPlot;
    Image * plotHistory;
    
    bool shouldDrawLines;
    
    bool displayData = false;
    Point<float> displayDataPoint;
    
    ProjectManager * projectManager;
    double sampleRate;
    int fftSize;
    int numOctaves;
    int numBands;
    
    Array<float> centralFrequencies;
    
    float scaleFactor = 0.5;
    
};

class SpectrogramComponent    : public Component, public Slider::Listener, public Timer, public Button::Listener
{
public:
    
    VISUALISER_SOURCE   visualiserSource;
    
    SpectrogramComponent(ProjectManager * pm, Rectangle<float> initialSize, bool popup);
    ~SpectrogramComponent();

    void resized() override;
    void paint (Graphics&g) override;
    
    void timerCallback() override { pushUpdate();  }
    
    bool shouldUpdate = false;
    void setShouldUpdate(bool should)
    {
        shouldUpdate = should;
        
        if (shouldUpdate) { startTimer(TIMER_UPDATE_RATE); }
        else { stopTimer(); }
    }
    
    void mouseDown (const MouseEvent& event) override;
    void mouseDrag (const MouseEvent& event) override;
    void mouseUp (const MouseEvent& event) override;
    void mouseWheelMove (const MouseEvent&event, const MouseWheelDetails&wheel)override
    {
        // when called, simply update delta to new position.
        // timer update callback will request the min/max fft as required
        
        didSwipe(wheel.deltaX, wheel.deltaY, event.position);
    }

    // Expose view range factors for sync/compare use-cases
    void setViewRangeFactors(float xMin, float xMax, float yMin, float yMax)
    {
        range_X_Min_Factor = jlimit(0.0f, 0.9f, xMin);
        range_X_Max_Factor = jlimit(0.1f, 1.0f, xMax);
        range_Y_Min_Factor = jlimit(0.0f, 0.9f, yMin);
        range_Y_Max_Factor = jlimit(0.1f, 1.0f, yMax);

        zoomRange_FreqLow       = getFrequencyForPosition(range_X_Min_Factor);
        zoomRange_FreqHigh      = getFrequencyForPosition(range_X_Max_Factor);
        zoomRange_AmplitudeLow  = getDBForY(range_Y_Min_Factor, 1.f, 0.f);
        zoomRange_AmplitudeHigh = getDBForY(range_Y_Max_Factor, 1.f, 0.f);

        viewRangeDidChange();
    }

    void getViewRangeFactors(float& xMin, float& xMax, float& yMin, float& yMax) const
    {
        xMin = range_X_Min_Factor;
        xMax = range_X_Max_Factor;
        yMin = range_Y_Min_Factor;
        yMax = range_Y_Max_Factor;
    }
    
    float range_X_Min_Factor = 0.f;
    float range_X_Max_Factor = 1.f;
    float range_Y_Min_Factor = 0.f;
    float range_Y_Max_Factor = 1.f;

    
    void didSwipe (float deltaX, float deltaY, Point<float> position)
    {

        // Delta X
        if (range_X_Min_Factor + deltaX >= 0 && range_X_Min_Factor + deltaX <= 0.9)
        {
            range_X_Min_Factor += deltaX;
        }
        
        if (range_X_Max_Factor + deltaX >= 0.1 && range_X_Max_Factor + deltaX <= 1.f)
        {
            range_X_Max_Factor += deltaX;
        }
        
        // Delta Y
        deltaY *= 0.5;
        
        if (range_X_Min_Factor + deltaY >= 0 && range_X_Min_Factor + deltaY <= 0.9)
        {
            range_X_Min_Factor += deltaY;
        }
        
        if (range_X_Max_Factor - deltaY >= 0.1 && range_X_Max_Factor - deltaY <= 1.f)
        {
            range_X_Max_Factor -= deltaY;
        }
        

        zoomRange_FreqLow       = getFrequencyForPosition(range_X_Min_Factor);
        zoomRange_FreqHigh      = getFrequencyForPosition(range_X_Max_Factor);
        zoomRange_AmplitudeLow  = getDBForY(range_Y_Min_Factor, 1.f, 0.f);
        zoomRange_AmplitudeHigh = getDBForY(range_Y_Max_Factor, 1.f, 0.f);

        viewRangeDidChange();
    }
    
    
//    void mouseMagnify (const MouseEvent&event, float magLevel) override
//    {
//        didMagnify(magLevel);
//    }
    
    void buttonClicked (Button*button) override
    {
//        if (button == buttonMagUp)
//        {
//            didMagnify(2.0);
//        }
//        else if (button == buttonMagDown)
//        {
//            didMagnify(0.5);
//        }
    }
    
    void pushUpdate();
    void drawLayersNew(Graphics &g);

    Image *imageBackground;
    Image *imageMainSpectrumPlot;
    
    bool shouldDrawLines = true;
    void drawBackground(Graphics & g);
    void drawSpectrumLines(Graphics& g);
    void drawDataDisplay(Graphics &g);
    void drawDBLinesNew(Graphics&g);
    void drawFreqLinesNew(Graphics&g);
    
    void createPathNew  (Graphics&g);

    // Math functions
    float getFrequencyForPosition (float pos);
    float getDBForY(float yPos, float top, float bottom);
    
    float getFrequencyForPositionPopup (float pos);
    float getDBForYPopup(float yPos, float top, float bottom);

    // drawing functions
    String stringForValue(double value, bool divide, bool decimals);
    
    void setScale(float factor) { scaleFactor = factor; }

private:
    bool isPopup;
    
    ProjectManager * projectManager;
    
    float activeHeight;
    float activeWidth;
    int maxOctaves = 16;
    float minFreq = 5.86;
    
    int fftSize;
    double sampleRate;
    bool shouldUpdateSpectrum;
    
    CriticalSection pathCreationLock;
    
    Path p;
    
    bool displayData = false;
    Point<float> displayDataPoint;
    
    float scaleFactor = 0.5;
    
    Image plotHistory;

    // Scaler zoomer
    double zoomRange_FreqLow        = kDefaultMinHertz;
    double zoomRange_FreqHigh       = kDefaultMaxHertz;
    double zoomRange_AmplitudeLow   = kDefaultMinDbFS;
    double zoomRange_AmplitudeHigh  = kDefaultMaxDbFS;
    
    std::unique_ptr<Slider> sliderRangeFrequency; // TwoValueHorizontal
    std::unique_ptr<Slider> sliderRangeAmplitude; // TwoValueHorizontal
    
    std::unique_ptr<TextButton> buttonMagUp;
    std::unique_ptr<TextButton> buttonMagDown;
    
    void viewRangeDidChange()
    {
        if (isPopup)
        {
            shouldDrawLines         = true;
            
            plotHistory             = Image(Image::RGB, getWidth(), getHeight(), true);
            
            repaint();
        }
    }
    
    virtual void sliderValueChanged (Slider* slider) override
    {
        if (slider == sliderRangeFrequency.get())
        {
            zoomRange_FreqLow       = getFrequencyForPosition(sliderRangeFrequency->getMinValue());
            zoomRange_FreqHigh      = getFrequencyForPosition(sliderRangeFrequency->getMaxValue());
            
            viewRangeDidChange();
        }
        else if (slider == sliderRangeAmplitude.get())
        {
            zoomRange_AmplitudeLow  = getDBForY(sliderRangeAmplitude->getMinValue(), 1.f, 0.f);
            zoomRange_AmplitudeHigh = getDBForY(sliderRangeAmplitude->getMaxValue(), 1.f, 0.f);

            viewRangeDidChange();
        }
        
        range_X_Min_Factor = sliderRangeFrequency->getMinValue();
        range_X_Max_Factor = sliderRangeFrequency->getMaxValue();
        range_Y_Min_Factor = sliderRangeAmplitude->getMinValue();
        range_Y_Max_Factor = sliderRangeAmplitude->getMaxValue();
    }
    
    
    float magnificationLevel = 1.f;
    void didMagnify(float magLevel)
    {
//        if ((magnificationLevel * magLevel) >= 1.f)
//        {
//            magnificationLevel *= magLevel;
//
//            if (range_X_Min_Factor * magLevel >= 0.f && range_X_Min_Factor * magLevel <= 1.f)
//            {
//                range_X_Min_Factor *= magLevel;
//            }
//
//            if ((range_X_Max_Factor / magLevel) >= 0.f && (range_X_Max_Factor / magLevel) <= 1.f)
//            {
//                range_X_Max_Factor /= magLevel;
//            }
//
//            zoomRange_FreqLow       = getFrequencyForPosition(range_X_Min_Factor);
//            zoomRange_FreqHigh      = getFrequencyForPosition(range_X_Max_Factor);
//            zoomRange_AmplitudeLow  = getDBForY(range_Y_Min_Factor, 1.f, 0.f);
//            zoomRange_AmplitudeHigh = getDBForY(range_Y_Max_Factor, 1.f, 0.f);
//
//            viewRangeDidChange();
//        }
//        else
//        {
//            magnificationLevel = 1.f;
//        }
        
        
    }
    
    void setRangeOfSliderFrequency(double newMin, double newMax)
    {
        sliderRangeFrequency->setRange(newMin, newMax);
    }
    
    void setRangeOfSliderAmplitude(double newMin, double newMax)
    {
        sliderRangeAmplitude->setRange(newMin, newMax);
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectrogramComponent)
};


class ColourSpectrumVisualiserComponent : public Component, public Timer
{
public:
    
    VISUALISER_SOURCE   visualiserSource;
    
    ColourSpectrumVisualiserComponent(ProjectManager * pm, int initW, int initH);
    ~ColourSpectrumVisualiserComponent();
    
    void mouseDown (const MouseEvent& event) override;
    void mouseDrag (const MouseEvent& event) override;
    void mouseUp (const MouseEvent& event) override;
    
    void mouseWheelMove (const MouseEvent&event, const MouseWheelDetails&wheel)override
    {
        didSwipe(wheel.deltaX, wheel.deltaY, event.position);
    }

    double zoomRange_FreqLow        = kDefaultMinHertz;
    double zoomRange_FreqHigh       = kDefaultMaxHertz;
    double zoomRange_AmplitudeLow   = kDefaultMinDbFS;
    double zoomRange_AmplitudeHigh  = kDefaultMaxDbFS;
    
    float range_X_Min_Factor        = 0.f;
    float range_X_Max_Factor        = 1.f;
    float range_Y_Min_Factor        = 0.f;
    float range_Y_Max_Factor        = 1.f;
    
    void didSwipe (float deltaX, float deltaY, Point<float> position)
    {
        
        // Delta X
        if (range_X_Min_Factor + deltaX >= 0 && range_X_Min_Factor + deltaX <= 0.9)
        {
            range_X_Min_Factor += deltaX;
        }
        
        if (range_X_Max_Factor + deltaX >= 0.1 && range_X_Max_Factor + deltaX <= 1.f)
        {
            range_X_Max_Factor += deltaX;
        }
        
        
        // Delta Y
        deltaY *= 0.5;
        
        if (range_X_Min_Factor + deltaY >= 0 && range_X_Min_Factor + deltaY <= 0.9)
        {
            range_X_Min_Factor += deltaY;
        }
        
        if (range_X_Max_Factor - deltaY >= 0.1 && range_X_Max_Factor - deltaY <= 1.f)
        {
            range_X_Max_Factor -= deltaY;
        }
        

        zoomRange_FreqLow       = getFrequencyForPosition(range_X_Min_Factor);
        zoomRange_FreqHigh      = getFrequencyForPosition(range_X_Max_Factor);

        
        viewRangeDidChange();
    }

    // drawing functions
    float getFrequencyForPosition (float posYFactor)
    {
        sampleRate      = projectManager->getSampleRate();
        float freq      = powf(10.f, posYFactor * (log10f(sampleRate / 2.f)-log10f(kDefaultMinHertz)) + log10f(kDefaultMinHertz));
        
        return freq;
    }

    bool isPopup            = false;
    bool shouldDrawLines    = true;
    
    void viewRangeDidChange()
    {
//        if (isPopup)
//        {
            shouldDrawLines         = true;
            
            repaint();
//        }
    }
    
    
    void paint (Graphics&g) override;
    void drawDisplayData(Graphics &g);
    void drawFrequencyLines(Graphics &g);
    void drawTimelineGrid(Graphics &g);
    void drawNextLineOfSpectrogram();
    
    void resized() override;

    void timerCallback() override
    {
        pushUpdate();
    }
    
    void pushUpdate();
    
    void setScale(float factor) { scaleFactor = factor; }
    
    bool shouldUpdate = false;
    void setShouldUpdate(bool should)
    {
        shouldUpdate = should;
        
        if (shouldUpdate)
        {
            startTimer(TIMER_UPDATE_RATE);
        }
        else
        {
            stopTimer();
        }
    }
    
private:
    ProjectManager * projectManager;
    float sampleRate;
    Image spectrogramImage;
    Image * newSpectrogramImage;
    
    
    
    float componentHeight;
    
    int fftSize;
    float * fftData;
    
    bool displayData = false;
    Point<float> displayDataPoint;
    
    float scaleFactor = 0.5;
    
    float logScale = 0.2f;
    
    ColourGradient gradientDecibels;
    
    void initGradient()
    {
        
        gradientDecibels.addColour(0.f, Colours::black);
        gradientDecibels.addColour(0.2f, Colours::purple);
        gradientDecibels.addColour(0.4f, Colours::red);
        gradientDecibels.addColour(0.6f, Colours::yellow);
        gradientDecibels.addColour(0.8f, Colours::white);
        
        double posDb = 0.0;
        gradientDecibels.getColourAtPosition(posDb);
        
    }
    
    inline int xToBinIndex (float x, float w, float minFreq, float maxFreq, float sr, int fftSize) const
    {
        float x2 = w - x;
        // find num octaves
        float xScale                = x2 / w;
        float freq                  = powf(10.f, xScale * (log10f(maxFreq)-log10f(minFreq)) + log10f(minFreq));
        int binIndex                = (int)freq / (sr / 2.f) * fftSize;
                                       
        return binIndex;
    }
    
    
};

