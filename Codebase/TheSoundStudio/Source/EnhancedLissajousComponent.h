/*
  ==============================================================================

    EnhancedLissajousComponent.h
    Created: 3 Sep 2025 8:00:00pm
    Author:  The Sound Studio Team

    Enhanced Lissajous Component with support for:
    - 2D to 6D Lissajous curves
    - Real-time parameter control
    - Multiple visualization modes
    - Interactive axis configuration

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "EnhancedLissajousEngine.h"
#include "ProjectManager.h"
#include <memory>

//==============================================================================
/**
    Enhanced Lissajous Component - Multi-dimensional curve visualization
    
    Features:
    - Support for 2D to 6D Lissajous curves (user configurable)
    - Real-time parameter adjustment for each axis
    - Multiple visualization modes and projections
    - Preset curves and analysis tools
    - Color-coded multi-axis display
*/
class EnhancedLissajousComponent : public Component,
                                 public EnhancedLissajousEngine::Listener,
                                 public Button::Listener,
                                 public Slider::Listener,
                                 public Timer
{
public:
    EnhancedLissajousComponent(ProjectManager* pm);
    ~EnhancedLissajousComponent() override;
    
    //==============================================================================
    // Component overrides
    void paint(Graphics& g) override;
    void resized() override;
    void timerCallback() override;
    
    //==============================================================================
    // Mouse interaction for 3D rotation
    void mouseDown(const MouseEvent& event) override;
    void mouseDrag(const MouseEvent& event) override;
    void mouseWheelMove(const MouseEvent& event, const MouseWheelDetails& wheel) override;
    
    //==============================================================================
    // Enhanced Lissajous Engine callbacks
    void curveUpdated(const EnhancedLissajousEngine& engine) override;
    void analysisUpdated(const EnhancedLissajousEngine::CurveAnalysis& analysis) override;
    
    //==============================================================================
    // UI Controls
    void buttonClicked(Button* button) override;
    void sliderValueChanged(Slider* slider) override;
    
    //==============================================================================
    // Visualization modes
    enum ViewMode
    {
        VIEW_2D_XY,         // 2D projection X-Y
        VIEW_2D_XZ,         // 2D projection X-Z
        VIEW_2D_YZ,         // 2D projection Y-Z
        VIEW_3D,            // 3D perspective view
        VIEW_ORTHOGRAPHIC,  // Orthographic 3D view
        VIEW_MULTI_PANEL    // Multiple 2D projections
    };
    
    void setViewMode(ViewMode mode) { viewMode = mode; repaint(); }
    ViewMode getViewMode() const { return viewMode; }
    
private:
    //==============================================================================
    // Core components
    ProjectManager* projectManager;
    std::unique_ptr<EnhancedLissajousEngine> lissajousEngine;
    
    //==============================================================================
    // Visualization
    ViewMode viewMode = VIEW_3D;
    
    // 3D rotation parameters
    float rotationX = 0.0f;
    float rotationY = 0.0f;
    float rotationZ = 0.0f;
    float zoom = 1.0f;
    Point<float> panOffset{0, 0};
    
    // Mouse interaction
    Point<int> lastMousePos;
    bool isDragging = false;
    
    //==============================================================================
    // UI Controls - Axis Configuration
    struct AxisControls
    {
        std::unique_ptr<Slider> frequencySlider;
        std::unique_ptr<Slider> amplitudeSlider;
        std::unique_ptr<Slider> phaseSlider;
        std::unique_ptr<ComboBox> waveShapeCombo;
        std::unique_ptr<ToggleButton> activeToggle;
        std::unique_ptr<Label> axisLabel;
        
        void setupControls(const String& axisName, Component* parent, Slider::Listener* listener, Button::Listener* buttonListener);
    };
    
    std::array<AxisControls, MAX_LISSAJOUS_AXES> axisControls;
    
    //==============================================================================
    // UI Controls - Global
    std::unique_ptr<Slider> numAxesSlider;
    std::unique_ptr<ComboBox> presetCombo;
    std::unique_ptr<ComboBox> viewModeCombo;
    std::unique_ptr<ComboBox> colorSchemeCombo;
    std::unique_ptr<ToggleButton> animationToggle;
    std::unique_ptr<Slider> animationSpeedSlider;
    std::unique_ptr<Slider> resolutionSlider;
    std::unique_ptr<Slider> timeSpanSlider;
    
    // Analysis display
    std::unique_ptr<Label> analysisLabel;
    
    //==============================================================================
    // Layout areas
    Rectangle<int> curveArea;
    Rectangle<int> controlsArea;
    Rectangle<int> axisControlsArea;
    Rectangle<int> globalControlsArea;
    Rectangle<int> analysisArea;
    
    //==============================================================================
    // Cached curve data for rendering
    std::vector<Point3D> currentCurve3D;
    std::vector<Point<float>> currentCurve2D;
    EnhancedLissajousEngine::CurveAnalysis currentAnalysis;
    mutable SpinLock curveLock;
    
    //==============================================================================
    // Rendering methods
    void drawCurve2D(Graphics& g);
    void drawCurve3D(Graphics& g);
    void drawMultiPanel(Graphics& g);
    void drawAnalysisInfo(Graphics& g);
    
    void drawAxes(Graphics& g);
    void drawGrid(Graphics& g);
    
    //==============================================================================
    // 3D transformation utilities
    struct Matrix3D
    {
        float m[3][3];
        
        Matrix3D() { identity(); }
        void identity();
        void rotateX(float angle);
        void rotateY(float angle);
        void rotateZ(float angle);
        Point3D transform(const Point3D& point) const;
    };
    
    Point<float> project3DTo2D(const Point3D& point3D, const Rectangle<float>& bounds) const;
    
    //==============================================================================
    // UI Setup and layout
    void setupControls();
    void layoutControls();
    void updateAxisControls();
    void updatePresetCombo();
    void updateAnalysisDisplay();
    
    //==============================================================================
    // Preset handling
    void loadPreset(int presetIndex);
    String getPresetName(int presetIndex) const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EnhancedLissajousComponent)
};