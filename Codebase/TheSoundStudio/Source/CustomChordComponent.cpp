/*
  ==============================================================================

    CustomChordComponent.cpp
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "CustomChordComponent.h"

//==============================================================================


CustomChordNoteComponent::CustomChordNoteComponent(int ref, ProjectManager * pm)
{
    projectManager  = pm;
    shortcutRef     = 0;
    noteRef         = ref; // 1 of max 12..
    
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
    addAndMakeVisible(backgroundImageComp.get());
    
    // containers
    containerView_Active = std::make_unique<Component>();
    addAndMakeVisible(containerView_Active.get());
    
    containerView_Inactive = std::make_unique<Component>();
    addAndMakeVisible(containerView_Inactive.get());
    
    containerView_Details = std::make_unique<Component>();
    addAndMakeVisible(containerView_Details.get());
    
    
    // Inactive view state 0
    button_AddActive = std::make_unique<ImageButton>();
    button_AddActive->setTriggeredOnMouseDown(true);
    button_AddActive->setImages (false, true, true,
                                 imageAddIcon, 0.999f, Colour (0x00000000),
                                 Image(), 1.000f, Colour (0x00000000),
                                 imageAddIcon, 0.75, Colour (0x00000000));
    button_AddActive->addListener(this);
    containerView_Inactive->addAndMakeVisible(button_AddActive.get());
    
    // Add Note / Active Settings view State 1
    button_AddNewNote = std::make_unique<ImageButton>();
    button_AddNewNote->setTriggeredOnMouseDown(true);
    button_AddNewNote->setImages (false, true, true,
                                  imageAddNote, 0.999f, Colour (0x00000000),
                                  Image(), 1.000f, Colour (0x00000000),
                                  imageAddNote, 0.75, Colour (0x00000000));
    button_AddNewNote->addListener(this);
    containerView_Active->addAndMakeVisible(button_AddNewNote.get());
    
    comboBox_Note = std::make_unique<ComboBox>();
    comboBox_Note->setSelectedId(0);
    comboBox_Note->addListener(this);
    comboBox_Note->setLookAndFeel(&lookAndFeel);
    containerView_Active->addAndMakeVisible(comboBox_Note.get());
    
    comboBox_Octave = std::make_unique<ComboBox>();
    comboBox_Octave->addItemList(ProjectStrings::getOctaveArray(), 1);
    comboBox_Octave->setSelectedId(0);
    comboBox_Octave->addListener(this);
    comboBox_Octave->setLookAndFeel(&lookAndFeel);
    containerView_Active->addAndMakeVisible(comboBox_Octave.get());
    
    
    // Details View with Delete state 2
    
    label_NoteValue = std::make_unique<Label>("", "C#Major");
    label_NoteValue->setFont(fontLight);
    label_NoteValue->setJustificationType(Justification::right);
    containerView_Details->addAndMakeVisible(label_NoteValue.get());
    
    
    label_OctaveValue  = std::make_unique<Label>("", "Minor");
    label_OctaveValue->setFont(fontLight);
    label_OctaveValue->setJustificationType(Justification::right);
    containerView_Details->addAndMakeVisible(label_OctaveValue.get());
    
    label_FrequencyLabel  = std::make_unique<Label>("", "432Hz");
    label_FrequencyLabel->setFont(fontLight);
    label_FrequencyLabel->setJustificationType(Justification::right);
//    containerView_Details->addAndMakeVisible(label_FrequencyLabel.get());
    addAndMakeVisible(label_FrequencyLabel.get());
    
    // Amplitude controls
    label_Amplitude = std::make_unique<Label>("", "100%");
    label_Amplitude->setFont(fontLight);
    label_Amplitude->setJustificationType(Justification::centred);
    label_Amplitude->setEditable(true);
    label_Amplitude->onTextChange = [this]() {
        String text = label_Amplitude->getText();
        // Remove % sign if present
        text = text.trimCharactersAtEnd("%");
        float amplitude = text.getFloatValue();
        // Clamp between 0 and 100
        amplitude = jmin(100.0f, jmax(0.0f, amplitude));
        // Update the parameter
        projectManager->setChordPlayerParameter(shortcutRef, CUSTOM_CHORD_AMPLITUDE_1 + noteRef, amplitude);
        // Update the label with % sign
        label_Amplitude->setText(String(amplitude, 1) + "%", dontSendNotification);
    };
    containerView_Details->addAndMakeVisible(label_Amplitude.get());
    
    // active buttons
    button_Settings = std::make_unique<ImageButton>();
    button_Settings->setTriggeredOnMouseDown(true);
    button_Settings->setImages (false, true, true,
                                imageSettingsIcon, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageSettingsIcon, 0.75, Colour (0x00000000));
    button_Settings->addListener(this);
    containerView_Details->addAndMakeVisible(button_Settings.get());
    
    button_Delete = std::make_unique<ImageButton>();
    button_Delete->setTriggeredOnMouseDown(true);
    button_Delete->setImages (false, true, true,
                              imageDelete, 0.999f, Colour (0x00000000),
                              Image(), 1.000f, Colour (0x00000000),
                              imageDelete, 0.75, Colour (0x00000000));
    button_Delete->addListener(this);
    containerView_Details->addAndMakeVisible(button_Delete.get());
    
    button_DeleteFirst = std::make_unique<ImageButton>();
    button_DeleteFirst->setTriggeredOnMouseDown(true);
    button_DeleteFirst->setImages (false, true, true,
                              imageDelete, 0.999f, Colour (0x00000000),
                              Image(), 1.000f, Colour (0x00000000),
                              imageDelete, 0.75, Colour (0x00000000));
    button_DeleteFirst->addListener(this);
    containerView_Active->addAndMakeVisible(button_DeleteFirst.get());
    
    setState(0);
    
}

CustomChordNoteComponent::~CustomChordNoteComponent()
{
    
}

void CustomChordNoteComponent::paint (Graphics&){}
void CustomChordNoteComponent::resized()
{
    
    backgroundImageComp     ->setBounds(noteBoxInset * scaleFactor, noteBoxInset * scaleFactor, backgroundWidth * scaleFactor, backgroundHeight * scaleFactor);
    containerView_Active    ->setBounds(0, 0, mainWidth * scaleFactor, mainHeight * scaleFactor);
    containerView_Inactive  ->setBounds(0, 0, mainWidth * scaleFactor, mainHeight * scaleFactor);
    containerView_Details   ->setBounds(0, 0, mainWidth * scaleFactor, mainHeight * scaleFactor);
    
    button_AddActive        ->setBounds(130 * scaleFactor, 110 * scaleFactor, 68 * scaleFactor, 68 * scaleFactor);
    button_AddNewNote       ->setBounds(70 * scaleFactor, 190 * scaleFactor, 173 * scaleFactor, 43 * scaleFactor);
    
    comboBox_Note           ->setBounds(170 * scaleFactor, (24 + 18 - 13) * scaleFactor, 111 * scaleFactor, 35 * scaleFactor);
    comboBox_Octave         ->setBounds(170 * scaleFactor, (80 + 18 - 20) * scaleFactor, 111 * scaleFactor, 35 * scaleFactor);
    
    int xx = 217;
    label_NoteValue         ->setBounds(xx * scaleFactor, 36 * scaleFactor, 76 * scaleFactor, 26 * scaleFactor);
    label_OctaveValue       ->setBounds(xx * scaleFactor, 80 * scaleFactor, 76 * scaleFactor, 26 * scaleFactor);
    label_FrequencyLabel    ->setBounds((xx - 30) * scaleFactor, 126 * scaleFactor, 106 * scaleFactor, 26 * scaleFactor);
    label_Amplitude         ->setBounds((xx - 30) * scaleFactor, 160 * scaleFactor, 106 * scaleFactor, 26 * scaleFactor);
    
    button_Settings         ->setBounds(26 * scaleFactor, 224 * scaleFactor, 31 * scaleFactor, 31 * scaleFactor);
    button_Delete           ->setBounds(0, 0, 34 * scaleFactor, 34 * scaleFactor);
    button_DeleteFirst      ->setBounds(0, 0, 34 * scaleFactor, 34 * scaleFactor);
    
}

void CustomChordNoteComponent::buttonClicked (Button*button)
{
    if (button == button_Delete.get())
    {
        setState(0);
        
        // send false to projectManager CUSTOM_CHORD_ACTIVE_1
        projectManager->setChordPlayerParameter(shortcutRef, CUSTOM_CHORD_ACTIVE_1+noteRef, 0);
    }
    if (button == button_DeleteFirst.get())
    {
        setState(0);
        
        // send false to projectManager CUSTOM_CHORD_ACTIVE_1
        projectManager->setChordPlayerParameter(shortcutRef, CUSTOM_CHORD_ACTIVE_1+noteRef, 0);
    }
    else if (button == button_Settings.get())
    {
        // I think settings returns to note/octave selector, addNote
        setState(1);
        projectManager->setChordPlayerParameter(shortcutRef, CUSTOM_CHORD_ACTIVE_1+noteRef, 1);
    }
    else if (button == button_AddActive.get())
    {
        setState(1);
        projectManager->setChordPlayerParameter(shortcutRef, CUSTOM_CHORD_ACTIVE_1+noteRef, 1);
    }
    else if (button == button_AddNewNote.get())
    {
        setState(2);
        // set to data structure
        
        // send to projectManager CUSTOM_CHORD_ACTIVE_1
        projectManager->setChordPlayerParameter(shortcutRef, CUSTOM_CHORD_ACTIVE_1 + noteRef, 2);
        projectManager->setChordPlayerParameter(shortcutRef, CUSTOM_CHORD_NOTE_1 + noteRef, chosenNote);
        projectManager->setChordPlayerParameter(shortcutRef, CUSTOM_CHORD_OCTAVE_1 + noteRef, chosenOctave);
        projectManager->setChordPlayerParameter(shortcutRef, CUSTOM_CHORD_AMPLITUDE_1 + noteRef, 100.0f); // Default amplitude 100%
        
        int noteVal = projectManager->getChordPlayerParameter(shortcutRef, CUSTOM_CHORD_NOTE_1 + noteRef).operator int();
        
        label_NoteValue->setText(ProjectStrings::getKeynoteArray().getReference(noteVal-1), dontSendNotification);
        
        int octaveVal = projectManager->getChordPlayerParameter(shortcutRef, CUSTOM_CHORD_OCTAVE_1 + noteRef).operator int();

        if (octaveVal > 0)
        {
            label_OctaveValue->setText(ProjectStrings::getOctaveArray().getReference(octaveVal-1), dontSendNotification);
        }
        
        int midiNote    = noteVal + ((octaveVal - 1) * 12) - 1;
        float freqVal   = projectManager->frequencyManager->scalesManager->getFrequencyForMIDINoteShortcut(midiNote, shortcutRef) * 2.f;
        
        
        label_FrequencyLabel->setText(String(freqVal, 3, false), dontSendNotification);
        
    }

}

void CustomChordNoteComponent::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == comboBox_Note.get())
    {
        chosenNote = comboBox_Note->getSelectedId();
        
        projectManager->setChordPlayerParameter(shortcutRef, CUSTOM_CHORD_NOTE_1 + noteRef, chosenNote);
        
//        label_FrequencyLabel
        
        int noteVal     = projectManager->getChordPlayerParameter(shortcutRef, CUSTOM_CHORD_NOTE_1 + noteRef).operator int();
        int octaveVal   = projectManager->getChordPlayerParameter(shortcutRef, CUSTOM_CHORD_OCTAVE_1 + noteRef).operator int();
        int midiNote    = noteVal + ((octaveVal - 1) * 12) - 1;
        float freqVal   = projectManager->frequencyManager->scalesManager->getFrequencyForMIDINoteShortcut(midiNote, shortcutRef) * 2.f;
        label_FrequencyLabel->setText(String(freqVal, 3, false), dontSendNotification);
    }
    else if (comboBoxThatHasChanged == comboBox_Octave.get())
    {
        chosenOctave = comboBox_Octave->getSelectedId();
        
        projectManager->setChordPlayerParameter(shortcutRef, CUSTOM_CHORD_OCTAVE_1 + noteRef, chosenOctave);
        
        int noteVal     = projectManager->getChordPlayerParameter(shortcutRef, CUSTOM_CHORD_NOTE_1 + noteRef).operator int();
        int octaveVal   = projectManager->getChordPlayerParameter(shortcutRef, CUSTOM_CHORD_OCTAVE_1 + noteRef).operator int();
        int midiNote    = noteVal + ((octaveVal - 1) * 12) - 1;
        float freqVal   = projectManager->frequencyManager->scalesManager->getFrequencyForMIDINoteShortcut(midiNote, shortcutRef) * 2.f;
        label_FrequencyLabel->setText(String(freqVal, 3, false), dontSendNotification);
    }
}

void CustomChordNoteComponent::syncGUI()
{
    ScalesManager * sm = projectManager->frequencyManager->scalesManager;
    
    sm->getComboBoxPopupMenuForKeynotes(*comboBox_Note->getRootMenu(), (SCALES_UNIT)shortcutRef);

    // active / state
    const auto isActive = (int)projectManager->getChordPlayerParameter(shortcutRef, CUSTOM_CHORD_ACTIVE_1 + noteRef);
    if (isActive)
    {
        // check state
        setState(isActive);
        
        // labels
        int noteVal = projectManager->getChordPlayerParameter(shortcutRef, CUSTOM_CHORD_NOTE_1 + noteRef).operator int();
        comboBox_Note->setSelectedId(noteVal);
        
        int octaveVal = projectManager->getChordPlayerParameter(shortcutRef, CUSTOM_CHORD_OCTAVE_1 + noteRef).operator int();
        comboBox_Octave->setSelectedId(octaveVal);

        label_NoteValue->setText(ProjectStrings::getKeynoteArray().getReference(noteVal-1), dontSendNotification);
        if (octaveVal > 0)
            label_OctaveValue->setText(ProjectStrings::getOctaveArray().getReference(octaveVal-1), dontSendNotification);
        int midiNote = noteVal + ((octaveVal - 1) * 12) - 1;
        float freqVal = projectManager->frequencyManager->scalesManager->getFrequencyForMIDINoteShortcut(midiNote, shortcutRef) * 2.f;
        label_FrequencyLabel->setText(String(freqVal, 3, false), dontSendNotification);
        
        // Load amplitude value
        float amplitude = projectManager->getChordPlayerParameter(shortcutRef, CUSTOM_CHORD_AMPLITUDE_1 + noteRef);
        if (amplitude <= 0) amplitude = 100.0f; // Default to 100% if not set
        label_Amplitude->setText(String(amplitude, 1) + "%", dontSendNotification);
    }
    else
    {
        setState(isActive);
        
        // reset
        label_NoteValue         ->setText("", dontSendNotification);
        label_OctaveValue       ->setText("", dontSendNotification);
        label_Amplitude         ->setText("100%", dontSendNotification);
        label_FrequencyLabel    ->setText("", dontSendNotification);
//        comboBox_Note           ->setSelectedId(0);
//        comboBox_Octave         ->setSelectedId(0);
        
        int noteVal = projectManager->getChordPlayerParameter(shortcutRef, CUSTOM_CHORD_NOTE_1 + noteRef).operator int();
        
        comboBox_Note->setSelectedId(noteVal);
        
        int octaveVal = projectManager->getChordPlayerParameter(shortcutRef, CUSTOM_CHORD_OCTAVE_1 + noteRef).operator int();

        comboBox_Octave->setSelectedId(octaveVal);
        
    }
}

void CustomChordNoteComponent::setShortcutRef(int s)
{
    shortcutRef = s;
}

void CustomChordNoteComponent::setState(int isActive)
{
    activeState = isActive;
    
    if (activeState == 0)
    {
        backgroundImageComp     ->setImage(imageBackgroundInactive);
        containerView_Inactive  ->setVisible(true);
        containerView_Active    ->setVisible(false);
        containerView_Details   ->setVisible(false);
        
        label_FrequencyLabel->setVisible(false);
    }
    else if (activeState == 1)
    {
        backgroundImageComp     ->setImage(imageDetailsBackground);
        containerView_Inactive  ->setVisible(false);
        containerView_Active    ->setVisible(true);
        containerView_Details   ->setVisible(false);
        
        label_FrequencyLabel->setVisible(true);
    }
    else if (activeState == 2)
    {
        backgroundImageComp     ->setImage(imageDetailsBackground);
        containerView_Inactive  ->setVisible(false);
        containerView_Active    ->setVisible(false);
        containerView_Details   ->setVisible(true);
        
        label_FrequencyLabel->setVisible(true);
    }
}
