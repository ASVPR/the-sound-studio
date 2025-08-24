/*
  ==============================================================================

    LissajousCurveAddChordComponent.h
    Created: 29 Sep 2019 10:38:46am
    Author:  Gary Jones

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "ProjectManager.h"
#include "CustomLookAndFeel.h"
#include "SpectrogramComponent.h"
#include "CustomRotarySlider.h"
#include "WaveTableOscViewComponent.h"
#include "PopupFFTWindow.h"
#include <memory>



//==============================================================================
/*
*/

class LissajousCustomChordNoteComponent    : public Component, public Button::Listener, public::ComboBox::Listener
{
public:
    LissajousCustomChordNoteComponent(int ref, ProjectManager * pm, int axis);
    
    ~LissajousCustomChordNoteComponent();

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
        lookAndFeel.setScale(scaleFactor);
    }
    

private:
    ProjectManager * projectManager;
    
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
    
    int paramIndexBase;
    
    int shortcutRef;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LissajousCustomChordNoteComponent)
};

// viewport et al
class LissajousCustomChordContainerComponent    : public Component, public Button::Listener
{
public:
    LissajousCustomChordContainerComponent(ProjectManager * pm, int axis)
    {
        projectManager  = pm;
        shortcutRef     = axis;
        // images
        imageLeftButton     = ImageCache::getFromMemory(BinaryData::ShortcutLeft2x_png, BinaryData::ShortcutLeft2x_pngSize);
        imageRightButton    = ImageCache::getFromMemory(BinaryData::ShortcutRight2x_png, BinaryData::ShortcutRight2x_pngSize);
        
        // buttons
        button_Left = std::make_unique<ImageButton>();
        button_Left->setTriggeredOnMouseDown(true);
        button_Left->setImages (false, true, true,
                                imageLeftButton, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageLeftButton, 0.75, Colour (0x00000000));
        button_Left->addListener(this);
        button_Left->setBounds(leftButtonLeftMargin, leftButtonTopMargin, buttonWidth, buttonHeight);
        addAndMakeVisible(button_Left.get());
        
        button_Right = std::make_unique<ImageButton>();
        button_Right->setTriggeredOnMouseDown(true);
        button_Right->setImages (false, true, true,
                                 imageRightButton, 0.999f, Colour (0x00000000),
                                 Image(), 1.000f, Colour (0x00000000),
                                 imageRightButton, 0.75, Colour (0x00000000));
        button_Right->addListener(this);
        button_Right->setBounds(rightButtonLeftMargin, rightButtonTopMargin, buttonWidth, buttonHeight);
        addAndMakeVisible(button_Right.get());
        
        shortcutsContainer = std::make_unique<Component>();
        shortcutsContainer->setBounds(0, 0, containerWidth, 344);
        
        // shortcuts array
        
        for (int i = 0; i < 12; i++)
        {
            noteComponent[i] = std::make_unique<LissajousCustomChordNoteComponent>(i, projectManager, shortcutRef);
            noteComponent[i]->setBounds((noteBoxWidth + noteBoxSpace) * i, 0, noteBoxWidth, noteBoxHeight);
            shortcutsContainer->addAndMakeVisible(noteComponent[i].get());
            // other shortcut stuff if required !
        }
        
        // viewport
        viewport = std::make_unique<Viewport>();
        viewport->setBounds(viewportLeftMargin, 0, viewportWidth, viewportHeight);
        
        viewport            ->setViewedComponent(shortcutsContainer.get());
        viewport            ->setScrollBarsShown(false, false, false, true);
        addAndMakeVisible(viewport.get());
    }
    
    ~LissajousCustomChordContainerComponent()
    {
        
    }
    
    void buttonClicked (Button*button) override
    {
        if (button == button_Left.get())
        {
            shiftToRight--; if(shiftToRight<=0)shiftToRight=0;
            moveViewport();
        }
        else if (button == button_Right.get())
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
        
        for (int i = 0; i < 12 ; i++)
        {
            noteComponent[i]->setScale(scaleFactor);
        }
    }
    
    
public: std::unique_ptr<LissajousCustomChordNoteComponent>noteComponent[12];
    
private:
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
    
    int shortcutRef; // == axis
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LissajousCustomChordContainerComponent)
};


// main popup with title etc
// needs be be full size,
// needs black opacity background in paint method..

