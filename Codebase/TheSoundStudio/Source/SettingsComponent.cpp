/*
  ==============================================================================

    SettingsComponent.cpp

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "SettingsComponent.h"
#include "PluginWindow.h"
#include "PopupFFTWindow.h"
#include "UI/PluginMenuBuilder.h"


//==============================================================================
SettingsComponent::SettingsComponent(ProjectManager * pm)
{
    projectManager = pm;
    projectManager->addUIListener(this);
    
    
    
    imageBlueButtonNormal               = ImageCache::getFromMemory(BinaryData::BlueButton_Normal_png, BinaryData::BlueButton_Normal_pngSize);
    imageBlueButtonSelected             = ImageCache::getFromMemory(BinaryData::BlueButton_Selected_png, BinaryData::BlueButton_Selected_pngSize);
    imageBlueCheckButtonNormal          = ImageCache::getFromMemory(BinaryData::Button_Checkbox_Normal_Max_png, BinaryData::Button_Checkbox_Normal_Max_pngSize);
    imageBlueCheckButtonSelected        = ImageCache::getFromMemory(BinaryData::Button_Checkbox_Selected_Max_png, BinaryData::Button_Checkbox_Selected_Max_pngSize);
    imagePluginPlayButton               = ImageCache::getFromMemory(BinaryData::ButtonPluginPlay, BinaryData::ButtonPluginPlaySize);
    
    imageButtonSave         = ImageCache::getFromMemory(BinaryData::ButtonSettingsSave, BinaryData::ButtonSettingsSaveSize);
    imageButtonLoad         = ImageCache::getFromMemory(BinaryData::ButtonSettingsLoad, BinaryData::ButtonSettingsLoadSize);
    imageButtonAudio        = ImageCache::getFromMemory(BinaryData::ButtonSettingsAudio, BinaryData::ButtonSettingsAudioSize);
    imageButtonChange       = ImageCache::getFromMemory(BinaryData::ButtonSettingsChange, BinaryData::ButtonSettingsChangeSize);
    imageButtonReset        = ImageCache::getFromMemory(BinaryData::ButtonSettingsResetToDefault, BinaryData::ButtonSettingsResetToDefaultSize);
    imageButtonScalesAdd    = ImageCache::getFromMemory(BinaryData::ButtonSettingsScalesAdd, BinaryData::ButtonSettingsScalesAddSize);
    imageButtonSpectrogram  = ImageCache::getFromMemory(BinaryData::ButtonSettingsSpectrogram, BinaryData::ButtonSettingsSpectrogramSize);
    imageButtonScan         = ImageCache::getFromMemory(BinaryData::ButtonSettingsScan, BinaryData::ButtonSettingsScanSize);
    
    
    // fonts
    Typeface::Ptr AssistantBold     = Typeface::createSystemTypefaceFor(BinaryData::AssistantBold_ttf, BinaryData::AssistantBold_ttfSize);
    Typeface::Ptr AssistantSemiBold   = Typeface::createSystemTypefaceFor(BinaryData::AssistantSemiBold_ttf, BinaryData::AssistantSemiBold_ttfSize);
    Font fontBold(AssistantBold);
    Font fontNormal(AssistantSemiBold);
    
    fontNormal.setHeight(40);
    fontBold.setHeight(30);

    textEditor_NotesFrequency = std::make_unique<TextEditor>();
    textEditor_NotesFrequency->setInputRestrictions(10, "0123456789.");
    textEditor_NotesFrequency->setBounds(288, 328, 117, 41);
    textEditor_NotesFrequency->setText("440");
    textEditor_NotesFrequency->addListener(this);
    textEditor_NotesFrequency->setLookAndFeel(&lookAndFeel);
    textEditor_NotesFrequency->setJustification(Justification::centred);
    textEditor_NotesFrequency->setFont(fontNormal);
    textEditor_NotesFrequency->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditor_NotesFrequency->applyFontToAllText(fontNormal);
    textEditor_NotesFrequency->applyColourToAllText(Colours::darkgrey);
    addAndMakeVisible(textEditor_NotesFrequency.get());
    
    
    // Locations Files
    
    label_LogFileLocation = std::make_unique<Label>("");
    label_LogFileLocation->setFont(fontNormal);
    label_LogFileLocation->setColour(Label::textColourId, Colours::lightgrey);
    label_LogFileLocation->setJustificationType(Justification::left);
    label_LogFileLocation->setText("file location", dontSendNotification);
    addAndMakeVisible(label_LogFileLocation.get());
    
    label_RecordFileLocation = std::make_unique<Label>("");
    label_RecordFileLocation->setFont(fontNormal);
    label_RecordFileLocation->setColour(Label::textColourId, Colours::lightgrey);
    label_RecordFileLocation->setJustificationType(Justification::left);
    addAndMakeVisible(label_RecordFileLocation.get());

    label_B_Frequency = std::make_unique<Label>("");
    label_B_Frequency->setFont(fontNormal);
    label_B_Frequency->setColour(Label::textColourId, Colours::lightgrey);
    label_B_Frequency->setJustificationType(Justification::centred);
    label_B_Frequency->setText("440", dontSendNotification);
    label_B_Frequency->setBounds(443, 328, 117, 41);
    addAndMakeVisible(label_B_Frequency.get());
    
    label_C_Frequency = std::make_unique<Label>("");
    label_C_Frequency->setFont(fontNormal);
    label_C_Frequency->setColour(Label::textColourId, Colours::lightgrey);
    label_C_Frequency->setJustificationType(Justification::centred);
    label_C_Frequency->setText("440", dontSendNotification);
    label_C_Frequency->setBounds(598, 328, 117, 41);
    addAndMakeVisible(label_C_Frequency.get());
    
    label_D_Frequency = std::make_unique<Label>("");
    label_D_Frequency->setFont(fontNormal);
    label_D_Frequency->setColour(Label::textColourId, Colours::lightgrey);
    label_D_Frequency->setJustificationType(Justification::centred);
    label_D_Frequency->setText("440", dontSendNotification);
    label_D_Frequency->setBounds(754, 328, 117, 41);
    addAndMakeVisible(label_D_Frequency.get());
    
    label_E_Frequency = std::make_unique<Label>("");
    label_E_Frequency->setFont(fontNormal);
    label_E_Frequency->setColour(Label::textColourId, Colours::lightgrey);
    label_E_Frequency->setJustificationType(Justification::centred);
    label_E_Frequency->setText("440", dontSendNotification);
    label_E_Frequency->setBounds(909, 328, 117, 41);
    addAndMakeVisible(label_E_Frequency.get());
    
    label_F_Frequency = std::make_unique<Label>("");
    label_F_Frequency->setFont(fontNormal);
    label_F_Frequency->setColour(Label::textColourId, Colours::lightgrey);
    label_F_Frequency->setJustificationType(Justification::centred);
    label_F_Frequency->setText("440", dontSendNotification);
    label_F_Frequency->setBounds(1064, 328, 117, 41);
    addAndMakeVisible(label_F_Frequency.get());
    
    label_G_Frequency = std::make_unique<Label>("");
    label_G_Frequency->setFont(fontNormal);
    label_G_Frequency->setColour(Label::textColourId, Colours::lightgrey);
    label_G_Frequency->setJustificationType(Justification::centred);
    label_G_Frequency->setText("440", dontSendNotification);
    label_G_Frequency->setBounds(1219, 328, 117, 41);
    addAndMakeVisible(label_G_Frequency.get());
    
    
    textEditor_AmplitudeMin = std::make_unique<TextEditor>();
    textEditor_AmplitudeMin->setInputRestrictions(10, "0123456789.");
    textEditor_AmplitudeMin->setBounds(242, 497, 125, 41);
    textEditor_AmplitudeMin->setText("0");
    textEditor_AmplitudeMin->addListener(this);
    textEditor_AmplitudeMin->setLookAndFeel(&lookAndFeel);
    textEditor_AmplitudeMin->setJustification(Justification::centred);
    textEditor_AmplitudeMin->setFont(fontNormal);
    textEditor_AmplitudeMin->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditor_AmplitudeMin->applyFontToAllText(fontNormal);
    textEditor_AmplitudeMin->applyColourToAllText(Colours::darkgrey);
    addAndMakeVisible(textEditor_AmplitudeMin.get());
    
    
    textEditor_AttackMin = std::make_unique<TextEditor>();
    textEditor_AttackMin->setInputRestrictions(10, "0123456789.");
    textEditor_AttackMin->setBounds(242, 596, 125, 41);
    textEditor_AttackMin->setText("0");
    textEditor_AttackMin->addListener(this);
    textEditor_AttackMin->setLookAndFeel(&lookAndFeel);
    textEditor_AttackMin->setJustification(Justification::centred);
    textEditor_AttackMin->setFont(fontNormal);
    textEditor_AttackMin->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditor_AttackMin->applyFontToAllText(fontNormal);
    textEditor_AttackMin->applyColourToAllText(Colours::darkgrey);
    addAndMakeVisible(textEditor_AttackMin.get());
    
    textEditor_DecayMin = std::make_unique<TextEditor>();
    textEditor_DecayMin->setInputRestrictions(10, "0123456789.");
    textEditor_DecayMin->setBounds(242, 660, 125, 41);
    textEditor_DecayMin->setText("0");
    textEditor_DecayMin->addListener(this);
    textEditor_DecayMin->setLookAndFeel(&lookAndFeel);
    textEditor_DecayMin->setJustification(Justification::centred);
    textEditor_DecayMin->setFont(fontNormal);
    textEditor_DecayMin->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditor_DecayMin->applyFontToAllText(fontNormal);
    textEditor_DecayMin->applyColourToAllText(Colours::darkgrey);
    addAndMakeVisible(textEditor_DecayMin.get());
    
    textEditor_SustainMin = std::make_unique<TextEditor>();
    textEditor_SustainMin->setInputRestrictions(10, "0123456789.");
    textEditor_SustainMin->setBounds(242, 723, 125, 41);
    textEditor_SustainMin->setText("0");
    textEditor_SustainMin->addListener(this);
    textEditor_SustainMin->setLookAndFeel(&lookAndFeel);
    textEditor_SustainMin->setJustification(Justification::centred);
    textEditor_SustainMin->setFont(fontNormal);
    textEditor_SustainMin->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditor_SustainMin->applyFontToAllText(fontNormal);
    textEditor_SustainMin->applyColourToAllText(Colours::darkgrey);
    addAndMakeVisible(textEditor_SustainMin.get());
    
    textEditor_ReleaseMin = std::make_unique<TextEditor>();
    textEditor_ReleaseMin->setInputRestrictions(10, "0123456789.");
    textEditor_ReleaseMin->setBounds(242, 787, 125, 41);
    textEditor_ReleaseMin->setText("0");
    textEditor_ReleaseMin->addListener(this);
    textEditor_ReleaseMin->setLookAndFeel(&lookAndFeel);
    textEditor_ReleaseMin->setJustification(Justification::centred);
    textEditor_ReleaseMin->setFont(fontNormal);
    textEditor_ReleaseMin->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditor_ReleaseMin->applyFontToAllText(fontNormal);
    textEditor_ReleaseMin->applyColourToAllText(Colours::darkgrey);
    addAndMakeVisible(textEditor_ReleaseMin.get());
    
    
    textEditor_AmplitudeMax = std::make_unique<TextEditor>();
    textEditor_AmplitudeMax->setInputRestrictions(10, "0123456789.");
    textEditor_AmplitudeMax->setBounds(432, 497, 125, 41);
    textEditor_AmplitudeMax->setText("100");
    textEditor_AmplitudeMax->addListener(this);
    textEditor_AmplitudeMax->setLookAndFeel(&lookAndFeel);
    textEditor_AmplitudeMax->setJustification(Justification::centred);
    textEditor_AmplitudeMax->setFont(fontNormal);
    textEditor_AmplitudeMax->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditor_AmplitudeMax->applyFontToAllText(fontNormal);
    textEditor_AmplitudeMax->applyColourToAllText(Colours::darkgrey);
    addAndMakeVisible(textEditor_AmplitudeMax.get());
    
    textEditor_AttackMax = std::make_unique<TextEditor>();
    textEditor_AttackMax->setInputRestrictions(10, "0123456789.");
    textEditor_AttackMax->setBounds(432, 596, 125, 41);
    textEditor_AttackMax->setText("10000");
    textEditor_AttackMax->addListener(this);
    textEditor_AttackMax->setLookAndFeel(&lookAndFeel);
    textEditor_AttackMax->setJustification(Justification::centred);
    textEditor_AttackMax->setFont(fontNormal);
    textEditor_AttackMax->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditor_AttackMax->applyFontToAllText(fontNormal);
    textEditor_AttackMax->applyColourToAllText(Colours::darkgrey);
    addAndMakeVisible(textEditor_AttackMax.get());
    
    textEditor_DecayMax = std::make_unique<TextEditor>();
    textEditor_DecayMax->setInputRestrictions(10, "0123456789.");
    textEditor_DecayMax->setBounds(432, 660, 125, 41);
    textEditor_DecayMax->setText("10000");
    textEditor_DecayMax->addListener(this);
    textEditor_DecayMax->setLookAndFeel(&lookAndFeel);
    textEditor_DecayMax->setJustification(Justification::centred);
    textEditor_DecayMax->setFont(fontNormal);
    textEditor_DecayMax->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditor_DecayMax->applyFontToAllText(fontNormal);
    textEditor_DecayMax->applyColourToAllText(Colours::darkgrey);
    addAndMakeVisible(textEditor_DecayMax.get());
    
    textEditor_SustainMax = std::make_unique<TextEditor>();
    textEditor_SustainMax->setInputRestrictions(10, "0123456789.");
    textEditor_SustainMax->setBounds(432, 723, 125, 41);
    textEditor_SustainMax->setText("100");
    textEditor_SustainMax->addListener(this);
    textEditor_SustainMax->setLookAndFeel(&lookAndFeel);
    textEditor_SustainMax->setJustification(Justification::centred);
    textEditor_SustainMax->setFont(fontNormal);
    textEditor_SustainMax->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditor_SustainMax->applyFontToAllText(fontNormal);
    textEditor_SustainMax->applyColourToAllText(Colours::darkgrey);
    addAndMakeVisible(textEditor_SustainMax.get());
    
    textEditor_ReleaseMax = std::make_unique<TextEditor>();
    textEditor_ReleaseMax->setInputRestrictions(10, "0123456789.");
    textEditor_ReleaseMax->setBounds(432, 787, 125, 41);
    textEditor_ReleaseMax->setText("10000");
    textEditor_ReleaseMax->addListener(this);
    textEditor_ReleaseMax->setLookAndFeel(&lookAndFeel);
    textEditor_ReleaseMax->setJustification(Justification::centred);
    textEditor_ReleaseMax->setFont(fontNormal);
    textEditor_ReleaseMax->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditor_ReleaseMax->applyFontToAllText(fontNormal);
    textEditor_ReleaseMax->applyColourToAllText(Colours::darkgrey);
    addAndMakeVisible(textEditor_ReleaseMax.get());
    
    int buttonW = 260;
    textButton_ResetAmp = std::make_unique<ImageButton>();
    textButton_ResetAmp->setTriggeredOnMouseDown(true);
    textButton_ResetAmp->setImages (false, true, true,
                                imageButtonReset, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageButtonReset, 1.0, Colour (0x00000000));
    textButton_ResetAmp->addListener(this);
    addAndMakeVisible(textButton_ResetAmp.get());

    textButton_ResetAttack = std::make_unique<ImageButton>();
    textButton_ResetAttack->setTriggeredOnMouseDown(true);
    textButton_ResetAttack->setImages (false, true, true,
                                                imageButtonReset, 0.999f, Colour (0x00000000),
                                                Image(), 1.000f, Colour (0x00000000),
                                                imageButtonReset, 1.0, Colour (0x00000000));
    textButton_ResetAttack->addListener(this);
    addAndMakeVisible(textButton_ResetAttack.get());
    
    textButton_ResetDecay = std::make_unique<ImageButton>();
    textButton_ResetDecay->setTriggeredOnMouseDown(true);
    textButton_ResetDecay->setImages (false, true, true,
                                                imageButtonReset, 0.999f, Colour (0x00000000),
                                                Image(), 1.000f, Colour (0x00000000),
                                                imageButtonReset, 1.0, Colour (0x00000000));
    textButton_ResetDecay->addListener(this);
    addAndMakeVisible(textButton_ResetDecay.get());
    
    textButton_ResetSustain = std::make_unique<ImageButton>();
    textButton_ResetSustain->setTriggeredOnMouseDown(true);
    textButton_ResetSustain->setImages (false, true, true,
                                                imageButtonReset, 0.999f, Colour (0x00000000),
                                                Image(), 1.000f, Colour (0x00000000),
                                                imageButtonReset, 1.0, Colour (0x00000000));
    
    textButton_ResetSustain->addListener(this);
    addAndMakeVisible(textButton_ResetSustain.get());
    
    textButton_ResetRelease = std::make_unique<ImageButton>();
    textButton_ResetRelease->setTriggeredOnMouseDown(true);
    textButton_ResetRelease->setImages (false, true, true,
                                                imageButtonReset, 0.999f, Colour (0x00000000),
                                                Image(), 1.000f, Colour (0x00000000),
                                                imageButtonReset, 1.0, Colour (0x00000000));
    textButton_ResetRelease->addListener(this);
    addAndMakeVisible(textButton_ResetRelease.get());

    textButton_ChangeLogFile = std::make_unique<ImageButton>();
    textButton_ChangeLogFile->setTriggeredOnMouseDown(true);
    textButton_ChangeLogFile->setImages (false, true, true,
                                         imageButtonChange, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                         imageButtonChange, 1.0, Colour (0x00000000));
    textButton_ChangeLogFile->addListener(this);
    addAndMakeVisible(textButton_ChangeLogFile.get());
    
    textButton_ChangeRecordFile = std::make_unique<ImageButton>();
    textButton_ChangeRecordFile->setTriggeredOnMouseDown(true);
    textButton_ChangeRecordFile->setImages (false, true, true,
                                imageButtonChange, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                            imageButtonChange, 1.0, Colour (0x00000000));
    textButton_ChangeRecordFile->addListener(this);
    addAndMakeVisible(textButton_ChangeRecordFile.get());
    
    buttonMixer = std::make_unique<TextButton>();
    buttonMixer->setTriggeredOnMouseDown(true);
    buttonMixer->addListener(this);
    buttonMixer->setButtonText("Mixer");
    addAndMakeVisible(buttonMixer.get());
    
    
    // audio setting stuff
    textButton_OpenAudioSettings = std::make_unique<ImageButton>();
    textButton_OpenAudioSettings->setTriggeredOnMouseDown(true);
    textButton_OpenAudioSettings->setImages (false, true, true,
                                imageButtonAudio, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageButtonAudio, 1.0, Colour (0x00000000));
    textButton_OpenAudioSettings->addListener(this);
    addAndMakeVisible(textButton_OpenAudioSettings.get());
    
    textButton_SpectrogramSetttingsPopup = std::make_unique<ImageButton>();
    textButton_SpectrogramSetttingsPopup->setTriggeredOnMouseDown(true);
    textButton_SpectrogramSetttingsPopup->setImages (false, true, true,
                                imageButtonSpectrogram, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                                     imageButtonSpectrogram, 1.0, Colour (0x00000000));
    textButton_SpectrogramSetttingsPopup->addListener(this);
    addAndMakeVisible(textButton_SpectrogramSetttingsPopup.get());
    

    comboBox_Scales = std::make_unique<ComboBox>();
    comboBox_Scales->setLookAndFeel(&lookAndFeel);
    
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
    //    comboBoxChordtype->setLookAndFeel(&lookAndFeel);
    comboBox_Scales->setBounds(1000, 497, 250, 41); //(242, 497, 125, 41);
    addAndMakeVisible(comboBox_Scales.get());

    
    comboBox_RecordFormat = std::make_unique<ComboBox>();
    comboBox_RecordFormat->addListener(this);
    PopupMenu * recordFormatMenu =  comboBox_RecordFormat->getRootMenu();
    comboBox_RecordFormat->setLookAndFeel(&lookAndFeel);
    
    // get audio record formats

    
    for (int i = 0; i < projectManager->formatManager.getNumKnownFormats(); i++)
    {
        String formatName = projectManager->formatManager.getKnownFormat(i)->getFormatName();
        
        recordFormatMenu->addItem(i+1, formatName);
    }
    
    
    addAndMakeVisible(comboBox_RecordFormat.get());
    comboBox_RecordFormat->setBounds(1080, 140, 250, 41);
    

    comboBox_NoiseType = std::make_unique<ComboBox>();
    comboBox_NoiseType->addListener(this);
    PopupMenu * noiseTypeMenu =  comboBox_NoiseType->getRootMenu();
    comboBox_NoiseType->setLookAndFeel(&lookAndFeel);
    noiseTypeMenu->addItem(1, "White");
    noiseTypeMenu->addItem(2, "Pink");
    addAndMakeVisible(comboBox_NoiseType.get());
    
    imageAddPluginIcon      = ImageCache::getFromMemory(BinaryData::ShortcutAdd2x_png, BinaryData::ShortcutAdd2x_pngSize);
    removePluginIcon        = ImageCache::getFromMemory(BinaryData::CloseButton2x_png, BinaryData::CloseButton2x_pngSize);
    openWindowPluginIcon    = ImageCache::getFromMemory(BinaryData::Button_SpectrumOpen_png, BinaryData::Button_SpectrumOpen_pngSize);
    
    for (int i = 0; i < NUM_PLUGIN_SLOTS; i++)
    {
        labelPluginSlot[i] = new Label("");
        labelPluginSlot[i]->setFont(fontNormal);
        labelPluginSlot[i]->setColour(Label::textColourId, Colours::lightgrey);
        labelPluginSlot[i]->setJustificationType(Justification::centredLeft);
        String plugString(String::formatted("Plugin %i", i+1));
        labelPluginSlot[i]->setText(plugString, dontSendNotification);;
        addAndMakeVisible(labelPluginSlot[i]);
        
        
        
        buttonAddPlugin[i] = new ImageButton();
        buttonAddPlugin[i]->addListener(this);
        buttonAddPlugin[i]->setTriggeredOnMouseDown(true);
        buttonAddPlugin[i]->setImages (false, true, true,
                                       imageAddPluginIcon, 0.999f, Colour (0x00000000),
                                       Image(), 1.000f, Colour (0x00000000),
                                       imageAddPluginIcon, 0.999f, Colour (0x00000000));
        addAndMakeVisible(buttonAddPlugin[i]);
        
        
        buttonRemovePlugin[i] = new ImageButton();
        buttonRemovePlugin[i]->addListener(this);
        buttonRemovePlugin[i]->setTriggeredOnMouseDown(true);
        buttonRemovePlugin[i]->setImages (false, true, true,
                                       removePluginIcon, 0.999f, Colour (0x00000000),
                                       Image(), 1.000f, Colour (0x00000000),
                                       removePluginIcon, 0.999f, Colour (0x00000000));
        addAndMakeVisible(buttonRemovePlugin[i]);
        
        
        buttonOpenPlugin[i] = new ImageButton();
        buttonOpenPlugin[i]->addListener(this);
        buttonOpenPlugin[i]->setTriggeredOnMouseDown(true);
        buttonOpenPlugin[i]->setImages (false, true, true,
                                       imagePluginPlayButton, 0.999f, Colour (0x00000000),
                                       Image(), 1.000f, Colour (0x00000000),
                                       imagePluginPlayButton, 0.999f, Colour (0x00000000));
        addAndMakeVisible(buttonOpenPlugin[i]);
        
        
        comboBoxPluginSelector[i] = new ComboBox();
        comboBoxPluginSelector[i]->setLookAndFeel(&lookAndFeel);
        comboBoxPluginSelector[i]->addListener(this);
        
        TSS::UI::populatePluginMenu(*comboBoxPluginSelector[i]->getRootMenu(),
                                    *projectManager->knownPluginList, &lookAndFeel);
        addAndMakeVisible(comboBoxPluginSelector[i]);
    }
    
    scanPluginsButton = std::make_unique<ImageButton>();
    scanPluginsButton->setTriggeredOnMouseDown(true);
    scanPluginsButton->setImages (false, true, true,
                                imageButtonScan, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageButtonScan, 1.0, Colour (0x00000000));
    scanPluginsButton->addListener(this);
    addAndMakeVisible(scanPluginsButton.get());

    
    
    
    // Load / Save Button
    button_Load = std::make_unique<ImageButton>();
    button_Load->setTriggeredOnMouseDown(true);
    button_Load->setImages (false, true, true,
                                imageButtonLoad, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageButtonLoad, 1.0, Colour (0x00000000));
    button_Load->addListener(this);
    addAndMakeVisible(button_Load.get());
    
    button_Save = std::make_unique<ImageButton>();
    button_Save->setTriggeredOnMouseDown(true);
    button_Save->setImages (false, true, true,
                                imageButtonSave, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageButtonSave, 1.0, Colour (0x00000000));
    button_Save->addListener(this);
    addAndMakeVisible(button_Save.get());
    
    // Radio Buttons
    
    button_FreqToChordMainHarmonics = std::make_unique<ImageButton>();
    button_FreqToChordMainHarmonics->setTriggeredOnMouseDown(true);
    button_FreqToChordMainHarmonics->setImages (false, true, true,
                                imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_FreqToChordMainHarmonics->addListener(this);
    button_FreqToChordMainHarmonics->setBounds(639, 0, 38, 38);
    addAndMakeVisible(button_FreqToChordMainHarmonics.get());
    
    button_FreqToChordAverage = std::make_unique<ImageButton>();
    button_FreqToChordAverage->setTriggeredOnMouseDown(true);
    button_FreqToChordAverage->setImages (false, true, true,
                                imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_FreqToChordAverage->addListener(this);
    button_FreqToChordAverage->setBounds(639, 0, 38, 38);
    addAndMakeVisible(button_FreqToChordAverage.get());
    
    button_FreqToChordEMA = std::make_unique<ImageButton>();
    button_FreqToChordEMA->setTriggeredOnMouseDown(true);
    button_FreqToChordEMA->setImages (false, true, true,
                                imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_FreqToChordEMA->addListener(this);
    button_FreqToChordEMA->setBounds(639, 0, 38, 38);
    addAndMakeVisible(button_FreqToChordEMA.get());

    
   // new scale buttons
    
    imageButtonScale25Normal    = ImageCache::getFromMemory(BinaryData::ButtonScale25Normal, BinaryData::ButtonScale25NormalSize);
    imageButtonScale25Selected  = ImageCache::getFromMemory(BinaryData::ButtonScale25Selected, BinaryData::ButtonScale25SelectedSize);
    
    imageButtonScale50Normal    = ImageCache::getFromMemory(BinaryData::ButtonScale50Normal, BinaryData::ButtonScale50NormalSize);
    imageButtonScale50Selected  = ImageCache::getFromMemory(BinaryData::ButtonScale50Selected, BinaryData::ButtonScale50SelectedSize);
    
    imageButtonScale75Normal    = ImageCache::getFromMemory(BinaryData::ButtonScale75Normal, BinaryData::ButtonScale75NormalSize);
    imageButtonScale75Selected  = ImageCache::getFromMemory(BinaryData::ButtonScale75Selected, BinaryData::ButtonScale75SelectedSize);
    
    imageButtonScale100Normal   = ImageCache::getFromMemory(BinaryData::ButtonScale100Normal, BinaryData::ButtonScale100NormalSize);
    imageButtonScale100Selected = ImageCache::getFromMemory(BinaryData::ButtonScale100Selected, BinaryData::ButtonScale100SelectedSize);
    
    buttonScale25 = std::make_unique<ImageButton>();
    buttonScale25->setTriggeredOnMouseDown(true);
    buttonScale25->setImages (false, true, true,
                           imageButtonScale25Normal, 0.999f, Colour (0x00000000),
                           Image(), 1.000f, Colour (0x00000000),
                              imageButtonScale25Selected, 0.75, Colour (0x00000000));
    buttonScale25->addListener(this);
    addAndMakeVisible(buttonScale25.get());
    
    buttonScale50 = std::make_unique<ImageButton>();
    buttonScale50->setTriggeredOnMouseDown(true);
    buttonScale50->setImages (false, true, true,
                              imageButtonScale50Normal, 0.999f, Colour (0x00000000),
                           Image(), 1.000f, Colour (0x00000000),
                              imageButtonScale50Selected, 0.75, Colour (0x00000000));
    buttonScale50->addListener(this);
    addAndMakeVisible(buttonScale50.get());
    
    buttonScale75 = std::make_unique<ImageButton>();
    buttonScale75->setTriggeredOnMouseDown(true);
    buttonScale75->setImages (false, true, true,
                              imageButtonScale75Normal, 0.999f, Colour (0x00000000),
                           Image(), 1.000f, Colour (0x00000000),
                              imageButtonScale75Selected, 0.75, Colour (0x00000000));
    buttonScale75->addListener(this);
    addAndMakeVisible(buttonScale75.get());
    
    buttonScale100 = std::make_unique<ImageButton>();
    buttonScale100->setTriggeredOnMouseDown(true);
    buttonScale100->setImages (false, true, true,
                               imageButtonScale100Normal, 0.999f, Colour (0x00000000),
                           Image(), 1.000f, Colour (0x00000000),
                               imageButtonScale100Selected, 0.75, Colour (0x00000000));
    buttonScale100->addListener(this);
    addAndMakeVisible(buttonScale100.get());

    // Fundamental Frequency
    fundamentalFrequencyAlgorithmChooser.addItemList(ProjectStrings::getFundamentalFrequencyAlgorithms(), 1);
    fundamentalFrequencyAlgorithmChooser.addListener(this);
    fundamentalFrequencyAlgorithmChooser.setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(fundamentalFrequencyAlgorithmChooser);

    // new text editors
    
    // Audio Settings component must be last in the list as it is a popover..
    // **********************************************************************
    
    component_AudioSettings = new AudioSettingsContainer();
    component_AudioSettings->setBounds(0, 0, 1566, 1096+344);
    addAndMakeVisible(component_AudioSettings);
    component_AudioSettings->setVisible(false);
    
    audioSetupComponent = new AudioDeviceSelectorComponent(*projectManager->getDeviceManager(),
                                                            0, 256, 0, 256, true, true, true, false);
    audioSetupComponent->setBounds(60, 300, 1300, 1000);
    audioSetupComponent->setItemHeight(70);
    component_AudioSettings->addAndMakeVisible(audioSetupComponent);
    
    // audioSetupComponent->setLookAndFeel(&lookAndFeel);
    
    spectrogramPopupComponent = new SpectrogramSettingsPopup(projectManager);
    addAndMakeVisible(spectrogramPopupComponent);
    spectrogramPopupComponent->setVisible(false);
    
    
    // grab default settings
    projectManager->syncSettingsGUI();
    
}

SettingsComponent::~SettingsComponent()
{
    textEditor_NotesFrequency->setLookAndFeel(nullptr);
    textEditor_AmplitudeMin->setLookAndFeel(nullptr);
    textEditor_AttackMin->setLookAndFeel(nullptr);
    textEditor_DecayMin->setLookAndFeel(nullptr);
    textEditor_SustainMin->setLookAndFeel(nullptr);
    textEditor_ReleaseMin->setLookAndFeel(nullptr);
    textEditor_AmplitudeMax->setLookAndFeel(nullptr);
    textEditor_AttackMax->setLookAndFeel(nullptr);
    textEditor_DecayMax->setLookAndFeel(nullptr);
    textEditor_SustainMax->setLookAndFeel(nullptr);
    textEditor_ReleaseMax->setLookAndFeel(nullptr);
    comboBox_Scales->setLookAndFeel(nullptr);
    comboBox_RecordFormat->setLookAndFeel(nullptr);
    comboBox_NoiseType->setLookAndFeel(nullptr);
    fundamentalFrequencyAlgorithmChooser.setLookAndFeel(nullptr);
    for (int i = 0; i < NUM_PLUGIN_SLOTS; i++)
    {
        comboBoxPluginSelector[i]->setLookAndFeel(nullptr);
    }
//    CustomLookAndFeel::setDefaultLookAndFeel(nullptr);
}

void SettingsComponent::buttonClicked (Button*button)
{
    if (button == textButton_ResetAmp.get())
    {
        // projectManager call
    }
    else if (button == textButton_ResetAttack.get())
    {
        
    }
    else if (button == textButton_ResetDecay.get())
    {
        
    }
    else if (button == textButton_ResetSustain.get())
    {
        
    }
    else if (button == textButton_ResetRelease.get())
    {
        
    }
    
    else if (button == buttonScale25.get())
    {
        projectManager->setProjectSettingsParameter(GUI_SCALE, 0);
    }
    else if (button == buttonScale50.get())
    {
        projectManager->setProjectSettingsParameter(GUI_SCALE, 1);
    }
    else if (button == buttonScale75.get())
    {
        projectManager->setProjectSettingsParameter(GUI_SCALE, 2);
    }
    else if (button == buttonScale100.get())
    {
        projectManager->setProjectSettingsParameter(GUI_SCALE, 3);
    }
    
    else if (button == button_FreqToChordMainHarmonics.get())
    {
        projectManager->setProjectSettingsParameter(FREQUENCY_TO_CHORD, 0);
    }
    else if (button == button_FreqToChordAverage.get())
    {
        projectManager->setProjectSettingsParameter(FREQUENCY_TO_CHORD, 1);
    }
    else if (button == button_FreqToChordEMA.get())
    {
        projectManager->setProjectSettingsParameter(FREQUENCY_TO_CHORD, 2);
    }
    
    
    else if (button == button_Load.get())
    {
        // projectManager->loadSettingsFile(); // Private method
    }
    else if (button == button_Save.get())
    {
        // projectManager->saveSettingsFile(); // Private method
        
    }
    else if (button == textButton_ChangeLogFile.get())
    {
        File dir = projectManager->logFileDirectory;
        FileBrowserComponent browser (FileBrowserComponent::openMode | FileBrowserComponent::canSelectDirectories,
                                      dir,
                                      nullptr,
                                      nullptr);
        
        FileChooserDialogBox dialogBox ("Log Files",
                                        "Select Folder for Log Files...",
                                        browser,
                                        false,
                                        browser.findColour (AlertWindow::backgroundColourId));
        
        dialogBox.show();

        File newFolder = browser.getSelectedFile(0);

        projectManager->logFileDirectory = newFolder;
        
        // update label string
        String folderLocation(projectManager->logFileDirectory.getFullPathName());
        label_LogFileLocation->setText(folderLocation, dontSendNotification);
        
    }
    else if (button == textButton_ChangeRecordFile.get())
    {
        File dir = projectManager->recordFileDirectory;
        FileBrowserComponent browser (FileBrowserComponent::openMode | FileBrowserComponent::canSelectDirectories,
                                      dir,
                                      nullptr,
                                      nullptr);
        
        FileChooserDialogBox dialogBox ("Record Files",
                                        "Select Folder for Record Files...",
                                        browser,
                                        false,
                                        browser.findColour (AlertWindow::backgroundColourId));
        
        dialogBox.show();

        File newFolder = browser.getSelectedFile(0);

        projectManager->recordFileDirectory = newFolder;
        
        // update label string
        String folderLocation(projectManager->recordFileDirectory.getFullPathName());
        label_RecordFileLocation->setText(folderLocation, dontSendNotification);
    }
    else if (button == textButton_OpenAudioSettings.get())
    {
        component_AudioSettings->setOpen(true);
    }
    else if (button == textButton_SpectrogramSetttingsPopup.get())
    {
        spectrogramPopupComponent->setVisible(true);
    }
    else if (button == buttonMixer.get())
    {
        PopupFFTWindow * mixerPopup;

        AudioMixerComponent * audioMixComponent = new AudioMixerComponent(projectManager);
        // Increased size for proper layout of all controls
        audioMixComponent->setBounds(0, 0, 1200, 700);


        mixerPopup = new PopupFFTWindow("Audio Mixer", audioMixComponent, Colours::black, DocumentWindow::allButtons, true);
        mixerPopup ->centreWithSize(audioMixComponent->getWidth(), audioMixComponent->getHeight());
        mixerPopup ->setResizable(true, true);
        mixerPopup ->setUsingNativeTitleBar(true);
        mixerPopup ->setVisible(true);

    }
    else if (button == scanPluginsButton.get())
    {
        rescanPlugins();
        
        for (int i = 0; i < NUM_PLUGIN_SLOTS; i++)
        {
            TSS::UI::populatePluginMenu(*comboBoxPluginSelector[i]->getRootMenu(),
                                        *projectManager->knownPluginList, &lookAndFeel);
        }
        
    }
    else
    {
        for (int i = 0; i < NUM_PLUGIN_SLOTS; i++)
        {
            if (button == buttonRemovePlugin[i])
            {
                PluginAssignProcessor * f = projectManager->pluginAssignProcessor[i].get();
//                PluginWindow::closeAllCurrentlyOpenWindows();
                PluginWindow::closeCurrentlyOpenWindowsFor(i);
                
                if (f->clearPlugin()) {
                    comboBoxPluginSelector[i]->setSelectedId(-1);
                }
            }
            else if (button == buttonOpenPlugin[i])
            {
                PluginAssignProcessor * f = projectManager->pluginAssignProcessor[i].get();
                if (f->hasPlugin)
                {
                    AudioPluginInstance * plugin = f->instance.get();
                    PluginWindow* const w = PluginWindow::openWindowFor (plugin, false, i);
                    w->toFront (true);
                }
                else
                {
                    // could do popup alert msg
                }
            }
        }
    }
    projectManager->logFileWriter->processLog_Settings_Parameters();
}

void SettingsComponent::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == comboBox_Scales.get())
    {
        projectManager->setProjectSettingsParameter(DEFAULT_SCALE, comboBox_Scales->getSelectedId());
        
    }
//    else if (comboBoxThatHasChanged == comboBox_GUIScale)
//    {
//        projectManager->setProjectSettingsParameter(GUI_SCALE, comboBox_GUIScale->getSelectedId());
//    }
    else if (comboBoxThatHasChanged == comboBoxPluginSelector[0])
    {
        PluginWindow::closeCurrentlyOpenWindowsFor(0);

        projectManager->setProjectSettingsParameter(PLUGIN_SELECTED_1, comboBoxPluginSelector[0]->getSelectedId()-1);
    }
    else if (comboBoxThatHasChanged == comboBoxPluginSelector[1])
    {
        PluginWindow::closeCurrentlyOpenWindowsFor(1);

        projectManager->setProjectSettingsParameter(PLUGIN_SELECTED_2, comboBoxPluginSelector[1]->getSelectedId()-1);

    }
    else if (comboBoxThatHasChanged == comboBoxPluginSelector[2])
    {
        PluginWindow::closeCurrentlyOpenWindowsFor(2);

        projectManager->setProjectSettingsParameter(PLUGIN_SELECTED_3, comboBoxPluginSelector[2]->getSelectedId()-1);
    }
    else if (comboBoxThatHasChanged == comboBoxPluginSelector[3])
    {
        PluginWindow::closeCurrentlyOpenWindowsFor(3);

        projectManager->setProjectSettingsParameter(PLUGIN_SELECTED_4, comboBoxPluginSelector[3]->getSelectedId()-1);
    }
    else if (comboBoxThatHasChanged == comboBox_RecordFormat.get())
    {
        projectManager->setProjectSettingsParameter(RECORD_FILE_FORMAT, comboBox_RecordFormat->getSelectedId());
    }
    else if (comboBoxThatHasChanged == comboBox_NoiseType.get())
    {
        projectManager->setProjectSettingsParameter(PANIC_NOISE_TYPE, comboBox_NoiseType->getSelectedId() - 1);
    }
    else if (comboBoxThatHasChanged == &fundamentalFrequencyAlgorithmChooser)
    {
        projectManager->setProjectSettingsParameter(FUNDAMENTAL_FREQUENCY_ALGORITHM, fundamentalFrequencyAlgorithmChooser.getSelectedItemIndex());
    }
    projectManager->logFileWriter->processLog_Settings_Parameters();
}

// Text editor listener overrides
/** Called when the user changes the text in some way. */
void SettingsComponent::textEditorTextChanged (TextEditor&editor)
{
}

