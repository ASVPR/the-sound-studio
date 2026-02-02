/*
  ==============================================================================

    EnhancedLissajousEngine.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <memory>
#include <vector>
#include <array>
#include <cmath>

//==============================================================================
// Enhanced Lissajous Configuration
#define MAX_LISSAJOUS_AXES 6
#define MAX_CURVE_POINTS 2048
#define CURVE_BUFFER_SIZE (MAX_CURVE_POINTS * MAX_LISSAJOUS_AXES)

//==============================================================================
// Lissajous Axis Data
struct LissajousAxis
{
    float frequency;        // Oscillation frequency (Hz)
    float amplitude;        // Amplitude (0-1)
    float phase;           // Phase offset (0-2π)
    float offset;          // DC offset (-1 to 1)
    bool isActive;         // Whether this axis is enabled
    
    // Modulation parameters
    float freqModDepth;    // Frequency modulation depth
    float freqModRate;     // Frequency modulation rate
    float ampModDepth;     // Amplitude modulation depth
    float ampModRate;      // Amplitude modulation rate
    
    // Wave shape
    enum WaveShape { SINE, TRIANGLE, SQUARE, SAWTOOTH, NOISE, CUSTOM };
    WaveShape waveShape;
    
    LissajousAxis() 
        : frequency(440.0f), amplitude(1.0f), phase(0.0f), offset(0.0f), isActive(true)
        , freqModDepth(0.0f), freqModRate(0.0f), ampModDepth(0.0f), ampModRate(0.0f)
        , waveShape(SINE)
    {}
};

//==============================================================================
// 3D Point structure for curve data
struct Point3D
{
    float x, y, z;
    float time;
    Colour color;
    
    Point3D() : x(0), y(0), z(0), time(0), color(Colours::white) {}
    Point3D(float x_, float y_, float z_, float t = 0) : x(x_), y(y_), z(z_), time(t), color(Colours::white) {}
};

//==============================================================================
// Multi-dimensional point for higher dimensions
struct PointND
{
    std::array<float, MAX_LISSAJOUS_AXES> coords;
    float time;
    Colour color;
    
    PointND() : time(0), color(Colours::white)
    {
        coords.fill(0.0f);
    }
    
    float& operator[](int axis) { return coords[axis]; }
    const float& operator[](int axis) const { return coords[axis]; }
};

//==============================================================================
// Curve generation modes
enum class CurveMode
{
    CLASSIC_2D,         // Traditional X-Y Lissajous
    CLASSIC_3D,         // X-Y-Z Lissajous
    MULTI_AXIS,         // Up to 6 axes
    PROJECTION_2D,      // Project higher dimensions to 2D
    PROJECTION_3D       // Project higher dimensions to 3D
};

//==============================================================================
// Color coding schemes for multi-axis curves
enum class ColorScheme
{
    SINGLE_COLOR,       // One color for entire curve
    TIME_BASED,         // Color changes over time
    VELOCITY_BASED,     // Color based on curve velocity
    AXIS_BASED,         // Color based on dominant axis
    RAINBOW,           // Rainbow progression
    CUSTOM             // User-defined colors
};

//==============================================================================
/**
    Enhanced Lissajous Engine - Advanced multi-dimensional curve generation
    
    This class implements enhanced Lissajous curve generation with:
    - Support for 2D to 6D curves (configurable number of axes)
    - Real-time parameter modulation
    - Advanced wave shapes and modulation
    - Color coding for visualization
    - Projection modes for higher dimensions
*/
class EnhancedLissajousEngine : public Timer
{
public:
    EnhancedLissajousEngine();
    ~EnhancedLissajousEngine();
    
    //==============================================================================
    // Configuration
    void initialize(double sampleRate = 44100.0, int updateRateHz = 60);
    void shutdown();
    
    //==============================================================================
    // Axis Management
    void setNumberOfAxes(int numAxes);
    int getNumberOfAxes() const { return numActiveAxes; }
    int getMaxAxes() const { return MAX_LISSAJOUS_AXES; }
    
    LissajousAxis& getAxis(int axisIndex);
    const LissajousAxis& getAxis(int axisIndex) const;
    
    void setAxisActive(int axisIndex, bool isActive);
    bool isAxisActive(int axisIndex) const;
    
    //==============================================================================
    // Curve Generation
    void setCurveMode(CurveMode mode) { curveMode = mode; needsRecalculation = true; }
    CurveMode getCurveMode() const { return curveMode; }
    
    void setResolution(int numPoints);
    int getResolution() const { return curveResolution; }
    
    void setTimeSpan(float seconds) { timeSpan = seconds; needsRecalculation = true; }
    float getTimeSpan() const { return timeSpan; }
    
