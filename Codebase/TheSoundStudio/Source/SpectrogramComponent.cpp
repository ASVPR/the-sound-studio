/*
  ==============================================================================

    SpectrogramComponent.cpp
    Created: 29 May 2019 4:12:00pm
    Author:  Gary Jones

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "SpectrogramComponent.h"
#include "Parameters.h"
#include "vConstants.h"


/*
 
 𝑃total_linear=𝑝21+𝑝22+...‾‾‾‾‾‾‾‾‾‾‾√

𝑃total_dB=20𝑙𝑜𝑔10(𝑃total_linear/20E-6)
 
Fixes
 1. move blue button
 2. change line width or remove
 3. text size propertionate with scaling
 4. process button move right
 5. Octave is bugged
 6. mouse pointer get freq/db
 7. decay rate on fft
 8.
 
 
 Things to do
 
 1. Fix Octave 1/3
 - add centre frequencies to bars, at bottom
 - add db value to each 1/3 band, will need to modify the meter class.
 - add dB chart on left side
 - add Frequency (hz) label on bottom axis
 - variable number of bands, or splits per octave..
 - increase range with sample rate increase
 
 2. FFT View Containers
 - add magnifier and scoll left/right/up/down for focusing in on frequencies
 
 3. FFT code
 - range increases with sample rate, e.g 96khz SR = 48khz resolution., or atleast offer boolean in settings
 -
 
 4. Spectrum Analyzer
 - keep path in memory, use float array for Y level of each bin, then update if higher, or decay
 - db lines improvement
 - hz lines should be more precise, and keep to 100hz, 200hz, 400hz, 800hz 1.6kz etc
 - viewport to zoom 2x, 3,x 4x, etc
 - mouse pointer demonstrates hz, db level with x, y lines
 - multichannel inputs, assign colours, data readouts..
 - record vid function
 
 
 
 2. Look at options for Colour Spectrum analyzer
 3. optimise fft, larger size, power, experiment
 4. on/off bool buttons on each display
 5. add db meters as standard on the size with explicit db readings !, maybe with output volume
 6.
 7. Open GL oscilloscope / FFT 3D
 8. Harmonograph
 */

#pragma mark Frequency Data Output

FrequencyDataComponent::FrequencyDataComponent(ProjectManager * pm)
{
    projectManager = pm;
    
    labelKeynote = new Label();
    labelKeynote->setText("Keynote", dontSendNotification);
    labelKeynote->setJustificationType(Justification::left);
    addAndMakeVisible(labelKeynote);
    
    labelPeakFrequency = new Label();
    labelPeakFrequency->setText("Peak Frequency", dontSendNotification);
    labelPeakFrequency->setJustificationType(Justification::left);
    addAndMakeVisible(labelPeakFrequency);
    
    labelPeakDB = new Label();
    labelPeakDB->setText("Peak dB", dontSendNotification);
    labelPeakDB->setJustificationType(Justification::left);
    addAndMakeVisible(labelPeakDB);
    
    labelEMA = new Label();
    labelEMA->setText("EMA", dontSendNotification);
    labelEMA->setJustificationType(Justification::left);
    addAndMakeVisible(labelEMA);
    
    labelEMAResult = new Label();
    labelEMAResult->setText("0.0hz", dontSendNotification);
    labelEMAResult->setJustificationType(Justification::left);
    addAndMakeVisible(labelEMAResult);
    
    labelOctave = new Label();
    labelOctave->setText("Octave", dontSendNotification);
    labelOctave->setJustificationType(Justification::left);
    addAndMakeVisible(labelOctave);
    
    labelUpperHarmonics = new Label();
    labelUpperHarmonics->setText("Harmonics + ", dontSendNotification);
    labelUpperHarmonics->setJustificationType(Justification::left);
    addAndMakeVisible(labelUpperHarmonics);
    
    labelLowerHarmonics = new Label();
    labelLowerHarmonics->setText("Intervals ", dontSendNotification);
    labelLowerHarmonics->setJustificationType(Justification::left);
    addAndMakeVisible(labelLowerHarmonics);
    
    labelKeynoteResult = new Label();
    labelKeynoteResult->setText("C#", dontSendNotification);
    labelKeynoteResult->setJustificationType(Justification::left);
    addAndMakeVisible(labelKeynoteResult);
    
    labelPeakFrequencyResult = new Label();
    // MAINTAINABILITY: Using named constant instead of hardcoded "432.333" for consistent frequency reference
    labelPeakFrequencyResult->setText(String(AudioConstants::ALTERNATIVE_A4_FREQ, 3) + " hz", dontSendNotification);
    labelPeakFrequencyResult->setJustificationType(Justification::left);
    addAndMakeVisible(labelPeakFrequencyResult);
    
    labelPeakDBResult = new Label();
    labelPeakDBResult->setText(" -6.6 dB", dontSendNotification);
    labelPeakDBResult->setJustificationType(Justification::left);
    addAndMakeVisible(labelPeakDBResult);
    
    labelUpperHarmonicsResult = new Label();
    labelUpperHarmonicsResult->setText("", dontSendNotification);
    labelUpperHarmonicsResult->setJustificationType(Justification::left);
    addAndMakeVisible(labelUpperHarmonicsResult);
    
    labelLowerHarmonicsResult = new Label();
    labelLowerHarmonicsResult->setText("", dontSendNotification);
    labelLowerHarmonicsResult->setJustificationType(Justification::left);
    addAndMakeVisible(labelLowerHarmonicsResult);
    
    labelOctaveResult = new Label();
    labelOctaveResult->setText("2", dontSendNotification);
    labelOctaveResult->setJustificationType(Justification::left);
    addAndMakeVisible(labelOctaveResult);
    
    upperHarmonics  = new Array<float> (6);
    intervals       = new Array<float> (6);
    for (int i = 0; i < 6; i++)
    {
        float c = 0.f;
        upperHarmonics->add(c);
        intervals->add(c);
    }


}

