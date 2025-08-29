/*
  ==============================================================================

    WaveTableOscViewComponent.cpp
    Created: 13 Mar 2020 11:29:03pm
    Author:  Gary Jones

  ==============================================================================
*/

#include "WaveTableOscViewComponent.h"


WaveTableEditControl::WaveTableEditControl()
{
    for (int i = 0; i < 2048; i++)
    {
        waveTablePoints[i] = 0.f;
    }
}
    
WaveTableEditControl::~WaveTableEditControl()
{
        
}
    
void WaveTableEditControl::paint (Graphics&g)
{
    // background
    g.fillAll(Colours::lightgrey);
        
    drawPoints(g);
        
    g.setColour(Colours::darkgrey);
    g.drawLine(0, getHeight() / 2.f, getWidth(), getHeight() / 2.f);
        
    //draw current waveform from array
        
    if (isInEditMode)
    {
        if (editMode == SELECT)
        {
            
        }
        else if (editMode == PEN)
        {
         
        }
        else if (editMode == LINE)
        {
            g.setColour(Colours::black);
            g.drawLine(drawLineStartPoint.x, drawLineStartPoint.y, drawLineEndPoint.x, drawLineEndPoint.y, 1.f);
        }
        else if (editMode == ERASE)
        {
            
        }
    }
}
    
void WaveTableEditControl::drawPoints(Graphics &g)
{
    p.clear();
 
    
    for(int x = 0; x < 2048; x++)
    {
        float val       = 1 - ((waveTablePoints[x] + 1.f) * 0.5);
        float h         = getHeight();
        float w         = getWidth();
        float yFloat    = h * val;
        float xFloat    = (w / 2047.f) * x;
            

        // bounds check
        
        if (yFloat > h) { yFloat = h; }
        if (yFloat < 0) { yFloat = 0; }
        
        if (xFloat < 0) { xFloat = 0; }
        if (xFloat > w) { xFloat = w; }
        
        p.lineTo(xFloat, yFloat);
    }
        
    p.lineTo(getWidth(), getHeight() / 2.f);
    p.lineTo(0.f, getHeight() / 2.f);
    p.closeSubPath();
        
    g.setColour(Colours::darkgrey);
    g.fillPath(p);
    g.setColour(Colours::black);
    g.strokePath(p, PathStrokeType(1));
}
    
void WaveTableEditControl::drawCircle(Graphics&g, Point<float> pos)
{
    Path circle;
    circle.startNewSubPath(pos);
}

void WaveTableEditControl::resized()
{
    
}
    
    
    
void WaveTableEditControl::mouseDown (const MouseEvent& event)
{
    isInEditMode = true;
    
    if (editMode == SELECT)
    {
        
    }
    else if (editMode == PEN)
    {
        drawLineStartPoint  = event.getPosition();
        lastDragPoint       = drawLineStartPoint;
    }
    else if (editMode == LINE)
    {
        drawLineStartPoint  = event.getPosition();
        lastDragPoint       = drawLineStartPoint;
    }
    else if (editMode == ERASE)
    {
        
    }
}

void WaveTableEditControl::mouseDrag (const MouseEvent& event)
{
    if (editMode == SELECT)
    {
        
    }
    else if (editMode == PEN)
    {
        if (event.getPosition().x <= getWidth())
        {
            processPenDrag(event);
        }
        
    }
    else if (editMode == LINE)
    {
        // draw line from start point to current point
        drawLineEndPoint = event.getPosition();
        
        repaint(); // in different colour
        
    }
    else if (editMode == ERASE)
    {
        // zeroes table(x)
    }
}

void WaveTableEditControl::mouseUp (const MouseEvent& event)
{
    isInEditMode = false;
    
    if (editMode == SELECT)
    {
        
    }
    else if (editMode == PEN)
    {
        if (event.getPosition().x <= getWidth())
        {
            processPenDrag(event);
        }
    }
    else if (editMode == LINE)
    {
        if (event.getPosition().x <= getWidth())
        {
            drawLineEndPoint = event.getPosition();

            processLineDrawing();
        }
        else
        {
            repaint();
        }
    }
    else if (editMode == ERASE)
    {
        
    }
}

