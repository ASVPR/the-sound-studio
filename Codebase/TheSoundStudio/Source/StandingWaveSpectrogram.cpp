/*
  ==============================================================================

    StandingWaveSpectrogram.cpp
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#include "StandingWaveSpectrogram.h"
#include <cmath>

StandingWaveSpectrogram::StandingWaveSpectrogram(ProjectManager* pm)
    : projectManager(pm)
{
    modeSelector = std::make_unique<juce::ComboBox>("Mode");
    modeSelector->addItem("1D Wave (String)", 1);
    modeSelector->addItem("2D Rectangular", 2);
    modeSelector->addItem("2D Circular (Drum)", 3);
    modeSelector->addItem("3D Cavity", 4);
    modeSelector->setSelectedId(1);
    modeSelector->onChange = [this]() {
        currentMode = static_cast<WaveMode>(modeSelector->getSelectedId() - 1);
        waveHistory.clear();
        repaint();
    };
    addAndMakeVisible(modeSelector.get());

    frequencySlider = std::make_unique<juce::Slider>("Frequency");
    frequencySlider->setRange(20.0, 2000.0, 1.0);
    frequencySlider->setValue(frequency);
    frequencySlider->setSliderStyle(juce::Slider::LinearHorizontal);
    frequencySlider->setTextBoxStyle(juce::Slider::TextBoxLeft, false, 60, 20);
    frequencySlider->onValueChange = [this]() {
        frequency = static_cast<float>(frequencySlider->getValue());
        numNodes = calculateNumNodes();
    };
    addAndMakeVisible(frequencySlider.get());

    dampingSlider = std::make_unique<juce::Slider>("Damping");
    dampingSlider->setRange(0.0, 1.0, 0.01);
    dampingSlider->setValue(damping);
    dampingSlider->setSliderStyle(juce::Slider::LinearHorizontal);
    dampingSlider->setTextBoxStyle(juce::Slider::TextBoxLeft, false, 50, 20);
    dampingSlider->onValueChange = [this]() {
        damping = static_cast<float>(dampingSlider->getValue());
    };
    addAndMakeVisible(dampingSlider.get());

    nodeSlider = std::make_unique<juce::Slider>("Nodes");
    nodeSlider->setRange(1, 20, 1);
    nodeSlider->setValue(numNodes);
    nodeSlider->setSliderStyle(juce::Slider::LinearHorizontal);
    nodeSlider->setTextBoxStyle(juce::Slider::TextBoxLeft, false, 40, 20);
    nodeSlider->onValueChange = [this]() {
        numNodes = static_cast<int>(nodeSlider->getValue());
        frequency = (numNodes * waveSpeed) / 2.0f;
        frequencySlider->setValue(frequency, juce::dontSendNotification);
    };
    addAndMakeVisible(nodeSlider.get());

    animationToggle = std::make_unique<juce::ToggleButton>("Animate");
    animationToggle->setToggleState(animationEnabled, juce::dontSendNotification);
    animationToggle->onClick = [this]() {
        animationEnabled = animationToggle->getToggleState();
    };
    addAndMakeVisible(animationToggle.get());

    clearButton = std::make_unique<juce::TextButton>("Clear History");
    clearButton->onClick = [this]() {
        waveHistory.clear();
        repaint();
    };
    addAndMakeVisible(clearButton.get());

    startTimer(33); // ~30 FPS
}

StandingWaveSpectrogram::~StandingWaveSpectrogram()
{
    stopTimer();
}

void StandingWaveSpectrogram::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);

    drawInfo(g);

    switch (currentMode)
    {
        case MODE_1D:
            draw1DStandingWave(g);
            drawWaveHistory(g);
            break;
        case MODE_2D_RECT:
            draw2DRectWave(g);
            break;
        case MODE_2D_CIRC:
            draw2DCircWave(g);
            break;
        case MODE_3D:
            draw1DStandingWave(g); // Fallback to 1D for now
            drawWaveHistory(g);
            break;
    }

    drawNodes(g);
}

void StandingWaveSpectrogram::resized()
{
    auto bounds = getLocalBounds();

    auto controlArea = bounds.removeFromTop(static_cast<int>(40 * scaleFactor));
    controlArea.reduce(static_cast<int>(10 * scaleFactor), static_cast<int>(5 * scaleFactor));

    int controlWidth = static_cast<int>(120 * scaleFactor);
    int sliderWidth = static_cast<int>(150 * scaleFactor);
    int spacing = static_cast<int>(10 * scaleFactor);

    modeSelector->setBounds(controlArea.removeFromLeft(controlWidth));
    controlArea.removeFromLeft(spacing);

    frequencySlider->setBounds(controlArea.removeFromLeft(sliderWidth));
    controlArea.removeFromLeft(spacing);

    nodeSlider->setBounds(controlArea.removeFromLeft(sliderWidth));
    controlArea.removeFromLeft(spacing);

    dampingSlider->setBounds(controlArea.removeFromLeft(sliderWidth));
    controlArea.removeFromLeft(spacing);

    animationToggle->setBounds(controlArea.removeFromLeft(static_cast<int>(80 * scaleFactor)));
    controlArea.removeFromLeft(spacing);

    clearButton->setBounds(controlArea.removeFromLeft(static_cast<int>(100 * scaleFactor)));
}

void StandingWaveSpectrogram::timerCallback()
{
    if (animationEnabled)
    {
        phase += 0.05f * animationSpeed;
        if (phase > juce::MathConstants<float>::twoPi)
            phase -= juce::MathConstants<float>::twoPi;
    }

    if (projectManager)
    {
        int fftChannel = 0;
        double peakFreq = 0.0;
        double peakDB = 0.0;
        double movingAvgFreq = 0.0;
        projectManager->getMovingAveragePeakData(fftChannel, peakFreq, peakDB, movingAvgFreq);

        if (movingAvgFreq > 20.0 && movingAvgFreq < 2000.0)
        {
            frequency = static_cast<float>(movingAvgFreq);
            frequencySlider->setValue(frequency, juce::dontSendNotification);
            numNodes = calculateNumNodes();
            nodeSlider->setValue(numNodes, juce::dontSendNotification);
        }
    }

    updateWaveData();
    repaint();
}

void StandingWaveSpectrogram::calculate1DWave(std::vector<float>& wave, float time)
{
    int size = wave.size();
    float wavelength = calculateWavelength();
    float k = juce::MathConstants<float>::twoPi / wavelength;
    float omega = juce::MathConstants<float>::twoPi * frequency;

    for (int i = 0; i < size; ++i)
    {
        float x = static_cast<float>(i) / size;
        float spatialPart = std::sin(k * x * numNodes);
        float temporalPart = std::cos(omega * time + phase);
        wave[i] = amplitude * spatialPart * temporalPart * std::exp(-damping * time);
    }
}

void StandingWaveSpectrogram::calculate2DRectWave(std::vector<std::vector<float>>& wave, float time)
{
    int width = wave.size();
    if (width == 0) return;
    int height = wave[0].size();

    float omega = juce::MathConstants<float>::twoPi * frequency;
    int m = numNodes;
    int n = numNodes;

    for (int i = 0; i < width; ++i)
    {
        for (int j = 0; j < height; ++j)
        {
            float x = static_cast<float>(i) / width;
            float y = static_cast<float>(j) / height;

            float spatialPart = std::sin(m * juce::MathConstants<float>::pi * x) *
                              std::sin(n * juce::MathConstants<float>::pi * y);
            float temporalPart = std::cos(omega * time + phase);

            wave[i][j] = amplitude * spatialPart * temporalPart * std::exp(-damping * time);
        }
    }
}

void StandingWaveSpectrogram::calculate2DCircWave(std::vector<std::vector<float>>& wave, float time)
{
    int width = wave.size();
    if (width == 0) return;
    int height = wave[0].size();

    float omega = juce::MathConstants<float>::twoPi * frequency;
    float centerX = width / 2.0f;
    float centerY = height / 2.0f;
    float maxRadius = std::min(width, height) / 2.0f;

    for (int i = 0; i < width; ++i)
    {
        for (int j = 0; j < height; ++j)
        {
            float dx = i - centerX;
            float dy = j - centerY;
            float r = std::sqrt(dx * dx + dy * dy) / maxRadius;
            float theta = std::atan2(dy, dx);

            if (r <= 1.0f)
            {
                int m = numNodes;
                float spatialPart = std::sin(m * juce::MathConstants<float>::pi * r) * std::cos(m * theta);
                float temporalPart = std::cos(omega * time + phase);
                wave[i][j] = amplitude * spatialPart * temporalPart * std::exp(-damping * time);
            }
            else
            {
                wave[i][j] = 0.0f;
            }
        }
    }
}

void StandingWaveSpectrogram::updateWaveData()
{
    std::vector<float> currentWave(200);
    calculate1DWave(currentWave, phase);

    waveHistory.push_back(currentWave);
    if (waveHistory.size() > historySize)
        waveHistory.pop_front();
}

void StandingWaveSpectrogram::draw1DStandingWave(juce::Graphics& g)
{
    auto waveArea = getWaveArea();
    float topHalfHeight = waveArea.getHeight() * 0.4f;
    auto topWaveArea = waveArea.removeFromTop(topHalfHeight);

    std::vector<float> wave(200);
    calculate1DWave(wave, phase);

    juce::Path wavePath;
    float xStep = topWaveArea.getWidth() / wave.size();
    float yCenter = topWaveArea.getCentreY();
    float yScale = topWaveArea.getHeight() * 0.4f;

    wavePath.startNewSubPath(topWaveArea.getX(), yCenter);
    for (size_t i = 0; i < wave.size(); ++i)
    {
        float x = topWaveArea.getX() + i * xStep;
        float y = yCenter - wave[i] * yScale;
        wavePath.lineTo(x, y);
    }

    g.setColour(waveColor);
    g.strokePath(wavePath, juce::PathStrokeType(2.0f * scaleFactor));

    g.setColour(gridColor);
    g.drawLine(topWaveArea.getX(), yCenter, topWaveArea.getRight(), yCenter, 1.0f);
}

void StandingWaveSpectrogram::draw2DRectWave(juce::Graphics& g)
{
    auto waveArea = getWaveArea();

    int width = 100;
    int height = 100;
    std::vector<std::vector<float>> wave(width, std::vector<float>(height));

    calculate2DRectWave(wave, phase);

    float cellWidth = waveArea.getWidth() / width;
    float cellHeight = waveArea.getHeight() / height;

    for (int i = 0; i < width; ++i)
    {
        for (int j = 0; j < height; ++j)
        {
            float value = wave[i][j];
            float intensity = (value + 1.0f) / 2.0f;
            juce::Colour color = juce::Colour::fromHSV(0.6f, intensity, intensity, 1.0f);

            g.setColour(color);
            g.fillRect(waveArea.getX() + i * cellWidth,
                      waveArea.getY() + j * cellHeight,
                      cellWidth, cellHeight);
        }
    }
}

void StandingWaveSpectrogram::draw2DCircWave(juce::Graphics& g)
{
    auto waveArea = getWaveArea();

    int width = 100;
    int height = 100;
    std::vector<std::vector<float>> wave(width, std::vector<float>(height));

    calculate2DCircWave(wave, phase);

    float cellWidth = waveArea.getWidth() / width;
    float cellHeight = waveArea.getHeight() / height;

    for (int i = 0; i < width; ++i)
    {
        for (int j = 0; j < height; ++j)
        {
            float value = wave[i][j];
            float intensity = std::abs(value);
            juce::Colour color;

            if (value > 0)
                color = juce::Colour::fromHSV(0.1f, intensity, intensity, 1.0f); // Orange/yellow
            else
                color = juce::Colour::fromHSV(0.6f, intensity, intensity, 1.0f); // Blue/cyan

            g.setColour(color);
            g.fillRect(waveArea.getX() + i * cellWidth,
                      waveArea.getY() + j * cellHeight,
                      cellWidth, cellHeight);
        }
    }
}

void StandingWaveSpectrogram::drawWaveHistory(juce::Graphics& g)
{
    auto waveArea = getWaveArea();
    waveArea.removeFromTop(waveArea.getHeight() * 0.4f);

    if (waveHistory.empty())
        return;

    float xStep = waveArea.getWidth() / waveHistory[0].size();
    float yStep = waveArea.getHeight() / historySize;

    for (size_t t = 0; t < waveHistory.size(); ++t)
    {
        const auto& wave = waveHistory[t];
        float y = waveArea.getY() + t * yStep;

        for (size_t i = 0; i < wave.size(); ++i)
        {
            float x = waveArea.getX() + i * xStep;
            float intensity = (wave[i] + 1.0f) / 2.0f;
            juce::Colour color = juce::Colour::fromHSV(0.6f - intensity * 0.3f, 1.0f, intensity, 1.0f);

            g.setColour(color);
            g.fillRect(x, y, xStep + 1, yStep + 1);
        }
    }
}

void StandingWaveSpectrogram::drawNodes(juce::Graphics& g)
{
    if (currentMode != MODE_1D)
        return;

    auto waveArea = getWaveArea();
    float topHalfHeight = waveArea.getHeight() * 0.4f;
    auto topWaveArea = waveArea.removeFromTop(topHalfHeight);

    float yCenter = topWaveArea.getCentreY();

    for (int i = 0; i <= numNodes; ++i)
    {
        float x = topWaveArea.getX() + (topWaveArea.getWidth() * i) / numNodes;

        g.setColour(nodeColor);
        g.fillEllipse(x - scaledNodeRadius, yCenter - scaledNodeRadius,
                     scaledNodeRadius * 2, scaledNodeRadius * 2);

        g.setColour(textColor);
        g.setFont(juce::Font(scaledFontSize * 0.8f));
        g.drawText(juce::String(i), static_cast<int>(x - 10), static_cast<int>(yCenter + scaledNodeRadius * 3),
                  20, 15, juce::Justification::centred);
    }
}

void StandingWaveSpectrogram::drawInfo(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    auto infoArea = bounds.removeFromBottom(static_cast<int>(30 * scaleFactor));
    infoArea.reduce(static_cast<int>(10 * scaleFactor), 0);

    g.setColour(textColor);
    g.setFont(juce::Font(scaledFontSize));

    juce::String info = "Frequency: " + juce::String(frequency, 1) + " Hz | ";
    info += "Wavelength: " + juce::String(calculateWavelength(), 2) + " m | ";
    info += "Nodes: " + juce::String(numNodes) + " | ";
    info += "Damping: " + juce::String(damping, 2);

    g.drawText(info, infoArea, juce::Justification::centred);
}

juce::Rectangle<float> StandingWaveSpectrogram::getWaveArea() const
{
    auto bounds = getLocalBounds().toFloat();
    bounds.removeFromTop(50 * scaleFactor);
    bounds.removeFromBottom(40 * scaleFactor);
    bounds.reduce(20 * scaleFactor, 10 * scaleFactor);
    return bounds;
}

int StandingWaveSpectrogram::calculateNumNodes() const
{
    return static_cast<int>((2.0f * frequency) / waveSpeed) + 1;
}

void StandingWaveSpectrogram::updateScaledValues()
{
    scaledMargin = static_cast<int>(10 * scaleFactor);
    scaledFontSize = 12.0f * scaleFactor;
    scaledNodeRadius = 4.0f * scaleFactor;
}