FrequencyDataComponent::~FrequencyDataComponent(){}

void FrequencyDataComponent::resized()
{
    int leftMargin = 10 * scaleFactor;
    int topMargin = 10 * scaleFactor;
    int lineSize = getHeight() / 7;
    int fieldWidth = (getWidth() - leftMargin) / 2.f;
    int leftMargin2 = (getWidth() / 3.f) - leftMargin;
    int fieldWidth2 = (getWidth() / 3.f) * 2 - leftMargin;
    int fontSize = 33 * scaleFactor;
    
    
    labelPeakFrequency->setBounds(leftMargin, topMargin + (lineSize * 0), fieldWidth, lineSize);
    labelPeakFrequency->setFont(fontSize);
    
    labelPeakFrequencyResult->setBounds(leftMargin2, topMargin + (lineSize * 0), fieldWidth2, lineSize);
    labelPeakFrequencyResult->setFont(fontSize);
    
    
    labelEMA->setBounds(leftMargin, topMargin + (lineSize * 1), fieldWidth, lineSize);
    labelEMA->setFont(fontSize);

    labelEMAResult->setBounds(leftMargin2, topMargin + (lineSize * 1), fieldWidth2, lineSize);
    labelEMAResult->setFont(fontSize);

    
    labelPeakDB->setBounds(leftMargin, topMargin + (lineSize * 2), fieldWidth, lineSize);
    labelPeakDB->setFont(fontSize);
    
    labelPeakDBResult->setBounds(leftMargin2, topMargin + (lineSize * 2), fieldWidth, lineSize);
    labelPeakDBResult->setFont(fontSize);
    
    labelKeynote->setBounds(leftMargin, topMargin + (lineSize * 3), fieldWidth, lineSize);
    labelKeynote->setFont(fontSize);
    
    labelKeynoteResult->setBounds(leftMargin2, topMargin + (lineSize * 3), fieldWidth2, lineSize);
    labelKeynoteResult->setFont(fontSize);
    
    
    labelOctave->setBounds(leftMargin, topMargin + (lineSize * 4), fieldWidth, lineSize);
    labelOctave->setFont(fontSize);
    
    labelOctaveResult->setBounds(leftMargin2, topMargin + (lineSize * 4), fieldWidth2, lineSize);
    labelOctaveResult->setFont(fontSize);
    
    labelUpperHarmonics->setBounds(leftMargin, topMargin + (lineSize * 5), fieldWidth, lineSize);
    labelUpperHarmonics->setFont(fontSize);
    
    labelUpperHarmonicsResult->setBounds(leftMargin2, topMargin + (lineSize * 5), fieldWidth2, lineSize);
    labelUpperHarmonicsResult->setFont(fontSize);
    
    labelLowerHarmonics->setBounds(leftMargin, topMargin + (lineSize * 6), fieldWidth, lineSize);
    labelLowerHarmonics->setFont(fontSize);
    
    labelLowerHarmonicsResult->setBounds(leftMargin2, topMargin + (lineSize * 6), fieldWidth2, lineSize);
    labelLowerHarmonicsResult->setFont(fontSize);

}

void FrequencyDataComponent::paint(Graphics &g)
{
    g.setColour(Colours::lightgrey);
    g.setOpacity (0.7);
    g.drawRoundedRectangle(0, 0, getWidth(), getHeight(), 0, 4 * scaleFactor);
}



void FrequencyDataComponent::pushUpdate()
{
    fftSize                 = projectManager->getFFTSize();
    sampleRate              = projectManager->getSampleRate();

    double ema;
//    projectManager->createFrequencyData(peakFrequency, peakDB, *upperHarmonics, *intervals, keynote, octave, ema);
    
    int fftRef = (int)visualiserSource - 1;
    projectManager->createFrequencyData(fftRef, peakFrequency, peakDB, *upperHarmonics, *intervals, keynote, octave, ema);
    
    
    String emaRes(ema, 3, false); emaRes.append("hz", 3);
    labelEMAResult->setText(emaRes, dontSendNotification);
    // only update if db is above -50db
    
    if (peakDB > -50.f)
    {
        String freqResult(peakFrequency, 3, false);
        freqResult.append(" hz", 3);
        
        labelPeakFrequencyResult->setText(freqResult, dontSendNotification);
        
        String dbResult(peakDB, 3, false);
        dbResult.append(" dB", 3);
        
        labelPeakDBResult->setText(dbResult, dontSendNotification);
        
        // harmonics
        
            String upperHarmonicString;
            String upperHarmonicStrings[5];
            String lowerHarmonicString;
            String lowerHarmonicStrings[5];

            for (int i = 0; i < 5; i++)
            {
                upperHarmonicStrings[i] = String(upperHarmonics->getReference(i+1), 1, false);
                upperHarmonicStrings[i].append(" - ", 3);
                upperHarmonicString.append(upperHarmonicStrings[i], 9);

                labelUpperHarmonicsResult->setText(upperHarmonicString, dontSendNotification);

                lowerHarmonicStrings[i] = String(intervals->getReference(i+1), 1, false);
                lowerHarmonicStrings[i].append(" - ", 3);
                lowerHarmonicString.append(lowerHarmonicStrings[i], 9);

                labelLowerHarmonicsResult->setText(lowerHarmonicString, dontSendNotification);


            }
        
        // keynote
        int midiNote = -1;
        float freqDif = 0.f;
        
        projectManager->frequencyManager->getMIDINoteForFrequency(peakFrequency, midiNote, keynote, octave, freqDif);
        
        if (midiNote >= 0)
        {
            String keynoteString(ProjectStrings::getKeynoteArray().getReference(keynote));
            
            if (freqDif > 0.f)
            {
                keynoteString.append(" +", 3);
            }
            else
            {
                keynoteString.append(" ", 3);
            }
            
            keynoteString.append(String(freqDif, 3, false), 7);
            keynoteString.append(" hz", 3);
            
            labelKeynoteResult->setText(keynoteString, dontSendNotification);
            
            labelOctaveResult->setText(String(octave), dontSendNotification);
        }

    }
}


