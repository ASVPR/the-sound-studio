/*
  ==============================================================================

    LissajousCurveAddChordComponent.cpp
    Created: 29 Sep 2019 10:38:46am
    Author:  Gary Jones

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "LissajousCurveAddChordComponent.h"
#include "LissajousCurveComponent.h"

//==============================================================================

LissajousCustomChordNoteComponent::LissajousCustomChordNoteComponent(int ref, ProjectManager * pm, int axis)
{
    projectManager  = pm;
    noteRef         = ref; // 1 of max 12..
    shortcutRef     = axis;
    
    // Font
    Typeface::Ptr AssistantLight        = Typeface::createSystemTypefaceFor(BinaryData::AssistantLight_ttf, BinaryData::AssistantLight_ttfSize);
    Typeface::Ptr AssistantSemiBold     = Typeface::createSystemTypefaceFor(BinaryData::AssistantSemiBold_ttf, BinaryData::AssistantSemiBold_ttfSize);
    Font fontSemiBold(AssistantSemiBold);
    Font fontLight(AssistantLight);
    
    fontSemiBold.setHeight(33);
    fontLight.setHeight(33);
    
    // Images
    imageSettingsIcon       = ImageCache::getFromMemory(BinaryData::settings2x_png, BinaryData::settings2x_pngSize);
    imageBackgroundActive   = ImageCache::getFromMemory(BinaryData::ChordNoteBackgroundActive_png, BinaryData::ChordNoteBackgroundActive_pngSize);
    imageBackgroundInactive   = ImageCache::getFromMemory(BinaryData::ChordNoteBackgroundInactive_png, BinaryData::ChordNoteBackgroundInactive_pngSize);
    
    imageDetailsBackground  = ImageCache::getFromMemory(BinaryData::CustomChordDetailsBackground_Max_png, BinaryData::CustomChordDetailsBackground_Max_pngSize);
    imageSettingsBackground = ImageCache::getFromMemory(BinaryData::CustomChordSettingsBackground_Max_png, BinaryData::CustomChordSettingsBackground_Max_pngSize);
    
    imageDelete             = ImageCache::getFromMemory(BinaryData::ShortcutClose2x_png, BinaryData::ShortcutClose2x_pngSize);
    imageAddIcon            = ImageCache::getFromMemory(BinaryData::ShortcutAdd2x_png, BinaryData::ShortcutAdd2x_pngSize);
    imageAddNote            = ImageCache::getFromMemory(BinaryData::AddButton2x_png, BinaryData::AddButton2x_pngSize);
    
    
    // background
    backgroundImageComp     = std::make_unique<ImageComponent>();
    backgroundImageComp     ->setImage(imageBackgroundInactive);
    backgroundImageComp     ->setBounds(noteBoxInset, noteBoxInset, backgroundWidth, backgroundHeight);
    addAndMakeVisible(backgroundImageComp.get());
    
    // containers
    containerView_Active = std::make_unique<Component>();
    containerView_Active->setBounds(0, 0, mainWidth, mainHeight);
    addAndMakeVisible(containerView_Active.get());
    
    containerView_Inactive = std::make_unique<Component>();
    containerView_Inactive->setBounds(0, 0, mainWidth, mainHeight);
    addAndMakeVisible(containerView_Inactive.get());
    
    containerView_Details = std::make_unique<Component>();
    containerView_Details->setBounds(0, 0, mainWidth, mainHeight);
    addAndMakeVisible(containerView_Details.get());
    
    
    // Inactive view state 0
    button_AddActive = std::make_unique<ImageButton>();
    button_AddActive->setTriggeredOnMouseDown(true);
    button_AddActive->setImages (false, true, true,
                                 imageAddIcon, 0.999f, Colour (0x00000000),
                                 Image(), 1.000f, Colour (0x00000000),
                                 imageAddIcon, 0.75, Colour (0x00000000));
    button_AddActive->addListener(this);
    button_AddActive->setBounds(130, 110, 68, 68);
    containerView_Inactive->addAndMakeVisible(button_AddActive.get());
    
    // Add Note / Active Settings view State 1
    button_AddNewNote = std::make_unique<ImageButton>();
    button_AddNewNote->setTriggeredOnMouseDown(true);
    button_AddNewNote->setImages (false, true, true,
                                  imageAddNote, 0.999f, Colour (0x00000000),
                                  Image(), 1.000f, Colour (0x00000000),
                                  imageAddNote, 0.75, Colour (0x00000000));
    button_AddNewNote->addListener(this);
    button_AddNewNote->setBounds(70, 190, 173, 43);
    containerView_Active->addAndMakeVisible(button_AddNewNote.get());
    
    ScalesManager * sm = projectManager->frequencyManager->scalesManager;
    
    comboBox_Note = std::make_unique<ComboBox>();
    sm->getComboBoxPopupMenuForChords(*comboBox_Note->getRootMenu(), SCALES_UNIT::LISSAJOUS_SCALE);

    comboBox_Note->setLookAndFeel(&lookAndFeel);
    
    comboBox_Note->setSelectedId(0);
    comboBox_Note->addListener(this);
    comboBox_Note->setBounds(170, 24+18, 111, 35);
    containerView_Active->addAndMakeVisible(comboBox_Note.get());
    
    comboBox_Octave = std::make_unique<ComboBox>();
    comboBox_Octave->addItemList(ProjectStrings::getOctaveArray(), 1);
    comboBox_Octave->setSelectedId(0);
    comboBox_Octave->addListener(this);
    comboBox_Octave->setLookAndFeel(&lookAndFeel);
    comboBox_Octave->setBounds(170, 80+18, 111, 35);
    containerView_Active->addAndMakeVisible(comboBox_Octave.get());
    
    
    
    
    // Details View with Delete state 2
    
    label_NoteValue = std::make_unique<Label>("", "C#Major");
    label_NoteValue->setBounds(217, 36, 76, 26);
    label_NoteValue->setFont(fontLight);
    label_NoteValue->setJustificationType(Justification::left);
    containerView_Details->addAndMakeVisible(label_NoteValue.get());
    
    
    label_OctaveValue  = std::make_unique<Label>("", "Minor");
    label_OctaveValue->setBounds(217, 80, 76, 26);
    label_OctaveValue->setFont(fontLight);
    label_OctaveValue->setJustificationType(Justification::left);
    containerView_Details->addAndMakeVisible(label_OctaveValue.get());
    
    label_FrequencyLabel  = std::make_unique<Label>("", "432Hz");
    label_FrequencyLabel->setBounds(217, 126, 76, 26);
    label_FrequencyLabel->setFont(fontLight);
    label_FrequencyLabel->setJustificationType(Justification::left);
    containerView_Details->addAndMakeVisible(label_FrequencyLabel.get());
    
    
    // active buttons
    button_Settings = std::make_unique<ImageButton>();
    button_Settings->setTriggeredOnMouseDown(true);
    button_Settings->setImages (false, true, true,
                                imageSettingsIcon, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageSettingsIcon, 0.75, Colour (0x00000000));
    button_Settings->addListener(this);
    button_Settings->setBounds(26, 224, 31, 31);
    containerView_Details->addAndMakeVisible(button_Settings.get());
    
    button_Delete = std::make_unique<ImageButton>();
    button_Delete->setTriggeredOnMouseDown(true);
    button_Delete->setImages (false, true, true,
                              imageDelete, 0.999f, Colour (0x00000000),
                              Image(), 1.000f, Colour (0x00000000),
                              imageDelete, 0.75, Colour (0x00000000));
    button_Delete->addListener(this);
    button_Delete->setBounds(0, 0, 34, 34);
    containerView_Details->addAndMakeVisible(button_Delete.get());
    
    paramIndexBase =  UNIT_1_CHORD_PLAYER_SHORTCUT_IS_ACTIVE + ((UNIT_1_CHORD_PLAYER_CHORDPLAYER_SCALE - UNIT_1_CHORD_PLAYER_SHORTCUT_IS_ACTIVE) * shortcutRef);
    
    setState(0);
    
}

LissajousCustomChordNoteComponent::~LissajousCustomChordNoteComponent() { }

void LissajousCustomChordNoteComponent::resized()
{
    backgroundImageComp     ->setBounds(noteBoxInset * scaleFactor, noteBoxInset * scaleFactor, backgroundWidth * scaleFactor, backgroundHeight * scaleFactor);
    containerView_Active    ->setBounds(0, 0, mainWidth * scaleFactor, mainHeight * scaleFactor);
    containerView_Inactive  ->setBounds(0, 0, mainWidth * scaleFactor, mainHeight * scaleFactor);
    containerView_Details   ->setBounds(0, 0, mainWidth * scaleFactor, mainHeight * scaleFactor);
    
    button_AddActive        ->setBounds(130 * scaleFactor, 110 * scaleFactor, 68 * scaleFactor, 68 * scaleFactor);
    button_AddNewNote       ->setBounds(70 * scaleFactor, 190 * scaleFactor, 173 * scaleFactor, 43 * scaleFactor);
    
    comboBox_Note           ->setBounds(170 * scaleFactor, (24 + 18) * scaleFactor, 111 * scaleFactor, 35 * scaleFactor);
    comboBox_Octave         ->setBounds(170 * scaleFactor, (80+18) * scaleFactor, 111 * scaleFactor, 35 * scaleFactor);
    
    label_NoteValue         ->setBounds(217 * scaleFactor, 36 * scaleFactor, 76 * scaleFactor, 26 * scaleFactor);
    label_OctaveValue       ->setBounds(217 * scaleFactor, 80 * scaleFactor, 76 * scaleFactor, 26 * scaleFactor);
    label_FrequencyLabel    ->setBounds(217 * scaleFactor, 126 * scaleFactor, 76 * scaleFactor, 26 * scaleFactor);
    
    button_Settings         ->setBounds(26 * scaleFactor, 224 * scaleFactor, 31 * scaleFactor, 31 * scaleFactor);
    button_Delete           ->setBounds(0, 0, 34 * scaleFactor, 34 * scaleFactor);
    
}

void LissajousCustomChordNoteComponent::paint (Graphics&){}


void LissajousCustomChordNoteComponent::buttonClicked (Button*button)
{
    if (button == button_Delete.get())
    {
        setState(0);
        
        // send false to projectManager CUSTOM_CHORD_ACTIVE_1
        projectManager->setLissajousParameter(paramIndexBase + CUSTOM_CHORD_ACTIVE_1+noteRef, 0);
    }
    else if (button == button_Settings.get())
    {
        // I think settings returns to note/octave selector, addNote
        setState(1);
        projectManager->setLissajousParameter(paramIndexBase + CUSTOM_CHORD_ACTIVE_1+noteRef, 1);
    }
    else if (button == button_AddActive.get())
    {
        setState(1);
        projectManager->setLissajousParameter(paramIndexBase +  CUSTOM_CHORD_ACTIVE_1+noteRef, 1);
    }
    else if (button == button_AddNewNote.get())
    {
        setState(2);
        // set to data structure
        
        // send to projectManager CUSTOM_CHORD_ACTIVE_1
        projectManager->setLissajousParameter(paramIndexBase +  CUSTOM_CHORD_ACTIVE_1+noteRef, 2);
        projectManager->setLissajousParameter(paramIndexBase +  CUSTOM_CHORD_NOTE_1 + noteRef, chosenNote);
        projectManager->setLissajousParameter(paramIndexBase +  CUSTOM_CHORD_OCTAVE_1 + noteRef, chosenOctave);
        
        
        int noteVal = projectManager->getLissajousParameter(paramIndexBase +  CUSTOM_CHORD_NOTE_1 + noteRef).operator int();
        
//        label_NoteValue->setText(projectManager->getStringForStringArray(ProjectManager::STRING_ARRAY_KEYNOTE, noteVal-1), dontSendNotification);
        label_NoteValue->setText(ProjectStrings::getKeynoteArray().getReference(noteVal-1), dontSendNotification);
        
        int octaveVal = projectManager->getLissajousParameter(paramIndexBase +  CUSTOM_CHORD_OCTAVE_1 + noteRef).operator int();
        
//        label_OctaveValue->setText(projectManager->getStringForStringArray(ProjectManager::STRING_ARRAY_OCTAVE, octaveVal-1), dontSendNotification);
        
        label_OctaveValue->setText(ProjectStrings::getOctaveArray().getReference(octaveVal-1), dontSendNotification);
        
    }

}

void LissajousCustomChordNoteComponent::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == comboBox_Note.get())
    {
        chosenNote = comboBox_Note->getSelectedId();
        
        projectManager->setLissajousParameter(paramIndexBase +  CUSTOM_CHORD_NOTE_1 + noteRef, chosenNote);
    }
    else if (comboBoxThatHasChanged == comboBox_Octave.get())
    {
        chosenOctave = comboBox_Octave->getSelectedId();
        
        projectManager->setLissajousParameter(paramIndexBase +  CUSTOM_CHORD_OCTAVE_1 + noteRef, chosenOctave);
    }

}

void LissajousCustomChordNoteComponent::syncGUI()
{
    // active / state
    const auto isActive = (int)projectManager->getLissajousParameter(paramIndexBase +  CUSTOM_CHORD_ACTIVE_1 + noteRef).operator bool();
    if (isActive)
    {
        // check state
        setState(isActive);
        
        // labels
        int noteVal = projectManager->getLissajousParameter(paramIndexBase +  CUSTOM_CHORD_NOTE_1 + noteRef).operator int();
        
        comboBox_Note->setSelectedId(noteVal);
        
        int octaveVal = projectManager->getLissajousParameter(paramIndexBase +  CUSTOM_CHORD_OCTAVE_1 + noteRef).operator int();

        comboBox_Octave->setSelectedId(octaveVal);
    
    }
    else
    {
        setState(isActive);
        
        // reset
        label_NoteValue         ->setText("", dontSendNotification);
        label_OctaveValue       ->setText("", dontSendNotification);
        label_FrequencyLabel    ->setText("", dontSendNotification);
        comboBox_Note           ->setSelectedId(0);
        comboBox_Octave         ->setSelectedId(0);
    }
}

void LissajousCustomChordNoteComponent::setState(int isActive)
{
    activeState = isActive;
    
    if (activeState == 0)
    {
        backgroundImageComp     ->setImage(imageBackgroundInactive);
        containerView_Inactive  ->setVisible(true);
        containerView_Active    ->setVisible(false);
        containerView_Details   ->setVisible(false);
    }
    else if (activeState == 1)
    {
        backgroundImageComp     ->setImage(imageSettingsBackground);
        containerView_Inactive  ->setVisible(false);
        containerView_Active    ->setVisible(true);
        containerView_Details   ->setVisible(false);
    }
    else if (activeState == 2)
    {
        backgroundImageComp     ->setImage(imageDetailsBackground);
        containerView_Inactive  ->setVisible(false);
        containerView_Active    ->setVisible(false);
        containerView_Details   ->setVisible(true);
    }
}

LissajousAddOnPopupComponent::LissajousAddOnPopupComponent(ProjectManager * pm, int axis)
{
    projectManager = pm;
    shortcutRef     = axis;
    
    backgroundImage = ImageCache::getFromMemory(BinaryData::AddOn_Background_Max_png, BinaryData::AddOn_Background_Max_pngSize);
    
    imageBlueCheckButtonNormal  = ImageCache::getFromMemory(BinaryData::Button_Checkbox_Normal_Max_png, BinaryData::Button_Checkbox_Normal_Max_pngSize);
    imageBlueCheckButtonSelected= ImageCache::getFromMemory(BinaryData::Button_Checkbox_Selected_Max_png, BinaryData::Button_Checkbox_Selected_Max_pngSize);
    
    // fonts
    Typeface::Ptr AssistantLight     = Typeface::createSystemTypefaceFor(BinaryData::AssistantLight_ttf, BinaryData::AssistantLight_ttfSize);
    
    Font fontLight(AssistantLight); fontLight.setHeight(33);
    
    
    int leftButtonBorder    = 24;
    int leftLabelBorder     = leftButtonBorder + 20;
    int yShift              = 27;
    
    
    // place buttons
    addonButton[AddOn_6] = std::make_unique<ImageButton>();
    addonButton[AddOn_6]->setTriggeredOnMouseDown(true);
    addonButton[AddOn_6]->setImages (false, true, true,
                                     imageBlueCheckButtonNormal, 0.999f, Colour (0x00000000),
                                     Image(), 1.000f, Colour (0x00000000),
                                     imageBlueCheckButtonSelected, 1.0, Colour (0x00000000));
    addonButton[AddOn_6]->addListener(this);
    addonButton[AddOn_6]->setBounds(leftButtonBorder, yShift, 16, 16);
    addAndMakeVisible(addonButton[AddOn_6].get());
    
    label[AddOn_6] = std::make_unique<Label>("", ProjectStrings::getAddons().getReference(AddOn_6));
    label[AddOn_6]->setBounds(leftLabelBorder, yShift, 64, 21);
    label[AddOn_6]->setJustificationType(Justification::left);
    label[AddOn_6]->setFont(fontLight);
    label[AddOn_6]->setColour(Label::textColourId, Colours::white);
    addAndMakeVisible(label[AddOn_6].get());
    
    //
    addonButton[AddOn_7] = std::make_unique<ImageButton>();
    addonButton[AddOn_7]->setTriggeredOnMouseDown(true);
    addonButton[AddOn_7]->setImages (false, true, true,
                                     imageBlueCheckButtonNormal, 0.999f, Colour (0x00000000),
                                     Image(), 1.000f, Colour (0x00000000),
                                     imageBlueCheckButtonSelected, 1.0, Colour (0x00000000));
    addonButton[AddOn_7]->addListener(this);
    addonButton[AddOn_7]->setBounds(leftButtonBorder, yShift * 2, 16, 16);
    addAndMakeVisible(addonButton[AddOn_7].get());
    
    label[AddOn_7] = std::make_unique<Label>("", ProjectStrings::getAddons().getReference(AddOn_7));
    label[AddOn_7]->setBounds(leftLabelBorder, yShift * 2, 64, 21);
    label[AddOn_7]->setJustificationType(Justification::left);
    label[AddOn_7]->setFont(fontLight);
    label[AddOn_7]->setColour(Label::textColourId, Colours::white);
    addAndMakeVisible(label[AddOn_7].get());
    
    //
    addonButton[AddOn_7Major] = std::make_unique<ImageButton>();
    addonButton[AddOn_7Major]->setTriggeredOnMouseDown(true);
    addonButton[AddOn_7Major]->setImages (false, true, true,
                                          imageBlueCheckButtonNormal, 0.999f, Colour (0x00000000),
                                          Image(), 1.000f, Colour (0x00000000),
                                          imageBlueCheckButtonSelected, 1.0, Colour (0x00000000));
    addonButton[AddOn_7Major]->addListener(this);
    addonButton[AddOn_7Major]->setBounds(leftButtonBorder, yShift * 3, 16, 16);
    addAndMakeVisible(addonButton[AddOn_7Major].get());
    
    label[AddOn_7Major] = std::make_unique<Label>("", ProjectStrings::getAddons().getReference(AddOn_7Major));
    label[AddOn_7Major]->setBounds(leftLabelBorder, yShift * 3, 64, 21);
    label[AddOn_7Major]->setJustificationType(Justification::left);
    label[AddOn_7Major]->setFont(fontLight);
    label[AddOn_7Major]->setColour(Label::textColourId, Colours::white);
    addAndMakeVisible(label[AddOn_7Major].get());
    
    ///// next
    leftButtonBorder    = 148;
    leftLabelBorder     = leftButtonBorder + 20;
    
    
    addonButton[AddOn_9] = std::make_unique<ImageButton>();
    addonButton[AddOn_9]->setTriggeredOnMouseDown(true);
    addonButton[AddOn_9]->setImages (false, true, true,
                                     imageBlueCheckButtonNormal, 0.999f, Colour (0x00000000),
                                     Image(), 1.000f, Colour (0x00000000),
                                     imageBlueCheckButtonSelected, 1.0, Colour (0x00000000));
    addonButton[AddOn_9]->addListener(this);
    addonButton[AddOn_9]->setBounds(leftButtonBorder, yShift * 1, 16, 16);
    addAndMakeVisible(addonButton[AddOn_9].get());
    
    label[AddOn_9] = std::make_unique<Label>("", ProjectStrings::getAddons().getReference(AddOn_9));
    label[AddOn_9]->setBounds(leftLabelBorder, yShift * 1, 64, 21);
    label[AddOn_9]->setJustificationType(Justification::left);
    label[AddOn_9]->setFont(fontLight);
    label[AddOn_9]->setColour(Label::textColourId, Colours::white);
    addAndMakeVisible(label[AddOn_9].get());
    
    addonButton[AddOn_9flat] = std::make_unique<ImageButton>();
    addonButton[AddOn_9flat]->setTriggeredOnMouseDown(true);
    addonButton[AddOn_9flat]->setImages (false, true, true,
                                         imageBlueCheckButtonNormal, 0.999f, Colour (0x00000000),
                                         Image(), 1.000f, Colour (0x00000000),
                                         imageBlueCheckButtonSelected, 1.0, Colour (0x00000000));
    addonButton[AddOn_9flat]->addListener(this);
    addonButton[AddOn_9flat]->setBounds(leftButtonBorder, yShift * 2, 16, 16);
    addAndMakeVisible(addonButton[AddOn_9flat].get());
    
    label[AddOn_9flat] = std::make_unique<Label>("", ProjectStrings::getAddons().getReference(AddOn_9flat));
    label[AddOn_9flat]->setBounds(leftLabelBorder, yShift * 2, 64, 21);
    label[AddOn_9flat]->setJustificationType(Justification::left);
    label[AddOn_9flat]->setFont(fontLight);
    label[AddOn_9flat]->setColour(Label::textColourId, Colours::white);
    addAndMakeVisible(label[AddOn_9flat].get());
    
    addonButton[AddOn_9sharp] = std::make_unique<ImageButton>();
    addonButton[AddOn_9sharp]->setTriggeredOnMouseDown(true);
    addonButton[AddOn_9sharp]->setImages (false, true, true,
                                          imageBlueCheckButtonNormal, 0.999f, Colour (0x00000000),
                                          Image(), 1.000f, Colour (0x00000000),
                                          imageBlueCheckButtonSelected, 1.0, Colour (0x00000000));
    addonButton[AddOn_9sharp]->addListener(this);
    addonButton[AddOn_9sharp]->setBounds(leftButtonBorder, yShift * 3, 16, 16);
    addAndMakeVisible(addonButton[AddOn_9sharp].get());
    
    label[AddOn_9sharp] = std::make_unique<Label>("", ProjectStrings::getAddons().getReference(AddOn_9sharp));
    label[AddOn_9sharp]->setBounds(leftLabelBorder, yShift * 3, 64, 21);
    label[AddOn_9sharp]->setJustificationType(Justification::left);
    label[AddOn_9sharp]->setFont(fontLight);
    label[AddOn_9sharp]->setColour(Label::textColourId, Colours::white);
    addAndMakeVisible(label[AddOn_9sharp].get());
    
    
    addonButton[AddOn_11] = std::make_unique<ImageButton>();
    addonButton[AddOn_11]->setTriggeredOnMouseDown(true);
    addonButton[AddOn_11]->setImages (false, true, true,
                                      imageBlueCheckButtonNormal, 0.999f, Colour (0x00000000),
                                      Image(), 1.000f, Colour (0x00000000),
                                      imageBlueCheckButtonSelected, 1.0, Colour (0x00000000));
    addonButton[AddOn_11]->addListener(this);
    addonButton[AddOn_11]->setBounds(leftButtonBorder, yShift * 4, 16, 16);
    addAndMakeVisible(addonButton[AddOn_11].get());
    
    label[AddOn_11] = std::make_unique<Label>("", ProjectStrings::getAddons().getReference(AddOn_11));
    label[AddOn_11]->setBounds(leftLabelBorder, yShift * 4, 64, 21);
    label[AddOn_11]->setJustificationType(Justification::left);
    label[AddOn_11]->setFont(fontLight);
    label[AddOn_11]->setColour(Label::textColourId, Colours::white);
    addAndMakeVisible(label[AddOn_11].get());
    
    
    addonButton[AddOn_11sharp] = std::make_unique<ImageButton>();
    addonButton[AddOn_11sharp]->setTriggeredOnMouseDown(true);
    addonButton[AddOn_11sharp]->setImages (false, true, true,
                                           imageBlueCheckButtonNormal, 0.999f, Colour (0x00000000),
                                           Image(), 1.000f, Colour (0x00000000),
                                           imageBlueCheckButtonSelected, 1.0, Colour (0x00000000));
    addonButton[AddOn_11sharp]->addListener(this);
    addonButton[AddOn_11sharp]->setBounds(leftButtonBorder, yShift * 5, 16, 16);
    addAndMakeVisible(addonButton[AddOn_11sharp].get());
    
    label[AddOn_11sharp] = std::make_unique<Label>("", ProjectStrings::getAddons().getReference(AddOn_11sharp));
    label[AddOn_11sharp]->setBounds(leftLabelBorder, yShift * 5, 64, 21);
    label[AddOn_11sharp]->setJustificationType(Justification::left);
    label[AddOn_11sharp]->setFont(fontLight);
    label[AddOn_11sharp]->setColour(Label::textColourId, Colours::white);
    addAndMakeVisible(label[AddOn_11sharp].get());
    
    
    ///// next
    leftButtonBorder    = 270;
    leftLabelBorder     = leftButtonBorder + 20;
    
    addonButton[AddOn_13] = std::make_unique<ImageButton>();
    addonButton[AddOn_13]->setTriggeredOnMouseDown(true);
    addonButton[AddOn_13]->setImages (false, true, true,
                                      imageBlueCheckButtonNormal, 0.999f, Colour (0x00000000),
                                      Image(), 1.000f, Colour (0x00000000),
                                      imageBlueCheckButtonSelected, 1.0, Colour (0x00000000));
    addonButton[AddOn_13]->addListener(this);
    addonButton[AddOn_13]->setBounds(leftButtonBorder, yShift * 1, 16, 16);
    addAndMakeVisible(addonButton[AddOn_13].get());
    
    label[AddOn_13] = std::make_unique<Label>("", ProjectStrings::getAddons().getReference(AddOn_13));
    label[AddOn_13]->setBounds(leftLabelBorder, yShift * 1, 64, 21);
    label[AddOn_13]->setJustificationType(Justification::left);
    label[AddOn_13]->setFont(fontLight);
    label[AddOn_13]->setColour(Label::textColourId, Colours::white);
    addAndMakeVisible(label[AddOn_13].get());
    
    
    addonButton[AddOn_13flat] = std::make_unique<ImageButton>();
    addonButton[AddOn_13flat]->setTriggeredOnMouseDown(true);
    addonButton[AddOn_13flat]->setImages (false, true, true,
                                          imageBlueCheckButtonNormal, 0.999f, Colour (0x00000000),
                                          Image(), 1.000f, Colour (0x00000000),
                                          imageBlueCheckButtonSelected, 1.0, Colour (0x00000000));
    addonButton[AddOn_13flat]->addListener(this);
    addonButton[AddOn_13flat]->setBounds(leftButtonBorder, yShift * 2, 16, 16);
    addAndMakeVisible(addonButton[AddOn_13flat].get());
    
    label[AddOn_13flat] = std::make_unique<Label>("", ProjectStrings::getAddons().getReference(AddOn_13flat));
    label[AddOn_13flat]->setBounds(leftLabelBorder, yShift * 2, 64, 21);
    label[AddOn_13flat]->setJustificationType(Justification::left);
    label[AddOn_13flat]->setFont(fontLight);
    label[AddOn_13flat]->setColour(Label::textColourId, Colours::white);
    addAndMakeVisible(label[AddOn_13flat].get());
    
    addonButton[AddOn_Inverted] = std::make_unique<ImageButton>();
    addonButton[AddOn_Inverted]->setTriggeredOnMouseDown(true);
    addonButton[AddOn_Inverted]->setImages (false, true, true,
                                            imageBlueCheckButtonNormal, 0.999f, Colour (0x00000000),
                                            Image(), 1.000f, Colour (0x00000000),
                                            imageBlueCheckButtonSelected, 1.0, Colour (0x00000000));
    addonButton[AddOn_Inverted]->addListener(this);
    addonButton[AddOn_Inverted]->setBounds(leftButtonBorder, yShift * 3, 16, 16);
    addAndMakeVisible(addonButton[AddOn_Inverted].get());
    
    label[AddOn_Inverted] = std::make_unique<Label>("", ProjectStrings::getAddons().getReference(AddOn_Inverted));
    label[AddOn_Inverted]->setBounds(leftLabelBorder, yShift * 3, 64, 21);
    label[AddOn_Inverted]->setJustificationType(Justification::left);
    label[AddOn_Inverted]->setFont(fontLight);
    label[AddOn_Inverted]->setColour(Label::textColourId, Colours::white);
    addAndMakeVisible(label[AddOn_Inverted].get());
    
    paramIndexBase =  UNIT_1_CHORD_PLAYER_SHORTCUT_IS_ACTIVE + ((UNIT_1_CHORD_PLAYER_CHORDPLAYER_SCALE - UNIT_1_CHORD_PLAYER_SHORTCUT_IS_ACTIVE) * shortcutRef);
    
}

LissajousAddOnPopupComponent::~LissajousAddOnPopupComponent()
{
    int leftButtonBorder    = 24 * scaleFactor;;
    int leftLabelBorder     = leftButtonBorder + (20 * scaleFactor);
    int yShift              = 27 * scaleFactor;
    
    int fontSize = 24;
    
    addonButton[AddOn_6]        ->setBounds(leftButtonBorder, yShift,       16 * scaleFactor, 16 * scaleFactor);
    label[AddOn_6]              ->setBounds(leftLabelBorder, yShift,        64 * scaleFactor, 21 * scaleFactor);
    label[AddOn_6]              ->setFont(fontSize * scaleFactor);
    
    addonButton[AddOn_7]        ->setBounds(leftButtonBorder, yShift * 2,   16 * scaleFactor, 16 * scaleFactor);
    label[AddOn_7]              ->setBounds(leftLabelBorder, yShift * 2,    64 * scaleFactor, 21 * scaleFactor);
    label[AddOn_7]              ->setFont(fontSize * scaleFactor);
    
    addonButton[AddOn_7Major]   ->setBounds(leftButtonBorder, yShift * 3,   16 * scaleFactor, 16 * scaleFactor);
    label[AddOn_7Major]         ->setBounds(leftLabelBorder, yShift * 3,    64 * scaleFactor, 21 * scaleFactor);
    label[AddOn_7Major]         ->setFont(fontSize * scaleFactor);
    
    ///// next
    leftButtonBorder    = 148 * scaleFactor;
    leftLabelBorder     = leftButtonBorder + (20 * scaleFactor);
    
    
    addonButton[AddOn_9]        ->setBounds(leftButtonBorder, yShift * 1,   16 * scaleFactor, 16 * scaleFactor);
    label[AddOn_9]              ->setBounds(leftLabelBorder, yShift * 1,    64 * scaleFactor, 21 * scaleFactor);
    label[AddOn_9]              ->setFont(fontSize * scaleFactor);
    
    addonButton[AddOn_9flat]    ->setBounds(leftButtonBorder, yShift * 2,   16 * scaleFactor, 16 * scaleFactor);
    label[AddOn_9flat]          ->setBounds(leftLabelBorder, yShift * 2,    64 * scaleFactor, 21 * scaleFactor);
    label[AddOn_9flat]          ->setFont(fontSize * scaleFactor);
    
    addonButton[AddOn_9sharp]   ->setBounds(leftButtonBorder, yShift * 3,   16 * scaleFactor, 16 * scaleFactor);
    label[AddOn_9sharp]         ->setBounds(leftLabelBorder, yShift * 3,    64 * scaleFactor, 21 * scaleFactor);
    label[AddOn_9sharp]         ->setFont(fontSize * scaleFactor);
    
    
    addonButton[AddOn_11]       ->setBounds(leftButtonBorder, yShift * 4,   16 * scaleFactor, 16 * scaleFactor);
    label[AddOn_11]             ->setBounds(leftLabelBorder, yShift * 4,    64 * scaleFactor, 21 * scaleFactor);
    label[AddOn_11]             ->setFont(fontSize * scaleFactor);
    
    addonButton[AddOn_11sharp]  ->setBounds(leftButtonBorder, yShift * 5,   16 * scaleFactor, 16 * scaleFactor);
    label[AddOn_11sharp]        ->setBounds(leftLabelBorder, yShift * 5, 64 * scaleFactor, 21 * scaleFactor);
    label[AddOn_11sharp]        ->setFont(fontSize * scaleFactor);
    
    ///// next
    leftButtonBorder            = 270 * scaleFactor;
    leftLabelBorder             = leftButtonBorder + (20 * scaleFactor);
    
    addonButton[AddOn_13]       ->setBounds(leftButtonBorder, yShift * 1,   16 * scaleFactor, 16 * scaleFactor);
    label[AddOn_13]             ->setBounds(leftLabelBorder, yShift * 1,    64 * scaleFactor, 21 * scaleFactor);
    label[AddOn_13]             ->setFont(fontSize * scaleFactor);
    
    
    addonButton[AddOn_13flat]   ->setBounds(leftButtonBorder, yShift * 2,   16 * scaleFactor, 16 * scaleFactor);
    label[AddOn_13flat]         ->setBounds(leftLabelBorder, yShift * 2,    64 * scaleFactor, 21 * scaleFactor);
    label[AddOn_13flat]         ->setFont(fontSize * scaleFactor);
    
    
    addonButton[AddOn_Inverted] ->setBounds(leftButtonBorder, yShift * 3,   16 * scaleFactor, 16 * scaleFactor);
    label[AddOn_Inverted]       ->setBounds(leftLabelBorder, yShift * 3,    64 * scaleFactor, 21 * scaleFactor);
    label[AddOn_Inverted]       ->setFont(33 * scaleFactor);
}

void LissajousAddOnPopupComponent::resized()
{
    
}

void LissajousAddOnPopupComponent::paint (Graphics&g)
{
    // draw background
    g.drawImage(backgroundImage, 0, 0, 357, 179, 0, 0, 357, 179);
}

void LissajousAddOnPopupComponent::buttonClicked (Button*button)
{
    if (button == addonButton[AddOn_6].get())
    {
        if (button->getToggleState())
        {
            projectManager->setLissajousParameter(paramIndexBase + ADD_ONS, AddOn_NONE);
        }
        else
        {
            projectManager->setLissajousParameter(paramIndexBase +  ADD_ONS, AddOn_6);
        }
        
    }
    else if (button == addonButton[AddOn_7].get())
    {
        if (button->getToggleState())
        {
            projectManager->setLissajousParameter(paramIndexBase +  ADD_ONS, AddOn_NONE);
        }
        else
        {
            projectManager->setLissajousParameter(paramIndexBase +  ADD_ONS, AddOn_7);
        }
    }
    else if (button == addonButton[AddOn_7Major].get())
    {
        if (button->getToggleState())
        {
            projectManager->setLissajousParameter(paramIndexBase +  ADD_ONS, AddOn_NONE);
        }
        else
        {
            projectManager->setLissajousParameter(paramIndexBase +  ADD_ONS, AddOn_7Major);
        }
    }
    else if (button == addonButton[AddOn_9].get())
    {
        if (button->getToggleState())
        {
            projectManager->setLissajousParameter(paramIndexBase +  ADD_ONS, AddOn_NONE);
        }
        else
        {
            projectManager->setLissajousParameter(paramIndexBase +  ADD_ONS, AddOn_9);
        }
    }
    else if (button == addonButton[AddOn_9flat].get())
    {
        if (button->getToggleState())
        {
            projectManager->setLissajousParameter(paramIndexBase +  ADD_ONS, AddOn_NONE);
        }
        else
        {
            projectManager->setLissajousParameter(paramIndexBase +  ADD_ONS, AddOn_9flat);
        }
    }
    else if (button == addonButton[AddOn_9sharp].get())
    {
        if (button->getToggleState())
        {
            projectManager->setLissajousParameter(paramIndexBase + ADD_ONS, AddOn_NONE);
        }
        else
        {
            projectManager->setLissajousParameter(paramIndexBase + ADD_ONS, AddOn_9sharp);
        }
    }
    else if (button == addonButton[AddOn_11].get())
    {
        if (button->getToggleState())
        {
            projectManager->setLissajousParameter(paramIndexBase + ADD_ONS, AddOn_NONE);
        }
        else
        {
            projectManager->setLissajousParameter(paramIndexBase + ADD_ONS, AddOn_11);
        }
    }
    else if (button == addonButton[AddOn_11sharp].get())
    {
        if (button->getToggleState())
        {
            projectManager->setLissajousParameter(paramIndexBase + ADD_ONS, AddOn_NONE);
        }
        else
        {
            projectManager->setLissajousParameter(paramIndexBase + ADD_ONS, AddOn_11sharp);
        }
    }
    else if (button == addonButton[AddOn_13].get())
    {
        if (button->getToggleState())
        {
            projectManager->setLissajousParameter(paramIndexBase + ADD_ONS, AddOn_NONE);
        }
        else
        {
            projectManager->setLissajousParameter(paramIndexBase + ADD_ONS, AddOn_13);
        }
    }
    else if (button == addonButton[AddOn_13flat].get())
    {
        if (button->getToggleState())
        {
            projectManager->setLissajousParameter(paramIndexBase + ADD_ONS, AddOn_NONE);
        }
        else
        {
            projectManager->setLissajousParameter(paramIndexBase + ADD_ONS, AddOn_13flat);
        }
    }
    else if (button == addonButton[AddOn_Inverted].get())
    {
        if (button->getToggleState())
        {
            projectManager->setLissajousParameter(paramIndexBase + ADD_ONS, AddOn_NONE);
        }
        else
        {
            projectManager->setLissajousParameter(paramIndexBase + ADD_ONS, AddOn_Inverted);
        }
    }
}

void LissajousAddOnPopupComponent::syncGUI()
{
    int val = projectManager->getLissajousParameter(paramIndexBase + ADD_ONS).operator int();
    
    for (int i = 0; i < NUM_ADDONS; i++)
    {
        if (i == val)
        {
            addonButton[i]->setToggleState(true, dontSendNotification);
        }
        else
        {
            addonButton[i]->setToggleState(false, dontSendNotification);
        }
    }
}


LissajousChordPlayerSettingsComponent::LissajousChordPlayerSettingsComponent(ProjectManager * pm, int axisRef)
{
    shortcutRef            = axisRef;
    
    projectManager  = pm;
    
    projectManager->addUIListener(this);
    
    // LookAndFeels
    
    mainBackgroundImage         = ImageCache::getFromMemory(BinaryData::ChordPlayerSettingsBackground_png, BinaryData::ChordPlayerSettingsBackground_pngSize);
    imageCloseButton            = ImageCache::getFromMemory(BinaryData::CloseButton2x_png, BinaryData::CloseButton2x_pngSize);
    imageBlueButtonNormal       = ImageCache::getFromMemory(BinaryData::BlueButton_Normal_png, BinaryData::BlueButton_Normal_pngSize);
    imageBlueButtonSelected     = ImageCache::getFromMemory(BinaryData::BlueButton_Selected_png, BinaryData::BlueButton_Selected_pngSize);
    imageAddButton              = ImageCache::getFromMemory(BinaryData::ApplyButton2x_png, BinaryData::ApplyButton2x_pngSize);
    
    imageBlueCheckButtonNormal  = ImageCache::getFromMemory(BinaryData::Button_Checkbox_Normal_Max_png, BinaryData::Button_Checkbox_Normal_Max_pngSize);
    imageBlueCheckButtonSelected= ImageCache::getFromMemory(BinaryData::Button_Checkbox_Selected_Max_png, BinaryData::Button_Checkbox_Selected_Max_pngSize);
    
    
    button_Close = std::make_unique<ImageButton>();
    button_Close->setTriggeredOnMouseDown(true);
    button_Close->setImages (false, true, true,
                           imageCloseButton, 0.999f, Colour (0x00000000),
                           Image(), 1.000f, Colour (0x00000000),
                           imageCloseButton, 0.75, Colour (0x00000000));
    button_Close->addListener(this);
    addAndMakeVisible(button_Close.get());
    
    // toggle between these two
    button_ChooseChordFromList = std::make_unique<ImageButton>();
    button_ChooseChordFromList->setTriggeredOnMouseDown(true);
    button_ChooseChordFromList->setImages (false, true, true,
                             imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                             Image(), 1.000f, Colour (0x00000000),
                             imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_ChooseChordFromList->addListener(this);
    addAndMakeVisible(button_ChooseChordFromList.get());
    
    
    button_ChooseChordFromFrequency = std::make_unique<ImageButton>();
    button_ChooseChordFromFrequency->setTriggeredOnMouseDown(true);
    button_ChooseChordFromFrequency->setImages (false, true, true,
                                           imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                           Image(), 1.000f, Colour (0x00000000),
                                           imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_ChooseChordFromFrequency->addListener(this);
    addAndMakeVisible(button_ChooseChordFromFrequency.get());
    
    // act as toggle between 5 buttons
    button_Default = std::make_unique<ImageButton>();
    button_Default->setTriggeredOnMouseDown(true);
    button_Default->setImages (false, true, true,
                                                imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                                Image(), 1.000f, Colour (0x00000000),
                                                imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_Default->addListener(this);
    addAndMakeVisible(button_Default.get());
    
    int shift = 20;
    button_Sine = std::make_unique<ImageButton>();
    button_Sine->setTriggeredOnMouseDown(true);
    button_Sine->setImages (false, true, true,
                                                imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                                Image(), 1.000f, Colour (0x00000000),
                                                imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_Sine->addListener(this);
    addAndMakeVisible(button_Sine.get());
    
    button_Triangle = std::make_unique<ImageButton>();
    button_Triangle->setTriggeredOnMouseDown(true);
    button_Triangle->setImages (false, true, true,
                                                imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                                Image(), 1.000f, Colour (0x00000000),
                                                imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_Triangle->addListener(this);
    addAndMakeVisible(button_Triangle.get());
    
    button_Square = std::make_unique<ImageButton>();
    button_Square->setTriggeredOnMouseDown(true);
    button_Square->setImages (false, true, true,
                                                imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                                Image(), 1.000f, Colour (0x00000000),
                                                imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_Square->addListener(this);
    addAndMakeVisible(button_Square.get());
    
    button_Sawtooth = std::make_unique<ImageButton>();
    button_Sawtooth->setTriggeredOnMouseDown(true);
    button_Sawtooth->setImages (false, true, true,
                                                imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                                Image(), 1.000f, Colour (0x00000000),
                                                imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_Sawtooth->addListener(this);
    addAndMakeVisible(button_Sawtooth.get());
    
    button_Wavetable = std::make_unique<ImageButton>();
    button_Wavetable->setTriggeredOnMouseDown(true);
    button_Wavetable->setImages (false, true, true,
                                                imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                                Image(), 1.000f, Colour (0x00000000),
                                                imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_Wavetable->addListener(this);
    addAndMakeVisible(button_Wavetable.get());
    
    
    button_WavetableEditor = std::make_unique<TextButton>("");
    button_WavetableEditor->setButtonText("WT Editor");
    button_WavetableEditor->setLookAndFeel(&lookAndFeel);
    button_WavetableEditor->addListener(this);
    addAndMakeVisible(button_WavetableEditor.get());
    
    
    button_Add = std::make_unique<ImageButton>();
    button_Add->setTriggeredOnMouseDown(true);
    button_Add->setImages (false, true, true,
                                imageAddButton, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageAddButton, 0.888, Colour (0x00000000));
    button_Add->addListener(this);
    addAndMakeVisible(button_Add.get());
    
    int shiftBack = 8;
    button_Multiplication = std::make_unique<ImageButton>();
    button_Multiplication->setTriggeredOnMouseDown(true);
    button_Multiplication->setImages (false, true, true,
                                imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_Multiplication->addListener(this);
    addAndMakeVisible(button_Multiplication.get());
    
    button_Division = std::make_unique<ImageButton>();
    button_Division->setTriggeredOnMouseDown(true);
    button_Division->setImages (false, true, true,
                                imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_Division->addListener(this);
    addAndMakeVisible(button_Division.get());
    
    
    
    // Knobs
    slider_Amplitude = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_AMPLITUDE);
    slider_Amplitude    ->setRange (0, 1.0, 0);
    slider_Amplitude    ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_Amplitude    ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_Amplitude    ->addListener (this);
    slider_Amplitude    ->setTextValueSuffix("%");
    slider_Amplitude    ->setNumDecimalPlacesToDisplay(1);
    slider_Amplitude    ->setLookAndFeel(&lookAndFeel);
    
    addAndMakeVisible(slider_Amplitude.get());
    
    int dif = 219;
    
    slider_Attack = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_ATTACK);
    slider_Attack       ->setRange (0, 1.0, 0);
    slider_Attack       ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_Attack       ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_Attack       ->addListener (this);
    slider_Attack       ->setTextValueSuffix("Ms");
    slider_Attack       ->setNumDecimalPlacesToDisplay(0);
    slider_Attack       ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_Attack.get());
    
    slider_Decay = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_DECAY);
    slider_Decay        ->setRange (0, 1.0, 0);
    slider_Decay        ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_Decay        ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_Decay        ->addListener (this);
    slider_Decay        ->setTextValueSuffix("Ms");
    slider_Decay        ->setNumDecimalPlacesToDisplay(0);
    slider_Decay        ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_Decay.get());
    
    slider_Sustain = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_SUSTAIN);
    slider_Sustain      ->setRange (0, 1.0, 0);
    slider_Sustain      ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_Sustain      ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_Sustain      ->addListener (this);
    slider_Sustain      ->setTextValueSuffix("%");
    slider_Sustain      ->setNumDecimalPlacesToDisplay(2);
    slider_Sustain        ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_Sustain.get());
    
    slider_Release = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_RELEASE);
    slider_Release      ->setRange (0, 1.0, 0);
    slider_Release      ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_Release      ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_Release      ->addListener (this);
    slider_Release      ->setTextValueSuffix("Ms");
    slider_Release      ->setNumDecimalPlacesToDisplay(0);
    slider_Release      ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_Release.get());
    
    // Font
    Typeface::Ptr AssistantLight        = Typeface::createSystemTypefaceFor(BinaryData::AssistantLight_ttf, BinaryData::AssistantLight_ttfSize);
    Typeface::Ptr AssistantSemiBold     = Typeface::createSystemTypefaceFor(BinaryData::AssistantSemiBold_ttf, BinaryData::AssistantSemiBold_ttfSize);
    Font fontSemiBold(AssistantSemiBold);
    Font fontLight(AssistantLight);
    
    fontSemiBold.setHeight(33);
    fontLight.setHeight(33);
    
    // Combobox
    comboBoxKeynote = std::make_unique<ComboBox>();
    comboBoxKeynote->setSelectedId(0);
    comboBoxKeynote->addListener(this);
    comboBoxKeynote->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(comboBoxKeynote.get());
    
    comboBoxChordtype = std::make_unique<ComboBox>();
    comboBoxChordtype->setSelectedId(0);
    comboBoxChordtype->addListener(this);
    comboBoxChordtype->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(comboBoxChordtype.get());

    comboBoxPlayingInstrument = std::make_unique<ComboBox>();
    
    // generate
    PopupMenu * comboBoxMenu =  comboBoxPlayingInstrument->getRootMenu();
    
    // Use synthesis-based instrument list instead of file system scanning
    // Use properly implemented synthesis instruments from SynthesisLibraryManager
    Array<String> synthInstruments = {
        "Grand Piano",      // Physical Modeling
        "Acoustic Guitar",  // Karplus-Strong
        "Harp",            // Karplus-Strong
        "Strings",         // Physical Modeling
        "Church Organ",    // Wavetable
        "Lead Synth",      // Wavetable
        "Pad Synth",       // Wavetable
        "Bass Synth"       // Wavetable
    };
    
    // Add synthesis instruments to combo box menu
    for (int i = 0; i < synthInstruments.size(); i++)
    {
        comboBoxMenu->addItem(i+1, synthInstruments[i]);
    }

    comboBoxPlayingInstrument->setLookAndFeel(&lookAndFeel);
    comboBoxPlayingInstrument->getRootMenu()->setLookAndFeel(&lookAndFeel);
    comboBoxPlayingInstrument->addListener(this);
    addAndMakeVisible(comboBoxPlayingInstrument.get());
    
    textEditorRepeat = std::make_unique<TextEditor>("");
    textEditorRepeat->setReturnKeyStartsNewLine(false);
    textEditorRepeat->setInputRestrictions(2, "0123456789");
    textEditorRepeat->setMultiLine(false);
    textEditorRepeat->setText("1");
    textEditorRepeat->addListener(this);
    textEditorRepeat->setLookAndFeel(&lookAndFeel);
    textEditorRepeat->setJustification(Justification::centred);
    textEditorRepeat->setFont(fontSemiBold);
    textEditorRepeat->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditorRepeat->applyFontToAllText(fontSemiBold);
    textEditorRepeat->applyColourToAllText(Colours::lightgrey);
    addAndMakeVisible(textEditorRepeat.get());
    
    textEditorPause = std::make_unique<TextEditor>("");
    textEditorPause->setReturnKeyStartsNewLine(false);
    textEditorPause->setInputRestrictions(5, "0123456789");
    textEditorPause->setMultiLine(false);
    textEditorPause->setText("1000");
    textEditorPause->addListener(this);
    textEditorPause->setLookAndFeel(&lookAndFeel);
    textEditorPause->setJustification(Justification::centred);
    textEditorPause->setFont(fontSemiBold);
    textEditorPause->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditorPause->applyFontToAllText(fontSemiBold);
    textEditorPause->applyColourToAllText(Colours::lightgrey);
    addAndMakeVisible(textEditorPause.get());
    
    
    textEditorLength = std::make_unique<TextEditor>("");
    textEditorLength->setReturnKeyStartsNewLine(false);
    textEditorLength->setMultiLine(false);
    textEditorLength->setInputRestrictions(5, "0123456789");
    textEditorLength->setText("1000");
    textEditorLength->addListener(this);
    textEditorLength->setLookAndFeel(&lookAndFeel);
    textEditorLength->setJustification(Justification::centred);
    textEditorLength->setFont(fontSemiBold);
    textEditorLength->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditorLength->applyFontToAllText(fontSemiBold);
    textEditorLength->applyColourToAllText(Colours::lightgrey);
    addAndMakeVisible(textEditorLength.get());
    
    button_ManipulateFreq = std::make_unique<ImageButton>();
    button_ManipulateFreq->setTriggeredOnMouseDown(true);
    button_ManipulateFreq->setImages (false, true, true,
                               imageBlueCheckButtonNormal, 0.999f, Colour (0x00000000),
                               Image(), 1.000f, Colour (0x00000000),
                               imageBlueCheckButtonSelected, 1.0, Colour (0x00000000));
    button_ManipulateFreq->addListener(this);
    addAndMakeVisible(button_ManipulateFreq.get());
    
    // TextEntryBoxes
    textEditorInsertFreq = std::make_unique<TextEditor>("");
    textEditorInsertFreq->setReturnKeyStartsNewLine(false);
    textEditorInsertFreq->setMultiLine(false);
    textEditorInsertFreq->setInputRestrictions(5, "0123456789");
    textEditorInsertFreq->setText("432");
    textEditorInsertFreq->addListener(this);
    textEditorInsertFreq->setLookAndFeel(&lookAndFeel);
    textEditorInsertFreq->setJustification(Justification::centred);
    textEditorInsertFreq->setFont(fontSemiBold);
    textEditorInsertFreq->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditorInsertFreq->applyFontToAllText(fontSemiBold);
    textEditorInsertFreq->applyColourToAllText(Colours::lightgrey);

    addAndMakeVisible(textEditorInsertFreq.get());
    
    textEditorMultiplication = std::make_unique<TextEditor>("");
    textEditorMultiplication->setReturnKeyStartsNewLine(false);
    textEditorMultiplication->setMultiLine(false);
    textEditorMultiplication->setInputRestrictions(6, "0123456789.");
    textEditorMultiplication->setText("1.0");
    textEditorMultiplication->addListener(this);
    textEditorMultiplication->setLookAndFeel(&lookAndFeel);
    textEditorMultiplication->setJustification(Justification::centred);
    textEditorMultiplication->setFont(fontSemiBold);
    textEditorMultiplication->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditorMultiplication->applyFontToAllText(fontSemiBold);
    textEditorMultiplication->applyColourToAllText(Colours::lightgrey);
    addAndMakeVisible(textEditorMultiplication.get());
    
    textEditorDivision = std::make_unique<TextEditor>("");
    textEditorDivision->setReturnKeyStartsNewLine(false);
    textEditorDivision->setMultiLine(false);
    textEditorDivision->setInputRestrictions(6, "0123456789.");
    textEditorDivision->setText("1.0");
    textEditorDivision->addListener(this);
    textEditorDivision->setLookAndFeel(&lookAndFeel);
    textEditorDivision->setJustification(Justification::centred);
    textEditorDivision->setFont(fontSemiBold);
    textEditorDivision->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditorDivision->applyFontToAllText(fontSemiBold);
    textEditorDivision->applyColourToAllText(Colours::lightgrey);
    addAndMakeVisible(textEditorDivision.get());
    
    textEditor_Octave = std::make_unique<TextEditor>("");
    textEditor_Octave->setReturnKeyStartsNewLine(false);
    textEditor_Octave->setMultiLine(false);
    textEditor_Octave->setInputRestrictions(6, "0123456789.");
    textEditor_Octave->setText("1.0");
    textEditor_Octave->addListener(this);
    textEditor_Octave->setLookAndFeel(&lookAndFeel);
    textEditor_Octave->setJustification(Justification::centred);
    textEditor_Octave->setFont(fontSemiBold);
    textEditor_Octave->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditor_Octave->applyFontToAllText(fontSemiBold);
    textEditor_Octave->applyColourToAllText(Colours::lightgrey);
    addAndMakeVisible(textEditor_Octave.get());
    
    
    button_AddCustomChord = std::make_unique<TextButton>("Add Custom Chord");
    button_AddCustomChord->addListener(this);
    button_AddCustomChord->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(button_AddCustomChord.get());
    
    button_Addons = std::make_unique<TextButton>("Addons");
    button_Addons->addListener(this);
    button_Addons->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(button_Addons.get());
    

    
    
    // Labels
    
    comboBox_Scales = std::make_unique<ComboBox>();
    PopupMenu * scalesMenu =  comboBox_Scales->getRootMenu();
    scalesMenu->setLookAndFeel(&lookAndFeel);
        // return full popup menu system for dropdown in Settings / ChordPlayer
    
    PopupMenu diatonicMenu;
    diatonicMenu.addItem(DIATONIC_PYTHAGOREAN, "Pythagorean");
    diatonicMenu.addItem(DIATONIC_ITERATION_FIFTH, "Iteration Fifth");
    diatonicMenu.addItem(DIATONIC_JUSTLY_TUNED, "Justly Tuned");
    
    scalesMenu->addSubMenu("Diatonic", diatonicMenu);
        
    PopupMenu chromaticMenu;
    chromaticMenu.addItem(CHROMATIC_PYTHAGOREAN, "Pythagorean");
    chromaticMenu.addItem(CHROMATIC_JUST_INTONATION, "Just Itonation");
    chromaticMenu.addItem(CHROMATIC_ET, "Equal Temperament");
        
    scalesMenu->addSubMenu("Chromatic", chromaticMenu);
        
    PopupMenu harmonicMenu;
//    harmonicMenu.addItem(HARMONIC_ET, "Equal Temperament");
//    harmonicMenu.addItem(HARMONIC_MODERN, "Modern");
    harmonicMenu.addItem(HARMONIC_SIMPLE, "Simple");
        
    scalesMenu->addSubMenu("Harmonic", harmonicMenu);
        
    scalesMenu->addItem(ENHARMONIC, "Enharmonic");
    scalesMenu->addItem(SOLFEGGIO, "Solfeggio");
    
    comboBox_Scales->setSelectedId(DIATONIC_PYTHAGOREAN);
    comboBox_Scales->addListener(this);
    comboBox_Scales->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(comboBox_Scales.get());
    
    
    imagePanicButton                    = ImageCache::getFromMemory(BinaryData::PanicButton2x_png, BinaryData::PanicButton2x_pngSize);
    
    button_Panic = std::make_unique<ImageButton>();
    button_Panic->setTriggeredOnMouseDown(true);
    button_Panic->setImages (false, true, true,
                              imagePanicButton, 0.999f, Colour (0x00000000),
                              Image(), 1.000f, Colour (0x00000000),
                              imagePanicButton, 0.6, Colour (0x00000000));
    button_Panic->addListener(this);
    addAndMakeVisible(button_Panic.get());
    
    // add on popup component
    addOnPopupComponent = std::make_unique<LissajousAddOnPopupComponent>(projectManager, shortcutRef);
    addAndMakeVisible(addOnPopupComponent.get());
    addOnPopupComponent->setVisible(false);
    
    customChordPopupComponent = std::make_unique<LissajousCustomChordPopupComponent>(projectManager, shortcutRef);
    addAndMakeVisible(customChordPopupComponent.get());
    customChordPopupComponent->setVisible(false);
    
    for (int i = AMPLITUDE_MIN; i <= RELEASE_MAX; i++)
    {
        updateMinMaxSettings(i);
    }
    
    wavetableEditorComponent = std::make_unique<WaveTableOscViewComponent>(projectManager, AUDIO_MODE::MODE_CHORD_PLAYER, shortcutRef);
    wavetableEditorComponent->setBounds(0, 0, 600, 400);
    
    popupWavetableWindow = std::make_unique<PopupFFTWindow>("Wavetable Editor - Chord Player", wavetableEditorComponent.get(), Colours::black, DocumentWindow::allButtons, true);
    popupWavetableWindow ->centreWithSize(600, 400);
    popupWavetableWindow ->setVisible(false);
    popupWavetableWindow ->setResizable(true, true);
    
    paramIndexBase =  UNIT_1_CHORD_PLAYER_SHORTCUT_IS_ACTIVE + ((UNIT_2_CHORD_PLAYER_SHORTCUT_IS_ACTIVE - UNIT_1_CHORD_PLAYER_SHORTCUT_IS_ACTIVE) * shortcutRef);
    
    printf("paramIndex = %i", paramIndexBase);
    
}

LissajousChordPlayerSettingsComponent::~LissajousChordPlayerSettingsComponent()
{
    
}


void LissajousChordPlayerSettingsComponent::resized()
{

    // This method is where you should set the bounds of any child
    // components that your component contains..
    
    int shift = 137;
    
    button_Close                    ->setBounds(1407 * scaleFactor, 130 * scaleFactor, 150 * scaleFactor, 28 * scaleFactor);
    button_ChooseChordFromList      ->setBounds(90 * scaleFactor, 261 * scaleFactor, 38 * scaleFactor, 38 * scaleFactor);
    button_ChooseChordFromFrequency ->setBounds(520 * scaleFactor, 261 * scaleFactor, 38 * scaleFactor, 38 * scaleFactor);
    
    button_Default                  ->setBounds(100 * scaleFactor, 695 * scaleFactor, 38 * scaleFactor, 38 * scaleFactor);
    button_Sine                     ->setBounds((420-shift) * scaleFactor, 695 * scaleFactor, 38 * scaleFactor, 38 * scaleFactor);
    button_Triangle                 ->setBounds((660-shift) * scaleFactor, 695 * scaleFactor, 38 * scaleFactor, 38 * scaleFactor);
    button_Square                   ->setBounds((910-shift) * scaleFactor, 695 * scaleFactor, 38 * scaleFactor, 38 * scaleFactor);
    button_Sawtooth                 ->setBounds((1180-shift) * scaleFactor, 695 * scaleFactor, 38 * scaleFactor, 38 * scaleFactor);
    button_Wavetable                ->setBounds((1465-shift) * scaleFactor, 695 * scaleFactor, 38 * scaleFactor, 38 * scaleFactor);
    
    button_WavetableEditor          ->setBounds((1465-shift) * scaleFactor, 613 * scaleFactor, 150 * scaleFactor, 43 * scaleFactor);
    
    button_Add                      ->setBounds(609 * scaleFactor, 1211 * scaleFactor, 341 * scaleFactor, 84 * scaleFactor);
    
    int shiftBack = 8;
    
    button_Multiplication           ->setBounds((1086-shiftBack) * scaleFactor, 325 * scaleFactor, 38 * scaleFactor, 38 * scaleFactor);
    button_Division                 ->setBounds((1317-shiftBack) * scaleFactor, 325 * scaleFactor, 38 * scaleFactor, 38 * scaleFactor);
    button_ManipulateFreq           ->setBounds(1060 * scaleFactor, 266 * scaleFactor, 30 * scaleFactor, 30 * scaleFactor);
    
    int dif = 219;
    
    slider_Amplitude                ->setBounds(428 * scaleFactor, 770 * scaleFactor, 130 * scaleFactor, 158 * scaleFactor);
    slider_Amplitude                ->setTextBoxStyle (Slider::TextBoxBelow, false, 78 * scaleFactor, 28 * scaleFactor);
    
    slider_Attack                   ->setBounds((426+(1 * dif)) * scaleFactor, 770 * scaleFactor, 130 * scaleFactor, 158 * scaleFactor);
    slider_Attack                   ->setTextBoxStyle (Slider::TextBoxBelow, false, 78 * scaleFactor, 28 * scaleFactor);
    
    slider_Decay                    ->setBounds((426+(3 * dif)) * scaleFactor, 770 * scaleFactor, 130 * scaleFactor, 158 * scaleFactor);
    slider_Decay                    ->setTextBoxStyle (Slider::TextBoxBelow, false, 78 * scaleFactor, 28 * scaleFactor);
    
    slider_Sustain                  ->setBounds((426+(2 * dif)) * scaleFactor, 770 * scaleFactor, 130 * scaleFactor, 158 * scaleFactor);
    slider_Sustain                  ->setTextBoxStyle (Slider::TextBoxBelow, false, 78 * scaleFactor, 28 * scaleFactor);
    
    slider_Release                  ->setBounds((426+(4 * dif)) * scaleFactor, 770 * scaleFactor, 130 * scaleFactor, 158 * scaleFactor);
    slider_Release                  ->setTextBoxStyle (Slider::TextBoxBelow, false, 78 * scaleFactor, 28 * scaleFactor);
    
    
    comboBoxKeynote                 ->setBounds(308 * scaleFactor, 326 * scaleFactor, 149 * scaleFactor, 35 * scaleFactor);
    comboBoxChordtype               ->setBounds(308 * scaleFactor, 390 * scaleFactor, 149 * scaleFactor, 35 * scaleFactor);
    comboBoxPlayingInstrument       ->setBounds(372 * scaleFactor, 613 * scaleFactor, 300 * scaleFactor, 43 * scaleFactor);
    
    textEditorRepeat                ->setBounds(460 * scaleFactor, 1108 * scaleFactor, 111 * scaleFactor, 35 * scaleFactor);
    textEditorRepeat->applyFontToAllText(28 * scaleFactor);
    
    textEditorPause                 ->setBounds(1084 * scaleFactor, 1108 * scaleFactor, 111 * scaleFactor, 35 * scaleFactor);
    textEditorPause->applyFontToAllText(28 * scaleFactor);
    
    textEditorLength                ->setBounds(780 * scaleFactor, 1108 * scaleFactor, 111 * scaleFactor, 35 * scaleFactor);
    textEditorLength->applyFontToAllText(28 * scaleFactor);
    
    textEditorInsertFreq            ->setBounds(792 * scaleFactor, 326 * scaleFactor, 119 * scaleFactor, 35 * scaleFactor);
    textEditorInsertFreq->applyFontToAllText(28 * scaleFactor);
    
    textEditorMultiplication        ->setBounds(1128 * scaleFactor, 374 * scaleFactor, 111 * scaleFactor, 35 * scaleFactor);
    textEditorMultiplication->applyFontToAllText(28 * scaleFactor);
    
    textEditorDivision              ->setBounds(1359 * scaleFactor, 374 * scaleFactor, 111 * scaleFactor, 35 * scaleFactor);
    textEditorDivision->applyFontToAllText(28 * scaleFactor);
    
    textEditor_Octave               ->setBounds(1174 * scaleFactor, 472 * scaleFactor, 149 * scaleFactor, 35 * scaleFactor);
    textEditor_Octave->applyFontToAllText(28 * scaleFactor);
    
    
    button_AddCustomChord           ->setBounds(168 * scaleFactor, 508 * scaleFactor, 260 * scaleFactor, 40 * scaleFactor);
    button_Addons                   ->setBounds(126 * scaleFactor, 445 * scaleFactor, 130 * scaleFactor, 40 * scaleFactor);
    
    comboBox_Scales                 ->setBounds(550 * scaleFactor, 500 * scaleFactor, 350 * scaleFactor, 41 * scaleFactor); //(242, 497, 125, 41);
    
    int panicLeftMargin             = 1360 * scaleFactor;
    int panicTopMargin              = 1164 * scaleFactor;
    int panicWidth                  = 180 * scaleFactor;
    int panicHeight                 = panicWidth;
    
    button_Panic                    ->setBounds(panicLeftMargin, panicTopMargin, panicWidth, panicHeight);
    
    addOnPopupComponent             ->setBounds(233 * scaleFactor, 462 * scaleFactor, 357 * scaleFactor, 179 * scaleFactor);
    customChordPopupComponent       ->setBounds(0, 0, 1564 * scaleFactor, 1440 * scaleFactor);

}

void LissajousChordPlayerSettingsComponent::paint (Graphics&g)
{
    g.setColour(Colours::black);
    g.setOpacity(0.88);
    g.fillAll();
    
    g.setOpacity(1.0);
    g.drawImage(mainBackgroundImage, 24*scaleFactor, 94*scaleFactor, 1502*scaleFactor, 1242*scaleFactor, 0, 0, 1502, 1242);
}

void LissajousChordPlayerSettingsComponent::buttonClicked (Button*button)
{
    if (button == button_Close.get())
    {
        closeView();
    }
    else if (button == button_ChooseChordFromList.get())
    {
        projectManager->setLissajousParameter(paramIndexBase + CHORD_SOURCE, false);
    }
    else if (button == button_ChooseChordFromFrequency.get())
    {
        projectManager->setLissajousParameter(paramIndexBase + CHORD_SOURCE, true);
    }
    else if (button == button_Multiplication.get())
    {
        projectManager->setLissajousParameter( paramIndexBase + MULTIPLY_OR_DIVISION, false);
    }
    else if (button == button_Division.get())
    {
        projectManager->setLissajousParameter( paramIndexBase + MULTIPLY_OR_DIVISION, true);
    }
    else if (button == button_ManipulateFreq.get())
    {
        projectManager->setLissajousParameter( paramIndexBase + MANIPULATE_CHOSEN_FREQUENCY, !button_ManipulateFreq->getToggleState());
    }
    else if (button == button_Default.get())
    {
        projectManager->setLissajousParameter( paramIndexBase + WAVEFORM_TYPE, 0);
    }
    else if (button == button_Sine.get())
    {
        projectManager->setLissajousParameter( paramIndexBase + WAVEFORM_TYPE, 1);
    }
    else if (button == button_Triangle.get())
    {
        projectManager->setLissajousParameter( paramIndexBase + WAVEFORM_TYPE, 2);
    }
    else if (button == button_Square.get())
    {
        projectManager->setLissajousParameter( paramIndexBase + WAVEFORM_TYPE, 3);
    }
    else if (button == button_Sawtooth.get())
    {
        projectManager->setLissajousParameter( paramIndexBase + WAVEFORM_TYPE, 4);
    }
    else if (button == button_Wavetable.get())
    {
        projectManager->setLissajousParameter(paramIndexBase + WAVEFORM_TYPE, 5);
    }
    else if (button == button_WavetableEditor.get())
    {
        if (!popupWavetableWindow ->isVisible()) { popupWavetableWindow ->setVisible(true); }
        else popupWavetableWindow ->setVisible(false);
    }
    else if (button == button_Add.get())
    {
        projectManager->setLissajousParameter(paramIndexBase +  SHORTCUT_IS_ACTIVE, true);
        closeView();
//        LissajousCurveComponent* parent = (LissajousCurveComponent*)getParentComponent();

        
    }
    else if (button == button_AddCustomChord.get())
    {
        customChordPopupComponent->openView();
    }
    else if (button == button_Addons.get())
    {
        addOnPopupComponent->setVisible(!addOnPopupComponent->isVisible());
        addOnPopupComponent->syncGUI();
    }
}

void LissajousChordPlayerSettingsComponent::sliderValueChanged (Slider* slider)
{
    if (slider == slider_Amplitude.get())
    {
        projectManager->setLissajousParameter( paramIndexBase + ENV_AMPLITUDE, slider_Amplitude->getValue());
    }
    else if (slider == slider_Attack.get())
    {
        projectManager->setLissajousParameter( paramIndexBase + ENV_ATTACK, slider_Attack->getValue());
    }
    else if (slider == slider_Decay.get())
    {
        projectManager->setLissajousParameter( paramIndexBase + ENV_DECAY, slider_Decay->getValue());
    }
    else if (slider == slider_Sustain.get())
    {
        projectManager->setLissajousParameter( paramIndexBase + ENV_SUSTAIN, slider_Sustain->getValue());
    }
    else if (slider == slider_Release.get())
    {
        projectManager->setLissajousParameter( paramIndexBase + ENV_RELEASE, slider_Release->getValue());
    }
}

void LissajousChordPlayerSettingsComponent::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == comboBoxKeynote.get())
    {
        projectManager->setLissajousParameter( paramIndexBase + KEYNOTE, comboBoxKeynote->getSelectedId());
    }
    else if (comboBoxThatHasChanged == comboBoxPlayingInstrument.get())
    {
        int selectedID = comboBoxPlayingInstrument->getSelectedId();
        projectManager->setLissajousParameter( paramIndexBase + INSTRUMENT_TYPE, selectedID);
    }
    else if (comboBoxThatHasChanged == comboBoxChordtype.get())
    {
        projectManager->setLissajousParameter( paramIndexBase + CHORD_TYPE, comboBoxChordtype->getSelectedId());
    }
    else if (comboBoxThatHasChanged == comboBox_Scales.get())
    {
        projectManager->setLissajousParameter( paramIndexBase + CHORDPLAYER_SCALE, comboBox_Scales->getSelectedId());
    }
}

void LissajousChordPlayerSettingsComponent::textEditorReturnKeyPressed (TextEditor&editor)
{
    if (&editor == textEditorPause.get())
    {
        int value = editor.getText().getIntValue();

        // bounds check
        if (value >= PAUSE_MAX) {  value = PAUSE_MAX; }
        if (value <= PAUSE_MIN) { value = PAUSE_MIN; }

        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setLissajousParameter( paramIndexBase + NUM_PAUSE, value);
    }
    else if (&editor == textEditorRepeat.get())
    {
        int value = editor.getText().getIntValue();
        
        // bounds check
        if (value >= REPEAT_MAX) {  value = REPEAT_MAX; }
        if (value <= REPEAT_MIN) { value = REPEAT_MIN; }
        
        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setLissajousParameter( paramIndexBase + NUM_REPEATS, value);
    }
    else if (&editor == textEditorLength.get())
    {
        int value = editor.getText().getIntValue();
        
        // bounds check
        if (value >= LENGTH_MAX) {  value = LENGTH_MAX; }
        if (value <= LENGTH_MIN) { value = LENGTH_MIN; }
        
        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setLissajousParameter( paramIndexBase + NUM_DURATION, value);
    }
    else if (&editor == textEditorDivision.get())
    {
        float value = editor.getText().getFloatValue();
        
        // bounds check
        if (value >= DIVISION_MAXIMUM) {  value = DIVISION_MAXIMUM; }
        if (value <= DIVISION_MINIMUM) { value = DIVISION_MINIMUM; }
        
        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setLissajousParameter( paramIndexBase + DIVISION_VALUE, value);
    }
    else if (&editor == textEditorMultiplication.get())
    {
        float value = editor.getText().getFloatValue();
        
        // bounds check
        if (value >= MULTIPLY_MAXIMUM) {  value = MULTIPLY_MAXIMUM; }
        if (value <= MULTIPLY_MINIMUM) {  value = MULTIPLY_MINIMUM; }
        
        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setLissajousParameter( paramIndexBase + MULTIPLY_VALUE, value);
    }
    else if (&editor == textEditorInsertFreq.get())
    {
        float value = editor.getText().getFloatValue();
        
        // bounds check
        if (value >= INSERT_FREQ_MAX) {  value = INSERT_FREQ_MAX; }
        if (value <= INSERT_FREQ_MIN) {  value = INSERT_FREQ_MIN; }
        
        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setLissajousParameter( paramIndexBase + INSERT_FREQUENCY, value);
    }
    else if (&editor == textEditor_Octave.get())
    {
        float value = editor.getText().getFloatValue();
        
        // bounds check
        if (value >= OCTAVE_MAX) {  value = OCTAVE_MAX; }
        if (value <= OCTAVE_MIN) {  value = OCTAVE_MIN; }
        
        String newVal(value); editor.setText(newVal);

        projectManager->setLissajousParameter( paramIndexBase + OCTAVE, value);
    }
    
    Component::unfocusAllComponents();
}

void LissajousChordPlayerSettingsComponent::openView()
{
//    for (int i = 0; i < TOTAL_NUM_CHORD_PLAYER_SHORTCUT_PARAMS; i++)
//    {
//        syncUI();
//    }
    
    setVisible(true);
}

void LissajousChordPlayerSettingsComponent::closeView()
{
    addOnPopupComponent->setVisible(false);
    
    setVisible(false);
    isSynced = false;
}

void LissajousChordPlayerSettingsComponent::syncUI(int paramIndex)
{
    if (paramIndex == (paramIndexBase + WAVEFORM_TYPE))
    {
        // act as toggle between 5 buttons WAVEFORM_TYPE
        int waveformType =  projectManager->getLissajousParameter( paramIndexBase + WAVEFORM_TYPE).operator int();
        switch (waveformType) {
        case 0:
        {
            button_Default  ->setToggleState(true,  dontSendNotification);
            button_Sine     ->setToggleState(false, dontSendNotification);
            button_Triangle ->setToggleState(false, dontSendNotification);
            button_Square   ->setToggleState(false, dontSendNotification);
            button_Sawtooth ->setToggleState(false, dontSendNotification);
            button_Wavetable->setToggleState(false, dontSendNotification);
        }
            break;
        case 1:
        {
            button_Default  ->setToggleState(false, dontSendNotification);
            button_Sine     ->setToggleState(true,  dontSendNotification);
            button_Triangle ->setToggleState(false, dontSendNotification);
            button_Square   ->setToggleState(false, dontSendNotification);
            button_Sawtooth ->setToggleState(false, dontSendNotification);
            button_Wavetable->setToggleState(false, dontSendNotification);
        }
            break;
        case 2:
        {
            button_Default  ->setToggleState(false, dontSendNotification);
            button_Sine     ->setToggleState(false, dontSendNotification);
            button_Triangle ->setToggleState(true,  dontSendNotification);
            button_Square   ->setToggleState(false, dontSendNotification);
            button_Sawtooth ->setToggleState(false, dontSendNotification);
            button_Wavetable->setToggleState(false, dontSendNotification);
        }
            break;
        case 3:
        {
            button_Default  ->setToggleState(false, dontSendNotification);
            button_Sine     ->setToggleState(false, dontSendNotification);
            button_Triangle ->setToggleState(false, dontSendNotification);
            button_Square   ->setToggleState(true,  dontSendNotification);
            button_Sawtooth ->setToggleState(false, dontSendNotification);
            button_Wavetable->setToggleState(false, dontSendNotification);
        }
            break;
        case 4:
        {
            button_Default  ->setToggleState(false, dontSendNotification);
            button_Sine     ->setToggleState(false, dontSendNotification);
            button_Triangle ->setToggleState(false, dontSendNotification);
            button_Square   ->setToggleState(false, dontSendNotification);
            button_Sawtooth ->setToggleState(true,  dontSendNotification);
            button_Wavetable->setToggleState(false, dontSendNotification);
        }
            break;
        case 5:
        {
            button_Default  ->setToggleState(false, dontSendNotification);
            button_Sine     ->setToggleState(false, dontSendNotification);
            button_Triangle ->setToggleState(false, dontSendNotification);
            button_Square   ->setToggleState(false, dontSendNotification);
            button_Sawtooth ->setToggleState(false, dontSendNotification);
            button_Wavetable->setToggleState(true, dontSendNotification);
        }
            break;
            
        default:
        {
            button_Default  ->setToggleState(false, dontSendNotification);
            button_Sine     ->setToggleState(true,  dontSendNotification);
            button_Triangle ->setToggleState(false, dontSendNotification);
            button_Square   ->setToggleState(false, dontSendNotification);
            button_Sawtooth ->setToggleState(false, dontSendNotification);
            button_Wavetable->setToggleState(false, dontSendNotification);
        }
            
            break;
        }
    }
    else if (paramIndex == (paramIndexBase + INSTRUMENT_TYPE))
    {
        int instrumentType = projectManager->getLissajousParameter( paramIndexBase + INSTRUMENT_TYPE).operator int();
        comboBoxPlayingInstrument->setSelectedId(instrumentType);
    }
    else if (paramIndex == (paramIndexBase + ENV_AMPLITUDE))
    {
        slider_Amplitude    ->setValue(projectManager->getLissajousParameter( paramIndexBase + ENV_AMPLITUDE).operator double());
    }
    else if (paramIndex == (paramIndexBase + ENV_ATTACK))
    {
        slider_Attack       ->setValue(projectManager->getLissajousParameter( paramIndexBase + ENV_ATTACK).operator double());
    }
    else if (paramIndex == (paramIndexBase + ENV_DECAY))
    {
        slider_Sustain      ->setValue(projectManager->getLissajousParameter( paramIndexBase + ENV_SUSTAIN).operator double());
    }
    else if (paramIndex == (paramIndexBase + ENV_SUSTAIN))
    {
        slider_Decay        ->setValue(projectManager->getLissajousParameter( paramIndexBase + ENV_DECAY).operator double());
    }
    else if (paramIndex == (paramIndexBase + ENV_RELEASE))
    {
        slider_Release      ->setValue(projectManager->getLissajousParameter( paramIndexBase + ENV_RELEASE).operator double());
    }
    else if (paramIndex == (paramIndexBase + CHORD_SOURCE))
    {
        //==============================================================
        bool chordSource = projectManager->getLissajousParameter( paramIndexBase + CHORD_SOURCE).operator bool();
        if (chordSource)
        {
            button_ChooseChordFromList      ->setToggleState(false, dontSendNotification);
            button_ChooseChordFromFrequency ->setToggleState(true, dontSendNotification);
        }
        else
        {
            button_ChooseChordFromList      ->setToggleState(true, dontSendNotification);
            button_ChooseChordFromFrequency ->setToggleState(false, dontSendNotification);
        }
        //==============================================================
    }
    else if (paramIndex == (paramIndexBase + KEYNOTE))
    {
        // Combobox
        int keyNote = projectManager->getLissajousParameter( paramIndexBase + KEYNOTE).operator int();
        comboBoxKeynote ->setSelectedId(keyNote);  //KEYNOTE
    }
    else if (paramIndex == (paramIndexBase + CHORD_TYPE))
    {
        int chordType = projectManager->getLissajousParameter( paramIndexBase + CHORD_TYPE).operator int();
        comboBoxChordtype ->setSelectedId(chordType) ; //CHORD_TYPE
    }

    else if (paramIndex == (paramIndexBase + CHORDPLAYER_SCALE))
    {
        int scale = projectManager->getLissajousParameter(paramIndexBase + CHORDPLAYER_SCALE).operator int();
        comboBox_Scales ->setSelectedId(scale);
    }
    

    else if (paramIndex == (paramIndexBase + MANIPULATE_CHOSEN_FREQUENCY))
    {
        //NUM_REPEATS
        
        bool manipulateFreq = projectManager->getLissajousParameter( paramIndexBase + MANIPULATE_CHOSEN_FREQUENCY).operator bool();
        button_ManipulateFreq->setToggleState(manipulateFreq, dontSendNotification); //MANIPULATE_CHOSEN_FREQUENCY,
    }
    else if (paramIndex == (paramIndexBase + MULTIPLY_OR_DIVISION))
    {
        bool multOrDivide = projectManager->getLissajousParameter( paramIndexBase + MULTIPLY_OR_DIVISION).operator bool();
        if (multOrDivide)
        {
            button_Multiplication   ->setToggleState(false, dontSendNotification);
            button_Division         ->setToggleState(true, dontSendNotification);
        }
        else
        {
            button_Multiplication   ->setToggleState(true, dontSendNotification);
            button_Division         ->setToggleState(false, dontSendNotification);
        }
    }
    else if (paramIndex == (paramIndexBase + INSERT_FREQUENCY))
    {
        // TextEntryBoxes
        float insertFreqVal = projectManager->getLissajousParameter( paramIndexBase + INSERT_FREQUENCY).operator float();
        String freqString(insertFreqVal); freqString.append("Hz", 2);
        textEditorInsertFreq->setText(freqString);
    }
    else if (paramIndex == (paramIndexBase + MULTIPLY_VALUE))
    {
        float multVal = projectManager->getLissajousParameter( paramIndexBase + MULTIPLY_VALUE).operator float();
        String multString(multVal);
        textEditorMultiplication->setText(multString);
    }
    else if (paramIndex == (paramIndexBase + DIVISION_VALUE))
    {
        float divVal = projectManager->getLissajousParameter(paramIndexBase +  DIVISION_VALUE).operator float();
        String divString(divVal);
        textEditorDivision->setText(divString);
    }
    else if (paramIndex == (paramIndexBase + NUM_PAUSE))
    {
        int pauseVal = projectManager->getLissajousParameter( paramIndexBase + NUM_PAUSE).operator int();
        String pauseString(pauseVal);
        pauseString.append("ms", 2);
        textEditorPause->setText(pauseString);
    }
    else if (paramIndex == (paramIndexBase + NUM_DURATION))
    {
        int lengthVal = projectManager->getLissajousParameter( paramIndexBase + NUM_DURATION).operator int();
        String lengthString(lengthVal);
        lengthString.append("ms", 2);
        textEditorLength->setText(lengthString);
    }
    else if (paramIndex == (paramIndexBase + NUM_REPEATS))
    {
        int numRepeats = projectManager->getLissajousParameter( paramIndexBase + NUM_REPEATS).operator int();
        String repeatsString(numRepeats);
        textEditorRepeat->setText(repeatsString);
    }
    else if (paramIndex == (paramIndexBase + OCTAVE))
    {
        float octave = projectManager->getLissajousParameter(paramIndexBase + OCTAVE).operator float();
        String octaveString(octave); textEditor_Octave->setText(octaveString);
    }
    
    // addons popup
    addOnPopupComponent->syncGUI();
    
    // custom chord unit
    customChordPopupComponent->syncGUI();
    
    isSynced = true;
}

void LissajousChordPlayerSettingsComponent::updateMinMaxSettings(int paramIndex)
{
    switch (paramIndex) {
        case AMPLITUDE_MIN:
        {
            double min = projectManager->getProjectSettingsParameter(AMPLITUDE_MIN);
            double max = projectManager->getProjectSettingsParameter(AMPLITUDE_MAX);
            
            slider_Amplitude->setRange(min, max, 0.1);
        }
            break;
            
        case AMPLITUDE_MAX:
        {
            double min = projectManager->getProjectSettingsParameter(AMPLITUDE_MIN);
            double max = projectManager->getProjectSettingsParameter(AMPLITUDE_MAX);
            
            slider_Amplitude->setRange(min, max, 0.1);
        }
            break;
        case ATTACK_MIN:
        {
            double min = projectManager->getProjectSettingsParameter(ATTACK_MIN);
            double max = projectManager->getProjectSettingsParameter(ATTACK_MAX);
            
            slider_Attack->setRange(min, max, 1);
        }
            break;
        case ATTACK_MAX:
        {
            double min = projectManager->getProjectSettingsParameter(ATTACK_MIN);
            double max = projectManager->getProjectSettingsParameter(ATTACK_MAX);
            
            slider_Attack->setRange(min, max, 1);
        }
            break;
        case DECAY_MIN:
        {
            double min = projectManager->getProjectSettingsParameter(DECAY_MIN);
            double max = projectManager->getProjectSettingsParameter(DECAY_MAX);
            
            slider_Decay->setRange(min, max, 1);
        }
            break;
        case DECAY_MAX:
        {
            double min = projectManager->getProjectSettingsParameter(DECAY_MIN);
            double max = projectManager->getProjectSettingsParameter(DECAY_MAX);
            
            slider_Decay->setRange(min, max, 1);
        }
            break;
        case SUSTAIN_MIN:
        {
            double min = projectManager->getProjectSettingsParameter(SUSTAIN_MIN);
            double max = projectManager->getProjectSettingsParameter(SUSTAIN_MAX);
            
            slider_Sustain->setRange(min, max, 0.1);
        }
            
        case SUSTAIN_MAX:
        {
            double min = projectManager->getProjectSettingsParameter(SUSTAIN_MIN);
            double max = projectManager->getProjectSettingsParameter(SUSTAIN_MAX);
            
            slider_Sustain->setRange(min, max, 0.1);
        }
            break;
        case RELEASE_MIN:
        {
            double min = projectManager->getProjectSettingsParameter(RELEASE_MIN);
            double max = projectManager->getProjectSettingsParameter(RELEASE_MAX);
            
            slider_Release->setRange(min, max, 1);
        }
            break;
        case RELEASE_MAX:
        {
            double min = projectManager->getProjectSettingsParameter(RELEASE_MIN);
            double max = projectManager->getProjectSettingsParameter(RELEASE_MAX);
            
            slider_Release->setRange(min, max, 1);
        }
            break;
            
        default: break;
    }
}






//==============================================================================
LissajousFrequencyPlayerSettingsComponent::LissajousFrequencyPlayerSettingsComponent(ProjectManager * pm, int axisRef)
{
    projectManager  = pm;
    shortcutRef     = axisRef;
    // LookAndFeels
    
    
    
    mainBackgroundImage         = ImageCache::getFromMemory(BinaryData::FrequencyPlayerSettingsBackgroundNew_png, BinaryData::FrequencyPlayerSettingsBackgroundNew_pngSize);
    imageCloseButton            = ImageCache::getFromMemory(BinaryData::CloseButton2x_png, BinaryData::CloseButton2x_pngSize);
    imageBlueButtonNormal       = ImageCache::getFromMemory(BinaryData::BlueButton_Normal_png, BinaryData::BlueButton_Normal_pngSize);
    imageBlueButtonSelected     = ImageCache::getFromMemory(BinaryData::BlueButton_Selected_png, BinaryData::BlueButton_Selected_pngSize);
    imageAddButton              = ImageCache::getFromMemory(BinaryData::AddButton2x_png, BinaryData::AddButton2x_pngSize);
    
    imageBlueCheckButtonNormal  = ImageCache::getFromMemory(BinaryData::Button_Checkbox_Normal_Max_png, BinaryData::Button_Checkbox_Normal_Max_pngSize);
    imageBlueCheckButtonSelected= ImageCache::getFromMemory(BinaryData::Button_Checkbox_Selected_Max_png, BinaryData::Button_Checkbox_Selected_Max_pngSize);
    
    
    button_Close = std::make_unique<ImageButton>();
    button_Close->setTriggeredOnMouseDown(true);
    button_Close->setImages (false, true, true,
                             imageCloseButton, 0.999f, Colour (0x00000000),
                             Image(), 1.000f, Colour (0x00000000),
                             imageCloseButton, 0.75, Colour (0x00000000));
    button_Close->addListener(this);
    button_Close->setBounds(1408, 275, 150, 28);
    addAndMakeVisible(button_Close.get());
    
    // toggle between these two
    button_ChooseSpecificFrequency = std::make_unique<ImageButton>();
    button_ChooseSpecificFrequency->setTriggeredOnMouseDown(true);
    button_ChooseSpecificFrequency->setImages (false, true, true,
                                           imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                           Image(), 1.000f, Colour (0x00000000),
                                           imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_ChooseSpecificFrequency->addListener(this);
    button_ChooseSpecificFrequency->setBounds(90, 360, 38, 38);
    addAndMakeVisible(button_ChooseSpecificFrequency.get());
    
    
    button_ChooseRangeOfFrequencies = std::make_unique<ImageButton>();
    button_ChooseRangeOfFrequencies->setTriggeredOnMouseDown(true);
    button_ChooseRangeOfFrequencies->setImages (false, true, true,
                                                imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                                Image(), 1.000f, Colour (0x00000000),
                                                imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_ChooseRangeOfFrequencies->addListener(this);
    button_ChooseRangeOfFrequencies->setBounds(628, 360, 38, 38);
    addAndMakeVisible(button_ChooseRangeOfFrequencies.get());
    
    
    int shift = 0;
    int waveY = 547;
    int shiftBack = 20;
    int mulDivY = 410;
    
    
    // act as toggle between 5 buttons
    button_Default = std::make_unique<ImageButton>();
    button_Default->setTriggeredOnMouseDown(true);
    button_Default->setImages (false, true, true,
                               imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                               Image(), 1.000f, Colour (0x00000000),
                               imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_Default->addListener(this);
    button_Default->setBounds(260, waveY, 38, 38);
//    addAndMakeVisible(button_Default.get());
    

    button_Sine = std::make_unique<ImageButton>();
    button_Sine->setTriggeredOnMouseDown(true);
    button_Sine->setImages (false, true, true,
                            imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                            Image(), 1.000f, Colour (0x00000000),
                            imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_Sine->addListener(this);
    button_Sine->setBounds(420-shift-10+6, waveY, 38, 38);
    addAndMakeVisible(button_Sine.get());
    
    button_Triangle = std::make_unique<ImageButton>();
    button_Triangle->setTriggeredOnMouseDown(true);
    button_Triangle->setImages (false, true, true,
                                imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_Triangle->addListener(this);
    button_Triangle->setBounds(660-shift+16-16, waveY, 38, 38);
    addAndMakeVisible(button_Triangle.get());
    
    button_Square = std::make_unique<ImageButton>();
    button_Square->setTriggeredOnMouseDown(true);
    button_Square->setImages (false, true, true,
                              imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                              Image(), 1.000f, Colour (0x00000000),
                              imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_Square->addListener(this);
    button_Square->setBounds(910-shift, waveY, 38, 38);
    addAndMakeVisible(button_Square.get());
    
    button_Sawtooth = std::make_unique<ImageButton>();
    button_Sawtooth->setTriggeredOnMouseDown(true);
    button_Sawtooth->setImages (false, true, true,
                                imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_Sawtooth->addListener(this);
    button_Sawtooth->setBounds(1180-shift, waveY, 38, 38);
    addAndMakeVisible(button_Sawtooth.get());
    
    button_Wavetable = std::make_unique<ImageButton>();
    button_Wavetable->setTriggeredOnMouseDown(true);
    button_Wavetable->setImages (false, true, true,
                                                imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                                Image(), 1.000f, Colour (0x00000000),
                                                imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_Wavetable->addListener(this);
    addAndMakeVisible(button_Wavetable.get());
    
    
    button_WavetableEditor = std::make_unique<TextButton>("");
    button_WavetableEditor->setButtonText("WT Editor");
    button_WavetableEditor->setLookAndFeel(&lookAndFeel);
    button_WavetableEditor->addListener(this);
    addAndMakeVisible(button_WavetableEditor.get());

    
    button_Add = std::make_unique<ImageButton>();
    button_Add->setTriggeredOnMouseDown(true);
    button_Add->setImages (false, true, true,
                           imageAddButton, 0.999f, Colour (0x00000000),
                           Image(), 1.000f, Colour (0x00000000),
                           imageAddButton, 0.888, Colour (0x00000000));
    button_Add->addListener(this);
    button_Add->setBounds(609, 980, 341, 84);
    addAndMakeVisible(button_Add.get());
    
    
    button_ManipulateFreq = std::make_unique<ImageButton>();
    button_ManipulateFreq->setTriggeredOnMouseDown(true);
    button_ManipulateFreq->setImages (false, true, true,
                                      imageBlueCheckButtonNormal, 0.999f, Colour (0x00000000),
                                      Image(), 1.000f, Colour (0x00000000),
                                      imageBlueCheckButtonSelected, 1.0, Colour (0x00000000));
    button_ManipulateFreq->addListener(this);
    button_ManipulateFreq->setBounds(1054, 357, 30, 30);
    addAndMakeVisible(button_ManipulateFreq.get());
    
    button_Log = std::make_unique<ImageButton>();
    button_Log->setTriggeredOnMouseDown(true);
    button_Log->setImages (false, true, true,
                                      imageBlueCheckButtonNormal, 0.999f, Colour (0x00000000),
                                      Image(), 1.000f, Colour (0x00000000),
                                      imageBlueCheckButtonSelected, 1.0, Colour (0x00000000));
    button_Log->addListener(this);
    button_Log->setBounds(632, 460, 30, 30);
    addAndMakeVisible(button_Log.get());
    
    
    button_Multiplication = std::make_unique<ImageButton>();
    button_Multiplication->setTriggeredOnMouseDown(true);
    button_Multiplication->setImages (false, true, true,
                                      imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                      Image(), 1.000f, Colour (0x00000000),
                                      imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_Multiplication->addListener(this);
    button_Multiplication->setBounds(1086-shiftBack, mulDivY, 38, 38);
    addAndMakeVisible(button_Multiplication.get());
    
    button_Division = std::make_unique<ImageButton>();
    button_Division->setTriggeredOnMouseDown(true);
    button_Division->setImages (false, true, true,
                                imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_Division->addListener(this);
    button_Division->setBounds(1317-shiftBack, mulDivY, 38, 38);
    addAndMakeVisible(button_Division.get());
    
    // Font
    Typeface::Ptr AssistantLight        = Typeface::createSystemTypefaceFor(BinaryData::AssistantLight_ttf, BinaryData::AssistantLight_ttfSize);
    Typeface::Ptr AssistantSemiBold     = Typeface::createSystemTypefaceFor(BinaryData::AssistantSemiBold_ttf, BinaryData::AssistantSemiBold_ttfSize);
    Font fontSemiBold(AssistantSemiBold);
    Font fontLight(AssistantLight);
    
    fontSemiBold.setHeight(33 * scaleFactor);
    fontLight.setHeight(33 * scaleFactor);
    
    int playY = 894;


    textEditorRepeat = std::make_unique<TextEditor>("");
    textEditorRepeat->setReturnKeyStartsNewLine(false);
    textEditorRepeat->setInputRestrictions(2, "0123456789");
    textEditorRepeat->setMultiLine(false);
    textEditorRepeat->setText("1");
    textEditorRepeat->addListener(this);
    textEditorRepeat->setLookAndFeel(&lookAndFeel);
    textEditorRepeat->setJustification(Justification::centred);
    textEditorRepeat->setFont(fontSemiBold);
    textEditorRepeat->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditorRepeat->applyFontToAllText(fontSemiBold);
    textEditorRepeat->applyColourToAllText(Colours::lightgrey);
    textEditorRepeat->setBounds(466, playY, 111, 35);
    addAndMakeVisible(textEditorRepeat.get());
    
    textEditorPause = std::make_unique<TextEditor>("");
    textEditorPause->setReturnKeyStartsNewLine(false);
    textEditorPause->setInputRestrictions(5, "0123456789");
    textEditorPause->setMultiLine(false);
    textEditorPause->setText("1000");
    textEditorPause->addListener(this);
    textEditorPause->setLookAndFeel(&lookAndFeel);
    textEditorPause->setJustification(Justification::centred);
    textEditorPause->setFont(fontSemiBold);
    textEditorPause->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditorPause->applyFontToAllText(fontSemiBold);
    textEditorPause->applyColourToAllText(Colours::lightgrey);
    textEditorPause->setBounds(1094, playY, 111, 35);
    addAndMakeVisible(textEditorPause.get());
    
    
    textEditorLength = std::make_unique<TextEditor>("");
    textEditorLength->setReturnKeyStartsNewLine(false);
    textEditorLength->setMultiLine(false);
    textEditorLength->setInputRestrictions(5, "0123456789");
    textEditorLength->setText("1000");
    textEditorLength->addListener(this);
    textEditorLength->setLookAndFeel(&lookAndFeel);
    textEditorLength->setJustification(Justification::centred);
    textEditorLength->setFont(fontSemiBold);
    textEditorLength->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditorLength->applyFontToAllText(fontSemiBold);
    textEditorLength->applyColourToAllText(Colours::lightgrey);
    textEditorLength->setBounds(794, playY, 111, 35);
    addAndMakeVisible(textEditorLength.get());
    

    
    // TextEntryBoxes
    textEditorInsertFreq = std::make_unique<TextEditor>("");
    textEditorInsertFreq->setReturnKeyStartsNewLine(false);
    textEditorInsertFreq->setMultiLine(false);
    textEditorInsertFreq->setInputRestrictions(12, "0123456789.");
    textEditorInsertFreq->setText("432");
    textEditorInsertFreq->addListener(this);
    textEditorInsertFreq->setLookAndFeel(&lookAndFeel);
    textEditorInsertFreq->setJustification(Justification::centred);
    textEditorInsertFreq->setFont(fontSemiBold);
    textEditorInsertFreq->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditorInsertFreq->applyFontToAllText(fontSemiBold);
    textEditorInsertFreq->applyColourToAllText(Colours::lightgrey);
    textEditorInsertFreq->setBounds(476, 360, 96, 35);
    addAndMakeVisible(textEditorInsertFreq.get());
    
    
    int y = 402;
    textEditorFreqFrom = std::make_unique<TextEditor>("");
    textEditorFreqFrom->setReturnKeyStartsNewLine(false);
    textEditorFreqFrom->setMultiLine(false);
    textEditorFreqFrom->setInputRestrictions(12, "0123456789.");
    textEditorFreqFrom->setText("432");
    textEditorFreqFrom->addListener(this);
    textEditorFreqFrom->setLookAndFeel(&lookAndFeel);
    textEditorFreqFrom->setJustification(Justification::centred);
    textEditorFreqFrom->setFont(fontSemiBold);
    textEditorFreqFrom->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditorFreqFrom->applyFontToAllText(fontSemiBold);
    textEditorFreqFrom->applyColourToAllText(Colours::lightgrey);
    textEditorFreqFrom->setBounds(708, y, 96, 35);
    addAndMakeVisible(textEditorFreqFrom.get());
    
    textEditorFreqTo = std::make_unique<TextEditor>("");
    textEditorFreqTo->setReturnKeyStartsNewLine(false);
    textEditorFreqTo->setMultiLine(false);
    textEditorFreqTo->setInputRestrictions(12, "0123456789.");
    textEditorFreqTo->setText("432");
    textEditorFreqTo->addListener(this);
    textEditorFreqTo->setLookAndFeel(&lookAndFeel);
    textEditorFreqTo->setJustification(Justification::centred);
    textEditorFreqTo->setFont(fontSemiBold);
    textEditorFreqTo->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditorFreqTo->applyFontToAllText(fontSemiBold);
    textEditorFreqTo->applyColourToAllText(Colours::lightgrey);
    textEditorFreqTo->setBounds(910, y, 96, 35);
    addAndMakeVisible(textEditorFreqTo.get());
    
    textEditorRangeLength = std::make_unique<TextEditor>("");
    textEditorRangeLength->setReturnKeyStartsNewLine(false);
    textEditorRangeLength->setMultiLine(false);
    textEditorRangeLength->setInputRestrictions(5, "0123456789.");
    textEditorRangeLength->setText("1000ms");
    textEditorRangeLength->addListener(this);
    textEditorRangeLength->setLookAndFeel(&lookAndFeel);
    textEditorRangeLength->setJustification(Justification::centred);
    textEditorRangeLength->setFont(fontSemiBold);
    textEditorRangeLength->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditorRangeLength->applyFontToAllText(fontSemiBold);
    textEditorRangeLength->applyColourToAllText(Colours::lightgrey);
    textEditorRangeLength->setBounds(907, 456, 96, 35);
    addAndMakeVisible(textEditorRangeLength.get());
    
    int multTy= 458;
    int shiftxx = 12;
    textEditorMultiplication = std::make_unique<TextEditor>("");
    textEditorMultiplication->setReturnKeyStartsNewLine(false);
    textEditorMultiplication->setMultiLine(false);
    textEditorMultiplication->setInputRestrictions(10, "0123456789.");
    textEditorMultiplication->setText("1.0");
    textEditorMultiplication->addListener(this);
    textEditorMultiplication->setLookAndFeel(&lookAndFeel);
    textEditorMultiplication->setJustification(Justification::centred);
    textEditorMultiplication->setFont(fontSemiBold);
    textEditorMultiplication->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditorMultiplication->applyFontToAllText(fontSemiBold);
    textEditorMultiplication->applyColourToAllText(Colours::lightgrey);
    textEditorMultiplication->setBounds(1128-shiftxx, multTy, 111, 35);
    addAndMakeVisible(textEditorMultiplication.get());
    
    textEditorDivision = std::make_unique<TextEditor>("");
    textEditorDivision->setReturnKeyStartsNewLine(false);
    textEditorDivision->setMultiLine(false);
    textEditorDivision->setInputRestrictions(10, "0123456789.");
    textEditorDivision->setText("1.0");
    textEditorDivision->addListener(this);
    textEditorDivision->setLookAndFeel(&lookAndFeel);
    textEditorDivision->setJustification(Justification::centred);
    textEditorDivision->setFont(fontSemiBold);
    textEditorDivision->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditorDivision->applyFontToAllText(fontSemiBold);
    textEditorDivision->applyColourToAllText(Colours::lightgrey);
    textEditorDivision->setBounds(1359-shiftxx, multTy, 111, 35);
    addAndMakeVisible(textEditorDivision.get());
    
    
    // Knobs
    int sliderY = 622;
    int sliderXShift = 14;
    int dif = 219;
    
    
    
    
    slider_Amplitude = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_AMPLITUDE);
    slider_Amplitude    ->setRange (0, 1.0, 0);
    slider_Amplitude    ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_Amplitude    ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_Amplitude    ->addListener (this);
    slider_Amplitude    ->setTextValueSuffix("%");
    slider_Amplitude    ->setBounds(428+sliderXShift, sliderY, 130, 158);
    slider_Amplitude    ->setNumDecimalPlacesToDisplay(1);
    slider_Amplitude    ->setLookAndFeel(&lookAndFeel);
    
    addAndMakeVisible(slider_Amplitude.get());
    
    
    slider_Attack = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_ATTACK);
    slider_Attack       ->setRange (0, 1.0, 0);
    slider_Attack       ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_Attack       ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_Attack       ->addListener (this);
    slider_Attack       ->setTextValueSuffix("Ms");
    slider_Attack       ->setBounds(426+(1 * dif)+sliderXShift, sliderY, 130, 158);
    slider_Attack       ->setNumDecimalPlacesToDisplay(0);
    slider_Attack       ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_Attack.get());
    
    slider_Decay = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_DECAY);
    slider_Decay        ->setRange (0, 1.0, 0);
    slider_Decay        ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_Decay        ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_Decay        ->addListener (this);
    slider_Decay        ->setTextValueSuffix("Ms");
    slider_Decay        ->setBounds(426+(3 * dif)+sliderXShift, sliderY, 130, 158);
    slider_Decay        ->setNumDecimalPlacesToDisplay(0);
    slider_Decay        ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_Decay.get());
    
    slider_Sustain = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_SUSTAIN);
    slider_Sustain      ->setRange (0, 1.0, 0);
    slider_Sustain      ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_Sustain      ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_Sustain      ->addListener (this);
    slider_Sustain      ->setTextValueSuffix("%");
    slider_Sustain      ->setBounds(426+(2 * dif)+sliderXShift, sliderY, 130, 158);
    slider_Sustain      ->setNumDecimalPlacesToDisplay(2);
    slider_Sustain        ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_Sustain.get());
    
    slider_Release = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_RELEASE);
    slider_Release      ->setRange (0, 1.0, 0);
    slider_Release      ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_Release      ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_Release      ->addListener (this);
    slider_Release      ->setTextValueSuffix("Ms");
    slider_Release      ->setBounds(426+(4 * dif)+sliderXShift, sliderY, 130, 158);
    slider_Release      ->setNumDecimalPlacesToDisplay(0);
    slider_Release      ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_Release.get());
    

    for (int i = AMPLITUDE_MIN; i <= RELEASE_MAX; i++)
    {
        updateMinMaxSettings(i);
    }
    
    wavetableEditorComponent = std::make_unique<WaveTableOscViewComponent>(projectManager, AUDIO_MODE::MODE_FREQUENCY_PLAYER, shortcutRef);
    wavetableEditorComponent->setBounds(0, 0, 600, 400);
    
    // need to update the contents of each editor for each shortcut.....
    
    popupWavetableWindow = std::make_unique<PopupFFTWindow>("Wavetable Editor - Frequency Player", wavetableEditorComponent.get(), Colours::black, DocumentWindow::allButtons, true);
    popupWavetableWindow ->centreWithSize(600, 400);
    popupWavetableWindow ->setVisible(false);
    popupWavetableWindow ->setResizable(true, true);
    
    paramIndexBase =  UNIT_1_FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE + ((UNIT_2_FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE - UNIT_1_FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE) * shortcutRef);
    
    printf("paramIndex = %i", paramIndexBase);

}

LissajousFrequencyPlayerSettingsComponent::~LissajousFrequencyPlayerSettingsComponent() { }


void LissajousFrequencyPlayerSettingsComponent::resized()
{
    button_Close->setBounds(1408 * scaleFactor, 275 * scaleFactor, 150 * scaleFactor, 28 * scaleFactor);
    
    int shift = 0;
    int waveY = 578 * scaleFactor;
    int shiftBack = 20;
    int mulDivY = 410 * scaleFactor;
    
    button_Sine->setBounds(260 * scaleFactor, waveY, 38 * scaleFactor, 38 * scaleFactor);
    button_Triangle->setBounds(500 * scaleFactor, waveY, 38 * scaleFactor, 38 * scaleFactor);
    button_Square->setBounds(740  * scaleFactor, waveY, 38 * scaleFactor, 38 * scaleFactor);
    button_Sawtooth->setBounds(1020  * scaleFactor, waveY, 38 * scaleFactor, 38 * scaleFactor);
    
    shift = 154;
    button_Wavetable                ->setBounds(1292 * scaleFactor, waveY, 38 * scaleFactor, 38 * scaleFactor);
    
    button_WavetableEditor          ->setBounds((1465-shift) * scaleFactor, 536 * scaleFactor, 150 * scaleFactor, 38 * scaleFactor);
    
    button_Add->setBounds(609 * scaleFactor, 980 * scaleFactor, 341 * scaleFactor, 84 * scaleFactor);
    button_ManipulateFreq->setBounds(1054 * scaleFactor, 357 * scaleFactor, 30 * scaleFactor, 30 * scaleFactor);
    button_Log->setBounds(632 * scaleFactor, 460 * scaleFactor, 30 * scaleFactor, 30 * scaleFactor);
    button_Multiplication->setBounds((1086-shiftBack) * scaleFactor, mulDivY, 38 * scaleFactor, 38 * scaleFactor);
    button_Division->setBounds((1317-shiftBack) * scaleFactor, mulDivY, 38 * scaleFactor, 38 * scaleFactor);
    
    button_ChooseSpecificFrequency->setBounds(90 * scaleFactor, 360 * scaleFactor, 38 * scaleFactor, 38 * scaleFactor);
    
    button_ChooseRangeOfFrequencies->setBounds(628 * scaleFactor, 360 * scaleFactor, 38 * scaleFactor, 38 * scaleFactor);
    
    int playY = 894 * scaleFactor;
    int fontSize = 24;
    
    textEditorRepeat->setBounds(466 * scaleFactor, playY, 111 * scaleFactor, 35 * scaleFactor);
    textEditorRepeat->applyFontToAllText(fontSize * scaleFactor);
    
    textEditorPause->setBounds(1094 * scaleFactor, playY, 111 * scaleFactor, 35 * scaleFactor);
    textEditorPause->applyFontToAllText(fontSize * scaleFactor);
    
    textEditorLength->setBounds(794 * scaleFactor, playY, 111 * scaleFactor, 35 * scaleFactor);
    textEditorLength->applyFontToAllText(fontSize * scaleFactor);
    
    textEditorInsertFreq->setBounds(454 * scaleFactor, 360 * scaleFactor, 150 * scaleFactor, 35 * scaleFactor);
    textEditorInsertFreq->applyFontToAllText(fontSize * scaleFactor);
    
    int y = 402 * scaleFactor;
    textEditorFreqFrom->setBounds(678 * scaleFactor, y, 150 * scaleFactor, 35 * scaleFactor);
    textEditorFreqFrom->applyFontToAllText(fontSize * scaleFactor);
    
    textEditorFreqTo->setBounds(860 * scaleFactor, y, 150 * scaleFactor, 35 * scaleFactor);
    textEditorFreqTo->applyFontToAllText(fontSize * scaleFactor);
    
    textEditorRangeLength->setBounds(890 * scaleFactor, 456 * scaleFactor, 110 * scaleFactor, 35 * scaleFactor);
    textEditorRangeLength->applyFontToAllText(fontSize * scaleFactor);
    
    int multTy= 458 * scaleFactor;
    int shiftxx = 12;
    
    textEditorMultiplication->setBounds((1128-shiftxx) * scaleFactor, multTy, 111 * scaleFactor, 35 * scaleFactor);
    textEditorMultiplication->applyFontToAllText(fontSize * scaleFactor);
    
    textEditorDivision->setBounds((1359-shiftxx) * scaleFactor, multTy, 111 * scaleFactor, 35 * scaleFactor);
    textEditorDivision->applyFontToAllText(fontSize * scaleFactor);
    
    int sliderY = 622 * scaleFactor;
    int sliderXShift = 14;
    int dif = 219;
    
    slider_Amplitude    ->setTextBoxStyle (Slider::TextBoxBelow, false, 78 * scaleFactor, 28 * scaleFactor);
    slider_Amplitude    ->setBounds((428+sliderXShift) * scaleFactor, sliderY, 130 * scaleFactor, 158 * scaleFactor);
    
    slider_Attack       ->setTextBoxStyle (Slider::TextBoxBelow, false, 78 * scaleFactor, 28 * scaleFactor);
    slider_Attack       ->setBounds((426+(1 * dif)+sliderXShift) * scaleFactor, sliderY, 130 * scaleFactor, 158 * scaleFactor);
    
    slider_Decay        ->setBounds((426+(3 * dif)+sliderXShift) * scaleFactor, sliderY, 130 * scaleFactor, 158 * scaleFactor);
    slider_Decay        ->setTextBoxStyle (Slider::TextBoxBelow, false, 78 * scaleFactor, 28 * scaleFactor);
    
    slider_Sustain      ->setBounds((426+(2 * dif)+sliderXShift) * scaleFactor, sliderY, 130 * scaleFactor, 158 * scaleFactor);
    slider_Sustain      ->setTextBoxStyle (Slider::TextBoxBelow, false, 78 * scaleFactor, 28 * scaleFactor);
    
    slider_Release      ->setBounds((426+(4 * dif)+sliderXShift) * scaleFactor, sliderY, 130 * scaleFactor, 158 * scaleFactor);
    slider_Release      ->setTextBoxStyle (Slider::TextBoxBelow, false, 78 * scaleFactor, 28 * scaleFactor);
    
}

void LissajousFrequencyPlayerSettingsComponent::paint (Graphics&g)
{
    g.setColour(Colours::black);
    g.setOpacity(0.88);
    g.fillAll();
    
    g.setOpacity(1.0);
    g.drawImage(mainBackgroundImage, 52 * scaleFactor, 252 * scaleFactor, 1463 * scaleFactor, 847 * scaleFactor, 0, 0, 1463, 847);
}

void LissajousFrequencyPlayerSettingsComponent::updateMinMaxSettings(int paramIndex)
{
    switch (paramIndex) {
        case AMPLITUDE_MIN:
        {
            double min = projectManager->getProjectSettingsParameter(AMPLITUDE_MIN);
            double max = projectManager->getProjectSettingsParameter(AMPLITUDE_MAX);
            
            slider_Amplitude->setRange(min, max, 0.1);
        }
            break;
            
        case AMPLITUDE_MAX:
        {
            double min = projectManager->getProjectSettingsParameter(AMPLITUDE_MIN);
            double max = projectManager->getProjectSettingsParameter(AMPLITUDE_MAX);
            
            slider_Amplitude->setRange(min, max, 0.1);
        }
            break;
        case ATTACK_MIN:
        {
            double min = projectManager->getProjectSettingsParameter(ATTACK_MIN);
            double max = projectManager->getProjectSettingsParameter(ATTACK_MAX);
            
            slider_Attack->setRange(min, max, 1);
        }
            break;
        case ATTACK_MAX:
        {
            double min = projectManager->getProjectSettingsParameter(ATTACK_MIN);
            double max = projectManager->getProjectSettingsParameter(ATTACK_MAX);
            
            slider_Attack->setRange(min, max, 1);
        }
            break;
        case DECAY_MIN:
        {
            double min = projectManager->getProjectSettingsParameter(DECAY_MIN);
            double max = projectManager->getProjectSettingsParameter(DECAY_MAX);
            
            slider_Decay->setRange(min, max, 1);
        }
            break;
        case DECAY_MAX:
        {
            double min = projectManager->getProjectSettingsParameter(DECAY_MIN);
            double max = projectManager->getProjectSettingsParameter(DECAY_MAX);
            
            slider_Decay->setRange(min, max, 1);
        }
            break;
        case SUSTAIN_MIN:
        {
            double min = projectManager->getProjectSettingsParameter(SUSTAIN_MIN);
            double max = projectManager->getProjectSettingsParameter(SUSTAIN_MAX);
            
            slider_Sustain->setRange(min, max, 0.1);
        }
            
        case SUSTAIN_MAX:
        {
            double min = projectManager->getProjectSettingsParameter(SUSTAIN_MIN);
            double max = projectManager->getProjectSettingsParameter(SUSTAIN_MAX);
            
            slider_Sustain->setRange(min, max, 0.1);
        }
            break;
        case RELEASE_MIN:
        {
            double min = projectManager->getProjectSettingsParameter(RELEASE_MIN);
            double max = projectManager->getProjectSettingsParameter(RELEASE_MAX);
            
            slider_Release->setRange(min, max, 1);
        }
            break;
        case RELEASE_MAX:
        {
            double min = projectManager->getProjectSettingsParameter(RELEASE_MIN);
            double max = projectManager->getProjectSettingsParameter(RELEASE_MAX);
            
            slider_Release->setRange(min, max, 1);
        }
            break;
            
        default: break;
    }
}

void LissajousFrequencyPlayerSettingsComponent::buttonClicked (Button*button)
{
    if (button == button_Close.get())
    {
        closeView();
        
        popupWavetableWindow->setVisible(false);
    }
    else if (button == button_ChooseSpecificFrequency.get())
    {
        projectManager->setLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_FREQ_SOURCE, false);
    }
    else if (button == button_ChooseRangeOfFrequencies.get())
    {
        projectManager->setLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_FREQ_SOURCE, true);
    }
    else if (button == button_Multiplication.get())
    {
        projectManager->setLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_MULTIPLY_OR_DIVISION, false);
    }
    else if (button == button_Division.get())
    {
        projectManager->setLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_MULTIPLY_OR_DIVISION, true);
    }
    else if (button == button_ManipulateFreq.get())
    {
        projectManager->setLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_MANIPULATE_CHOSEN_FREQUENCY, !button_ManipulateFreq->getToggleState());
    }
    else if (button == button_Default.get())
    {
        projectManager->setLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_WAVEFORM_TYPE, 0);
    }
    else if (button == button_Sine.get())
    {
        projectManager->setLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_WAVEFORM_TYPE, 1);
    }
    else if (button == button_Triangle.get())
    {
        projectManager->setLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_WAVEFORM_TYPE, 2);
    }
    else if (button == button_Square.get())
    {
        projectManager->setLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_WAVEFORM_TYPE, 3);
    }
    else if (button == button_Sawtooth.get())
    {
        projectManager->setLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_WAVEFORM_TYPE, 4);
    }
    else if (button == button_Wavetable.get())
    {
        projectManager->setLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_WAVEFORM_TYPE, 5);
    }
    else if (button == button_WavetableEditor.get())
    {
        if (!popupWavetableWindow ->isVisible())
        {
            popupWavetableWindow ->setVisible(true);
            wavetableEditorComponent->setShortcut(shortcutRef);
        }
        else popupWavetableWindow ->setVisible(false);
    }
    else if (button == button_Log.get())
    {
        projectManager->setLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_RANGE_LOG, !button_Log->getToggleState());
    }
    else if (button == button_Add.get())
    {
        closeView();
        
        projectManager->setLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE, true);
        
//        projectManager->logFileWriter->processLog_FrequencyPlayer_Parameters();
    }
}

void LissajousFrequencyPlayerSettingsComponent::sliderValueChanged (Slider* slider)
{
    if (slider == slider_Amplitude.get())
    {
        projectManager->setLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_AMPLITUDE, slider_Amplitude->getValue());
    }
    else if (slider == slider_Attack.get())
    {
        projectManager->setLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_ATTACK, slider_Attack->getValue());
    }
    else if (slider == slider_Decay.get())
    {
        projectManager->setLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_DECAY, slider_Decay->getValue());
    }
    else if (slider == slider_Sustain.get())
    {
        projectManager->setLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_SUSTAIN, slider_Sustain->getValue());
    }
    else if (slider == slider_Release.get())
    {
        projectManager->setLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_RELEASE, slider_Release->getValue());
    }
}

void LissajousFrequencyPlayerSettingsComponent::textEditorReturnKeyPressed (TextEditor&editor)
{
    if (&editor == textEditorPause.get())
    {
        int value = editor.getText().getIntValue();
        
        // bounds check
        if (value >= PAUSE_MAX) {  value = PAUSE_MAX; }
        if (value <= PAUSE_MIN) {  value = PAUSE_MIN; }
        
        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_NUM_PAUSE, value);
    }
    else if (&editor == textEditorRepeat.get())
    {
        int value = editor.getText().getIntValue();
        
        // bounds check
        if (value >= REPEAT_MAX) {  value = REPEAT_MAX; }
        if (value <= REPEAT_MIN) { value = REPEAT_MIN; }
        
        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_NUM_REPEATS, value);
    }
    else if (&editor == textEditorLength.get())
    {
        int value = editor.getText().getIntValue();
        
        // bounds check
        if (value >= LENGTH_MAX) {  value = LENGTH_MAX; }
        if (value <= LENGTH_MIN) { value = LENGTH_MIN; }
        
        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_NUM_DURATION, value);
    }
    else if (&editor == textEditorDivision.get())
    {
        double value = editor.getText().getDoubleValue();
        
        // bounds check
        if (value >= DIVISION_MAXIMUM) {  value = DIVISION_MAXIMUM; }
        if (value <= DIVISION_MINIMUM) { value = DIVISION_MINIMUM; }
        
//        String newVal(value, 3, false); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_DIVISION_VALUE, value);
    }
    else if (&editor == textEditorMultiplication.get())
    {
        double value = editor.getText().getDoubleValue();
        
        // bounds check
        if (value >= MULTIPLY_MAXIMUM) {  value = MULTIPLY_MAXIMUM; }
        if (value <= MULTIPLY_MINIMUM) {  value = MULTIPLY_MINIMUM; }
        
//        String newVal(value, 3, false); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_MULTIPLY_VALUE, value);
    }
    else if (&editor == textEditorInsertFreq.get())
    {
        double value = editor.getText().getDoubleValue();
        
        // bounds check
        if (value >= FREQUENCY_MAX) {  value = FREQUENCY_MAX; }
        if (value <= FREQUENCY_MIN) {  value = FREQUENCY_MIN; }
        
//        String newVal(value, 3, false); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_CHOOSE_FREQ, value);
    }
    else if (&editor == textEditorFreqFrom.get())
    {
        double value = editor.getText().getDoubleValue();
        
        // bounds check
        if (value >= FREQUENCY_MAX) {  value = FREQUENCY_MAX; }
        if (value <= FREQUENCY_MIN) {  value = FREQUENCY_MIN; }
        
//        String newVal(value, 3, false); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_RANGE_MIN, value);
    }
    else if (&editor == textEditorFreqTo.get())
    {
        double value = editor.getText().getDoubleValue();
        
        // bounds check
        if (value >= FREQUENCY_MAX) {  value = FREQUENCY_MAX; }
        if (value <= FREQUENCY_MIN) {  value = FREQUENCY_MIN; }
        
//        String newVal(value, 3, false); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_RANGE_MAX, value);
    }
    else if (&editor == textEditorRangeLength.get())
    {
        float value = editor.getText().getFloatValue();
        
        // bounds check
        if (value >= LENGTH_MAX) {  value = LENGTH_MAX; }
        if (value <= LENGTH_MIN) {  value = LENGTH_MIN; }
        
        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_RANGE_LENGTH, value);
    }
    
    Component::unfocusAllComponents();
}

void LissajousFrequencyPlayerSettingsComponent::openView(int shortcut)
{
    shortcutRef = shortcut;
    
    for (int i = 0; i < TOTAL_NUM_FREQUENCY_PLAYER_SHORTCUT_PARAMS; i++)
    {
        syncUI(paramIndexBase + i);
    }
    
    setVisible(true);
}

void LissajousFrequencyPlayerSettingsComponent::closeView()
{
    setVisible(false);
    isSynced = false;
}

void LissajousFrequencyPlayerSettingsComponent::syncUI(int paramIndex)
{
    if (paramIndex == (paramIndexBase + FREQUENCY_PLAYER_FREQ_SOURCE))
    {
        // grabs
        // CHORD_SOURCE
        //==============================================================
        bool source = projectManager->getLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_FREQ_SOURCE).operator bool();
        if (source)
        {
            button_ChooseSpecificFrequency      ->setToggleState(false, dontSendNotification);
            button_ChooseRangeOfFrequencies ->setToggleState(true, dontSendNotification);
        }
        else
        {
            button_ChooseSpecificFrequency      ->setToggleState(true, dontSendNotification);
            button_ChooseRangeOfFrequencies ->setToggleState(false, dontSendNotification);
        }
        //==============================================================
    }
    else if (paramIndex == (paramIndexBase + FREQUENCY_PLAYER_AMPLITUDE))
    {
        slider_Amplitude    ->setValue(projectManager->getLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_AMPLITUDE).operator double());
    }
    else if (paramIndex == (paramIndexBase + FREQUENCY_PLAYER_ATTACK))
    {
        slider_Attack       ->setValue(projectManager->getLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_ATTACK).operator double());
    }
    else if (paramIndex == (paramIndexBase + FREQUENCY_PLAYER_SUSTAIN))
    {
        slider_Sustain      ->setValue(projectManager->getLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_SUSTAIN).operator double());
    }
    else if (paramIndex == (paramIndexBase + FREQUENCY_PLAYER_DECAY))
    {
        slider_Decay        ->setValue(projectManager->getLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_DECAY).operator double());
    }
    else if (paramIndex == (paramIndexBase + FREQUENCY_PLAYER_RELEASE))
    {
        slider_Release      ->setValue(projectManager->getLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_RELEASE).operator double());
    }
    
    else if (paramIndex == (paramIndexBase + FREQUENCY_PLAYER_WAVEFORM_TYPE))
    {
        // act as toggle between 5 buttons WAVEFORM_TYPE
        int waveformType =  projectManager->getLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_WAVEFORM_TYPE).operator int();
        switch (waveformType) {
            case 0:
            {
                button_Default  ->setToggleState(true,  dontSendNotification);
                button_Sine     ->setToggleState(false, dontSendNotification);
                button_Triangle ->setToggleState(false, dontSendNotification);
                button_Square   ->setToggleState(false, dontSendNotification);
                button_Sawtooth ->setToggleState(false, dontSendNotification);
                button_Wavetable->setToggleState(false, dontSendNotification);
            }
                break;
            case 1:
            {
                button_Default  ->setToggleState(false, dontSendNotification);
                button_Sine     ->setToggleState(true,  dontSendNotification);
                button_Triangle ->setToggleState(false, dontSendNotification);
                button_Square   ->setToggleState(false, dontSendNotification);
                button_Sawtooth ->setToggleState(false, dontSendNotification);
                button_Wavetable->setToggleState(false, dontSendNotification);
            }
                break;
            case 2:
            {
                button_Default  ->setToggleState(false, dontSendNotification);
                button_Sine     ->setToggleState(false, dontSendNotification);
                button_Triangle ->setToggleState(true,  dontSendNotification);
                button_Square   ->setToggleState(false, dontSendNotification);
                button_Sawtooth ->setToggleState(false, dontSendNotification);
                button_Wavetable->setToggleState(false, dontSendNotification);
            }
                break;
            case 3:
            {
                button_Default  ->setToggleState(false, dontSendNotification);
                button_Sine     ->setToggleState(false, dontSendNotification);
                button_Triangle ->setToggleState(false, dontSendNotification);
                button_Square   ->setToggleState(true,  dontSendNotification);
                button_Sawtooth ->setToggleState(false, dontSendNotification);
                button_Wavetable->setToggleState(false, dontSendNotification);
            }
                break;
            case 4:
            {
                button_Default  ->setToggleState(false, dontSendNotification);
                button_Sine     ->setToggleState(false, dontSendNotification);
                button_Triangle ->setToggleState(false, dontSendNotification);
                button_Square   ->setToggleState(false, dontSendNotification);
                button_Sawtooth ->setToggleState(true,  dontSendNotification);
                button_Wavetable->setToggleState(false, dontSendNotification);
            }
                break;
            case 5:
            {
                button_Default  ->setToggleState(false, dontSendNotification);
                button_Sine     ->setToggleState(false, dontSendNotification);
                button_Triangle ->setToggleState(false, dontSendNotification);
                button_Square   ->setToggleState(false, dontSendNotification);
                button_Sawtooth ->setToggleState(false, dontSendNotification);
                button_Wavetable->setToggleState(true, dontSendNotification);
            }
                break;
                
            default:
            {
                button_Default  ->setToggleState(false, dontSendNotification);
                button_Sine     ->setToggleState(true,  dontSendNotification);
                button_Triangle ->setToggleState(false, dontSendNotification);
                button_Square   ->setToggleState(false, dontSendNotification);
                button_Sawtooth ->setToggleState(false, dontSendNotification);
                button_Wavetable->setToggleState(false, dontSendNotification);
            }
                
                break;
        }
    }
    else if (paramIndex == (paramIndexBase + FREQUENCY_PLAYER_MANIPULATE_CHOSEN_FREQUENCY))
    {
        bool manipulateFreq = projectManager->getLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_MANIPULATE_CHOSEN_FREQUENCY).operator bool();
        button_ManipulateFreq->setToggleState(manipulateFreq, dontSendNotification); //MANIPULATE_CHOSEN_FREQUENCY,
    }
    else if (paramIndex == (paramIndexBase + FREQUENCY_PLAYER_MULTIPLY_OR_DIVISION))
    {
        bool multOrDivide = projectManager->getLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_MULTIPLY_OR_DIVISION).operator bool();
        if (multOrDivide)
        {
            button_Multiplication   ->setToggleState(false, dontSendNotification);
            button_Division         ->setToggleState(true, dontSendNotification);
        }
        else
        {
            button_Multiplication   ->setToggleState(true, dontSendNotification);
            button_Division         ->setToggleState(false, dontSendNotification);
        }
    }
    else if (paramIndex == (paramIndexBase + FREQUENCY_PLAYER_RANGE_LOG))
    {
        bool logBool = projectManager->getLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_RANGE_LOG).operator bool();
        button_Log->setToggleState(logBool, dontSendNotification);
    }
    else if (paramIndex == (paramIndexBase + FREQUENCY_PLAYER_CHOOSE_FREQ))
    {
        double insertFreqVal = projectManager->getLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_CHOOSE_FREQ).operator double();
        String freqString(insertFreqVal, 3, false); /*freqString.append("Hz", 2);*/
        textEditorInsertFreq->setText(freqString);
    }
    else if (paramIndex == (paramIndexBase + FREQUENCY_PLAYER_RANGE_MIN))
    {
        double minFreqVal = projectManager->getLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_RANGE_MIN).operator double();
        String minFreqString(minFreqVal, 3, false); /*minFreqString.append("Hz", 2);*/
        textEditorFreqFrom->setText(minFreqString);
    }
    else if (paramIndex == (paramIndexBase + FREQUENCY_PLAYER_RANGE_MAX))
    {
        double maxFreqVal = projectManager->getLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_RANGE_MAX).operator double();
        String maxFreqString(maxFreqVal, 3, false); /*maxFreqString.append("Hz", 2);*/
        textEditorFreqTo->setText(maxFreqString);
    }
    
    else if (paramIndex == (paramIndexBase + FREQUENCY_PLAYER_MULTIPLY_VALUE))
    {
        double multVal = projectManager->getLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_MULTIPLY_VALUE).operator double();
        String multString(multVal, 3, false);
        textEditorMultiplication->setText(multString);
    }
    else if (paramIndex == (paramIndexBase + FREQUENCY_PLAYER_DIVISION_VALUE))
    {
        double divVal = projectManager->getLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_DIVISION_VALUE).operator double();
        String divString(divVal, 3, false);
        textEditorDivision->setText(divString);
    }
    else if (paramIndex == (paramIndexBase + FREQUENCY_PLAYER_NUM_PAUSE))
    {
        int pauseVal = projectManager->getLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_NUM_PAUSE).operator int();
        String pauseString(pauseVal);
        pauseString.append("ms", 2);
        textEditorPause->setText(pauseString);
    }
    
    else if (paramIndex == (paramIndexBase + FREQUENCY_PLAYER_NUM_DURATION))
    {
        int lengthVal = projectManager->getLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_NUM_DURATION).operator int();
        String lengthString(lengthVal);
        lengthString.append("ms", 2);
        textEditorLength->setText(lengthString);
    }
    else if (paramIndex == (paramIndexBase + FREQUENCY_PLAYER_NUM_REPEATS))
    {
        int numRepeats = projectManager->getLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_NUM_REPEATS).operator int();
        String repeatsString(numRepeats);
        textEditorRepeat->setText(repeatsString);
    }
    else if (paramIndex == (paramIndexBase + FREQUENCY_PLAYER_RANGE_LENGTH))
    {
        int rangeLengthVal = projectManager->getLissajousParameter(paramIndexBase + FREQUENCY_PLAYER_RANGE_LENGTH).operator int();
        String lengthString2(rangeLengthVal);
        lengthString2.append("ms", 2);
        textEditorRangeLength->setText(lengthString2);
    }

    isSynced = true;
}