/** Called when the user presses the return key. */
void SettingsComponent::textEditorReturnKeyPressed (TextEditor&editor)
{
    if (&editor == textEditor_NotesFrequency.get())
    {
        // add hz to the string
        double value = editor.getText().getDoubleValue();
        
        
        // bounds check
        if (value >= MAX_BASE_A_FREQUENCY) {  value = MAX_BASE_A_FREQUENCY; }
        if (value <= MIN_BASE_A_FREQUENCY) { value = MIN_BASE_A_FREQUENCY; }
        
        
        String newVal(value, 3, false); editor.setText(newVal);
        
        // should add suffix
        
        // send to projectManager
        projectManager->setProjectSettingsParameter(BASE_FREQUENCY_A, value);
    }
    else if (&editor == textEditor_AmplitudeMin.get())
    {
        // add hz to the string
        double value = editor.getText().getDoubleValue();
        
        
        // bounds check
        if (value >= MAX_AMPLITUDE) { value = MAX_AMPLITUDE; } if (value <= MIN_AMPLITUDE) { value = MIN_AMPLITUDE; }
        
//        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setProjectSettingsParameter(AMPLITUDE_MIN, value);
        
    }
    else if (&editor == textEditor_AmplitudeMax.get())
    {
        // add hz to the string
        double value = editor.getText().getDoubleValue();
        
        
        // bounds check
        if (value >= MAX_AMPLITUDE) { value = MAX_AMPLITUDE; } if (value <= MIN_AMPLITUDE) { value = MIN_AMPLITUDE; }
        
//        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setProjectSettingsParameter(AMPLITUDE_MAX, value);
        
    }
    else if (&editor == textEditor_AttackMin.get())
    {
        // add hz to the string
        double value = editor.getText().getDoubleValue();
        
        
        // bounds check
        if (value >= MAX_ATTACK) { value = MAX_ATTACK; } if (value <= MIN_ATTACK) { value = MIN_ATTACK; }
        
//        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setProjectSettingsParameter(ATTACK_MIN, value);
        
    }
    else if (&editor == textEditor_AttackMax.get())
    {
        // add hz to the string
        double value = editor.getText().getDoubleValue();
        
        
        // bounds check
        if (value >= MAX_ATTACK) { value = MAX_ATTACK; } if (value <= MIN_ATTACK) { value = MIN_ATTACK; }
        
//        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setProjectSettingsParameter(ATTACK_MAX, value);
        
    }
    else if (&editor == textEditor_DecayMin.get())
    {
        // add hz to the string
        double value = editor.getText().getDoubleValue();
        
        
        // bounds check
        if (value >= MAX_DECAY) { value = MAX_DECAY; } if (value <= MIN_DECAY) { value = MIN_DECAY; }
        
//        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setProjectSettingsParameter(DECAY_MIN, value);
        
    }
    else if (&editor == textEditor_DecayMax.get())
    {
        // add hz to the string
        double value = editor.getText().getDoubleValue();
        
        
        // bounds check
        if (value >= MAX_DECAY) { value = MAX_DECAY; } if (value <= MIN_DECAY) { value = MIN_DECAY; }
        
//        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setProjectSettingsParameter(DECAY_MAX, value);
        
    }
    else if (&editor == textEditor_SustainMin.get())
    {
        // add hz to the string
        double value = editor.getText().getDoubleValue();
        
        
        // bounds check
        if (value >= MAX_SUSTAIN) { value = MAX_SUSTAIN; } if (value <= MIN_SUSTAIN) { value = MIN_SUSTAIN; }
        
//        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setProjectSettingsParameter(SUSTAIN_MIN, value);
        
    }
    else if (&editor == textEditor_SustainMax.get())
    {
        // add hz to the string
        double value = editor.getText().getDoubleValue();
        
        
        // bounds check
        if (value >= MAX_SUSTAIN) { value = MAX_SUSTAIN; } if (value <= MIN_SUSTAIN) { value = MIN_SUSTAIN; }
        
//        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setProjectSettingsParameter(SUSTAIN_MAX, value);
        
    }
    else if (&editor == textEditor_ReleaseMin.get())
    {
        // add hz to the string
        double value = editor.getText().getDoubleValue();
        
        
        // bounds check
        if (value >= MAX_RELEASE) { value = MAX_RELEASE;  } if (value <= MIN_RELEASE) { value = MIN_RELEASE; }
        
//        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setProjectSettingsParameter(RELEASE_MIN, value);
        
    }
    else if (&editor == textEditor_ReleaseMax.get())
    {
        // add hz to the string
        double value = editor.getText().getDoubleValue();
        
        
        // bounds check
        if (value >= MAX_RELEASE) { value = MAX_RELEASE;  } if (value <= MIN_RELEASE) { value = MIN_RELEASE; }
        
//        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setProjectSettingsParameter(RELEASE_MAX, value);
        
    }
    // unfocus
    unfocusAllComponents();
    projectManager->logFileWriter->processLog_Settings_Parameters();
}