#pragma mark Octave Analyzer

OctaveVisualiserComponent2::OctaveVisualiserComponent2(ProjectManager * pm)
{
    projectManager  = pm;
    sampleRate      = projectManager->getSampleRate();
    numOctaves      = 10;
    numBands        = 32;
}

OctaveVisualiserComponent2::~OctaveVisualiserComponent2(){}

void OctaveVisualiserComponent2::mouseDown (const MouseEvent& event)
{
    displayData = true;
    displayDataPoint = event.position;
}

void OctaveVisualiserComponent2::mouseDrag (const MouseEvent& event)
{
    displayDataPoint = event.position;
}

void OctaveVisualiserComponent2::mouseUp (const MouseEvent& event)
{
    displayData = false;
}

void OctaveVisualiserComponent2::paint (Graphics&g)
{
    Array<float> magnitudes;
    
//    projectManager->createOctaveMagnitudes(magnitudes, numBands, kDefaultMinHertz, sampleRate / 2.f, centralFrequencies );
    int fftRef = (int)visualiserSource - 1;
    projectManager->createOctaveMagnitudes(fftRef, magnitudes, numBands, kDefaultMinHertz, sampleRate / 2.f, centralFrequencies );

    float space = 2 * scaleFactor;
    float bandW = (getWidth() / numBands);
    
    // set background colour
    g.setColour(Colours::black);
    g.fillAll();
    
    for (int i = 0; i < numBands; i++)
    {
        float x     = bandW * i;
        float mag   = magnitudes.getReference(i);
        
        // catch bad mag / inf+
        if (std::isnan(mag) || std::isinf(mag))
        {
            mag = 0.f;
        }

        float top   = magnitudeToY(mag); //dbValue or magnitude
        float right = x + bandW - space;
        
        Rectangle<float> barRect(Rectangle<float>::leftTopRightBottom(x, getHeight() - top, right, getHeight()));
        
        Colour barColour(projectManager->getSettingsColorParameter(2));
        
        g.setColour(barColour);
        g.fillRect(barRect);
        g.setColour(Colours::darkgrey);
        g.setOpacity (1.0);
        g.drawRect(barRect);
        
        
        
        
        if (getWidth() > 800)
        {
            g.setColour(Colours::lightgrey);
            // draw central freq @ bottom
            float textH = 20;
            String freqString(centralFrequencies.getReference(i), 1, false);
            freqString.append("hz", 2);
            g.setFont(textH * scaleFactor);
            g.drawText(freqString, x, getHeight() - (textH * scaleFactor), bandW, textH * scaleFactor, Justification::centred);
        }
    }
    
    g.setColour(Colours::lightgrey);
    g.setOpacity (0.7);
    g.drawRoundedRectangle(0, 0, getWidth(), getHeight(), 0, 4 * scaleFactor);
    
    drawDBLinesNew(g);
    
    if (displayData)
    {
        drawDataDisplay(g);
    }
}

void OctaveVisualiserComponent2::drawDBLinesNew(Graphics&g)
{
    int numDivs = 5;
    
    for (int i = 0 ; i < numDivs; i++)
    {
        g.setColour(Colours::lightgrey);
        g.setOpacity(0.6);
        g.drawLine(0, (getHeight() / numDivs) * i, getWidth(), (getHeight() / numDivs)*i, 1 * scaleFactor);
        
        int db = (int)getDBForY((getHeight() / numDivs) * i, 0, getHeight());
        String dbString(db);
        String db2String(" dB");
        dbString.append(db2String, 3);

        float fontSize = 24;
        float textWidth = 120 * scaleFactor;
        
        g.setFont(fontSize*scaleFactor);
        g.drawText(dbString, getWidth() - textWidth, (getHeight() / numDivs) * i, textWidth, fontSize * scaleFactor, Justification::right);
    }
}