    //==============================================================================
    // Visual Settings
    void setColorScheme(ColorScheme scheme) { colorScheme = scheme; needsColorUpdate = true; }
    ColorScheme getColorScheme() const { return colorScheme; }
    
    void setPrimaryColor(Colour color) { primaryColor = color; needsColorUpdate = true; }
    void setSecondaryColor(Colour color) { secondaryColor = color; needsColorUpdate = true; }
    
    //==============================================================================
    // Curve Data Access
    const std::vector<Point3D>& get3DCurve() const { return curve3D; }
    const std::vector<PointND>& getMultiAxisCurve() const { return curveND; }
    
    // Get 2D projection of current curve
    std::vector<Point<float>> get2DProjection(int xAxis = 0, int yAxis = 1) const;
    
    //==============================================================================
    // Real-time Control
    void setGlobalTimeScale(float scale) { globalTimeScale = scale; }
    void setGlobalAmplitude(float amp) { globalAmplitude = amp; }
    void setGlobalPhaseShift(float phase) { globalPhaseShift = phase; }
    
    //==============================================================================
    // Animation
    void setAnimationSpeed(float speed) { animationSpeed = speed; }
    void setAnimationMode(bool enabled) { animationEnabled = enabled; }
    bool isAnimationEnabled() const { return animationEnabled; }
    
    //==============================================================================
    // Presets
    void loadClassic2D();           // Classic X-Y Lissajous
    void loadClassic3D();           // Classic X-Y-Z Lissajous  
    void loadFlower();              // Flower-like pattern
    void loadSpiral();              // Spiral pattern
    void loadChaotic();             // Chaotic multi-axis pattern
    void loadHarmonics();           // Musical harmonic ratios
    
    //==============================================================================
    // Analysis
    struct CurveAnalysis
    {
        float totalLength;          // Arc length of curve
        float averageVelocity;      // Average velocity
        float maxVelocity;          // Maximum velocity
        Point3D centerOfMass;       // Center of mass
        float boundingBoxSize;      // Size of bounding box
        int numSelfIntersections;   // Self-intersection count
    };
    
    CurveAnalysis analyzeCurve() const;
    
    //==============================================================================
    // Listener interface for curve updates
    class Listener
    {
    public:
        virtual ~Listener() = default;
        virtual void curveUpdated(const EnhancedLissajousEngine& engine) = 0;
        virtual void analysisUpdated(const CurveAnalysis& analysis) = 0;
    };
    
    void addListener(Listener* listener);
    void removeListener(Listener* listener);
    
    //==============================================================================
    // Timer callback for animation
    void timerCallback() override;
    
private:
    //==============================================================================
    // Core data
    std::array<LissajousAxis, MAX_LISSAJOUS_AXES> axes;
    int numActiveAxes = 3;
    
    //==============================================================================
    // Curve generation settings
    CurveMode curveMode = CurveMode::CLASSIC_3D;
    int curveResolution = 1024;
    float timeSpan = 4.0f;          // Time span in seconds
    double sampleRate = 44100.0;
    
    //==============================================================================
    // Generated curve data
    std::vector<Point3D> curve3D;
    std::vector<PointND> curveND;
    mutable std::mutex curveMutex;
    
    //==============================================================================
    // Visual settings
    ColorScheme colorScheme = ColorScheme::TIME_BASED;
    Colour primaryColor = Colours::cyan;
    Colour secondaryColor = Colours::magenta;
    
    //==============================================================================
    // Real-time parameters
    float globalTimeScale = 1.0f;
    float globalAmplitude = 1.0f;
    float globalPhaseShift = 0.0f;
    
    //==============================================================================
    // Animation
    bool animationEnabled = false;
    float animationSpeed = 1.0f;
    float animationTime = 0.0f;
    int updateRateHz = 60;
    
    //==============================================================================
    // Update flags
    std::atomic<bool> needsRecalculation{true};
    std::atomic<bool> needsColorUpdate{true};
    
    //==============================================================================
    // Listener management
    std::vector<Listener*> listeners;
    mutable SpinLock listenersLock;
    
    //==============================================================================
    // Internal processing
    void generateCurve();
    void generate2DCurve();
    void generate3DCurve();
    void generateMultiAxisCurve();
    
    void updateColors();
    void calculateTimeBasedColors();
    void calculateVelocityBasedColors();
    void calculateAxisBasedColors();
    
    float evaluateAxis(int axisIndex, float time) const;
    float evaluateWaveShape(LissajousAxis::WaveShape shape, float phase) const;
    
    Point3D projectToSphericalCoords(const PointND& point) const;
    Point3D projectToCylindricalCoords(const PointND& point) const;
    
    void notifyListeners();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EnhancedLissajousEngine)
};