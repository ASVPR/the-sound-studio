/*
  ==============================================================================

    HarmonicsSurface3D.cpp

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#include "HarmonicsSurface3D.h"

HarmonicsSurface3D::HarmonicsSurface3D(ProjectManager* pm)
    : OpenGLVisualizerBase(pm)
{
    camera.setPosition(0, 3, 8);
    camera.setTarget(0, 0, 0);

    modeSelector = std::make_unique<juce::ComboBox>("Display Mode");
    modeSelector->addItem("Surface", 1);
    modeSelector->addItem("Wireframe", 2);
    modeSelector->addItem("Points", 3);
    modeSelector->addItem("Filled + Wireframe", 4);
    modeSelector->setSelectedId(1);
    modeSelector->onChange = [this]() {
        displayMode = static_cast<DisplayMode>(modeSelector->getSelectedId() - 1);
    };
    addAndMakeVisible(modeSelector.get());

    harmonicsSlider = std::make_unique<juce::Slider>("Harmonics");
    harmonicsSlider->setRange(1, 32, 1);
    harmonicsSlider->setValue(maxHarmonics);
    harmonicsSlider->setSliderStyle(juce::Slider::LinearHorizontal);
    harmonicsSlider->setTextBoxStyle(juce::Slider::TextBoxLeft, false, 50, 20);
    harmonicsSlider->onValueChange = [this]() {
        maxHarmonics = static_cast<int>(harmonicsSlider->getValue());
    };
    addAndMakeVisible(harmonicsSlider.get());

    historySlider = std::make_unique<juce::Slider>("History");
    historySlider->setRange(10, 500, 10);
    historySlider->setValue(historySize);
    historySlider->setSliderStyle(juce::Slider::LinearHorizontal);
    historySlider->setTextBoxStyle(juce::Slider::TextBoxLeft, false, 50, 20);
    historySlider->onValueChange = [this]() {
        historySize = static_cast<int>(historySlider->getValue());
    };
    addAndMakeVisible(historySlider.get());

    scaleSlider = std::make_unique<juce::Slider>("Amplitude Scale");
    scaleSlider->setRange(0.1, 5.0, 0.1);
    scaleSlider->setValue(amplitudeScale);
    scaleSlider->setSliderStyle(juce::Slider::LinearHorizontal);
    scaleSlider->setTextBoxStyle(juce::Slider::TextBoxLeft, false, 50, 20);
    scaleSlider->onValueChange = [this]() {
        amplitudeScale = static_cast<float>(scaleSlider->getValue());
    };
    addAndMakeVisible(scaleSlider.get());

    gridToggle = std::make_unique<juce::ToggleButton>("Show Grid");
    gridToggle->setToggleState(showGrid, juce::dontSendNotification);
    gridToggle->onClick = [this]() {
        showGrid = gridToggle->getToggleState();
    };
    addAndMakeVisible(gridToggle.get());

    axesToggle = std::make_unique<juce::ToggleButton>("Show Axes");
    axesToggle->setToggleState(showAxes, juce::dontSendNotification);
    axesToggle->onClick = [this]() {
        showAxes = axesToggle->getToggleState();
    };
    addAndMakeVisible(axesToggle.get());
}

HarmonicsSurface3D::~HarmonicsSurface3D()
{
}

void HarmonicsSurface3D::resized()
{
    OpenGLVisualizerBase::resized();

    auto bounds = getLocalBounds();
    auto controlArea = bounds.removeFromTop(40);
    controlArea.reduce(10, 5);

    int controlWidth = 120;
    int sliderWidth = 150;
    int spacing = 10;

    modeSelector->setBounds(controlArea.removeFromLeft(controlWidth));
    controlArea.removeFromLeft(spacing);

    harmonicsSlider->setBounds(controlArea.removeFromLeft(sliderWidth));
    controlArea.removeFromLeft(spacing);

    historySlider->setBounds(controlArea.removeFromLeft(sliderWidth));
    controlArea.removeFromLeft(spacing);

    scaleSlider->setBounds(controlArea.removeFromLeft(sliderWidth));
    controlArea.removeFromLeft(spacing);

    gridToggle->setBounds(controlArea.removeFromLeft(80));
    controlArea.removeFromLeft(spacing);

    axesToggle->setBounds(controlArea.removeFromLeft(80));
}

void HarmonicsSurface3D::initializeGL()
{
}

void HarmonicsSurface3D::renderScene()
{
    const juce::ScopedLock lock(renderLock);

    if (harmonicHistory.empty() || vertices.empty())
        return;

    glBindVertexArray(vertexArrayObject);

    switch (displayMode)
    {
        case SURFACE:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
            break;

        case WIREFRAME:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
            break;

        case POINTS:
            glPointSize(3.0f);
            glDrawElements(GL_POINTS, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
            break;

        case FILLED_WIREFRAME:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);

            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glLineWidth(1.0f);
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
            break;
    }

    glBindVertexArray(0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void HarmonicsSurface3D::cleanupGL()
{
}

void HarmonicsSurface3D::updateVisualizationData()
{
    if (!projectManager)
        return;

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
        std::vector<float> currentHarmonics(maxHarmonics);

        float binResolution = sampleRate / static_cast<float>(currentFFTSize);

        for (int h = 0; h < maxHarmonics; ++h)
        {
            float harmonicFreq = fundamentalFreq * (h + 1);
            int bin = static_cast<int>(harmonicFreq / binResolution);

            if (bin >= 0 && bin < fftDataSize)
            {
                float amplitude = fftData[bin];
                float db = 20.0f * std::log10(std::max(amplitude, 1e-10f));
                db = juce::jlimit(-80.0f, 0.0f, db);
                float normalizedAmp = (db + 80.0f) / 80.0f;
                currentHarmonics[h] = normalizedAmp * amplitudeScale;
            }
            else
            {
                currentHarmonics[h] = 0.0f;
            }
        }

        harmonicHistory.push_back(currentHarmonics);

        while (harmonicHistory.size() > static_cast<size_t>(historySize))
            harmonicHistory.pop_front();

        buildSurfaceMesh();
    }
}

void HarmonicsSurface3D::buildSurfaceMesh()
{
    const juce::ScopedLock lock(renderLock);

    vertices.clear();
    indices.clear();

    if (harmonicHistory.empty())
        return;

    int timeSteps = static_cast<int>(harmonicHistory.size());
    int freqSteps = maxHarmonics;

    float xScale = 8.0f / freqSteps;
    float zScale = 8.0f / timeSteps;

    for (int t = 0; t < timeSteps; ++t)
    {
        const auto& harmonics = harmonicHistory[t];

        for (int f = 0; f < freqSteps && f < static_cast<int>(harmonics.size()); ++f)
        {
            Vertex3D vertex;

            float x = (f - freqSteps / 2.0f) * xScale;
            float z = (t - timeSteps / 2.0f) * zScale;
            float y = harmonics[f];

            vertex.position[0] = x;
            vertex.position[1] = y;
            vertex.position[2] = z;

            juce::Colour color = getAmplitudeColor(harmonics[f]);
            vertex.color[0] = color.getFloatRed();
            vertex.color[1] = color.getFloatGreen();
            vertex.color[2] = color.getFloatBlue();
            vertex.color[3] = 1.0f;

            vertices.push_back(vertex);
        }
    }

    calculateNormals();

    for (int t = 0; t < timeSteps - 1; ++t)
    {
        for (int f = 0; f < freqSteps - 1; ++f)
        {
            unsigned int topLeft = t * freqSteps + f;
            unsigned int topRight = t * freqSteps + (f + 1);
            unsigned int bottomLeft = (t + 1) * freqSteps + f;
            unsigned int bottomRight = (t + 1) * freqSteps + (f + 1);

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    updateBuffers();
}

void HarmonicsSurface3D::calculateNormals()
{
    int timeSteps = static_cast<int>(harmonicHistory.size());
    int freqSteps = maxHarmonics;

    for (int t = 0; t < timeSteps; ++t)
    {
        for (int f = 0; f < freqSteps; ++f)
        {
            int index = t * freqSteps + f;
            if (index >= static_cast<int>(vertices.size()))
                continue;

            juce::Vector3D<float> normal = calculateNormal(f, t);

            vertices[index].normal[0] = normal.x;
            vertices[index].normal[1] = normal.y;
            vertices[index].normal[2] = normal.z;
        }
    }
}

juce::Vector3D<float> HarmonicsSurface3D::calculateNormal(int x, int z)
{
    int timeSteps = static_cast<int>(harmonicHistory.size());
    int freqSteps = maxHarmonics;

    int index = z * freqSteps + x;

    if (index >= static_cast<int>(vertices.size()))
        return juce::Vector3D<float>(0, 1, 0);

    juce::Vector3D<float> center(vertices[index].position[0],
                                  vertices[index].position[1],
                                  vertices[index].position[2]);

    juce::Vector3D<float> dx(0, 0, 0);
    juce::Vector3D<float> dz(0, 0, 0);

    if (x < freqSteps - 1)
    {
        int rightIndex = z * freqSteps + (x + 1);
        juce::Vector3D<float> right(vertices[rightIndex].position[0],
                                    vertices[rightIndex].position[1],
                                    vertices[rightIndex].position[2]);
        dx = right - center;
    }

    if (z < timeSteps - 1)
    {
        int downIndex = (z + 1) * freqSteps + x;
        juce::Vector3D<float> down(vertices[downIndex].position[0],
                                   vertices[downIndex].position[1],
                                   vertices[downIndex].position[2]);
        dz = down - center;
    }

    juce::Vector3D<float> normal = dx.crossProduct(dz);
    return normal.normalised();
}

juce::Colour HarmonicsSurface3D::getFrequencyColor(float frequency) const
{
    float hue = std::fmod(std::log2(frequency / 440.0f) + 10.0f, 1.0f);
    return juce::Colour::fromHSV(hue, 0.8f, 1.0f, 1.0f);
}

juce::Colour HarmonicsSurface3D::getAmplitudeColor(float amplitude) const
{
    float normalized = juce::jlimit(0.0f, 1.0f, amplitude);

    if (normalized < 0.5f)
    {
        return juce::Colour::fromHSV(0.6f, 1.0f, normalized * 2.0f, 1.0f);
    }
    else
    {
        return juce::Colour::fromHSV(0.0f, 1.0f, 1.0f, 1.0f);
    }
}