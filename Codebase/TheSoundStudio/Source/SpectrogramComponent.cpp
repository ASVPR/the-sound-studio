/*
  ==============================================================================

    SpectrogramComponent.cpp
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "SpectrogramComponent.h"
#include "Parameters.h"
#include "vConstants.h"
#include "ResponsiveUIHelper.h"
#include "UI/DesignSystem.h"

namespace
{
float scaledWithMin(float baseValue, float scaleFactor, float minValue)
{
    return jmax(minValue, baseValue * scaleFactor);
}

float getAxisFontSize(float scaleFactor)
{
    return ResponsiveUIHelper::getReadableFontSize(
        24.0f, scaleFactor, TSS::Design::Usability::visualiserAxisMinFont);
}

float getValueFontSize(float scaleFactor)
{
    return ResponsiveUIHelper::getReadableFontSize(
        24.0f, scaleFactor, TSS::Design::Usability::visualiserValueMinFont);
}

float getLegendFontSize(float scaleFactor)
{
    return ResponsiveUIHelper::getReadableFontSize(
        14.0f, scaleFactor, TSS::Design::Usability::visualiserLegendMinFont);
}
} // namespace


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
    
    labelKeynote = std::make_unique<Label>();
    labelKeynote->setText("Keynote", dontSendNotification);
    labelKeynote->setJustificationType(Justification::left);
    addAndMakeVisible(labelKeynote.get());
    
    labelPeakFrequency = std::make_unique<Label>();
    labelPeakFrequency->setText("Peak Frequency", dontSendNotification);
    labelPeakFrequency->setJustificationType(Justification::left);
    addAndMakeVisible(labelPeakFrequency.get());
    
    labelPeakDB = std::make_unique<Label>();
    labelPeakDB->setText("Peak dB", dontSendNotification);
    labelPeakDB->setJustificationType(Justification::left);
    addAndMakeVisible(labelPeakDB.get());
    
    labelEMA = std::make_unique<Label>();
    labelEMA->setText("EMA", dontSendNotification);
    labelEMA->setJustificationType(Justification::left);
    addAndMakeVisible(labelEMA.get());
    
    labelEMAResult = std::make_unique<Label>();
    labelEMAResult->setText("0.0hz", dontSendNotification);
    labelEMAResult->setJustificationType(Justification::left);
    addAndMakeVisible(labelEMAResult.get());
    
    labelOctave = std::make_unique<Label>();
    labelOctave->setText("Octave", dontSendNotification);
    labelOctave->setJustificationType(Justification::left);
    addAndMakeVisible(labelOctave.get());
    
    labelUpperHarmonics = std::make_unique<Label>();
    labelUpperHarmonics->setText("Harmonics + ", dontSendNotification);
    labelUpperHarmonics->setJustificationType(Justification::left);
    addAndMakeVisible(labelUpperHarmonics.get());
    
    labelLowerHarmonics = std::make_unique<Label>();
    labelLowerHarmonics->setText("Intervals ", dontSendNotification);
    labelLowerHarmonics->setJustificationType(Justification::left);
    addAndMakeVisible(labelLowerHarmonics.get());
    
    labelKeynoteResult = std::make_unique<Label>();
    labelKeynoteResult->setText("C#", dontSendNotification);
    labelKeynoteResult->setJustificationType(Justification::left);
    addAndMakeVisible(labelKeynoteResult.get());
    
    labelPeakFrequencyResult = std::make_unique<Label>();
    // MAINTAINABILITY: Using named constant instead of hardcoded "432.333" for consistent frequency reference
    labelPeakFrequencyResult->setText(String(AudioConstants::ALTERNATIVE_A4_FREQ, 3) + " hz", dontSendNotification);
    labelPeakFrequencyResult->setJustificationType(Justification::left);
    addAndMakeVisible(labelPeakFrequencyResult.get());
    
    labelPeakDBResult = std::make_unique<Label>();
    labelPeakDBResult->setText(" -6.6 dB", dontSendNotification);
    labelPeakDBResult->setJustificationType(Justification::left);
    addAndMakeVisible(labelPeakDBResult.get());
    
    labelUpperHarmonicsResult = std::make_unique<Label>();
    labelUpperHarmonicsResult->setText("", dontSendNotification);
    labelUpperHarmonicsResult->setJustificationType(Justification::left);
    addAndMakeVisible(labelUpperHarmonicsResult.get());
    
    labelLowerHarmonicsResult = std::make_unique<Label>();
    labelLowerHarmonicsResult->setText("", dontSendNotification);
    labelLowerHarmonicsResult->setJustificationType(Justification::left);
    addAndMakeVisible(labelLowerHarmonicsResult.get());
    
    labelOctaveResult = std::make_unique<Label>();
    labelOctaveResult->setText("2", dontSendNotification);
    labelOctaveResult->setJustificationType(Justification::left);
    addAndMakeVisible(labelOctaveResult.get());
    
    upperHarmonics  = new Array<float> (6);
    intervals       = new Array<float> (6);
    for (int i = 0; i < 6; i++)
    {
        float c = 0.f;
        upperHarmonics->add(c);
        intervals->add(c);
    }


}

FrequencyDataComponent::~FrequencyDataComponent()
{
    // CRITICAL: Stop timer before destruction to prevent race condition
    // where timer callback accesses member variables during destruction
    stopTimer();
}

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
    // Safety check: projectManager must be valid
    if (projectManager == nullptr)
        return;

    fftSize                 = projectManager->getFFTSize();
    sampleRate              = projectManager->getSampleRate();

    // Validate sample rate before proceeding
    if (sampleRate <= 0.0f)
        return;

    double ema = 0.0;
//    projectManager->createFrequencyData(peakFrequency, peakDB, *upperHarmonics, *intervals, keynote, octave, ema);

    const int fftRef = static_cast<int>(visualiserSource) - 1;
    if (fftRef < 0 || fftRef >= 8)
        return;

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

        // Safety check: frequencyManager must be valid
        if (projectManager->frequencyManager == nullptr)
            return;

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
    : projectManager(pm)
    , sampleRate(44100.0)  // Safe default
    , numOctaves(10)
    , numBands(32)
{
    // Get actual sample rate if projectManager is valid
    if (projectManager != nullptr)
    {
        const double sr = projectManager->getSampleRate();
        if (sr > 0.0)
            sampleRate = sr;
    }
}

OctaveVisualiserComponent2::~OctaveVisualiserComponent2()
{
    // CRITICAL: Stop timer before destruction to prevent race condition
    stopTimer();
}

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

// FFT Improvements: Add zoom functionality
void OctaveVisualiserComponent2::didSwipe (float deltaX, float deltaY, Point<float> position)
{
    // Delta X (horizontal zoom/pan)
    if (range_X_Min_Factor + deltaX >= 0 && range_X_Min_Factor + deltaX <= 0.9)
    {
        range_X_Min_Factor += deltaX;
    }
    
    if (range_X_Max_Factor + deltaX >= 0.1 && range_X_Max_Factor + deltaX <= 1.f)
    {
        range_X_Max_Factor += deltaX;
    }
    
    // Delta Y (vertical zoom)
    deltaY *= 0.5;
    
    if (range_Y_Min_Factor + deltaY >= 0 && range_Y_Min_Factor + deltaY <= 0.9)
    {
        range_Y_Min_Factor += deltaY;
    }
    
    if (range_Y_Max_Factor - deltaY >= 0.1 && range_Y_Max_Factor - deltaY <= 1.f)
    {
        range_Y_Max_Factor -= deltaY;
    }
    
    // Update zoom ranges
    zoomRange_FreqLow       = kDefaultMinHertz + (range_X_Min_Factor * (kDefaultMaxHertz - kDefaultMinHertz));
    zoomRange_FreqHigh      = kDefaultMinHertz + (range_X_Max_Factor * (kDefaultMaxHertz - kDefaultMinHertz));
    zoomRange_AmplitudeLow  = kDefaultMinDbFS + (range_Y_Min_Factor * (kDefaultMaxDbFS - kDefaultMinDbFS));
    zoomRange_AmplitudeHigh = kDefaultMinDbFS + (range_Y_Max_Factor * (kDefaultMaxDbFS - kDefaultMinDbFS));
}

void OctaveVisualiserComponent2::paint (Graphics&g)
{
    // Validate projectManager pointer
    if (projectManager == nullptr)
    {
        g.fillAll(Colours::black);
        return;
    }

    // Set background first
    g.setColour(Colours::black);
    g.fillAll();

    // Validate fftRef is in valid range (0-7)
    const int fftRef = static_cast<int>(visualiserSource) - 1;
    if (fftRef < 0 || fftRef >= 8)
        return;

    // Validate sampleRate before using
    if (sampleRate <= 0.0)
        return;

    // Get magnitude data - use local copy for thread safety
    Array<float> magnitudes;
    Array<float> localCentralFrequencies;
    int localNumBands = numBands;

    projectManager->createOctaveMagnitudes(fftRef, magnitudes, localNumBands, kDefaultMinHertz, static_cast<float>(sampleRate / 2.0), localCentralFrequencies);

    // Comprehensive safety check: ensure we have valid and consistent data
    if (localNumBands <= 0 ||
        magnitudes.size() < localNumBands ||
        localCentralFrequencies.size() < localNumBands)
    {
        return;
    }

    // Update member variable only after validation
    numBands = localNumBands;
    centralFrequencies = localCentralFrequencies;

    const float space = 2.0f * scaleFactor;
    const float bandW = static_cast<float>(getWidth()) / static_cast<float>(numBands);

    for (int i = 0; i < numBands; i++)
    {
        const float x = bandW * static_cast<float>(i);
        float mag = magnitudes[i];

        // Sanitize magnitude value - catch NaN, Inf, and negative values
        if (std::isnan(mag) || std::isinf(mag) || mag < 0.0f)
        {
            mag = 0.0f;
        }

        const float top = magnitudeToY(mag);
        const float right = x + bandW - space;

        Rectangle<float> barRect(Rectangle<float>::leftTopRightBottom(x, static_cast<float>(getHeight()) - top, right, static_cast<float>(getHeight())));

        Colour barColour(projectManager->getSettingsColorParameter(2));

        g.setColour(barColour);
        g.fillRect(barRect);
        g.setColour(Colours::darkgrey);
        g.setOpacity(1.0f);
        g.drawRect(barRect);

        // Draw frequency labels only if width is sufficient
        if (getWidth() > 800 && i < centralFrequencies.size())
        {
            g.setColour(Colours::lightgrey);
            const float textH = getAxisFontSize(scaleFactor);
            String freqString(centralFrequencies.getReference(i), 1, false);
            freqString.append("hz", 2);
            g.setFont(textH);
            g.drawText(freqString, x, static_cast<float>(getHeight()) - textH, bandW, textH, Justification::centred);
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

        const float fontSize = getAxisFontSize(scaleFactor);
        const float textWidth = jmax(120.0f * scaleFactor, fontSize * 4.5f);
        
        g.setFont(fontSize);
        g.drawText(dbString, getWidth() - textWidth, (getHeight() / numDivs) * i, textWidth, fontSize, Justification::right);
    }
}


void OctaveVisualiserComponent2::drawDataDisplay(Graphics &g)
{
    const float fontSize = getValueFontSize(scaleFactor);
    const float boxWidth = jmax(150.0f * scaleFactor, fontSize * 6.0f);
    const float boxHeight = jmax(50.0f * scaleFactor, fontSize * 2.4f);
    
    g.setColour(Colours::white);
    
    g.drawLine(0, displayDataPoint.y, getWidth(), displayDataPoint.y, 1.f * scaleFactor);
    g.drawLine(displayDataPoint.x, 0, displayDataPoint.x, getHeight(), 1.f * scaleFactor);

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

    // Bounds check: ensure band index is valid for centralFrequencies array
    if (!centralFrequencies.isEmpty() && band >= 0 && band < centralFrequencies.size())
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
        g.drawText(freqString, drawPositionX + leftBorder, displayDataPoint.y + topBorder,
                   boxWidth - (leftBorder * 2), boxHeight * 0.5f - (topBorder * 2), Justification::left);
        
        g.drawText(dbString, drawPositionX + leftBorder, displayDataPoint.y + topBorder + fontSize,
                   boxWidth - (leftBorder * 2), boxHeight * 0.5f - (topBorder * 2), Justification::left);
        
        g.drawText(bandString, drawPositionX + leftBorder, displayDataPoint.y + topBorder + fontSize + fontSize,
                   boxWidth - (leftBorder * 2), boxHeight * 0.5f - (topBorder * 2), Justification::left);
    }
    else
    {
        drawPositionX-= boxWidth;
        
        g.drawText(freqString, drawPositionX + leftBorder, displayDataPoint.y + topBorder,
                   boxWidth - (leftBorder * 2), boxHeight * 0.5f - (topBorder * 2), Justification::right);
        
        g.drawText(dbString, drawPositionX + leftBorder, displayDataPoint.y + topBorder + fontSize,
                   boxWidth - (leftBorder * 2), boxHeight * 0.5f - (topBorder * 2), Justification::right);
        
        g.drawText(bandString, drawPositionX + leftBorder, displayDataPoint.y + topBorder + fontSize + fontSize,
                   boxWidth - (leftBorder * 2), boxHeight * 0.5f - (topBorder * 2), Justification::right);
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
    // Safety check: projectManager must be valid
    if (projectManager == nullptr)
        return;

    fftSize                 = projectManager->getFFTSize();
    const double sr         = projectManager->getSampleRate();

    // Validate sample rate before using
    if (sr > 0.0)
    {
        sampleRate = sr;
        numOctaves = static_cast<int>(log2(sampleRate / 2.0) - log2(kDefaultMinHertz));
    }

    repaint();
}








#pragma mark Spectrum Analyzer
SpectrogramComponent::SpectrogramComponent(ProjectManager * pm, Rectangle<float> initialSize, bool popup)
    : isPopup(popup)
    , projectManager(pm)
    , fftSize(2048)           // Safe default
    , sampleRate(44100.0)     // Safe default
    , shouldUpdateSpectrum(false)
{
    // Get actual values if projectManager is valid
    if (projectManager != nullptr)
    {
        fftSize = projectManager->getFFTSize();
        const double sr = projectManager->getSampleRate();
        if (sr > 0.0)
            sampleRate = sr;
    }

    // Ensure valid image dimensions (at least 1x1)
    const int imgWidth = jmax(1, static_cast<int>(initialSize.getWidth()));
    const int imgHeight = jmax(1, static_cast<int>(initialSize.getHeight()));

    plotHistory             = Image(Image::RGB, imgWidth, imgHeight, true);
    imageMainSpectrumPlot   = new Image(Image::RGB, imgWidth, imgHeight, true);
    imageBackground         = new Image(Image::RGB, imgWidth, imgHeight, true);

    p.preallocateSpace (8 + 32768);
}

SpectrogramComponent::~SpectrogramComponent()
{
    // CRITICAL: Stop timer before destruction to prevent race condition
    stopTimer();
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
        if (getWidth() > 0 && getHeight() > 0 && getWidth() != imageBackground->getWidth())
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
    // Safety check: projectManager must be valid
    if (projectManager == nullptr)
        return;

    fftSize = projectManager->getFFTSize();

    const double sr = projectManager->getSampleRate();
    if (sr > 0.0 && sampleRate != sr)
    {
        sampleRate = sr;
        shouldDrawLines = true;
    }

    shouldUpdateSpectrum = true;

    repaint();
}

void SpectrogramComponent::paint (Graphics&g)
{
    g.setColour(Colours::black);
    g.fillAll();

    if (projectManager == nullptr)
        return;

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

    // Option A: Overlay gauge showing current peak frequency
    // Read peak/moving-average from active analyser and draw a marker + label
    {
        const int fftRef = (int)visualiserSource - 1;
        double peakFreq = 0.0, peakDb = 0.0, movingAvg = 0.0;
        projectManager->getMovingAveragePeakData(fftRef, peakFreq, peakDb, movingAvg);

        if (peakFreq > 0.0)
        {
            // Convert frequency to x position depending on current view (popup vs embedded)
            auto freqToX = [this](double f) -> float
            {
                if (isPopup)
                {
                    const double octaves = (log2(zoomRange_FreqHigh) - log2(zoomRange_FreqLow));
                    if (octaves <= 0.0) return 0.0f;
                    const double pos = (log2(f / zoomRange_FreqLow)) / octaves;
                    return (float) jlimit(0.0, 1.0, pos) * (float) getWidth();
                }
                else
                {
                    const double sr = projectManager->getSampleRate();
                    const double nyquist = jmax(1.0, sr / 2.0);
                    const double octaves = (log2(nyquist) - log2(kDefaultMinHertz));
                    if (octaves <= 0.0) return 0.0f;
                    const double pos = (log2(f / kDefaultMinHertz)) / octaves;
                    return (float) jlimit(0.0, 1.0, pos) * (float) getWidth();
                }
            };

            const float x = freqToX(peakFreq);

            // Draw marker
            g.setColour(Colours::orange.withAlpha(0.9f));
            g.drawLine(x, 0.0f, x, (float) getHeight(), 1.5f * scaleFactor);

            // Draw label at top-right
            const float labelFont = getLegendFontSize(scaleFactor);
            const float labelH = jmax(22.0f * scaleFactor, labelFont * 1.4f);
            const float labelW = jmax(160.0f * scaleFactor, labelFont * 8.0f);
            const float pad = 6.0f * scaleFactor;
            const Rectangle<float> box((float) getWidth() - labelW - pad, pad, labelW, labelH);
            g.setColour(Colours::black.withAlpha(0.6f));
            g.fillRoundedRectangle(box, 4.0f * scaleFactor);
            g.setColour(Colours::white);
            g.setFont(labelFont);

            String txt;
            txt << "Peak: " << String(peakFreq, 1, false) << " Hz";
            g.drawText(txt, box.toNearestInt(), Justification::centredRight, false);
        }
    }
    
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
        const float fontSize = getValueFontSize(scaleFactor);
        const float boxWidth = jmax(150.0f * scaleFactor, fontSize * 6.0f);
        const float boxHeight = jmax(50.0f * scaleFactor, fontSize * 2.4f);

        g.setColour(Colours::white);
        
        g.drawLine(0, displayDataPoint.y, getWidth(), displayDataPoint.y, 1.f * scaleFactor);
        g.drawLine(displayDataPoint.x, 0, displayDataPoint.x, getHeight(), 1.f * scaleFactor);
        
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
            g.drawText(freqString, drawPositionX + leftBorder, displayDataPoint.y + topBorder,
                       boxWidth - (leftBorder * 2), boxHeight * 0.5f - (topBorder * 2), Justification::left);
            
            g.drawText(dbString, drawPositionX + leftBorder, displayDataPoint.y + topBorder + fontSize,
                       boxWidth - (leftBorder * 2), boxHeight * 0.5f - (topBorder * 2), Justification::left);
        }
        else
        {
            drawPositionX-= boxWidth;
            
            g.drawText(freqString, drawPositionX + leftBorder, displayDataPoint.y + topBorder,
                       boxWidth - (leftBorder * 2), boxHeight * 0.5f - (topBorder * 2), Justification::right);
            
            g.drawText(dbString, drawPositionX + leftBorder, displayDataPoint.y + topBorder + fontSize,
                       boxWidth - (leftBorder * 2), boxHeight * 0.5f - (topBorder * 2), Justification::right);
        }
    }
    else
    {
        // simple Text for Db and Frequency
        // draw inside little box
        // above pointer, or below if pointer if at the top
        // to the right of the pointer unless it is to the right
        const float fontSize = getValueFontSize(scaleFactor);
        const float boxWidth = jmax(150.0f * scaleFactor, fontSize * 6.0f);
        const float boxHeight = jmax(50.0f * scaleFactor, fontSize * 2.4f);
        
        float xText = 0;
        
        g.setColour(Colours::white);
        
        g.drawLine(0, displayDataPoint.y, getWidth(), displayDataPoint.y, 1.f * scaleFactor);
        g.drawLine(displayDataPoint.x, 0, displayDataPoint.x, getHeight(), 1.f * scaleFactor);
        
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
            g.drawText(freqString, drawPositionX + leftBorder, displayDataPoint.y + topBorder,
                       boxWidth - (leftBorder * 2), boxHeight * 0.5f - (topBorder * 2), Justification::left);
            
            g.drawText(dbString, drawPositionX + leftBorder, displayDataPoint.y + topBorder + fontSize,
                       boxWidth - (leftBorder * 2), boxHeight * 0.5f - (topBorder * 2), Justification::left);
        }
        else
        {
            drawPositionX-= boxWidth;
            
            g.drawText(freqString, drawPositionX + leftBorder, displayDataPoint.y + topBorder,
                       boxWidth - (leftBorder * 2), boxHeight * 0.5f - (topBorder * 2), Justification::right);
            
            g.drawText(dbString, drawPositionX + leftBorder, displayDataPoint.y + topBorder + fontSize,
                       boxWidth - (leftBorder * 2), boxHeight * 0.5f - (topBorder * 2), Justification::right);
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

            const float fontSize = getAxisFontSize(scaleFactor);
            const float textWidth = jmax(120.0f * scaleFactor, fontSize * 4.5f);
            
            g.setFont(fontSize);
            g.drawText(dbString, getWidth() - textWidth, (getHeight() / numDivs) * i, textWidth, fontSize, Justification::right);
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

            const float fontSize = getAxisFontSize(scaleFactor);
            const float textWidth = jmax(120.0f * scaleFactor, fontSize * 4.5f);
            
            g.setFont(fontSize);
            g.drawText(dbString, getWidth() - textWidth, (getHeight() / numDivs) * i, textWidth, fontSize, Justification::right);
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

            const float fontSize = getAxisFontSize(scaleFactor);
            const float textWidth = jmax(120.0f * scaleFactor, fontSize * 4.5f);
            g.setFont(fontSize);
            g.drawText(freqString, ((getWidth() / numDivs) * i) - textWidth, getHeight() - fontSize, textWidth, fontSize, Justification::right);
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

            const float fontSize = getAxisFontSize(scaleFactor);
            const float textWidth = jmax(120.0f * scaleFactor, fontSize * 4.5f);
            g.setFont(fontSize);
            g.drawText(freqString, ((getWidth() / numDivs) * i) - textWidth, getHeight() - fontSize, textWidth, fontSize, Justification::right);
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
    // Safety check: projectManager must be valid
    if (projectManager != nullptr)
    {
        const double sr = projectManager->getSampleRate();
        if (sr > 0.0)
            sampleRate = sr;
    }

    // Use current sampleRate (either updated or default)
    const float octaves = static_cast<float>(log2(sampleRate / 2.0) - log2(kDefaultMinHertz));

    return static_cast<float>(kDefaultMinHertz * std::pow(2.0f, pos * octaves));
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

ColourSpectrumVisualiserComponent::ColourSpectrumVisualiserComponent(ProjectManager * pm, int initW, int initH)
    : projectManager(pm)
    , sampleRate(44100.0f)    // Safe default
    , fftSize(2048)           // Safe default
    , visualiserSource(VISUALISER_SOURCE::OUTPUT_1)
{
    // Get actual values if projectManager is valid
    if (projectManager != nullptr)
    {
        const float sr = projectManager->getSampleRate();
        if (sr > 0.0f)
            sampleRate = sr;
        fftSize = projectManager->getFFTSize();
    }

    newSpectrogramImage = std::make_unique<Image>(Image::RGB, jmax(1, initW), jmax(1, initH), true);
}

ColourSpectrumVisualiserComponent::~ColourSpectrumVisualiserComponent()
{
    // CRITICAL: Stop timer before destruction to prevent race condition
    stopTimer();
}

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
    
    if (newSpectrogramImage != nullptr)
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
    const float fontSize = getValueFontSize(scaleFactor);
    const float boxWidth = jmax(150.0f * scaleFactor, fontSize * 6.0f);
    const float boxHeight = jmax(50.0f * scaleFactor, fontSize * 2.4f);
    
    g.setColour(Colours::white);
    
    g.drawLine(0, displayDataPoint.y, getWidth(), displayDataPoint.y, 1.f * scaleFactor);
    g.drawLine(displayDataPoint.x, 0, displayDataPoint.x, getHeight(), 1.f * scaleFactor);
    
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
    g.drawText(freqString, displayDataPoint.x - boxWidth + leftBorder, displayDataPoint.y + topBorder,
               boxWidth - (leftBorder * 2), boxHeight * 0.5f - (topBorder * 2), Justification::right);

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

        const float fontSize = getAxisFontSize(scaleFactor);
        const float textWidth = jmax(120.0f * scaleFactor, fontSize * 4.5f);
        
        g.setFont(fontSize);
        g.drawText(freqString, 0, (getHeight() / numDivs) * i, textWidth, fontSize, Justification::left);
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

        const float fontSize = getAxisFontSize(scaleFactor);
        const float textWidth = jmax(120.0f * scaleFactor, fontSize * 4.5f);

        g.setFont(fontSize);
        g.drawText(freqString, getWidth() - (w + textWidth), getHeight() - fontSize, textWidth, fontSize, Justification::right);
        
    }

}

void ColourSpectrumVisualiserComponent::resized()
{
    if (newSpectrogramImage != nullptr)
    {
        Image rescaledImage(newSpectrogramImage->createCopy());
        newSpectrogramImage = std::make_unique<Image>(rescaledImage.rescaled(jmax(1, getWidth()), jmax(1, getHeight())));
    }
    else
    {
        newSpectrogramImage = std::make_unique<Image>(Image::RGB, jmax(1, getWidth()), jmax(1, getHeight()), true);
    }
}

void ColourSpectrumVisualiserComponent::pushUpdate()
{
    // Safety check: projectManager must be valid
    if (projectManager == nullptr)
        return;

    fftSize = projectManager->getFFTSize();
    const float sr = projectManager->getSampleRate();
    if (sr > 0.0f)
        sampleRate = sr;

    drawNextLineOfSpectrogram();

    repaint();
}

void ColourSpectrumVisualiserComponent::drawNextLineOfSpectrogram()
{
    // Safety checks
    if (projectManager == nullptr || newSpectrogramImage == nullptr)
        return;

    const int fftRef = static_cast<int>(visualiserSource) - 1;
    if (fftRef < 0 || fftRef >= 8)
        return;

    projectManager->createColourSpectrum(fftRef, *newSpectrogramImage, zoomRange_FreqLow, zoomRange_FreqHigh, 0.2);
}