void OctaveVisualiserComponent2::drawDataDisplay(Graphics &g)
{
    float boxWidth = 150 * scaleFactor;
    float boxHeight = 50 * scaleFactor;
    
    g.setColour(Colours::white);
    
    g.drawLine(0, displayDataPoint.y, getWidth(), displayDataPoint.y, 1.f * scaleFactor);
    g.drawLine(displayDataPoint.x, 0, displayDataPoint.x, getHeight(), 1.f * scaleFactor);

    float fontSize = 24 * scaleFactor;

    float position = displayDataPoint.x;
    float bandW = getWidth() / numBands;

    int band = 0;
    for (int i = 0; i < numBands; i++)
    {
        if (position >= (bandW * i) && position < (bandW * (i+1)))
        {
            band = i;
        }
    }

    float centralFreq = 0.f;

    if (!centralFrequencies.isEmpty())
    {
        centralFreq = centralFrequencies.getReference(band);
    }


    String freqString(centralFreq, 1, false);
    String hzString(" Hz");
    freqString.append(hzString, 3);

    float positiondb = displayDataPoint.y;
    float db = getDBForY(positiondb, 0, getHeight());
    String dbString(db, 1, false);
    String db2String(" dB");
    dbString.append(db2String, 3);
    
    String bandString(String::formatted("Band %i", band+1));
    
    
    float leftBorder = 3;
    float topBorder = 3;
    g.setFont(fontSize);
    float drawPositionX = displayDataPoint.x;
    
    if (drawPositionX < boxWidth)
    {
        g.drawText(freqString, drawPositionX + leftBorder, displayDataPoint.y + topBorder, boxWidth - (leftBorder * 2), boxHeight / 2 - (topBorder*2), Justification::left);
        
        g.drawText(dbString, drawPositionX + leftBorder, displayDataPoint.y + topBorder + fontSize, boxWidth - (leftBorder * 2), boxHeight / 2 - (topBorder*2), Justification::left);
        
        g.drawText(bandString, drawPositionX + leftBorder, displayDataPoint.y + topBorder + fontSize + fontSize, boxWidth - (leftBorder * 2), boxHeight / 2 - (topBorder*2), Justification::left);
    }
    else
    {
        drawPositionX-= boxWidth;
        
        g.drawText(freqString, drawPositionX + leftBorder, displayDataPoint.y + topBorder, boxWidth - (leftBorder * 2), boxHeight / 2 - (topBorder*2), Justification::right);
        
        g.drawText(dbString, drawPositionX + leftBorder, displayDataPoint.y + topBorder + fontSize, boxWidth - (leftBorder * 2), boxHeight / 2 - (topBorder*2), Justification::right);
        
        g.drawText(bandString, drawPositionX + leftBorder, displayDataPoint.y + topBorder + fontSize + fontSize, boxWidth - (leftBorder * 2), boxHeight / 2 - (topBorder*2), Justification::right);
    }
    
}

inline float OctaveVisualiserComponent2::magnitudeToY (float magnitude) const
{
    const float infinity = -80.0f;
    return jmap (Decibels::gainToDecibels (magnitude, infinity),
                 infinity, kDefaultMaxDbFS, 0.0f, (float)getHeight());
}

float OctaveVisualiserComponent2::getDBForY(float yPos, float top, float bottom)
{
    const float infinity = -80.0f;
    
    float result = jmap(yPos, bottom, top, infinity, kDefaultMaxDbFS);
    
    return result;
}


void OctaveVisualiserComponent2::resized()
{
    // should prob recreate the images with correct sizes...
//    if (getWidth() != imageBackground->getWidth())
//    {
//        imageBackground         = new Image(Image::RGB, getWidth(), getHeight(), true);
//        imageMainSpectrumPlot   = new Image(Image::RGB, getWidth(), getHeight(), true);
//        plotHistory             = new Image(Image::RGB, getWidth(), getHeight(), true);
//        
//        shouldDrawLines         = true;
//    }
}

void OctaveVisualiserComponent2::pushUpdate()
{
    fftSize                 = projectManager->getFFTSize();
    sampleRate              = projectManager->getSampleRate();
    numOctaves              = (int)(log2(sampleRate / 2.f) - log2(kDefaultMinHertz));
    
    repaint();
}








#pragma mark Spectrum Analyzer
SpectrogramComponent::SpectrogramComponent(ProjectManager * pm, Rectangle<float> initialSize, bool popup)
{
    isPopup                 = popup;
    projectManager          = pm;
    shouldUpdateSpectrum    = false;
    fftSize                 = projectManager->getFFTSize();
    sampleRate              = projectManager->getSampleRate();
    
    plotHistory             = Image(Image::RGB, initialSize.getWidth(), initialSize.getHeight(), true);
    imageMainSpectrumPlot   = new Image(Image::RGB, initialSize.getWidth(), initialSize.getHeight(), true);
    
    
    if (isPopup)
    {
        imageBackground = new Image(Image::RGB, initialSize.getWidth(), initialSize.getHeight(), true);
    }
    else
    {
        imageBackground = new Image(Image::RGB, initialSize.getWidth(), initialSize.getHeight(), true);
    }
    
    p.preallocateSpace (8 + 32768);
}

SpectrogramComponent::~SpectrogramComponent()
{
    
}