void WaveTableEditControl::processPenDrag(const MouseEvent &event)
{
    // bounds check
    
    
    if (event.getPosition().x >= 0 && event.getPosition().x <= getWidth() && event.getPosition().y >= 0 && event.getPosition().y <= getHeight())
    {
            float h             = getHeight();
            float w             = getWidth();
            
            float yPix          = (float)event.getPosition().y;
            float yFloat        = yPix / h;
            float val           = 1 - (yFloat / 0.5);
            int xEnd            = convertXToIndex(event.getPosition().x);
            
            int lastX           = convertXToIndex(lastDragPoint.x);
            float yLastPix      = (float)lastDragPoint.y;
            float yLastFloat    = yLastPix / h;
            float lastYVal      = 1 - (yLastFloat / 0.5);

            // process interpolation between last drag point and this
            
            float yAcc          = (lastYVal - val) / (lastX - xEnd);
            

            waveTablePoints[lastX] = lastYVal;

            if (lastX > 0 && xEnd >0)
            {
                for (int x = lastX+1; x <= xEnd; x++)
                {
                    waveTablePoints[x]  = waveTablePoints[x-1] + yAcc;
                }
            }
        //    for (int x = lastX+1; x <= xEnd; x++)
        //    {
        //        waveTablePoints[x]  = waveTablePoints[x-1] + yAcc;
        //    }

            // needs to check if x is greater than or less..
            
            lastDragPoint = event.getPosition();

            repaint();
    }
    
    

}

void WaveTableEditControl::processLineDrawing()
{
    float h             = getHeight();
    float w             = getWidth();
    float yStartPix     = (float)drawLineStartPoint.y;
    float yStartFloat   = yStartPix / h;
    float yStartval     = 1 - (yStartFloat / 0.5);
    int startX          = convertXToIndex(drawLineStartPoint.x);
    
    float yEndPix       = (float)drawLineEndPoint.y;
    float yEndFloat     = yEndPix / h;
    float yEndVal       = 1 - (yEndFloat / 0.5);
    int endX            = convertXToIndex(drawLineEndPoint.x);

    float yAcc          = (yStartval - yEndVal) / (startX - endX);

    waveTablePoints[startX] = yStartval;
    
    for (int x = startX+1; x <= endX; x++)
    {
        waveTablePoints[x]  = waveTablePoints[x-1] + yAcc;
    }

    repaint();
}

int WaveTableEditControl::convertXToIndex(int x)
{
    float w         = getWidth();
    float xFloat    = (x / w) * 2047.f;
    
    int xIndex      = floor(xFloat);
    
    return xIndex;
    
}



void WaveTableEditControl::mouseDoubleClick (const MouseEvent& event)
{
    
}

void WaveTableEditControl::loadWaveTable(double * waveData)
{
    for (int i = 0; i < 2048; i++) {
        waveTablePoints[i] = waveData[i];
    }
    
    repaint();
}

void WaveTableEditControl::interpolate()
{
    
}

void WaveTableEditControl::setEditMode(EDIT_MODE mode)
{
    editMode = mode;
}


#pragma mark WaveTableOscView Component