void SettingsComponent::updateUI(int settingIndex)
{
    label_LogFileLocation       ->setText(String(projectManager->logFileDirectory.getFullPathName()),     dontSendNotification);
    label_RecordFileLocation    ->setText(String(projectManager->recordFileDirectory.getFullPathName()),  dontSendNotification);

    
    if (settingIndex == BASE_FREQUENCY_A)
    {
        double baseA = projectManager->frequencyManager->getFrequencyForMIDINote(69);
        String baseAString(baseA, 3, false); textEditor_NotesFrequency->setText(baseAString);
        
        double baseB = projectManager->frequencyManager->getFrequencyForMIDINote(71);
        String baseBString(baseB, 3, false); label_B_Frequency->setText(baseBString, dontSendNotification);
        
        double baseC = projectManager->frequencyManager->getFrequencyForMIDINote(72);
        String baseCString(baseC, 3 , false); label_C_Frequency->setText(baseCString, dontSendNotification);
        
        double baseD = projectManager->frequencyManager->getFrequencyForMIDINote(74);
        String baseDString(baseD, 3, false); label_D_Frequency->setText(baseDString, dontSendNotification);
        
        double baseE = projectManager->frequencyManager->getFrequencyForMIDINote(76);
        String baseEString(baseE, 3, false); label_E_Frequency->setText(baseEString, dontSendNotification);
        
        double baseF = projectManager->frequencyManager->getFrequencyForMIDINote(77);
        String baseFString(baseF, 3, false); label_F_Frequency->setText(baseFString, dontSendNotification);
        
        double baseG = projectManager->frequencyManager->getFrequencyForMIDINote(79);
        String baseGString(baseG, 3, false); label_G_Frequency->setText(baseGString, dontSendNotification);
    }
    else if (settingIndex == DEFAULT_SCALE)
    {
        int scale = (int)projectManager->getProjectSettingsParameter(DEFAULT_SCALE);
        comboBox_Scales->setSelectedId(scale);
        
        double baseB = projectManager->frequencyManager->getFrequencyForMIDINote(71);
        String baseBString(baseB, 3, false); label_B_Frequency->setText(baseBString, dontSendNotification);
        
        double baseC = projectManager->frequencyManager->getFrequencyForMIDINote(60);
        String baseCString(baseC, 3 , false); label_C_Frequency->setText(baseCString, dontSendNotification);
        
        double baseD = projectManager->frequencyManager->getFrequencyForMIDINote(62);
        String baseDString(baseD, 3, false); label_D_Frequency->setText(baseDString, dontSendNotification);
        
        double baseE = projectManager->frequencyManager->getFrequencyForMIDINote(64);
        String baseEString(baseE, 3, false); label_E_Frequency->setText(baseEString, dontSendNotification);
        
        double baseF = projectManager->frequencyManager->getFrequencyForMIDINote(65);
        String baseFString(baseF, 3, false); label_F_Frequency->setText(baseFString, dontSendNotification);
        
        double baseG = projectManager->frequencyManager->getFrequencyForMIDINote(67);
        String baseGString(baseG, 3, false); label_G_Frequency->setText(baseGString, dontSendNotification);
        
    }
    else if (settingIndex == RECORD_FILE_FORMAT)
    {
       int format = (int)projectManager->getProjectSettingsParameter(RECORD_FILE_FORMAT);
       comboBox_RecordFormat->setSelectedId(format, dontSendNotification);
    }
    else if (settingIndex == PANIC_NOISE_TYPE)
    {
        int type = (int)projectManager->getProjectSettingsParameter(PANIC_NOISE_TYPE);
        comboBox_NoiseType->setSelectedId(type + 1, dontSendNotification);
    }
    else if (settingIndex == AMPLITUDE_MIN)
    {
        double val = projectManager->getProjectSettingsParameter(AMPLITUDE_MIN);
        String valString(val, 3, false); textEditor_AmplitudeMin->setText(valString);
    }
    else if (settingIndex == AMPLITUDE_MAX)
    {
        double val = projectManager->getProjectSettingsParameter(AMPLITUDE_MAX);
        String valString(val, 3, false); textEditor_AmplitudeMax->setText(valString);
    }
    else if (settingIndex == ATTACK_MIN)
    {
        double val = projectManager->getProjectSettingsParameter(ATTACK_MIN);
        String valString(val, 3, false); textEditor_AttackMin->setText(valString);
    }
    else if (settingIndex == ATTACK_MAX)
    {
        double val = projectManager->getProjectSettingsParameter(ATTACK_MAX);
        String valString(val, 3, false); textEditor_AttackMax->setText(valString);
    }
    else if (settingIndex == DECAY_MIN)
    {
        double val = projectManager->getProjectSettingsParameter(DECAY_MIN);
        String valString(val, 3, false); textEditor_DecayMin->setText(valString);
    }
    else if (settingIndex == DECAY_MAX)
    {
        double val = projectManager->getProjectSettingsParameter(DECAY_MAX);
        String valString(val, 3, false); textEditor_DecayMax->setText(valString);
    }
    else if (settingIndex == SUSTAIN_MIN)
    {
        double val = projectManager->getProjectSettingsParameter(SUSTAIN_MIN);
        String valString(val, 3, false); textEditor_SustainMin->setText(valString);
    }
    else if (settingIndex == SUSTAIN_MAX)
    {
        double val = projectManager->getProjectSettingsParameter(SUSTAIN_MAX);
        String valString(val, 3, false); textEditor_SustainMax->setText(valString);
    }
    else if (settingIndex == RELEASE_MIN)
    {
        double val = projectManager->getProjectSettingsParameter(RELEASE_MIN);
        String valString(val, 3, false); textEditor_ReleaseMin->setText(valString);
    }
    else if (settingIndex == RELEASE_MAX)
    {
        double val = projectManager->getProjectSettingsParameter(RELEASE_MAX);
        String valString(val, 3, false); textEditor_ReleaseMax->setText(valString);
    }
    else if (settingIndex == FUNDAMENTAL_FREQUENCY_ALGORITHM)
    {
        const auto index = static_cast<int>(projectManager->getProjectSettingsParameter(FUNDAMENTAL_FREQUENCY_ALGORITHM));
        fundamentalFrequencyAlgorithmChooser.setSelectedItemIndex(index);
    }
    else if (settingIndex == PLUGIN_SELECTED_1)
    {
        int val = projectManager->getProjectSettingsParameter(PLUGIN_SELECTED_1);
        
        comboBoxPluginSelector[0]->setSelectedId(val+1, dontSendNotification);
    }
    else if (settingIndex == PLUGIN_SELECTED_2)
    {
        int val = projectManager->getProjectSettingsParameter(PLUGIN_SELECTED_2);
        
        comboBoxPluginSelector[1]->setSelectedId(val+1, dontSendNotification);
    }
    else if (settingIndex == PLUGIN_SELECTED_3)
    {
        int val = projectManager->getProjectSettingsParameter(PLUGIN_SELECTED_3);
        
        comboBoxPluginSelector[2]->setSelectedId(val+1, dontSendNotification);
    }
    else if (settingIndex == PLUGIN_SELECTED_4)
    {
        int val = projectManager->getProjectSettingsParameter(PLUGIN_SELECTED_4);
        
        comboBoxPluginSelector[3]->setSelectedId(val+1, dontSendNotification);
    }
    else if (settingIndex == FREQUENCY_TO_CHORD)
    {
        int val = projectManager->getProjectSettingsParameter(FREQUENCY_TO_CHORD);
        
        if (val == 0)
        {
            button_FreqToChordMainHarmonics ->setToggleState(true, dontSendNotification);
            button_FreqToChordAverage       ->setToggleState(false, dontSendNotification);
            button_FreqToChordEMA           ->setToggleState(false, dontSendNotification);
        }
        else if (val == 1)
        {
            button_FreqToChordMainHarmonics ->setToggleState(false, dontSendNotification);
            button_FreqToChordAverage       ->setToggleState(true, dontSendNotification);
            button_FreqToChordEMA           ->setToggleState(false, dontSendNotification);
        }
        else if (val == 2)
        {
            button_FreqToChordMainHarmonics ->setToggleState(false, dontSendNotification);
            button_FreqToChordAverage       ->setToggleState(false, dontSendNotification);
            button_FreqToChordEMA           ->setToggleState(true, dontSendNotification);
        }
    }
    else
    {
        spectrogramPopupComponent->updateUI(settingIndex);
    }
    
    if (settingIndex == GUI_SCALE)
    {
        int val = projectManager->getProjectSettingsParameter(GUI_SCALE);
        
        bool res[4];
        for (int i = 0; i < 4; i++)
        {
            res[i] = false; if (val == i) res[i] = true;
        }
        
        buttonScale25->setToggleState(res[0], dontSendNotification);
        buttonScale50->setToggleState(res[1], dontSendNotification);
        buttonScale75->setToggleState(res[2], dontSendNotification);
        buttonScale100->setToggleState(res[3], dontSendNotification);
        
    }
}