class LissajousCustomChordPopupComponent : public Component, public Button::Listener
{
public:
    LissajousCustomChordPopupComponent(ProjectManager * pm, int axis)
    {
        shortcutRef = axis;
        projectManager = pm;
        
        imageBackground = ImageCache::getFromMemory(BinaryData::AddCustomChordBackground_png, BinaryData::AddCustomChordBackground_pngSize);
        
        imageAddChordButton = ImageCache::getFromMemory(BinaryData::AddChordButton_png, BinaryData::AddChordButton_pngSize);
        
        imageCloseButton = ImageCache::getFromMemory(BinaryData::CloseButton2x_png, BinaryData::CloseButton2x_pngSize);
        
        // background()
        backgroundComp = std::make_unique<ImageComponent>();
        backgroundComp->setImage(imageBackground);
        backgroundComp->setBounds(42, 366, 1499, 639);
        addAndMakeVisible(backgroundComp.get());
        
        button_Close = std::make_unique<ImageButton>();
        button_Close->setTriggeredOnMouseDown(true);
        button_Close->setImages (false, true, true,
                                 imageCloseButton, 0.999f, Colour (0x00000000),
                                 Image(), 1.000f, Colour (0x00000000),
                                 imageCloseButton, 0.75, Colour (0x00000000));
        button_Close->addListener(this);
        button_Close->setBounds(1470, 405, 28, 28);
        addAndMakeVisible(button_Close.get());
        
        button_AddChord = std::make_unique<ImageButton>();
        button_AddChord->setTriggeredOnMouseDown(true);
        button_AddChord->setImages (false, true, true,
                                 imageAddChordButton, 0.999f, Colour (0x00000000),
                                 Image(), 1.000f, Colour (0x00000000),
                                 imageAddChordButton, 0.75, Colour (0x00000000));
        button_AddChord->addListener(this);
        button_AddChord->setBounds(627, 866, 306, 75);
        addAndMakeVisible(button_AddChord.get());
        
        customChordContainerComponent = std::make_unique<LissajousCustomChordContainerComponent>(projectManager, shortcutRef);
        addAndMakeVisible(customChordContainerComponent.get());
        customChordContainerComponent->setBounds(42, 536, 1475, 320); //
    }
    
    ~LissajousCustomChordPopupComponent()
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
        if (button == button_Close.get())
        {
            projectManager->setLissajousParameter( CUSTOM_CHORD, false);
            closeView();
        }
        else if (button == button_AddChord.get())
        {
            projectManager->setLissajousParameter( CUSTOM_CHORD, true);
            closeView();
        }
    }
    
    void openView()
    {
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
    
    float scaleFactor = 0.5;
    void setScale(float factor)
    {
        scaleFactor = factor;
        
        customChordContainerComponent->setScale(scaleFactor);
        
        lookAndFeel.setScale(scaleFactor);
    }
    
    
    
private:
    ProjectManager * projectManager;
    
    std::unique_ptr<LissajousCustomChordContainerComponent> customChordContainerComponent;
    std::unique_ptr<ImageButton> button_Close;
    std::unique_ptr<ImageButton> button_AddChord;
    
    Image imageAddChordButton;
    Image imageBackground;
    Image imageCloseButton;
    
    std::unique_ptr<ImageComponent> backgroundComp;
    
    // ui variables
    
    CustomLookAndFeel lookAndFeel;
    
    int paramIndexBase;
    
    int shortcutRef;
    
    
};

class LissajousAddOnPopupComponent : public Component, public Button::Listener
{
public:

    LissajousAddOnPopupComponent(ProjectManager * pm, int axisRef);
    ~LissajousAddOnPopupComponent();
    
    void paint (Graphics&g) override;
    void resized() override;
    void buttonClicked (Button*button)override;
    void setShortcutRef(int newShortcutRef);
    void syncGUI();
    
    float scaleFactor = 0.5;
    void setScale(float factor)
    {
        scaleFactor = factor;
    }
    
    
private:
    ProjectManager * projectManager;
    Image backgroundImage;
    Image imageBlueCheckButtonNormal;
    Image imageBlueCheckButtonSelected;
    std::unique_ptr<ImageButton> addonButton[NUM_ADDONS];
    std::unique_ptr<Label> label[NUM_ADDONS];
    
    CustomLookAndFeel lookAndFeel;
    
    int paramIndexBase;
    
    int shortcutRef;
};


class LissajousChordPlayerSettingsComponent    : public Component, public Button::Listener, public Slider::Listener, public ComboBox::Listener, public TextEditor::Listener, public ProjectManager::UIListener
{
public:
    LissajousChordPlayerSettingsComponent(ProjectManager * pm, int axisRef);
    ~LissajousChordPlayerSettingsComponent();

    void paint (Graphics&g) override;
    void resized() override;
    
