/*
  ==============================================================================

    SettingsComponent.cpp
    Created: 13 Mar 2019 9:56:56pm
    Author:  Gary Jones

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "SettingsComponent.h"
#include "PluginWindow.h"
#include "PopupFFTWindow.h"


//==============================================================================
SettingsComponent::SettingsComponent(ProjectManager * pm)
{
    projectManager = pm;
    projectManager->addUIListener(this);
    
    
    
    imageBackground                     = ImageCache::getFromMemory(BinaryData::SettingsBackgroundNew, BinaryData::SettingsBackgroundNewSize);
    
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

    textEditor_NotesFrequency = new TextEditor();
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
    addAndMakeVisible(textEditor_NotesFrequency);
    
    
    // Locations Files
    
    label_LogFileLocation = new Label("");
    label_LogFileLocation->setFont(fontNormal);
    label_LogFileLocation->setColour(Label::textColourId, Colours::lightgrey);
    label_LogFileLocation->setJustificationType(Justification::left);
    label_LogFileLocation->setText("file location", dontSendNotification);
    addAndMakeVisible(label_LogFileLocation);
    
    label_RecordFileLocation = new Label("");
    label_RecordFileLocation->setFont(fontNormal);
    label_RecordFileLocation->setColour(Label::textColourId, Colours::lightgrey);
    label_RecordFileLocation->setJustificationType(Justification::left);
    addAndMakeVisible(label_RecordFileLocation);

    label_B_Frequency = new Label("");
    label_B_Frequency->setFont(fontNormal);
    label_B_Frequency->setColour(Label::textColourId, Colours::lightgrey);
    label_B_Frequency->setJustificationType(Justification::centred);
    label_B_Frequency->setText("440", dontSendNotification);
    label_B_Frequency->setBounds(443, 328, 117, 41);
    addAndMakeVisible(label_B_Frequency);
    
    label_C_Frequency = new Label("");
    label_C_Frequency->setFont(fontNormal);
    label_C_Frequency->setColour(Label::textColourId, Colours::lightgrey);
    label_C_Frequency->setJustificationType(Justification::centred);
    label_C_Frequency->setText("440", dontSendNotification);
    label_C_Frequency->setBounds(598, 328, 117, 41);
    addAndMakeVisible(label_C_Frequency);
    
    label_D_Frequency = new Label("");
    label_D_Frequency->setFont(fontNormal);
    label_D_Frequency->setColour(Label::textColourId, Colours::lightgrey);
    label_D_Frequency->setJustificationType(Justification::centred);
    label_D_Frequency->setText("440", dontSendNotification);
    label_D_Frequency->setBounds(754, 328, 117, 41);
    addAndMakeVisible(label_D_Frequency);
    
    label_E_Frequency = new Label("");
    label_E_Frequency->setFont(fontNormal);
    label_E_Frequency->setColour(Label::textColourId, Colours::lightgrey);
    label_E_Frequency->setJustificationType(Justification::centred);
    label_E_Frequency->setText("440", dontSendNotification);
    label_E_Frequency->setBounds(909, 328, 117, 41);
    addAndMakeVisible(label_E_Frequency);
    
    label_F_Frequency = new Label("");
    label_F_Frequency->setFont(fontNormal);
    label_F_Frequency->setColour(Label::textColourId, Colours::lightgrey);
    label_F_Frequency->setJustificationType(Justification::centred);
    label_F_Frequency->setText("440", dontSendNotification);
    label_F_Frequency->setBounds(1064, 328, 117, 41);
    addAndMakeVisible(label_F_Frequency);
    
    label_G_Frequency = new Label("");
    label_G_Frequency->setFont(fontNormal);
    label_G_Frequency->setColour(Label::textColourId, Colours::lightgrey);
    label_G_Frequency->setJustificationType(Justification::centred);
    label_G_Frequency->setText("440", dontSendNotification);
    label_G_Frequency->setBounds(1219, 328, 117, 41);
    addAndMakeVisible(label_G_Frequency);
    
    
    textEditor_AmplitudeMin = new TextEditor();
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
    addAndMakeVisible(textEditor_AmplitudeMin);
    
    
    textEditor_AttackMin = new TextEditor();
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
    addAndMakeVisible(textEditor_AttackMin);
    
    textEditor_DecayMin = new TextEditor();
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
    addAndMakeVisible(textEditor_DecayMin);
    
    textEditor_SustainMin = new TextEditor();
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
    addAndMakeVisible(textEditor_SustainMin);
    
    textEditor_ReleaseMin = new TextEditor();
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
    addAndMakeVisible(textEditor_ReleaseMin);
    
    
    textEditor_AmplitudeMax = new TextEditor();
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
    addAndMakeVisible(textEditor_AmplitudeMax);
    
    textEditor_AttackMax = new TextEditor();
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
    addAndMakeVisible(textEditor_AttackMax);
    
    textEditor_DecayMax = new TextEditor();
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
    addAndMakeVisible(textEditor_DecayMax);
    
    textEditor_SustainMax = new TextEditor();
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
    addAndMakeVisible(textEditor_SustainMax);
    
    textEditor_ReleaseMax = new TextEditor();
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
    addAndMakeVisible(textEditor_ReleaseMax);
    
    int buttonW = 260;
    textButton_ResetAmp = new ImageButton();
    textButton_ResetAmp->setTriggeredOnMouseDown(true);
    textButton_ResetAmp->setImages (false, true, true,
                                imageButtonReset, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageButtonReset, 1.0, Colour (0x00000000));
    textButton_ResetAmp->addListener(this);
    addAndMakeVisible(textButton_ResetAmp);

    textButton_ResetAttack = new ImageButton();
    textButton_ResetAttack->setTriggeredOnMouseDown(true);
    textButton_ResetAttack->setImages (false, true, true,
                                                imageButtonReset, 0.999f, Colour (0x00000000),
                                                Image(), 1.000f, Colour (0x00000000),
                                                imageButtonReset, 1.0, Colour (0x00000000));
    textButton_ResetAttack->addListener(this);
    addAndMakeVisible(textButton_ResetAttack);
    
    textButton_ResetDecay = new ImageButton();
    textButton_ResetDecay->setTriggeredOnMouseDown(true);
    textButton_ResetDecay->setImages (false, true, true,
                                                imageButtonReset, 0.999f, Colour (0x00000000),
                                                Image(), 1.000f, Colour (0x00000000),
                                                imageButtonReset, 1.0, Colour (0x00000000));
    textButton_ResetDecay->addListener(this);
    addAndMakeVisible(textButton_ResetDecay);
    
    textButton_ResetSustain = new ImageButton();
    textButton_ResetSustain->setTriggeredOnMouseDown(true);
    textButton_ResetSustain->setImages (false, true, true,
                                                imageButtonReset, 0.999f, Colour (0x00000000),
                                                Image(), 1.000f, Colour (0x00000000),
                                                imageButtonReset, 1.0, Colour (0x00000000));
    
    textButton_ResetSustain->addListener(this);
    addAndMakeVisible(textButton_ResetSustain);
    
    textButton_ResetRelease = new ImageButton();
    textButton_ResetRelease->setTriggeredOnMouseDown(true);
    textButton_ResetRelease->setImages (false, true, true,
                                                imageButtonReset, 0.999f, Colour (0x00000000),
                                                Image(), 1.000f, Colour (0x00000000),
                                                imageButtonReset, 1.0, Colour (0x00000000));
    textButton_ResetRelease->addListener(this);
    addAndMakeVisible(textButton_ResetRelease);

    textButton_ChangeLogFile = new ImageButton();
    textButton_ChangeLogFile->setTriggeredOnMouseDown(true);
    textButton_ChangeLogFile->setImages (false, true, true,
                                         imageButtonChange, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                         imageButtonChange, 1.0, Colour (0x00000000));
    textButton_ChangeLogFile->addListener(this);
    addAndMakeVisible(textButton_ChangeLogFile);
    
    textButton_ChangeRecordFile = new ImageButton();
    textButton_ChangeRecordFile->setTriggeredOnMouseDown(true);
    textButton_ChangeRecordFile->setImages (false, true, true,
                                imageButtonChange, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                            imageButtonChange, 1.0, Colour (0x00000000));
    textButton_ChangeRecordFile->addListener(this);
    addAndMakeVisible(textButton_ChangeRecordFile);
    
    buttonMixer = new TextButton();
    buttonMixer->setTriggeredOnMouseDown(true);
    buttonMixer->addListener(this);
    buttonMixer->setButtonText("Mixer");
    addAndMakeVisible(buttonMixer);
    
    
    // audio setting stuff
    textButton_OpenAudioSettings = new ImageButton();
    textButton_OpenAudioSettings->setTriggeredOnMouseDown(true);
    textButton_OpenAudioSettings->setImages (false, true, true,
                                imageButtonAudio, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageButtonAudio, 1.0, Colour (0x00000000));
    textButton_OpenAudioSettings->addListener(this);
    addAndMakeVisible(textButton_OpenAudioSettings);
    
    textButton_SpectrogramSetttingsPopup = new ImageButton();
    textButton_SpectrogramSetttingsPopup->setTriggeredOnMouseDown(true);
    textButton_SpectrogramSetttingsPopup->setImages (false, true, true,
                                imageButtonSpectrogram, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                                     imageButtonSpectrogram, 1.0, Colour (0x00000000));
    textButton_SpectrogramSetttingsPopup->addListener(this);
    addAndMakeVisible(textButton_SpectrogramSetttingsPopup);
    

    comboBox_Scales = new ComboBox();
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
    addAndMakeVisible(comboBox_Scales);

    
    comboBox_RecordFormat = new ComboBox();
    comboBox_RecordFormat->addListener(this);
    PopupMenu * recordFormatMenu =  comboBox_RecordFormat->getRootMenu();
    comboBox_RecordFormat->setLookAndFeel(&lookAndFeel);
    
    // get audio record formats

    
    for (int i = 0; i < projectManager->formatManager.getNumKnownFormats(); i++)
    {
        String formatName = projectManager->formatManager.getKnownFormat(i)->getFormatName();
        
        recordFormatMenu->addItem(i+1, formatName);
    }
    
    
    addAndMakeVisible(comboBox_RecordFormat);
    comboBox_RecordFormat->setBounds(1080, 140, 250, 41);
    

    comboBox_NoiseType = new ComboBox();
    comboBox_NoiseType->addListener(this);
    PopupMenu * noiseTypeMenu =  comboBox_NoiseType->getRootMenu();
    comboBox_NoiseType->setLookAndFeel(&lookAndFeel);
    noiseTypeMenu->addItem(1, "White");
    noiseTypeMenu->addItem(2, "Pink");
    addAndMakeVisible(comboBox_NoiseType);
    
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
        
        PopupMenu * pluginMenu =  comboBoxPluginSelector[i]->getRootMenu();
        
        PopupMenu auMenu;   auMenu.setLookAndFeel(&lookAndFeel);
        PopupMenu vstMenu;  vstMenu.setLookAndFeel(&lookAndFeel);
        PopupMenu vst3Menu;  vst3Menu.setLookAndFeel(&lookAndFeel);
            
        // do search for plugins and add to au/vst menu
        for (int i = 0; i < projectManager->knownPluginList->getNumTypes(); i++)
        {
            PluginDescription * desc = projectManager->knownPluginList->getType(i);
                
            if (!desc->isInstrument)
            {
                if (desc->pluginFormatName == "AudioUnit")
                {
                    auMenu.addItem(i+1, desc->name);
                }
                else if (desc->pluginFormatName =="VST")
                {
                    vstMenu.addItem(i+1, desc->name);
                }
                else if (desc->pluginFormatName =="VST3")
                {
                    vst3Menu.addItem(i+1, desc->name);
                }
            }
        }
            
        pluginMenu->addSubMenu("AU", auMenu);
        pluginMenu->addSubMenu("VST", vstMenu);
        pluginMenu->addSubMenu("VST3", vstMenu);
        
        addAndMakeVisible(comboBoxPluginSelector[i]);
    }
    
    scanPluginsButton = new ImageButton();
    scanPluginsButton->setTriggeredOnMouseDown(true);
    scanPluginsButton->setImages (false, true, true,
                                imageButtonScan, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageButtonScan, 1.0, Colour (0x00000000));
    scanPluginsButton->addListener(this);
    addAndMakeVisible(scanPluginsButton);

    
    
    
    // Load / Save Button
    button_Load = new ImageButton();
    button_Load->setTriggeredOnMouseDown(true);
    button_Load->setImages (false, true, true,
                                imageButtonLoad, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageButtonLoad, 1.0, Colour (0x00000000));
    button_Load->addListener(this);
    addAndMakeVisible(button_Load);
    
    button_Save = new ImageButton();
    button_Save->setTriggeredOnMouseDown(true);
    button_Save->setImages (false, true, true,
                                imageButtonSave, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageButtonSave, 1.0, Colour (0x00000000));
    button_Save->addListener(this);
    addAndMakeVisible(button_Save);
    
    // Radio Buttons
    
    button_FreqToChordMainHarmonics = new ImageButton();
    button_FreqToChordMainHarmonics->setTriggeredOnMouseDown(true);
    button_FreqToChordMainHarmonics->setImages (false, true, true,
                                imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_FreqToChordMainHarmonics->addListener(this);
    button_FreqToChordMainHarmonics->setBounds(639, 0, 38, 38);
    addAndMakeVisible(button_FreqToChordMainHarmonics);
    
    button_FreqToChordAverage = new ImageButton();
    button_FreqToChordAverage->setTriggeredOnMouseDown(true);
    button_FreqToChordAverage->setImages (false, true, true,
                                imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_FreqToChordAverage->addListener(this);
    button_FreqToChordAverage->setBounds(639, 0, 38, 38);
    addAndMakeVisible(button_FreqToChordAverage);
    
    button_FreqToChordEMA = new ImageButton();
    button_FreqToChordEMA->setTriggeredOnMouseDown(true);
    button_FreqToChordEMA->setImages (false, true, true,
                                imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageBlueButtonSelected, 1.0, Colour (0x00000000));
    button_FreqToChordEMA->addListener(this);
    button_FreqToChordEMA->setBounds(639, 0, 38, 38);
    addAndMakeVisible(button_FreqToChordEMA);

    
   // new scale buttons
    
    imageButtonScale25Normal    = ImageCache::getFromMemory(BinaryData::ButtonScale25Normal, BinaryData::ButtonScale25NormalSize);
    imageButtonScale25Selected  = ImageCache::getFromMemory(BinaryData::ButtonScale25Selected, BinaryData::ButtonScale25SelectedSize);
    
    imageButtonScale50Normal    = ImageCache::getFromMemory(BinaryData::ButtonScale50Normal, BinaryData::ButtonScale50NormalSize);
    imageButtonScale50Selected  = ImageCache::getFromMemory(BinaryData::ButtonScale50Selected, BinaryData::ButtonScale50SelectedSize);
    
    imageButtonScale75Normal    = ImageCache::getFromMemory(BinaryData::ButtonScale75Normal, BinaryData::ButtonScale75NormalSize);
    imageButtonScale75Selected  = ImageCache::getFromMemory(BinaryData::ButtonScale75Selected, BinaryData::ButtonScale75SelectedSize);
    
    imageButtonScale100Normal   = ImageCache::getFromMemory(BinaryData::ButtonScale100Normal, BinaryData::ButtonScale100NormalSize);
    imageButtonScale100Selected = ImageCache::getFromMemory(BinaryData::ButtonScale100Selected, BinaryData::ButtonScale100SelectedSize);
    
    buttonScale25 = new ImageButton();
    buttonScale25->setTriggeredOnMouseDown(true);
    buttonScale25->setImages (false, true, true,
                           imageButtonScale25Normal, 0.999f, Colour (0x00000000),
                           Image(), 1.000f, Colour (0x00000000),
                              imageButtonScale25Selected, 0.75, Colour (0x00000000));
    buttonScale25->addListener(this);
    addAndMakeVisible(buttonScale25);
    
    buttonScale50 = new ImageButton();
    buttonScale50->setTriggeredOnMouseDown(true);
    buttonScale50->setImages (false, true, true,
                              imageButtonScale50Normal, 0.999f, Colour (0x00000000),
                           Image(), 1.000f, Colour (0x00000000),
                              imageButtonScale50Selected, 0.75, Colour (0x00000000));
    buttonScale50->addListener(this);
    addAndMakeVisible(buttonScale50);
    
    buttonScale75 = new ImageButton();
    buttonScale75->setTriggeredOnMouseDown(true);
    buttonScale75->setImages (false, true, true,
                              imageButtonScale75Normal, 0.999f, Colour (0x00000000),
                           Image(), 1.000f, Colour (0x00000000),
                              imageButtonScale75Selected, 0.75, Colour (0x00000000));
    buttonScale75->addListener(this);
    addAndMakeVisible(buttonScale75);
    
    buttonScale100 = new ImageButton();
    buttonScale100->setTriggeredOnMouseDown(true);
    buttonScale100->setImages (false, true, true,
                               imageButtonScale100Normal, 0.999f, Colour (0x00000000),
                           Image(), 1.000f, Colour (0x00000000),
                               imageButtonScale100Selected, 0.75, Colour (0x00000000));
    buttonScale100->addListener(this);
    addAndMakeVisible(buttonScale100);

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
    
    audioSetupComponent = new AudioDeviceSelectorComponent (*projectManager->getDeviceManager(),
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
    if (button == textButton_ResetAmp)
    {
        // projectManager call
    }
    else if (button == textButton_ResetAttack)
    {
        
    }
    else if (button == textButton_ResetDecay)
    {
        
    }
    else if (button == textButton_ResetSustain)
    {
        
    }
    else if (button == textButton_ResetRelease)
    {
        
    }
    
    else if (button == buttonScale25)
    {
        projectManager->setProjectSettingsParameter(GUI_SCALE, 0);
    }
    else if (button == buttonScale50)
    {
        projectManager->setProjectSettingsParameter(GUI_SCALE, 1);
    }
    else if (button == buttonScale75)
    {
        projectManager->setProjectSettingsParameter(GUI_SCALE, 2);
    }
    else if (button == buttonScale100)
    {
        projectManager->setProjectSettingsParameter(GUI_SCALE, 3);
    }
    
    else if (button == button_FreqToChordMainHarmonics)
    {
        projectManager->setProjectSettingsParameter(FREQUENCY_TO_CHORD, 0);
    }
    else if (button == button_FreqToChordAverage)
    {
        projectManager->setProjectSettingsParameter(FREQUENCY_TO_CHORD, 1);
    }
    else if (button == button_FreqToChordEMA)
    {
        projectManager->setProjectSettingsParameter(FREQUENCY_TO_CHORD, 2);
    }
    
    
    else if (button == button_Load)
    {
        projectManager->loadSettingsFile();
    }
    else if (button == button_Save)
    {
        projectManager->saveSettingsFile();
        
    }
    else if (button == textButton_ChangeLogFile)
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
    else if (button == textButton_ChangeRecordFile)
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
    else if (button == textButton_OpenAudioSettings)
    {
        component_AudioSettings->setOpen(true);
    }
    else if (button == textButton_SpectrogramSetttingsPopup)
    {
        spectrogramPopupComponent->setVisible(true);
    }
    else if (button == buttonMixer)
    {
        PopupFFTWindow * mixerPopup;
        
        AudioMixerComponent * audioMixComponent = new AudioMixerComponent(projectManager);
        audioMixComponent->setBounds(0, 0, 800, 400);
        
        
        mixerPopup = new PopupFFTWindow("Audio Mixer", audioMixComponent, Colours::black, DocumentWindow::allButtons, true);
        mixerPopup ->centreWithSize(audioMixComponent->getWidth(), audioMixComponent->getHeight());
        mixerPopup ->setResizable(true, true);
        mixerPopup ->setVisible(true);
        
    }
    else if (button == scanPluginsButton)
    {
        rescanPlugins();
        
        for (int i = 0; i < NUM_PLUGIN_SLOTS; i++)
        {
            // when finished it should update the plugin List of the combobox..
            PopupMenu * pluginMenu =  comboBoxPluginSelector[i]->getRootMenu();
            
            pluginMenu->clear();
            
            PopupMenu auMenu;   auMenu.setLookAndFeel(&lookAndFeel);
            PopupMenu vstMenu;  vstMenu.setLookAndFeel(&lookAndFeel);
            PopupMenu vst3Menu;  vst3Menu.setLookAndFeel(&lookAndFeel);
                
            // do search for plugins and add to au/vst menu
            for (int i = 0; i < projectManager->knownPluginList->getNumTypes(); i++)
            {
                PluginDescription * desc = projectManager->knownPluginList->getType(i);
                    
                if (!desc->isInstrument)
                {
                    if (desc->pluginFormatName == "AudioUnit")
                    {
                        auMenu.addItem(i+1, desc->name);
                    }
                    else if (desc->pluginFormatName =="VST")
                    {
                        vstMenu.addItem(i+1, desc->name);
                    }
                    else if (desc->pluginFormatName =="VST3")
                    {
                        vst3Menu.addItem(i+1, desc->name);
                    }
                }
            }
                
            pluginMenu->addSubMenu("AU", auMenu);
            pluginMenu->addSubMenu("VST", vstMenu);
            pluginMenu->addSubMenu("VST3", vstMenu);
        }
        
    }
    else
    {
        for (int i = 0; i < NUM_PLUGIN_SLOTS; i++)
        {
            if (button == buttonRemovePlugin[i])
            {
                PluginAssignProcessor * f = projectManager->pluginAssignProcessor[i];
//                PluginWindow::closeAllCurrentlyOpenWindows();
                PluginWindow::closeCurrentlyOpenWindowsFor(i);
                
                if (f->clearPlugin()) {
                    comboBoxPluginSelector[i]->setSelectedId(-1);
                }
            }
            else if (button == buttonOpenPlugin[i])
            {
                PluginAssignProcessor * f = projectManager->pluginAssignProcessor[i];
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
    if (comboBoxThatHasChanged == comboBox_Scales)
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
    else if (comboBoxThatHasChanged == comboBox_RecordFormat)
    {
        projectManager->setProjectSettingsParameter(RECORD_FILE_FORMAT, comboBox_RecordFormat->getSelectedId());
    }
    else if (comboBoxThatHasChanged == comboBox_NoiseType)
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
    if (&editor == textEditor_NotesFrequency)
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
    else if (&editor == textEditor_AmplitudeMin)
    {
        // add hz to the string
        double value = editor.getText().getDoubleValue();
        
        
        // bounds check
        if (value >= MAX_AMPLITUDE) { value = MAX_AMPLITUDE; } if (value <= MIN_AMPLITUDE) { value = MIN_AMPLITUDE; }
        
//        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setProjectSettingsParameter(AMPLITUDE_MIN, value);
        
    }
    else if (&editor == textEditor_AmplitudeMax)
    {
        // add hz to the string
        double value = editor.getText().getDoubleValue();
        
        
        // bounds check
        if (value >= MAX_AMPLITUDE) { value = MAX_AMPLITUDE; } if (value <= MIN_AMPLITUDE) { value = MIN_AMPLITUDE; }
        
//        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setProjectSettingsParameter(AMPLITUDE_MAX, value);
        
    }
    else if (&editor == textEditor_AttackMin)
    {
        // add hz to the string
        double value = editor.getText().getDoubleValue();
        
        
        // bounds check
        if (value >= MAX_ATTACK) { value = MAX_ATTACK; } if (value <= MIN_ATTACK) { value = MIN_ATTACK; }
        
//        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setProjectSettingsParameter(ATTACK_MIN, value);
        
    }
    else if (&editor == textEditor_AttackMax)
    {
        // add hz to the string
        double value = editor.getText().getDoubleValue();
        
        
        // bounds check
        if (value >= MAX_ATTACK) { value = MAX_ATTACK; } if (value <= MIN_ATTACK) { value = MIN_ATTACK; }
        
//        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setProjectSettingsParameter(ATTACK_MAX, value);
        
    }
    else if (&editor == textEditor_DecayMin)
    {
        // add hz to the string
        double value = editor.getText().getDoubleValue();
        
        
        // bounds check
        if (value >= MAX_DECAY) { value = MAX_DECAY; } if (value <= MIN_DECAY) { value = MIN_DECAY; }
        
//        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setProjectSettingsParameter(DECAY_MIN, value);
        
    }
    else if (&editor == textEditor_DecayMax)
    {
        // add hz to the string
        double value = editor.getText().getDoubleValue();
        
        
        // bounds check
        if (value >= MAX_DECAY) { value = MAX_DECAY; } if (value <= MIN_DECAY) { value = MIN_DECAY; }
        
//        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setProjectSettingsParameter(DECAY_MAX, value);
        
    }
    else if (&editor == textEditor_SustainMin)
    {
        // add hz to the string
        double value = editor.getText().getDoubleValue();
        
        
        // bounds check
        if (value >= MAX_SUSTAIN) { value = MAX_SUSTAIN; } if (value <= MIN_SUSTAIN) { value = MIN_SUSTAIN; }
        
//        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setProjectSettingsParameter(SUSTAIN_MIN, value);
        
    }
    else if (&editor == textEditor_SustainMax)
    {
        // add hz to the string
        double value = editor.getText().getDoubleValue();
        
        
        // bounds check
        if (value >= MAX_SUSTAIN) { value = MAX_SUSTAIN; } if (value <= MIN_SUSTAIN) { value = MIN_SUSTAIN; }
        
//        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setProjectSettingsParameter(SUSTAIN_MAX, value);
        
    }
    else if (&editor == textEditor_ReleaseMin)
    {
        // add hz to the string
        double value = editor.getText().getDoubleValue();
        
        
        // bounds check
        if (value >= MAX_RELEASE) { value = MAX_RELEASE;  } if (value <= MIN_RELEASE) { value = MIN_RELEASE; }
        
//        String newVal(value); editor.setText(newVal);
        
        // send to projectManager
        projectManager->setProjectSettingsParameter(RELEASE_MIN, value);
        
    }
    else if (&editor == textEditor_ReleaseMax)
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
    g.drawImage(imageBackground, 0, 0, 1560 * scaleFactor, 1440 * scaleFactor, 0, 0, 1560, 1440);

    g.setColour(Colour::fromString("ff424242"));
    g.fillRoundedRectangle({851 * scaleFactor, 858 * scaleFactor, 679 * scaleFactor, 100 * scaleFactor}, 5.f);

    g.setColour(Colours::white);
    g.setFont(Font{30.f * scaleFactor}.withStyle(Font::FontStyleFlags::bold));
    g.drawFittedText("Find fundamental frequency:", Rectangle<double>{878 * scaleFactor, 890 * scaleFactor, 338 * scaleFactor, 39 * scaleFactor}.toNearestInt(), Justification::centredLeft, 1);
}

void SettingsComponent::resized()
{
    int fontSize = 29;
    
    float labelW = 600;
    float labelX = 360;
    label_LogFileLocation->setBounds(labelX * scaleFactor, 60 * scaleFactor, labelW * scaleFactor , 41 * scaleFactor);
    label_LogFileLocation->setFont(fontSize * scaleFactor);
    
    label_RecordFileLocation->setBounds((labelX + 60) * scaleFactor, 125 * scaleFactor, labelW * scaleFactor, 41 * scaleFactor);
    label_RecordFileLocation->setFont(fontSize * scaleFactor);
    
    float freqY = 296;
    float xAdj = 204;
    
    textEditor_NotesFrequency->setBounds((288 - xAdj) * scaleFactor, freqY * scaleFactor, 117 * scaleFactor, 41 * scaleFactor);
    textEditor_NotesFrequency->applyFontToAllText(fontSize  * scaleFactor);
    
    label_B_Frequency->setBounds((443 - xAdj) * scaleFactor, freqY * scaleFactor, 117 * scaleFactor, 41 * scaleFactor);
    label_B_Frequency->setFont(fontSize  * scaleFactor);
    
    label_C_Frequency->setBounds((598 - xAdj) * scaleFactor, freqY * scaleFactor, 117 * scaleFactor, 41 * scaleFactor);
    label_C_Frequency->setFont(fontSize  * scaleFactor);
    
    label_D_Frequency->setBounds((754 - xAdj) * scaleFactor, freqY * scaleFactor, 117 * scaleFactor, 41 * scaleFactor);
    label_D_Frequency->setFont(fontSize  * scaleFactor);
    
    label_E_Frequency->setBounds((909 - xAdj) * scaleFactor, freqY * scaleFactor, 117 * scaleFactor, 41 * scaleFactor);
    label_E_Frequency->setFont(fontSize * scaleFactor);
    
    label_F_Frequency->setBounds((1064 - xAdj) * scaleFactor, freqY * scaleFactor, 117 * scaleFactor, 41 * scaleFactor);
    label_F_Frequency->setFont(fontSize  * scaleFactor);
    
    label_G_Frequency->setBounds((1219 - xAdj) * scaleFactor, freqY * scaleFactor, 117 * scaleFactor, 41 * scaleFactor);
    label_G_Frequency->setFont(fontSize  * scaleFactor);
    
    float xMin = 260;
    float xMax = 455;
    
    fontSize = 31;
    
    textEditor_AmplitudeMin->setBounds(xMin * scaleFactor, 478 * scaleFactor, 125 * scaleFactor, 41 * scaleFactor);
    textEditor_AmplitudeMin->applyFontToAllText(fontSize  * scaleFactor);
    
    textEditor_AttackMin->setBounds(xMin * scaleFactor, 577 * scaleFactor, 125 * scaleFactor, 41 * scaleFactor);
    textEditor_AttackMin->applyFontToAllText(fontSize  * scaleFactor);
    
    textEditor_DecayMin->setBounds(xMin * scaleFactor, 642 * scaleFactor, 125 * scaleFactor, 41 * scaleFactor);
    textEditor_DecayMin->applyFontToAllText(fontSize * scaleFactor);
    
    textEditor_SustainMin->setBounds(xMin * scaleFactor, 705 * scaleFactor, 125 * scaleFactor, 41 * scaleFactor);
    textEditor_SustainMin->applyFontToAllText(fontSize * scaleFactor);
    
    textEditor_ReleaseMin->setBounds(xMin * scaleFactor, 769 * scaleFactor, 125 * scaleFactor, 41 * scaleFactor);
    textEditor_ReleaseMin->applyFontToAllText(fontSize * scaleFactor);
    
    int maxW = 130;
    
    textEditor_AmplitudeMax->setBounds(xMax * scaleFactor, 478 * scaleFactor, maxW * scaleFactor, 41 * scaleFactor);
    textEditor_AmplitudeMax->applyFontToAllText(fontSize * scaleFactor);
    
    textEditor_AttackMax->setBounds(xMax * scaleFactor, 577 * scaleFactor, maxW * scaleFactor, 41 * scaleFactor);
    textEditor_AttackMax->applyFontToAllText(fontSize * scaleFactor);
    
    textEditor_DecayMax->setBounds(xMax * scaleFactor, 642 * scaleFactor, maxW * scaleFactor, 41 * scaleFactor);
    textEditor_DecayMax->applyFontToAllText(fontSize * scaleFactor);
    
    textEditor_SustainMax->setBounds(xMax * scaleFactor, 705 * scaleFactor, maxW * scaleFactor, 41 * scaleFactor);
    textEditor_SustainMax->applyFontToAllText(fontSize * scaleFactor);
    
    textEditor_ReleaseMax->setBounds(xMax * scaleFactor, 769 * scaleFactor, maxW * scaleFactor, 41 * scaleFactor);
    textEditor_ReleaseMax->applyFontToAllText(fontSize * scaleFactor);
    
    int buttonW = 160 * scaleFactor;
    textButton_ResetAmp->setBounds(633 * scaleFactor,  478 * scaleFactor, buttonW , 40 * scaleFactor);
    textButton_ResetAttack->setBounds(633 * scaleFactor,  577 * scaleFactor, buttonW , 40 * scaleFactor);
    textButton_ResetDecay->setBounds(633 * scaleFactor, 642 * scaleFactor, buttonW , 40 * scaleFactor);
    textButton_ResetSustain->setBounds(633 * scaleFactor, 705 * scaleFactor, buttonW , 40 * scaleFactor);
    textButton_ResetRelease->setBounds(633 * scaleFactor, 769 * scaleFactor, buttonW , 40 * scaleFactor);
    
    textButton_ChangeLogFile->setBounds(1020 * scaleFactor, 62 * scaleFactor, 117 * scaleFactor, 41 * scaleFactor);
    textButton_ChangeRecordFile->setBounds(1020 * scaleFactor, 125 * scaleFactor, 117 * scaleFactor, 41 * scaleFactor);
    textButton_OpenAudioSettings->setBounds(1190 * scaleFactor, 62 * scaleFactor, 310 * scaleFactor, 41 * scaleFactor);
    
    textButton_SpectrogramSetttingsPopup->setBounds(1240 * scaleFactor, 740 * scaleFactor, 280 * scaleFactor, 41 * scaleFactor);
    
    spectrogramPopupComponent->setBounds(0 * scaleFactor, 0 * scaleFactor, getWidth(), getHeight());
    
    scanPluginsButton   ->setBounds(360 * scaleFactor, 873 * scaleFactor, 150 * scaleFactor, 41 * scaleFactor);
    
    comboBox_Scales->setBounds(1180 * scaleFactor, 308 * scaleFactor, 330 * scaleFactor, 41 * scaleFactor);
    
    audioSetupComponent->setBounds(60 * scaleFactor, 300 * scaleFactor, 1300 * scaleFactor, 1000 * scaleFactor);
    audioSetupComponent->setItemHeight(70 * scaleFactor);
    
    // plugin rack
    int comboWidth     = 228;
    int buttonWidth    = 41;
    int centreX         = 160;
    int y               = 960;
    int ySpace          = 60;
    
    fontSize = 28;
    
    for (int i = 0; i < NUM_PLUGIN_SLOTS; i++)
    {
        labelPluginSlot[i]          ->setBounds((centreX - 120) * scaleFactor, (y + (i * ySpace)) * scaleFactor, comboWidth * scaleFactor, 41 * scaleFactor);
        labelPluginSlot[i]          ->setFont(fontSize * scaleFactor);
        
        comboBoxPluginSelector[i]   ->setBounds((centreX + 80) * scaleFactor, (y + (i * ySpace)) * scaleFactor, comboWidth * scaleFactor, 41 * scaleFactor);
        
        buttonRemovePlugin[i]       ->setBounds((centreX + comboWidth + 90 ) * scaleFactor, (y + 11 + (i * ySpace)) * scaleFactor, 22 * scaleFactor, 22 * scaleFactor);
        
        buttonOpenPlugin[i]         ->setBounds((centreX ) * scaleFactor, (y + (i * ySpace) + 10 ) * scaleFactor, 66 * scaleFactor, 22 * scaleFactor);
    }
    
//    comboBox_RecordFormat->setBounds(1260 * scaleFactor, 1300 * scaleFactor, 250 * scaleFactor, 41 * scaleFactor);
    
    comboBox_NoiseType->setBounds(1350 * scaleFactor, 130 * scaleFactor, 150 * scaleFactor, 41 * scaleFactor);
    
    button_Load->setBounds(1000 * scaleFactor, 1300 * scaleFactor, 257 * scaleFactor, 69 * scaleFactor);
    button_Save->setBounds(1280 * scaleFactor, 1300 * scaleFactor, 257 * scaleFactor, 69 * scaleFactor);

    // Freq to Chord
    freqY = 1150;
    float bX = 870;
    button_FreqToChordMainHarmonics ->setBounds(bX * scaleFactor, 478 * scaleFactor, 38 * scaleFactor, 38 * scaleFactor);
    button_FreqToChordAverage       ->setBounds(bX * scaleFactor, 528 * scaleFactor, 38 * scaleFactor, 38 * scaleFactor);
    button_FreqToChordEMA           ->setBounds(bX * scaleFactor, 580 * scaleFactor, 38 * scaleFactor, 38 * scaleFactor);
    
    
    float scaleButtonX = 874;
    float sp = 80;
    float scY = 726;
    
    buttonScale25    ->setBounds((scaleButtonX + (sp * 0)) * scaleFactor, scY * scaleFactor, 70 * scaleFactor, 70 * scaleFactor);
    buttonScale50   ->setBounds((scaleButtonX + (sp * 1)) * scaleFactor, scY * scaleFactor, 70 * scaleFactor, 70 * scaleFactor);
    buttonScale75   ->setBounds((scaleButtonX + (sp * 2)) * scaleFactor, scY * scaleFactor, 70 * scaleFactor, 70 * scaleFactor);
    buttonScale100  ->setBounds((scaleButtonX + (sp * 3)) * scaleFactor, scY * scaleFactor, 70 * scaleFactor, 70 * scaleFactor);
    
    buttonMixer     ->setBounds((centreX - 120) * scaleFactor, (y + (6 * ySpace) + 10 ) * scaleFactor, 310 * scaleFactor, 41 * scaleFactor);

    fundamentalFrequencyAlgorithmChooser.setBounds(1243 * scaleFactor, 889 * scaleFactor, 263 * scaleFactor, 45 * scaleFactor);
}

void SettingsComponent::rescanPlugins()
{
    // only scan for audio units

    // only scan for FX , not instruments


    AlertWindow alertWindow(TRANS("Plugin Scanning"), \
                            TRANS("If you choose to scan folders that contain non-plugin files, "
                                  "then scanning may take a long time, and can cause crashes when "
                                  "attempting to load unsuitable files."),
                            AlertWindow::WarningIcon);
    
    alertWindow.addButton("OK", 1);
    alertWindow.addButton("Cancel", 0);
    
//    double val = 0;
//
//    alertWindow.addProgressBarComponent(val);
    
    int result = alertWindow.runModalLoop();

    
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
