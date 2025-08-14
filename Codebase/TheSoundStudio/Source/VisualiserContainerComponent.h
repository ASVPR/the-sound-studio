/*
  ==============================================================================

    VisualiserContainerComponent.h
    Created: 23 Feb 2020 10:39:48am
    Author:  Gary Jones

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "ProjectManager.h"
#include "HighQualityMeter.h"
#include "PopupFFTWindow.h"
#include "CustomLookAndFeel.h"
#include "SpectrogramComponent.h"
//#include "OscilloscopeVisualiserComponent.h"
#include "LissajousCurveComponent.h"
#include <memory>

// SVP 190
// adapt this class to be variably selected,
// need single popdown, top left corner, selects type of visual unit
// button top right, to popout..

class VisualiserSelectorComponent : public Component, public Button::Listener, public ComboBox::Listener, public ProjectManager::UIListener
{
public:
    
    enum VISUALISER_TYPE
    {
        SPECTROGRAM = 1,
        COLOUR,
        OCTAVE,
        OSCILLOSCOPE,
        LISSAJOUS,
        FREQ_DATA,
        TOTAL_NUM_TYPES
    }visualiserType;
    
    VISUALISER_SOURCE   visualiserSource;
    
    VisualiserSelectorComponent(ProjectManager * pm, VISUALISER_TYPE initialType, AUDIO_MODE mode)
    {
        audioMode       = mode;
        
        projectManager  = pm;
        projectManager  ->addUIListener(this);
        
        shouldUpdate    = false;
        
        // fonts
        Typeface::Ptr AssistantBold         = Typeface::createSystemTypefaceFor(BinaryData::AssistantBold_ttf, BinaryData::AssistantBold_ttfSize);
        Typeface::Ptr AssistantSemiBold     = Typeface::createSystemTypefaceFor(BinaryData::AssistantSemiBold_ttf, BinaryData::AssistantSemiBold_ttfSize);
        Font fontBold(AssistantBold);
        Font fontNormal(AssistantSemiBold);
        
        fontNormal.setHeight(33);
        fontBold.setHeight(33);

        imageButtonWindows                  = ImageCache::getFromMemory(BinaryData::Button_SpectrumOpen_png, BinaryData::Button_SpectrumOpen_pngSize);
        
        button_Popup = new ImageButton();
        button_Popup->setTriggeredOnMouseDown(true);
        button_Popup->setImages (false, true, true,
                                 imageButtonWindows, 0.999f, Colour (0x00000000),
                                 Image(), 1.000f, Colour (0x00000000),
                                 imageButtonWindows, 0.75, Colour (0x00000000));
        button_Popup->addListener(this);
        addAndMakeVisible(button_Popup);
        
        buttonShouldProcess = new ToggleButton("On / Off");
        buttonShouldProcess->setLookAndFeel(&lookAndFeel);
        buttonShouldProcess->addListener(this);
        buttonShouldProcess->setToggleState(true, dontSendNotification);
        addAndMakeVisible(buttonShouldProcess);
        
        comboBoxTypeSelector = new ComboBox();
        comboBoxTypeSelector->addListener(this);
        comboBoxTypeSelector->setLookAndFeel(&lookAndFeel);
        
        for (int i = 0; i < TOTAL_NUM_TYPES; i++)
        {
            switch (i)
            {
                case SPECTROGRAM:   comboBoxTypeSelector->addItem("Spectrogram", i); break;
                case COLOUR:        comboBoxTypeSelector->addItem("Colour", i); break;
                case OCTAVE:        comboBoxTypeSelector->addItem("Octave", i); break;
                case OSCILLOSCOPE:  comboBoxTypeSelector->addItem("Oscilloscope", i); break;
                case LISSAJOUS:     comboBoxTypeSelector->addItem("Lissajous", i); break;
                case FREQ_DATA:     comboBoxTypeSelector->addItem("Data", i); break;
            }
        }
        comboBoxTypeSelector->setSelectedId((int)initialType, dontSendNotification);
        addAndMakeVisible(comboBoxTypeSelector);

        visualiserType = (VISUALISER_TYPE)0;
        
        
        comboBoxSourceSelector = new ComboBox();
        comboBoxSourceSelector->addListener(this);
        comboBoxSourceSelector->setLookAndFeel(&lookAndFeel);
        
        getAvailableSourcesForSourceComboBox();

        addAndMakeVisible(comboBoxSourceSelector);

        visualiserSource = (VISUALISER_SOURCE)OUTPUT_1;

    }
    
    ~VisualiserSelectorComponent()
    {
        buttonShouldProcess->setLookAndFeel(nullptr);
        comboBoxTypeSelector->setLookAndFeel(nullptr);
        comboBoxSourceSelector->setLookAndFeel(nullptr);
    }
    

    void getAvailableSourcesForSourceComboBox()
    {
        //
        comboBoxSourceSelector->clear();
        
        for (int input = 0; input < 4; input++)
        {
            bool isAvailable = (bool)projectManager->getProjectSettingsParameter(MIXER_INPUT_FFT_1 + input);
            
            if (isAvailable)
            {
                String itemString("Input ");
                String index(input+1); itemString.append(index, 1);
                
                comboBoxSourceSelector->addItem(itemString, input + 1);
            }
        }
        
        for (int output = 0; output < 4; output++)
        {
            bool isAvailable = (bool)projectManager->getProjectSettingsParameter(MIXER_OUTPUT_FFT_1+output);
            
            if (isAvailable)
            {
                String itemString("Output ");
                String index(output+1); itemString.append(index, 1);
                
                comboBoxSourceSelector->addItem(itemString, output+4+1);
            }
        }
    }
    
    void resized() override
    {
        float comboBoxL         = 0 * scaleFactor;
        float comboBoxW         = 200 * scaleFactor;
        float comboBoxH         = 38 * scaleFactor;
        
        float topSectionH       = 42 * scaleFactor;
        float labelL            = comboBoxL + comboBoxW + (20 * scaleFactor);
        float labelW            = 300 * scaleFactor;
        float buttonL           = getWidth() - topSectionH;
        
        float comboBoxSourceL   = buttonL - comboBoxW - (20 * scaleFactor);
        
        comboBoxTypeSelector    ->setBounds(comboBoxL, 0, comboBoxW, comboBoxH);
        comboBoxSourceSelector  ->setBounds(comboBoxSourceL, 0, comboBoxW, comboBoxH);
//        labelVisualiserType     ->setBounds(labelL, 0, labelW, topSectionH);
        button_Popup            ->setBounds(buttonL, 0, topSectionH, topSectionH);
        buttonShouldProcess     ->setBounds(labelL, 0, labelW, topSectionH);
        
        Rectangle<float> spectrumRect(getWidth(), getHeight() - topSectionH);
        
        switch ((int)visualiserType)
        {
            case SPECTROGRAM:
            {
                spectrogramComponent->setBounds(0, (42  * scaleFactor), spectrumRect.getWidth(), spectrumRect.getHeight());
                spectrogramComponent->setScale(scaleFactor);
            }
                break;
            case COLOUR:
            {

                colourComponent->setBounds(0, (42  * scaleFactor), spectrumRect.getWidth(), spectrumRect.getHeight());
                colourComponent->setScale(scaleFactor);
            }
                break;
            case OCTAVE:
            {
                octaveComponent->setBounds(0, (42  * scaleFactor), spectrumRect.getWidth(), spectrumRect.getHeight());
                octaveComponent->setScale(scaleFactor);
            }
                break;
            case OSCILLOSCOPE:
            {
                oscilloscopeComponent->setBounds(0, (42  * scaleFactor), spectrumRect.getWidth(), spectrumRect.getHeight());
                oscilloscopeComponent->setScale(scaleFactor);
            }
                break;
            case LISSAJOUS:
            {
                lissajousComponent->setBounds(0, (42  * scaleFactor), spectrumRect.getWidth(), spectrumRect.getHeight());
                lissajousComponent->setScale(scaleFactor);
            }
                break;
            case FREQ_DATA:
            {
                frequencyDataComponent->setBounds(0, (42  * scaleFactor), spectrumRect.getWidth(), spectrumRect.getHeight());
                frequencyDataComponent->setScale(scaleFactor);
            }
                break;
        }
    }
    
    void buttonClicked (Button*button) override
    {
        if (button == button_Popup)
        {
            // open popup
            switch ((int)visualiserType)
            {
                case SPECTROGRAM:
                {
                    if(popupFFTWindow != nullptr) delete popupFFTWindow; popupFFTWindow = nullptr;
                    
                    Rectangle<float> spectrumRect(getWidth(), getHeight());
                    
                    SpectrogramComponent * comp = new SpectrogramComponent(projectManager, spectrumRect, true);
                    comp->setBounds(0,0, fftWidth* scaleFactor, fftHeight* scaleFactor);
                    comp->setScale(scaleFactor);
                    comp->visualiserSource = visualiserSource;
                    
                    popupFFTWindow = new PopupFFTWindow("Spectrogram", comp, Colours::black, DocumentWindow::allButtons, true);
                    popupFFTWindow ->centreWithSize(comp->getWidth(), comp->getHeight());
                    popupFFTWindow ->setResizable(true, true);
                    popupFFTWindow ->setVisible(true);
                    
                    comp->setShouldUpdate(true);

                }
                    break;
    
                case COLOUR:
                {
                    if(popupFFTWindow != nullptr) delete popupFFTWindow; popupFFTWindow = nullptr;
                    
                    Rectangle<float> spectrumRect(getWidth(), getHeight());
                    
                    ColourSpectrumVisualiserComponent * comp  = new ColourSpectrumVisualiserComponent(projectManager, spectrumRect.getWidth(), spectrumRect.getHeight());
                    comp->setBounds(0,0, spectrumRect.getWidth(), spectrumRect.getHeight());
                    comp->setScale(scaleFactor);
                    comp->visualiserSource = visualiserSource;
                    
                    popupFFTWindow = new PopupFFTWindow("Colour Spectrogram", comp, Colours::black, DocumentWindow::allButtons, true);
                    popupFFTWindow ->centreWithSize(comp->getWidth(), comp->getHeight());
                    popupFFTWindow ->setResizable(true, true);
                    popupFFTWindow ->setVisible(true);
                    
                    comp->setShouldUpdate(true);
                    
                } break;
                case OCTAVE:
                {
                    if(popupFFTWindow != nullptr) delete popupFFTWindow; popupFFTWindow = nullptr;
                    
                    Rectangle<float> spectrumRect(getWidth(), getHeight());
                    
                    OctaveVisualiserComponent2 * comp = new OctaveVisualiserComponent2(projectManager);
                    comp->setBounds(0,0, spectrumRect.getWidth(), spectrumRect.getHeight());
                    comp->setScale(scaleFactor);
                    comp->visualiserSource = visualiserSource;
                    
                    popupFFTWindow = new PopupFFTWindow("Octave Analyzer", comp, Colours::black, DocumentWindow::allButtons, true);
                    popupFFTWindow ->centreWithSize(comp->getWidth(), comp->getHeight());
                    popupFFTWindow ->setResizable(true, true);
                    popupFFTWindow ->setVisible(true);
                    
                    comp->setShouldUpdate(true);
                    
                } break;
                case OSCILLOSCOPE:
                {
                    if(popupFFTWindow != nullptr) delete popupFFTWindow; popupFFTWindow = nullptr;
                    
                    Rectangle<float> spectrumRect(getWidth(), getHeight());
                    
                    OscilloscopeComponent * comp = new OscilloscopeComponent(projectManager);
                    comp->setBounds(0,0, spectrumRect.getWidth(), spectrumRect.getHeight());
                    comp->setScale(scaleFactor);
                    comp->visualiserSource = visualiserSource;
                    
                    popupFFTWindow = new PopupFFTWindow("Oscilloscope", comp, Colours::black, DocumentWindow::allButtons, true);
                    popupFFTWindow ->centreWithSize(comp->getWidth(), comp->getHeight());
                    popupFFTWindow ->setResizable(true, true);
                    popupFFTWindow ->setVisible(true);
                    
                    comp->setShouldUpdate(true);
                } break;
                case LISSAJOUS:
                {
                    if(popupFFTWindow != nullptr) delete popupFFTWindow; popupFFTWindow = nullptr;
                    
                    Rectangle<float> spectrumRect(getWidth(), getHeight());
                    
                    LissajousCurveViewerComponent * comp = new LissajousCurveViewerComponent(projectManager, false);
                    comp->setBounds(0,0, spectrumRect.getWidth(), spectrumRect.getHeight());
                    comp->setScale(scaleFactor);
                    comp->visualiserSource = visualiserSource;
                    
                    popupFFTWindow = new PopupFFTWindow("Lissajous", comp, Colours::black, DocumentWindow::allButtons, true);
                    popupFFTWindow ->centreWithSize(comp->getWidth(), comp->getHeight());
                    popupFFTWindow ->setResizable(true, true);
                    popupFFTWindow ->setVisible(true);
                    
                    comp->setShouldUpdate(true);
    
                } break;
                case FREQ_DATA:
                {
                    if(popupFFTWindow != nullptr) delete popupFFTWindow; popupFFTWindow = nullptr;
                    
                    Rectangle<float> spectrumRect(getWidth(), getHeight());
                    
                    FrequencyDataComponent * comp = new FrequencyDataComponent(projectManager);
                    comp->setBounds(0,0, spectrumRect.getWidth(), spectrumRect.getHeight());
                    comp->setScale(scaleFactor);
                    comp->visualiserSource = visualiserSource;
                    
                    popupFFTWindow = new PopupFFTWindow("Frequency Data", comp, Colours::black, DocumentWindow::allButtons, true);
                    popupFFTWindow ->centreWithSize(comp->getWidth(), comp->getHeight());
                    popupFFTWindow ->setResizable(true, true);
                    popupFFTWindow ->setVisible(true);
                    
                    comp->setShouldUpdate(true);
                } break;
            }
        }
        else if (button == buttonShouldProcess)
        {
            switch ((int)visualiserType)
            {
                case SPECTROGRAM:   spectrogramComponent->setShouldUpdate(buttonShouldProcess->getToggleState()); break;
                case COLOUR:        colourComponent->setShouldUpdate(buttonShouldProcess->getToggleState()); break;
                case OCTAVE:        octaveComponent->setShouldUpdate(buttonShouldProcess->getToggleState()); break;
                case OSCILLOSCOPE:  oscilloscopeComponent->setShouldUpdate(buttonShouldProcess->getToggleState()); break;
                case LISSAJOUS:     lissajousComponent->setShouldUpdate(buttonShouldProcess->getToggleState()); break;
                case FREQ_DATA:     frequencyDataComponent->setShouldUpdate(buttonShouldProcess->getToggleState()); break;
            }
        }
    }
    
    void setProcessingActive(bool isActive)
    {
        buttonShouldProcess->setToggleState(isActive, sendNotification);
    }
    
    
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged) override
    {
        if (comboBoxThatHasChanged == comboBoxSourceSelector)
        {
            setNewVisualiserSource((VISUALISER_SOURCE)comboBoxThatHasChanged->getSelectedId());
        }
        else if (comboBoxThatHasChanged == comboBoxTypeSelector)
        {
            setNewVisualiserType((VISUALISER_TYPE)comboBoxThatHasChanged->getSelectedId());
        }
        
    }
    
    void setNewVisualiserSource(VISUALISER_SOURCE source)
    {
        visualiserSource = source;
        
        setNewVisualiserType(visualiserType);
    }

    void setNewVisualiserType(VISUALISER_TYPE type)
    {
        shouldUpdate = false;
        
//        stopTimer();
        
        // remove current type
        switch ((int)visualiserType)
        {
            case SPECTROGRAM:   if (spectrogramComponent != nullptr) delete spectrogramComponent; spectrogramComponent = nullptr; break;
            case COLOUR:        if (colourComponent != nullptr) delete colourComponent; colourComponent = nullptr; break;
            case OCTAVE:        if (octaveComponent != nullptr) delete octaveComponent; octaveComponent = nullptr; break;
            case OSCILLOSCOPE:  if (oscilloscopeComponent != nullptr) delete oscilloscopeComponent; oscilloscopeComponent = nullptr; break;
            case LISSAJOUS:     if (lissajousComponent != nullptr) delete lissajousComponent; lissajousComponent = nullptr; break;
            case FREQ_DATA:     if (frequencyDataComponent != nullptr) delete frequencyDataComponent; frequencyDataComponent = nullptr; break;
        }
        
    
        visualiserType = type;
        
        // update component..
//        Rectangle<float> spectrumRect(fftWidth * scaleFactor, fftHeight * scaleFactor);
        Rectangle<float> spectrumRect(getWidth(), getHeight() - (42  * scaleFactor));
        
        switch ((int)visualiserType)
        {
            case SPECTROGRAM:
            {
                spectrogramComponent = new SpectrogramComponent(projectManager, spectrumRect, false);
                spectrogramComponent->setBounds(0, (42  * scaleFactor), spectrumRect.getWidth(), spectrumRect.getHeight());
                addAndMakeVisible(spectrogramComponent);
                
                spectrogramComponent->visualiserSource = visualiserSource;
                
                spectrogramComponent->setShouldUpdate(buttonShouldProcess->getToggleState());
                spectrogramComponent->setScale(scaleFactor);
            }
                break;
            case COLOUR:
            {
                colourComponent = new ColourSpectrumVisualiserComponent(projectManager, spectrumRect.getWidth(), spectrumRect.getHeight());
                colourComponent->setBounds(0, (42  * scaleFactor), spectrumRect.getWidth(), spectrumRect.getHeight());
                addAndMakeVisible(colourComponent);
                
                colourComponent->visualiserSource = visualiserSource;
                
                colourComponent->setShouldUpdate(buttonShouldProcess->getToggleState());
                colourComponent->setScale(scaleFactor);
            }
                break;
            case OCTAVE:
            {
                octaveComponent = new OctaveVisualiserComponent2(projectManager);
                octaveComponent->setBounds(0, (42  * scaleFactor), spectrumRect.getWidth(), spectrumRect.getHeight());
                addAndMakeVisible(octaveComponent);
                
                octaveComponent->visualiserSource = visualiserSource;
                
                octaveComponent->setShouldUpdate(buttonShouldProcess->getToggleState());
                octaveComponent->setScale(scaleFactor);
            }
                break;
            case OSCILLOSCOPE:
            {
                oscilloscopeComponent = new OscilloscopeComponent(projectManager);
                oscilloscopeComponent->setBounds(0, (42  * scaleFactor), spectrumRect.getWidth(), spectrumRect.getHeight());
                addAndMakeVisible(oscilloscopeComponent);
                
                oscilloscopeComponent->visualiserSource = visualiserSource;
                
                oscilloscopeComponent->setShouldUpdate(buttonShouldProcess->getToggleState());
                oscilloscopeComponent->setScale(scaleFactor);
            }
                break;
            case LISSAJOUS:
            {
                lissajousComponent = new LissajousCurveViewerComponent(projectManager, false);
                lissajousComponent->setBounds(0, (42  * scaleFactor), spectrumRect.getWidth(), spectrumRect.getHeight());
                addAndMakeVisible(lissajousComponent);
                
                lissajousComponent->visualiserSource = visualiserSource;
                
                lissajousComponent->setShouldUpdate(buttonShouldProcess->getToggleState());
                lissajousComponent->setScale(scaleFactor);
            }
                break;
            case FREQ_DATA:
            {
                frequencyDataComponent = new FrequencyDataComponent(projectManager);
                frequencyDataComponent->setBounds(0, (42  * scaleFactor), spectrumRect.getWidth(), spectrumRect.getHeight());
                addAndMakeVisible(frequencyDataComponent);
                
                frequencyDataComponent->visualiserSource = visualiserSource;
                
                frequencyDataComponent->setShouldUpdate(buttonShouldProcess->getToggleState());
                frequencyDataComponent->setScale(scaleFactor);
            }
                break;
        }
        
        shouldUpdate = true;
        
        
    }
    
    virtual void updateSettingsUIParameter(int param) override
    {
        // probably redundant
        
        if (param >= MIXER_INPUT_FFT_1 && param <= MIXER_OUTPUT_FFT_4)
        {
            getAvailableSourcesForSourceComboBox();
        }
    }
    
    virtual void freezeFFTProcessing(bool should) override
    {
        shouldFreeze = should;
    }
    
    virtual void updateAudioMode(AUDIO_MODE mode) override
    {
        if (audioMode == mode)
        {
            shouldUpdate = true;
        }
        else
        {
            shouldUpdate = false;
        }
    }
    
    float scaleFactor = 0.5;
    void setScale(float factor)
    {
        scaleFactor = factor;
        
        lookAndFeel.setScale(scaleFactor);
            
        setNewVisualiserType(visualiserType);
    }

    void showInputMenu(bool show)
    {
        comboBoxSourceSelector->setVisible(show);
    }
    
private:
    double sampleRate;
    bool shouldFreeze;
    
    AUDIO_MODE audioMode;
    bool shouldUpdate; // boolean if active, and audioMode is correct, will start Timer, else no timer... time should stop when switching vis types..
    
    CustomLookAndFeel lookAndFeel;
    
    ProjectManager * projectManager;
    
    std::unique_ptr<ComboBox> comboBoxTypeSelector;
    std::unique_ptr<ComboBox> comboBoxSourceSelector; // select input or output ref
    std::unique_ptr<ImageButton> buttonPopup;
    std::unique_ptr<Label> labelVisualiserType;
    
    SpectrogramComponent *              spectrogramComponent = nullptr;
    OctaveVisualiserComponent2 *        octaveComponent = nullptr;
    ColourSpectrumVisualiserComponent*  colourComponent = nullptr;
    FrequencyDataComponent*             frequencyDataComponent = nullptr;
    OscilloscopeComponent*              oscilloscopeComponent = nullptr;
    LissajousCurveViewerComponent*      lissajousComponent = nullptr;
    
    bool popupsAreVisible = false;
    PopupFFTWindow * popupFFTWindow = nullptr; // reinitialise whenever called..
    
    
    Image imageButtonWindows;
    std::unique_ptr<ImageButton> button_Popup;
    
    ToggleButton * buttonShouldProcess;
    
    int mainContainerHeight = 1096;
    int fftLeftMargin       = 48;
    int fftTopMargin        = 64;
    int fftWidth            = 1476;
    int fftShortWidth       = 1100;
    int fftHeight           = 310;
    int fftShortHeight      = 254;
    
    int colorLeftMargin     = fftLeftMargin;
    int colorTopMargin      = fftTopMargin + fftHeight + 67;
    int colorWidth          = 742;
    int colorHeight         = 271;
    
    int octaveLeftMergin    = 822;
    int octaveTopMargin     = colorTopMargin;
    int octaveWidth         = 699;
    int octaveHeight        = colorHeight;
    

    
};

class VisualiserContainerComponent2 : public Component, public ProjectManager::UIListener
{
public:
    
    AUDIO_MODE audioMode; // set on init to determine timer callbacks

    
    VisualiserContainerComponent2(ProjectManager * pm, AUDIO_MODE newMode)
    {
        projectManager  = pm;
        audioMode       = newMode;
        
        if (audioMode == AUDIO_MODE::MODE_REALTIME_ANALYSIS)
        {
            numVisualisers  = 4;
            
            Rectangle<float> spectrumRect(fftWidth, fftHeight);

            visualiserSelectorComponent[0] = new VisualiserSelectorComponent(projectManager, VisualiserSelectorComponent::VISUALISER_TYPE::SPECTROGRAM, audioMode);
            addAndMakeVisible(visualiserSelectorComponent[0]);
            visualiserSelectorComponent[0]->setBounds(fftLeftMargin * scaleFactor, fftTopMargin * scaleFactor, fftWidth * scaleFactor, fftHeight * scaleFactor);
            visualiserSelectorComponent[0]->setNewVisualiserType(VisualiserSelectorComponent::VISUALISER_TYPE::SPECTROGRAM);
            
            visualiserSelectorComponent[1] = new VisualiserSelectorComponent(projectManager, VisualiserSelectorComponent::VISUALISER_TYPE::COLOUR, audioMode);
            addAndMakeVisible(visualiserSelectorComponent[1]);
            visualiserSelectorComponent[1]->setBounds(colorLeftMargin * scaleFactor, colorTopMargin * scaleFactor, fftWidth * scaleFactor, colorHeight * scaleFactor);
            visualiserSelectorComponent[1]->setNewVisualiserType(VisualiserSelectorComponent::VISUALISER_TYPE::COLOUR);
            
            visualiserSelectorComponent[2] = new VisualiserSelectorComponent(projectManager, VisualiserSelectorComponent::VISUALISER_TYPE::FREQ_DATA, audioMode);
            addAndMakeVisible(visualiserSelectorComponent[2]);
            visualiserSelectorComponent[2]->setBounds(colorLeftMargin * scaleFactor, (colorTopMargin + colorHeight + 40) * scaleFactor, colorWidth * scaleFactor , 350.f * scaleFactor);
            visualiserSelectorComponent[2]->setNewVisualiserType(VisualiserSelectorComponent::VISUALISER_TYPE::FREQ_DATA);
            
            visualiserSelectorComponent[3] = new VisualiserSelectorComponent(projectManager, VisualiserSelectorComponent::VISUALISER_TYPE::OSCILLOSCOPE, audioMode);
            addAndMakeVisible(visualiserSelectorComponent[3]);
            visualiserSelectorComponent[3]->setBounds(820 * scaleFactor, (colorTopMargin + colorHeight + 40) * scaleFactor, colorWidth * scaleFactor , 350.f * scaleFactor);
            visualiserSelectorComponent[3]->setNewVisualiserType(VisualiserSelectorComponent::VISUALISER_TYPE::OSCILLOSCOPE);
        }
        else if (audioMode == AUDIO_MODE::MODE_CHORD_PLAYER || audioMode == AUDIO_MODE::MODE_FREQUENCY_PLAYER)
        {
            numVisualisers  = 3;
            
            Rectangle<float> spectrumRect(fftWidth, fftHeight);
            
            visualiserSelectorComponent[0] = new VisualiserSelectorComponent(projectManager, VisualiserSelectorComponent::VISUALISER_TYPE::SPECTROGRAM, audioMode);
            addAndMakeVisible(visualiserSelectorComponent[0]);
            visualiserSelectorComponent[0]->setBounds(fftLeftMargin * scaleFactor, fftTopMargin * scaleFactor, fftWidth * scaleFactor, fftHeight * scaleFactor);
            visualiserSelectorComponent[0]->setNewVisualiserType(VisualiserSelectorComponent::VISUALISER_TYPE::SPECTROGRAM);
            
            visualiserSelectorComponent[1] = new VisualiserSelectorComponent(projectManager, VisualiserSelectorComponent::VISUALISER_TYPE::COLOUR, audioMode);
            addAndMakeVisible(visualiserSelectorComponent[1]);
            visualiserSelectorComponent[1]->setBounds(colorLeftMargin * scaleFactor, colorTopMargin * scaleFactor, colorWidth * scaleFactor, colorHeight * scaleFactor);
            visualiserSelectorComponent[1]->setNewVisualiserType(VisualiserSelectorComponent::VISUALISER_TYPE::COLOUR);
            
            visualiserSelectorComponent[2] = new VisualiserSelectorComponent(projectManager, VisualiserSelectorComponent::VISUALISER_TYPE::OCTAVE, audioMode);
            addAndMakeVisible(visualiserSelectorComponent[2]);
            visualiserSelectorComponent[2]->setBounds(octaveLeftMergin * scaleFactor, octaveTopMargin * scaleFactor, octaveWidth * scaleFactor, octaveHeight * scaleFactor);
            visualiserSelectorComponent[2]->setNewVisualiserType(VisualiserSelectorComponent::VISUALISER_TYPE::OCTAVE);
            
        }
        else if (audioMode == AUDIO_MODE::MODE_CHORD_SCANNER || audioMode == AUDIO_MODE::MODE_FREQUENCY_SCANNER)
        {
            numVisualisers  = 1;
            
            Rectangle<float> spectrumRect(fftWidthScanner, fftHeightScanner);

            visualiserSelectorComponent[0] = new VisualiserSelectorComponent(projectManager, VisualiserSelectorComponent::VISUALISER_TYPE::SPECTROGRAM, audioMode);
            addAndMakeVisible(visualiserSelectorComponent[0]);
            visualiserSelectorComponent[0]->setBounds(fftLeftMarginScanner * scaleFactor, fftTopMarginScanner * scaleFactor, fftWidthScanner * scaleFactor, fftHeightScanner * scaleFactor);
            visualiserSelectorComponent[0]->setNewVisualiserType(VisualiserSelectorComponent::VISUALISER_TYPE::SPECTROGRAM);
        }

    }
    
    ~VisualiserContainerComponent2() {}
    
    void resized() override
    {
        if (audioMode == AUDIO_MODE::MODE_REALTIME_ANALYSIS)
        {
            visualiserSelectorComponent[0]   ->setBounds(fftLeftMargin * scaleFactor, fftTopMargin * scaleFactor, fftWidth * scaleFactor, fftHeight * scaleFactor);
            visualiserSelectorComponent[1]   ->setBounds(colorLeftMargin * scaleFactor, colorTopMargin * scaleFactor, fftWidth * scaleFactor, colorHeight * scaleFactor);
            visualiserSelectorComponent[2]   ->setBounds(colorLeftMargin * scaleFactor, (colorTopMargin + colorHeight + 40) * scaleFactor, colorWidth * scaleFactor , 350.f * scaleFactor);
            visualiserSelectorComponent[3]   ->setBounds(820 * scaleFactor, (colorTopMargin + colorHeight + 40) * scaleFactor, colorWidth * scaleFactor , 350.f * scaleFactor);
        }
        else if (audioMode == AUDIO_MODE::MODE_CHORD_PLAYER || audioMode == AUDIO_MODE::MODE_FREQUENCY_PLAYER)
        {
            visualiserSelectorComponent[0]->setBounds(fftLeftMargin * scaleFactor, fftTopMargin * scaleFactor, fftWidth * scaleFactor, fftHeight * scaleFactor);
            visualiserSelectorComponent[1]->setBounds(colorLeftMargin * scaleFactor, colorTopMargin * scaleFactor, colorWidth * scaleFactor, colorHeight * scaleFactor);
            visualiserSelectorComponent[2]->setBounds(octaveLeftMergin * scaleFactor, octaveTopMargin * scaleFactor, octaveWidth * scaleFactor, octaveHeight * scaleFactor);
            
        }
        else if (audioMode == AUDIO_MODE::MODE_CHORD_SCANNER || audioMode == AUDIO_MODE::MODE_FREQUENCY_SCANNER)
        {
            visualiserSelectorComponent[0]->setBounds(fftLeftMarginScanner * scaleFactor, fftTopMarginScanner * scaleFactor, fftWidthScanner * scaleFactor, fftHeightScanner * scaleFactor);

        }
    }

    
    // Listener
    virtual void updateSettingsUIParameter(int param) override{}
    
    bool shouldFreeze = false;
    
    virtual void freezeFFTProcessing(bool should) override
    {
        shouldFreeze = should;
    }

    float scaleFactor = 0.5;
    void setScale(float factor)
    {
        scaleFactor = factor;
        
        for (int i = 0; i < numVisualisers; i++)
        {
            visualiserSelectorComponent[i]->setScale(scaleFactor);
        }
        
        lookAndFeel.setScale(scaleFactor);
    }
    
    int numVisualisers;
    
    void setPopupsAreVisible(bool shouldBeVisible)
    {
        
    }

private:
    ProjectManager * projectManager;

    float activeHeight;
    float activeWidth;
    
    CustomLookAndFeel lookAndFeel;


    std::unique_ptr<VisualiserSelectorComponent> visualiserSelectorComponent[4];
    

    // UI Layout Variables
    int mainContainerHeight = 1096;
    int fftLeftMargin       = 48;
    int fftTopMargin        = 30;
    int fftWidth            = 1476;
    int fftShortWidth       = 1100;
    int fftHeight           = 340;
    int fftShortHeight      = 254;
    
    int fftLeftMarginScanner       = 48;
    int fftTopMarginScanner        = 30;
    int fftWidthScanner            = 1100;
    int fftHeightScanner           = 280;
    
    
    int colorLeftMargin     = fftLeftMargin;
    int colorTopMargin      = fftTopMargin + fftHeight + 30;
    int colorWidth          = 742;
    int colorHeight         = 300;
    
    int octaveLeftMergin    = 822;
    int octaveTopMargin     = colorTopMargin;
    int octaveWidth         = 699;
    int octaveHeight        = colorHeight;
    
    
   
};

//
//
//class VisualiserContainerComponent : public Component,  public Button::Listener, public ProjectManager::UIListener
//{
//public:
//
//    AUDIO_MODE mode; // set on init to determine timer callbacks
//
//    VisualiserContainerComponent(ProjectManager * pm, AUDIO_MODE newMode);
//    ~VisualiserContainerComponent();
//    void paint (Graphics&g)override;
//    void resized() override;
//
//    void buttonClicked (Button*button)override;
//
//    void setPopupsAreVisible(bool shouldBeVisible);
//
//    void calculatePeakValues(float * fft);
//    void calculateVUMeters();
//    double linearToDb(float lin) { return 20.0 * log10(lin); }
//
//    // add reset peaks button.. otherwise keep highest value in labels..
//
//    void updateLabels();
//
//    // Listener
//    virtual void updateSettingsUIParameter(int param) override;
//
//    bool shouldFreeze = false;
//
//    virtual void freezeFFTProcessing(bool should) override
//    {
//        shouldFreeze = should;
//    }
//
//    float scaleFactor = 0.5;
//    void setScale(float factor);
//    void pushUpdate();
//
//
//private:
//    ProjectManager * projectManager;
//
//    float * fftData;
//    float * fftDataCopy;
//    int fftSize;
//    double sampleRate;
//
//
//    std::unique_ptr<Label> peakFrequencyLabel;
//    std::unique_ptr<Label> peakAmplitudeLabel;
//    std::unique_ptr<Label> peakMovingAvgLabel; // needs to move to Spectrum analyser
//
//    float peakFrequencyValue;
//    float peakAmplitudeValue;
//
//    float activeHeight;
//    float activeWidth;
//
//    // Internal Visualisers
//    std::unique_ptr<Component> containerView_FFT;
//    std::unique_ptr<Component> containerView_ColorSpectrum;
//    std::unique_ptr<Component> containerView_OctaveSpectrum;
//
//    std::unique_ptr<Label> label_FFT;
//    std::unique_ptr<Label> label_ColorSpectrum;
//    std::unique_ptr<Label> label_OctaveSpectrum;
//
//
//    bool popupsAreVisible = false;
//    std::unique_ptr<PopupFFTWindow> popupFFTWindow[3];
//
//    SpectrogramComponent *              spectrogramComponent = nullptr;
//    OctaveVisualiserComponent2 *        octaveComponent = nullptr;
//    ColourSpectrumVisualiserComponent*  colourComponent = nullptr;
//    FrequencyDataComponent*             frequencyDataComponent = nullptr;
//
//    SpectrogramComponent *      spectrogramComponentForPopup;
//    OctaveVisualiserComponent2 * octaveComponentForPopup;
//    ColourSpectrumVisualiserComponent* colourComponentForPopup;
//
//    std::unique_ptr<ImageButton> button_OpenWindowsSpectrum;
//    std::unique_ptr<ImageButton> button_OpenWindowsOctave;
//    std::unique_ptr<ImageButton> button_OpenWindowsColour;
//
//
//    // UI Layout Variables
//    int mainContainerHeight = 1096;
//    int fftLeftMargin       = 48;
//    int fftTopMargin        = 64;
//    int fftWidth            = 1476;
//    int fftShortWidth       = 1100;
//    int fftHeight           = 310;
//    int fftShortHeight      = 254;
//
//    int colorLeftMargin     = fftLeftMargin;
//    int colorTopMargin      = fftTopMargin + fftHeight + 67;
//    int colorWidth          = 742;
//    int colorHeight         = 271;
//
//    int octaveLeftMergin    = 822;
//    int octaveTopMargin     = colorTopMargin;
//    int octaveWidth         = 699;
//    int octaveHeight        = colorHeight;
//
//    Image imageFFTMockup;
//    Image imageColorSpectrumMockup;
//    Image imageOctaveSpectrumMockup;
//    Image imageButtonWindows;
//
//    ToggleButton * fftProcessButton;
//    ToggleButton * colourProcessButton;
//    ToggleButton * octaveProcessButton;
//
//    CustomLookAndFeel lookAndFeel;
//
//};
//