void SpectrogramComponent::resized()
{
    if (isPopup)
    {
        // should prob recreate the images with correct sizes...
        if (getWidth() != imageBackground->getWidth())
        {
            imageBackground         = new Image(Image::RGB, getWidth(), getHeight(), true);
            imageMainSpectrumPlot   = new Image(Image::RGB, getWidth(), getHeight(), true);
            plotHistory             =  Image(Image::RGB, getWidth(), getHeight(), true);
            
            shouldDrawLines         = true;
        }
    }
    else
    {
        // should prob recreate the images with correct sizes...
        if (getWidth() != imageBackground->getWidth())
        {
            imageBackground         = new Image(Image::RGB, getWidth(), getHeight(), true);
            imageMainSpectrumPlot   = new Image(Image::RGB, getWidth(), getHeight(), true);
            plotHistory             = Image(Image::RGB, getWidth(), getHeight(), true);
            
//            plotHistory.rescaled(getWidth(), getHeight());
            
            shouldDrawLines         = true;
        }
    }
    
    repaint();
}

void SpectrogramComponent::mouseDown (const MouseEvent& event)
{
    displayData         = true;
    displayDataPoint    = event.position;
}

void SpectrogramComponent::mouseDrag (const MouseEvent& event)
{
    displayDataPoint    = event.position;
}

void SpectrogramComponent::mouseUp (const MouseEvent& event)
{
    displayData = false;
}

void SpectrogramComponent::pushUpdate()
{
    fftSize                 = projectManager->getFFTSize();
    
    if (sampleRate != projectManager->getSampleRate())
    {
        sampleRate              = projectManager->getSampleRate();
        shouldDrawLines         = true;
    }
    
    shouldUpdateSpectrum    = true;
    
    repaint();
}

void SpectrogramComponent::paint (Graphics&g) 
{
    g.setColour(Colours::black);
    g.fillAll();
    
    drawLayersNew(g);
}

void SpectrogramComponent::drawLayersNew(Graphics &g)
{
    drawBackground(g);

    
    if(shouldUpdateSpectrum)
    {
        Image newSpectrumPlot(Image::RGB, getWidth(), getHeight(), true);
        
        Graphics g_plot(newSpectrumPlot);
        drawSpectrumLines(g_plot);
        
        shouldUpdateSpectrum = false;

        // work out delay
        double delayMS              = projectManager->getProjectSettingsParameter(FFT_DELAY_MS);
        float numFramesRequired     = (delayMS / 1000.f) * TIMER_UPDATE_RATE;
        double pAlpha               = pow(0.1, 1.f/ numFramesRequired);
        
        plotHistory.multiplyAllAlphas(pAlpha);
        Graphics g_history(plotHistory);
        
        Colour delayColour(projectManager->getSettingsColorParameter(1));
        g_history.setColour (delayColour);
        g_history.drawImageAt (newSpectrumPlot, 0, 0, true);
        
        g.drawImage(plotHistory, 0, 0, getWidth(), getHeight(), 0, 0, imageMainSpectrumPlot->getWidth(), imageMainSpectrumPlot->getHeight());
    
        g.drawImage(newSpectrumPlot, 0, 0, getWidth(), getHeight(), 0, 0, imageMainSpectrumPlot->getWidth(), imageMainSpectrumPlot->getHeight());
        
    }
    
//    drawBackground(g);
//    g.setColour(Colours::transparentWhite);
//    g.setOpacity(1.f);
    
    drawDBLinesNew(g);  // Optimsation.. put into bitmap to reduce calculations
    drawFreqLinesNew(g); // as above // only redraw upon cnage of view size, and then only when the size has settled.. otherwise it should stretch..
    
    if (displayData)
        drawDataDisplay(g);
    
    
    g.drawRoundedRectangle(0, 0, getWidth(), getHeight(), 0, 8 * scaleFactor);
    
    
}


void SpectrogramComponent::drawBackground(Graphics &g)
{
    // set background colour
    
    g.setColour(Colours::lightgrey);
//    g.setOpacity (0.7);
    
    

}


void SpectrogramComponent::drawSpectrumLines(Graphics& g)
{
    createPathNew(g);
}