void SettingsComponent::paint (Graphics& g)
{
    g.fillAll(juce::Colour(45, 44, 44));

    const float sx = getWidth() / 1566.0f;
    const float sy = getHeight() / 1440.0f;

    g.setColour(Colour::fromString("ff424242"));
    g.fillRoundedRectangle(851*sx, 858*sy, 679*sx, 100*sy, 5.f);

    g.setColour(Colours::white);
    g.setFont(Font{30.f * sx}.withStyle(Font::FontStyleFlags::bold));
    g.drawFittedText("Find fundamental frequency:", Rectangle<int>((int)(878*sx), (int)(890*sy), (int)(338*sx), (int)(39*sy)), Justification::centredLeft, 1);
}

void SettingsComponent::resized()
{
    const float sx = getWidth() / 1566.0f;
    const float sy = getHeight() / 1440.0f;

    int fontSize = 29;

    float labelW = 600;
    float labelX = 360;
    label_LogFileLocation->setBounds((int)(labelX * sx), (int)(60 * sy), (int)(labelW * sx), (int)(41 * sy));
    label_LogFileLocation->setFont((int)(fontSize * sx));

    label_RecordFileLocation->setBounds((int)((labelX + 60) * sx), (int)(125 * sy), (int)((labelW + 200) * sx), (int)(41 * sy));
    label_RecordFileLocation->setFont((int)(fontSize * sx));

    float freqY = 296;
    float xAdj = 204;

    textEditor_NotesFrequency->setBounds((int)((288 - xAdj) * sx), (int)(freqY * sy), (int)(117 * sx), (int)(41 * sy));
    textEditor_NotesFrequency->applyFontToAllText((int)(fontSize * sx));

    label_B_Frequency->setBounds((int)((443 - xAdj) * sx), (int)(freqY * sy), (int)(117 * sx), (int)(41 * sy));
    label_B_Frequency->setFont((int)(fontSize * sx));

    label_C_Frequency->setBounds((int)((598 - xAdj) * sx), (int)(freqY * sy), (int)(117 * sx), (int)(41 * sy));
    label_C_Frequency->setFont((int)(fontSize * sx));

    label_D_Frequency->setBounds((int)((754 - xAdj) * sx), (int)(freqY * sy), (int)(117 * sx), (int)(41 * sy));
    label_D_Frequency->setFont((int)(fontSize * sx));

    label_E_Frequency->setBounds((int)((909 - xAdj) * sx), (int)(freqY * sy), (int)(117 * sx), (int)(41 * sy));
    label_E_Frequency->setFont((int)(fontSize * sx));

    label_F_Frequency->setBounds((int)((1064 - xAdj) * sx), (int)(freqY * sy), (int)(117 * sx), (int)(41 * sy));
    label_F_Frequency->setFont((int)(fontSize * sx));

    label_G_Frequency->setBounds((int)((1219 - xAdj) * sx), (int)(freqY * sy), (int)(117 * sx), (int)(41 * sy));
    label_G_Frequency->setFont((int)(fontSize * sx));

    float xMin = 260;
    float xMax = 455;

    fontSize = 31;

    textEditor_AmplitudeMin->setBounds((int)(xMin * sx), (int)(478 * sy), (int)(125 * sx), (int)(41 * sy));
    textEditor_AmplitudeMin->applyFontToAllText((int)(fontSize * sx));

    textEditor_AttackMin->setBounds((int)(xMin * sx), (int)(577 * sy), (int)(125 * sx), (int)(41 * sy));
    textEditor_AttackMin->applyFontToAllText((int)(fontSize * sx));

    textEditor_DecayMin->setBounds((int)(xMin * sx), (int)(642 * sy), (int)(125 * sx), (int)(41 * sy));
    textEditor_DecayMin->applyFontToAllText((int)(fontSize * sx));

    textEditor_SustainMin->setBounds((int)(xMin * sx), (int)(705 * sy), (int)(125 * sx), (int)(41 * sy));
    textEditor_SustainMin->applyFontToAllText((int)(fontSize * sx));

    textEditor_ReleaseMin->setBounds((int)(xMin * sx), (int)(769 * sy), (int)(125 * sx), (int)(41 * sy));
    textEditor_ReleaseMin->applyFontToAllText((int)(fontSize * sx));

    int maxW = 130;

    textEditor_AmplitudeMax->setBounds((int)(xMax * sx), (int)(478 * sy), (int)(maxW * sx), (int)(41 * sy));
    textEditor_AmplitudeMax->applyFontToAllText((int)(fontSize * sx));

    textEditor_AttackMax->setBounds((int)(xMax * sx), (int)(577 * sy), (int)(maxW * sx), (int)(41 * sy));
    textEditor_AttackMax->applyFontToAllText((int)(fontSize * sx));

    textEditor_DecayMax->setBounds((int)(xMax * sx), (int)(642 * sy), (int)(maxW * sx), (int)(41 * sy));
    textEditor_DecayMax->applyFontToAllText((int)(fontSize * sx));

    textEditor_SustainMax->setBounds((int)(xMax * sx), (int)(705 * sy), (int)(maxW * sx), (int)(41 * sy));
    textEditor_SustainMax->applyFontToAllText((int)(fontSize * sx));

    textEditor_ReleaseMax->setBounds((int)(xMax * sx), (int)(769 * sy), (int)(maxW * sx), (int)(41 * sy));
    textEditor_ReleaseMax->applyFontToAllText((int)(fontSize * sx));

    int buttonW = (int)(160 * sx);
    textButton_ResetAmp->setBounds((int)(633 * sx), (int)(478 * sy), buttonW, (int)(40 * sy));
    textButton_ResetAttack->setBounds((int)(633 * sx), (int)(577 * sy), buttonW, (int)(40 * sy));
    textButton_ResetDecay->setBounds((int)(633 * sx), (int)(642 * sy), buttonW, (int)(40 * sy));
    textButton_ResetSustain->setBounds((int)(633 * sx), (int)(705 * sy), buttonW, (int)(40 * sy));
    textButton_ResetRelease->setBounds((int)(633 * sx), (int)(769 * sy), buttonW, (int)(40 * sy));

    textButton_ChangeLogFile->setBounds((int)(1020 * sx), (int)(62 * sy), (int)(117 * sx), (int)(41 * sy));
    textButton_ChangeRecordFile->setBounds((int)(1020 * sx), (int)(125 * sy), (int)(117 * sx), (int)(41 * sy));
    textButton_OpenAudioSettings->setBounds((int)(1190 * sx), (int)(62 * sy), (int)(310 * sx), (int)(41 * sy));

    textButton_SpectrogramSetttingsPopup->setBounds((int)(1240 * sx), (int)(740 * sy), (int)(280 * sx), (int)(41 * sy));

    spectrogramPopupComponent->setBounds(0, 0, getWidth(), getHeight());

    scanPluginsButton->setBounds((int)(360 * sx), (int)(873 * sy), (int)(150 * sx), (int)(41 * sy));

    comboBox_Scales->setBounds((int)(1180 * sx), (int)(308 * sy), (int)(330 * sx), (int)(41 * sy));

    audioSetupComponent->setBounds((int)(60 * sx), (int)(300 * sy), (int)(1300 * sx), (int)(1000 * sy));
    audioSetupComponent->setItemHeight((int)(70 * sy));

    int comboWidth = 228;
    int centreX = 160;
    int y = 960;
    int ySpace = 60;

    fontSize = 28;

    for (int i = 0; i < NUM_PLUGIN_SLOTS; i++)
    {
        labelPluginSlot[i]->setBounds((int)((centreX - 120) * sx), (int)((y + (i * ySpace)) * sy), (int)(comboWidth * sx), (int)(41 * sy));
        labelPluginSlot[i]->setFont((int)(fontSize * sx));

        comboBoxPluginSelector[i]->setBounds((int)((centreX + 80) * sx), (int)((y + (i * ySpace)) * sy), (int)(comboWidth * sx), (int)(41 * sy));

        buttonRemovePlugin[i]->setBounds((int)((centreX + comboWidth + 90) * sx), (int)((y + 11 + (i * ySpace)) * sy), (int)(22 * sx), (int)(22 * sy));

        buttonOpenPlugin[i]->setBounds((int)(centreX * sx), (int)((y + (i * ySpace) + 10) * sy), (int)(66 * sx), (int)(22 * sy));
    }

    comboBox_NoiseType->setBounds((int)(1350 * sx), (int)(130 * sy), (int)(150 * sx), (int)(41 * sy));

    button_Load->setBounds((int)(1000 * sx), (int)(1300 * sy), (int)(257 * sx), (int)(69 * sy));
    button_Save->setBounds((int)(1280 * sx), (int)(1300 * sy), (int)(257 * sx), (int)(69 * sy));

    float bX = 870;
    button_FreqToChordMainHarmonics->setBounds((int)(bX * sx), (int)(478 * sy), (int)(38 * sx), (int)(38 * sy));
    button_FreqToChordAverage->setBounds((int)(bX * sx), (int)(528 * sy), (int)(38 * sx), (int)(38 * sy));
    button_FreqToChordEMA->setBounds((int)(bX * sx), (int)(580 * sy), (int)(38 * sx), (int)(38 * sy));

    float scaleButtonX = 874;
    float sp = 80;
    float scY = 726;

    buttonScale25->setBounds((int)((scaleButtonX + (sp * 0)) * sx), (int)(scY * sy), (int)(70 * sx), (int)(70 * sy));
    buttonScale50->setBounds((int)((scaleButtonX + (sp * 1)) * sx), (int)(scY * sy), (int)(70 * sx), (int)(70 * sy));
    buttonScale75->setBounds((int)((scaleButtonX + (sp * 2)) * sx), (int)(scY * sy), (int)(70 * sx), (int)(70 * sy));
    buttonScale100->setBounds((int)((scaleButtonX + (sp * 3)) * sx), (int)(scY * sy), (int)(70 * sx), (int)(70 * sy));

    buttonMixer->setBounds((int)((centreX - 120) * sx), (int)((y + (6 * ySpace) + 10) * sy), (int)(310 * sx), (int)(41 * sy));

    fundamentalFrequencyAlgorithmChooser.setBounds((int)(1243 * sx), (int)(889 * sy), (int)(263 * sx), (int)(45 * sy));
}

