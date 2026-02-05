/*
  ==============================================================================

    OpenGLVisualizerBase.h
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "ProjectManager.h"
#include "ResponsiveUIHelper.h"
#include <vector>
#include <memory>

struct Vertex3D
{
    float position[3];
    float normal[3];
    float color[4];
    float texCoord[2];
};

struct Matrix4x4
{
    float m[16];

    Matrix4x4() { identity(); }

    void identity()
    {
        for (int i = 0; i < 16; ++i)
            m[i] = (i % 5 == 0) ? 1.0f : 0.0f;
    }

    static Matrix4x4 perspective(float fov, float aspect, float near, float far);
    static Matrix4x4 lookAt(float eyeX, float eyeY, float eyeZ,
                           float centerX, float centerY, float centerZ,
                           float upX, float upY, float upZ);
    static Matrix4x4 rotation(float angle, float x, float y, float z);
    static Matrix4x4 translation(float x, float y, float z);
    static Matrix4x4 scale(float x, float y, float z);

    Matrix4x4 operator*(const Matrix4x4& other) const;
};

class Camera3D
{
public:
    Camera3D()
        : position(0, 0, 5),
          target(0, 0, 0),
          up(0, 1, 0),
          fov(45.0f),
          nearPlane(0.1f),
          farPlane(100.0f)
    {}

    void setPosition(float x, float y, float z) { position = juce::Vector3D<float>(x, y, z); }
    void setTarget(float x, float y, float z) { target = juce::Vector3D<float>(x, y, z); }
    void setUp(float x, float y, float z) { up = juce::Vector3D<float>(x, y, z); }

    void rotate(float deltaX, float deltaY);
    void zoom(float delta);
    void pan(float deltaX, float deltaY);

    Matrix4x4 getViewMatrix() const;
    Matrix4x4 getProjectionMatrix(float aspectRatio) const;

    juce::Vector3D<float> position;
    juce::Vector3D<float> target;
    juce::Vector3D<float> up;

    float fov;
    float nearPlane;
    float farPlane;

private:
    float distance = 5.0f;
    float azimuth = 0.0f;
    float elevation = 0.0f;
};

class OpenGLVisualizerBase : public ResponsiveComponent,
                              public juce::OpenGLRenderer,
                              public juce::Timer
{
public:
    OpenGLVisualizerBase(ProjectManager* pm);
    ~OpenGLVisualizerBase() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void newOpenGLContextCreated() override;
    void renderOpenGL() override;
    void openGLContextClosing() override;

    void timerCallback() override;

    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel) override;

protected:
    virtual void initializeGL() = 0;
    virtual void renderScene() = 0;
    virtual void cleanupGL() = 0;
    virtual void updateVisualizationData() = 0;

    void updateScaledValues() override;

    void setupLighting();
    void drawGrid(int size, float spacing);
    void drawAxes(float length);

    struct ShaderProgram
    {
        std::unique_ptr<juce::OpenGLShaderProgram> shader;
        std::unique_ptr<juce::OpenGLShaderProgram::Attribute> position;
        std::unique_ptr<juce::OpenGLShaderProgram::Attribute> normal;
        std::unique_ptr<juce::OpenGLShaderProgram::Attribute> color;
        std::unique_ptr<juce::OpenGLShaderProgram::Uniform> projectionMatrix;
        std::unique_ptr<juce::OpenGLShaderProgram::Uniform> viewMatrix;
        std::unique_ptr<juce::OpenGLShaderProgram::Uniform> modelMatrix;
        std::unique_ptr<juce::OpenGLShaderProgram::Uniform> lightPosition;
    };

    ProjectManager* projectManager;
    juce::OpenGLContext openGLContext;

    Camera3D camera;

    std::unique_ptr<ShaderProgram> basicShader;

    bool showGrid = true;
    bool showAxes = true;
    bool enableLighting = true;

    juce::Point<int> lastMousePos;

    juce::CriticalSection renderLock;

    std::vector<Vertex3D> vertices;
    std::vector<unsigned int> indices;

    GLuint vertexBuffer = 0;
    GLuint indexBuffer = 0;
    GLuint vertexArrayObject = 0;

    void createBuffers();
    void updateBuffers();
    void deleteBuffers();

    static const char* vertexShader;
    static const char* fragmentShader;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OpenGLVisualizerBase)
};

inline float degreesToRadians(float degrees)
{
    return degrees * juce::MathConstants<float>::pi / 180.0f;
}

inline juce::Vector3D<float> sphericalToCartesian(float radius, float azimuth, float elevation)
{
    float x = radius * std::cos(elevation) * std::sin(azimuth);
    float y = radius * std::sin(elevation);
    float z = radius * std::cos(elevation) * std::cos(azimuth);
    return juce::Vector3D<float>(x, y, z);
}