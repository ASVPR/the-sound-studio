/*
  ==============================================================================

    SpectraHarmonicsChart.cpp

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#include "SpectraHarmonicsChart.h"
#include <cmath>

SpectraHarmonicsChart::SpectraHarmonicsChart(ProjectManager* pm)
    : projectManager(pm)
{
    // Initialize UI components
    modeSelector = std::make_unique<ComboBox>("Mode Selector");
    modeSelector->addItem("Bars", 1);
    modeSelector->addItem("Lines", 2);
    modeSelector->addItem("Stem Plot", 3);
    modeSelector->addItem("Waterfall", 4);
    modeSelector->setSelectedId(1);
    modeSelector->onChange = [this]() {
        currentMode = static_cast<DisplayMode>(modeSelector->getSelectedId() - 1);
        repaint();
    };
    addAndMakeVisible(modeSelector.get());
    
    harmonicsSlider = std::make_unique<Slider>("Harmonics");
    harmonicsSlider->setRange(1, 32, 1);
    harmonicsSlider->setValue(maxHarmonics);
    harmonicsSlider->setSliderStyle(Slider::LinearHorizontal);
    harmonicsSlider->setTextBoxStyle(Slider::TextBoxLeft, false, 50, 20);
    harmonicsSlider->onValueChange = [this]() {
        maxHarmonics = static_cast<int>(harmonicsSlider->getValue());
        repaint();
    };
    addAndMakeVisible(harmonicsSlider.get());
    
    exportButton = std::make_unique<TextButton>("Export");
    exportButton->onClick = [this]() { exportSnapshot(); };
    addAndMakeVisible(exportButton.get());
    
    peakLabelsToggle = std::make_unique<ToggleButton>("Show Peak Labels");
    peakLabelsToggle->setToggleState(showPeakLabels, dontSendNotification);
    peakLabelsToggle->onClick = [this]() {
        showPeakLabels = peakLabelsToggle->getToggleState();
        repaint();
    };
    addAndMakeVisible(peakLabelsToggle.get());
    
    gridToggle = std::make_unique<ToggleButton>("Show Grid");
    gridToggle->setToggleState(showGridLines, dontSendNotification);
    gridToggle->onClick = [this]() {
        showGridLines = gridToggle->getToggleState();
        repaint();
    };
    addAndMakeVisible(gridToggle.get());
    
    // Initialize arrays
    harmonicFrequencies.resize(maxHarmonics);
    harmonicAmplitudes.resize(maxHarmonics);
    
    // Start timer for animation
    startTimer(50); // 20 FPS
}

SpectraHarmonicsChart::~SpectraHarmonicsChart()
{
    stopTimer();
}

void SpectraHarmonicsChart::paint(Graphics& g)
{
    drawBackground(g);
    
    if (showGridLines)
        drawGridLines(g);
    
    drawAxis(g);
    
    switch (currentMode)
    {
        case BARS:
            drawBars(g);
            break;
        case LINES:
            drawLines(g);
            break;
        case STEM_PLOT:
            drawStemPlot(g);
            break;
        case WATERFALL:
            // Waterfall view would require historical data
            drawBars(g); // Fallback to bars for now
            break;
    }
    
    if (showPeakLabels)
        drawPeakLabels(g);
    
    drawLegend(g);
}

void SpectraHarmonicsChart::resized()
{
    auto bounds = getLocalBounds();
    
    // Position controls at the top
    auto controlArea = bounds.removeFromTop(static_cast<int>(40 * scaleFactor));
    controlArea.reduce(static_cast<int>(10 * scaleFactor), static_cast<int>(5 * scaleFactor));
    
    int controlWidth = static_cast<int>(120 * scaleFactor);
    int spacing = static_cast<int>(10 * scaleFactor);
    
    modeSelector->setBounds(controlArea.removeFromLeft(controlWidth));
    controlArea.removeFromLeft(spacing);
    
    harmonicsSlider->setBounds(controlArea.removeFromLeft(static_cast<int>(200 * scaleFactor)));
    controlArea.removeFromLeft(spacing);
    
    peakLabelsToggle->setBounds(controlArea.removeFromLeft(controlWidth));
    controlArea.removeFromLeft(spacing);
    
    gridToggle->setBounds(controlArea.removeFromLeft(controlWidth));
    controlArea.removeFromLeft(spacing);
    
    exportButton->setBounds(controlArea.removeFromLeft(static_cast<int>(80 * scaleFactor)));
}

void SpectraHarmonicsChart::timerCallback()
{
    if (projectManager)
    {
        int fftChannel = 0;

        double peakFreq = 0.0;
        double peakDB = 0.0;
        double movingAvgFreq = 0.0;
        projectManager->getMovingAveragePeakData(fftChannel, peakFreq, peakDB, movingAvgFreq);

        if (movingAvgFreq > 20.0 && movingAvgFreq < 20000.0)
        {
            fundamentalFreq = static_cast<float>(movingAvgFreq);
        }

        sampleRate = static_cast<float>(projectManager->getSampleRate());
        int currentFFTSize = projectManager->getFFTSize();

        const float* fftData = projectManager->analyzerPool.getAnalyzer(fftChannel).getFFTData();
        int fftDataSize = projectManager->analyzerPool.getAnalyzer(fftChannel).getFFTDataSize();

        if (fftData && fftDataSize > 0 && fundamentalFreq > 0)
        {
            detectHarmonics(fftData, fftDataSize);
        }

        repaint();
    }
}

void SpectraHarmonicsChart::setFFTData(const float* fftData, int fftSize, float sr)
{
    sampleRate = sr;
    detectHarmonics(fftData, fftSize);
    repaint();
}

void SpectraHarmonicsChart::detectHarmonics(const float* fftData, int fftSize)
{
    if (fundamentalFreq <= 0 || sampleRate <= 0)
        return;
    
    // Calculate bin resolution
    float binResolution = sampleRate / static_cast<float>(fftSize);
    
    // Detect harmonics
    for (int h = 0; h < maxHarmonics; ++h)
    {
        float harmonicFreq = fundamentalFreq * static_cast<float>(h + 1);
        harmonicFrequencies.set(h, harmonicFreq);
        
        // Get amplitude at this frequency
        float amplitude = getAmplitudeAtFrequency(fftData, fftSize, harmonicFreq);
        
        // Convert to dB
        float db = 20.0f * std::log10(std::max(amplitude, 1e-10f));
        harmonicAmplitudes.set(h, db);
    }
}

float SpectraHarmonicsChart::getAmplitudeAtFrequency(const float* fftData, int fftSize, float frequency)
{
    if (!fftData || fftSize <= 0 || frequency <= 0)
        return 0.0f;
    
    float binResolution = sampleRate / static_cast<float>(fftSize);
    float binIndex = frequency / binResolution;
    
    // Use interpolation for more accurate amplitude
    return interpolateAmplitude(fftData, fftSize, binIndex);
}

float SpectraHarmonicsChart::interpolateAmplitude(const float* fftData, int fftSize, float binIndex)
{
    int lowerBin = static_cast<int>(std::floor(binIndex));
    int upperBin = static_cast<int>(std::ceil(binIndex));
    
    // Bounds checking
    if (lowerBin < 0) return 0.0f;
    if (upperBin >= fftSize / 2) return 0.0f;
    
    if (lowerBin == upperBin)
        return fftData[lowerBin];
    
    // Linear interpolation
    float fraction = binIndex - static_cast<float>(lowerBin);
    return fftData[lowerBin] * (1.0f - fraction) + fftData[upperBin] * fraction;
}

void SpectraHarmonicsChart::drawBackground(Graphics& g)
{
    g.fillAll(Colours::black);
    
    // Draw gradient background
    auto chartArea = getChartArea();
    ColourGradient gradient(Colours::darkgrey.darker(), chartArea.getTopLeft(),
                           Colours::black, chartArea.getBottomRight(), false);
    g.setGradientFill(gradient);
    g.fillRect(chartArea);
}

void SpectraHarmonicsChart::drawGridLines(Graphics& g)
{
    auto chartArea = getChartArea();
    g.setColour(gridColor);
    
    // Vertical grid lines (frequency)
    for (int h = 0; h < maxHarmonics; ++h)
    {
        float freq = fundamentalFreq * static_cast<float>(h + 1);
        float x = frequencyToX(freq, chartArea);
        g.drawLine(x, chartArea.getY(), x, chartArea.getBottom(), 0.5f);
    }
    
    // Horizontal grid lines (amplitude)
    int numLines = 10;
    float dbStep = (maxDb - minDb) / static_cast<float>(numLines);
    for (int i = 0; i <= numLines; ++i)
    {
        float db = minDb + (dbStep * static_cast<float>(i));
        float y = amplitudeToY(db, chartArea);
        g.drawLine(chartArea.getX(), y, chartArea.getRight(), y, 0.5f);
    }
}

void SpectraHarmonicsChart::drawBars(Graphics& g)
{
    auto chartArea = getChartArea();
    
    for (int h = 0; h < harmonicAmplitudes.size(); ++h)
    {
        float freq = harmonicFrequencies[h];
        float amp = harmonicAmplitudes[h];
        
        float x = frequencyToX(freq, chartArea);
        float y = amplitudeToY(amp, chartArea);
        float barHeight = chartArea.getBottom() - y;
        
        // Draw bar with gradient
        Rectangle<float> barRect(x - scaledBarWidth / 2, y, scaledBarWidth, barHeight);
        
        ColourGradient barGradient(barColor, barRect.getTopLeft(),
                                  barColor.darker(0.5f), barRect.getBottomLeft(), false);
        g.setGradientFill(barGradient);
        g.fillRect(barRect);
        
        // Draw bar outline
        g.setColour(barColor.brighter());
        g.drawRect(barRect, 1.0f);
    }
}

void SpectraHarmonicsChart::drawLines(Graphics& g)
{
    auto chartArea = getChartArea();
    
    Path linePath;
    bool firstPoint = true;
    
    for (int h = 0; h < harmonicAmplitudes.size(); ++h)
    {
        float freq = harmonicFrequencies[h];
        float amp = harmonicAmplitudes[h];
        
        float x = frequencyToX(freq, chartArea);
        float y = amplitudeToY(amp, chartArea);
        
        if (firstPoint)
        {
            linePath.startNewSubPath(x, y);
            firstPoint = false;
        }
        else
        {
            linePath.lineTo(x, y);
        }
        
        // Draw point marker
        g.setColour(lineColor);
        g.fillEllipse(x - 3, y - 3, 6, 6);
    }
    
    g.setColour(lineColor);
    g.strokePath(linePath, PathStrokeType(2.0f));
}

void SpectraHarmonicsChart::drawStemPlot(Graphics& g)
{
    auto chartArea = getChartArea();
    float baseline = chartArea.getBottom();
    
    for (int h = 0; h < harmonicAmplitudes.size(); ++h)
    {
        float freq = harmonicFrequencies[h];
        float amp = harmonicAmplitudes[h];
        
        float x = frequencyToX(freq, chartArea);
        float y = amplitudeToY(amp, chartArea);
        
        // Draw stem line
        g.setColour(lineColor.withAlpha(0.7f));
        g.drawLine(x, baseline, x, y, 2.0f);
        
        // Draw circle at top
        g.setColour(lineColor);
        g.fillEllipse(x - 4, y - 4, 8, 8);
    }
}

void SpectraHarmonicsChart::drawAxis(Graphics& g)
{
    auto chartArea = getChartArea();
    
    g.setColour(textColor);
    g.setFont(Font(scaledFontSize));
    
    // X-axis
    g.drawLine(chartArea.getX(), chartArea.getBottom(),
               chartArea.getRight(), chartArea.getBottom(), 2.0f);
    
    // Y-axis
    g.drawLine(chartArea.getX(), chartArea.getY(),
               chartArea.getX(), chartArea.getBottom(), 2.0f);
    
    // X-axis labels (frequencies)
    for (int h = 0; h < maxHarmonics; h += 2)
    {
        float freq = fundamentalFreq * static_cast<float>(h + 1);
        float x = frequencyToX(freq, chartArea);
        
        String label = String(static_cast<int>(freq)) + " Hz";
        g.drawText(label, static_cast<int>(x - 30), static_cast<int>(chartArea.getBottom() + 5),
                   60, scaledAxisLabelHeight, Justification::centred);
    }
    
    // Y-axis labels (dB)
    int numLabels = 5;
    float dbStep = (maxDb - minDb) / static_cast<float>(numLabels);
    for (int i = 0; i <= numLabels; ++i)
    {
        float db = minDb + (dbStep * static_cast<float>(i));
        float y = amplitudeToY(db, chartArea);
        
        String label = String(static_cast<int>(db)) + " dB";
        g.drawText(label, static_cast<int>(chartArea.getX() - 50), static_cast<int>(y - 10),
                   45, 20, Justification::right);
    }
    
    // Axis titles
    g.setFont(Font(scaledFontSize * 1.2f));
    g.drawText("Harmonic Frequencies (Hz)", chartArea.getX(), 
               static_cast<int>(chartArea.getBottom() + 30),
               static_cast<int>(chartArea.getWidth()), 20, Justification::centred);
    
    // Y-axis title (rotated)
    g.saveState();
    g.addTransform(AffineTransform::rotation(-juce::MathConstants<float>::halfPi,
                                            chartArea.getX() - 70,
                                            chartArea.getCentreY()));
    g.drawText("Amplitude (dB)", static_cast<int>(chartArea.getX() - 70 - chartArea.getHeight() / 2),
               static_cast<int>(chartArea.getCentreY() - 10),
               static_cast<int>(chartArea.getHeight()), 20, Justification::centred);
    g.restoreState();
}

void SpectraHarmonicsChart::drawLegend(Graphics& g)
{
    auto bounds = getLocalBounds();
    auto legendArea = bounds.removeFromBottom(static_cast<int>(30 * scaleFactor));
    legendArea.reduce(static_cast<int>(10 * scaleFactor), 0);
    
    g.setColour(textColor);
    g.setFont(Font(scaledFontSize * 0.9f));
    
    String info = "Fundamental: " + String(fundamentalFreq, 1) + " Hz | ";
    info += "Harmonics: " + String(maxHarmonics) + " | ";
    info += "Sample Rate: " + String(static_cast<int>(sampleRate)) + " Hz";
    
    g.drawText(info, legendArea, Justification::centred);
}

void SpectraHarmonicsChart::drawPeakLabels(Graphics& g)
{
    auto chartArea = getChartArea();
    
    g.setFont(Font(scaledFontSize * 0.8f));
    
    for (int h = 0; h < harmonicAmplitudes.size(); ++h)
    {
        float freq = harmonicFrequencies[h];
        float amp = harmonicAmplitudes[h];
        
        // Only label significant peaks
        if (amp > minDb + 10.0f)
        {
            float x = frequencyToX(freq, chartArea);
            float y = amplitudeToY(amp, chartArea);
            
            String label = "H" + String(h + 1) + "\n" + String(amp, 1) + "dB";
            
            // Draw background for better readability
            Rectangle<float> labelRect(x - 20, y - 25, 40, 20);
            g.setColour(Colours::black.withAlpha(0.7f));
            g.fillRoundedRectangle(labelRect, 2.0f);
            
            g.setColour(textColor);
            g.drawText(label, labelRect.toNearestInt(), Justification::centred);
        }
    }
}

Rectangle<float> SpectraHarmonicsChart::getChartArea() const
{
    auto bounds = getLocalBounds().toFloat();
    
    // Reserve space for controls and margins
    bounds.removeFromTop(50.0f * scaleFactor);
    bounds.removeFromBottom(40.0f * scaleFactor);
    bounds.removeFromLeft(60.0f * scaleFactor);
    bounds.removeFromRight(20.0f * scaleFactor);
    
    return bounds;
}

float SpectraHarmonicsChart::frequencyToX(float freq, const Rectangle<float>& area) const
{
    if (maxHarmonics <= 0) return area.getX();
    
    float maxFreq = fundamentalFreq * static_cast<float>(maxHarmonics);
    float normalizedFreq = freq / maxFreq;
    
    return area.getX() + (normalizedFreq * area.getWidth());
}

float SpectraHarmonicsChart::amplitudeToY(float amp, const Rectangle<float>& area) const
{
    float normalizedAmp = (amp - minDb) / (maxDb - minDb);
    normalizedAmp = jlimit(0.0f, 1.0f, normalizedAmp);
    
    // Invert Y axis (higher values at top)
    return area.getBottom() - (normalizedAmp * area.getHeight());
}

void SpectraHarmonicsChart::exportSnapshot()
{
    // Create an image of the current chart
    Image snapshot(Image::ARGB, getWidth(), getHeight(), true);
    Graphics g(snapshot);
    paint(g);
    
    // Save to file
    FileChooser chooser("Save Harmonics Chart",
                       File::getSpecialLocation(File::userDocumentsDirectory)
                           .getChildFile("TSS")
                           .getChildFile("Exports"),
                       "*.png");
    
    if (chooser.browseForFileToSave(true))
    {
        File file = chooser.getResult();
        if (!file.hasFileExtension(".png"))
            file = file.withFileExtension(".png");
        
        PNGImageFormat pngFormat;
        FileOutputStream stream(file);
        if (stream.openedOk())
        {
            pngFormat.writeImageToStream(snapshot, stream);
        }
    }
}

void SpectraHarmonicsChart::updateScaledValues()
{
    scaledMargin = static_cast<int>(10 * scaleFactor);
    scaledAxisLabelHeight = static_cast<int>(20 * scaleFactor);
    scaledFontSize = 12.0f * scaleFactor;
    scaledBarWidth = 20.0f * scaleFactor;
    
    // Update component sizes - font sizes will be handled by LookAndFeel
}