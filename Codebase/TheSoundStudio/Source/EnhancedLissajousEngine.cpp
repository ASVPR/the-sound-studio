/*
  ==============================================================================

    EnhancedLissajousEngine.cpp

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#include "EnhancedLissajousEngine.h"

//==============================================================================
EnhancedLissajousEngine::EnhancedLissajousEngine()
{
    // Initialize default axis parameters
    axes[0].frequency = 1.0f;    // X axis
    axes[1].frequency = 2.0f;    // Y axis  
    axes[2].frequency = 3.0f;    // Z axis
    axes[3].frequency = 5.0f;    // 4th dimension
    axes[4].frequency = 7.0f;    // 5th dimension
    axes[5].frequency = 11.0f;   // 6th dimension
    
    // Set default phases for interesting patterns
    for (int i = 0; i < MAX_LISSAJOUS_AXES; ++i)
    {
        axes[i].phase = i * MathConstants<float>::pi / 4.0f; // Staggered phases
    }
}

EnhancedLissajousEngine::~EnhancedLissajousEngine()
{
    shutdown();
}

//==============================================================================
// Configuration

void EnhancedLissajousEngine::initialize(double newSampleRate, int updateRate)
{
    sampleRate = newSampleRate;
    updateRateHz = updateRate;
    
    // Reserve memory for curve data
    curve3D.reserve(MAX_CURVE_POINTS);
    curveND.reserve(MAX_CURVE_POINTS);
    
    generateCurve();
    
    // Start animation timer if needed
    if (animationEnabled)
        startTimer(1000 / updateRateHz);
}

void EnhancedLissajousEngine::shutdown()
{
    stopTimer();
}

//==============================================================================
// Axis Management

void EnhancedLissajousEngine::setNumberOfAxes(int numAxes)
{
    numActiveAxes = jlimit(2, MAX_LISSAJOUS_AXES, numAxes);
    
    // Activate/deactivate axes as needed
    for (int i = 0; i < MAX_LISSAJOUS_AXES; ++i)
    {
        axes[i].isActive = (i < numActiveAxes);
    }
    
    needsRecalculation = true;
}

LissajousAxis& EnhancedLissajousEngine::getAxis(int axisIndex)
{
    jassert(axisIndex >= 0 && axisIndex < MAX_LISSAJOUS_AXES);
    return axes[axisIndex];
}

const LissajousAxis& EnhancedLissajousEngine::getAxis(int axisIndex) const
{
    jassert(axisIndex >= 0 && axisIndex < MAX_LISSAJOUS_AXES);
    return axes[axisIndex];
}

void EnhancedLissajousEngine::setAxisActive(int axisIndex, bool isActive)
{
    if (axisIndex >= 0 && axisIndex < MAX_LISSAJOUS_AXES)
    {
        axes[axisIndex].isActive = isActive;
        needsRecalculation = true;
    }
}

bool EnhancedLissajousEngine::isAxisActive(int axisIndex) const
{
    if (axisIndex >= 0 && axisIndex < MAX_LISSAJOUS_AXES)
        return axes[axisIndex].isActive;
    return false;
}

//==============================================================================
// Curve Generation

void EnhancedLissajousEngine::setResolution(int numPoints)
{
    curveResolution = jlimit(64, MAX_CURVE_POINTS, numPoints);
    needsRecalculation = true;
}

//==============================================================================
// Curve Data Access

std::vector<Point<float>> EnhancedLissajousEngine::get2DProjection(int xAxis, int yAxis) const
{
    std::lock_guard<std::mutex> lock(curveMutex);
    
    std::vector<Point<float>> projection;
    projection.reserve(curveND.size());
    
    xAxis = jlimit(0, numActiveAxes - 1, xAxis);
    yAxis = jlimit(0, numActiveAxes - 1, yAxis);
    
    for (const auto& point : curveND)
    {
        projection.emplace_back(point[xAxis], point[yAxis]);
    }
    
    return projection;
}

//==============================================================================
// Presets

void EnhancedLissajousEngine::loadClassic2D()
{
    setNumberOfAxes(2);
    
    axes[0].frequency = 3.0f;
    axes[0].amplitude = 1.0f;
    axes[0].phase = 0.0f;
    axes[0].waveShape = LissajousAxis::SINE;
    
    axes[1].frequency = 4.0f;
    axes[1].amplitude = 1.0f;
    axes[1].phase = MathConstants<float>::pi / 2.0f;
    axes[1].waveShape = LissajousAxis::SINE;
    
    setCurveMode(CurveMode::CLASSIC_2D);
    needsRecalculation = true;
}

void EnhancedLissajousEngine::loadClassic3D()
{
    setNumberOfAxes(3);
    
    axes[0].frequency = 1.0f;
    axes[0].amplitude = 1.0f;
    axes[0].phase = 0.0f;
    axes[0].waveShape = LissajousAxis::SINE;
    
    axes[1].frequency = 2.0f;
    axes[1].amplitude = 1.0f;
    axes[1].phase = MathConstants<float>::pi / 2.0f;
    axes[1].waveShape = LissajousAxis::SINE;
    
    axes[2].frequency = 3.0f;
    axes[2].amplitude = 0.8f;
    axes[2].phase = MathConstants<float>::pi / 4.0f;
    axes[2].waveShape = LissajousAxis::SINE;
    
    setCurveMode(CurveMode::CLASSIC_3D);
    needsRecalculation = true;
}

void EnhancedLissajousEngine::loadFlower()
{
    setNumberOfAxes(4);
    
    // Flower pattern with harmonic relationships
    axes[0].frequency = 5.0f;
    axes[0].amplitude = 1.0f;
    axes[0].waveShape = LissajousAxis::SINE;
    
    axes[1].frequency = 8.0f;
    axes[1].amplitude = 0.8f;
    axes[1].waveShape = LissajousAxis::SINE;
    
    axes[2].frequency = 13.0f;
    axes[2].amplitude = 0.6f;
    axes[2].waveShape = LissajousAxis::TRIANGLE;
    
    axes[3].frequency = 21.0f;
    axes[3].amplitude = 0.4f;
    axes[3].waveShape = LissajousAxis::SINE;
    
    setColorScheme(ColorScheme::RAINBOW);
    setCurveMode(CurveMode::PROJECTION_3D);
    needsRecalculation = true;
}

void EnhancedLissajousEngine::loadSpiral()
{
    setNumberOfAxes(3);
    
    axes[0].frequency = 1.0f;
    axes[0].amplitude = 1.0f;
    axes[0].waveShape = LissajousAxis::SINE;
    
    axes[1].frequency = 1.0f;
    axes[1].amplitude = 1.0f;
    axes[1].phase = MathConstants<float>::pi / 2.0f;
    axes[1].waveShape = LissajousAxis::SINE;
    
    // Slowly increasing amplitude for spiral effect
    axes[2].frequency = 0.1f;
    axes[2].amplitude = 2.0f;
    axes[2].waveShape = LissajousAxis::SAWTOOTH;
    
    setColorScheme(ColorScheme::TIME_BASED);
    setCurveMode(CurveMode::CLASSIC_3D);
    needsRecalculation = true;
}

void EnhancedLissajousEngine::loadChaotic()
{
    setNumberOfAxes(6); // Use all 6 axes
    
    // Prime number frequencies for aperiodic behavior
    float primeFreqs[] = {2.0f, 3.0f, 5.0f, 7.0f, 11.0f, 13.0f};
    
    for (int i = 0; i < 6; ++i)
    {
        axes[i].frequency = primeFreqs[i] * 0.5f;
        axes[i].amplitude = 1.0f / (i + 1); // Decreasing amplitudes
        axes[i].phase = i * MathConstants<float>::pi / 3.0f;
        axes[i].waveShape = (LissajousAxis::WaveShape)(i % 4); // Cycle through shapes
    }
    
    setColorScheme(ColorScheme::VELOCITY_BASED);
    setCurveMode(CurveMode::MULTI_AXIS);
    needsRecalculation = true;
}

void EnhancedLissajousEngine::loadHarmonics()
{
    setNumberOfAxes(5);
    
    // Musical harmonic series: 1, 2, 3, 4, 5
    for (int i = 0; i < 5; ++i)
    {
        axes[i].frequency = 110.0f * (i + 1); // A2 and harmonics
        axes[i].amplitude = 1.0f / std::sqrt(i + 1); // Natural harmonic falloff
        axes[i].phase = 0.0f;
        axes[i].waveShape = LissajousAxis::SINE;
    }
    
    setColorScheme(ColorScheme::AXIS_BASED);
    setCurveMode(CurveMode::PROJECTION_3D);
    needsRecalculation = true;
}

//==============================================================================
// Analysis

EnhancedLissajousEngine::CurveAnalysis EnhancedLissajousEngine::analyzeCurve() const
{
    std::lock_guard<std::mutex> lock(curveMutex);
    
    CurveAnalysis analysis;
    
    if (curve3D.empty())
        return analysis;
    
    // Calculate arc length and velocities
    float totalLength = 0.0f;
    float totalVelocity = 0.0f;
    float maxVel = 0.0f;
    
    Point3D minBounds = curve3D[0];
    Point3D maxBounds = curve3D[0];
    Point3D centerSum(0, 0, 0);
    
    for (size_t i = 1; i < curve3D.size(); ++i)
    {
        const auto& p1 = curve3D[i - 1];
        const auto& p2 = curve3D[i];
        
        // Arc length
        float dx = p2.x - p1.x;
        float dy = p2.y - p1.y;
        float dz = p2.z - p1.z;
        float segmentLength = std::sqrt(dx*dx + dy*dy + dz*dz);
        totalLength += segmentLength;
        
        // Velocity
        float dt = p2.time - p1.time;
        if (dt > 0)
        {
            float velocity = segmentLength / dt;
            totalVelocity += velocity;
            maxVel = jmax(maxVel, velocity);
        }
        
        // Bounding box
        minBounds.x = jmin(minBounds.x, p2.x);
        minBounds.y = jmin(minBounds.y, p2.y);
        minBounds.z = jmin(minBounds.z, p2.z);
        
        maxBounds.x = jmax(maxBounds.x, p2.x);
        maxBounds.y = jmax(maxBounds.y, p2.y);
        maxBounds.z = jmax(maxBounds.z, p2.z);
        
        // Center of mass
        centerSum.x += p2.x;
        centerSum.y += p2.y;
        centerSum.z += p2.z;
    }
    
    analysis.totalLength = totalLength;
    analysis.averageVelocity = totalVelocity / (curve3D.size() - 1);
    analysis.maxVelocity = maxVel;
    
    analysis.centerOfMass.x = centerSum.x / curve3D.size();
    analysis.centerOfMass.y = centerSum.y / curve3D.size();
    analysis.centerOfMass.z = centerSum.z / curve3D.size();
    
    float bx = maxBounds.x - minBounds.x;
    float by = maxBounds.y - minBounds.y;
    float bz = maxBounds.z - minBounds.z;
    analysis.boundingBoxSize = std::sqrt(bx*bx + by*by + bz*bz);
    
    // TODO: Calculate self-intersections (computationally expensive)
    analysis.numSelfIntersections = 0;
    
    return analysis;
}

//==============================================================================
// Internal Processing

void EnhancedLissajousEngine::generateCurve()
{
    std::lock_guard<std::mutex> lock(curveMutex);
    
    switch (curveMode)
    {
        case CurveMode::CLASSIC_2D:
            generate2DCurve();
            break;
        case CurveMode::CLASSIC_3D:
            generate3DCurve();
            break;
        case CurveMode::MULTI_AXIS:
        case CurveMode::PROJECTION_2D:
        case CurveMode::PROJECTION_3D:
            generateMultiAxisCurve();
            break;
    }
    
    if (needsColorUpdate.exchange(false))
        updateColors();
    
    needsRecalculation = false;
}

void EnhancedLissajousEngine::generate2DCurve()
{
    curve3D.clear();
    curve3D.reserve(curveResolution);
    
    for (int i = 0; i < curveResolution; ++i)
    {
        float t = (float)i / curveResolution * timeSpan;
        
        Point3D point;
        point.x = evaluateAxis(0, t);
        point.y = evaluateAxis(1, t);
        point.z = 0.0f;
        point.time = t;
        
        curve3D.push_back(point);
    }
}

void EnhancedLissajousEngine::generate3DCurve()
{
    curve3D.clear();
    curve3D.reserve(curveResolution);
    
    for (int i = 0; i < curveResolution; ++i)
    {
        float t = (float)i / curveResolution * timeSpan;
        
        Point3D point;
        point.x = evaluateAxis(0, t);
        point.y = evaluateAxis(1, t);
        point.z = evaluateAxis(2, t);
        point.time = t;
        
        curve3D.push_back(point);
    }
}

void EnhancedLissajousEngine::generateMultiAxisCurve()
{
    curveND.clear();
    curveND.reserve(curveResolution);
    
    curve3D.clear();
    curve3D.reserve(curveResolution);
    
    for (int i = 0; i < curveResolution; ++i)
    {
        float t = (float)i / curveResolution * timeSpan;
        
        PointND pointND;
        pointND.time = t;
        
        // Generate all active axes
        for (int axis = 0; axis < numActiveAxes; ++axis)
        {
            if (axes[axis].isActive)
                pointND[axis] = evaluateAxis(axis, t);
        }
        
        curveND.push_back(pointND);
        
        // Create 3D projection for visualization
        Point3D point3D;
        if (curveMode == CurveMode::PROJECTION_3D)
        {
            // Project higher dimensions to 3D
            if (numActiveAxes >= 6)
                point3D = projectToSphericalCoords(pointND);
            else
                point3D = projectToCylindricalCoords(pointND);
        }
        else
        {
            // Use first 3 dimensions directly
            point3D.x = (numActiveAxes > 0) ? pointND[0] : 0.0f;
            point3D.y = (numActiveAxes > 1) ? pointND[1] : 0.0f;
            point3D.z = (numActiveAxes > 2) ? pointND[2] : 0.0f;
        }
        
        point3D.time = t;
        curve3D.push_back(point3D);
    }
}

void EnhancedLissajousEngine::updateColors()
{
    switch (colorScheme)
    {
        case ColorScheme::TIME_BASED:
            calculateTimeBasedColors();
            break;
        case ColorScheme::VELOCITY_BASED:
            calculateVelocityBasedColors();
            break;
        case ColorScheme::AXIS_BASED:
            calculateAxisBasedColors();
            break;
        case ColorScheme::RAINBOW:
            calculateTimeBasedColors(); // Similar to time-based
            break;
        case ColorScheme::SINGLE_COLOR:
            for (auto& point : curve3D)
                point.color = primaryColor;
            for (auto& point : curveND)
                point.color = primaryColor;
            break;
        case ColorScheme::CUSTOM:
            // User would implement custom coloring
            break;
    }
}

void EnhancedLissajousEngine::calculateTimeBasedColors()
{
    for (size_t i = 0; i < curve3D.size(); ++i)
    {
        float ratio = (float)i / curve3D.size();
        
        if (colorScheme == ColorScheme::RAINBOW)
        {
            curve3D[i].color = Colour::fromHSV(ratio, 0.8f, 0.9f, 1.0f);
        }
        else
        {
            curve3D[i].color = primaryColor.interpolatedWith(secondaryColor, ratio);
        }
    }
    
    for (size_t i = 0; i < curveND.size(); ++i)
    {
        float ratio = (float)i / curveND.size();
        
        if (colorScheme == ColorScheme::RAINBOW)
        {
            curveND[i].color = Colour::fromHSV(ratio, 0.8f, 0.9f, 1.0f);
        }
        else
        {
            curveND[i].color = primaryColor.interpolatedWith(secondaryColor, ratio);
        }
    }
}

void EnhancedLissajousEngine::calculateVelocityBasedColors()
{
    if (curve3D.size() < 2)
        return;
    
    std::vector<float> velocities;
    velocities.reserve(curve3D.size());
    
    // Calculate velocities
    for (size_t i = 1; i < curve3D.size(); ++i)
    {
        const auto& p1 = curve3D[i - 1];
        const auto& p2 = curve3D[i];
        
        float dx = p2.x - p1.x;
        float dy = p2.y - p1.y;
        float dz = p2.z - p1.z;
        float dt = p2.time - p1.time;
        
        float velocity = (dt > 0) ? std::sqrt(dx*dx + dy*dy + dz*dz) / dt : 0.0f;
        velocities.push_back(velocity);
    }
    
    // Find min/max velocities for normalization
    auto minMax = std::minmax_element(velocities.begin(), velocities.end());
    float minVel = *minMax.first;
    float maxVel = *minMax.second;
    float velRange = maxVel - minVel;
    
    // Apply colors based on normalized velocity
    curve3D[0].color = primaryColor; // First point
    
    for (size_t i = 1; i < curve3D.size(); ++i)
    {
        float normalizedVel = (velRange > 0) ? (velocities[i - 1] - minVel) / velRange : 0.0f;
        curve3D[i].color = Colour::fromHSV(240.0f * (1.0f - normalizedVel) / 360.0f, 0.8f, 0.9f, 1.0f);
    }
}

void EnhancedLissajousEngine::calculateAxisBasedColors()
{
    for (size_t i = 0; i < curve3D.size(); ++i)
    {
        const auto& point = curve3D[i];
        
        // Find dominant axis
        float maxAmp = 0.0f;
        int dominantAxis = 0;
        
        if (i < curveND.size())
        {
            for (int axis = 0; axis < numActiveAxes; ++axis)
            {
                float amp = std::abs(curveND[i][axis]);
                if (amp > maxAmp)
                {
                    maxAmp = amp;
                    dominantAxis = axis;
                }
            }
        }
        
        // Color based on dominant axis
        float hue = (360.0f / numActiveAxes) * dominantAxis;
        curve3D[i].color = Colour::fromHSV(hue / 360.0f, 0.7f, 0.8f, 1.0f);
    }
}

float EnhancedLissajousEngine::evaluateAxis(int axisIndex, float time) const
{
    if (axisIndex < 0 || axisIndex >= MAX_LISSAJOUS_AXES || !axes[axisIndex].isActive)
        return 0.0f;
    
    const auto& axis = axes[axisIndex];
    
    // Apply global modulations
    float effectiveFreq = axis.frequency * globalTimeScale;
    float effectiveAmp = axis.amplitude * globalAmplitude;
    float effectivePhase = axis.phase + globalPhaseShift;
    
    // Apply frequency and amplitude modulation
    if (axis.freqModDepth > 0.0f)
    {
        float freqMod = axis.freqModDepth * std::sin(2.0f * MathConstants<float>::pi * axis.freqModRate * time);
        effectiveFreq *= (1.0f + freqMod);
    }
    
    if (axis.ampModDepth > 0.0f)
    {
        float ampMod = axis.ampModDepth * std::sin(2.0f * MathConstants<float>::pi * axis.ampModRate * time);
        effectiveAmp *= (1.0f + ampMod);
    }
    
    // Calculate phase
    float phase = 2.0f * MathConstants<float>::pi * effectiveFreq * time + effectivePhase;
    
    // Evaluate waveform
    float waveValue = evaluateWaveShape(axis.waveShape, phase);
    
    return effectiveAmp * waveValue + axis.offset;
}

float EnhancedLissajousEngine::evaluateWaveShape(LissajousAxis::WaveShape shape, float phase) const
{
    switch (shape)
    {
        case LissajousAxis::SINE:
            return std::sin(phase);
            
        case LissajousAxis::TRIANGLE:
            {
                float normalizedPhase = std::fmod(phase, 2.0f * MathConstants<float>::pi);
                if (normalizedPhase < 0) normalizedPhase += 2.0f * MathConstants<float>::pi;
                
                if (normalizedPhase < MathConstants<float>::pi)
                    return -1.0f + 2.0f * normalizedPhase / MathConstants<float>::pi;
                else
                    return 3.0f - 2.0f * normalizedPhase / MathConstants<float>::pi;
            }
            
        case LissajousAxis::SQUARE:
            return (std::sin(phase) >= 0) ? 1.0f : -1.0f;
            
        case LissajousAxis::SAWTOOTH:
            {
                float normalizedPhase = std::fmod(phase, 2.0f * MathConstants<float>::pi);
                if (normalizedPhase < 0) normalizedPhase += 2.0f * MathConstants<float>::pi;
                return -1.0f + normalizedPhase / MathConstants<float>::pi;
            }
            
        case LissajousAxis::NOISE:
            // Simple pseudo-random based on phase
            return 2.0f * (std::sin(phase * 12.9898f + 78.233f) * 43758.5453f - std::floor(std::sin(phase * 12.9898f + 78.233f) * 43758.5453f)) - 1.0f;
            
        case LissajousAxis::CUSTOM:
        default:
            return std::sin(phase); // Default to sine
    }
}

Point3D EnhancedLissajousEngine::projectToSphericalCoords(const PointND& point) const
{
    // Convert 6D coordinates to spherical coordinates
    float r = 0.0f;
    for (int i = 0; i < numActiveAxes; ++i)
    {
        r += point[i] * point[i];
    }
    r = std::sqrt(r);
    
    if (r < 0.001f) // Avoid division by zero
        return Point3D(0, 0, 0);
    
    // Use first few dimensions for spherical angles
    float theta = std::atan2(point[1], point[0]); // Azimuth
    float phi = std::acos(point[2] / r);          // Polar
    
    // Project to 3D Cartesian
    Point3D result;
    result.x = r * std::sin(phi) * std::cos(theta);
    result.y = r * std::sin(phi) * std::sin(theta);
    result.z = r * std::cos(phi);
    
    return result;
}

Point3D EnhancedLissajousEngine::projectToCylindricalCoords(const PointND& point) const
{
    // Simple cylindrical projection for 4-5D
    Point3D result;
    result.x = point[0];
    result.y = point[1];
    
    if (numActiveAxes >= 3)
        result.z = point[2];
    
    // Modulate with higher dimensions
    if (numActiveAxes >= 4)
    {
        float modulation = point[3] * 0.3f;
        result.x += modulation * std::cos(point.time);
        result.y += modulation * std::sin(point.time);
    }
    
    if (numActiveAxes >= 5)
    {
        result.z += point[4] * 0.5f;
    }
    
    return result;
}

//==============================================================================
// Listener Management

void EnhancedLissajousEngine::addListener(Listener* listener)
{
    const SpinLock::ScopedLockType lock(listenersLock);
    listeners.push_back(listener);
}

void EnhancedLissajousEngine::removeListener(Listener* listener)
{
    const SpinLock::ScopedLockType lock(listenersLock);
    listeners.erase(std::remove(listeners.begin(), listeners.end(), listener), listeners.end());
}

void EnhancedLissajousEngine::notifyListeners()
{
    const SpinLock::ScopedLockType lock(listenersLock);
    
    for (auto* listener : listeners)
    {
        listener->curveUpdated(*this);
        
        auto analysis = analyzeCurve();
        listener->analysisUpdated(analysis);
    }
}

//==============================================================================
// Timer Callback

void EnhancedLissajousEngine::timerCallback()
{
    if (animationEnabled)
    {
        animationTime += animationSpeed / updateRateHz;
        
        // Update global phase for animation
        globalPhaseShift = animationTime * 0.5f;
        
        needsRecalculation = true;
    }
    
    if (needsRecalculation.exchange(false))
    {
        generateCurve();
        notifyListeners();
    }
}