/*
  ==============================================================================

    CustomChordComponent.h
    Created: 16 Mar 2019 7:53:22pm
    Author:  Gary Jones

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "ProjectManager.h"
#include "CustomLookAndFeel.h"
#include <memory>

//==============================================================================
/*
*/

// square component, comes in 2 variants,
// settings buttons,
//
class CustomChordNoteComponent    : public Component, public Button::Listener, public::ComboBox::Listener
{
public:
    CustomChordNoteComponent(int ref, ProjectManager * pm);
    
    ~CustomChordNoteComponent();

    void paint (Graphics&) override;
    void resized() override;
    void buttonClicked (Button*button)override;
    
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged)override;
    void syncGUI();
    void setShortcutRef(int s);
    
    float scaleFactor = 0.5;
    void setScale(float factor)
    {
        scaleFactor = factor;
        
        label_NoteValue->setFont(28 * scaleFactor);
        label_OctaveValue->setFont(28 * scaleFactor);
        label_FrequencyLabel->setFont(28 * scaleFactor);
    }

private:
    ProjectManager * projectManager;
    
    int shortcutRef     = 0;
    int noteRef         = 0;
    
    int chosenNote      = 0;
    int chosenOctave    = 0;
    
    int activeState     = 0; // 0=not active, 1=addnote/octave, 2= details+delete
    
    void setState(int isActive);
    
    std::unique_ptr<ImageComponent> backgroundImageComp;
    
    std::unique_ptr<Component> containerView_Inactive;
    std::unique_ptr<Component> containerView_Active;
    std::unique_ptr<Component> containerView_Details;
    
    std::unique_ptr<ImageButton> button_Delete;
    std::unique_ptr<ImageButton> button_DeleteFirst;
    std::unique_ptr<ImageButton> button_Settings;
    std::unique_ptr<ImageButton> button_AddNewNote;
    std::unique_ptr<ImageButton> button_AddActive;
    
    std::unique_ptr<ComboBox> comboBox_Note;
    std::unique_ptr<ComboBox> comboBox_Octave;
    
    std::unique_ptr<Label> label_NoteValue;
    std::unique_ptr<Label> label_OctaveValue;
    std::unique_ptr<Label> label_FrequencyLabel;
    
    Image imageBackgroundActive;
    Image imageBackgroundInactive;
    Image imageDetailsBackground;
    Image imageSettingsBackground;
    Image imageDeleteButton;
    Image imageAddNote;
    Image imageAddIcon;
    Image imageSettingsIcon;
    Image imageDelete; // delete, not close i think !!!!
    
    
    // UI Layout Variables
    int insetDelete         = 16;
    int deleteTopMargin     = 21;
    int deleteSize          = 35;
    
    int backgroundTopMargin = 38;
    int backgroundLeftMargin= insetDelete;
    int backgroundWidth     = 292;
    int backgroundHeight    = 260;
    
    int settingsLeftMargin  = 16 + backgroundLeftMargin;
    int settingsTopMargin   = 223 + backgroundTopMargin;
    int settingsSize        = 32;
    
    int noteBoxInset            = 17;
    int noteBoxWidth            = 292+34;
    int noteBoxHeight           = 260+34;
    int noteBoxSpace            = 41-noteBoxInset;
    int containerWidth          = (noteBoxWidth + noteBoxSpace) * 12;\
    
    int mainWidth = 290;
    int mainHeight = 259;
    
    CustomLookAndFeel lookAndFeel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomChordNoteComponent)
};

