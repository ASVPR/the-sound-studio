/*
  ==============================================================================

    ChordPlayerSettingsComponent.cpp
    Created: 14 Mar 2019 9:01:08pm
    Author:  Ziv Elovitch - The Sound Studio Team

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "ChordPlayerSettingsComponent.h"

//==============================================================================

AddOnPopupComponent::AddOnPopupComponent(ProjectManager * pm)
{
    projectManager = pm;
    shortcutRef = 0;
    
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
    addAndMakeVisible(addonButton[AddOn_6].get());
    
    label[AddOn_6] = std::make_unique<Label>("", ProjectStrings::getAddons().getReference(AddOn_6));
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
    addAndMakeVisible(addonButton[AddOn_7].get());
    
    label[AddOn_7] = std::make_unique<Label>("", ProjectStrings::getAddons().getReference(AddOn_7));
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
    addAndMakeVisible(addonButton[AddOn_7Major].get());
    
    label[AddOn_7Major] = std::make_unique<Label>("", ProjectStrings::getAddons().getReference(AddOn_7Major));
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
    addAndMakeVisible(addonButton[AddOn_9].get());
    
    label[AddOn_9] = std::make_unique<Label>("", ProjectStrings::getAddons().getReference(AddOn_9));
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
    addAndMakeVisible(addonButton[AddOn_9flat].get());
    
    label[AddOn_9flat] = std::make_unique<Label>("", ProjectStrings::getAddons().getReference(AddOn_9flat));
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
    addAndMakeVisible(addonButton[AddOn_9sharp].get());
    
    label[AddOn_9sharp] = std::make_unique<Label>("", ProjectStrings::getAddons().getReference(AddOn_9sharp));
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
    addAndMakeVisible(addonButton[AddOn_11].get());
    
    label[AddOn_11] = std::make_unique<Label>("", ProjectStrings::getAddons().getReference(AddOn_11));
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
    addAndMakeVisible(addonButton[AddOn_11sharp].get());
    
    label[AddOn_11sharp] = std::make_unique<Label>("", ProjectStrings::getAddons().getReference(AddOn_11sharp));
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
    addAndMakeVisible(addonButton[AddOn_13].get());
    
    label[AddOn_13] = std::make_unique<Label>("", ProjectStrings::getAddons().getReference(AddOn_13));
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
    addAndMakeVisible(addonButton[AddOn_13flat].get());
    
    label[AddOn_13flat] = std::make_unique<Label>("", ProjectStrings::getAddons().getReference(AddOn_13flat));
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
    addAndMakeVisible(addonButton[AddOn_Inverted].get());
    
    label[AddOn_Inverted] = std::make_unique<Label>("", ProjectStrings::getAddons().getReference(AddOn_Inverted));
    label[AddOn_Inverted]->setJustificationType(Justification::left);
    label[AddOn_Inverted]->setFont(fontLight);
    label[AddOn_Inverted]->setColour(Label::textColourId, Colours::white);
    addAndMakeVisible(label[AddOn_Inverted].get());
    
    
    
}

AddOnPopupComponent::~AddOnPopupComponent() { }

void AddOnPopupComponent::resized()
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

void AddOnPopupComponent::paint (Graphics&g)
{
    // draw background
    g.drawImage(backgroundImage, 0, 0, 357, 179, 0, 0, 357, 179);
}

void AddOnPopupComponent::buttonClicked (Button*button)
{
    if (button == addonButton[AddOn_6].get())
    {
        if (button->getToggleState())
        {
            projectManager->setChordPlayerParameter(shortcutRef, ADD_ONS, AddOn_NONE);
        }
        else
        {
            projectManager->setChordPlayerParameter(shortcutRef, ADD_ONS, AddOn_6);
        }
        
    }
    else if (button == addonButton[AddOn_7].get())
    {
        if (button->getToggleState())
        {
            projectManager->setChordPlayerParameter(shortcutRef, ADD_ONS, AddOn_NONE);
        }
        else
        {
            projectManager->setChordPlayerParameter(shortcutRef, ADD_ONS, AddOn_7);
        }
    }
    else if (button == addonButton[AddOn_7Major].get())
    {
        if (button->getToggleState())
        {
            projectManager->setChordPlayerParameter(shortcutRef, ADD_ONS, AddOn_NONE);
        }
        else
        {
            projectManager->setChordPlayerParameter(shortcutRef, ADD_ONS, AddOn_7Major);
        }
    }
    else if (button == addonButton[AddOn_9].get())
    {
        if (button->getToggleState())
        {
            projectManager->setChordPlayerParameter(shortcutRef, ADD_ONS, AddOn_NONE);
        }
        else
        {
            projectManager->setChordPlayerParameter(shortcutRef, ADD_ONS, AddOn_9);
        }
    }
    else if (button == addonButton[AddOn_9flat].get())
    {
        if (button->getToggleState())
        {
            projectManager->setChordPlayerParameter(shortcutRef, ADD_ONS, AddOn_NONE);
        }
        else
        {
            projectManager->setChordPlayerParameter(shortcutRef, ADD_ONS, AddOn_9flat);
        }
    }
    else if (button == addonButton[AddOn_9sharp].get())
    {
        if (button->getToggleState())
        {
            projectManager->setChordPlayerParameter(shortcutRef, ADD_ONS, AddOn_NONE);
        }
        else
        {
            projectManager->setChordPlayerParameter(shortcutRef, ADD_ONS, AddOn_9sharp);
        }
    }
    else if (button == addonButton[AddOn_11].get())
    {
        if (button->getToggleState())
        {
            projectManager->setChordPlayerParameter(shortcutRef, ADD_ONS, AddOn_NONE);
        }
        else
        {
            projectManager->setChordPlayerParameter(shortcutRef, ADD_ONS, AddOn_11);
        }
    }
    else if (button == addonButton[AddOn_11sharp].get())
    {
        if (button->getToggleState())
        {
            projectManager->setChordPlayerParameter(shortcutRef, ADD_ONS, AddOn_NONE);
        }
        else
        {
            projectManager->setChordPlayerParameter(shortcutRef, ADD_ONS, AddOn_11sharp);
        }
    }
    else if (button == addonButton[AddOn_13].get())
    {
        if (button->getToggleState())
        {
            projectManager->setChordPlayerParameter(shortcutRef, ADD_ONS, AddOn_NONE);
        }
        else
        {
            projectManager->setChordPlayerParameter(shortcutRef, ADD_ONS, AddOn_13);
        }
    }
    else if (button == addonButton[AddOn_13flat].get())
    {
        if (button->getToggleState())
        {
            projectManager->setChordPlayerParameter(shortcutRef, ADD_ONS, AddOn_NONE);
        }
        else
        {
            projectManager->setChordPlayerParameter(shortcutRef, ADD_ONS, AddOn_13flat);
        }
    }
    else if (button == addonButton[AddOn_Inverted].get())
    {
        if (button->getToggleState())
        {
            projectManager->setChordPlayerParameter(shortcutRef, ADD_ONS, AddOn_NONE);
        }
        else
        {
            projectManager->setChordPlayerParameter(shortcutRef, ADD_ONS, AddOn_Inverted);
        }
    }
}

void AddOnPopupComponent::setShortcutRef(int newShortcutRef)
{
    shortcutRef = newShortcutRef;
}

void AddOnPopupComponent::syncGUI()
{
    int val = projectManager->getChordPlayerParameter(shortcutRef, ADD_ONS).operator int();
    
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


ChordPlayerSettingsComponent::ChordPlayerSettingsComponent(ProjectManager * pm)
{
    projectManager  = pm;
    shortcutRef     = 0;
    
    projectManager->addUIListener(this);
    
    // LookAndFeels
    
    mainBackgroundImage         = ImageCache::getFromMemory(BinaryData::ChordPlayerSettingsBackground2_png, BinaryData::ChordPlayerSettingsBackground2_pngSize);
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
    slider_Amplitude    = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_AMPLITUDE);
    slider_Amplitude    ->setRange (0, 1.0, 0);
    slider_Amplitude    ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_Amplitude    ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_Amplitude    ->addListener (this);
    slider_Amplitude    ->setTextValueSuffix("%");
    slider_Amplitude    ->setNumDecimalPlacesToDisplay(1);
    slider_Amplitude    ->setLookAndFeel(&lookAndFeel);
    
    addAndMakeVisible(slider_Amplitude.get());
    
    int dif = 219;
    
    slider_Attack       = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_ATTACK);
    slider_Attack       ->setRange (0, 1.0, 0);
    slider_Attack       ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_Attack       ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_Attack       ->addListener (this);
    slider_Attack       ->setTextValueSuffix("Ms");
    slider_Attack       ->setNumDecimalPlacesToDisplay(0);
    slider_Attack       ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_Attack.get());
    
    slider_Decay        = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_DECAY);
    slider_Decay        ->setRange (0, 1.0, 0);
    slider_Decay        ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_Decay        ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_Decay        ->addListener (this);
    slider_Decay        ->setTextValueSuffix("Ms");
    slider_Decay        ->setNumDecimalPlacesToDisplay(0);
    slider_Decay        ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_Decay.get());
    
    slider_Sustain      = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_SUSTAIN);
    slider_Sustain      ->setRange (0, 1.0, 0);
    slider_Sustain      ->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    slider_Sustain      ->setTextBoxStyle (Slider::TextBoxBelow, false, 78, 28);
    slider_Sustain      ->addListener (this);
    slider_Sustain      ->setTextValueSuffix("%");
    slider_Sustain      ->setNumDecimalPlacesToDisplay(2);
    slider_Sustain        ->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(slider_Sustain.get());
    
    slider_Release      = std::make_unique<CustomRotarySlider>(CustomRotarySlider::ROTARY_RELEASE);
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
    comboBoxKeynote     = std::make_unique<ComboBox>();
    comboBoxKeynote->setSelectedId(0);
    comboBoxKeynote->addListener(this);
    comboBoxKeynote->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(comboBoxKeynote.get());
    
    comboBoxChordtype= std::make_unique<ComboBox>();
    comboBoxChordtype->setSelectedId(0);
    comboBoxChordtype->addListener(this);
    comboBoxChordtype->setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(comboBoxChordtype.get());

    comboBoxPlayingInstrument = std::make_unique<ComboBox>();
    
    // generate
    PopupMenu * comboBoxMenu =  comboBoxPlayingInstrument->getRootMenu();
    
    // Use synthesis-based instrument list with properly implemented synthesis engines
    // Physical Modeling: Piano, Strings
    // Karplus-Strong: Guitar, Harp
    // Wavetable: Synthesizers, Organ
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
    
    comboBoxOutputSelection = std::make_unique<ComboBox>();
    comboBoxOutputSelection->setLookAndFeel(&lookAndFeel);
    comboBoxOutputSelection->addListener(this);
    comboBoxOutputSelection->setSelectedId(0);
    projectManager->getOutputsPopupMenu(*comboBoxOutputSelection->getRootMenu());
    addAndMakeVisible(comboBoxOutputSelection.get());
    
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
    
    textEditorDivision= std::make_unique<TextEditor>("");
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
    addOnPopupComponent = std::make_unique<AddOnPopupComponent>(projectManager);
    addAndMakeVisible(addOnPopupComponent.get());
    addOnPopupComponent->setVisible(false);
    
    customChordPopupComponent = std::make_unique<CustomChordPopupComponent>(projectManager);
    addAndMakeVisible(customChordPopupComponent.get());
    customChordPopupComponent->setVisible(false);
    
    for (int i = AMPLITUDE_MIN; i <= RELEASE_MAX; i++)
    {
        updateMinMaxSettings(i);
    }
    
    wavetableEditorComponent          = new WaveTableOscViewComponent(projectManager, AUDIO_MODE::MODE_CHORD_PLAYER, shortcutRef);
    wavetableEditorComponent->setBounds(0, 0, 600, 400);
    
    // Hand ownership of the editor to the popup window to avoid double-deletion
    popupWavetableWindow = std::make_unique<PopupFFTWindow>("Wavetable Editor - Chord Player", wavetableEditorComponent, Colours::black, DocumentWindow::allButtons, true);
    popupWavetableWindow ->centreWithSize(600, 400);
    popupWavetableWindow ->setVisible(false);
    popupWavetableWindow ->setResizable(true, true);
    
}

ChordPlayerSettingsComponent::~ChordPlayerSettingsComponent()
{
    // Remove ourselves as a UI listener to prevent accessing invalid pointers during destruction
    if (projectManager)
    {
        projectManager->removeUIListener(this);
        projectManager = nullptr; // Clear the pointer
    }
    
    // Safely close popup window - use .get() to check raw pointer
    if (popupWavetableWindow)
    {
        if (popupWavetableWindow->isVisible())
        {
            popupWavetableWindow->setVisible(false);
        }
        popupWavetableWindow.reset(); // Explicitly reset the unique_ptr
    }
    
    // Safely hide child components
    if (addOnPopupComponent)
    {
        if (addOnPopupComponent->isVisible())
        {
            addOnPopupComponent->setVisible(false);
        }
        // Remove from parent before destruction
        removeChildComponent(addOnPopupComponent.get());
        addOnPopupComponent.reset(); // Explicitly reset the unique_ptr
    }
    
    if (customChordPopupComponent)
    {
        if (customChordPopupComponent->isVisible())
        {
            customChordPopupComponent->setVisible(false);
        }
        // Remove from parent before destruction
        removeChildComponent(customChordPopupComponent.get());
        customChordPopupComponent.reset(); // Explicitly reset the unique_ptr
    }
    
    // wavetableEditorComponent is owned by popupWavetableWindow (setContentOwned)
    // and will be deleted when the window is destroyed.
}


void ChordPlayerSettingsComponent::resized()
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
    
    float xDif = 200;
    
    textEditorRepeat                ->setBounds((460 - xDif) * scaleFactor, 1108 * scaleFactor, 111 * scaleFactor, 35 * scaleFactor);
    textEditorRepeat->applyFontToAllText(28 * scaleFactor);
    
    textEditorPause                 ->setBounds((1084- xDif) * scaleFactor, 1108 * scaleFactor, 111 * scaleFactor, 35 * scaleFactor);
    textEditorPause->applyFontToAllText(28 * scaleFactor);
    
    textEditorLength                ->setBounds((780- xDif) * scaleFactor, 1108 * scaleFactor, 111 * scaleFactor, 35 * scaleFactor);
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
    
    comboBoxOutputSelection         -> setBounds(1150 * scaleFactor, 1102 * scaleFactor, 250 * scaleFactor, 43 * scaleFactor);
    

}

void ChordPlayerSettingsComponent::paint (Graphics&g)
{
    g.setColour(Colours::black);
    g.setOpacity(0.88);
    g.fillAll();
    
    g.setOpacity(1.0);
    g.drawImage(mainBackgroundImage, 24 * scaleFactor, 94 * scaleFactor, 1502 * scaleFactor, 1242 * scaleFactor, 0, 0, 1502, 1242);
}

void ChordPlayerSettingsComponent::buttonClicked (Button*button)
{
    if (button == button_Close.get())
    {
        closeView();

        popupWavetableWindow->setVisible(false);
        
    }
    else if (button == button_ChooseChordFromList.get())
    {
        projectManager->setChordPlayerParameter(shortcutRef, CHORD_SOURCE, false);
    }
    else if (button == button_ChooseChordFromFrequency.get())
    {
        projectManager->setChordPlayerParameter(shortcutRef, CHORD_SOURCE, true);
    }
    else if (button == button_Multiplication.get())
    {
        projectManager->setChordPlayerParameter(shortcutRef, MULTIPLY_OR_DIVISION, false);
    }
    else if (button == button_Division.get())
    {
        projectManager->setChordPlayerParameter(shortcutRef, MULTIPLY_OR_DIVISION, true);
    }
    else if (button == button_ManipulateFreq.get())
    {
        projectManager->setChordPlayerParameter(shortcutRef, MANIPULATE_CHOSEN_FREQUENCY, !button_ManipulateFreq->getToggleState());
    }
    else if (button == button_Default.get())
    {
        projectManager->setChordPlayerParameter(shortcutRef, WAVEFORM_TYPE, 0);
    }
    else if (button == button_Sine.get())
    {
        projectManager->setChordPlayerParameter(shortcutRef, WAVEFORM_TYPE, 1);
    }
    else if (button == button_Triangle.get())
    {
        projectManager->setChordPlayerParameter(shortcutRef, WAVEFORM_TYPE, 2);
    }
    else if (button == button_Square.get())
    {
        projectManager->setChordPlayerParameter(shortcutRef, WAVEFORM_TYPE, 3);
    }
    else if (button == button_Sawtooth.get())
    {
        projectManager->setChordPlayerParameter(shortcutRef, WAVEFORM_TYPE, 4);
    }
    else if (button == button_Wavetable.get())
    {
        projectManager->setChordPlayerParameter(shortcutRef, WAVEFORM_TYPE, 5);
    }
    else if (button == button_WavetableEditor.get())
    {
        if (!popupWavetableWindow ->isVisible())
        {
            popupWavetableWindow ->setVisible(true);
            wavetableEditorComponent->setShortcut(shortcutRef);
        }
        else
            popupWavetableWindow ->setVisible(false);
    }
    else if (button == button_Add.get())
    {
        projectManager->setChordPlayerParameter(shortcutRef, SHORTCUT_IS_ACTIVE,  true);
        
        closeView();
        
        projectManager->logFileWriter->processLog_ChordPlayer_Parameters();
    }
    else if (button == button_AddCustomChord.get())
    {
        customChordPopupComponent->openView(shortcutRef);
        customChordPopupComponent->syncGUI();
    }
    else if (button == button_Addons.get())
    {
        addOnPopupComponent->setShortcutRef(shortcutRef);
        addOnPopupComponent->setVisible(!addOnPopupComponent->isVisible());
        addOnPopupComponent->syncGUI();
    }
    else if (button == button_Panic.get())
    {
        projectManager->setPlayerCommand(PLAYER_COMMANDS::COMMAND_PLAYER_PANIC);
    }
}

void ChordPlayerSettingsComponent::sliderValueChanged (Slider* slider)
{
    if (slider == slider_Amplitude.get())
    {
        projectManager->setChordPlayerParameter(shortcutRef, ENV_AMPLITUDE, slider_Amplitude->getValue());
    }
    else if (slider == slider_Attack.get())
    {
        projectManager->setChordPlayerParameter(shortcutRef, ENV_ATTACK, slider_Attack->getValue());
    }
    else if (slider == slider_Decay.get())
    {
        projectManager->setChordPlayerParameter(shortcutRef, ENV_DECAY, slider_Decay->getValue());
    }
    else if (slider == slider_Sustain.get())
    {
        projectManager->setChordPlayerParameter(shortcutRef, ENV_SUSTAIN, slider_Sustain->getValue());
    }
    else if (slider == slider_Release.get())
    {
        projectManager->setChordPlayerParameter(shortcutRef, ENV_RELEASE, slider_Release->getValue());
    }
}

void ChordPlayerSettingsComponent::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == comboBoxKeynote.get())
    {
        projectManager->setChordPlayerParameter(shortcutRef, KEYNOTE, comboBoxKeynote->getSelectedId());
        
        projectManager->setChordPlayerParameter(shortcutRef, CUSTOM_CHORD, false);
    }
    else if (comboBoxThatHasChanged == comboBoxPlayingInstrument.get())
    {
        int selectedID = comboBoxPlayingInstrument->getSelectedId();
        projectManager->setChordPlayerParameter(shortcutRef, INSTRUMENT_TYPE, selectedID);
    }
    else if (comboBoxThatHasChanged == comboBoxChordtype.get())
    {
        projectManager->setChordPlayerParameter(shortcutRef, CHORD_TYPE, comboBoxChordtype->getSelectedId());
        
        projectManager->setChordPlayerParameter(shortcutRef, CUSTOM_CHORD, false);
    }
    else if (comboBoxThatHasChanged == comboBoxOutputSelection.get())
    {
        projectManager->setChordPlayerParameter(shortcutRef, CHORD_PLAYER_OUTPUT_SELECTION, comboBoxOutputSelection->getSelectedId());
    }
    
    if (comboBoxThatHasChanged == comboBox_Scales.get())
    {
        projectManager->setChordPlayerParameter(shortcutRef, CHORDPLAYER_SCALE, comboBox_Scales->getSelectedId());
    }
    
}

void ChordPlayerSettingsComponent::textEditorReturnKeyPressed (TextEditor&editor)
{
    if (&editor == textEditorPause.get())
    {
        int value = editor.getText().getIntValue();

        // bounds check
        if (value >= PAUSE_MAX) {  value = PAUSE_MAX; }
        if (value <= PAUSE_MIN) { value = PAUSE_MIN; }

        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setChordPlayerParameter(shortcutRef, NUM_PAUSE, value);
    }
    else if (&editor == textEditorRepeat.get())
    {
        int value = editor.getText().getIntValue();
        
        // bounds check
        if (value >= REPEAT_MAX) {  value = REPEAT_MAX; }
        if (value <= REPEAT_MIN) { value = REPEAT_MIN; }
        
        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setChordPlayerParameter(shortcutRef, NUM_REPEATS, value);
    }
    else if (&editor == textEditorLength.get())
    {
        int value = editor.getText().getIntValue();
        
        // bounds check
        if (value >= LENGTH_MAX) {  value = LENGTH_MAX; }
        if (value <= LENGTH_MIN) { value = LENGTH_MIN; }
        
        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setChordPlayerParameter(shortcutRef, NUM_DURATION, value);
    }
    else if (&editor == textEditorDivision.get())
    {
        float value = editor.getText().getFloatValue();
        
        // bounds check
        if (value >= DIVISION_MAXIMUM) {  value = DIVISION_MAXIMUM; }
        if (value <= DIVISION_MINIMUM) { value = DIVISION_MINIMUM; }
        
        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setChordPlayerParameter(shortcutRef, DIVISION_VALUE, value);
    }
    else if (&editor == textEditorMultiplication.get())
    {
        float value = editor.getText().getFloatValue();
        
        // bounds check
        if (value >= MULTIPLY_MAXIMUM) {  value = MULTIPLY_MAXIMUM; }
        if (value <= MULTIPLY_MINIMUM) {  value = MULTIPLY_MINIMUM; }
        
        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setChordPlayerParameter(shortcutRef, MULTIPLY_VALUE, value);
    }
    else if (&editor == textEditorInsertFreq.get())
    {
        float value = editor.getText().getFloatValue();
        
        // bounds check
        if (value >= INSERT_FREQ_MAX) {  value = INSERT_FREQ_MAX; }
        if (value <= INSERT_FREQ_MIN) {  value = INSERT_FREQ_MIN; }
        
        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setChordPlayerParameter(shortcutRef, INSERT_FREQUENCY, value);
    }
    else if (&editor == textEditor_Octave.get())
    {
        float value = editor.getText().getFloatValue();
        
        // bounds check
        if (value >= OCTAVE_MAX) {  value = OCTAVE_MAX; }
        if (value <= OCTAVE_MIN) {  value = OCTAVE_MIN; }
        
        String newVal(value); editor.setText(newVal);

        projectManager->setChordPlayerParameter(shortcutRef, OCTAVE, value);
    }
    
    Component::unfocusAllComponents();
}

void ChordPlayerSettingsComponent::openView(int shortcut)
{
    shortcutRef = shortcut;
    
    syncUI();
    
    setVisible(true);
}

void ChordPlayerSettingsComponent::closeView()
{
    addOnPopupComponent->setVisible(false);
    
    popupWavetableWindow->setVisible(false);
    
    setVisible(false);
    isSynced = false;
}

void ChordPlayerSettingsComponent::syncUI()
{
    // grabs
    // CHORD_SOURCE
    //==============================================================
    bool chordSource = projectManager->getChordPlayerParameter(shortcutRef, CHORD_SOURCE).operator bool();
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

    
    slider_Amplitude    ->setValue(projectManager->getChordPlayerParameter(shortcutRef, ENV_AMPLITUDE).operator double());
    slider_Attack       ->setValue(projectManager->getChordPlayerParameter(shortcutRef, ENV_ATTACK).operator double());
    slider_Sustain      ->setValue(projectManager->getChordPlayerParameter(shortcutRef, ENV_SUSTAIN).operator double());
    slider_Decay        ->setValue(projectManager->getChordPlayerParameter(shortcutRef, ENV_DECAY).operator double());
    slider_Release      ->setValue(projectManager->getChordPlayerParameter(shortcutRef, ENV_RELEASE).operator double());
    
    // act as toggle between 5 buttons WAVEFORM_TYPE
    int waveformType =  projectManager->getChordPlayerParameter(shortcutRef, WAVEFORM_TYPE).operator int();
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
    
    int instrumentType = projectManager->getChordPlayerParameter(shortcutRef, INSTRUMENT_TYPE).operator int();
    comboBoxPlayingInstrument->setSelectedId(instrumentType);
    
    // Combobox
    int keyNote = projectManager->getChordPlayerParameter(shortcutRef, KEYNOTE).operator int();
    comboBoxKeynote ->setSelectedId(keyNote);  //KEYNOTE

    int chordType = projectManager->getChordPlayerParameter(shortcutRef, CHORD_TYPE).operator int();
    comboBoxChordtype ->setSelectedId(chordType) ; //CHORD_TYPE
    
    float octave = projectManager->getChordPlayerParameter(shortcutRef, OCTAVE).operator float();
    String octaveString(octave); textEditor_Octave->setText(octaveString);
    
    int scale = projectManager->getChordPlayerParameter(shortcutRef, CHORDPLAYER_SCALE).operator int();
    comboBox_Scales ->setSelectedId(scale); //OCTAVE
    
    int output = projectManager->getChordPlayerParameter(shortcutRef, CHORD_PLAYER_OUTPUT_SELECTION).operator int();
    comboBoxOutputSelection ->setSelectedId(output); //OCTAVE
   
    //NUM_REPEATS
    
    int pause = projectManager->getChordPlayerParameter(shortcutRef, NUM_PAUSE).operator int();
    
    bool manipulateFreq = projectManager->getChordPlayerParameter(shortcutRef, MANIPULATE_CHOSEN_FREQUENCY).operator bool();
    button_ManipulateFreq->setToggleState(manipulateFreq, dontSendNotification); //MANIPULATE_CHOSEN_FREQUENCY,
    
    bool multOrDivide = projectManager->getChordPlayerParameter(shortcutRef, MULTIPLY_OR_DIVISION).operator bool();
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
    
    // TextEntryBoxes
    float insertFreqVal = projectManager->getChordPlayerParameter(shortcutRef, INSERT_FREQUENCY).operator float();
    String freqString(insertFreqVal); freqString.append("Hz", 2);
    textEditorInsertFreq->setText(freqString);
    
    float multVal = projectManager->getChordPlayerParameter(shortcutRef, MULTIPLY_VALUE).operator float();
    String multString(multVal);
    textEditorMultiplication->setText(multString);
    
    float divVal = projectManager->getChordPlayerParameter(shortcutRef, DIVISION_VALUE).operator float();
    String divString(divVal);
    textEditorDivision->setText(divString);
    
    int pauseVal = projectManager->getChordPlayerParameter(shortcutRef, NUM_PAUSE).operator int();
    String pauseString(pauseVal);
    pauseString.append("ms", 2);
    textEditorPause->setText(pauseString);
    
    int lengthVal = projectManager->getChordPlayerParameter(shortcutRef, NUM_DURATION).operator int();
    String lengthString(lengthVal);
    lengthString.append("ms", 2);
    textEditorLength->setText(lengthString);
    
    int numRepeats = projectManager->getChordPlayerParameter(shortcutRef, NUM_REPEATS).operator int();
    String repeatsString(numRepeats);
    textEditorRepeat->setText(repeatsString);
    
    
    // addons popup
    addOnPopupComponent->syncGUI();
    
    // custom chord unit
    customChordPopupComponent->syncGUI();
    
    isSynced = true;
}

void ChordPlayerSettingsComponent::updateMinMaxSettings(int paramIndex)
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
