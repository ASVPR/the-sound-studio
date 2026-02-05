/*
  ==============================================================================

    OpenGLVisualizerBase.cpp
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#include "OpenGLVisualizerBase.h"
#include <cmath>

const char* OpenGLVisualizerBase::vertexShader = R"(
    #version 330 core

    layout(location = 0) in vec3 position;
    layout(location = 1) in vec3 normal;
    layout(location = 2) in vec4 color;

    uniform mat4 projectionMatrix;
    uniform mat4 viewMatrix;
    uniform mat4 modelMatrix;

    out vec3 FragPos;
    out vec3 Normal;
    out vec4 Color;

    void main()
    {
        FragPos = vec3(modelMatrix * vec4(position, 1.0));
        Normal = mat3(transpose(inverse(modelMatrix))) * normal;
        Color = color;
        gl_Position = projectionMatrix * viewMatrix * vec4(FragPos, 1.0);
    }
)";

const char* OpenGLVisualizerBase::fragmentShader = R"(
    #version 330 core

    in vec3 FragPos;
    in vec3 Normal;
    in vec4 Color;

    uniform vec3 lightPosition;

    out vec4 FragColor;

    void main()
    {
        vec3 lightDir = normalize(lightPosition - FragPos);
        vec3 norm = normalize(Normal);

        float ambient = 0.3;
        float diff = max(dot(norm, lightDir), 0.0);

        vec3 result = (ambient + diff) * Color.rgb;
        FragColor = vec4(result, Color.a);
    }
)";

Matrix4x4 Matrix4x4::perspective(float fov, float aspect, float near, float far)
{
    Matrix4x4 result;
    float tanHalfFov = std::tan(fov / 2.0f);

    result.m[0] = 1.0f / (aspect * tanHalfFov);
    result.m[5] = 1.0f / tanHalfFov;
    result.m[10] = -(far + near) / (far - near);
    result.m[11] = -1.0f;
    result.m[14] = -(2.0f * far * near) / (far - near);
    result.m[15] = 0.0f;

    return result;
}

Matrix4x4 Matrix4x4::lookAt(float eyeX, float eyeY, float eyeZ,
                            float centerX, float centerY, float centerZ,
                            float upX, float upY, float upZ)
{
    juce::Vector3D<float> f(centerX - eyeX, centerY - eyeY, centerZ - eyeZ);
    f = f.normalised();

    juce::Vector3D<float> u(upX, upY, upZ);
    u = u.normalised();

    juce::Vector3D<float> s = f.crossProduct(u).normalised();
    u = s.crossProduct(f);

    Matrix4x4 result;
    result.m[0] = s.x;
    result.m[4] = s.y;
    result.m[8] = s.z;
    result.m[1] = u.x;
    result.m[5] = u.y;
    result.m[9] = u.z;
    result.m[2] = -f.x;
    result.m[6] = -f.y;
    result.m[10] = -f.z;
    result.m[12] = -s.dotProduct(juce::Vector3D<float>(eyeX, eyeY, eyeZ));
    result.m[13] = -u.dotProduct(juce::Vector3D<float>(eyeX, eyeY, eyeZ));
    result.m[14] = f.dotProduct(juce::Vector3D<float>(eyeX, eyeY, eyeZ));

    return result;
}

Matrix4x4 Matrix4x4::rotation(float angle, float x, float y, float z)
{
    float c = std::cos(angle);
    float s = std::sin(angle);
    float t = 1.0f - c;

    Matrix4x4 result;
    result.m[0] = t * x * x + c;
    result.m[1] = t * x * y + s * z;
    result.m[2] = t * x * z - s * y;
    result.m[4] = t * x * y - s * z;
    result.m[5] = t * y * y + c;
    result.m[6] = t * y * z + s * x;
    result.m[8] = t * x * z + s * y;
    result.m[9] = t * y * z - s * x;
    result.m[10] = t * z * z + c;

    return result;
}

Matrix4x4 Matrix4x4::translation(float x, float y, float z)
{
    Matrix4x4 result;
    result.m[12] = x;
    result.m[13] = y;
    result.m[14] = z;
    return result;
}

Matrix4x4 Matrix4x4::scale(float x, float y, float z)
{
    Matrix4x4 result;
    result.m[0] = x;
    result.m[5] = y;
    result.m[10] = z;
    return result;
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4& other) const
{
    Matrix4x4 result;
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            result.m[i * 4 + j] = 0;
            for (int k = 0; k < 4; ++k)
                result.m[i * 4 + j] += m[i * 4 + k] * other.m[k * 4 + j];
        }
    }
    return result;
}

void Camera3D::rotate(float deltaX, float deltaY)
{
    azimuth += deltaX;
    elevation += deltaY;

    elevation = juce::jlimit(-juce::MathConstants<float>::pi / 2.0f + 0.1f,
                             juce::MathConstants<float>::pi / 2.0f - 0.1f,
                             elevation);

    position = target + sphericalToCartesian(distance, azimuth, elevation);
}

void Camera3D::zoom(float delta)
{
    distance *= (1.0f - delta * 0.1f);
    distance = juce::jlimit(1.0f, 50.0f, distance);

    position = target + sphericalToCartesian(distance, azimuth, elevation);
}

void Camera3D::pan(float deltaX, float deltaY)
{
    juce::Vector3D<float> forward = (target - position).normalised();
    juce::Vector3D<float> right = forward.crossProduct(up).normalised();
    juce::Vector3D<float> actualUp = right.crossProduct(forward);

    juce::Vector3D<float> offset = right * deltaX + actualUp * deltaY;

    position += offset;
    target += offset;
}

Matrix4x4 Camera3D::getViewMatrix() const
{
    return Matrix4x4::lookAt(position.x, position.y, position.z,
                             target.x, target.y, target.z,
                             up.x, up.y, up.z);
}

Matrix4x4 Camera3D::getProjectionMatrix(float aspectRatio) const
{
    return Matrix4x4::perspective(degreesToRadians(fov), aspectRatio, nearPlane, farPlane);
}

OpenGLVisualizerBase::OpenGLVisualizerBase(ProjectManager* pm)
    : projectManager(pm)
{
    openGLContext.setRenderer(this);
    openGLContext.attachTo(*this);
    openGLContext.setContinuousRepainting(true);

    setWantsKeyboardFocus(true);

    startTimer(33);
}

OpenGLVisualizerBase::~OpenGLVisualizerBase()
{
    stopTimer();
    openGLContext.detach();
}

void OpenGLVisualizerBase::paint(juce::Graphics& g)
{
}

void OpenGLVisualizerBase::resized()
{
    ResponsiveComponent::resized();
}

void OpenGLVisualizerBase::newOpenGLContextCreated()
{
    basicShader = std::make_unique<ShaderProgram>();
    basicShader->shader = std::make_unique<juce::OpenGLShaderProgram>(openGLContext);

    if (basicShader->shader->addVertexShader(vertexShader) &&
        basicShader->shader->addFragmentShader(fragmentShader) &&
        basicShader->shader->link())
    {
        basicShader->position = std::make_unique<juce::OpenGLShaderProgram::Attribute>(*basicShader->shader, "position");
        basicShader->normal = std::make_unique<juce::OpenGLShaderProgram::Attribute>(*basicShader->shader, "normal");
        basicShader->color = std::make_unique<juce::OpenGLShaderProgram::Attribute>(*basicShader->shader, "color");

        basicShader->projectionMatrix = std::make_unique<juce::OpenGLShaderProgram::Uniform>(*basicShader->shader, "projectionMatrix");
        basicShader->viewMatrix = std::make_unique<juce::OpenGLShaderProgram::Uniform>(*basicShader->shader, "viewMatrix");
        basicShader->modelMatrix = std::make_unique<juce::OpenGLShaderProgram::Uniform>(*basicShader->shader, "modelMatrix");
        basicShader->lightPosition = std::make_unique<juce::OpenGLShaderProgram::Uniform>(*basicShader->shader, "lightPosition");
    }

    createBuffers();
    initializeGL();
}

void OpenGLVisualizerBase::renderOpenGL()
{
    jassert(juce::OpenGLHelpers::isContextActive());

    const juce::ScopedLock lock(renderLock);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (basicShader && basicShader->shader)
    {
        basicShader->shader->use();

        float aspectRatio = getWidth() / (float)getHeight();
        Matrix4x4 projection = camera.getProjectionMatrix(aspectRatio);
        Matrix4x4 view = camera.getViewMatrix();
        Matrix4x4 model;
        model.identity();

        basicShader->projectionMatrix->setMatrix4(projection.m, 1, false);
        basicShader->viewMatrix->setMatrix4(view.m, 1, false);
        basicShader->modelMatrix->setMatrix4(model.m, 1, false);
        basicShader->lightPosition->set(5.0f, 5.0f, 5.0f);

        if (showGrid)
            drawGrid(10, 1.0f);

        if (showAxes)
            drawAxes(5.0f);

        renderScene();
    }
}

void OpenGLVisualizerBase::openGLContextClosing()
{
    cleanupGL();
    deleteBuffers();
    basicShader.reset();
}

void OpenGLVisualizerBase::timerCallback()
{
    updateVisualizationData();
    openGLContext.triggerRepaint();
}

void OpenGLVisualizerBase::mouseDown(const juce::MouseEvent& e)
{
    lastMousePos = e.getPosition();
}

void OpenGLVisualizerBase::mouseDrag(const juce::MouseEvent& e)
{
    auto delta = e.getPosition() - lastMousePos;

    if (e.mods.isLeftButtonDown())
    {
        camera.rotate(delta.x * 0.01f, delta.y * 0.01f);
    }
    else if (e.mods.isRightButtonDown() || e.mods.isMiddleButtonDown())
    {
        camera.pan(-delta.x * 0.01f, delta.y * 0.01f);
    }

    lastMousePos = e.getPosition();
}

void OpenGLVisualizerBase::mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel)
{
    camera.zoom(wheel.deltaY);
}

void OpenGLVisualizerBase::updateScaledValues()
{
}

void OpenGLVisualizerBase::setupLighting()
{
}

void OpenGLVisualizerBase::drawGrid(int size, float spacing)
{
    glBegin(GL_LINES);
    glColor4f(0.3f, 0.3f, 0.3f, 0.5f);

    float extent = size * spacing / 2.0f;

    for (int i = -size/2; i <= size/2; ++i)
    {
        float pos = i * spacing;

        glVertex3f(pos, 0, -extent);
        glVertex3f(pos, 0, extent);

        glVertex3f(-extent, 0, pos);
        glVertex3f(extent, 0, pos);
    }

    glEnd();
}

void OpenGLVisualizerBase::drawAxes(float length)
{
    glBegin(GL_LINES);

    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
    glVertex3f(0, 0, 0);
    glVertex3f(length, 0, 0);

    glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
    glVertex3f(0, 0, 0);
    glVertex3f(0, length, 0);

    glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, length);

    glEnd();
}

void OpenGLVisualizerBase::createBuffers()
{
    glGenVertexArrays(1, &vertexArrayObject);
    glGenBuffers(1, &vertexBuffer);
    glGenBuffers(1, &indexBuffer);
}

void OpenGLVisualizerBase::updateBuffers()
{
    if (vertices.empty())
        return;

    glBindVertexArray(vertexArrayObject);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex3D), vertices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, color));

    glBindVertexArray(0);
}

void OpenGLVisualizerBase::deleteBuffers()
{
    if (vertexArrayObject != 0)
    {
        glDeleteVertexArrays(1, &vertexArrayObject);
        vertexArrayObject = 0;
    }

    if (vertexBuffer != 0)
    {
        glDeleteBuffers(1, &vertexBuffer);
        vertexBuffer = 0;
    }

    if (indexBuffer != 0)
    {
        glDeleteBuffers(1, &indexBuffer);
        indexBuffer = 0;
    }
}