//void LissajousFrequencyPlayerSettingsComponent::syncUI(int paramIndex)
//{
//    // need to sort this out... needs axis ref, then multiplier to get the correct param Index...
//
//
//    // grabs
//    // CHORD_SOURCE
//    //==============================================================
//    bool source = projectManager->getFrequencyPlayerParameter(shortcutRef, FREQUENCY_PLAYER_FREQ_SOURCE).operator bool();
//    if (source)
//    {
//        button_ChooseSpecificFrequency      ->setToggleState(false, dontSendNotification);
//        button_ChooseRangeOfFrequencies ->setToggleState(true, dontSendNotification);
//    }
//    else
//    {
//        button_ChooseSpecificFrequency      ->setToggleState(true, dontSendNotification);
//        button_ChooseRangeOfFrequencies ->setToggleState(false, dontSendNotification);
//    }
//    //==============================================================
//
//    slider_Amplitude    ->setValue(projectManager->getFrequencyPlayerParameter(shortcutRef, FREQUENCY_PLAYER_AMPLITUDE).operator double());
//    slider_Attack       ->setValue(projectManager->getFrequencyPlayerParameter(shortcutRef, FREQUENCY_PLAYER_ATTACK).operator double());
//    slider_Sustain      ->setValue(projectManager->getFrequencyPlayerParameter(shortcutRef, FREQUENCY_PLAYER_SUSTAIN).operator double());
//    slider_Decay        ->setValue(projectManager->getFrequencyPlayerParameter(shortcutRef, FREQUENCY_PLAYER_DECAY).operator double());
//    slider_Release      ->setValue(projectManager->getFrequencyPlayerParameter(shortcutRef, FREQUENCY_PLAYER_RELEASE).operator double());
//
//    // act as toggle between 5 buttons WAVEFORM_TYPE
//    int waveformType =  projectManager->getFrequencyPlayerParameter(shortcutRef, FREQUENCY_PLAYER_WAVEFORM_TYPE).operator int();
//    switch (waveformType) {
//        case 0:
//        {
//            button_Default  ->setToggleState(true,  dontSendNotification);
//            button_Sine     ->setToggleState(false, dontSendNotification);
//            button_Triangle ->setToggleState(false, dontSendNotification);
//            button_Square   ->setToggleState(false, dontSendNotification);
//            button_Sawtooth ->setToggleState(false, dontSendNotification);
//            button_Wavetable->setToggleState(false, dontSendNotification);
//        }
//            break;
//        case 1:
//        {
//            button_Default  ->setToggleState(false, dontSendNotification);
//            button_Sine     ->setToggleState(true,  dontSendNotification);
//            button_Triangle ->setToggleState(false, dontSendNotification);
//            button_Square   ->setToggleState(false, dontSendNotification);
//            button_Sawtooth ->setToggleState(false, dontSendNotification);
//            button_Wavetable->setToggleState(false, dontSendNotification);
//        }
//            break;
//        case 2:
//        {
//            button_Default  ->setToggleState(false, dontSendNotification);
//            button_Sine     ->setToggleState(false, dontSendNotification);
//            button_Triangle ->setToggleState(true,  dontSendNotification);
//            button_Square   ->setToggleState(false, dontSendNotification);
//            button_Sawtooth ->setToggleState(false, dontSendNotification);
//            button_Wavetable->setToggleState(false, dontSendNotification);
//        }
//            break;
//        case 3:
//        {
//            button_Default  ->setToggleState(false, dontSendNotification);
//            button_Sine     ->setToggleState(false, dontSendNotification);
//            button_Triangle ->setToggleState(false, dontSendNotification);
//            button_Square   ->setToggleState(true,  dontSendNotification);
//            button_Sawtooth ->setToggleState(false, dontSendNotification);
//            button_Wavetable->setToggleState(false, dontSendNotification);
//        }
//            break;
//        case 4:
//        {
//            button_Default  ->setToggleState(false, dontSendNotification);
//            button_Sine     ->setToggleState(false, dontSendNotification);
//            button_Triangle ->setToggleState(false, dontSendNotification);
//            button_Square   ->setToggleState(false, dontSendNotification);
//            button_Sawtooth ->setToggleState(true,  dontSendNotification);
//            button_Wavetable->setToggleState(false, dontSendNotification);
//        }
//            break;
//        case 5:
//        {
//            button_Default  ->setToggleState(false, dontSendNotification);
//            button_Sine     ->setToggleState(false, dontSendNotification);
//            button_Triangle ->setToggleState(false, dontSendNotification);
//            button_Square   ->setToggleState(false, dontSendNotification);
//            button_Sawtooth ->setToggleState(false, dontSendNotification);
//            button_Wavetable->setToggleState(true, dontSendNotification);
//        }
//            break;
//
//        default:
//        {
//            button_Default  ->setToggleState(false, dontSendNotification);
//            button_Sine     ->setToggleState(true,  dontSendNotification);
//            button_Triangle ->setToggleState(false, dontSendNotification);
//            button_Square   ->setToggleState(false, dontSendNotification);
//            button_Sawtooth ->setToggleState(false, dontSendNotification);
//            button_Wavetable->setToggleState(false, dontSendNotification);
//        }
//
//            break;
//    }
//
//    //NUM_REPEATS
//
//    int pause = projectManager->getFrequencyPlayerParameter(shortcutRef, FREQUENCY_PLAYER_NUM_PAUSE).operator int();
//    //    comboBoxPause ->setSelectedId(pause);  //NUM_PAUSE
//
//    bool manipulateFreq = projectManager->getFrequencyPlayerParameter(shortcutRef, FREQUENCY_PLAYER_MANIPULATE_CHOSEN_FREQUENCY).operator bool();
//    button_ManipulateFreq->setToggleState(manipulateFreq, dontSendNotification); //MANIPULATE_CHOSEN_FREQUENCY,
//
//    bool multOrDivide = projectManager->getFrequencyPlayerParameter(shortcutRef, FREQUENCY_PLAYER_MULTIPLY_OR_DIVISION).operator bool();
//    if (multOrDivide)
//    {
//        button_Multiplication   ->setToggleState(false, dontSendNotification);
//        button_Division         ->setToggleState(true, dontSendNotification);
//    }
//    else
//    {
//        button_Multiplication   ->setToggleState(true, dontSendNotification);
//        button_Division         ->setToggleState(false, dontSendNotification);
//    }
//
//    // Button Log needs im-ple,enting ******
//    bool logBool = projectManager->getFrequencyPlayerParameter(shortcutRef, FREQUENCY_PLAYER_RANGE_LOG).operator bool();
//    button_Log->setToggleState(logBool, dontSendNotification);
//
//
//    // TextEntryBoxes
//    double insertFreqVal = projectManager->getFrequencyPlayerParameter(shortcutRef, FREQUENCY_PLAYER_CHOOSE_FREQ).operator double();
//    String freqString(insertFreqVal, 3, false); /*freqString.append("Hz", 2);*/
//    textEditorInsertFreq->setText(freqString);
//
//    double minFreqVal = projectManager->getFrequencyPlayerParameter(shortcutRef, FREQUENCY_PLAYER_RANGE_MIN).operator double();
//    String minFreqString(minFreqVal, 3, false); /*minFreqString.append("Hz", 2);*/
//    textEditorFreqFrom->setText(minFreqString);
//
//    double maxFreqVal = projectManager->getFrequencyPlayerParameter(shortcutRef, FREQUENCY_PLAYER_RANGE_MAX).operator double();
//    String maxFreqString(maxFreqVal, 3, false); /*maxFreqString.append("Hz", 2);*/
//    textEditorFreqTo->setText(maxFreqString);
//
//    double multVal = projectManager->getFrequencyPlayerParameter(shortcutRef, FREQUENCY_PLAYER_MULTIPLY_VALUE).operator double();
//    String multString(multVal, 3, false);
//    textEditorMultiplication->setText(multString);
//
//    double divVal = projectManager->getFrequencyPlayerParameter(shortcutRef, FREQUENCY_PLAYER_DIVISION_VALUE).operator double();
//    String divString(divVal, 3, false);
//    textEditorDivision->setText(divString);
//
//    int pauseVal = projectManager->getFrequencyPlayerParameter(shortcutRef, FREQUENCY_PLAYER_NUM_PAUSE).operator int();
//    String pauseString(pauseVal);
//    pauseString.append("ms", 2);
//    textEditorPause->setText(pauseString);
//
//    int lengthVal = projectManager->getFrequencyPlayerParameter(shortcutRef, FREQUENCY_PLAYER_NUM_DURATION).operator int();
//    String lengthString(lengthVal);
//    lengthString.append("ms", 2);
//    textEditorLength->setText(lengthString);
//
//    int numRepeats = projectManager->getFrequencyPlayerParameter(shortcutRef, FREQUENCY_PLAYER_NUM_REPEATS).operator int();
//    String repeatsString(numRepeats);
//    textEditorRepeat->setText(repeatsString);
//
//    int rangeLengthVal = projectManager->getFrequencyPlayerParameter(shortcutRef, FREQUENCY_PLAYER_RANGE_LENGTH).operator int();
//    String lengthString2(rangeLengthVal);
//    lengthString2.append("ms", 2);
//    textEditorRangeLength->setText(lengthString2);
//
//    isSynced = true;
//}