    void buttonClicked (Button*button)override;
    void sliderValueChanged (Slider* slider)override;
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged)override;
    void textEditorReturnKeyPressed (TextEditor&editor)override;
    
    void openView();
    void closeView();
    
    void handleWaveformToggle() {}
    void handleChordOriginToggle() {}
    void handleManipulateFrequency() {}
    
    void syncUI(int paramIndex);

    void updateMinMaxSettings(int paramIndex) override;
    
    void updateScalesComponents()
    {
        ScalesManager * sm = projectManager->frequencyManager->scalesManager;
        
        sm->getComboBoxPopupMenuForChords(*comboBoxChordtype->getRootMenu(), (SCALES_UNIT)shortcutRef);
        sm->getComboBoxPopupMenuForKeynotes(*comboBoxKeynote->getRootMenu(), (SCALES_UNIT)shortcutRef);
        
        int paramIndex =  UNIT_1_CHORD_PLAYER_CHORDPLAYER_SCALE + ((UNIT_1_CHORD_PLAYER_CHORDPLAYER_SCALE - UNIT_1_CHORD_PLAYER_SHORTCUT_IS_ACTIVE) * shortcutRef);
                                                                   
        comboBox_Scales->setSelectedId(projectManager->getLissajousParameter(paramIndex));
        
        // update for custom chord
        
        customChordPopupComponent->syncGUI();
    }
    
    float scaleFactor = 0.5;
    void setScale(float factor)
    {
        scaleFactor = factor;
        
        customChordPopupComponent   ->setScale(scaleFactor);
        addOnPopupComponent         ->setScale(scaleFactor);
        
        lookAndFeel.setScale(scaleFactor);
    }
    
    
private:
    
    ProjectManager * projectManager;
    bool isSynced;
    int shortcutRef;
    
    std::unique_ptr<LissajousCustomChordPopupComponent> customChordPopupComponent;
    std::unique_ptr<LissajousAddOnPopupComponent> addOnPopupComponent;

    std::unique_ptr<ImageButton> button_Close;
    
    // toggle between these two
    std::unique_ptr<ImageButton> button_ChooseChordFromList;
    std::unique_ptr<ImageButton> button_ChooseChordFromFrequency;

    std::unique_ptr<CustomRotarySlider> slider_Amplitude;
    std::unique_ptr<CustomRotarySlider> slider_Attack;
    std::unique_ptr<CustomRotarySlider> slider_Sustain;
    std::unique_ptr<CustomRotarySlider> slider_Decay;
    std::unique_ptr<CustomRotarySlider> slider_Release;
    
    // act as toggle between 5 buttons
    std::unique_ptr<ImageButton> button_Default;      // sampler
    std::unique_ptr<ImageButton> button_Sine;         // synth
    std::unique_ptr<ImageButton> button_Triangle;
    std::unique_ptr<ImageButton> button_Square;
    std::unique_ptr<ImageButton> button_Sawtooth;
    std::unique_ptr<ImageButton> button_Wavetable;    // wavetable
    
    std::unique_ptr<TextButton> button_AddCustomChord; // opens popup..
    std::unique_ptr<TextButton> button_Addons; // opens popup..
    
    std::unique_ptr<ImageButton> button_Add;
    
    std::unique_ptr<TextButton>   button_WavetableEditor;
    
    std::unique_ptr<WaveTableOscViewComponent> wavetableEditorComponent;
    std::unique_ptr<PopupFFTWindow> popupWavetableWindow;
    
    // Combobox
    std::unique_ptr<ComboBox> comboBoxKeynote;
    std::unique_ptr<ComboBox> comboBoxChordtype;
    
    std::unique_ptr<TextEditor> textEditor_Octave;
    
    std::unique_ptr<ComboBox> comboBoxPlayingInstrument;
    std::unique_ptr<ComboBox> comboBox_Scales;
    
    // Text Entry for Repeater
    std::unique_ptr<TextEditor> textEditorRepeat;
    std::unique_ptr<TextEditor> textEditorPause;
    std::unique_ptr<TextEditor> textEditorLength;
    
    std::unique_ptr<ImageButton> button_ManipulateFreq;
    
    // toggle between these two
    std::unique_ptr<ImageButton> button_Multiplication;
    std::unique_ptr<ImageButton> button_Division;
    
    // TextEntryBoxes
    std::unique_ptr<TextEditor> textEditorInsertFreq;
    std::unique_ptr<TextEditor> textEditorMultiplication;
    std::unique_ptr<TextEditor> textEditorDivision;
    
    std::unique_ptr<ImageButton> button_Panic;
    Image imagePanicButton;
    
    // Labels
    Label labelInsertFrequency;
    Label labelClosestChord;
    Label labelEstimatedOctave;
    
    Label labelClosestChordValue;
    Label labelEstimatedOctaveValue;
    
    // Images
    Image mainBackgroundImage;
    Image sectionTopLeftBackground;
    Image section;
    
    Image imageCloseButton;
    Image imageBlueButtonNormal;
    Image imageBlueButtonSelected;
    Image imageAddButton;
    Image imageBlueCheckButtonNormal;
    Image imageBlueCheckButtonSelected;
    
    Image imageSliderKnob;
    Image imageSliderBackground;
    
    // LookAndFeel
    CustomLookAndFeel  lookAndFeel;
    
    Array<String> instrumentFilePath;
    String getInstrumentFilePath(int selectedID)
    {
        return instrumentFilePath[selectedID];
    }
    
    int paramIndexBase;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LissajousChordPlayerSettingsComponent)
};