void SpectrogramComponent::drawDataDisplay(Graphics &g)
{
    if (isPopup)
    {
        // simple Text for Db and Frequency
        // draw inside little box
        // above pointer, or below if pointer if at the top
        // to the right of the pointer unless it is to the right
        float boxWidth = 150 * scaleFactor;
        float boxHeight = 50 * scaleFactor;

        g.setColour(Colours::white);
        
        g.drawLine(0, displayDataPoint.y, getWidth(), displayDataPoint.y, 1.f * scaleFactor);
        g.drawLine(displayDataPoint.x, 0, displayDataPoint.x, getHeight(), 1.f * scaleFactor);
        
        float fontSize = 24 * scaleFactor;
        
        float position = displayDataPoint.x;
        
        
        Justification justify(Justification::left);
        
        float freq = getFrequencyForPositionPopup(position / getWidth());
        String freqString(freq, 1, false);
        String hzString(" Hz");
        freqString.append(hzString, 3);
        
        float positiondb = displayDataPoint.y;
        float db = getDBForY(positiondb, 0, getHeight());
        String dbString(db, 1, false);
        String db2String(" dB");
        dbString.append(db2String, 3);
        
        float leftBorder = 3;
        float topBorder = 3;
        g.setFont(fontSize);
        float drawPositionX = displayDataPoint.x;
        
        if (drawPositionX < boxWidth)
        {
            g.drawText(freqString, drawPositionX + leftBorder, displayDataPoint.y + topBorder, boxWidth - (leftBorder * 2), boxHeight / 2 - (topBorder*2), Justification::left);
            
            g.drawText(dbString, drawPositionX + leftBorder, displayDataPoint.y + topBorder + fontSize, boxWidth - (leftBorder * 2), boxHeight / 2 - (topBorder*2), Justification::left);
        }
        else
        {
            drawPositionX-= boxWidth;
            
            g.drawText(freqString, drawPositionX + leftBorder, displayDataPoint.y + topBorder, boxWidth - (leftBorder * 2), boxHeight / 2 - (topBorder*2), Justification::right);
            
            g.drawText(dbString, drawPositionX + leftBorder, displayDataPoint.y + topBorder + fontSize, boxWidth - (leftBorder * 2), boxHeight / 2 - (topBorder*2), Justification::right);
        }
    }
    else
    {
        // simple Text for Db and Frequency
        // draw inside little box
        // above pointer, or below if pointer if at the top
        // to the right of the pointer unless it is to the right
        float boxWidth = 150 * scaleFactor;
        float boxHeight = 50 * scaleFactor;
        
        float xText = 0;
        
        g.setColour(Colours::white);
        
        g.drawLine(0, displayDataPoint.y, getWidth(), displayDataPoint.y, 1.f * scaleFactor);
        g.drawLine(displayDataPoint.x, 0, displayDataPoint.x, getHeight(), 1.f * scaleFactor);
        
        float fontSize = 24 * scaleFactor;
        
        float position = displayDataPoint.x;
        
        
        Justification justify(Justification::left);
        
        float freq = getFrequencyForPosition(position / getWidth());
        String freqString(freq, 1, false);
        String hzString(" Hz");
        freqString.append(hzString, 3);
        
        float positiondb = displayDataPoint.y;
        float db = getDBForY(positiondb, 0, getHeight());
        String dbString(db, 1, false);
        String db2String(" dB");
        dbString.append(db2String, 3);
        
        float leftBorder = 3;
        float topBorder = 3;
        g.setFont(fontSize);
        float drawPositionX = displayDataPoint.x;
        
        if (drawPositionX < boxWidth)
        {
            g.drawText(freqString, drawPositionX + leftBorder, displayDataPoint.y + topBorder, boxWidth - (leftBorder * 2), boxHeight / 2 - (topBorder*2), Justification::left);
            
            g.drawText(dbString, drawPositionX + leftBorder, displayDataPoint.y + topBorder + fontSize, boxWidth - (leftBorder * 2), boxHeight / 2 - (topBorder*2), Justification::left);
        }
        else
        {
            drawPositionX-= boxWidth;
            
            g.drawText(freqString, drawPositionX + leftBorder, displayDataPoint.y + topBorder, boxWidth - (leftBorder * 2), boxHeight / 2 - (topBorder*2), Justification::right);
            
            g.drawText(dbString, drawPositionX + leftBorder, displayDataPoint.y + topBorder + fontSize, boxWidth - (leftBorder * 2), boxHeight / 2 - (topBorder*2), Justification::right);
        }
    }
}

void SpectrogramComponent::drawDBLinesNew(Graphics&g)
{
    if (isPopup)
    {
        int numDivs = 5;
        
        for (int i = 0 ; i < numDivs; i++)
        {
            g.setColour(Colours::lightgrey);
            g.setOpacity(0.6);
            g.drawLine(0, (getHeight() / numDivs) * i, getWidth(), (getHeight() / numDivs)*i, 1 * scaleFactor);
            
            float db = getDBForYPopup((getHeight() / numDivs) * i, 0, getHeight());
            String dbString(db, 1, false);
            String db2String(" dB");
            dbString.append(db2String, 3);

            float fontSize = 24;
            float textWidth = 120 * scaleFactor;
            
            g.setFont(fontSize*scaleFactor);
            g.drawText(dbString, getWidth() - textWidth, (getHeight() / numDivs) * i, textWidth, fontSize * scaleFactor, Justification::right);
        }
    }
    else
    {
        int numDivs = 5;
        
        for (int i = 0 ; i < numDivs; i++)
        {
            g.setColour(Colours::lightgrey);
            g.setOpacity(0.6);
            g.drawLine(0, (getHeight() / numDivs) * i, getWidth(), (getHeight() / numDivs)*i, 1 * scaleFactor);
            
            float db = getDBForY((getHeight() / numDivs) * i, 0, getHeight());
            String dbString(db, 1, false);
            String db2String(" dB");
            dbString.append(db2String, 3);

            float fontSize = 24;
            float textWidth = 120 * scaleFactor;
            
            g.setFont(fontSize*scaleFactor);
            g.drawText(dbString, getWidth() - textWidth, (getHeight() / numDivs) * i, textWidth, fontSize * scaleFactor, Justification::right);
        }
    }

}

