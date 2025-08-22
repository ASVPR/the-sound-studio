/*
  ==============================================================================

    WaveTableOscViewComponent.h
    Created: 13 Mar 2020 11:29:03pm
    Author:  Gary Jones

  ==============================================================================
*/

#pragma once


#include <JuceHeader.h>
#include "ProjectManager.h"
#include "CustomLookAndFeel.h"

/*
 
3. save table file
4. load tablefile & preset name label
 
 */

class WaveTableEditControl : public Component
{
public:
    
    WaveTableEditControl();
    ~WaveTableEditControl() override;
    
    void paint (Graphics&g) override;
    void drawPoints(Graphics &g);
    void drawCircle(Graphics&g, Point<float> pos);
    void resized() override;
    
    enum EDIT_MODE {
        SELECT = 0, PEN, LINE, ERASE
    } editMode;
    
    juce::Point<int> drawLineStartPoint;
    juce::Point<int> drawLineEndPoint;
    
    juce::Point<int> lastDragPoint;
    
    void mouseDown (const MouseEvent& event) override;
    void mouseDrag (const MouseEvent& event) override;
    void mouseUp (const MouseEvent& event) override;
    void mouseDoubleClick (const MouseEvent& event) override;
    
    void processPenDrag(const MouseEvent &event);
    void processLineDrawing();
    void interpolate();
    int convertXToIndex(int x);
    void loadWaveTable(double * waveData);
    double * getWavetable() { return waveTablePoints; }
    void setEditMode(EDIT_MODE mode);
    
private:
    bool isInEditMode;
    double waveTablePoints[2048];
    float viewWidth;
    float viewHeight;
    float magnifierX;
    float magnifierY;
    
    CriticalSection pathCreationLock;
    
    Path p;
};

class WaveTableOscViewComponent : public Component, public Button::Listener, public ComboBox::Listener, public ProjectManager::UIListener
{
public:
    WaveTableOscViewComponent(ProjectManager * pm, AUDIO_MODE m, int ref);

    ~WaveTableOscViewComponent() override
    {

    }
    
    void paint (Graphics&) override;
    void resized() override;
    void buttonClicked (Button* button) override;
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged) override;
    
    void updateSettingsUIParameter(int settingIndex) override;
    
    float scaleFactor = 0.5;
    void setScale(float factor);
    
    void setShortcut(int shortcut)
    {
        shortcutRef = shortcut;
        
        waveTableEditControl->loadWaveTable(projectManager->getWavetableBufferForCurrentEditor(mode, shortcutRef));
    }
    
private:

    CustomLookAndFeel lookAndFeel;
    
    ProjectManager * projectManager;
    AUDIO_MODE mode;
    int shortcutRef;
    
    std::unique_ptr<ComboBox> comboBox_WaveType; // enum for sin, saw, square and custom.
    
    std::unique_ptr<TextButton> buttonDrawLine;
    std::unique_ptr<TextButton>  buttonDrawPen;
//    TextButton * buttonEraser;
    std::unique_ptr<TextButton>  buttonRenderWavetable;
    std::unique_ptr<TextButton>  buttonLoadWavetableFile;
    std::unique_ptr<TextButton>  buttonSaveWavetableFile;
    
    std::unique_ptr<WaveTableEditControl> waveTableEditControl;
};