// viewport et al
class CustomChordContainerComponent    : public Component, public Button::Listener
{
public:
    CustomChordContainerComponent(ProjectManager * pm)
    {
        projectManager = pm;
        shortcutRef = 0;
        // images
        imageLeftButton     = ImageCache::getFromMemory(BinaryData::ShortcutLeft2x_png, BinaryData::ShortcutLeft2x_pngSize);
        imageRightButton    = ImageCache::getFromMemory(BinaryData::ShortcutRight2x_png, BinaryData::ShortcutRight2x_pngSize);
        
        // buttons
        button_Left = new ImageButton();
        button_Left->setTriggeredOnMouseDown(true);
        button_Left->setImages (false, true, true,
                                imageLeftButton, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageLeftButton, 0.75, Colour (0x00000000));
        button_Left->addListener(this);
        button_Left->setBounds(leftButtonLeftMargin, leftButtonTopMargin, buttonWidth, buttonHeight);
        addAndMakeVisible(button_Left);
        
        button_Right = new ImageButton();
        button_Right->setTriggeredOnMouseDown(true);
        button_Right->setImages (false, true, true,
                                 imageRightButton, 0.999f, Colour (0x00000000),
                                 Image(), 1.000f, Colour (0x00000000),
                                 imageRightButton, 0.75, Colour (0x00000000));
        button_Right->addListener(this);
        button_Right->setBounds(rightButtonLeftMargin, rightButtonTopMargin, buttonWidth, buttonHeight);
        addAndMakeVisible(button_Right);
        
        shortcutsContainer = new Component();
        shortcutsContainer->setBounds(0, 0, containerWidth, 344);
        
        // shortcuts array
        
        for (int i = 0; i < 12; i++)
        {
            noteComponent[i] = new CustomChordNoteComponent(i, projectManager);
            noteComponent[i]->setBounds((noteBoxWidth + noteBoxSpace) * i, 0, noteBoxWidth, noteBoxHeight);
            shortcutsContainer->addAndMakeVisible(noteComponent[i]);
            // other shortcut stuff if required !
        }
        
        // viewport
        viewport = new Viewport();
        viewport->setBounds(viewportLeftMargin, 0, viewportWidth, viewportHeight);
        
        viewport            ->setViewedComponent(shortcutsContainer);
        viewport            ->setScrollBarsShown(false, false, false, true);
        addAndMakeVisible(viewport);
    }
    
    ~CustomChordContainerComponent()
    {
        
    }
    
    void buttonClicked (Button*button) override
    {
        if (button == button_Left)
        {
            shiftToRight--; if(shiftToRight<=0)shiftToRight=0;
            moveViewport();
        }
        else if (button == button_Right)
        {
            shiftToRight++; if(shiftToRight>=5)shiftToRight=5;
            moveViewport();
        }
    }
    
    void moveViewport()
    {
//        int x = (shortcutWidth + shortcutSpace) * shiftToRight;
//        viewport->setViewPosition(x, 0);
    }
    
    void setShortcutRef(int s)
    {
        shortcutRef = s;
        
        for (int i = 0; i < 12; i++) {
            noteComponent[i]->setShortcutRef(shortcutRef);
        }
    }
    
    void syncGUI()
    {
        for (int i = 0; i < 12; i++)
        {
            noteComponent[i]->syncGUI();
        }
    }
    
    float scaleFactor = 0.5;
    void setScale(float factor)
    {
        scaleFactor = factor;
        
        for (int i = 0; i < 12; i++)
        {
            noteComponent[i]->setScale(scaleFactor);
        }
    }
    
    void resized() override
    {
        button_Left->setBounds(leftButtonLeftMargin * scaleFactor, leftButtonTopMargin * scaleFactor, buttonWidth * scaleFactor, buttonHeight * scaleFactor);
        button_Right->setBounds(rightButtonLeftMargin * scaleFactor, rightButtonTopMargin * scaleFactor, buttonWidth * scaleFactor, buttonHeight * scaleFactor);
        shortcutsContainer->setBounds(0, 0, containerWidth * scaleFactor, 344 * scaleFactor);
        
        for (int i = 0; i < 12; i++)
        {
            noteComponent[i]->setBounds((noteBoxWidth + noteBoxSpace)  * scaleFactor * i, 0, noteBoxWidth * scaleFactor, noteBoxHeight * scaleFactor);
        }
        
        viewport->setBounds(viewportLeftMargin * scaleFactor, 0, viewportWidth * scaleFactor, viewportHeight * scaleFactor);
    }
    
public:
    std::unique_ptr<CustomChordNoteComponent>noteComponent[12];
    
private:
    int shortcutRef = 0;
    ProjectManager * projectManager;
    int shiftToRight = 0; // number of moves right into list
    std::unique_ptr<ImageButton>button_Left;
    std::unique_ptr<ImageButton>button_Right;
    
    std::unique_ptr<Viewport>viewport;
    
    std::unique_ptr<Component>shortcutsContainer;
    
    Image imageLeftButton;
    Image imageRightButton;
    
    // UI Variables
    int leftButtonLeftMargin    = 48;
    int leftButtonTopMargin     = 126;
    
    int rightButtonLeftMargin   = 1440; //1468;
    int rightButtonTopMargin    = leftButtonTopMargin;
    
    int buttonWidth             = 27;
    int buttonHeight            = 47;
    
    int noteBoxInset            = 17;
    int noteBoxWidth            = 292+34;
    int noteBoxHeight           = 280;
    int noteBoxSpace            = 20-noteBoxInset;
    int containerWidth          = (noteBoxWidth + noteBoxSpace) * 12;
    
    
    int viewportLeftMargin      = 107 - 10;
    int viewportTopMargin       = 0;
    int viewportHeight          = 320;
    int viewportWidth           = (noteBoxWidth + noteBoxSpace) * 4;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomChordContainerComponent)
};