void SpectrogramComponent::drawFreqLinesNew(Graphics&g)
{
    if (isPopup)
    {
        int numDivs = 10;
        
        for (int i = 1 ; i < numDivs+1; i++)
        {
            g.setColour(Colours::lightgrey);
            g.setOpacity(0.6);
            g.drawLine((getWidth() / numDivs) * i, 0, (getWidth() / numDivs) * i, getHeight(), 1 * scaleFactor);

            float position = (getWidth() / numDivs) * i;
            float freq = getFrequencyForPositionPopup(position / getWidth());
            
            
            String freqString(freq, 1, false);
            String freq2String(" Hz");
            freqString.append(freq2String, 3);

            float fontSize = 24;
            float textWidth = 120 * scaleFactor;
            g.setFont(fontSize*scaleFactor);
            g.drawText(freqString,((getWidth() / numDivs) * i) - textWidth, getHeight() - (fontSize*scaleFactor), textWidth, fontSize*scaleFactor, Justification::right);
        }
    }
    else
    {
        int numDivs = 10;
        
        for (int i = 1 ; i < numDivs+1; i++)
        {
            g.setColour(Colours::lightgrey);
            g.setOpacity(0.6);
            g.drawLine((getWidth() / numDivs) * i, 0, (getWidth() / numDivs) * i, getHeight(), 1 * scaleFactor);

            float position = (getWidth() / numDivs) * i;
            float freq = getFrequencyForPosition(position / getWidth());

            String freqString(freq, 1, false);
            String freq2String(" Hz");
            freqString.append(freq2String, 3);

            float fontSize = 24;
            float textWidth = 120 * scaleFactor;
            g.setFont(fontSize*scaleFactor);
            g.drawText(freqString,((getWidth() / numDivs) * i) - textWidth, getHeight() - (fontSize*scaleFactor), textWidth, fontSize*scaleFactor, Justification::right);
        }
    }
}


void SpectrogramComponent::createPathNew (Graphics&g)
{
    if (isPopup)
    {
        // need to get bounds - topMargin, and plot it correctly
//        projectManager->createAnalyserPlotOptimisedWithRange(p, getBounds(), zoomRange_FreqLow, zoomRange_FreqHigh,  zoomRange_AmplitudeLow, zoomRange_AmplitudeHigh,   false);
        
        int fftRef = (int)visualiserSource - 1;
        projectManager->createAnalyserPlotOptimisedWithRange(fftRef, p, getBounds(), zoomRange_FreqLow, zoomRange_FreqHigh,  zoomRange_AmplitudeLow, zoomRange_AmplitudeHigh,   false);
        
    }
    else
    {
//        projectManager->createAnalyserPlot(p, getBounds(), kDefaultMinHertz, sampleRate / 2.f, false);
        
        int fftRef = (int)visualiserSource - 1;
        projectManager->createAnalyserPlot(fftRef, p, getBounds(), kDefaultMinHertz, sampleRate / 2.f, false);
    }
    
    Colour mainColour(projectManager->getSettingsColorParameter(0));

//    g.setOpacity(1.f);
    g.setColour(mainColour);
    g.fillPath(p);
//    g.setColour(Colour::fromRGBA(70, 240, 70, 200));
//    g.strokePath(p, PathStrokeType(1));

}







// drawing functions
float SpectrogramComponent::getFrequencyForPosition (float pos)
{
    sampleRate      = projectManager->getSampleRate();
    
    float octaves   =  (log2(sampleRate / 2.f) - log2(kDefaultMinHertz));
    
    return kDefaultMinHertz * std::pow (2.0f, pos * octaves); // only doing 10 octaves...
}

float SpectrogramComponent::getDBForY(float yPos, float top, float bottom)
{
    const float infinity = kDefaultMinDbFS;
    
    float result = jmap(yPos, bottom, top, infinity, kDefaultMaxDbFS);
    
    return result;
}

float SpectrogramComponent::getFrequencyForPositionPopup (float pos)
{
    float octaves =  (log2(zoomRange_FreqHigh) - log2(zoomRange_FreqLow));
    
    return zoomRange_FreqLow * std::pow (2.0f, pos * octaves); 
}

float SpectrogramComponent::getDBForYPopup(float yPos, float top, float bottom)
{
    float result = jmap(yPos, bottom, top, (float)zoomRange_AmplitudeLow, (float)zoomRange_AmplitudeHigh);
    
    return result;
}


// Plot functions
String SpectrogramComponent::stringForValue(double value, bool divide, bool decimals)
{
    String theString;
    double temp = value;
    
    if(divide && value >= 1000) temp = temp / 1000;
    
    temp = (floor(temp *10))/10;    // chop everything after 1 decimal place
    
    //if we do not have trailing zeros or don't want decimal
    if (decimals == false || floor(temp) == temp)
    {
        theString = String(temp);
    }
    else
    {
        theString = String(temp);
    }
    
    return theString;
}





#pragma mark Colour Spectrum Analyzer

ColourSpectrumVisualiserComponent::ColourSpectrumVisualiserComponent(ProjectManager * pm, int initW, int initH) /*: spectrogramImage (Image::RGB, initW, initH, true)*/
{
    
    projectManager      = pm;
    sampleRate          = projectManager->getSampleRate();
    fftSize             = projectManager->getFFTSize();
    newSpectrogramImage = new Image (Image::RGB, initW, initH, true);
    
    
}

ColourSpectrumVisualiserComponent::~ColourSpectrumVisualiserComponent(){}

void ColourSpectrumVisualiserComponent::mouseDown (const MouseEvent& event)
{
    displayData = true;
    displayDataPoint = event.position;
}

void ColourSpectrumVisualiserComponent::mouseDrag (const MouseEvent& event)
{
    displayDataPoint = event.position;
}

void ColourSpectrumVisualiserComponent::mouseUp (const MouseEvent& event)
{
    displayData = false;
}


