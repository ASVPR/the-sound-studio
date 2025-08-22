/*
  ==============================================================================

    SettingsComponent.h
    Created: 13 Mar 2019 9:56:56pm
    Author:  Gary Jones

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "CustomLookAndFeel.h"
#include "ProjectManager.h"
#include "HighQualityMeter.h"
#include "MenuViewInterface.h"
#include <memory>

class AudioMixerComponent : public Component, public Slider::Listener, public Button::Listener
{
public:
    AudioMixerComponent(ProjectManager * pm)
    {
        projectManager = pm;
        
        for (int i = 0 ; i < 4; i++)
        {
            sliderInputChannel[i]           = std::make_unique<Slider>();
            sliderInputChannel[i]           ->setRange (0, 1.0, 0);
            sliderInputChannel[i]           ->setSliderStyle (Slider::LinearVertical);
            sliderInputChannel[i]           ->addListener (this);
            addAndMakeVisible(sliderInputChannel[i].get());
            
            
            sliderOutputChannel[i]          = std::make_unique<Slider>();
            sliderOutputChannel[i]          ->setRange (0, 1.0, 0);
            sliderOutputChannel[i]          ->setSliderStyle (Slider::LinearVertical);
            sliderOutputChannel[i]          ->addListener (this);
            addAndMakeVisible(sliderOutputChannel[i].get());
            
            buttonFFTInputChannel[i]        = std::make_unique<TextButton>("");
            buttonFFTInputChannel[i]        ->setButtonText("FFT");
            buttonFFTInputChannel[i]        ->addListener(this);
            addAndMakeVisible(buttonFFTInputChannel[i].get());
            
            buttonFFTOutputChannel[i]       = std::make_unique<TextButton>("");
            buttonFFTOutputChannel[i]       ->setButtonText("FFT");
            buttonFFTOutputChannel[i]       ->addListener(this);
            addAndMakeVisible(buttonFFTOutputChannel[i].get());
        }
        
        syncUI();
    }
    
    
    ~AudioMixerComponent()
    {
        
    }
    
    void resized() override
    {
        for (int i = 0 ; i < 4; i++)
        {
            float leftBorder    = 10.f;
            float topBorder     = 10.f;
            float buttonH       = 50.f;
            float sliderH       = getHeight() - topBorder - buttonH;
            float space         = (getWidth() - (leftBorder*2)) / 8;
            
            
            sliderInputChannel[i]   ->setBounds(leftBorder + (space * i), topBorder, space, sliderH);
            sliderOutputChannel[i]  ->setBounds(leftBorder + (space * (i + 4)), topBorder, space, sliderH);
            
            buttonFFTInputChannel[i] ->setBounds(leftBorder + (space * i), topBorder+sliderH, space, buttonH);
            buttonFFTOutputChannel[i]->setBounds(leftBorder + (space * (i + 4)), topBorder+sliderH, space, buttonH);
        }
    }
    
    void sliderValueChanged (Slider* slider) override
    {
        if (slider == sliderInputChannel[0].get())
        {
            projectManager->setProjectSettingsParameter(MIXER_INPUT_GAIN_1, sliderInputChannel[0]->getValue());
        }
        else if (slider == sliderInputChannel[1].get())
        {
            projectManager->setProjectSettingsParameter(MIXER_INPUT_GAIN_2, sliderInputChannel[1]->getValue());
        }
        else if (slider == sliderInputChannel[2].get())
        {
            projectManager->setProjectSettingsParameter(MIXER_INPUT_GAIN_3, sliderInputChannel[2]->getValue());
        }
        else if (slider == sliderInputChannel[3].get())
        {
            projectManager->setProjectSettingsParameter(MIXER_INPUT_GAIN_4, sliderInputChannel[3]->getValue());
        }
        else if (slider == sliderOutputChannel[0].get())
        {
            projectManager->setProjectSettingsParameter(MIXER_OUTPUT_GAIN_1, sliderOutputChannel[0]->getValue());
        }
        else if (slider == sliderOutputChannel[1].get())
        {
            projectManager->setProjectSettingsParameter(MIXER_OUTPUT_GAIN_2, sliderOutputChannel[1]->getValue());
        }
        else if (slider == sliderOutputChannel[2].get())
        {
            projectManager->setProjectSettingsParameter(MIXER_OUTPUT_GAIN_3, sliderOutputChannel[2]->getValue());
        }
        else if (slider == sliderOutputChannel[3].get())
        {
            projectManager->setProjectSettingsParameter(MIXER_OUTPUT_GAIN_4, sliderOutputChannel[3]->getValue());
        }
        
        syncUI();
    }
    
    void buttonClicked (Button*button) override
    {
        if (button == buttonFFTInputChannel[0].get())
        {
            projectManager->setProjectSettingsParameter(MIXER_INPUT_FFT_1, !buttonFFTInputChannel[0]->getToggleState());
        }
        else if (button == buttonFFTInputChannel[1].get())
        {
            projectManager->setProjectSettingsParameter(MIXER_INPUT_FFT_2, !buttonFFTInputChannel[1]->getToggleState());
        }
        else if (button == buttonFFTInputChannel[2].get())
        {
            projectManager->setProjectSettingsParameter(MIXER_INPUT_FFT_3, !buttonFFTInputChannel[2]->getToggleState());
        }
        else if (button == buttonFFTInputChannel[3].get())
        {
            projectManager->setProjectSettingsParameter(MIXER_INPUT_FFT_4, !buttonFFTInputChannel[3]->getToggleState());
        }
        else if (button == buttonFFTOutputChannel[0].get())
        {
            projectManager->setProjectSettingsParameter(MIXER_OUTPUT_FFT_1, !buttonFFTOutputChannel[0]->getToggleState());
        }
        else if (button == buttonFFTOutputChannel[1].get())
        {
            projectManager->setProjectSettingsParameter(MIXER_OUTPUT_FFT_2, !buttonFFTOutputChannel[1]->getToggleState());
        }
        else if (button == buttonFFTOutputChannel[2].get())
        {
            projectManager->setProjectSettingsParameter(MIXER_OUTPUT_FFT_3, !buttonFFTOutputChannel[2]->getToggleState());
        }
        else if (button == buttonFFTOutputChannel[3].get())
        {
            projectManager->setProjectSettingsParameter(MIXER_OUTPUT_FFT_4, !buttonFFTOutputChannel[3]->getToggleState());
        }
        
        syncUI();
    }
    
    void syncUI()
    {
        for (int i = 0 ; i < 4; i++)
        {
            sliderInputChannel[i]   ->setValue(projectManager->getProjectSettingsParameter(MIXER_INPUT_GAIN_1 + i), dontSendNotification);
            sliderOutputChannel[i]  ->setValue(projectManager->getProjectSettingsParameter(MIXER_OUTPUT_GAIN_1 + i), dontSendNotification);
            
            buttonFFTInputChannel[i] ->setToggleState((bool)projectManager->getProjectSettingsParameter(MIXER_INPUT_FFT_1 + i), dontSendNotification);
            buttonFFTOutputChannel[i]->setToggleState((bool)projectManager->getProjectSettingsParameter(MIXER_OUTPUT_FFT_1 + i), dontSendNotification);
        }
    }
    
    
private:
    ProjectManager * projectManager;
    
    std::unique_ptr<Slider> sliderInputChannel[4];
    std::unique_ptr<Slider> sliderOutputChannel[4];
    
    std::unique_ptr<TextButton> buttonFFTInputChannel[4];
    std::unique_ptr<TextButton> buttonFFTOutputChannel[4];

};


class RoundedColourOutputComponentWithSelector : public Component, public ChangeListener
{
public:
    RoundedColourOutputComponentWithSelector(ProjectManager * pm, int i)
    {
        projectManager = pm;
        
        index = i;
        
        mainColour = Colours::green;

    }
    
    ~RoundedColourOutputComponentWithSelector() { }
    
    void paint(Graphics&g) override
    {
        ColourGradient gradient(mainColour, 0, 0, mainColour, getWidth(), getHeight(), false);
        
        g.setGradientFill(gradient);
        
        g.fillRoundedRectangle(0, 0, getWidth(), getHeight(), 6);
    }
    
    void mouseDown(const MouseEvent& event) override
    {

        auto colourSelector = std::make_unique<ColourSelector>();
        colourSelector->setName ("background");
        colourSelector->setCurrentColour (mainColour);
        colourSelector->setSize (300, 400);
        
        colourSelector->addChangeListener(this);

        CallOutBox::launchAsynchronously (std::unique_ptr<juce::Component>(colourSelector.release()), this->getScreenBounds(), nullptr);
        
    }
    
    void changeListenerCallback (ChangeBroadcaster* source) override
    {
        if (ColourSelector* cs = dynamic_cast <ColourSelector*> (source))
        {
            projectManager->setSettingsColorParameter(index, cs->getCurrentColour().getARGB());
        }
        
    }

    void setColour(Colour newColour)
    {
        mainColour = newColour;
        
        repaint();
    }
    
    void setColour(int agrb)
    {
        mainColour = Colour(agrb);
        
        repaint();
        
    }
    
private:
    ProjectManager * projectManager;
    Colour mainColour;
    
    float value = 1.f;
    int index = 0;
};


class SpectrogramSettingsPopup : public Component, public TextEditor::Listener, public Button::Listener, public ProjectManager::UIListener, public ComboBox::Listener
{
public:
    SpectrogramSettingsPopup(ProjectManager * pm)
    {
        projectManager = pm;
        projectManager->addUIListener(this);
        
        imageBackground = ImageCache::getFromMemory(BinaryData::SettingsSpectrogramBackground, BinaryData::SettingsSpectrogramBackgroundSize);
        imageCloseButton            = ImageCache::getFromMemory(BinaryData::CloseButton2x_png, BinaryData::CloseButton2x_pngSize);
        imageBlueButtonNormal       = ImageCache::getFromMemory(BinaryData::BlueButton_Normal_png, BinaryData::BlueButton_Normal_pngSize);
        imageBlueButtonSelected     = ImageCache::getFromMemory(BinaryData::BlueButton_Selected_png, BinaryData::BlueButton_Selected_pngSize);
        imageAddButton              = ImageCache::getFromMemory(BinaryData::ApplyButton2x_png, BinaryData::ApplyButton2x_pngSize);
        
        imageBlueCheckButtonNormal  = ImageCache::getFromMemory(BinaryData::Button_Checkbox_Normal_Max_png, BinaryData::Button_Checkbox_Normal_Max_pngSize);
        imageBlueCheckButtonSelected= ImageCache::getFromMemory(BinaryData::Button_Checkbox_Selected_Max_png, BinaryData::Button_Checkbox_Selected_Max_pngSize);
        
        // fonts
        Typeface::Ptr AssistantBold     = Typeface::createSystemTypefaceFor(BinaryData::AssistantBold_ttf, BinaryData::AssistantBold_ttfSize);
        Typeface::Ptr AssistantSemiBold   = Typeface::createSystemTypefaceFor(BinaryData::AssistantSemiBold_ttf, BinaryData::AssistantSemiBold_ttfSize);
        Font fontBold(AssistantBold);
        Font fontNormal(AssistantSemiBold);
        
        fontNormal.setHeight(40);
        fontBold.setHeight(30);
        
        button_Close = std::make_unique<ImageButton>();
        button_Close->setTriggeredOnMouseDown(true);
        button_Close->setImages (false, true, true,
                               imageCloseButton, 0.999f, Colour (0x00000000),
                               Image(), 1.000f, Colour (0x00000000),
                               imageCloseButton, 0.75, Colour (0x00000000));
        button_Close->addListener(this);
        addAndMakeVisible(button_Close.get());
        
        button_Add = std::make_unique<ImageButton>();
        button_Add->setTriggeredOnMouseDown(true);
        button_Add->setImages (false, true, true,
                                    imageAddButton, 0.999f, Colour (0x00000000),
                                    Image(), 1.000f, Colour (0x00000000),
                                    imageAddButton, 0.888, Colour (0x00000000));
        button_Add->addListener(this);
        addAndMakeVisible(button_Add.get());
        
        
        comboBox_FFTSize = std::make_unique<ComboBox>();
        comboBox_FFTSize->setLookAndFeel(&lookAndFeel);
        comboBox_FFTSize->addListener(this);
        PopupMenu * fftSizeMenu =  comboBox_FFTSize->getRootMenu();
        fftSizeMenu ->setLookAndFeel(&lookAndFeel);

        fftSizeMenu->addItem(1, "1024 Samples");
        fftSizeMenu->addItem(2, "2048 Samples");
        fftSizeMenu->addItem(3, "4096 Samples");
        fftSizeMenu->addItem(4, "8192 Samples");
        fftSizeMenu->addItem(5, "16384 Samples");
        fftSizeMenu->addItem(6, "32768 Samples");
        
        
        addAndMakeVisible(comboBox_FFTSize.get());
        comboBox_FFTSize->setBounds(1000, 880 + 46, 250, 41); //(242, 497, 125, 41);
        
        comboBox_FFTWindow = std::make_unique<ComboBox>();
        comboBox_FFTWindow->setLookAndFeel(&lookAndFeel);
        comboBox_FFTWindow->addListener(this);
        PopupMenu * fftWindowMenu =  comboBox_FFTWindow->getRootMenu();
        fftWindowMenu ->setLookAndFeel(&lookAndFeel);

        fftWindowMenu->addItem(1, "Rectangular");
        fftWindowMenu->addItem(2, "Triangular");
        fftWindowMenu->addItem(3, "Hann");
        fftWindowMenu->addItem(4, "Hamming");
        fftWindowMenu->addItem(5, "Blackman");
        fftWindowMenu->addItem(6, "BlackmanHarris");
        fftWindowMenu->addItem(7, "Flat Top");
        fftWindowMenu->addItem(8, "Kaiser");
        
        addAndMakeVisible(comboBox_FFTWindow.get());
        comboBox_FFTWindow->setBounds(1000, 980 + 46, 250, 41); //(242, 497, 125, 41);
        
        
        // Bool button
        button_HighestPeakFreqBool = std::make_unique<ImageButton>();
        button_HighestPeakFreqBool->setTriggeredOnMouseDown(true);
        button_HighestPeakFreqBool->setImages (false, true, true,
                                    imageBlueCheckButtonNormal, 0.999f, Colour (0x00000000),
                                    Image(), 1.000f, Colour (0x00000000),
                                    imageBlueCheckButtonSelected, 1.0, Colour (0x00000000));
        button_HighestPeakFreqBool->addListener(this);
        button_HighestPeakFreqBool->setBounds(666, 6, 38, 38);
        addAndMakeVisible(button_HighestPeakFreqBool.get());
        
        button_HighestPeakOctaveBool = std::make_unique<ImageButton>();
        button_HighestPeakOctaveBool->setTriggeredOnMouseDown(true);
        button_HighestPeakOctaveBool->setImages (false, true, true,
                                    imageBlueCheckButtonNormal, 0.999f, Colour (0x00000000),
                                    Image(), 1.000f, Colour (0x00000000),
                                    imageBlueCheckButtonSelected, 1.0, Colour (0x00000000));
        button_HighestPeakOctaveBool->addListener(this);
        button_HighestPeakOctaveBool->setBounds(666, 6, 38, 38);
        addAndMakeVisible(button_HighestPeakOctaveBool.get());
        
        textEditor_NumHighestPeakFreqs = std::make_unique<TextEditor>();
        textEditor_NumHighestPeakFreqs->setInputRestrictions(1, "0123456789");
        textEditor_NumHighestPeakFreqs->setBounds(432, 787, 125, 41);
        textEditor_NumHighestPeakFreqs->setText("5 peaks");
        textEditor_NumHighestPeakFreqs->addListener(this);
        textEditor_NumHighestPeakFreqs->setLookAndFeel(&lookAndFeel);
        textEditor_NumHighestPeakFreqs->setJustification(Justification::centred);
        textEditor_NumHighestPeakFreqs->setFont(fontNormal);
        textEditor_NumHighestPeakFreqs->setColour(TextEditor::textColourId, Colours::darkgrey);
        textEditor_NumHighestPeakFreqs->applyFontToAllText(fontNormal);
        textEditor_NumHighestPeakFreqs->applyColourToAllText(Colours::darkgrey);
        addAndMakeVisible(textEditor_NumHighestPeakFreqs.get());
        
        textEditor_NumHighestPeakOctaves = std::make_unique<TextEditor>();
        textEditor_NumHighestPeakOctaves->setInputRestrictions(1, "0123456789");
        textEditor_NumHighestPeakOctaves->setBounds(432, 787, 125, 41);
        textEditor_NumHighestPeakOctaves->setText("5 octaves");
        textEditor_NumHighestPeakOctaves->addListener(this);
        textEditor_NumHighestPeakOctaves->setLookAndFeel(&lookAndFeel);
        textEditor_NumHighestPeakOctaves->setJustification(Justification::centred);
        textEditor_NumHighestPeakOctaves->setFont(fontNormal);
        textEditor_NumHighestPeakOctaves->setColour(TextEditor::textColourId, Colours::darkgrey);
        textEditor_NumHighestPeakOctaves->applyFontToAllText(fontNormal);
        textEditor_NumHighestPeakOctaves->applyColourToAllText(Colours::darkgrey);
        addAndMakeVisible(textEditor_NumHighestPeakOctaves.get());
        
        textEditor_FFTDelay = std::make_unique<TextEditor>();
        textEditor_FFTDelay->setInputRestrictions(5, "0123456789");
        textEditor_FFTDelay->setBounds(432, 787, 125, 41);
        textEditor_FFTDelay->setText("1000ms");
        textEditor_FFTDelay->addListener(this);
        textEditor_FFTDelay->setLookAndFeel(&lookAndFeel);
        textEditor_FFTDelay->setJustification(Justification::centred);
        textEditor_FFTDelay->setFont(fontNormal);
        textEditor_FFTDelay->setColour(TextEditor::textColourId, Colours::darkgrey);
        textEditor_FFTDelay->applyFontToAllText(fontNormal);
        textEditor_FFTDelay->applyColourToAllText(Colours::darkgrey);
        addAndMakeVisible(textEditor_FFTDelay.get());
        
        colourSelectorFFTMain = new RoundedColourOutputComponentWithSelector(projectManager, 0);
        addAndMakeVisible(colourSelectorFFTMain);
        
        colourSelectorFFTSec = new RoundedColourOutputComponentWithSelector(projectManager, 1);
        addAndMakeVisible(colourSelectorFFTSec);
        
        colourSelectorOctaveMain = new RoundedColourOutputComponentWithSelector(projectManager, 2);
        addAndMakeVisible(colourSelectorOctaveMain);
        
        colourSelectorOctaveSec = new RoundedColourOutputComponentWithSelector(projectManager, 3);
        addAndMakeVisible(colourSelectorOctaveSec);
        
        colourSelectorColourMain = new RoundedColourOutputComponentWithSelector(projectManager, 4);
        addAndMakeVisible(colourSelectorColourMain);
        
        colourSelectorColourSec = new RoundedColourOutputComponentWithSelector(projectManager, 5);
        addAndMakeVisible(colourSelectorColourSec);
        
        colourSelector3DMain = new RoundedColourOutputComponentWithSelector(projectManager, 6);
        addAndMakeVisible(colourSelector3DMain);
        
        colourSelector3DSec = new RoundedColourOutputComponentWithSelector(projectManager, 7);
        addAndMakeVisible(colourSelector3DSec);
        
        colourSelectorFrequencyMain = new RoundedColourOutputComponentWithSelector(projectManager, 8);
        addAndMakeVisible(colourSelectorFrequencyMain);
        
        colourSelectorFrequencySec = new RoundedColourOutputComponentWithSelector(projectManager, 9);
        addAndMakeVisible(colourSelectorFrequencySec);
        
        colourSelectorLissajousMain = new RoundedColourOutputComponentWithSelector(projectManager, 10);
        addAndMakeVisible(colourSelectorLissajousMain);
        
        colourSelectorLissajousSec = new RoundedColourOutputComponentWithSelector(projectManager, 11);
        addAndMakeVisible(colourSelectorLissajousSec);
        
        
    }
    
    ~SpectrogramSettingsPopup()
    {
        
    }
    
    void paint (Graphics&g) override
    {
        g.drawImage(imageBackground, 0, 0, 1560 * scaleFactor, 1440 * scaleFactor, 0, 0, 1560, 1440);
    }
    
    void resized() override
    {
        button_Close                    ->setBounds(1380 * scaleFactor, 224 * scaleFactor, 150 * scaleFactor, 28 * scaleFactor);
        
        button_Add                      ->setBounds(609 * scaleFactor, 1040 * scaleFactor, 341 * scaleFactor, 84 * scaleFactor);
        
        comboBox_FFTSize                ->setBounds(156 * scaleFactor, 872 * scaleFactor, 200 * scaleFactor, 41 * scaleFactor);
        
        comboBox_FFTWindow              ->setBounds(420 * scaleFactor, 872 * scaleFactor, 200 * scaleFactor, 41 * scaleFactor);
        
        textEditor_NumHighestPeakFreqs  ->setBounds(600 * scaleFactor, 523 * scaleFactor, 125 * scaleFactor, 41 * scaleFactor);
        textEditor_NumHighestPeakFreqs  ->applyFontToAllText(33 * scaleFactor);
        
        textEditor_NumHighestPeakOctaves->setBounds(550 * scaleFactor, 662 * scaleFactor, 125 * scaleFactor, 41 * scaleFactor);
        textEditor_NumHighestPeakOctaves->applyFontToAllText(33 * scaleFactor);
        
        textEditor_FFTDelay             ->setBounds(954 * scaleFactor, 406 * scaleFactor, 125 * scaleFactor, 41 * scaleFactor);
        textEditor_FFTDelay             ->applyFontToAllText(33 * scaleFactor);
        
        button_HighestPeakFreqBool      ->setBounds(155 * scaleFactor, 472 * scaleFactor, 38 * scaleFactor, 38 * scaleFactor);
        button_HighestPeakOctaveBool    ->setBounds(155 * scaleFactor, 610 * scaleFactor, 38 * scaleFactor, 38 * scaleFactor);
        
        float mainX = 1227 * scaleFactor;
        float secX  = 1316 * scaleFactor;
        float fftY  = 595 * scaleFactor;
        float ySpace = 65 * scaleFactor;
        float colourW = 64 * scaleFactor;
        float colourH = 27 * scaleFactor;

        colourSelectorFFTMain   ->setBounds(mainX, fftY + (ySpace * 0), colourW, colourH);
        colourSelectorFFTSec    ->setBounds(secX, fftY + (ySpace * 0), colourW, colourH);
        
        colourSelectorOctaveMain->setBounds(mainX, fftY + (ySpace * 1), colourW, colourH);
        colourSelectorOctaveSec->setBounds(secX, fftY + (ySpace * 1), colourW, colourH);
        
        colourSelectorColourMain->setBounds(mainX, fftY + (ySpace * 2), colourW, colourH);
        colourSelectorColourSec->setBounds(secX, fftY + (ySpace * 2), colourW, colourH);
        
        colourSelector3DMain->setBounds(mainX, fftY + (ySpace * 3), colourW, colourH);
        colourSelector3DSec->setBounds(secX, fftY + (ySpace * 3), colourW, colourH);
        
        colourSelectorFrequencyMain->setBounds(mainX, fftY + (ySpace * 4), colourW, colourH);
        colourSelectorFrequencySec->setBounds(secX, fftY + (ySpace * 4), colourW, colourH);
        
        colourSelectorLissajousMain->setBounds(mainX, fftY + (ySpace * 5), colourW, colourH);
        colourSelectorLissajousSec->setBounds(secX, fftY + (ySpace * 5), colourW, colourH);


    }
    
    // Text editor listener overrides
    void textEditorTextChanged (TextEditor&editor)override {}
    void textEditorReturnKeyPressed (TextEditor&editor)override
    {
        if (&editor == textEditor_NumHighestPeakFreqs.get())
        {
            // add hz to the string
            int value = editor.getText().getIntValue();
                
            // send to projectManager
            projectManager->setProjectSettingsParameter(NUMBER_HIGHEST_PEAK_FREQUENCIES, value);
        }
        else if (&editor == textEditor_NumHighestPeakOctaves.get())
        {
            // add hz to the string
            int value = editor.getText().getIntValue();
                
            // send to projectManager
            projectManager->setProjectSettingsParameter(NUMBER_HIGHEST_PEAK_OCTAVES, value);
        }
        else if (&editor == textEditor_FFTDelay.get())
        {
            // add hz to the string
            int value = editor.getText().getIntValue();
                
            // send to projectManager
            projectManager->setProjectSettingsParameter(FFT_DELAY_MS, value);
        }
    }
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged)override
    {
        if (comboBoxThatHasChanged == comboBox_FFTSize.get())
        {
            projectManager->setProjectSettingsParameter(FFT_SIZE, comboBox_FFTSize->getSelectedId());
        }
        else if (comboBoxThatHasChanged == comboBox_FFTWindow.get())
        {
            projectManager->setProjectSettingsParameter(FFT_WINDOW, comboBox_FFTWindow->getSelectedId());
        }
    }
    
    void buttonClicked (Button*button) override
    {
        if (button == button_HighestPeakFreqBool.get())
        {
            projectManager->setProjectSettingsParameter(SHOW_HIGHEST_PEAK_FREQUENCY, !button->getToggleState());
        }
        else if (button == button_HighestPeakOctaveBool.get())
        {
            projectManager->setProjectSettingsParameter(SHOW_HIGHEST_PEAK_OCTAVES, !button->getToggleState());
        }
        else if (button == button_Close.get())
        {
            setVisible(false);
        }
        else if (button == button_Add.get())
        {
            setVisible(false);
        }
    }
    
    void updateUI(int settingIndex)
    {
        if (settingIndex == FFT_SIZE)
        {
            int size = (int)projectManager->getProjectSettingsParameter(FFT_SIZE);
            comboBox_FFTSize->setSelectedId(size, dontSendNotification);
        }
        else if (settingIndex == FFT_WINDOW)
        {
            int size = (int)projectManager->getProjectSettingsParameter(FFT_WINDOW);
            comboBox_FFTWindow->setSelectedId(size, dontSendNotification);
        }
        else if (settingIndex == SHOW_HIGHEST_PEAK_FREQUENCY)
        {
            bool b = projectManager->getProjectSettingsParameter(SHOW_HIGHEST_PEAK_FREQUENCY);
            button_HighestPeakFreqBool->setToggleState(b, dontSendNotification);
        }
        else if (settingIndex == NUMBER_HIGHEST_PEAK_FREQUENCIES)
        {
            int val = projectManager->getProjectSettingsParameter(NUMBER_HIGHEST_PEAK_FREQUENCIES);
            
            String valString(val); textEditor_NumHighestPeakFreqs->setText(valString);
        }
        else if (settingIndex == SHOW_HIGHEST_PEAK_OCTAVES)
        {
            bool b = projectManager->getProjectSettingsParameter(SHOW_HIGHEST_PEAK_OCTAVES);
            button_HighestPeakOctaveBool->setToggleState(b, dontSendNotification);
        }
        else if (settingIndex == NUMBER_HIGHEST_PEAK_OCTAVES)
        {
            int val = projectManager->getProjectSettingsParameter(NUMBER_HIGHEST_PEAK_OCTAVES);
            
            String valString(val); textEditor_NumHighestPeakOctaves->setText(valString);
        }
        else if (settingIndex == FFT_DELAY_MS)
        {
            int val = projectManager->getProjectSettingsParameter(FFT_DELAY_MS);
            
            String valString(val); textEditor_FFTDelay->setText(valString);
        }
        else if (settingIndex == FFT_COLOR_SPEC_MAIN)
        {
            uint32 col = projectManager->getSettingsColorParameter(0);
            colourSelectorFFTMain->setColour(col);
        }
        else if (settingIndex == FFT_COLOR_SPEC_SEC)
        {
            uint32 col = projectManager->getSettingsColorParameter(1);
            colourSelectorFFTSec->setColour(col);
        }
        
        else if (settingIndex == FFT_COLOR_OCTAVE_MAIN)
        {
            uint32 col = projectManager->getSettingsColorParameter(2);
            colourSelectorOctaveMain->setColour(col);
        }
        else if (settingIndex == FFT_COLOR_OCTAVE_SEC)
        {
            uint32 col = projectManager->getSettingsColorParameter(3);
            colourSelectorOctaveSec->setColour(col);
        }
        
        else if (settingIndex == FFT_COLOR_COLOR_MAIN)
        {
            uint32 col = projectManager->getSettingsColorParameter(4);
            colourSelectorColourMain->setColour(col);
        }
        else if (settingIndex == FFT_COLOR_COLOR_SEC)
        {
            uint32 col = projectManager->getSettingsColorParameter(5);
            colourSelectorColourSec->setColour(col);
        }
        
        else if (settingIndex == FFT_COLOR_3D_MAIN)
        {
            uint32 col = projectManager->getSettingsColorParameter(6);
            colourSelector3DMain->setColour(col);
        }
        else if (settingIndex == FFT_COLOR_3D_SEC)
        {
            uint32 col = projectManager->getSettingsColorParameter(7);
            colourSelector3DSec->setColour(col);
        }
        
        else if (settingIndex == FFT_COLOR_FREQUENCY_MAIN)
        {
            uint32 col = projectManager->getSettingsColorParameter(8);
            colourSelectorFrequencyMain->setColour(col);
        }
        else if (settingIndex == FFT_COLOR_FREQUENCY_SEC)
        {
            uint32 col = projectManager->getSettingsColorParameter(9);
            colourSelectorFrequencySec->setColour(col);
        }
        
        else if (settingIndex == LISSAJOUS_COLOR_MAIN)
        {
            uint32 col = projectManager->getSettingsColorParameter(10);
            colourSelectorLissajousMain->setColour(col);
        }
        else if (settingIndex == LISSAJOUS_COLOR_SEC)
        {
            uint32 col = projectManager->getSettingsColorParameter(11);
            colourSelectorLissajousSec->setColour(col);
        }
    }
    
    // listener override
    void updateSettingsUIParameter(int settingIndex)override
    {
        updateUI(settingIndex);
    }
    
    float scaleFactor = 0.5;
    void setScale(float factor)
    {
        scaleFactor = factor;
        lookAndFeel.setScale(scaleFactor);
    }

private:
    
    CustomLookAndFeel lookAndFeel;
    
    ProjectManager * projectManager;
    
    Image imageBackground;
    
    std::unique_ptr<ImageButton> button_Add;
    std::unique_ptr<ImageButton> button_Close;
    
    std::unique_ptr<ImageButton>  button_HighestPeakFreqBool;
    std::unique_ptr<ImageButton>  button_HighestPeakOctaveBool;
    
    std::unique_ptr<TextEditor> textEditor_FFTDelay;
    std::unique_ptr<TextEditor>   textEditor_NumHighestPeakFreqs;
    std::unique_ptr<TextEditor>   textEditor_NumHighestPeakOctaves;
    
    
    std::unique_ptr<ComboBox> comboBox_FFTSize;
    std::unique_ptr<ComboBox> comboBox_FFTWindow;
    
    Image imageAddButton;
    Image imageCloseButton;
    Image imageBlueCheckButtonNormal;
    Image imageBlueCheckButtonSelected;
    Image imageBlueButtonNormal;
    Image imageBlueButtonSelected;
    
    RoundedColourOutputComponentWithSelector *colourSelectorFFTMain;
    RoundedColourOutputComponentWithSelector *colourSelectorFFTSec;
    
    RoundedColourOutputComponentWithSelector *colourSelectorOctaveMain;
    RoundedColourOutputComponentWithSelector *colourSelectorOctaveSec;
    
    RoundedColourOutputComponentWithSelector *colourSelectorColourMain;
    RoundedColourOutputComponentWithSelector *colourSelectorColourSec;
    
    RoundedColourOutputComponentWithSelector *colourSelector3DMain;
    RoundedColourOutputComponentWithSelector *colourSelector3DSec;
    
    RoundedColourOutputComponentWithSelector *colourSelectorFrequencyMain;
    RoundedColourOutputComponentWithSelector *colourSelectorFrequencySec;
    
    RoundedColourOutputComponentWithSelector *colourSelectorLissajousMain;
    RoundedColourOutputComponentWithSelector *colourSelectorLissajousSec;
    
    
    
};


class SettingsComponent :
    public MenuViewInterface,
    public TextEditor::Listener,
    public Button::Listener,
    public ProjectManager::UIListener,
    public ComboBox::Listener
{
public:
    SettingsComponent(ProjectManager * pm);
    ~SettingsComponent();

    void paint (Graphics&g) override;
    void resized() override;
    
    // Text editor listener overrides
    void textEditorTextChanged (TextEditor&editor)override;
    void textEditorReturnKeyPressed (TextEditor&editor)override;
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged)override;
    void buttonClicked (Button*button) override;
    
    void updateUI(int settingIndex);
    
    // listener override
    void updateSettingsUIParameter(int settingIndex)override;
    
    float scaleFactor = 0.5;
    void setScale(float factor) override
    {
        scaleFactor = factor;
        lookAndFeel.setScale(scaleFactor);
        component_AudioSettings->setScale(scaleFactor);
        component_AudioSettings->resized();
    }

private:
    
    CustomLookAndFeel lookAndFeel;
    
    ProjectManager * projectManager;
    
    SpectrogramSettingsPopup * spectrogramPopupComponent;
    
    AudioDeviceSelectorComponent * audioSetupComponent;
    std::unique_ptr<ImageButton> textButton_OpenAudioSettings;
    
    class AudioSettingsContainer : public Component, public Button::Listener
    { public: // simple container class to open and close the audio settings
        AudioSettingsContainer(){
            mainBackgroundImage         = ImageCache::getFromMemory(BinaryData::AudioSettingsBackground_png, BinaryData::AudioSettingsBackground_pngSize); imageCloseButton            = ImageCache::getFromMemory(BinaryData::CloseButton2x_png, BinaryData::CloseButton2x_pngSize);
            
            button_Close = std::make_unique<ImageButton>(); button_Close->setTriggeredOnMouseDown(true);
            button_Close->setImages (false, true, true,
                                     imageCloseButton, 0.999f, Colour (0x00000000),
                                     Image(), 1.000f, Colour (0x00000000),
                                     imageCloseButton, 0.75, Colour (0x00000000));
            button_Close->addListener(this); button_Close->setBounds(1407, 130, 150, 28);
            addAndMakeVisible(button_Close.get());
        }
        ~AudioSettingsContainer(){}
        
        void paint (Graphics&g) override
        {
            g.setColour(Colours::black);
            g.setOpacity(0.88);
            g.fillAll(); g.setOpacity(1.0);
            g.drawImage(mainBackgroundImage, 24 * scaleFactor, 94 * scaleFactor, 1502*scaleFactor, 1242*scaleFactor, 0, 0, 1502, 1242);
            
        }
        
        void resized() override
        {
            button_Close->setBounds(1407 * scaleFactor, 130 * scaleFactor, 150 * scaleFactor, 28 * scaleFactor);
        }
        void buttonClicked (Button*button) override { setVisible(false); }
        void setOpen(bool shouldOpen) { setVisible(true); }
        
        float scaleFactor = 0.5;
        void setScale(float factor) { scaleFactor = factor; }
        
        
    private:
        std::unique_ptr<ImageButton> button_Close; Image mainBackgroundImage; Image imageCloseButton;
    };
    
    AudioSettingsContainer* component_AudioSettings;

    Image imageBackground;
    
    std::unique_ptr<TextEditor> textEditor_AmplitudeMin;
    std::unique_ptr<TextEditor> textEditor_AttackMin;
    std::unique_ptr<TextEditor> textEditor_DecayMin;
    std::unique_ptr<TextEditor> textEditor_SustainMin;
    std::unique_ptr<TextEditor> textEditor_ReleaseMin;
    
    std::unique_ptr<TextEditor> textEditor_AmplitudeMax;
    std::unique_ptr<TextEditor> textEditor_AttackMax;
    std::unique_ptr<TextEditor> textEditor_DecayMax;
    std::unique_ptr<TextEditor> textEditor_SustainMax;
    std::unique_ptr<TextEditor> textEditor_ReleaseMax;
    
    std::unique_ptr<Label>  label_LogFileLocation;
    std::unique_ptr<Label>  label_RecordFileLocation;
    
    std::unique_ptr<ImageButton>  button_LogFileLocation;
    std::unique_ptr<ImageButton>  button_RecordFileLocation;
    
    std::unique_ptr<TextEditor> textEditor_NotesFrequency;
    std::unique_ptr<Label> label_B_Frequency;
    std::unique_ptr<Label> label_C_Frequency;
    std::unique_ptr<Label> label_D_Frequency;
    std::unique_ptr<Label> label_E_Frequency;
    std::unique_ptr<Label> label_F_Frequency;
    std::unique_ptr<Label> label_G_Frequency;
    
    std::unique_ptr<ImageButton> textButton_ResetAmp;
    std::unique_ptr<ImageButton> textButton_ResetAttack;
    std::unique_ptr<ImageButton> textButton_ResetDecay;
    std::unique_ptr<ImageButton> textButton_ResetSustain;
    std::unique_ptr<ImageButton> textButton_ResetRelease;
    
    std::unique_ptr<ImageButton> textButton_ChangeLogFile;
    std::unique_ptr<ImageButton> textButton_ChangeRecordFile;
    
    std::unique_ptr<ImageButton> textButton_SpectrogramSetttingsPopup;
    
    std::unique_ptr<ImageButton> buttonScale25;
    std::unique_ptr<ImageButton> buttonScale50;
    std::unique_ptr<ImageButton> buttonScale75;
    std::unique_ptr<ImageButton> buttonScale100;
    
    std::unique_ptr<TextButton> buttonMixer;
    
    Image imageButtonScale25Normal;
    Image imageButtonScale25Selected;
    
    Image imageButtonScale50Normal;
    Image imageButtonScale50Selected;
    
    Image imageButtonScale75Normal;
    Image imageButtonScale75Selected;
    
    Image imageButtonScale100Normal;
    Image imageButtonScale100Selected;
    
    Image imageButtonSave;
    Image imageButtonLoad;
    Image imageButtonAudio;
    Image imageButtonChange;
    Image imageButtonReset;
    Image imageButtonScalesAdd;
    Image imageButtonSpectrogram;
    Image imageButtonScan;
    
    std::unique_ptr<ComboBox> comboBox_Scales;
    std::unique_ptr<ComboBox> comboBox_GUIScale;
    std::unique_ptr<ComboBox> comboBox_RecordFormat;
    
    std::unique_ptr<ComboBox> comboBox_NoiseType;
    
//    Label * fftWindowLabel;
//    Label * fftLabel;
//    Label * scalesLabel;
//    Label * guiScaleLabel;
//    
//    Label * noiseTypeLabel;
    
    
    // Plugin Slot code
    
    void rescanPlugins();
    std::unique_ptr<ImageButton>   scanPluginsButton;
    
    Image imageAddPluginIcon;
    Image removePluginIcon;
    Image openWindowPluginIcon;
    
    Image imageBlueButtonNormal;
    Image imageBlueButtonSelected;
    Image imageAddButton;
    Image imageBlueCheckButtonNormal;
    Image imageBlueCheckButtonSelected;
    Image imagePluginPlayButton;
    
    Label *         labelPluginSlot[NUM_PLUGIN_SLOTS];
    ImageButton *   buttonAddPlugin[NUM_PLUGIN_SLOTS];
    ImageButton *   buttonRemovePlugin[NUM_PLUGIN_SLOTS];
    ImageButton *   buttonOpenPlugin[NUM_PLUGIN_SLOTS];
    ComboBox *      comboBoxPluginSelector[NUM_PLUGIN_SLOTS];
    
    std::unique_ptr<ImageButton>   button_Save;
    std::unique_ptr<ImageButton>   button_Load;
    
    std::unique_ptr<ImageButton>  button_FreqToChordMainHarmonics;
    std::unique_ptr<ImageButton>  button_FreqToChordAverage;
    std::unique_ptr<ImageButton>  button_FreqToChordEMA;

    ComboBox fundamentalFrequencyAlgorithmChooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettingsComponent)
};