class LissajousFrequencyPlayerSettingsComponent    : public Component, public Button::Listener, public TextEditor::Listener, public ProjectManager::UIListener, public Slider::Listener
{
public:
    LissajousFrequencyPlayerSettingsComponent(ProjectManager * pm,  int axisRef);
    ~LissajousFrequencyPlayerSettingsComponent();

    void paint (Graphics&g) override;
    void resized() override;
    
    void buttonClicked (Button*button)override;
    void textEditorReturnKeyPressed (TextEditor&editor)override;
    void sliderValueChanged (Slider* slider) override;
    
    void openView(int shortcut);
    void closeView();
    
    void handleWaveformToggle() {}
    void handleChordOriginToggle() {}
    void handleManipulateFrequency() {}
    
    void syncUI(int paramIndex);
    
    void updateMinMaxSettings(int paramIndex) override;
    
    float scaleFactor = 0.5;
    void setScale(float factor)
    {
        scaleFactor = factor;
        lookAndFeel.setScale(scaleFactor);
    }
    
private:
    
    ProjectManager * projectManager;
    bool isSynced;
    int shortcutRef;
    
    std::unique_ptr<ImageButton> button_Close;
    
    // toggle between these two
    std::unique_ptr<ImageButton> button_ChooseSpecificFrequency;
    std::unique_ptr<ImageButton> button_ChooseRangeOfFrequencies;
    
    // act as toggle between 5 buttons
    std::unique_ptr<ImageButton> button_Default;
    std::unique_ptr<ImageButton> button_Sine;
    std::unique_ptr<ImageButton> button_Triangle;
    std::unique_ptr<ImageButton> button_Square;
    std::unique_ptr<ImageButton> button_Sawtooth;
    std::unique_ptr<ImageButton> button_Wavetable;    // wavetable
    
    std::unique_ptr<ImageButton> button_Add;
    
    std::unique_ptr<TextButton>   button_WavetableEditor;
    std::unique_ptr<WaveTableOscViewComponent> wavetableEditorComponent;
    std::unique_ptr<PopupFFTWindow> popupWavetableWindow;
    
    // Text Entry for Repeater
    std::unique_ptr<TextEditor> textEditorRepeat;
    std::unique_ptr<TextEditor> textEditorPause;
    std::unique_ptr<TextEditor> textEditorLength;
    
    std::unique_ptr<ImageButton> button_ManipulateFreq;
    
    // new stuff
    std::unique_ptr<ImageButton> button_Log;
    
    std::unique_ptr<TextEditor> textEditorRangeLength;
    
    std::unique_ptr<CustomRotarySlider> slider_Amplitude;
    std::unique_ptr<CustomRotarySlider> slider_Attack;
    std::unique_ptr<CustomRotarySlider> slider_Sustain;
    std::unique_ptr<CustomRotarySlider> slider_Decay;
    std::unique_ptr<CustomRotarySlider> slider_Release;
    
    // toggle between these two
    std::unique_ptr<ImageButton> button_Multiplication;
    std::unique_ptr<ImageButton> button_Division;
    
    
    // TextEntryBoxes
    std::unique_ptr<TextEditor> textEditorInsertFreq;
    std::unique_ptr<TextEditor> textEditorFreqFrom;
    std::unique_ptr<TextEditor> textEditorFreqTo;
    std::unique_ptr<TextEditor> textEditorMultiplication;
    std::unique_ptr<TextEditor> textEditorDivision;

    // Images
    Image mainBackgroundImage;
    Image sectionTopLeftBackground;
    Image section;
    
    
    Image imageCloseButton;
    Image imageBlueButtonNormal;
    Image imageBlueButtonSelected;
    Image imageAddButton;
    Image imageBlueCheckButtonNormal;
    Image imageBlueCheckButtonSelected;
    
    Image imageSliderKnob;
    Image imageSliderBackground;
    
    // LookAndFeel
    CustomLookAndFeel  lookAndFeel;
    
    int paramIndexBase;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LissajousFrequencyPlayerSettingsComponent)
};