// main popup with title etc
// needs be be full size,
// needs black opacity background in paint method..

class CustomChordPopupComponent : public Component, public Button::Listener
{
public:
    CustomChordPopupComponent(ProjectManager * pm)
    {
        projectManager = pm;
        
        imageBackground = ImageCache::getFromMemory(BinaryData::AddCustomChordBackground_png, BinaryData::AddCustomChordBackground_pngSize);
        
        imageAddChordButton = ImageCache::getFromMemory(BinaryData::AddChordButton_png, BinaryData::AddChordButton_pngSize);
        
        imageCloseButton = ImageCache::getFromMemory(BinaryData::CloseButton2x_png, BinaryData::CloseButton2x_pngSize);
        
        // nackground()
        backgroundComp = new ImageComponent();
        backgroundComp->setImage(imageBackground);
        backgroundComp->setBounds(42, 366, 1499, 639);
        addAndMakeVisible(backgroundComp);
        
        button_Close = new ImageButton();
        button_Close->setTriggeredOnMouseDown(true);
        button_Close->setImages (false, true, true,
                                 imageCloseButton, 0.999f, Colour (0x00000000),
                                 Image(), 1.000f, Colour (0x00000000),
                                 imageCloseButton, 0.75, Colour (0x00000000));
        button_Close->addListener(this);
        button_Close->setBounds(1470, 405, 28, 28);
        addAndMakeVisible(button_Close);
        
        button_AddChord = new ImageButton();
        button_AddChord->setTriggeredOnMouseDown(true);
        button_AddChord->setImages (false, true, true,
                                 imageAddChordButton, 0.999f, Colour (0x00000000),
                                 Image(), 1.000f, Colour (0x00000000),
                                 imageAddChordButton, 0.75, Colour (0x00000000));
        button_AddChord->addListener(this);
        button_AddChord->setBounds(627, 866, 306, 75);
        addAndMakeVisible(button_AddChord);
        
        customChordContainerComponent = new CustomChordContainerComponent(projectManager);
        addAndMakeVisible(customChordContainerComponent);
        customChordContainerComponent->setBounds(42, 536, 1475, 320); //
    }
    
    ~CustomChordPopupComponent()
    {
        
    }
    
    void paint (Graphics&g) override
    {
        // add transparent black background fill
        g.setColour(Colours::black);
        g.setOpacity(0.88);
        g.fillAll();
        
    }
    void resized() override
    {
        backgroundComp                  ->setBounds(42 * scaleFactor, 366 * scaleFactor, 1499 * scaleFactor, 639 * scaleFactor);
        button_Close                    ->setBounds(1470 * scaleFactor, 405 * scaleFactor, 28 * scaleFactor, 28 * scaleFactor);
        button_AddChord                 ->setBounds(627 * scaleFactor, 866 * scaleFactor, 306 * scaleFactor, 75 * scaleFactor);
        customChordContainerComponent   ->setBounds(42 * scaleFactor, 536 * scaleFactor, 1475 * scaleFactor, 320 * scaleFactor);
    }
    
    void buttonClicked (Button*button) override
    {
        if (button == button_Close)
        {
            projectManager->setChordPlayerParameter(shortcutRef, CUSTOM_CHORD, false);
            syncGUI();
            closeView();
        }
        else if (button == button_AddChord)
        {
            projectManager->setChordPlayerParameter(shortcutRef, CUSTOM_CHORD, true);
            closeView();
        }
    }
    
    void openView(int shortcut)
    {
        
        shortcutRef = shortcut;
        // sync parameters
        customChordContainerComponent->setShortcutRef(shortcutRef);
        
        setVisible(true);
    }
    void closeView()
    {
        // sync parameters
        setVisible(false);
    }
    
    void syncGUI()
    {
        customChordContainerComponent->syncGUI();
    }
    
    float scaleFactor = 0.f;
    void setScale(float factor)
    {
        scaleFactor = factor;
        customChordContainerComponent->setScale(scaleFactor);
        
    }
    
    
private:
    ProjectManager * projectManager;
    int shortcutRef;
    
    std::unique_ptr<CustomChordContainerComponent> customChordContainerComponent;
    std::unique_ptr<ImageButton> button_Close;
    std::unique_ptr<ImageButton> button_AddChord;
    
    Image imageAddChordButton;
    Image imageBackground;
    Image imageCloseButton;
    
    // ui variables
    ImageComponent * backgroundComp;
    
    
};