void ColourSpectrumVisualiserComponent::paint (Graphics&g)
{
    // set background colour
    g.setColour(Colours::black);
    g.fillAll();
    
    g.setOpacity (1.0f);
    
    
    g.drawImage (*newSpectrogramImage, getLocalBounds().toFloat());
    
    drawFrequencyLines(g);
    drawTimelineGrid(g);
    
    if (displayData)
    {
        drawDisplayData(g);
    }
    
    g.setColour(Colours::lightgrey);
    g.setOpacity (1.0);
    g.drawRoundedRectangle(0, 0, getWidth(), getHeight(), 0, 4 * scaleFactor);

}

void ColourSpectrumVisualiserComponent::drawDisplayData(Graphics &g)
{
    float boxWidth = 150 * scaleFactor;
    float boxHeight = 50 * scaleFactor;
    
    g.setColour(Colours::white);
    
    g.drawLine(0, displayDataPoint.y, getWidth(), displayDataPoint.y, 1.f * scaleFactor);
    g.drawLine(displayDataPoint.x, 0, displayDataPoint.x, getHeight(), 1.f * scaleFactor);
    
    float fontSize = 24 * scaleFactor;
    
    float y                     = getHeight() - displayDataPoint.y;
    float yScale                = y / getHeight();
//    float freq                  = powf(10.f, yScale * (log10f(sampleRate / 2.f) - log10f(kDefaultMinHertz)) + log10f(kDefaultMinHertz));
    float freq                  = powf(10.f, yScale * (log10f(zoomRange_FreqHigh)-log10f(zoomRange_FreqLow)) + log10f(zoomRange_FreqLow));

    String freqString(freq, 1, false);
    String hzString(" Hz");
    freqString.append(hzString, 3);
    
    float leftBorder = 3;
    float topBorder = 3;
    
    g.setFont(fontSize);
    g.drawText(freqString, displayDataPoint.x - boxWidth + leftBorder, displayDataPoint.y + topBorder, boxWidth - (leftBorder * 2), boxHeight / 2 - (topBorder*2), Justification::right);

}


void ColourSpectrumVisualiserComponent::drawFrequencyLines(Graphics &g)
{
    int numDivs = 5;
    
    for (int i = 0 ; i < numDivs; i++)
    {
        g.setColour(Colours::lightgrey);
        g.setOpacity(0.45);
        g.drawLine(0, (getHeight() / numDivs) * i, getWidth(), (getHeight() / numDivs)*i, 1 * scaleFactor);

        float y                     = getHeight() - ((getHeight() / numDivs)*i);
        float yScale                = y / getHeight();
//        float freq                  = powf(10.f, yScale * (log10f(sampleRate / 2.f)-log10f(kDefaultMinHertz)) + log10f(kDefaultMinHertz));
        
        float freq                  = powf(10.f, yScale * (log10f(zoomRange_FreqHigh)-log10f(zoomRange_FreqLow)) + log10f(zoomRange_FreqLow));

        int freqhz   = (int)freq;
        String freqString(freqhz);
        String freq2String("hz");
        freqString.append(freq2String, 3);

        float fontSize = 24;
        float textWidth = 120 * scaleFactor;
        
        g.setFont(fontSize*scaleFactor);
        g.drawText(freqString, 0, (getHeight() / numDivs) * i, textWidth, fontSize * scaleFactor, Justification::left);
    }
}

void ColourSpectrumVisualiserComponent::drawTimelineGrid(Graphics &g)
{
    // draw line and text every 0.1 second ?
    
    int numPixelsPerSec = TIMER_UPDATE_RATE;
    
    for (int sec = 11; (sec * numPixelsPerSec) < getWidth(); sec+=10)
    {
        int w = sec * numPixelsPerSec;
        
        int x = getWidth() - w;
        
        g.setColour(Colours::lightgrey);
        g.setOpacity(0.45);
        g.drawLine(x, 0, x, getHeight(), 1 * scaleFactor);
        

        
        String freqString(sec-1);
        String freq2String("secs");
        freqString.append(freq2String, 4);

        float fontSize = 24;
        float textWidth = 120 * scaleFactor;

        g.setFont(fontSize*scaleFactor);
        g.drawText(freqString, getWidth() - (w + textWidth), getHeight() - (fontSize * scaleFactor), textWidth, fontSize * scaleFactor, Justification::right);
        
    }

}

void ColourSpectrumVisualiserComponent::resized()
{
    Image rescaledImage(newSpectrogramImage->createCopy());
    newSpectrogramImage = new Image(rescaledImage.rescaled(getWidth(), getHeight()));
}

void ColourSpectrumVisualiserComponent::pushUpdate()
{
    fftSize     = projectManager->getFFTSize();
    sampleRate  = projectManager->getSampleRate();
    
    drawNextLineOfSpectrogram();
    
    repaint();
}

void ColourSpectrumVisualiserComponent::drawNextLineOfSpectrogram()
{
//    projectManager->createColourSpectrum(*newSpectrogramImage, zoomRange_FreqLow, zoomRange_FreqHigh, 0.2);
    int fftRef = (int)visualiserSource - 1;
    
    projectManager->createColourSpectrum(fftRef, *newSpectrogramImage, zoomRange_FreqLow, zoomRange_FreqHigh, 0.2);
    
    
//    projectManager->createColourSpectrum(*newSpectrogramImage, kDefaultMinHertz, sampleRate / 2.f, 0.2);
}
