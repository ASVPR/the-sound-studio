/*
  ==============================================================================

    ShortcutComponent.cpp

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "ShortcutComponent.h"
#include "Parameters.h"

//==============================================================================
ShortcutComponent::ShortcutComponent(int shortcut, ProjectManager * pm)
{
    projectManager  = pm;
    shortcutRef     = shortcut;
    
    chord = "";
    chordType = "";
    frequencyHz = "";
    
    // Font
    Typeface::Ptr AssistantLight     = Typeface::createSystemTypefaceFor(BinaryData::AssistantLight_ttf, BinaryData::AssistantLight_ttfSize);
    Typeface::Ptr AssistantSemiBold   = Typeface::createSystemTypefaceFor(BinaryData::AssistantSemiBold_ttf, BinaryData::AssistantSemiBold_ttfSize);
    Font fontSemiBold(AssistantSemiBold);
    Font fontLight(AssistantLight);
    
    fontSemiBold.setHeight(33);
    fontLight.setHeight(33);
    
    // Images
    imageSettingsIcon       = ImageCache::getFromMemory(BinaryData::settings2x_png, BinaryData::settings2x_pngSize);
    imageMuteIcon           = ImageCache::getFromMemory(BinaryData::ShortcutMute2x_png, BinaryData::ShortcutMute2x_pngSize);
    imageLoopIcon           = ImageCache::getFromMemory(BinaryData::ShortcutLoop2x_png, BinaryData::ShortcutLoop2x_pngSize);

    imageDelete             = ImageCache::getFromMemory(BinaryData::ShortcutClose2x_png, BinaryData::ShortcutClose2x_pngSize);
    imageAddIcon            = ImageCache::getFromMemory(BinaryData::ShortcutAdd2x_png, BinaryData::ShortcutAdd2x_pngSize);
    
    
    
    
    // containers
    containerView_Active = std::make_unique<Component>();
    addAndMakeVisible(containerView_Active.get());

    containerView_Inactive = std::make_unique<Component>();
    addAndMakeVisible(containerView_Inactive.get());
    
    // active buttons
    button_Settings = std::make_unique<ImageButton>();
    button_Settings->setTriggeredOnMouseDown(true);
    button_Settings->setImages (false, true, true,
                                imageSettingsIcon, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageSettingsIcon, 0.75, Colour (0x00000000));
    button_Settings->addListener(this);
    containerView_Active->addAndMakeVisible(button_Settings.get());
    
    button_Mute = std::make_unique<ImageButton>();
    button_Mute->setTriggeredOnMouseDown(true);
    button_Mute->setImages (false, true, true,
                            imageMuteIcon, 0.5f, Colour (0x00000000),
                            Image(), 1.000f, Colour (0x00000000),
                            imageMuteIcon, 0.99, Colour (0x00000000));
    button_Mute->addListener(this);
    containerView_Active->addAndMakeVisible(button_Mute.get());
    
    button_Loop = std::make_unique<ImageButton>();
    button_Loop->setTriggeredOnMouseDown(true);
    button_Loop->setImages (false, true, true,
                            imageLoopIcon, 0.5f, Colour (0x00000000),
                            Image(), 1.000f, Colour (0x00000000),
                            imageLoopIcon, 0.999, Colour (0x00000000));
    button_Loop->addListener(this);
    
    
    button_Delete = std::make_unique<ImageButton>();
    button_Delete->setTriggeredOnMouseDown(true);
    button_Delete->setImages (false, true, true,
                              imageDelete, 0.999f, Colour (0x00000000),
                              Image(), 1.000f, Colour (0x00000000),
                              imageDelete, 0.75, Colour (0x00000000));
    button_Delete->addListener(this);
    containerView_Active->addAndMakeVisible(button_Delete.get());
    
    
    label_Chord = std::make_unique<Label>();
    label_Chord->setText("Chord", dontSendNotification);
    label_Chord->setJustificationType(Justification::left);
    fontSemiBold.setHeight(33);
    label_Chord->setFont(fontSemiBold);
    containerView_Active->addAndMakeVisible(label_Chord.get());
    
    
    label_Chordtype = std::make_unique<Label>();
    label_Chordtype->setText("Chord type", dontSendNotification);
    label_Chordtype->setJustificationType(Justification::left);
    fontSemiBold.setHeight(33);
    label_Chordtype->setFont(fontSemiBold);
    containerView_Active->addAndMakeVisible(label_Chordtype.get());
    
    label_Scale = std::make_unique<Label>();
    label_Scale->setText("Scale", dontSendNotification);
    label_Scale->setJustificationType(Justification::left);
    fontSemiBold.setHeight(33);
    label_Scale->setFont(fontSemiBold);
    containerView_Active->addAndMakeVisible(label_Scale.get());
    
    label_Instrument = std::make_unique<Label>();
    label_Instrument->setText("Instrument", dontSendNotification);
    label_Instrument->setJustificationType(Justification::left);
    fontSemiBold.setHeight(33);
    label_Instrument->setFont(fontSemiBold);
    containerView_Active->addAndMakeVisible(label_Instrument.get());
    
    label_Frequency = std::make_unique<Label>();
    label_Frequency->setText("Frequency", dontSendNotification);
    label_Frequency->setJustificationType(Justification::left);
    fontSemiBold.setHeight(33);
    label_Frequency->setFont(fontSemiBold);
    containerView_Active->addAndMakeVisible(label_Frequency.get());
    
    label_ChordValue = std::make_unique<Label>();
    label_ChordValue->setText(chord, dontSendNotification);
    label_ChordValue->setJustificationType(Justification::right);
    label_ChordValue->setFont(fontLight);
    containerView_Active->addAndMakeVisible(label_ChordValue.get());
    
    label_ChordtypeValue = std::make_unique<Label>();
    label_ChordtypeValue->setText(chordType, dontSendNotification);
    label_ChordtypeValue->setJustificationType(Justification::right);
    label_ChordtypeValue->setFont(fontLight);
    containerView_Active->addAndMakeVisible(label_ChordtypeValue.get());
    
    label_ScaleValue = std::make_unique<Label>();
    label_ScaleValue->setText(chordType, dontSendNotification);
    label_ScaleValue->setJustificationType(Justification::right);
    label_ScaleValue->setFont(fontLight);
    containerView_Active->addAndMakeVisible(label_ScaleValue.get());
    
    label_InstrumentValue = std::make_unique<Label>();
    label_InstrumentValue->setText(chordType, dontSendNotification);
    label_InstrumentValue->setJustificationType(Justification::right);
    label_InstrumentValue->setFont(fontLight);
    containerView_Active->addAndMakeVisible(label_InstrumentValue.get());
    
    
    label_FrequencyValue = std::make_unique<Label>();
    label_FrequencyValue->setText(frequencyHz, dontSendNotification);
    label_FrequencyValue->setJustificationType(Justification::right);
    label_FrequencyValue->setFont(fontLight);
    containerView_Active->addAndMakeVisible(label_FrequencyValue.get());
    
    
    shortcutRefLabel = std::make_unique<Label>();
    String shortcutString("Shortcut ");
    shortcutString.append(ProjectStrings::getShortcuts().getReference(shortcutRef), 2);
    shortcutRefLabel->setText(shortcutString, dontSendNotification);
    shortcutRefLabel->setJustificationType(Justification::centred);
    fontLight.setHeight(33);
    shortcutRefLabel->setFont(fontLight);
    containerView_Active->addAndMakeVisible(shortcutRefLabel.get());
    
    
    // inactive button
    button_Add = std::make_unique<ImageButton>();
    button_Add->setTriggeredOnMouseDown(true);
    button_Add->setImages (false, true, true,
                           imageAddIcon, 0.999f, Colour (0x00000000),
                           Image(), 1.000f, Colour (0x00000000),
                           imageAddIcon, 0.75, Colour (0x00000000));
    button_Add->addListener(this);
    containerView_Inactive->addAndMakeVisible(button_Add.get());
    
    setState(0);

}

void ShortcutComponent::resized()
{
    auto bounds = getLocalBounds();
    const int w = bounds.getWidth();
    const int h = bounds.getHeight();
    const float s = w / 335.0f;  // reference card width

    int fontSize = (int)(26.0f * s);
    int bgMargin = (int)(16.0f * s);
    int bgTop = (int)(38.0f * s);
    int bgW = w - bgMargin * 2;
    int bgH = (int)(271.0f * s);

    containerView_Active->setBounds(bounds);
    containerView_Inactive->setBounds(bounds);

    // Delete button top-left
    int delSize = (int)(35.0f * s);
    button_Delete->setBounds(0, (int)(21.0f * s), delSize, delSize);

    // Labels area
    int labelY = (int)(54.0f * s);
    int labelH = (int)(40.0f * s);
    int leftX = (int)(24.0f * s);
    int labelW = (int)(120.0f * s);
    int rightX = w - leftX - labelW;

    label_Chord->setBounds(leftX, labelY, labelW, labelH);
    label_ChordValue->setBounds(rightX, labelY, labelW, labelH);
    labelY += labelH;

    label_Chordtype->setBounds(leftX, labelY, labelW, labelH);
    label_ChordtypeValue->setBounds(rightX, labelY, labelW, labelH);
    labelY += labelH;

    label_Scale->setBounds(leftX, labelY, labelW, labelH);
    label_ScaleValue->setBounds(rightX, labelY, labelW, labelH);
    labelY += labelH;

    label_Instrument->setBounds(leftX, labelY, labelW, labelH);
    label_InstrumentValue->setBounds(rightX, labelY, labelW, labelH);
    labelY += labelH;

    label_Frequency->setBounds(leftX, labelY, labelW, labelH);
    label_FrequencyValue->setBounds(rightX, labelY, labelW, labelH);

    // Bottom buttons
    int btnSize = (int)(32.0f * s);
    int btnY = bgTop + bgH - btnSize - (int)(16.0f * s);
    button_Settings->setBounds(bgMargin + (int)(16.0f * s), btnY, btnSize, btnSize);
    button_Mute->setBounds(bgMargin + (int)(136.0f * s), btnY, btnSize, btnSize);
    button_Loop->setBounds(bgMargin + (int)(247.0f * s), btnY, (int)(39.0f * s), (int)(35.0f * s));

    // Shortcut ref label at bottom
    shortcutRefLabel->setBounds(0, bgTop + bgH + (int)(5.0f * s), w, labelH);

    // Add button (inactive state)
    int addSize = (int)(71.0f * s);
    button_Add->setBounds((w - addSize) / 2, bgTop + (int)(100.0f * s), addSize, addSize);

    // Fonts
    label_Chord->setFont(fontSize);
    label_Chordtype->setFont(fontSize);
    label_Scale->setFont(fontSize);
    label_Instrument->setFont(fontSize);
    label_Frequency->setFont(fontSize);
    label_ChordValue->setFont(fontSize);
    label_ChordtypeValue->setFont(fontSize);
    label_ScaleValue->setFont(fontSize);
    label_InstrumentValue->setFont(fontSize);
    label_FrequencyValue->setFont(fontSize);
    shortcutRefLabel->setFont((int)(28.0f * s));
}


void ShortcutComponent::buttonClicked (Button*button)
{
    if (button == button_Settings.get())
    {
        shortcutListeners.call(&ShortcutListener::openChordPlayerSettingsForShortcut, shortcutRef);
    }
    else if (button == button_Loop.get())
    {
        projectManager->setChordPlayerParameter(shortcutRef, SHORTCUT_PLAY_AT_SAME_TIME, !button_Loop->getToggleState());
    }
    else if (button == button_Mute.get())
    {
        projectManager->setChordPlayerParameter(shortcutRef, SHORTCUT_MUTE, !button_Mute->getToggleState());
    }
    else if (button == button_Delete.get())
    {
        projectManager->setChordPlayerParameter(shortcutRef, SHORTCUT_IS_ACTIVE, false);
        
        
        
//         projectManager->reorderShortcuts();
    }
    else if (button == button_Add.get())
    {
        projectManager->initDefaultChordPlayerParametersForShortcut(shortcutRef);
        
        
        shortcutListeners.call(&ShortcutListener::openChordPlayerSettingsForShortcut, shortcutRef);
    }
}

void ShortcutComponent::setState(bool isActive)
{
    activeState = isActive;
    
    if (activeState)
    {
        containerView_Active    ->setVisible(true);
        containerView_Inactive  ->setVisible(false);
    }
    else
    {
        containerView_Active    ->setVisible(false);
        containerView_Inactive  ->setVisible(true);
    }
}

void ShortcutComponent::setMute(bool isMuted)
{
    muted = isMuted; button_Mute->setToggleState(muted, dontSendNotification);
}

void ShortcutComponent::setLoop(bool isLooped)
{
    loop = isLooped; button_Loop->setToggleState(loop, dontSendNotification);
}

void ShortcutComponent::setChordString(String c)
{
    chord = c; label_ChordValue->setText(chord, dontSendNotification);
}
void ShortcutComponent::setChordTypeString(String type)
{
    chordType = type; label_ChordtypeValue->setText(chordType, dontSendNotification);
}
void ShortcutComponent::setFrequency(float newFrequency)
{
    String freq(newFrequency, 3, false);
    freq.append("Hz", 2);
    frequencyHz = freq;
    label_FrequencyValue->setText(frequencyHz, dontSendNotification);
}

void ShortcutComponent::setInstrumentString(String instrumentString, String titleString)
{
    label_Instrument->setText(titleString, dontSendNotification);
    
    instString = instrumentString; label_InstrumentValue->setText(instString, dontSendNotification);
}

void ShortcutComponent::setScaleString(String newScaleString)
{
    scaleString = newScaleString; label_ScaleValue->setText(scaleString, dontSendNotification);
}


ShortcutContainerComponent::ShortcutContainerComponent(ProjectManager * pm)
{
    projectManager = pm;
    
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
    addAndMakeVisible(button_Left.get());
    
    button_Right = std::make_unique<ImageButton>();
    button_Right->setTriggeredOnMouseDown(true);
    button_Right->setImages (false, true, true,
                             imageRightButton, 0.999f, Colour (0x00000000),
                             Image(), 1.000f, Colour (0x00000000),
                             imageRightButton, 0.75, Colour (0x00000000));
    button_Right->addListener(this);
    addAndMakeVisible(button_Right.get());
    
    shortcutsContainer = std::make_unique<Component>();

    // shortcuts array

    for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
    {
        shortcutComponent[i] = std::make_unique<ShortcutComponent>(i, projectManager);
        shortcutsContainer->addAndMakeVisible(shortcutComponent[i].get());
    }

    // viewport
    viewport = std::make_unique<Viewport>();
    viewport            ->setViewedComponent(shortcutsContainer.get());
    viewport            ->setScrollBarsShown(false, false, false, true);
    addAndMakeVisible(viewport.get());
}

void ShortcutContainerComponent::resized()
{
    auto bounds = getLocalBounds();
    const int w = bounds.getWidth();
    const int h = bounds.getHeight();

    int btnW = jmax(20, (int)(w * 0.02f));
    int btnH = jmax(35, (int)(h * 0.14f));
    int btnY = (h - btnH) / 2;

    button_Left->setBounds(btnW, btnY, btnW, btnH);
    button_Right->setBounds(w - btnW * 2, btnY, btnW, btnH);

    int vpLeft = btnW * 3;
    int vpWidth = w - vpLeft * 2;

    int cardW = vpWidth / 4;  // show 4 cards at a time
    int containerW = cardW * NUM_SHORTCUT_SYNTHS;

    shortcutsContainer->setBounds(0, 0, containerW, h);

    for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
    {
        shortcutComponent[i]->setBounds(cardW * i, 0, cardW, h);
        shortcutComponent[i]->setScale(scaleFactor);
    }

    viewport->setBounds(vpLeft, 0, vpWidth, h);
}

void ShortcutContainerComponent::buttonClicked (Button*button)
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

void ShortcutContainerComponent::moveViewport()
{
    int cardW = viewport->getWidth() / 4;
    int x = cardW * shiftToRight;
    viewport->setViewPosition(x, 0);
}


// Frequency

ShortcutComponentFrequency::ShortcutComponentFrequency(int shortcut, ProjectManager * pm)
{
    projectManager  = pm;
    shortcutRef     = shortcut;
    

    waveType = "";
    frequencyHz = "";
    
    // Font
    Typeface::Ptr AssistantLight     = Typeface::createSystemTypefaceFor(BinaryData::AssistantLight_ttf, BinaryData::AssistantLight_ttfSize);
    Typeface::Ptr AssistantSemiBold   = Typeface::createSystemTypefaceFor(BinaryData::AssistantSemiBold_ttf, BinaryData::AssistantSemiBold_ttfSize);
    Font fontSemiBold(AssistantSemiBold);
    Font fontLight(AssistantLight);
    
    fontSemiBold.setHeight(33);
    fontLight.setHeight(33);
    
    // Images
    imageSettingsIcon       = ImageCache::getFromMemory(BinaryData::settings2x_png, BinaryData::settings2x_pngSize);
    imageMuteIcon           = ImageCache::getFromMemory(BinaryData::ShortcutMute2x_png, BinaryData::ShortcutMute2x_pngSize);
    imageLoopIcon           = ImageCache::getFromMemory(BinaryData::ShortcutLoop2x_png, BinaryData::ShortcutLoop2x_pngSize);

    imageDelete             = ImageCache::getFromMemory(BinaryData::ShortcutClose2x_png, BinaryData::ShortcutClose2x_pngSize);
    imageAddIcon            = ImageCache::getFromMemory(BinaryData::ShortcutAdd2x_png, BinaryData::ShortcutAdd2x_pngSize);
    
    
    
    
    // containers
    containerView_Active = std::make_unique<Component>();
    addAndMakeVisible(containerView_Active.get());

    containerView_Inactive = std::make_unique<Component>();
    addAndMakeVisible(containerView_Inactive.get());

    // active buttons
    button_Settings = std::make_unique<ImageButton>();
    button_Settings->setTriggeredOnMouseDown(true);
    button_Settings->setImages (false, true, true,
                                imageSettingsIcon, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageSettingsIcon, 0.75, Colour (0x00000000));
    button_Settings->addListener(this);
    containerView_Active->addAndMakeVisible(button_Settings.get());

    button_Mute = std::make_unique<ImageButton>();
    button_Mute->setTriggeredOnMouseDown(true);
    button_Mute->setImages (false, true, true,
                            imageMuteIcon, 0.5f, Colour (0x00000000),
                            Image(), 1.000f, Colour (0x00000000),
                            imageMuteIcon, 0.99, Colour (0x00000000));
    button_Mute->addListener(this);
    containerView_Active->addAndMakeVisible(button_Mute.get());

    button_Loop = std::make_unique<ImageButton>();
    button_Loop->setTriggeredOnMouseDown(true);
    button_Loop->setImages (false, true, true,
                            imageLoopIcon, 0.5f, Colour (0x00000000),
                            Image(), 1.000f, Colour (0x00000000),
                            imageLoopIcon, 0.999, Colour (0x00000000));
    button_Loop->addListener(this);


    button_Delete = std::make_unique<ImageButton>();
    button_Delete->setTriggeredOnMouseDown(true);
    button_Delete->setImages (false, true, true,
                              imageDelete, 0.999f, Colour (0x00000000),
                              Image(), 1.000f, Colour (0x00000000),
                              imageDelete, 0.75, Colour (0x00000000));
    button_Delete->addListener(this);
    containerView_Active->addAndMakeVisible(button_Delete.get());


    label_Wavetype = std::make_unique<Label>();
    label_Wavetype->setText("Wave type", dontSendNotification);
    label_Wavetype->setJustificationType(Justification::left);
    fontSemiBold.setHeight(33);
    label_Wavetype->setFont(fontSemiBold);
    containerView_Active->addAndMakeVisible(label_Wavetype.get());


    label_Frequency = std::make_unique<Label>();
    label_Frequency->setText("Frequency", dontSendNotification);
    label_Frequency->setJustificationType(Justification::left);
    fontSemiBold.setHeight(33);
    label_Frequency->setFont(fontSemiBold);
    containerView_Active->addAndMakeVisible(label_Frequency.get());

    label_WavetypeValue = std::make_unique<Label>();
    label_WavetypeValue->setText(waveType, dontSendNotification);
    label_WavetypeValue->setJustificationType(Justification::right);
    label_WavetypeValue->setFont(fontLight);
    containerView_Active->addAndMakeVisible(label_WavetypeValue.get());


    label_FrequencyValue = std::make_unique<Label>();
    label_FrequencyValue->setText(frequencyHz, dontSendNotification);
    label_FrequencyValue->setJustificationType(Justification::right);
    label_FrequencyValue->setFont(fontLight);
    containerView_Active->addAndMakeVisible(label_FrequencyValue.get());


    shortcutRefLabel = std::make_unique<Label>();
    String shortcutString("Shortcut ");
    shortcutString.append(ProjectStrings::getShortcuts().getReference(shortcutRef), 2);
    shortcutRefLabel->setText(shortcutString, dontSendNotification);
    shortcutRefLabel->setJustificationType(Justification::centred);
    fontLight.setHeight(33);
    shortcutRefLabel->setFont(fontLight);
    containerView_Active->addAndMakeVisible(shortcutRefLabel.get());


    // inactive button
    button_Add = std::make_unique<ImageButton>();
    button_Add->setTriggeredOnMouseDown(true);
    button_Add->setImages (false, true, true,
                           imageAddIcon, 0.999f, Colour (0x00000000),
                           Image(), 1.000f, Colour (0x00000000),
                           imageAddIcon, 0.75, Colour (0x00000000));
    button_Add->addListener(this);
    containerView_Inactive->addAndMakeVisible(button_Add.get());
    
    setState(0);
    
}

void ShortcutComponentFrequency::resized()
{
    auto bounds = getLocalBounds();
    const int w = bounds.getWidth();
    const int h = bounds.getHeight();
    const float s = w / 335.0f;

    int fontSize = (int)(28.0f * s);
    int bgMargin = (int)(16.0f * s);
    int bgTop = (int)(38.0f * s);
    int bgH = (int)(271.0f * s);

    containerView_Active->setBounds(bounds);
    containerView_Inactive->setBounds(bounds);

    button_Delete->setBounds(0, (int)(21.0f * s), (int)(35.0f * s), (int)(35.0f * s));

    int labelY = (int)(94.0f * s);
    int labelH = (int)(40.0f * s);
    int leftX = (int)(24.0f * s);
    int labelW = (int)(120.0f * s);
    int rightX = w - leftX - labelW;

    label_Wavetype->setBounds(leftX, labelY, labelW, labelH);
    label_WavetypeValue->setBounds(rightX, labelY, labelW, labelH);
    label_Wavetype->setFont(fontSize);
    label_WavetypeValue->setFont(fontSize);
    labelY += labelH;

    label_Frequency->setBounds(leftX, labelY, labelW, labelH);
    label_FrequencyValue->setBounds(rightX, labelY, labelW, labelH);
    label_Frequency->setFont(fontSize);
    label_FrequencyValue->setFont(fontSize);

    int btnSize = (int)(32.0f * s);
    int btnY = bgTop + bgH - btnSize - (int)(16.0f * s);
    button_Settings->setBounds(bgMargin + (int)(16.0f * s), btnY, btnSize, btnSize);
    button_Mute->setBounds(bgMargin + (int)(136.0f * s), btnY, btnSize, btnSize);
    button_Loop->setBounds(bgMargin + (int)(247.0f * s), btnY, (int)(39.0f * s), (int)(35.0f * s));

    shortcutRefLabel->setBounds(0, bgTop + bgH + (int)(5.0f * s), w, labelH);
    shortcutRefLabel->setFont(fontSize);

    int addSize = (int)(71.0f * s);
    button_Add->setBounds((w - addSize) / 2, bgTop + (int)(100.0f * s), addSize, addSize);
}

void ShortcutComponentFrequency::buttonClicked (Button*button)
{
    if (button == button_Settings.get())
    {
        shortcutListeners.call(&ShortcutFrequencyListener::openFrequencyPlayerSettingsForShortcut, shortcutRef);
    }
    else if (button == button_Loop.get())
    {
        projectManager->setFrequencyPlayerParameter(shortcutRef, FREQUENCY_PLAYER_SHORTCUT_PLAY_AT_SAME_TIME, !button_Loop->getToggleState());
    }
    else if (button == button_Mute.get())
    {
        projectManager->setFrequencyPlayerParameter(shortcutRef, FREQUENCY_PLAYER_SHORTCUT_MUTE, !button_Mute->getToggleState());
    }
    else if (button == button_Delete.get())
    {
        projectManager->setFrequencyPlayerParameter(shortcutRef, FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE, false);
        
       
    }
    else if (button == button_Add.get())
    {

        projectManager->initDefaultFrequencyPlayerParametersForShortcut(shortcutRef);
        
        shortcutListeners.call(&ShortcutFrequencyListener::openFrequencyPlayerSettingsForShortcut, shortcutRef);
    }
}

void ShortcutComponentFrequency::setState(bool isActive)
{
    activeState = isActive;
    
    if (activeState)
    {
        containerView_Active    ->setVisible(true);
        containerView_Inactive  ->setVisible(false);
    }
    else
    {
        containerView_Active    ->setVisible(false);
        containerView_Inactive  ->setVisible(true);
    }
}

void ShortcutComponentFrequency::setMute(bool isMuted)
{
    muted = isMuted; button_Mute->setToggleState(muted, dontSendNotification);
}

void ShortcutComponentFrequency::setLoop(bool isLooped)
{
    loop = isLooped; button_Loop->setToggleState(loop, dontSendNotification);
}

void ShortcutComponentFrequency::setWaveTypeString(String type)
{
    waveType = type; label_WavetypeValue->setText(waveType, dontSendNotification);
}
void ShortcutComponentFrequency::setFrequency(float newFrequency)
{
    String freq(newFrequency);
    freq.append("Hz", 2);
    frequencyHz = freq;
    label_FrequencyValue->setText(frequencyHz, dontSendNotification);
}


ShortcutFrequencyContainerComponent::ShortcutFrequencyContainerComponent(ProjectManager * pm)
{
    projectManager = pm;
    
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
    addAndMakeVisible(button_Left.get());

    button_Right = std::make_unique<ImageButton>();
    button_Right->setTriggeredOnMouseDown(true);
    button_Right->setImages (false, true, true,
                             imageRightButton, 0.999f, Colour (0x00000000),
                             Image(), 1.000f, Colour (0x00000000),
                             imageRightButton, 0.75, Colour (0x00000000));
    button_Right->addListener(this);
    addAndMakeVisible(button_Right.get());

    shortcutsContainer = std::make_unique<Component>();

    // shortcuts array

    for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
    {
        shortcutComponent[i] = std::make_unique<ShortcutComponentFrequency>(i, projectManager);
        shortcutsContainer->addAndMakeVisible(shortcutComponent[i].get());
    }

    // viewport
    viewport = std::make_unique<Viewport>();
    viewport            ->setViewedComponent(shortcutsContainer.get());
    viewport            ->setScrollBarsShown(false, false, false, true);
    addAndMakeVisible(viewport.get());
}

void ShortcutFrequencyContainerComponent::resized()
{
    auto bounds = getLocalBounds();
    const int w = bounds.getWidth();
    const int h = bounds.getHeight();

    int btnW = jmax(20, (int)(w * 0.02f));
    int btnH = jmax(35, (int)(h * 0.14f));
    int btnY = (h - btnH) / 2;

    button_Left->setBounds(btnW, btnY, btnW, btnH);
    button_Right->setBounds(w - btnW * 2, btnY, btnW, btnH);

    int vpLeft = btnW * 3;
    int vpWidth = w - vpLeft * 2;

    int cardW = vpWidth / 4;
    int containerW = cardW * NUM_SHORTCUT_SYNTHS;

    shortcutsContainer->setBounds(0, 0, containerW, h);

    for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
    {
        shortcutComponent[i]->setBounds(cardW * i, 0, cardW, h);
        shortcutComponent[i]->setScale(scaleFactor);
    }

    viewport->setBounds(vpLeft, 0, vpWidth, h);
}

void ShortcutFrequencyContainerComponent::buttonClicked (Button*button)
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

void ShortcutFrequencyContainerComponent::moveViewport()
{
    int cardW = viewport->getWidth() / 4;
    int x = cardW * shiftToRight;
    viewport->setViewPosition(x, 0);
}