WaveTableOscViewComponent::WaveTableOscViewComponent(ProjectManager * pm, AUDIO_MODE m, int ref)
{
    projectManager  = pm;
    mode            = m;
    shortcutRef     = ref;
    
    comboBox_WaveType = std::make_unique<ComboBox>();
    PopupMenu * waveTypeMenu =  comboBox_WaveType->getRootMenu();
    waveTypeMenu->setLookAndFeel(&lookAndFeel);
    
        // return full popup menu system for dropdown in Settings / ChordPlayer
    
    waveTypeMenu->addItem(1, "Sine");
    waveTypeMenu->addItem(2, "Triangle");
    waveTypeMenu->addItem(3, "Square");
    waveTypeMenu->addItem(4, "Saw");
    waveTypeMenu->addItem(5, "Custom");
    
    comboBox_WaveType->addListener(this);
    comboBox_WaveType->setLookAndFeel(&lookAndFeel);
    
    addAndMakeVisible(*comboBox_WaveType);

    buttonDrawLine = std::make_unique<TextButton>("");
    buttonDrawLine->setButtonText("Line");
    buttonDrawLine->setLookAndFeel(&lookAndFeel);
    buttonDrawLine->addListener(this);
    addAndMakeVisible(buttonDrawLine.get());
    
    buttonDrawPen = std::make_unique<TextButton>("");
    buttonDrawPen->setButtonText("Pen");
    buttonDrawPen->setLookAndFeel(&lookAndFeel);
    buttonDrawPen->addListener(this);
    addAndMakeVisible(buttonDrawPen.get());
    
    buttonRenderWavetable = std::make_unique<TextButton>("");
    buttonRenderWavetable->setButtonText("Render");
    buttonRenderWavetable->setLookAndFeel(&lookAndFeel);
    buttonRenderWavetable->addListener(this);
    addAndMakeVisible(buttonRenderWavetable.get());
    
    buttonLoadWavetableFile = std::make_unique<TextButton>("");
    buttonLoadWavetableFile->setButtonText("Load");
    buttonLoadWavetableFile->setLookAndFeel(&lookAndFeel);
    buttonLoadWavetableFile->addListener(this);
    addAndMakeVisible(buttonLoadWavetableFile.get());
    
    buttonSaveWavetableFile = std::make_unique<TextButton>("");
    buttonSaveWavetableFile->setButtonText("Save");
    buttonSaveWavetableFile->setLookAndFeel(&lookAndFeel);
    buttonSaveWavetableFile->addListener(this);
    addAndMakeVisible(buttonSaveWavetableFile.get());
    
    waveTableEditControl = std::make_unique<WaveTableEditControl>();
    addAndMakeVisible(waveTableEditControl.get());
    
}
void WaveTableOscViewComponent::paint (Graphics&) 
{
    
}

void WaveTableOscViewComponent::resized()
{
    int topBorder           = 40 * scaleFactor;
    int buttonWidth         = 80 * scaleFactor;
    int buttonHeight        = topBorder;
    int comboBoxWidth       = 200 * scaleFactor;
    
    waveTableEditControl        ->setBounds(0, topBorder, getWidth(), getHeight() - topBorder);
    comboBox_WaveType           ->setBounds(0, 0, comboBoxWidth, buttonHeight);
    buttonDrawLine              ->setBounds(comboBoxWidth, 0, buttonWidth, buttonHeight);
    buttonDrawPen               ->setBounds(comboBoxWidth + (buttonWidth), 0, buttonWidth, buttonHeight);
    buttonRenderWavetable       ->setBounds(comboBoxWidth + (buttonWidth * 2), 0, buttonWidth, buttonHeight);
    
    buttonLoadWavetableFile     ->setBounds(getWidth() - (buttonWidth*1), 0, buttonWidth, buttonHeight);
    buttonSaveWavetableFile     ->setBounds(getWidth() - (buttonWidth*2), 0, buttonWidth, buttonHeight);
}

