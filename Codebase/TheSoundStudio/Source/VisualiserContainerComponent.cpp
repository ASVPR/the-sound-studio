/*
  ==============================================================================

    VisualiserContainerComponent.cpp
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#include "VisualiserContainerComponent.h"
#include "RealtimeAnalysisProcessor.h"


// Visualiser Container Component

/* Notes for optimisation

 1. Update rate refactor.. TIMER_UPDATE_RATE
 2. 


 */


//
//
//VisualiserContainerComponent::VisualiserContainerComponent(ProjectManager * pm, AUDIO_MODE newMode)
//{
//    mode            = newMode;
//    projectManager  = pm;
//    projectManager  ->addUIListener(this);
//    sampleRate      = projectManager->sample_rate;
//    
//    shouldFreeze    = false;
//    
//    // fonts
//    Typeface::Ptr AssistantBold     = Typeface::createSystemTypefaceFor(BinaryData::AssistantBold_ttf, BinaryData::AssistantBold_ttfSize);
//    Typeface::Ptr AssistantSemiBold   = Typeface::createSystemTypefaceFor(BinaryData::AssistantSemiBold_ttf, BinaryData::AssistantSemiBold_ttfSize);
//    Font fontBold(AssistantBold);
//    Font fontNormal(AssistantSemiBold);
//    
//    fontNormal.setHeight(33);
//    fontBold.setHeight(33);
//
//    imageButtonWindows                  = ImageCache::getFromMemory(BinaryData::Button_SpectrumOpen_png, BinaryData::Button_SpectrumOpen_pngSize);
//    
//    
//    if (mode == AUDIO_MODE::MODE_REALTIME_ANALYSIS)
//    {
//        containerView_FFT = new Component();
//        containerView_FFT->setBounds(fftLeftMargin, fftTopMargin, fftWidth, fftHeight);
//        
//        containerView_ColorSpectrum = new Component();
//        containerView_ColorSpectrum->setBounds(colorLeftMargin, colorTopMargin, colorWidth, colorHeight);
//        
//        Rectangle<float> spectrumRect(fftWidth, fftHeight);
//        
//        spectrogramComponent = new SpectrogramComponent(projectManager, spectrumRect, false);
//        spectrogramComponent->setBounds(0, 0, fftWidth, fftHeight);
//        containerView_FFT->addAndMakeVisible(spectrogramComponent);
//        addAndMakeVisible(containerView_FFT);
//        
//        colourComponent = new ColourSpectrumVisualiserComponent(projectManager, colorWidth, colorHeight);
//        colourComponent->setBounds(0, 0, fftWidth, colorHeight);
//        containerView_ColorSpectrum->addAndMakeVisible(colourComponent);
//        addAndMakeVisible(containerView_ColorSpectrum);
//        
//        frequencyDataComponent = new FrequencyDataComponent(projectManager);
//        addAndMakeVisible(frequencyDataComponent);
//        
//        spectrogramComponentForPopup = new SpectrogramComponent(projectManager, spectrumRect, true);
//        spectrogramComponentForPopup->setBounds(0, 0, fftWidth, fftHeight);
//        
//        colourComponentForPopup = new ColourSpectrumVisualiserComponent(projectManager, colorWidth, colorHeight);
//        colourComponentForPopup->setBounds(0, 0, fftWidth, colorHeight);
//        
//        
//        button_OpenWindowsSpectrum = new ImageButton();
//        button_OpenWindowsSpectrum->setTriggeredOnMouseDown(true);
//        button_OpenWindowsSpectrum->setImages (false, true, true,
//                                  imageButtonWindows, 0.999f, Colour (0x00000000),
//                                  Image(), 1.000f, Colour (0x00000000),
//                                  imageButtonWindows, 0.75, Colour (0x00000000));
//        button_OpenWindowsSpectrum->addListener(this);
//        addAndMakeVisible(button_OpenWindowsSpectrum);
//        
//        button_OpenWindowsColour = new ImageButton();
//        button_OpenWindowsColour->setTriggeredOnMouseDown(true);
//        button_OpenWindowsColour->setImages (false, true, true,
//                                  imageButtonWindows, 0.999f, Colour (0x00000000),
//                                  Image(), 1.000f, Colour (0x00000000),
//                                  imageButtonWindows, 0.75, Colour (0x00000000));
//        button_OpenWindowsColour->addListener(this);
//        addAndMakeVisible(button_OpenWindowsColour);
//        
//        label_FFT   = new Label();
//        label_FFT->setText("FFT Spectrum", dontSendNotification);
//        label_FFT->setJustificationType(Justification::left);
//        addAndMakeVisible(label_FFT);
//        
//        label_ColorSpectrum   = new Label();
//        label_ColorSpectrum->setText("Color Spectrum", dontSendNotification);
//        label_ColorSpectrum->setJustificationType(Justification::left);
//        addAndMakeVisible(label_ColorSpectrum);
//        
//        fftProcessButton    = new ToggleButton("On / Off");
//        fftProcessButton->setLookAndFeel(&lookAndFeel);
//        fftProcessButton->addListener(this);
//        fftProcessButton->setToggleState(true, dontSendNotification);
//        addAndMakeVisible(fftProcessButton);
//        
//        colourProcessButton = new ToggleButton("On / Off");
//        colourProcessButton->setLookAndFeel(&lookAndFeel);
//        colourProcessButton->addListener(this);
//        colourProcessButton->setToggleState(true, dontSendNotification);
//        addAndMakeVisible(colourProcessButton);
//
//        
//        popupFFTWindow[0] = new PopupFFTWindow("Spectrum Analyzer", spectrogramComponentForPopup,
//                                                     Colours::black, DocumentWindow::allButtons, true);
//        popupFFTWindow[0] ->centreWithSize(spectrogramComponent->getWidth(), spectrogramComponent->getHeight());
//        popupFFTWindow[0] ->setVisible(false);
//        popupFFTWindow[0] ->setResizable(true, true);
//        
//        popupFFTWindow[1] = new PopupFFTWindow("Colour Spectrum Visualizer", colourComponentForPopup,
//                                               Colours::black, DocumentWindow::allButtons, true);
//        popupFFTWindow[1] ->centreWithSize(colourComponent->getWidth(), colourComponent->getHeight());
//        popupFFTWindow[1] ->setVisible(false);
//        popupFFTWindow[1] ->setResizable(true, true);
//        
//    }
//    else if (mode == AUDIO_MODE::MODE_CHORD_PLAYER || mode == AUDIO_MODE::MODE_FREQUENCY_PLAYER)
//    {
//        containerView_FFT = new Component();
//        containerView_FFT->setBounds(fftLeftMargin, fftTopMargin, fftWidth, fftHeight);
//        
//        containerView_ColorSpectrum = new Component();
//        containerView_ColorSpectrum->setBounds(colorLeftMargin, colorTopMargin, colorWidth, colorHeight);
//
//        containerView_OctaveSpectrum = new Component();
//        containerView_OctaveSpectrum->setBounds(octaveLeftMergin, octaveTopMargin, octaveWidth, octaveHeight);
//        
//        Rectangle<float> spectrumRect(fftWidth, fftHeight);
//        spectrogramComponent = new SpectrogramComponent(projectManager, spectrumRect, false);
//        spectrogramComponent->setBounds(0, 0, fftWidth, fftHeight);
//        containerView_FFT->addAndMakeVisible(spectrogramComponent);
//        addAndMakeVisible(containerView_FFT);
//        
//        octaveComponent = new OctaveVisualiserComponent2(projectManager);
//        octaveComponent->setBounds(0, 0, octaveWidth, octaveHeight);
//        containerView_OctaveSpectrum->addAndMakeVisible(octaveComponent);
//        addAndMakeVisible(containerView_OctaveSpectrum);
//        
//        colourComponent = new ColourSpectrumVisualiserComponent(projectManager, colorWidth, colorHeight);
//        colourComponent->setBounds(0, 0, colorWidth, colorHeight);
//        containerView_ColorSpectrum->addAndMakeVisible(colourComponent);
//        addAndMakeVisible(containerView_ColorSpectrum);
//        
//        spectrogramComponentForPopup = new SpectrogramComponent(projectManager, spectrumRect, true);
//        spectrogramComponentForPopup->setBounds(0, 0, fftWidth, fftHeight);
//        
//        octaveComponentForPopup = new OctaveVisualiserComponent2(projectManager);
//        octaveComponentForPopup->setBounds(0, 0, octaveWidth, octaveHeight);
//        
//        colourComponentForPopup = new ColourSpectrumVisualiserComponent(projectManager, colorWidth, colorHeight);
//        colourComponentForPopup->setBounds(0, 0, colorWidth, colorHeight);
//        
//        button_OpenWindowsSpectrum = new ImageButton();
//        button_OpenWindowsSpectrum->setTriggeredOnMouseDown(true);
//        button_OpenWindowsSpectrum->setImages (false, true, true,
//                                  imageButtonWindows, 0.999f, Colour (0x00000000),
//                                  Image(), 1.000f, Colour (0x00000000),
//                                  imageButtonWindows, 0.75, Colour (0x00000000));
//        button_OpenWindowsSpectrum->addListener(this);
//        addAndMakeVisible(button_OpenWindowsSpectrum);
//        
//        button_OpenWindowsOctave = new ImageButton();
//        button_OpenWindowsOctave->setTriggeredOnMouseDown(true);
//        button_OpenWindowsOctave->setImages (false, true, true,
//                                  imageButtonWindows, 0.999f, Colour (0x00000000),
//                                  Image(), 1.000f, Colour (0x00000000),
//                                  imageButtonWindows, 0.75, Colour (0x00000000));
//        button_OpenWindowsOctave->addListener(this);
//        addAndMakeVisible(button_OpenWindowsOctave);
//        
//        button_OpenWindowsColour = new ImageButton();
//        button_OpenWindowsColour->setTriggeredOnMouseDown(true);
//        button_OpenWindowsColour->setImages (false, true, true,
//                                  imageButtonWindows, 0.999f, Colour (0x00000000),
//                                  Image(), 1.000f, Colour (0x00000000),
//                                  imageButtonWindows, 0.75, Colour (0x00000000));
//        button_OpenWindowsColour->addListener(this);
//        addAndMakeVisible(button_OpenWindowsColour);
//        
//        label_FFT   = new Label();
//        label_FFT->setText("FFT Spectrum", dontSendNotification);
//        label_FFT->setJustificationType(Justification::left);
//        addAndMakeVisible(label_FFT);
//        
//        label_ColorSpectrum   = new Label();
//        label_ColorSpectrum->setText("Color Spectrum", dontSendNotification);
//        label_ColorSpectrum->setJustificationType(Justification::left);
//        addAndMakeVisible(label_ColorSpectrum);
//        
//        label_OctaveSpectrum   = new Label();
//        label_OctaveSpectrum->setText("Octave Spectrum", dontSendNotification);
//        label_OctaveSpectrum->setJustificationType(Justification::left);
//        addAndMakeVisible(label_OctaveSpectrum);
//        
//        fftProcessButton    = new ToggleButton("On / Off");
//        fftProcessButton->setLookAndFeel(&lookAndFeel);
//        fftProcessButton->addListener(this);
//        fftProcessButton->setToggleState(true, dontSendNotification);
//        addAndMakeVisible(fftProcessButton);
//        
//        colourProcessButton = new ToggleButton("On / Off");
//        colourProcessButton->setLookAndFeel(&lookAndFeel);
//        colourProcessButton->addListener(this);
//        colourProcessButton->setToggleState(true, dontSendNotification);
//        addAndMakeVisible(colourProcessButton);
//        
//        octaveProcessButton = new ToggleButton("On / Off");
//        octaveProcessButton->setLookAndFeel(&lookAndFeel);
//        octaveProcessButton->addListener(this);
//        octaveProcessButton->setToggleState(true, dontSendNotification);
//        addAndMakeVisible(octaveProcessButton);
//        
//        popupFFTWindow[0] = new PopupFFTWindow("Spectrum Analyzer", spectrogramComponentForPopup,
//                                                     Colours::black, DocumentWindow::allButtons, true);
//        popupFFTWindow[0] ->centreWithSize(spectrogramComponent->getWidth(), spectrogramComponent->getHeight());
//        popupFFTWindow[0] ->setVisible(false);
//        popupFFTWindow[0] ->setResizable(true, true);
//        
//        popupFFTWindow[1] = new PopupFFTWindow("Colour Spectrum Visualizer", colourComponentForPopup,
//                                                     Colours::black, DocumentWindow::allButtons, true);
//        popupFFTWindow[1] ->centreWithSize(colourComponent->getWidth(), colourComponent->getHeight());
//        popupFFTWindow[1] ->setVisible(false);
//        popupFFTWindow[1] ->setResizable(true, true);
//        
//        popupFFTWindow[2] = new PopupFFTWindow("Octave Visualizer", octaveComponentForPopup,
//                                                     Colours::black, DocumentWindow::allButtons, true);
//        popupFFTWindow[2] ->centreWithSize(octaveComponent->getWidth(), octaveComponent->getHeight());
//        popupFFTWindow[2] ->setVisible(false);
//        popupFFTWindow[2] ->setResizable(true, true);
//        
//    }
//    else if (mode == AUDIO_MODE::MODE_CHORD_SCANNER || AUDIO_MODE::MODE_FREQUENCY_SCANNER)
//    {
//        // FFT
//        containerView_FFT = new Component();
//        containerView_FFT->setBounds(fftLeftMargin, fftTopMargin, fftWidth, fftHeight);
//        
//        Rectangle<float> spectrumRect(fftWidth, fftHeight);
//        
//        spectrogramComponent = new SpectrogramComponent(projectManager, spectrumRect, false);
//        spectrogramComponent->setBounds(0, 0, fftWidth, fftHeight);
//        containerView_FFT->addAndMakeVisible(spectrogramComponent);
//        addAndMakeVisible(containerView_FFT);
//        
//        spectrogramComponentForPopup = new SpectrogramComponent(projectManager, spectrumRect, true);
//        spectrogramComponentForPopup->setBounds(0, 0, fftWidth, fftHeight);
//        
//        button_OpenWindowsSpectrum = new ImageButton();
//        button_OpenWindowsSpectrum->setTriggeredOnMouseDown(true);
//        button_OpenWindowsSpectrum->setImages (false, true, true,
//                                  imageButtonWindows, 0.999f, Colour (0x00000000),
//                                  Image(), 1.000f, Colour (0x00000000),
//                                  imageButtonWindows, 0.75, Colour (0x00000000));
//        button_OpenWindowsSpectrum->addListener(this);
//        addAndMakeVisible(button_OpenWindowsSpectrum);
//        
//        button_OpenWindowsOctave = new ImageButton();
//        button_OpenWindowsOctave->setTriggeredOnMouseDown(true);
//        button_OpenWindowsOctave->setImages (false, true, true,
//                                  imageButtonWindows, 0.999f, Colour (0x00000000),
//                                  Image(), 1.000f, Colour (0x00000000),
//                                  imageButtonWindows, 0.75, Colour (0x00000000));
//        button_OpenWindowsOctave->addListener(this);
//        
//        button_OpenWindowsColour = new ImageButton();
//        button_OpenWindowsColour->setTriggeredOnMouseDown(true);
//        button_OpenWindowsColour->setImages (false, true, true,
//                                  imageButtonWindows, 0.999f, Colour (0x00000000),
//                                  Image(), 1.000f, Colour (0x00000000),
//                                  imageButtonWindows, 0.75, Colour (0x00000000));
//        button_OpenWindowsColour->addListener(this);
//        
//        label_FFT   = new Label();
//        label_FFT->setText("FFT Spectrum", dontSendNotification);
//        label_FFT->setJustificationType(Justification::left);
//        addAndMakeVisible(label_FFT);
//        
//        fftProcessButton    = new ToggleButton("On / Off");
//        fftProcessButton->setLookAndFeel(&lookAndFeel);
//        fftProcessButton->addListener(this);
//        fftProcessButton->setToggleState(true, dontSendNotification);
//        addAndMakeVisible(fftProcessButton);
//        
//        popupFFTWindow[0] = new PopupFFTWindow("Spectrum Analyzer", spectrogramComponentForPopup,
//                                                     Colours::black, DocumentWindow::allButtons, true);
//        
//        popupFFTWindow[0] ->centreWithSize(spectrogramComponent->getWidth(), spectrogramComponent->getHeight());
//        popupFFTWindow[0] ->setVisible(false);
//        popupFFTWindow[0] ->setResizable(true, true);
//        
//    }
//
//    peakFrequencyLabel = new Label("", "Peak Frequency : 0.0hz  0.0dB  EMA : 0.0hz");
//    addAndMakeVisible(peakFrequencyLabel);
//}
//
//VisualiserContainerComponent::~VisualiserContainerComponent()
//{
//    
//}
//
//void VisualiserContainerComponent::paint (Graphics&g)
//{
//}
//
//void VisualiserContainerComponent::resized()
//{
//    if (mode == AUDIO_MODE::MODE_REALTIME_ANALYSIS)
//    {
//        containerView_FFT               ->setBounds(fftLeftMargin * scaleFactor, fftTopMargin * scaleFactor, fftWidth * scaleFactor, fftHeight * scaleFactor);
//        containerView_ColorSpectrum     ->setBounds(colorLeftMargin * scaleFactor, colorTopMargin * scaleFactor, fftWidth * scaleFactor, colorHeight * scaleFactor);
//        spectrogramComponent            ->setBounds(0, 0, fftWidth * scaleFactor, fftHeight * scaleFactor);
//        colourComponent                 ->setBounds(0, 0, fftWidth * scaleFactor, colorHeight * scaleFactor);
//        spectrogramComponentForPopup    ->setBounds(0, 0, fftWidth * scaleFactor, fftHeight * scaleFactor);
//        colourComponentForPopup         ->setBounds(0, 0, fftWidth * scaleFactor, colorHeight * scaleFactor);
//        
//        frequencyDataComponent->setBounds(colorLeftMargin * scaleFactor, (colorTopMargin + colorHeight + 40) * scaleFactor, colorWidth * scaleFactor , 350.f * scaleFactor);
//        
//        button_OpenWindowsSpectrum->setBounds(spectrogramComponent->getWidth(), 16 * scaleFactor, 55 * scaleFactor, 55 * scaleFactor);
//        
//        button_OpenWindowsColour->setBounds((colorLeftMargin * scaleFactor) + colourComponent->getWidth() - (50 * scaleFactor), (colorTopMargin-52) * scaleFactor, 55 * scaleFactor, 55 * scaleFactor);
//        
//        label_FFT->setBounds(fftLeftMargin * scaleFactor, 20 * scaleFactor, 300 * scaleFactor, 40 * scaleFactor);
//        label_FFT->setFont(33  * scaleFactor);
//        
//        label_ColorSpectrum->setBounds(colorLeftMargin * scaleFactor, (colorTopMargin-45) * scaleFactor, 300 * scaleFactor, 40 * scaleFactor);
//        label_ColorSpectrum->setFont(33  * scaleFactor);
//        
//        
//
//    }
//    else if (mode == AUDIO_MODE::MODE_CHORD_PLAYER || mode == AUDIO_MODE::MODE_FREQUENCY_PLAYER)
//    {
//        containerView_FFT               ->setBounds(fftLeftMargin * scaleFactor, fftTopMargin * scaleFactor, fftWidth * scaleFactor, fftHeight * scaleFactor);
//        containerView_ColorSpectrum     ->setBounds(colorLeftMargin * scaleFactor, colorTopMargin * scaleFactor, colorWidth * scaleFactor, colorHeight * scaleFactor);
//        containerView_OctaveSpectrum    ->setBounds(octaveLeftMergin * scaleFactor, octaveTopMargin * scaleFactor, octaveWidth * scaleFactor, octaveHeight * scaleFactor);
//        spectrogramComponent            ->setBounds(0, 0, fftWidth * scaleFactor, fftHeight * scaleFactor);
//        octaveComponent                 ->setBounds(0, 0, octaveWidth * scaleFactor, octaveHeight * scaleFactor);
//        colourComponent                 ->setBounds(0, 0, colorWidth * scaleFactor, colorHeight * scaleFactor);
//        spectrogramComponentForPopup    ->setBounds(0, 0, fftWidth * scaleFactor, fftHeight * scaleFactor);
//        octaveComponentForPopup         ->setBounds(0, 0, octaveWidth * scaleFactor, octaveHeight * scaleFactor);
//        colourComponentForPopup         ->setBounds(0, 0, colorWidth * scaleFactor, colorHeight * scaleFactor);
//        
//        button_OpenWindowsSpectrum->setBounds(spectrogramComponent->getWidth(), 16 * scaleFactor, 55 * scaleFactor, 55 * scaleFactor);
//        
//        button_OpenWindowsOctave->setBounds((octaveLeftMergin * scaleFactor) + octaveComponent->getWidth() - (50 * scaleFactor), (octaveTopMargin-52) * scaleFactor, 55 * scaleFactor, 55 * scaleFactor);
//        
//        button_OpenWindowsColour->setBounds((colorLeftMargin * scaleFactor) + colourComponent->getWidth() - (50 * scaleFactor), (colorTopMargin-52) * scaleFactor, 55 * scaleFactor, 55 * scaleFactor);
//        
//        label_FFT->setBounds(fftLeftMargin * scaleFactor, 20 * scaleFactor, 300 * scaleFactor, 40 * scaleFactor);
//        label_FFT->setFont(33  * scaleFactor);
//        
//        label_ColorSpectrum->setBounds(colorLeftMargin * scaleFactor, (colorTopMargin-45) * scaleFactor, 300 * scaleFactor, 40 * scaleFactor);
//        label_ColorSpectrum->setFont(33  * scaleFactor);
//        
//        label_OctaveSpectrum->setBounds(octaveLeftMergin * scaleFactor, (octaveTopMargin-45) * scaleFactor, 300 * scaleFactor, 40 * scaleFactor);
//        label_OctaveSpectrum->setFont(33 * scaleFactor);
//        
//        fftProcessButton->setBounds(label_FFT->getWidth() + 2, (fftTopMargin-40) * scaleFactor, 200 * scaleFactor, 30 * scaleFactor);
//        colourProcessButton->setBounds(label_ColorSpectrum->getWidth() + 2, (colorTopMargin-40) * scaleFactor, 200 * scaleFactor, 30 * scaleFactor);
//        octaveProcessButton->setBounds((octaveLeftMergin * scaleFactor) + label_OctaveSpectrum->getWidth()+2, (octaveTopMargin-40) * scaleFactor, 200 * scaleFactor, 30 * scaleFactor);
//        
//        float x = label_FFT->getWidth() + fftProcessButton->getWidth() + 5;
//        float w = spectrogramComponent->getWidth() - x;
//        peakFrequencyLabel->setBounds(x, (fftTopMargin-40) * scaleFactor, w, 30 * scaleFactor);
//        peakFrequencyLabel->setFont(30 * scaleFactor);
//    }
//    else if (mode == AUDIO_MODE::MODE_CHORD_SCANNER || AUDIO_MODE::MODE_FREQUENCY_SCANNER)
//    {
//        containerView_FFT               ->setBounds(fftLeftMargin * scaleFactor, fftTopMargin * scaleFactor, fftWidth * scaleFactor, fftHeight * scaleFactor);
//
//        spectrogramComponent            ->setBounds(0, 0, fftShortWidth * scaleFactor, fftShortHeight * scaleFactor);
//
//        spectrogramComponentForPopup    ->setBounds(0, 0, fftWidth * scaleFactor, fftHeight * scaleFactor);
//        
//        button_OpenWindowsSpectrum->setBounds(spectrogramComponent->getWidth(), 16 * scaleFactor, 55 * scaleFactor, 55 * scaleFactor);
//        
//        label_FFT->setBounds(fftLeftMargin * scaleFactor, 20 * scaleFactor, 300 * scaleFactor, 40 * scaleFactor);
//        label_FFT->setFont(33  * scaleFactor);
//        
//        
//        fftProcessButton->setBounds(/*(fftLeftMargin+180) * scaleFactor*/ label_FFT->getWidth() + 2, (fftTopMargin-40) * scaleFactor, 200 * scaleFactor, 30 * scaleFactor);
//        
//        float x = label_FFT->getWidth() + fftProcessButton->getWidth() + 5;
//        float w = spectrogramComponent->getWidth() - x;
//        peakFrequencyLabel->setBounds(x, (fftTopMargin-40) * scaleFactor, w, 30 * scaleFactor);
//        peakFrequencyLabel->setFont(30 * scaleFactor);
//
//    }
//    
//
//}
//
//void VisualiserContainerComponent::buttonClicked (Button*button)
//{
//    popupsAreVisible = true;
//    
//    if (button == button_OpenWindowsSpectrum)
//    {
//        popupFFTWindow[0]->setVisible(true);
//    }
//    else if (button == button_OpenWindowsOctave)
//    {
//        popupFFTWindow[2]->setVisible(true);
//    }
//    else if (button == button_OpenWindowsColour)
//    {
//        popupFFTWindow[1]->setVisible(true);
//    }
//}
//
//void VisualiserContainerComponent::setPopupsAreVisible(bool shouldBeVisible)
//{
//    popupsAreVisible = shouldBeVisible;
//    
//    if (popupFFTWindow[0] != nullptr) { popupFFTWindow[0]->setVisible(shouldBeVisible); }
//    if (popupFFTWindow[1] != nullptr) { popupFFTWindow[1]->setVisible(shouldBeVisible); }
//    if (popupFFTWindow[2] != nullptr) { popupFFTWindow[2]->setVisible(shouldBeVisible); }
//}
//
//void VisualiserContainerComponent::updateSettingsUIParameter(int param)
//{
//    if (param == FFT_SIZE)
//    {
//        fftSize = projectManager->getFFTSize();
//    }
//}
//
//
//
//
//
//void VisualiserContainerComponent::setScale(float factor)
//{
//    scaleFactor = factor;
//
//    lookAndFeel.setScale(scaleFactor);
//
//    
//    if (mode == AUDIO_MODE::MODE_REALTIME_ANALYSIS)
//    {
//        if (spectrogramComponent != nullptr) spectrogramComponent->setScale(scaleFactor);
//        if (colourComponent != nullptr) colourComponent->setScale(scaleFactor);
//        
//        spectrogramComponentForPopup    ->setScale(scaleFactor);
//        colourComponentForPopup         ->setScale(scaleFactor);
//    }
//    else if (mode == AUDIO_MODE::MODE_CHORD_PLAYER || mode == AUDIO_MODE::MODE_FREQUENCY_PLAYER)
//    {
//        if (spectrogramComponent != nullptr) spectrogramComponent->setScale(scaleFactor);
//        if (octaveComponent != nullptr) octaveComponent->setScale(scaleFactor);
//        if (colourComponent != nullptr) colourComponent->setScale(scaleFactor);
//        
//        
//        spectrogramComponentForPopup    ->setScale(scaleFactor);
//        colourComponentForPopup         ->setScale(scaleFactor);
//        octaveComponentForPopup         ->setScale(scaleFactor);
//    }
//    else if (mode == AUDIO_MODE::MODE_CHORD_SCANNER || AUDIO_MODE::MODE_FREQUENCY_SCANNER)
//    {
//        if (spectrogramComponent != nullptr) spectrogramComponent->setScale(scaleFactor);
//        
//        spectrogramComponentForPopup    ->setScale(scaleFactor);
//    }
//}
//
//
//void VisualiserContainerComponent::pushUpdate()
//{
////    if (!shouldFreeze)
////    {
////        // need logic to determine if fft should update for view..
////        if (projectManager->mode == mode)
////        {
////            if ( projectManager->checkForNewAnalyserData() )
////            {
////                fftData     = projectManager->getFFTData();
////                sampleRate  = projectManager->getSampleRate();
////                fftSize     = projectManager->getFFTSize();
////
////                if (mode == AUDIO_MODE::MODE_REALTIME_ANALYSIS)
////                {
////                    frequencyDataComponent->pushUpdate(fftSize, sampleRate);
////
////                    if (popupFFTWindow[0]->isVisible())
////                    {
////                        spectrogramComponentForPopup    ->pushUpdate(fftData, fftSize, sampleRate, false);
////                    }
////                    else
////                    {
////                        spectrogramComponent    ->pushUpdate(fftData, fftSize, sampleRate, false);
////                    }
////
////                    if (popupFFTWindow[1]->isVisible())
////                    {
////                        colourComponentForPopup         ->pushUpdate(fftData, fftSize, sampleRate, false);
////                    }
////                    else
////                    {
////                        colourComponent         ->pushUpdate(fftData, fftSize, sampleRate, false);
////                    }
////                }
////                else if (mode == AUDIO_MODE::MODE_CHORD_PLAYER || mode == AUDIO_MODE::MODE_FREQUENCY_PLAYER)
////                {
////                    if (fftProcessButton->getToggleState())
////                    {
////                        if (popupFFTWindow[0]->isVisible())
////                        {
////                            spectrogramComponentForPopup    ->pushUpdate(fftData, fftSize, sampleRate, false);
////                        }
////                        else
////                        {
////                            spectrogramComponent    ->pushUpdate(fftData, fftSize, sampleRate, false);
////                        }
////
////                        double freq; double db; double ema;
////
////                        projectManager->getMovingAveragePeakData(freq, db, ema);
////
////                        String peakString("Peak Freq : ");
////                        String freqString(freq, 3, false); freqString.append("hz ", 3);
////                        String dbString(db, 3, false); dbString.append("dB ", 3);
////                        String emaString(ema, 3, false); emaString.append("hz ", 3);
////                        peakString.append(freqString, 10);
////                        peakString.append(dbString, 10);
////                        peakString.append(" EMA : ", 8);
////                        peakString.append(emaString, 10);
////
////                        peakFrequencyLabel->setText(peakString, dontSendNotification);
////                    }
////
////                    if (octaveProcessButton->getToggleState())
////                    {
////                        if (popupFFTWindow[2]->isVisible()) {
////                            octaveComponentForPopup         ->pushUpdate(fftData, fftSize, sampleRate, false);
////                        }
////                        else
////                        {
////                            octaveComponent         ->pushUpdate(fftData, fftSize, sampleRate, false);
////                        }
////
////                    }
////
////                    if (colourProcessButton->getToggleState())
////                    {
////                        if (popupFFTWindow[1]->isVisible()) {
////                            colourComponentForPopup         ->pushUpdate(fftData, fftSize, sampleRate, false);
////                        }
////                        else
////                        {
////                            colourComponent         ->pushUpdate(fftData, fftSize, sampleRate, false);
////                        }
////
////                    }
////
////                }
////                else if (mode == AUDIO_MODE::MODE_CHORD_SCANNER || AUDIO_MODE::MODE_FREQUENCY_SCANNER)
////                {
////                    if (fftProcessButton->getToggleState())
////                    {
////                        if (popupFFTWindow[0]->isVisible()) {
////                            spectrogramComponentForPopup    ->pushUpdate(fftData, fftSize, sampleRate, false);
////                        }
////                        else
////                        {
////                            spectrogramComponent    ->pushUpdate(fftData, fftSize, sampleRate, false);
////                        }
////
////                        double freq; double db; double ema;
////
////                        projectManager->getMovingAveragePeakData(freq, db, ema);
////
////                        String peakString("Peak Freq : ");
////                        String freqString(freq, 3, false); freqString.append("hz ", 3);
////                        String dbString(db, 3, false); dbString.append("dB ", 3);
////                        String emaString(ema, 3, false); emaString.append("hz ", 3);
////                        peakString.append(freqString, 10);
////                        peakString.append(dbString, 10);
////                        peakString.append(" EMA : ", 8);
////                        peakString.append(emaString, 10);
////
////                        peakFrequencyLabel->setText(peakString, dontSendNotification);
////
////                    }
////                }
////            }
////        }
////    }
//}
//
//void VisualiserContainerComponent::calculatePeakValues(float * fft)
//{
//    // run through fft to find highest Value, get corresponding frequency, convert bin to dB, push display..
//    int binRef          = 0;
//    
//    float maxVal = 0;
//    for (int index = 1; index <= fftSize; index++)
//    {
//        if (fft[index] > maxVal)
//        {
//            maxVal = fft[index];
//            binRef = index;
//        }
//    }
//    
//    // then cal frequency from binRef
//    peakFrequencyValue = (sampleRate * binRef) / fftSize;
//    
////    // then get amplitude dB fom peakAmpLinear;
//    peakAmplitudeValue =  linearToDb(maxVal); //fft[binRef]);
//
//    updateLabels();
//}
//
//void VisualiserContainerComponent::updateLabels()
//{
//    // get peak amp + frequency from FFT bins
//    String peakFrequencyString("Peak Frequency : ");
//    String peakString(peakFrequencyValue);
//    peakString.append("Hz", 2);
//    peakFrequencyString.append(peakString, 10);
//    
//    peakFrequencyLabel->setText(peakFrequencyString, dontSendNotification);
//    
//    String peakAmpString("Peak Amplitude : ");
//    String peakString2(peakAmplitudeValue);
//    peakString2.append("dB", 2);
//    peakAmpString.append(peakString2, 10);
//    
//    peakAmplitudeLabel->setText(peakAmpString, dontSendNotification);
//}
//
//
//// add reset peaks button.. otherwise keep highest value in labels..
//void VisualiserContainerComponent::calculateVUMeters()
//{
//    // prob best to simply call back to rojectManager for vu value
////    meter_outputL->setValue(0, projectManager->getVUMeterValue(0));
////    meter_outputR->setValue(0, projectManager->getVUMeterValue(1));
////
////    meter_outputL->repaint();
////    meter_outputR->repaint();
//}