void SettingsComponent::rescanPlugins()
{
    // only scan for audio units

    // only scan for FX , not instruments


    // Skip plugin scanning popup to prevent blocking main window visibility
    // Automatically proceed with scanning without user interaction
    int result = 1; // Simulate user clicking "OK"
    
    if (result)
    {
        // create new window with progress bar and text label
        
        String formatType;
        
        for (int i = 0; i < projectManager->pluginFormatManager.getNumFormats(); i++)
        {
            AudioPluginFormat* const format = projectManager->pluginFormatManager.getFormat(i);
            
            formatType = format->getName();
            
            
            
            PluginDirectoryScanner * scanner = new PluginDirectoryScanner(*projectManager->knownPluginList, *format, format->getDefaultLocationsToSearch(), true, projectManager->deadMansPedalFile);
            
            String plugName;
            
            
            
            
            
            while (scanner->scanNextFile(true, plugName))
            {
//                val = scanner->getProgress();
//
//                String processingString("Scanning ");
//                processingString.append(formatType, 10);
//                processingString.append(" : ", 3);
//                processingString.append(plugName, 30);
//
//                alertWindow.addTextBlock(processingString);
//
//
            }
            
        }
    }
    
}


//void SettingsComponent::rescanPlugins()
//{
//    // only scan for audio units
//
//    // only scan for FX , not instruments
//
//
//    for (int i = 0; i < projectManager->pluginFormatManager.getNumFormats(); i++)
//    {
//        AudioPluginFormat* const format = projectManager->pluginFormatManager.getFormat(i);
//
//        PluginDirectoryScanner * scanner = new PluginDirectoryScanner(*projectManager->knownPluginList, *format, format->getDefaultLocationsToSearch(), true, projectManager->deadMansPedalFile);
//        String plugName;
//
//        // move alert popup before it scans each format..
//        // then add a progress bar which iterates through the plugin names as they scan
//
//        bool shouldScan = AlertWindow::showOkCancelBox (AlertWindow::WarningIcon,
//                                      TRANS("Plugin Scanning"),
//                                      TRANS("If you choose to scan folders that contain non-plugin files, "
//                                            "then scanning may take a long time, and can cause crashes when "
//                                            "attempting to load unsuitable files."),
//                                      TRANS ("Scan"),
//                                      "",
//                                      nullptr,
//                                      nullptr);
//
//        if (shouldScan)
//        {
//            while (scanner->scanNextFile(true, plugName))
//            { }
//        }
//    }
//}

void SettingsComponent::updateSettingsUIParameter(int settingIndex)
{
    updateUI(settingIndex);
}