void WaveTableOscViewComponent::buttonClicked (Button* button)
{
    if (button == buttonDrawPen.get())
    {
        waveTableEditControl->setEditMode(WaveTableEditControl::EDIT_MODE::PEN);
    }
    else if (button == buttonDrawLine.get())
    {
        waveTableEditControl->setEditMode(WaveTableEditControl::EDIT_MODE::LINE);
    }
    else if (button == buttonRenderWavetable.get())
    {
        projectManager->setWavetableBuffer(mode, shortcutRef, waveTableEditControl->getWavetable());
    }
    else if (button == buttonLoadWavetableFile.get())
    {
        // open directory, load into
        
        WildcardFileFilter wildcardFilter (".table", String(), "Wavetable files");
        File dir = File::getSpecialLocation (File::userDocumentsDirectory).getChildFile ("TSS/Wavetables");
        FileBrowserComponent browser (FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles,
                                      dir,
                                      nullptr,
                                      nullptr);
        
        FileChooserDialogBox dialogBox ("Open a TSS Wavetable file",
                                        "Please choose the file that you want to open...",
                                        browser,
                                        false,
                                        browser.findColour (AlertWindow::backgroundColourId));
        
        dialogBox.show();

        File newTableFile = browser.getSelectedFile(0);
        
        // check if newfile is .table file
        if (newTableFile.exists())
        {
            if (newTableFile.getFileExtension() == ".table")
            {
                printf("works");
                
                FileInputStream inputStream(newTableFile);
                
                ValueTree wavetableFileValueTree = ValueTree::readFromStream(inputStream);
                ValueTree tableTree(wavetableFileValueTree.getChildWithName("TableTree"));
                
                double tableArray[2048];
                
                for (int p = 0; p < 2048; p++)
                {
                    String key(String::formatted("%i", p));
                    tableArray[p] = tableTree.getProperty(key);
                }
                
                waveTableEditControl->loadWaveTable(tableArray);
                
                projectManager->setWavetableBuffer(mode, shortcutRef, tableArray);
            }
        }
    }
    else if (button == buttonSaveWavetableFile.get())
    {
        File dir = File::getSpecialLocation (File::userDocumentsDirectory).getChildFile ("TSS/Wavetables");
        FileBrowserComponent browser (FileBrowserComponent::saveMode | FileBrowserComponent::canSelectFiles,
                                             dir,
                                             nullptr,
                                             nullptr);
               
        FileChooserDialogBox dialogBox ("Save a TSS Wavetable file",
                                               "Please choose or name the file that you want to save",
                                               browser,
                                               false,
                                               browser.findColour (AlertWindow::backgroundColourId));
        
        dialogBox.show();
               
        
        File newTableFile = browser.getSelectedFile(0);
        
        if (newTableFile.existsAsFile()) {
            newTableFile.deleteFile();
        }
        
        File newTableFileWithExtension = newTableFile.withFileExtension(".table");

        FileOutputStream fileStream(newTableFileWithExtension);

        if (fileStream.openedOk())
        {
            double * tableArray = waveTableEditControl->getWavetable();

            ValueTree wavetableFileValueTree("maintree");
            ValueTree tableTree("TableTree");
            
            for (int p = 0; p < 2048; p++)
            {
                String key(String::formatted("%i", p));
                tableTree.setProperty(key, tableArray[p], nullptr);
            }
            
            wavetableFileValueTree.addChild(tableTree, 0, nullptr);
            
            wavetableFileValueTree.writeToStream(fileStream);
        }
    }
    
}

void WaveTableOscViewComponent::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged->getSelectedId() == 1)
    {
        projectManager->initBaseWavetable(mode, shortcutRef, 0);
    }
    else if (comboBoxThatHasChanged->getSelectedId() == 2)
    {
        projectManager->initBaseWavetable(mode, shortcutRef, 1);
    }
    else if (comboBoxThatHasChanged->getSelectedId() == 3)
    {
        projectManager->initBaseWavetable(mode, shortcutRef, 2);
    }
    else if (comboBoxThatHasChanged->getSelectedId() == 4)
    {
        projectManager->initBaseWavetable(mode, shortcutRef, 3);
    }
    else if (comboBoxThatHasChanged->getSelectedId() == 5)
    {
        projectManager->initBaseWavetable(mode, shortcutRef, 4);
    }
    
    waveTableEditControl->loadWaveTable(projectManager->getWavetableBufferForCurrentEditor(mode, shortcutRef));
}

void WaveTableOscViewComponent::setScale(float factor)
{
    scaleFactor = factor;
    lookAndFeel.setScale(scaleFactor);
}

void WaveTableOscViewComponent::updateSettingsUIParameter(int settingIndex)
{
    
}
