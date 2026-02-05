/*
  ==============================================================================

    ProjectManager.cpp
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#include "ProjectManager.h"
#include "RealtimeAnalysisProcessor.h"
#include "MainComponent.h"

ProjectManager::ProjectManager() : backgroundThread("Audio Recorder Thread")
{
    try 
    {
        // Initialize directories with proper error handling
        logFileDirectory = File::getSpecialLocation(File::userDocumentsDirectory).getChildFile("TSS/Logs");
        recordFileDirectory = File::getSpecialLocation(File::userDocumentsDirectory).getChildFile("TSS/Recordings");
        profileDirectory = File::getSpecialLocation(File::userDocumentsDirectory).getChildFile("TSS/Profiles");
        
        // Create directories if they don't exist
        logFileDirectory.createDirectory();
        recordFileDirectory.createDirectory();
        profileDirectory.createDirectory();
        
        // FIXED: Exception-safe property initialization
        initializeApplicationProperties();
        
        // FIXED: Exception-safe plugin system initialization
        initializePluginSystem();
        
        // FIXED: Exception-safe FFT initialization
        initFFT();
        
        // FIXED: Thread-safe mode initialization
        currentMode.store(AUDIO_MODE::MODE_CHORD_PLAYER);
        mode = MODE_CHORD_PLAYER; // Keep for backward compatibility
        
        // FIXED: Smart pointer initialization for better memory safety
        logFileWriter = std::make_unique<LogFileWriter>(this);
        if (logFileWriter) {
            logFileWriter->initNewLogFileForAudioMode(currentMode.load());
            logFileWriter->initNewSettingsLogFile();
        }
        
        sampleLibraryManager = std::make_unique<SynthesisLibraryManager>();
        synthesisEngine = std::make_unique<SynthesisEngine>();
        
        frequencyManager = std::make_unique<FrequencyManager>();
        frequencyManager->setBaseAFrequency(432);
        
        // Initialize synthesis engine with frequency manager
        synthesisEngine->initialize(44100.0, frequencyManager.get());
        
        // Ensure project settings exist before any processors that query them
        // (e.g., FundamentalFrequencyProcessor uses FUNDAMENTAL_FREQUENCY_ALGORITHM on construct)
        projectSettings = std::make_unique<ValueTree>("ProjectSettings");
        initDefaultProjectSettings();

        // FIXED: Exception-safe processor initialization
        initializeProcessors();
        
    }
    catch (const std::exception& e) 
    {
        // FIXED: Proper cleanup on construction failure
        DBG("ProjectManager construction failed: " << e.what());
        cleanup();
        throw; // Re-throw to notify caller
    }
    
    // FIXED: Initialize plugin processors with smart pointers
    for (int i = 0; i < NUM_PLUGIN_SLOTS; i++)
    {
        pluginAssignProcessor[i] = std::make_unique<PluginAssignProcessor>(pluginFormatManager, knownPluginList.get());
    }
    
    formatManager.registerBasicFormats();
    
    // projectSettings is already created and initialized above
    
    for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
    {
        chordPlayerParameters[i] = std::make_unique<ValueTree>(String::formatted("ChordPlayerShortcutSynth%i", i));
        frequencyPlayerParameters[i] = std::make_unique<ValueTree>(String::formatted("FrequencyPlayerShortcutSynth%i", i));
    }
    
    chordScannerParameters = std::make_unique<ValueTree>("ChordScanner");
    frequencyScannerParameters = std::make_unique<ValueTree>("FrequencyScanner");
    frequencyToLightParameters = std::make_unique<ValueTree>("FrequencyToLight");
    lissajousParameters = std::make_unique<ValueTree>("LissajousCurve");
    
    initDefaultChordPlayerParameters();
    initDefaultChordScannerParameters();
    initDefaultFrequencyPlayerParameters();
    initDefaultFrequencyScannerParameters();
    initDefaultFrequencyToLightParameters();
    initDefaultLissajousParameters();
    
    vuMeterValues[0] = 0.f;
    vuMeterValues[1] = 0.f;
    
    noiseType = WHITE_NOISE;
    
    
    backgroundThread.startThread();

}

ProjectManager::~ProjectManager()
{
    // FIXED: Proper cleanup order and thread safety
    cleanup();
    
    outputAnalyser.stopThread(1000);

    // Stop analyzers
    for (int i = 0; i < 8; i++)
    {
        analyser[i].stopThread(1000);
    }
}

void ProjectManager::changeListenerCallback (ChangeBroadcaster*)
{
    // menuItemsChanged();
    
    // save the plugin list every time it gets changed, so that if we're scanning
    // and it crashes, we've still saved the previous ones
    std::unique_ptr<XmlElement> savedPluginList (knownPluginList->createXml());
    
    if (savedPluginList != nullptr)
    {
        PropertiesFile*userSettings = appProperties->getUserSettings();
        
        userSettings->setValue ("pluginList", savedPluginList.get());
        appProperties->saveIfNeeded();
    }
}

void ProjectManager::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    sample_rate                 = sampleRate;
    
    chordPlayerProcessor        ->prepareToPlay(sampleRate, samplesPerBlockExpected);
    chordScannerProcessor       ->prepareToPlay(sampleRate, samplesPerBlockExpected);
    fundamentalFrequencyProcessor->prepareToPlay(sampleRate, samplesPerBlockExpected);
    frequencyPlayerProcessor    ->prepareToPlay(sampleRate, samplesPerBlockExpected);
    frequencyScannerProcessor   ->prepareToPlay(sampleRate, samplesPerBlockExpected);
    lissajousProcessor          ->prepareToPlay(sampleRate, samplesPerBlockExpected);
    feedbackModuleProcessor     ->prepareToPlay(sampleRate, samplesPerBlockExpected);
    
    for (int i = 0; i < NUM_PLUGIN_SLOTS; i++)
    {
        pluginAssignProcessor[i]       ->prepareToPlay(sampleRate, samplesPerBlockExpected);
    }
    
    
//    visualisersRingBuffer = new RingBuffer<GLfloat> (2, samplesPerBlockExpected * 10);
    
    // **** need to set to FFTs
//    outputAnalyser.setupAnalyser (int (sampleRate), float (sampleRate));
//    
    for (int i = 0; i < 8; i++)
    {
        analyser[i].setupAnalyser(int (sampleRate), float (sampleRate));
    }
    
    setOscilloscopeRefreshRate(refreshRate);
}

void ProjectManager::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    // process input gains
    
    for (int chan = 0; chan < 4 && chan < bufferToFill.buffer->getNumChannels(); chan++)
    {
        bufferToFill.buffer->applyGain(chan, 0, bufferToFill.buffer->getNumSamples(), inputGain[chan]);
    }
    
    // process input FFTs...
    if (shouldProcessFFT)
    {
        for (int i = 0; i < 4; i ++)
        {
            if (inputFFT[i])
            {
                if (i < bufferToFill.buffer->getNumChannels())
                {
                    analyser[i].addAudioData(*bufferToFill.buffer, i, 1);
                }
            }
        }
    }

    if (panicButtonIsDown)
    {
        processPanicButtonNoise(*bufferToFill.buffer);
    }
    else
    {
//        if (mode != MODE_REALTIME_ANALYSIS && mode != MODE_FUNDAMENTAL_FREQUENCY)
//        {
//            bufferToFill.clearActiveBufferRegion();
//        }
        
        // FIXED: Use atomic mode access for thread safety
        const auto currentAudioMode = currentMode.load();
        switch (currentAudioMode)
        {
            case MODE_NONE:
            case MODE_FREQUENCY_TO_LIGHT:
            {
                // do nothing with the audio path
            }
                break;
            case MODE_CHORD_PLAYER:
            {
                bufferToFill.clearActiveBufferRegion();
                
                if (chordPlayerProcessor) {
                    processChordPlayer(*bufferToFill.buffer);
                }
            }
                break;
            case MODE_CHORD_SCANNER:
            {
                bufferToFill.clearActiveBufferRegion();
                
                if (chordScannerProcessor) {
                    processChordScanner(*bufferToFill.buffer);
                }
            }
                break;
            case MODE_FUNDAMENTAL_FREQUENCY:
            {
                processFundamentalFrequency(*bufferToFill.buffer);
            }
                break;
            case MODE_FEEDBACK_MODULE:
            {
                processFeedbackModule(*bufferToFill.buffer);
            }
                break;
            case MODE_FREQUENCY_PLAYER:
            {
                bufferToFill.clearActiveBufferRegion();
                
                if (frequencyPlayerProcessor) {
                    processFrequencyPlayer(*bufferToFill.buffer);
                }
            }
                break;
                
            case MODE_FREQUENCY_SCANNER:
            {
                bufferToFill.clearActiveBufferRegion();
                
                if (frequencyScannerProcessor) {
                    processFrequencyScanner(*bufferToFill.buffer);
                }
            }
                break;
                
            case MODE_LISSAJOUS_CURVES:
            {
                if (lissajousProcessor) {
                    processLissajousCurve(*bufferToFill.buffer);
                }
            }
                break;
                
            case MODE_REALTIME_ANALYSIS:
            {
                processRealtimeAnalysis(*bufferToFill.buffer);
            }
                break;
                
            default: break;
        }
        
        MidiBuffer midiBufTemp;
        
        for (int i = 0; i < NUM_PLUGIN_SLOTS; i++)
        {
            pluginAssignProcessor[i]->processBlock(*bufferToFill.buffer, midiBufTemp);
        }
        
    }
    
    // process output gains
    
    for (int chan = 0; chan < 4 && chan < bufferToFill.buffer->getNumChannels(); chan++)
    {
        bufferToFill.buffer->applyGain(chan, 0, bufferToFill.buffer->getNumSamples(), outputGain[chan]);
    }
    
    // grab FFT data here
    if (shouldProcessFFT)
    {
        // process input FFTs...
        if (shouldProcessFFT)
        {
            for (int i = 0; i < 4; i ++)
            {
                if (outputFFT[i])
                {
                    if (i < bufferToFill.buffer->getNumChannels())
                    {
                        analyser[i + 4].addAudioData(*bufferToFill.buffer, i, 1);
                    }
                }
            }
        }
    }
    

    for (int i = 0; i< bufferToFill.buffer->getNumSamples(); i++)
    {
        // Default stereo buffer (channels 0 and 1)
        visualiserRingBuffer.setSample(0, visualiserBufferCounter, bufferToFill.buffer->getSample(0, i));
        visualiserRingBuffer.setSample(1, visualiserBufferCounter, bufferToFill.buffer->getSample(1, i));

        // Per-channel oscilloscope buffers for input selection support
        int numChannels = bufferToFill.buffer->getNumChannels();
        for (int ch = 0; ch < numOscilloscopeChannels && ch < numChannels; ++ch)
        {
            float sample = bufferToFill.buffer->getSample(ch, i);
            oscilloscopeBuffers[ch].setSample(0, visualiserBufferCounter, sample);
            oscilloscopeBuffers[ch].setSample(1, visualiserBufferCounter, sample);
        }

        visualiserBufferCounter++;

        if (visualiserBufferCounter >= visualiserBufferSize) visualiserBufferCounter = 0;
    }
        
    if (mode == MODE_REALTIME_ANALYSIS)
    {
        // delete output buffer to avoid feedback loop
        bufferToFill.buffer->clear();
    }
//    }
 
}

void ProjectManager::processChordPlayer(AudioBuffer<float>& buffer)
{
    MidiBuffer tempMidiBuffer;
    chordPlayerProcessor->processBlock(buffer, tempMidiBuffer);
    
    // record here...
    if (shouldRecord /*&& transportState == TRANSPORT_STATE::PLAYING*/)
    {
        if (activeWriter != nullptr)
        {
            activeWriter->write (buffer.getArrayOfReadPointers(), buffer.getNumSamples());
            
            recordCounterInSamples += buffer.getNumSamples();
        }
    }
}

void ProjectManager::processChordScanner(AudioBuffer<float>& buffer)
{
    MidiBuffer tempMidiBuffer;
    chordScannerProcessor->processBlock(buffer, tempMidiBuffer);
}

void ProjectManager::processFrequencyPlayer(AudioBuffer<float>& buffer)
{
    MidiBuffer tempMidiBuffer;
    frequencyPlayerProcessor->processBlock(buffer, tempMidiBuffer);
    
    // record here...
    if (shouldRecord /*&& transportState == TRANSPORT_STATE::PLAYING*/)
    {
        if (activeWriter != nullptr)
        {
            activeWriter->write (buffer.getArrayOfReadPointers(), buffer.getNumSamples());
            
            recordCounterInSamples += buffer.getNumSamples();
        }
    }
}

void ProjectManager::processFrequencyScanner(AudioBuffer<float>& buffer)
{
    MidiBuffer tempMidiBuffer;
    frequencyScannerProcessor->processBlock(buffer, tempMidiBuffer);
}

void ProjectManager::processLissajousCurve(AudioBuffer<float>& buffer)
{
    MidiBuffer tempMidiBuffer;
    lissajousProcessor->processBlock(buffer, tempMidiBuffer);
}

void ProjectManager::processRealtimeAnalysis(AudioBuffer<float>& buffer)
{
    //======================================================================================
    // record here...
    if (shouldRecord /*&& transportState == TRANSPORT_STATE::PLAYING*/) // check playstate in realtime analysis
    {
        if (activeWriter != nullptr)
        {
            if (buffer.getNumChannels() > 1)
            {
                activeWriter->write (buffer.getArrayOfReadPointers(), buffer.getNumSamples());
                recordCounterInSamples += buffer.getNumSamples();
            }
        }
    }
    //======================================================================================

    // Check if analysis is active via the processor
    if (realtimeAnalysisProcessor && realtimeAnalysisProcessor->isPlaying())
    {
        // Ensure FFT processing is enabled (in case it was disabled elsewhere)
        shouldProcessFFT = true;

        // Ensure at least the first input channel FFT is enabled for visualization
        // This is critical for the realtime analysis to work with microphone input
        if (!inputFFT[0])
        {
            inputFFT[0] = true;
        }

        // Process input audio through analyzers for each enabled input channel
        // This ensures the FFT data is available for visualizers set to INPUT sources
        for (int i = 0; i < 4 && i < buffer.getNumChannels(); ++i)
        {
            if (inputFFT[i])
            {
                analyser[i].addAudioData(buffer, i, 1);
            }
        }
    }

    // Process through meters, ffts etc.. are already handled globally in getNextAudioBlock
    // based on the inputFFT/outputFFT flags and the active mode.
}

void ProjectManager::processFundamentalFrequency(AudioBuffer<float>& buffer)
{
    MidiBuffer tempMidiBuffer;
    fundamentalFrequencyProcessor->processBlock(buffer, tempMidiBuffer);
}

void ProjectManager::processFeedbackModule(AudioBuffer<float>& buffer)
{
    MidiBuffer tempMidiBuffer;
    feedbackModuleProcessor->processBlock(buffer, tempMidiBuffer);
}

void ProjectManager::releaseResources()
{
//    inputAnalyser.stopThread (1000);
//    outputAnalyser.stopThread (1000);
//    delete visualisersRingBuffer;
}

void ProjectManager::setOversampingFactor(int newFactor)
{
//    chordPlayerProcessor->setOversamplingFactor(newFactor);
}

void ProjectManager::setMode(AUDIO_MODE newMode)
{
    // FIXED: Use thread-safe atomic mode setting
    setAudioMode(newMode);

    Logger::writeToLog("ProjectManager::setMode -> " + String((int)newMode));

    // Enable FFT processing for realtime analysis mode
    if (newMode == MODE_REALTIME_ANALYSIS)
    {
        // Enable FFT processing globally
        shouldProcessFFT = true;

        // Enable input FFT for channel 1 by default (so users can see input audio)
        // This ensures the source selector combo box will have INPUT options
        if (!inputFFT[0])
        {
            setProjectSettingsParameter(MIXER_INPUT_FFT_1, 1.0);
        }

        Logger::writeToLog("Realtime Analysis: Enabled FFT processing and Input 1 FFT");
    }

    // depending on mode we should resync all the params
    initGUISync(newMode);

    uiListeners.call(&UIListener::updateAudioMode, newMode);
}

void ProjectManager::shortcutKeyPressed(CHORD_PLAYER_SHORTCUT_KEY keyRef)
{

}


//===============================================================================
#pragma mark Project Settings
//===============================================================================

void ProjectManager::initGUISync()
{
    // Called from MainComponent once everything has been built.
    // should update everything to be in sync with parameter valuetrees / processor
    
    
    
    setProjectSettingsParameter(GUI_SCALE, projectSettings->getProperty(getIdentifierForSettingIndex(GUI_SCALE)));
    
    
    //----------------------------------------------------------------
    // ChordPlayer
    //----------------------------------------------------------------
    for (int synthRef = 0; synthRef < NUM_SHORTCUT_SYNTHS; synthRef++)
    {
        // sync to chord player processor / synths
        for (int index = 0; index < TOTAL_NUM_CHORD_PLAYER_SHORTCUT_PARAMS; index++)
        {
            // needs to call ui fuction
            uiListeners.call(&UIListener::updateChordPlayerUIParameter, synthRef, index);
        }
    }
    
    //----------------------------------------------------------------
    // chordscanner
    //----------------------------------------------------------------
    for (int index = 0; index < TOTAL_NUM_CHORD_SCANNER_PARAMS; index++)
    {
        // needs to call ui fuction
        uiListeners.call(&UIListener::updateChordScannerUIParameter, index);
    }
    
    //----------------------------------------------------------------
    // fundamental Frequency
    //----------------------------------------------------------------
    for (int index = 0; index < TOTAL_NUM_FUNDAMENTAL_FREQUENCY_PARAMS; index++)
    {
        // needs to call ui fuction
        uiListeners.call(&UIListener::updateFundamentalFrequencyUIParameter, index);
    }
    
    //----------------------------------------------------------------
    // feedback Module
    //----------------------------------------------------------------
    for (int index = 0; index < TOTAL_NUM_FUNDAMENTAL_FEEDBACK_PARAMS; index++)
    {
        // needs to call ui fuction
        uiListeners.call(&UIListener::updateFundamentalFeedbackUIParameter, index);
    }
    
    //----------------------------------------------------------------
    // Frequency Player
    //----------------------------------------------------------------
    for (int synthRef = 0; synthRef < NUM_SHORTCUT_SYNTHS; synthRef++)
    {
        for (int index = 0; index < TOTAL_NUM_FREQUENCY_PLAYER_SHORTCUT_PARAMS; index++)
        {
            // needs to call ui fuction
            uiListeners.call(&UIListener::updateFrequencyPlayerUIParameter, synthRef, index);
        }
    }
    
    //----------------------------------------------------------------
    // Frequency Scanner
    //----------------------------------------------------------------
    for (int index = 0; index < TOTAL_NUM_FREQUENCY_SCANNER_PARAMS; index++)
    {
        // needs to call ui fuction
        uiListeners.call(&UIListener::updateFrequencyScannerUIParameter, index);
    }
    
    
    //----------------------------------------------------------------
    // Lissajous Curve
    //----------------------------------------------------------------
    
    // Set lissajous params (uses chordplayer shortcut params X 1 + Lissajous Params
    // chordplayer params are first in list and keep enumeration..
    // final lissajous params minus chordplayer params to keep enums
//    for (int index = 0; index < TOTAL_NUM_CHORD_PLAYER_SHORTCUT_PARAMS + TOTAL_NUM_LISSAJOUS_CURVE_PARAMS; index++)
    for (int index = 0; index < TOTAL_NUM_LISSAJOUS_CURVE_PARAMS; index++)
    {
        uiListeners.call(&UIListener::updateLissajousCurveUIParameter, index);
    }
    
    
    
    //----------------------------------------------------------------
    // Frequency To Light
    //----------------------------------------------------------------
    for (int index = 0; index < FREQUENCY_TO_LIGHT_PARAMS::TOTAL_NUM_FREQUENCY_LIGHT_PARAMS; index++)
    {
        uiListeners.call(&UIListener::updateFrequencyToLightUIParameter, index);
    }
}

void ProjectManager::initGUISync(AUDIO_MODE mode)
{
    // Called when new mode opens, to force synchronisation of the parameters hwich may have changed in the background
    
    if (mode == AUDIO_MODE::MODE_CHORD_PLAYER)
    {
        //----------------------------------------------------------------
        // ChordPlayer
        //----------------------------------------------------------------
        for (int synthRef = 0; synthRef < NUM_SHORTCUT_SYNTHS; synthRef++)
        {
            // sync to chord player processor / synths
            for (int index = 0; index < TOTAL_NUM_CHORD_PLAYER_SHORTCUT_PARAMS; index++)
            {
                // needs to call ui fuction
                uiListeners.call(&UIListener::updateChordPlayerUIParameter, synthRef, index);
            }
        }
    }
    else if (mode == AUDIO_MODE::MODE_CHORD_SCANNER)
    {
        //----------------------------------------------------------------
        // chordscanner
        //----------------------------------------------------------------
        for (int index = 0; index < TOTAL_NUM_CHORD_SCANNER_PARAMS; index++)
        {
            // needs to call ui fuction
            uiListeners.call(&UIListener::updateChordScannerUIParameter, index);
        }
    }
    else if (mode == AUDIO_MODE::MODE_FREQUENCY_PLAYER)
    {
        //----------------------------------------------------------------
        // Frequency Player
        //----------------------------------------------------------------
        for (int synthRef = 0; synthRef < NUM_SHORTCUT_SYNTHS; synthRef++)
        {
            for (int index = 0; index < TOTAL_NUM_FREQUENCY_PLAYER_SHORTCUT_PARAMS; index++)
            {
                // needs to call ui fuction
                uiListeners.call(&UIListener::updateFrequencyPlayerUIParameter, synthRef, index);
            }
        }
    }
    else if (mode == AUDIO_MODE::MODE_FREQUENCY_SCANNER)
    {
        //----------------------------------------------------------------
        // Frequency Scanner
        //----------------------------------------------------------------
        for (int index = 0; index < TOTAL_NUM_FREQUENCY_SCANNER_PARAMS; index++)
        {
            // needs to call ui fuction
            uiListeners.call(&UIListener::updateFrequencyScannerUIParameter, index);
        }
    }
    else if (mode == AUDIO_MODE::MODE_FUNDAMENTAL_FREQUENCY)
    {
        //----------------------------------------------------------------
        // Fundamental Frequency
        //----------------------------------------------------------------
        for (int index = 0; index < TOTAL_NUM_FUNDAMENTAL_FREQUENCY_PARAMS; index++)
        {
            // needs to call ui fuction
            uiListeners.call(&UIListener::updateFundamentalFrequencyUIParameter, index);
        }
    }
    else if (mode == AUDIO_MODE::MODE_FEEDBACK_MODULE)
    {
        for (int index = 0; index < TOTAL_NUM_FUNDAMENTAL_FEEDBACK_PARAMS; index++)
        {
            uiListeners.call(&UIListener::updateFundamentalFeedbackUIParameter, index);
        }
    }
    else if (mode == AUDIO_MODE::MODE_FREQUENCY_TO_LIGHT)
    {
        //----------------------------------------------------------------
        // Frequency To Light
        //----------------------------------------------------------------
        for (int index = 0; index < FREQUENCY_TO_LIGHT_PARAMS::TOTAL_NUM_FREQUENCY_LIGHT_PARAMS; index++)
        {
            uiListeners.call(&UIListener::updateFrequencyToLightUIParameter, index);
        }
    }
    else if (mode == AUDIO_MODE::MODE_REALTIME_ANALYSIS)
    {
        
    }
    else if (mode == AUDIO_MODE::MODE_LISSAJOUS_CURVES)
    {
        //----------------------------------------------------------------
        // Lissajous Curve
        //----------------------------------------------------------------
        
        // Set lissajous params (uses chordplayer shortcut params X 1 + Lissajous Params
        // chordplayer params are first in list and keep enumeration..
        // final lissajous params minus chordplayer params to keep enums
        for (int index = 0; index < TOTAL_NUM_CHORD_PLAYER_SHORTCUT_PARAMS + TOTAL_NUM_LISSAJOUS_CURVE_PARAMS; index++)
        {
            uiListeners.call(&UIListener::updateLissajousCurveUIParameter, index);
        }
    }
}

String ProjectManager::getIdentifierForSettingIndex(int index)
{
    switch (index) {
        case SAMPLE_RATE:                       return "SAMPLE_RATE"; break;
        case OVERSAMPLE_FACTOR:                 return "OVERSAMPLE_FACTOR"; break;
        case DEFAULT_SCALE:                     return "DEFAULT_SCALE"; break;
        case LOG_FILE_LOCATION:                 return "LOG_FILE_LOCATION"; break;
        case RECORD_FILE_LOCATION:              return "RECORD_FILE_LOCATION"; break;
        case RECORD_FILE_FORMAT:                return "RECORD_FILE_FORMAT"; break;
        case BASE_FREQUENCY_A:                  return "BASE_FREQUENCY_A"; break;
        case AMPLITUDE_MIN:                     return "AMPLITUDE_MIN"; break;
        case AMPLITUDE_MAX:                     return "AMPLITUDE_MAX"; break;
        case ATTACK_MIN:                        return "ATTACK_MIN"; break;
        case ATTACK_MAX:                        return "ATTACK_MAX"; break;
        case DECAY_MIN:                         return "DECAY_MIN"; break;
        case DECAY_MAX:                         return "DECAY_MAX"; break;
        case SUSTAIN_MIN:                       return "SUSTAIN_MIN"; break;
        case SUSTAIN_MAX:                       return "SUSTAIN_MAX"; break;
        case RELEASE_MIN:                       return "RELEASE_MIN"; break;
        case RELEASE_MAX:                       return "RELEASE_MAX"; break;
        case SHOW_HIGHEST_PEAK_FREQUENCY:       return "SHOW_HIGHEST_PEAK_FREQUENCY"; break;
        case NUMBER_HIGHEST_PEAK_FREQUENCIES:   return "NUMBER_HIGHEST_PEAK_FREQUENCIES"; break;
        case SHOW_HIGHEST_PEAK_OCTAVES:         return "SHOW_HIGHEST_PEAK_OCTAVES"; break;
        case NUMBER_HIGHEST_PEAK_OCTAVES:       return "NUMBER_HIGHEST_PEAK_OCTAVES"; break;
        case PLAYER_PLAY_IN_LOOP:               return "PLAYER_PLAY_IN_LOOP"; break;
        case PLAYER_PLAY_SIMULTANEOUS:          return "PLAYER_PLAY_SIMULTANEOUS"; break;
        case FFT_SIZE:                          return "FFT_SIZE"; break;
        case FFT_WINDOW:                        return "FFT_WINDOW"; break;
        case FFT_DELAY_MS:                      return "FFT_DELAY_MS"; break;
            
        case FFT_COLOR_SPEC_MAIN:               return "FFT_COLOR_SPEC_MAIN"; break;
        case FFT_COLOR_SPEC_SEC:                return "FFT_COLOR_SPEC_SEC"; break;
        case FFT_COLOR_OCTAVE_MAIN:             return "FFT_COLOR_OCTAVE_MAIN"; break;
        case FFT_COLOR_OCTAVE_SEC:              return "FFT_COLOR_OCTAVE_SEC"; break;
        case FFT_COLOR_COLOR_MAIN:              return "FFT_COLOR_COLOR_MAIN"; break;
        case FFT_COLOR_COLOR_SEC:               return "FFT_COLOR_COLOR_SEC"; break;
        case FFT_COLOR_3D_MAIN:                 return "FFT_COLOR_3D_MAIN"; break;
        case FFT_COLOR_3D_SEC:                  return "FFT_COLOR_3D_SEC"; break;
        case FFT_COLOR_FREQUENCY_MAIN:          return "FFT_COLOR_FREQUENCY_MAIN"; break;
        case FFT_COLOR_FREQUENCY_SEC:           return "FFT_COLOR_FREQUENCY_SEC"; break;
        case LISSAJOUS_COLOR_MAIN:              return "LISSAJOUS_COLOR_MAIN"; break;
        case LISSAJOUS_COLOR_SEC:               return "LISSAJOUS_COLOR_SEC"; break;
            
            
        case GUI_SCALE:                         return "GUI_SCALE"; break;
        case FUNDAMENTAL_FREQUENCY_ALGORITHM:   return "FUNDAMENTAL_FREQUENCY_ALGORITHM"; break;
        case PANIC_NOISE_TYPE:                  return "PANIC_NOISE_TYPE"; break;
        case PLUGIN_SELECTED_1:                 return "PLUGIN_SELECTED_1"; break;
        case PLUGIN_SELECTED_2:                 return "PLUGIN_SELECTED_2"; break;
        case PLUGIN_SELECTED_3:                 return "PLUGIN_SELECTED_3"; break;
        case PLUGIN_SELECTED_4:                 return "PLUGIN_SELECTED_4"; break;
        case FREQUENCY_TO_CHORD:                return "FREQUENCY_TO_CHORD"; break;
            
        case MIXER_INPUT_GAIN_1:                return "MIXER_INPUT_GAIN_1"; break;
        case MIXER_INPUT_GAIN_2:                return "MIXER_INPUT_GAIN_2"; break;
        case MIXER_INPUT_GAIN_3:                return "MIXER_INPUT_GAIN_3"; break;
        case MIXER_INPUT_GAIN_4:                return "MIXER_INPUT_GAIN_4"; break;
            
        case MIXER_OUTPUT_GAIN_1:                return "MIXER_OUTPUT_GAIN_1"; break;
        case MIXER_OUTPUT_GAIN_2:                return "MIXER_OUTPUT_GAIN_2"; break;
        case MIXER_OUTPUT_GAIN_3:                return "MIXER_OUTPUT_GAIN_3"; break;
        case MIXER_OUTPUT_GAIN_4:                return "MIXER_OUTPUT_GAIN_4"; break;
            
        case MIXER_INPUT_FFT_1:                return "MIXER_INPUT_FFT_1"; break;
        case MIXER_INPUT_FFT_2:                return "MIXER_INPUT_FFT_2"; break;
        case MIXER_INPUT_FFT_3:                return "MIXER_INPUT_FFT_3"; break;
        case MIXER_INPUT_FFT_4:                return "MIXER_INPUT_FFT_4"; break;
            
        case MIXER_OUTPUT_FFT_1:                return "MIXER_OUTPUT_FFT_1"; break;
        case MIXER_OUTPUT_FFT_2:                return "MIXER_OUTPUT_FFT_2"; break;
        case MIXER_OUTPUT_FFT_3:                return "MIXER_OUTPUT_FFT_3"; break;
        case MIXER_OUTPUT_FFT_4:                return "MIXER_OUTPUT_FFT_4"; break;
            
        default: return ""; break;
    }
}

void ProjectManager::initDefaultProjectSettings()
{
    projectSettings->setProperty(getIdentifierForSettingIndex(SAMPLE_RATE),          (double)48000,                          nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(OVERSAMPLE_FACTOR),    (int)1,                                 nullptr);
    
    projectSettings->setProperty(getIdentifierForSettingIndex(DEFAULT_SCALE),        (SCALES)CHROMATIC_PYTHAGOREAN,          nullptr);
    
    projectSettings->setProperty(getIdentifierForSettingIndex(LOG_FILE_LOCATION),    (String)"/location of file",            nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(RECORD_FILE_LOCATION), (String)"/location of recorded file",   nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(RECORD_FILE_FORMAT),   (int) 1,                         nullptr);
    
    projectSettings->setProperty(getIdentifierForSettingIndex(BASE_FREQUENCY_A),     (double)432.0,                          nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(AMPLITUDE_MIN),        (double)0,                         nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(AMPLITUDE_MAX),        (double)100.0,                          nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(ATTACK_MIN),           (double)0.0,                            nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(ATTACK_MAX),           (double)1000.0,                           nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(DECAY_MIN),            (double)0.0,                            nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(DECAY_MAX),            (double)1000.0,                           nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(SUSTAIN_MIN),          (double)0.0,                            nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(SUSTAIN_MAX),          (double)100.0,                            nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(RELEASE_MIN),          (double)0.0,                            nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(RELEASE_MAX),          (double)1000.0,                           nullptr);
    
    
    projectSettings->setProperty(getIdentifierForSettingIndex(SHOW_HIGHEST_PEAK_FREQUENCY),              (bool)false,        nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(NUMBER_HIGHEST_PEAK_FREQUENCIES),          (int)1,             nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(SHOW_HIGHEST_PEAK_OCTAVES),                (bool)false,         nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(NUMBER_HIGHEST_PEAK_OCTAVES),              (int)1,             nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(PLAYER_PLAY_IN_LOOP),                      (bool)false,         nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(PLAYER_PLAY_SIMULTANEOUS),                 (bool)false,         nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(FFT_SIZE),                                 (int)4,         nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(FFT_WINDOW),                               (int)6,         nullptr);
    
    projectSettings->setProperty(getIdentifierForSettingIndex(FFT_DELAY_MS),                             (int)1000,         nullptr);
    
    projectSettings->setProperty(getIdentifierForSettingIndex(FFT_COLOR_SPEC_MAIN),                     (int32)Colours::green.getARGB(),         nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(FFT_COLOR_SPEC_SEC),                      (int32)Colours::red.getARGB(),         nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(FFT_COLOR_OCTAVE_MAIN),                   (int32)Colours::green.getARGB(),         nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(FFT_COLOR_OCTAVE_SEC),                    (int32)Colours::red.getARGB(),         nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(FFT_COLOR_COLOR_MAIN),                    (int32)Colours::green.getARGB(),         nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(FFT_COLOR_COLOR_SEC),                     (int32)Colours::red.getARGB(),         nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(FFT_COLOR_3D_MAIN),                       (int32)Colours::green.getARGB(),         nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(FFT_COLOR_3D_SEC),                        (int32)Colours::red.getARGB(),         nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(FFT_COLOR_FREQUENCY_MAIN),                (int32)Colours::green.getARGB(),         nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(FFT_COLOR_FREQUENCY_SEC),                 (int32)Colours::red.getARGB(),         nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(LISSAJOUS_COLOR_MAIN),                    (int32)Colours::green.getARGB(),         nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(LISSAJOUS_COLOR_SEC),                     (int32)Colours::red.getARGB(),         nullptr);
    
    
    projectSettings->setProperty(getIdentifierForSettingIndex(GUI_SCALE),                                (int)1,         nullptr);

    projectSettings->setProperty(getIdentifierForSettingIndex(FUNDAMENTAL_FREQUENCY_ALGORITHM), 0, nullptr);
    
    projectSettings->setProperty(getIdentifierForSettingIndex(PANIC_NOISE_TYPE),                                 (int)0,         nullptr);
    
    projectSettings->setProperty(getIdentifierForSettingIndex(PLUGIN_SELECTED_1),                                (int)-1,         nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(PLUGIN_SELECTED_2),                                (int)-1,         nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(PLUGIN_SELECTED_3),                                (int)-1,         nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(PLUGIN_SELECTED_4),                                (int)-1,         nullptr);
    
    projectSettings->setProperty(getIdentifierForSettingIndex(FREQUENCY_TO_CHORD),                               (int)0,          nullptr);
    
    projectSettings->setProperty(getIdentifierForSettingIndex(MIXER_INPUT_GAIN_1),                               (double)1.f,          nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(MIXER_INPUT_GAIN_2),                               (double)1.f,          nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(MIXER_INPUT_GAIN_3),                               (double)1.f,          nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(MIXER_INPUT_GAIN_4),                               (double)1.f,          nullptr);
    
    projectSettings->setProperty(getIdentifierForSettingIndex(MIXER_OUTPUT_GAIN_1),                              (double)1.f,          nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(MIXER_OUTPUT_GAIN_2),                              (double)1.f,          nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(MIXER_OUTPUT_GAIN_3),                              (double)1.f,          nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(MIXER_OUTPUT_GAIN_4),                              (double)1.f,          nullptr);
    
    projectSettings->setProperty(getIdentifierForSettingIndex(MIXER_INPUT_FFT_1),                                (bool)true,          nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(MIXER_INPUT_FFT_2),                                (bool)false,          nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(MIXER_INPUT_FFT_3),                                (bool)false,          nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(MIXER_INPUT_FFT_4),                                (bool)false,          nullptr);
    
    projectSettings->setProperty(getIdentifierForSettingIndex(MIXER_OUTPUT_FFT_1),                               (bool)true,          nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(MIXER_OUTPUT_FFT_2),                               (bool)false,          nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(MIXER_OUTPUT_FFT_3),                               (bool)false,          nullptr);
    projectSettings->setProperty(getIdentifierForSettingIndex(MIXER_OUTPUT_FFT_4),                               (bool)false,          nullptr);

    setProjectSettingsParameter(FFT_SIZE, projectSettings->getProperty(getIdentifierForSettingIndex(FFT_SIZE)));
    setProjectSettingsParameter(FFT_WINDOW, projectSettings->getProperty(getIdentifierForSettingIndex(FFT_WINDOW)));
    setProjectSettingsParameter(PANIC_NOISE_TYPE, projectSettings->getProperty(getIdentifierForSettingIndex(PANIC_NOISE_TYPE)));
    
    
    // set mixer parameters ....
    for (int i = MIXER_INPUT_GAIN_1; i <= MIXER_OUTPUT_FFT_4; i++ )
    {
        setProjectSettingsParameter(i, projectSettings->getProperty(getIdentifierForSettingIndex(i)));
    }
    
    
//    for (int i = 0; i < TOTAL_NUM_SETTINGS; i++)
//    {
//        setProjectSettingsParameter(i, projectSettings->getProperty(getIdentifierForSettingIndex(i)));
//    }
    
//    for (int i = 0; i < 12; i++)
//    {
//        setSettingsColorParameter(i, getProjectSettingsParameter(FFT_COLOR_SPEC_MAIN + i))
//    }
    

}

// caled from gui controls
void ProjectManager::setProjectSettingsParameter(int index, double newVal)
{
    projectSettings->setProperty(getIdentifierForSettingIndex(index), newVal, nullptr);
    
    switch (index) {
        case BASE_FREQUENCY_A:
        {
            baseAFrequency = newVal;
            
            // set to ChordManager
            frequencyManager->setBaseAFrequency(baseAFrequency);
            // Update synthesis engines to reflect new concert A (A4) reference
            if (synthesisEngine)
                synthesisEngine->updateTuningReference(baseAFrequency);
            
            uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, DEFAULT_SCALE);
        }
            break;
            
        case DEFAULT_SCALE:
        {
            defaultScale = (SCALES)newVal;
            frequencyManager->setScale(defaultScale);

            setLissajousParameter(CHORDPLAYER_SCALE, defaultScale);
            
            for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++) {
                setChordPlayerParameter(i, CHORDPLAYER_SCALE, defaultScale);
            }
            
            // set min / max to actual rotaries next.. will need listener call function..
            uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, DEFAULT_SCALE);

        }
            break;
            
        case RECORD_FILE_FORMAT:
        {
            recordFileFormatIndex = (int)newVal - 1;

            // set min / max to actual rotaries next.. will need listener call function..
            uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, RECORD_FILE_FORMAT);

        }
            break;
            
        case AMPLITUDE_MIN:
        {
            amplitude_min = newVal;
            // set min / max to actual rotaries next.. will need listener call function..
            uiListeners.call(&::ProjectManager::UIListener::updateMinMaxSettings, index);
            
        }
            break;
            
        case AMPLITUDE_MAX:
        {
            amplitude_max   = newVal;
            
            uiListeners.call(&::ProjectManager::UIListener::updateMinMaxSettings, index);
        }
            break;
        case ATTACK_MIN:
        {
            attack_min      = newVal;
            
            uiListeners.call(&::ProjectManager::UIListener::updateMinMaxSettings, index);
        }
            break;
        case ATTACK_MAX:
        {
            attack_max      = newVal;
            
            uiListeners.call(&::ProjectManager::UIListener::updateMinMaxSettings, index);
        }
            break;
        case DECAY_MIN:
        {
            decay_min       = newVal;
            
            uiListeners.call(&::ProjectManager::UIListener::updateMinMaxSettings, index);
        }
            break;
        case DECAY_MAX:
        {
            decay_max       = newVal;
            
            uiListeners.call(&::ProjectManager::UIListener::updateMinMaxSettings, index);
        }
            break;
        case SUSTAIN_MIN:
        {
            sustain_min     = newVal;
            
            uiListeners.call(&::ProjectManager::UIListener::updateMinMaxSettings, index);
        }
            
        case SUSTAIN_MAX:
        {
            sustain_max     = newVal;
            
            uiListeners.call(&::ProjectManager::UIListener::updateMinMaxSettings, index);
        }
            break;
        case RELEASE_MIN:
        {
            release_min     = newVal;
            
            uiListeners.call(&::ProjectManager::UIListener::updateMinMaxSettings, index);
        }
            break;
        case RELEASE_MAX:
        {
            release_max     = newVal;
            
            uiListeners.call(&::ProjectManager::UIListener::updateMinMaxSettings, index);
        }
            break;
            
        case PLAYER_PLAY_SIMULTANEOUS: // CHECK FOR ERRORS **************
        {
            chordPlayerProcessor->repeater->setPlaySimultaneous((bool)newVal);
            frequencyPlayerProcessor->repeater->setPlaySimultaneous((bool)newVal);
            
            uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, DEFAULT_SCALE);
        }
            break;
            
        case FFT_SIZE:
        {
            setNewFFTSize((int)newVal);
            
            uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, FFT_SIZE);
        }
            break;
        
        case FFT_WINDOW:
        {
            setNewFFTWindowFunction((int)newVal-1);
            
            uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, FFT_WINDOW);
        }
            break;
            
        case FFT_DELAY_MS:
        {
            uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, FFT_DELAY_MS);
        }
            break;
            
        case SHOW_HIGHEST_PEAK_FREQUENCY:
        {
            uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, SHOW_HIGHEST_PEAK_FREQUENCY);
        }
            break;
            
        case NUMBER_HIGHEST_PEAK_FREQUENCIES:
        {
            uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, NUMBER_HIGHEST_PEAK_FREQUENCIES);
        }
            break;
            
        case SHOW_HIGHEST_PEAK_OCTAVES:
        {
            uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, SHOW_HIGHEST_PEAK_OCTAVES);
        }
            break;
            
        case NUMBER_HIGHEST_PEAK_OCTAVES:
        {
            uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, NUMBER_HIGHEST_PEAK_OCTAVES);
        }
            break;
            
        case PANIC_NOISE_TYPE:
        {
            noiseType = (NOISE_TYPE)newVal;
            
            uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, PANIC_NOISE_TYPE);
        }
            break;
            
        case PLUGIN_SELECTED_1:
        {
            bool didLoad = pluginAssignProcessor[0]->loadPluginRef((int)newVal);
            
            if (didLoad)
            {
                uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, PLUGIN_SELECTED_1);
            }
            else
            {
//                AlertWindow::showNativeDialogBox("Invalid Plugin", "Sorry, we could not load the plugin file you selected", false);
                
                projectSettings->setProperty(getIdentifierForSettingIndex(index), -1, nullptr);
                
                uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, PLUGIN_SELECTED_1);
            }
            
            
            
        }
            break;
            
        case PLUGIN_SELECTED_2:
        {
            bool didLoad = pluginAssignProcessor[1]->loadPluginRef((int)newVal);
            
            if (didLoad)
            {
                uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, PLUGIN_SELECTED_2);
            }
            else
            {
//                AlertWindow::showNativeDialogBox("Invalid Plugin", "Sorry, we could not load the plugin file you selected", false);
                
                projectSettings->setProperty(getIdentifierForSettingIndex(index), -1, nullptr);
                
                uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, PLUGIN_SELECTED_1);
            }
        }
            break;
            
        case PLUGIN_SELECTED_3:
        {
            bool didLoad = pluginAssignProcessor[2]->loadPluginRef((int)newVal);
            
            if (didLoad)
            {
                uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, PLUGIN_SELECTED_3);
            }
            else
            {
                projectSettings->setProperty(getIdentifierForSettingIndex(index), -1, nullptr);
                
                uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, PLUGIN_SELECTED_1);
            }
        }
            break;
            
        case PLUGIN_SELECTED_4:
        {
            bool didLoad = pluginAssignProcessor[3]->loadPluginRef((int)newVal);
            
            if (didLoad)
            {
                uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, PLUGIN_SELECTED_4);
            }
            else
            {
                projectSettings->setProperty(getIdentifierForSettingIndex(index), -1, nullptr);
                
                uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, PLUGIN_SELECTED_1);
            }
        }
            break;
            
        case FREQUENCY_TO_CHORD:
        {
            // do something here !!!!!
            
            uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, FREQUENCY_TO_CHORD);
        }
            break;
            
            
            
        case GUI_SCALE:
        {
            // do something here !!!!!
            int scale = (int)newVal;
            float factor = 0;
            
            if (scale == 0) factor = 0.25; else if (scale == 1) factor = 0.5; else if (scale == 2) factor = 0.75; else if (scale == 3) factor = 1.f; else factor = 0.5;
            mainComponent->setScale(factor);
            
            uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, GUI_SCALE);
        }
            break;

        case FUNDAMENTAL_FREQUENCY_ALGORITHM:
            uiListeners.call(&ProjectManager::UIListener::updateSettingsUIParameter, FUNDAMENTAL_FREQUENCY_ALGORITHM); break;
            
        case MIXER_INPUT_GAIN_1: inputGain[0] = newVal; uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, MIXER_INPUT_GAIN_1); break;
        case MIXER_INPUT_GAIN_2: inputGain[1] = newVal; uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, MIXER_INPUT_GAIN_2); break;
        case MIXER_INPUT_GAIN_3: inputGain[2] = newVal; uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, MIXER_INPUT_GAIN_3); break;
        case MIXER_INPUT_GAIN_4: inputGain[3] = newVal; uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, MIXER_INPUT_GAIN_4); break;
            
        case MIXER_OUTPUT_GAIN_1: outputGain[0] = newVal; uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, MIXER_OUTPUT_GAIN_1); break;
        case MIXER_OUTPUT_GAIN_2: outputGain[1] = newVal; uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, MIXER_OUTPUT_GAIN_2); break;
        case MIXER_OUTPUT_GAIN_3: outputGain[2] = newVal; uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, MIXER_OUTPUT_GAIN_3); break;
        case MIXER_OUTPUT_GAIN_4: outputGain[3] = newVal; uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, MIXER_OUTPUT_GAIN_4); break;
            
        case MIXER_INPUT_FFT_1: inputFFT[0] = (bool)newVal; uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, MIXER_INPUT_FFT_1); break;
        case MIXER_INPUT_FFT_2: inputFFT[1] = (bool)newVal; uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, MIXER_INPUT_FFT_2); break;
        case MIXER_INPUT_FFT_3: inputFFT[2] = (bool)newVal; uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, MIXER_INPUT_FFT_3); break;
        case MIXER_INPUT_FFT_4: inputFFT[3] = (bool)newVal; uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, MIXER_INPUT_FFT_4); break;
            
        case MIXER_OUTPUT_FFT_1: outputFFT[0] = (bool)newVal; uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, MIXER_OUTPUT_FFT_1); break;
        case MIXER_OUTPUT_FFT_2: outputFFT[1] = (bool)newVal; uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, MIXER_OUTPUT_FFT_2); break;
        case MIXER_OUTPUT_FFT_3: outputFFT[2] = (bool)newVal; uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, MIXER_OUTPUT_FFT_3); break;
        case MIXER_OUTPUT_FFT_4: outputFFT[3] = (bool)newVal; uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, MIXER_OUTPUT_FFT_4); break;
            

            
//        case FFT_COLOR_SPEC_MAIN:
//        {
//            uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, FFT_COLOR_SPEC_MAIN);
//        }
//
//        case FFT_COLOR_SPEC_SEC:
//        {
//            uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, FFT_COLOR_SPEC_SEC);
//        }
//
//        case FFT_COLOR_OCTAVE_MAIN:
//        {
//            uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, FFT_COLOR_OCTAVE_MAIN);
//        }
//
//        case FFT_COLOR_OCTAVE_SEC:
//        {
//            uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, FFT_COLOR_OCTAVE_SEC);
//        }
//
//        case FFT_COLOR_COLOR_MAIN:
//        {
//            uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, FFT_COLOR_COLOR_MAIN);
//        }
//
//        case FFT_COLOR_COLOR_SEC:
//        {
//            uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, FFT_COLOR_COLOR_SEC);
//        }
//
//        case FFT_COLOR_3D_MAIN:
//        {
//            uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, FFT_COLOR_3D_MAIN);
//        }
//
//        case FFT_COLOR_3D_SEC:
//        {
//            uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, FFT_COLOR_3D_SEC);
//        }
//
//        case FFT_COLOR_FREQUENCY_MAIN:
//        {
//            uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, FFT_COLOR_FREQUENCY_MAIN);
//        }
//
//        case FFT_COLOR_FREQUENCY_SEC:
//        {
//            uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, FFT_COLOR_FREQUENCY_SEC);
//        }
//
//        case LISSAJOUS_COLOR_MAIN:
//        {
//            uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, LISSAJOUS_COLOR_MAIN);
//        }
//
//        case LISSAJOUS_COLOR_SEC:
//        {
//            uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, LISSAJOUS_COLOR_SEC);
//        }
            
        default: break;
    }

}

double ProjectManager::getProjectSettingsParameter(int index)
{
    if (projectSettings == nullptr)
    {
        // Fallback: return sane default if accessed too early
        return 0.0;
    }
    const auto key = getIdentifierForSettingIndex(index);
    // If the property is missing, juce::var -> double yields 0.0
    return projectSettings->getProperty(key).operator double();
}

void ProjectManager::setDirectoryFileForSettingParameter(int index, File newDirectory)
{
    if (index == LOG_FILE_LOCATION)         { logFileDirectory = newDirectory; }
    else if (index == RECORD_FILE_LOCATION) { recordFileDirectory = newDirectory; }
}

File ProjectManager::getDirectoryFileForSettingParameter(int index)
{
    if (index == LOG_FILE_LOCATION)         { return logFileDirectory; }
    else if (index == RECORD_FILE_LOCATION) { return recordFileDirectory; }
    else return File();
}

void ProjectManager::syncSettingsGUI()
{
    for (int i = 0; i < TOTAL_NUM_SETTINGS; i++)
    {
        uiListeners.call(&UIListener::updateSettingsUIParameter, i);
    }
    
    logFileWriter->processLog_Settings_Parameters();
}

//===============================================================================
#pragma mark Commands
//===============================================================================
void ProjectManager::setPlayerCommand(PLAYER_COMMANDS command)
{
    if (mode == MODE_CHORD_PLAYER)
    {
        chordPlayerProcessor->setPlayerCommand(command);
        
        if (command == PLAYER_COMMANDS::COMMAND_PLAYER_PLAYPAUSE)
        {
            PLAY_STATE playState = chordPlayerProcessor->getPlaystate();
            
            if (playState == PLAYING)
            {
                uiListeners.call(&UIListener::freezeFFTProcessing, false);
            }
            else if (playState == PAUSED)
            {
                uiListeners.call(&UIListener::freezeFFTProcessing, true);
            }
        }
    }
    else if (mode == MODE_CHORD_SCANNER)
    {
        chordScannerProcessor->setPlayerCommand(command);
        
        if (command == PLAYER_COMMANDS::COMMAND_PLAYER_PLAYPAUSE)
        {
            PLAY_STATE playState = chordScannerProcessor->getPlaystate();
            
            if (playState == PLAYING)
            {
                uiListeners.call(&UIListener::freezeFFTProcessing, false);
            }
            else if (playState == PAUSED)
            {
                uiListeners.call(&UIListener::freezeFFTProcessing, true);
            }
        }
    }
    else if (mode == MODE_FREQUENCY_PLAYER)
    {
        frequencyPlayerProcessor->setPlayerCommand(command);
        
        if (command == PLAYER_COMMANDS::COMMAND_PLAYER_PLAYPAUSE)
        {
            PLAY_STATE playState = frequencyPlayerProcessor->getPlaystate();
            
            if (playState == PLAYING)
            {
                uiListeners.call(&UIListener::freezeFFTProcessing, false);
            }
            else if (playState == PAUSED)
            {
                uiListeners.call(&UIListener::freezeFFTProcessing, true);
            }
        }
    }
    else if (mode == MODE_FREQUENCY_SCANNER)
    {
        frequencyScannerProcessor->setPlayerCommand(command);
        
        if (command == PLAYER_COMMANDS::COMMAND_PLAYER_PLAYPAUSE)
        {
            PLAY_STATE playState = frequencyScannerProcessor->getPlaystate();
            
            if (playState == PLAYING)
            {
                uiListeners.call(&UIListener::freezeFFTProcessing, false);
            }
            else if (playState == PAUSED)
            {
                uiListeners.call(&UIListener::freezeFFTProcessing, true);
            }
        }
    }
    else if (mode == MODE_LISSAJOUS_CURVES)
    {
        lissajousProcessor->setPlayerCommand(command);
        
    }
    else if (mode == MODE_REALTIME_ANALYSIS)
    {
        if (realtimeAnalysisProcessor)
            realtimeAnalysisProcessor->setPlayerCommand(command);
    }
}

void ProjectManager::setPlayerPlayMode(PLAY_MODE pmode)
{
    if (pmode == PLAY_MODE::NORMAL)
    {
        setProjectSettingsParameter(PLAYER_PLAY_IN_LOOP, 0);
    }
    else
    {
        setProjectSettingsParameter(PLAYER_PLAY_IN_LOOP, 1);
    }
    
    if (mode == MODE_CHORD_PLAYER)
    {
        chordPlayerProcessor->setPlayerPlayMode(pmode);
    }
    else if (mode == MODE_CHORD_SCANNER)
    {
        chordScannerProcessor->setPlayerPlayMode(pmode);
    }
    else if (mode == MODE_FREQUENCY_PLAYER)
    {
        frequencyPlayerProcessor->setPlayerPlayMode(pmode);
    }
    else if (mode == MODE_FREQUENCY_SCANNER)
    {
        frequencyScannerProcessor->setPlayerPlayMode(pmode);
    }
    else if (mode == MODE_LISSAJOUS_CURVES)
    {
        lissajousProcessor->setPlayerPlayMode(pmode);
    }
}

// Commands
void ProjectManager::shortcutKeyDown(int shortcutRef)
{
    if (mode == MODE_CHORD_PLAYER)
    {
        chordPlayerProcessor->triggerKeyDown(shortcutRef);
    }
    else if (mode == MODE_FREQUENCY_PLAYER)
    {
        frequencyPlayerProcessor->triggerKeyDown(shortcutRef);
    }
}

void ProjectManager::shortcutKeyUp(int shortcutRef)
{
    if (mode == MODE_CHORD_PLAYER)
    {
        chordPlayerProcessor->triggerKeyUp(shortcutRef);
    }
    else if (mode == MODE_FREQUENCY_PLAYER)
    {
        frequencyPlayerProcessor->triggerKeyUp(shortcutRef);
    }
}

    //===============================================================================
#pragma mark Wavetable interface
    //===============================================================================
    
double * ProjectManager::getWavetableBufferForCurrentEditor(AUDIO_MODE mode, int shortcut)
{
    if (mode == AUDIO_MODE::MODE_CHORD_PLAYER)
    {
        return chordPlayerProcessor->wavetableSynth[shortcut]->getBaseWavetableBuffer();
    }
    else if (mode == AUDIO_MODE::MODE_CHORD_SCANNER)
    {
        return chordScannerProcessor->wavetableSynth->getBaseWavetableBuffer();
    }
    else if (mode == AUDIO_MODE::MODE_FREQUENCY_PLAYER)
    {
        return frequencyPlayerProcessor->wavetableSynth[shortcut]->getBaseWavetableBuffer();
    }
    else if (mode == AUDIO_MODE::MODE_FREQUENCY_SCANNER)
    {
        return frequencyScannerProcessor->wavetableSynth->getBaseWavetableBuffer();
    }
    else if (mode == AUDIO_MODE::MODE_LISSAJOUS_CURVES)
    {
//        return lissajousProcessor->wavetableSynth->getBaseWavetableBuffer();
    }
    else
        return new double[2048];
}

void * ProjectManager::setWavetableBuffer(AUDIO_MODE mode, int shortcut, double * tableArray)
{
    if (mode == AUDIO_MODE::MODE_CHORD_PLAYER)
    {
        chordPlayerProcessor->wavetableSynth[shortcut]->setWavetableBuffer(tableArray);
    }
    else if (mode == AUDIO_MODE::MODE_CHORD_SCANNER)
    {
        chordScannerProcessor->wavetableSynth->setWavetableBuffer(tableArray);
    }
    else if (mode == AUDIO_MODE::MODE_FREQUENCY_PLAYER)
    {
        frequencyPlayerProcessor->wavetableSynth[shortcut]->setWavetableBuffer(tableArray);
    }
    else if (mode == AUDIO_MODE::MODE_FREQUENCY_SCANNER)
    {
        frequencyScannerProcessor->wavetableSynth->setWavetableBuffer(tableArray);
    }
    else if (mode == AUDIO_MODE::MODE_LISSAJOUS_CURVES)
    {
//        lissajousProcessor->wavetableSynth->setWavetableBuffer(tableArray);
    }
}

void ProjectManager::initBaseWavetable(AUDIO_MODE mode, int shortcutRef, int newType)
{
    if (mode == AUDIO_MODE::MODE_CHORD_PLAYER)
    {
        chordPlayerProcessor->wavetableSynth[shortcutRef]->initBaseWaveType(newType);
    }
    else if (mode == AUDIO_MODE::MODE_CHORD_SCANNER)
    {
        chordScannerProcessor->wavetableSynth->initBaseWaveType(newType);
    }
    else if (mode == AUDIO_MODE::MODE_FREQUENCY_PLAYER)
    {
        frequencyPlayerProcessor->wavetableSynth[shortcutRef]->initBaseWaveType(newType);
    }
    else if (mode == AUDIO_MODE::MODE_FREQUENCY_SCANNER)
    {
        frequencyScannerProcessor->wavetableSynth->initBaseWaveType(newType);
    }
    else if (mode == AUDIO_MODE::MODE_LISSAJOUS_CURVES)
    {
//        lissajousProcessor->wavetableSynth->initBaseWaveType(newType);
    }
}


//===============================================================================
#pragma mark Lissajous / Chord Player Parameters
//===============================================================================

String ProjectManager::getIdentifierForLissajousParameterIndex(int index)
{
    switch (index) {
            
        case UNIT_1_PROCESSOR_TYPE:        return "UNIT_1_PROCESSOR_TYPE"; break;
        case UNIT_2_PROCESSOR_TYPE:        return "UNIT_2_PROCESSOR_TYPE"; break;
        case UNIT_3_PROCESSOR_TYPE:        return "UNIT_3_PROCESSOR_TYPE"; break;
            
        // Add internal Lissajous Parameters
        case UNIT_1_FREE_FLOW:              return "UNIT_1_FREE_FLOW"; break;
        case UNIT_2_FREE_FLOW:              return "UNIT_2_FREE_FLOW"; break;
        case UNIT_3_FREE_FLOW:              return "UNIT_3_FREE_FLOW"; break;

        // Add internal Lissajous Parameters
        case UNIT_1_PHASE:                  return "UNIT_1_PHASE"; break;
        case UNIT_2_PHASE:                  return "UNIT_2_PHASE"; break;
        case UNIT_3_PHASE:                  return "UNIT_3_PHASE"; break;
 
        // Add internal Lissajous Parameters
        case AMPLITUDE_X:                   return "AMPLITUDE_X"; break;
        case AMPLITUDE_Y:                   return "AMPLITUDE_Y"; break;
        case AMPLITUDE_Z:                   return "AMPLITUDE_Z"; break;
       
        case UNIT_1_FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE:               return "UNIT_1_FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE"; break;
        case UNIT_1_FREQUENCY_PLAYER_SHORTCUT_MUTE:                    return "UNIT_1_FREQUENCY_PLAYER_SHORTCUT_MUTE"; break;
        case UNIT_1_FREQUENCY_PLAYER_SHORTCUT_PLAY_AT_SAME_TIME:       return "UNIT_1_FREQUENCY_PLAYER_SHORTCUT_PLAY_AT_SAME_TIME"; break;
        case UNIT_1_FREQUENCY_PLAYER_FREQ_SOURCE:                      return "UNIT_1_FREQUENCY_PLAYER_FREQ_SOURCE"; break;
        case UNIT_1_FREQUENCY_PLAYER_CHOOSE_FREQ:                      return "UNIT_1_FREQUENCY_PLAYER_CHOOSE_FREQ"; break;
        case UNIT_1_FREQUENCY_PLAYER_RANGE_MIN:                        return "UNIT_1_FREQUENCY_PLAYER_RANGE_MIN"; break;
        case UNIT_1_FREQUENCY_PLAYER_RANGE_MAX:                        return "UNIT_1_FREQUENCY_PLAYER_RANGE_MAX"; break;
        case UNIT_1_FREQUENCY_PLAYER_RANGE_LENGTH:                     return "UNIT_1_FREQUENCY_PLAYER_RANGE_LENGTH"; break;
        case UNIT_1_FREQUENCY_PLAYER_RANGE_LOG:                        return "UNIT_1_FREQUENCY_PLAYER_RANGE_LOG"; break;
        case UNIT_1_FREQUENCY_PLAYER_MANIPULATE_CHOSEN_FREQUENCY:      return "UNIT_1_FREQUENCY_PLAYER_MANIPULATE_CHOSEN_FREQUENCY"; break;
        case UNIT_1_FREQUENCY_PLAYER_MULTIPLY_OR_DIVISION:             return "UNIT_1_FREQUENCY_PLAYER_MULTIPLY_OR_DIVISION"; break;
        case UNIT_1_FREQUENCY_PLAYER_MULTIPLY_VALUE:                   return "UNIT_1_FREQUENCY_PLAYER_MULTIPLY_VALUE"; break;
        case UNIT_1_FREQUENCY_PLAYER_DIVISION_VALUE:                   return "UNIT_1_FREQUENCY_PLAYER_DIVISION_VALUE"; break;
        case UNIT_1_FREQUENCY_PLAYER_WAVEFORM_TYPE:                    return "UNIT_1_FREQUENCY_PLAYER_WAVEFORM_TYPE"; break;
        case UNIT_1_FREQUENCY_PLAYER_AMPLITUDE:                        return "UNIT_1_FREQUENCY_PLAYER_AMPLITUDE"; break;
        case UNIT_1_FREQUENCY_PLAYER_ATTACK:                           return "UNIT_1_FREQUENCY_PLAYER_ATTACK"; break;
        case UNIT_1_FREQUENCY_PLAYER_DECAY:                            return "UNIT_1_FREQUENCY_PLAYER_DECAY"; break;
        case UNIT_1_FREQUENCY_PLAYER_SUSTAIN:                          return "UNIT_1_FREQUENCY_PLAYER_SUSTAIN"; break;
        case UNIT_1_FREQUENCY_PLAYER_RELEASE:                          return "UNIT_1_FREQUENCY_PLAYER_RELEASE"; break;
        case UNIT_1_FREQUENCY_PLAYER_NUM_REPEATS:                      return "UNIT_1_FREQUENCY_PLAYER_NUM_REPEATS"; break;
        case UNIT_1_FREQUENCY_PLAYER_NUM_PAUSE:                        return "UNIT_1_FREQUENCY_PLAYER_NUM_PAUSE"; break;
        case UNIT_1_FREQUENCY_PLAYER_NUM_DURATION:                     return "UNIT_1_FREQUENCY_PLAYER_NUM_DURATION"; break;
            
        case UNIT_2_FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE:               return "UNIT_2_FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE"; break;
        case UNIT_2_FREQUENCY_PLAYER_SHORTCUT_MUTE:                    return "UNIT_2_FREQUENCY_PLAYER_SHORTCUT_MUTE"; break;
        case UNIT_2_FREQUENCY_PLAYER_SHORTCUT_PLAY_AT_SAME_TIME:       return "UNIT_2_FREQUENCY_PLAYER_SHORTCUT_PLAY_AT_SAME_TIME"; break;
        case UNIT_2_FREQUENCY_PLAYER_FREQ_SOURCE:                      return "UNIT_2_FREQUENCY_PLAYER_FREQ_SOURCE"; break;
        case UNIT_2_FREQUENCY_PLAYER_CHOOSE_FREQ:                      return "UNIT_2_FREQUENCY_PLAYER_CHOOSE_FREQ"; break;
        case UNIT_2_FREQUENCY_PLAYER_RANGE_MIN:                        return "UNIT_2_FREQUENCY_PLAYER_RANGE_MIN"; break;
        case UNIT_2_FREQUENCY_PLAYER_RANGE_MAX:                        return "UNIT_2_FREQUENCY_PLAYER_RANGE_MAX"; break;
        case UNIT_2_FREQUENCY_PLAYER_RANGE_LENGTH:                     return "UNIT_2_FREQUENCY_PLAYER_RANGE_LENGTH"; break;
        case UNIT_2_FREQUENCY_PLAYER_RANGE_LOG:                        return "UNIT_2_FREQUENCY_PLAYER_RANGE_LOG"; break;
        case UNIT_2_FREQUENCY_PLAYER_MANIPULATE_CHOSEN_FREQUENCY:      return "UNIT_2_FREQUENCY_PLAYER_MANIPULATE_CHOSEN_FREQUENCY"; break;
        case UNIT_2_FREQUENCY_PLAYER_MULTIPLY_OR_DIVISION:             return "UNIT_2_FREQUENCY_PLAYER_MULTIPLY_OR_DIVISION"; break;
        case UNIT_2_FREQUENCY_PLAYER_MULTIPLY_VALUE:                   return "UNIT_2_FREQUENCY_PLAYER_MULTIPLY_VALUE"; break;
        case UNIT_2_FREQUENCY_PLAYER_DIVISION_VALUE:                   return "UNIT_2_FREQUENCY_PLAYER_DIVISION_VALUE"; break;
        case UNIT_2_FREQUENCY_PLAYER_WAVEFORM_TYPE:                    return "UNIT_2_FREQUENCY_PLAYER_WAVEFORM_TYPE"; break;
        case UNIT_2_FREQUENCY_PLAYER_AMPLITUDE:                        return "UNIT_2_FREQUENCY_PLAYER_AMPLITUDE"; break;
        case UNIT_2_FREQUENCY_PLAYER_ATTACK:                           return "UNIT_2_FREQUENCY_PLAYER_ATTACK"; break;
        case UNIT_2_FREQUENCY_PLAYER_DECAY:                            return "UNIT_2_FREQUENCY_PLAYER_DECAY"; break;
        case UNIT_2_FREQUENCY_PLAYER_SUSTAIN:                          return "UNIT_2_FREQUENCY_PLAYER_SUSTAIN"; break;
        case UNIT_2_FREQUENCY_PLAYER_RELEASE:                          return "UNIT_2_FREQUENCY_PLAYER_RELEASE"; break;
        case UNIT_2_FREQUENCY_PLAYER_NUM_REPEATS:                      return "UNIT_2_FREQUENCY_PLAYER_NUM_REPEATS"; break;
        case UNIT_2_FREQUENCY_PLAYER_NUM_PAUSE:                        return "UNIT_2_FREQUENCY_PLAYER_NUM_PAUSE"; break;
        case UNIT_2_FREQUENCY_PLAYER_NUM_DURATION:                     return "UNIT_2_FREQUENCY_PLAYER_NUM_DURATION"; break;
            
        case UNIT_3_FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE:               return "UNIT_3_FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE"; break;
        case UNIT_3_FREQUENCY_PLAYER_SHORTCUT_MUTE:                    return "UNIT_3_FREQUENCY_PLAYER_SHORTCUT_MUTE"; break;
        case UNIT_3_FREQUENCY_PLAYER_SHORTCUT_PLAY_AT_SAME_TIME:       return "UNIT_3_FREQUENCY_PLAYER_SHORTCUT_PLAY_AT_SAME_TIME"; break;
        case UNIT_3_FREQUENCY_PLAYER_FREQ_SOURCE:                      return "UNIT_3_FREQUENCY_PLAYER_FREQ_SOURCE"; break;
        case UNIT_3_FREQUENCY_PLAYER_CHOOSE_FREQ:                      return "UNIT_3_FREQUENCY_PLAYER_CHOOSE_FREQ"; break;
        case UNIT_3_FREQUENCY_PLAYER_RANGE_MIN:                        return "UNIT_3_FREQUENCY_PLAYER_RANGE_MIN"; break;
        case UNIT_3_FREQUENCY_PLAYER_RANGE_MAX:                        return "UNIT_3_FREQUENCY_PLAYER_RANGE_MAX"; break;
        case UNIT_3_FREQUENCY_PLAYER_RANGE_LENGTH:                     return "UNIT_3_FREQUENCY_PLAYER_RANGE_LENGTH"; break;
        case UNIT_3_FREQUENCY_PLAYER_RANGE_LOG:                        return "UNIT_3_FREQUENCY_PLAYER_RANGE_LOG"; break;
        case UNIT_3_FREQUENCY_PLAYER_MANIPULATE_CHOSEN_FREQUENCY:      return "UNIT_3_FREQUENCY_PLAYER_MANIPULATE_CHOSEN_FREQUENCY"; break;
        case UNIT_3_FREQUENCY_PLAYER_MULTIPLY_OR_DIVISION:             return "UNIT_3_FREQUENCY_PLAYER_MULTIPLY_OR_DIVISION"; break;
        case UNIT_3_FREQUENCY_PLAYER_MULTIPLY_VALUE:                   return "UNIT_3_FREQUENCY_PLAYER_MULTIPLY_VALUE"; break;
        case UNIT_3_FREQUENCY_PLAYER_DIVISION_VALUE:                   return "UNIT_3_FREQUENCY_PLAYER_DIVISION_VALUE"; break;
        case UNIT_3_FREQUENCY_PLAYER_WAVEFORM_TYPE:                    return "UNIT_3_FREQUENCY_PLAYER_WAVEFORM_TYPE"; break;
        case UNIT_3_FREQUENCY_PLAYER_AMPLITUDE:                        return "UNIT_3_FREQUENCY_PLAYER_AMPLITUDE"; break;
        case UNIT_3_FREQUENCY_PLAYER_ATTACK:                           return "UNIT_3_FREQUENCY_PLAYER_ATTACK"; break;
        case UNIT_3_FREQUENCY_PLAYER_DECAY:                            return "UNIT_3_FREQUENCY_PLAYER_DECAY"; break;
        case UNIT_3_FREQUENCY_PLAYER_SUSTAIN:                          return "UNIT_3_FREQUENCY_PLAYER_SUSTAIN"; break;
        case UNIT_3_FREQUENCY_PLAYER_RELEASE:                          return "UNIT_3_FREQUENCY_PLAYER_RELEASE"; break;
        case UNIT_3_FREQUENCY_PLAYER_NUM_REPEATS:                      return "UNIT_3_FREQUENCY_PLAYER_NUM_REPEATS"; break;
        case UNIT_3_FREQUENCY_PLAYER_NUM_PAUSE:                        return "UNIT_3_FREQUENCY_PLAYER_NUM_PAUSE"; break;
        case UNIT_3_FREQUENCY_PLAYER_NUM_DURATION:                     return "UNIT_3_FREQUENCY_PLAYER_NUM_DURATION"; break;
            
            // chordplayer 1
        case UNIT_1_CHORD_PLAYER_SHORTCUT_IS_ACTIVE:                return "UNIT_1_CHORD_PLAYER_SHORTCUT_IS_ACTIVE"; break;
        case UNIT_1_CHORD_PLAYER_SHORTCUT_MUTE:                     return "UNIT_1_CHORD_PLAYER_SHORTCUT_MUTE"; break;
        case UNIT_1_CHORD_PLAYER_SHORTCUT_PLAY_AT_SAME_TIME:        return "UNIT_1_CHORD_PLAYER_SHORTCUT_PLAY_AT_SAME_TIME"; break;
        case UNIT_1_CHORD_PLAYER_INSTRUMENT_TYPE:                   return "UNIT_1_CHORD_PLAYER_INSTRUMENT_TYPE"; break;
        case UNIT_1_CHORD_PLAYER_WAVEFORM_TYPE:                     return "UNIT_1_CHORD_PLAYER_WAVEFORM_TYPE"; break;
        case UNIT_1_CHORD_PLAYER_ENV_AMPLITUDE:                     return "UNIT_1_CHORD_PLAYER_ENV_AMPLITUDE"; break;
        case UNIT_1_CHORD_PLAYER_ENV_ATTACK:                        return "UNIT_1_CHORD_PLAYER_ENV_ATTACK"; break;
        case UNIT_1_CHORD_PLAYER_ENV_SUSTAIN:                       return "UNIT_1_CHORD_PLAYER_ENV_SUSTAIN"; break;
        case UNIT_1_CHORD_PLAYER_ENV_DECAY:                         return "UNIT_1_CHORD_PLAYER_ENV_DECAY"; break;
        case UNIT_1_CHORD_PLAYER_ENV_RELEASE:                       return "UNIT_1_CHORD_PLAYER_ENV_RELEASE"; break;
        case UNIT_1_CHORD_PLAYER_NUM_REPEATS:                       return "UNIT_1_CHORD_PLAYER_NUM_REPEATS"; break;
        case UNIT_1_CHORD_PLAYER_NUM_PAUSE:                         return "UNIT_1_CHORD_PLAYER_NUM_PAUSE"; break;
        case UNIT_1_CHORD_PLAYER_NUM_DURATION:                      return "UNIT_1_CHORD_PLAYER_NUM_DURATION"; break;
        case UNIT_1_CHORD_PLAYER_CHORD_SOURCE:                      return "UNIT_1_CHORD_PLAYER_CHORD_SOURCE"; break;
        case UNIT_1_CHORD_PLAYER_KEYNOTE:                           return "UNIT_1_CHORD_PLAYER_KEYNOTE"; break;
        case UNIT_1_CHORD_PLAYER_OCTAVE:                            return "UNIT_1_CHORD_PLAYER_OCTAVE"; break;
        case UNIT_1_CHORD_PLAYER_CHORD_TYPE:                        return "UNIT_1_CHORD_PLAYER_CHORD_TYPE"; break;
        case UNIT_1_CHORD_PLAYER_ADD_ONS:                           return "UNIT_1_CHORD_PLAYER_ADD_ONS"; break;
        case UNIT_1_CHORD_PLAYER_INSERT_FREQUENCY:                  return "UNIT_1_CHORD_PLAYER_INSERT_FREQUENCY"; break;
        case UNIT_1_CHORD_PLAYER_MANIPULATE_CHOSEN_FREQUENCY:       return "UNIT_1_CHORD_PLAYER_MANIPULATE_CHOSEN_FREQUENCY"; break;
        case UNIT_1_CHORD_PLAYER_MULTIPLY_OR_DIVISION:              return "UNIT_1_CHORD_PLAYER_MULTIPLY_OR_DIVISION"; break;
        case UNIT_1_CHORD_PLAYER_MULTIPLY_VALUE:                    return "UNIT_1_CHORD_PLAYER_MULTIPLY_VALUE"; break;
        case UNIT_1_CHORD_PLAYER_DIVISION_VALUE:                    return "UNIT_1_CHORD_PLAYER_DIVISION_VALUE"; break;
        case UNIT_1_CHORD_PLAYER_CUSTOM_CHORD:                      return "UNIT_1_CHORD_PLAYER_CUSTOM_CHORD"; break;
            
        case UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_1:             return "UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_1"; break;
        case UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_2:             return "UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_2"; break;
        case UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_3:             return "UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_3"; break;
        case UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_4:             return "UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_4"; break;
        case UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_5:             return "UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_5"; break;
        case UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_6:             return "UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_6"; break;
        case UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_7:             return "UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_7"; break;
        case UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_8:             return "UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_8"; break;
        case UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_9:             return "UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_9"; break;
        case UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_10:            return "UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_10"; break;
        case UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_11:            return "UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_11"; break;
        case UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_12:            return "UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_12"; break;
            
        case UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_1:               return "UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_1"; break;
        case UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_2:               return "UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_2"; break;
        case UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_3:               return "UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_3"; break;
        case UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_4:               return "UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_4"; break;
        case UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_5:               return "UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_5"; break;
        case UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_6:               return "UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_6"; break;
        case UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_7:               return "UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_7"; break;
        case UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_8:               return "UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_8"; break;
        case UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_9:               return "UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_9"; break;
        case UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_10:              return "UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_10"; break;
        case UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_11:              return "UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_11"; break;
        case UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_12:              return "UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_12"; break;
            
        case UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_1:             return "UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_1"; break;
        case UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_2:             return "UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_2"; break;
        case UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_3:             return "UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_3"; break;
        case UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_4:             return "UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_4"; break;
        case UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_5:             return "UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_5"; break;
        case UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_6:             return "UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_6"; break;
        case UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_7:             return "UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_7"; break;
        case UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_8:             return "UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_8"; break;
        case UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_9:             return "UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_9"; break;
        case UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_10:            return "UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_10"; break;
        case UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_11:            return "UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_11"; break;
        case UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_12:            return "UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_12"; break;
            
        case UNIT_1_CHORD_PLAYER_CHORDPLAYER_SCALE:                 return "UNIT_1_CHORD_PLAYER_CHORDPLAYER_SCALE"; break;
            
        // chordplayer 2
        case UNIT_2_CHORD_PLAYER_SHORTCUT_IS_ACTIVE:                return "UNIT_2_CHORD_PLAYER_SHORTCUT_IS_ACTIVE"; break;
        case UNIT_2_CHORD_PLAYER_SHORTCUT_MUTE:                     return "UNIT_2_CHORD_PLAYER_SHORTCUT_MUTE"; break;
        case UNIT_2_CHORD_PLAYER_SHORTCUT_PLAY_AT_SAME_TIME:        return "UNIT_2_CHORD_PLAYER_SHORTCUT_PLAY_AT_SAME_TIME"; break;
        case UNIT_2_CHORD_PLAYER_INSTRUMENT_TYPE:                   return "UNIT_2_CHORD_PLAYER_INSTRUMENT_TYPE"; break;
        case UNIT_2_CHORD_PLAYER_WAVEFORM_TYPE:                     return "UNIT_2_CHORD_PLAYER_WAVEFORM_TYPE"; break;
        case UNIT_2_CHORD_PLAYER_ENV_AMPLITUDE:                     return "UNIT_2_CHORD_PLAYER_ENV_AMPLITUDE"; break;
        case UNIT_2_CHORD_PLAYER_ENV_ATTACK:                        return "UNIT_2_CHORD_PLAYER_ENV_ATTACK"; break;
        case UNIT_2_CHORD_PLAYER_ENV_SUSTAIN:                       return "UNIT_2_CHORD_PLAYER_ENV_SUSTAIN"; break;
        case UNIT_2_CHORD_PLAYER_ENV_DECAY:                         return "UNIT_2_CHORD_PLAYER_ENV_DECAY"; break;
        case UNIT_2_CHORD_PLAYER_ENV_RELEASE:                       return "UNIT_2_CHORD_PLAYER_ENV_RELEASE"; break;
        case UNIT_2_CHORD_PLAYER_NUM_REPEATS:                       return "UNIT_2_CHORD_PLAYER_NUM_REPEATS"; break;
        case UNIT_2_CHORD_PLAYER_NUM_PAUSE:                         return "UNIT_2_CHORD_PLAYER_NUM_PAUSE"; break;
        case UNIT_2_CHORD_PLAYER_NUM_DURATION:                      return "UNIT_2_CHORD_PLAYER_NUM_DURATION"; break;
        case UNIT_2_CHORD_PLAYER_CHORD_SOURCE:                      return "UNIT_2_CHORD_PLAYER_CHORD_SOURCE"; break;
        case UNIT_2_CHORD_PLAYER_KEYNOTE:                           return "UNIT_2_CHORD_PLAYER_KEYNOTE"; break;
        case UNIT_2_CHORD_PLAYER_OCTAVE:                            return "UNIT_2_CHORD_PLAYER_OCTAVE"; break;
        case UNIT_2_CHORD_PLAYER_CHORD_TYPE:                        return "UNIT_2_CHORD_PLAYER_CHORD_TYPE"; break;
        case UNIT_2_CHORD_PLAYER_ADD_ONS:                           return "UNIT_2_CHORD_PLAYER_ADD_ONS"; break;
        case UNIT_2_CHORD_PLAYER_INSERT_FREQUENCY:                  return "UNIT_2_CHORD_PLAYER_INSERT_FREQUENCY"; break;
        case UNIT_2_CHORD_PLAYER_MANIPULATE_CHOSEN_FREQUENCY:       return "UNIT_2_CHORD_PLAYER_MANIPULATE_CHOSEN_FREQUENCY"; break;
        case UNIT_2_CHORD_PLAYER_MULTIPLY_OR_DIVISION:              return "UNIT_2_CHORD_PLAYER_MULTIPLY_OR_DIVISION"; break;
        case UNIT_2_CHORD_PLAYER_MULTIPLY_VALUE:                    return "UNIT_2_CHORD_PLAYER_MULTIPLY_VALUE"; break;
        case UNIT_2_CHORD_PLAYER_DIVISION_VALUE:                    return "UNIT_2_CHORD_PLAYER_DIVISION_VALUE"; break;
        case UNIT_2_CHORD_PLAYER_CUSTOM_CHORD:                      return "UNIT_2_CHORD_PLAYER_CUSTOM_CHORD"; break;
            
        case UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_1:             return "UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_1"; break;
        case UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_2:             return "UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_2"; break;
        case UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_3:             return "UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_3"; break;
        case UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_4:             return "UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_4"; break;
        case UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_5:             return "UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_5"; break;
        case UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_6:             return "UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_6"; break;
        case UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_7:             return "UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_7"; break;
        case UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_8:             return "UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_8"; break;
        case UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_9:             return "UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_9"; break;
        case UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_10:            return "UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_10"; break;
        case UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_11:            return "UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_11"; break;
        case UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_12:            return "UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_12"; break;
            
        case UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_1:               return "UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_1"; break;
        case UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_2:               return "UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_2"; break;
        case UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_3:               return "UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_3"; break;
        case UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_4:               return "UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_4"; break;
        case UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_5:               return "UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_5"; break;
        case UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_6:               return "UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_6"; break;
        case UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_7:               return "UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_7"; break;
        case UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_8:               return "UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_8"; break;
        case UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_9:               return "UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_9"; break;
        case UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_10:              return "UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_10"; break;
        case UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_11:              return "UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_11"; break;
        case UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_12:              return "UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_12"; break;
            
        case UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_1:             return "UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_1"; break;
        case UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_2:             return "UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_2"; break;
        case UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_3:             return "UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_3"; break;
        case UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_4:             return "UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_4"; break;
        case UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_5:             return "UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_5"; break;
        case UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_6:             return "UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_6"; break;
        case UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_7:             return "UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_7"; break;
        case UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_8:             return "UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_8"; break;
        case UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_9:             return "UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_9"; break;
        case UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_10:            return "UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_10"; break;
        case UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_11:            return "UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_11"; break;
        case UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_12:            return "UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_12"; break;
            
        case UNIT_2_CHORD_PLAYER_CHORDPLAYER_SCALE:                 return "UNIT_2_CHORD_PLAYER_CHORDPLAYER_SCALE"; break;
            
        // chordplayer 3
        case UNIT_3_CHORD_PLAYER_SHORTCUT_IS_ACTIVE:                return "UNIT_3_CHORD_PLAYER_SHORTCUT_IS_ACTIVE"; break;
        case UNIT_3_CHORD_PLAYER_SHORTCUT_MUTE:                     return "UNIT_3_CHORD_PLAYER_SHORTCUT_MUTE"; break;
        case UNIT_3_CHORD_PLAYER_SHORTCUT_PLAY_AT_SAME_TIME:        return "UNIT_3_CHORD_PLAYER_SHORTCUT_PLAY_AT_SAME_TIME"; break;
        case UNIT_3_CHORD_PLAYER_INSTRUMENT_TYPE:                   return "UNIT_3_CHORD_PLAYER_INSTRUMENT_TYPE"; break;
        case UNIT_3_CHORD_PLAYER_WAVEFORM_TYPE:                     return "UNIT_3_CHORD_PLAYER_WAVEFORM_TYPE"; break;
        case UNIT_3_CHORD_PLAYER_ENV_AMPLITUDE:                     return "UNIT_3_CHORD_PLAYER_ENV_AMPLITUDE"; break;
        case UNIT_3_CHORD_PLAYER_ENV_ATTACK:                        return "UNIT_3_CHORD_PLAYER_ENV_ATTACK"; break;
        case UNIT_3_CHORD_PLAYER_ENV_SUSTAIN:                       return "UNIT_3_CHORD_PLAYER_ENV_SUSTAIN"; break;
        case UNIT_3_CHORD_PLAYER_ENV_DECAY:                         return "UNIT_3_CHORD_PLAYER_ENV_DECAY"; break;
        case UNIT_3_CHORD_PLAYER_ENV_RELEASE:                       return "UNIT_3_CHORD_PLAYER_ENV_RELEASE"; break;
        case UNIT_3_CHORD_PLAYER_NUM_REPEATS:                       return "UNIT_3_CHORD_PLAYER_NUM_REPEATS"; break;
        case UNIT_3_CHORD_PLAYER_NUM_PAUSE:                         return "UNIT_3_CHORD_PLAYER_NUM_PAUSE"; break;
        case UNIT_3_CHORD_PLAYER_NUM_DURATION:                      return "UNIT_3_CHORD_PLAYER_NUM_DURATION"; break;
        case UNIT_3_CHORD_PLAYER_CHORD_SOURCE:                      return "UNIT_3_CHORD_PLAYER_CHORD_SOURCE"; break;
        case UNIT_3_CHORD_PLAYER_KEYNOTE:                           return "UNIT_3_CHORD_PLAYER_KEYNOTE"; break;
        case UNIT_3_CHORD_PLAYER_OCTAVE:                            return "UNIT_3_CHORD_PLAYER_OCTAVE"; break;
        case UNIT_3_CHORD_PLAYER_CHORD_TYPE:                        return "UNIT_3_CHORD_PLAYER_CHORD_TYPE"; break;
        case UNIT_3_CHORD_PLAYER_ADD_ONS:                           return "UNIT_3_CHORD_PLAYER_ADD_ONS"; break;
        case UNIT_3_CHORD_PLAYER_INSERT_FREQUENCY:                  return "UNIT_3_CHORD_PLAYER_INSERT_FREQUENCY"; break;
        case UNIT_3_CHORD_PLAYER_MANIPULATE_CHOSEN_FREQUENCY:       return "UNIT_3_CHORD_PLAYER_MANIPULATE_CHOSEN_FREQUENCY"; break;
        case UNIT_3_CHORD_PLAYER_MULTIPLY_OR_DIVISION:              return "UNIT_3_CHORD_PLAYER_MULTIPLY_OR_DIVISION"; break;
        case UNIT_3_CHORD_PLAYER_MULTIPLY_VALUE:                    return "UNIT_3_CHORD_PLAYER_MULTIPLY_VALUE"; break;
        case UNIT_3_CHORD_PLAYER_DIVISION_VALUE:                    return "UNIT_3_CHORD_PLAYER_DIVISION_VALUE"; break;
        case UNIT_3_CHORD_PLAYER_CUSTOM_CHORD:                      return "UNIT_3_CHORD_PLAYER_CUSTOM_CHORD"; break;
            
        case UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_1:             return "UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_1"; break;
        case UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_2:             return "UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_2"; break;
        case UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_3:             return "UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_3"; break;
        case UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_4:             return "UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_4"; break;
        case UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_5:             return "UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_5"; break;
        case UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_6:             return "UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_6"; break;
        case UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_7:             return "UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_7"; break;
        case UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_8:             return "UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_8"; break;
        case UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_9:             return "UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_9"; break;
        case UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_10:            return "UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_10"; break;
        case UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_11:            return "UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_11"; break;
        case UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_12:            return "UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_12"; break;
            
        case UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_1:               return "UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_1"; break;
        case UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_2:               return "UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_2"; break;
        case UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_3:               return "UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_3"; break;
        case UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_4:               return "UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_4"; break;
        case UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_5:               return "UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_5"; break;
        case UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_6:               return "UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_6"; break;
        case UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_7:               return "UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_7"; break;
        case UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_8:               return "UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_8"; break;
        case UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_9:               return "UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_9"; break;
        case UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_10:              return "UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_10"; break;
        case UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_11:              return "UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_11"; break;
        case UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_12:              return "UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_12"; break;
            
        case UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_1:             return "UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_1"; break;
        case UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_2:             return "UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_2"; break;
        case UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_3:             return "UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_3"; break;
        case UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_4:             return "UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_4"; break;
        case UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_5:             return "UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_5"; break;
        case UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_6:             return "UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_6"; break;
        case UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_7:             return "UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_7"; break;
        case UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_8:             return "UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_8"; break;
        case UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_9:             return "UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_9"; break;
        case UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_10:            return "UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_10"; break;
        case UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_11:            return "UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_11"; break;
        case UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_12:            return "UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_12"; break;
            
        case UNIT_3_CHORD_PLAYER_CHORDPLAYER_SCALE:                 return "UNIT_3_CHORD_PLAYER_CHORDPLAYER_SCALE"; break;

        default: return ""; break;
    }
}

void ProjectManager::initDefaultLissajousParameters()
{

    // set default properties
    
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_PROCESSOR_TYPE),                                (int)0,        nullptr); // 0 = Freq, 1 = chord, 2 = fileplayer
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_PROCESSOR_TYPE),                                (int)0,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_PROCESSOR_TYPE),                                (int)0,        nullptr);

    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_FREE_FLOW),                                     (bool)false,   nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_FREE_FLOW),                                     (bool)false,   nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_FREE_FLOW),                                     (bool)false,   nullptr);
    
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_PHASE),                                         (double)0.f,   nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_PHASE),                                         (double)0.f,   nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_PHASE),                                         (double)0.f,   nullptr);

    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(AMPLITUDE_X),                                          (double)100.f,   nullptr); // could be -100..100% to shift
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(AMPLITUDE_Y),                                          (double)100.f,   nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(AMPLITUDE_Z),                                          (double)100.f,   nullptr);


    // Unit 1 Freq Player

    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_FREQUENCY_PLAYER_SHORTCUT_MUTE),                (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_FREQUENCY_PLAYER_SHORTCUT_PLAY_AT_SAME_TIME),   (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_FREQUENCY_PLAYER_FREQ_SOURCE),                  (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_FREQUENCY_PLAYER_CHOOSE_FREQ),                  (double)432.0,      nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_FREQUENCY_PLAYER_RANGE_MIN),                    (double)144.0,      nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_FREQUENCY_PLAYER_RANGE_MAX),                    (double)432.0,      nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_FREQUENCY_PLAYER_RANGE_LENGTH),                 (float)1000.0,      nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_FREQUENCY_PLAYER_RANGE_LOG),                    (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_FREQUENCY_PLAYER_MANIPULATE_CHOSEN_FREQUENCY),  (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_FREQUENCY_PLAYER_MULTIPLY_OR_DIVISION),         (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_FREQUENCY_PLAYER_MULTIPLY_VALUE),               (double)1.0,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_FREQUENCY_PLAYER_DIVISION_VALUE),               (double)1.0,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_FREQUENCY_PLAYER_WAVEFORM_TYPE),                (int)1,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_FREQUENCY_PLAYER_AMPLITUDE),                    (double)100.0,      nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_FREQUENCY_PLAYER_ATTACK),                       (double)10.0,       nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_FREQUENCY_PLAYER_SUSTAIN),                      (double)100.0,      nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_FREQUENCY_PLAYER_DECAY),                        (double)1.0,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_FREQUENCY_PLAYER_RELEASE),                      (double)300.0,      nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_FREQUENCY_PLAYER_NUM_REPEATS),                  (int)1,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_FREQUENCY_PLAYER_NUM_PAUSE),                    (int)1000,          nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_FREQUENCY_PLAYER_NUM_DURATION),                 (int)300,           nullptr);


    // Unit 2 Freq Player

    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_FREQUENCY_PLAYER_SHORTCUT_MUTE),                (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_FREQUENCY_PLAYER_SHORTCUT_PLAY_AT_SAME_TIME),   (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_FREQUENCY_PLAYER_FREQ_SOURCE),                  (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_FREQUENCY_PLAYER_CHOOSE_FREQ),                  (double)432.0,      nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_FREQUENCY_PLAYER_RANGE_MIN),                    (double)144.0,      nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_FREQUENCY_PLAYER_RANGE_MAX),                    (double)432.0,      nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_FREQUENCY_PLAYER_RANGE_LENGTH),                 (float)1000.0,      nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_FREQUENCY_PLAYER_RANGE_LOG),                    (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_FREQUENCY_PLAYER_MANIPULATE_CHOSEN_FREQUENCY),  (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_FREQUENCY_PLAYER_MULTIPLY_OR_DIVISION),         (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_FREQUENCY_PLAYER_MULTIPLY_VALUE),               (double)1.0,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_FREQUENCY_PLAYER_DIVISION_VALUE),               (double)1.0,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_FREQUENCY_PLAYER_WAVEFORM_TYPE),                (int)1,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_FREQUENCY_PLAYER_AMPLITUDE),                    (double)100.0,      nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_FREQUENCY_PLAYER_ATTACK),                       (double)10.0,       nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_FREQUENCY_PLAYER_SUSTAIN),                      (double)100.0,      nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_FREQUENCY_PLAYER_DECAY),                        (double)1.0,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_FREQUENCY_PLAYER_RELEASE),                      (double)300.0,      nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_FREQUENCY_PLAYER_NUM_REPEATS),                  (int)1,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_FREQUENCY_PLAYER_NUM_PAUSE),                    (int)1000,          nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_FREQUENCY_PLAYER_NUM_DURATION),                 (int)300,           nullptr);

    // Unit 2 Freq Player

    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_FREQUENCY_PLAYER_SHORTCUT_MUTE),                (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_FREQUENCY_PLAYER_SHORTCUT_PLAY_AT_SAME_TIME),   (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_FREQUENCY_PLAYER_FREQ_SOURCE),                  (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_FREQUENCY_PLAYER_CHOOSE_FREQ),                  (double)432.0,      nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_FREQUENCY_PLAYER_RANGE_MIN),                    (double)144.0,      nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_FREQUENCY_PLAYER_RANGE_MAX),                    (double)432.0,      nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_FREQUENCY_PLAYER_RANGE_LENGTH),                 (float)1000.0,      nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_FREQUENCY_PLAYER_RANGE_LOG),                    (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_FREQUENCY_PLAYER_MANIPULATE_CHOSEN_FREQUENCY),  (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_FREQUENCY_PLAYER_MULTIPLY_OR_DIVISION),         (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_FREQUENCY_PLAYER_MULTIPLY_VALUE),               (double)1.0,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_FREQUENCY_PLAYER_DIVISION_VALUE),               (double)1.0,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_FREQUENCY_PLAYER_WAVEFORM_TYPE),                (int)1,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_FREQUENCY_PLAYER_AMPLITUDE),                    (double)100.0,      nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_FREQUENCY_PLAYER_ATTACK),                       (double)10.0,       nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_FREQUENCY_PLAYER_SUSTAIN),                      (double)100.0,      nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_FREQUENCY_PLAYER_DECAY),                        (double)1.0,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_FREQUENCY_PLAYER_RELEASE),                      (double)300.0,      nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_FREQUENCY_PLAYER_NUM_REPEATS),                  (int)1,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_FREQUENCY_PLAYER_NUM_PAUSE),                    (int)1000,          nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_FREQUENCY_PLAYER_NUM_DURATION),                 (int)300,           nullptr);

    // Init 1 ChordPlayer Player

    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_SHORTCUT_IS_ACTIVE),                        (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_SHORTCUT_MUTE),                             (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_SHORTCUT_PLAY_AT_SAME_TIME),                (bool)false,        nullptr);
    
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_WAVEFORM_TYPE),                             (int)1,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_INSTRUMENT_TYPE),                           (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_ENV_AMPLITUDE),                             (float)100.0,       nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_ENV_ATTACK),                                (float)10,          nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_ENV_SUSTAIN),                               (double)100.0,      nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_ENV_DECAY),                                 (double)0.5,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_ENV_RELEASE),                               (double)300,        nullptr);
    
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_NUM_REPEATS),                               (int)1,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_NUM_PAUSE),                                 (double)1000,       nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_NUM_DURATION),                              (double)300,        nullptr);
    
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_CHORD_SOURCE),                              (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_KEYNOTE),                                   (int)KEYNOTES::KEY_C, nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_OCTAVE),                                    (int)4,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_CHORD_TYPE),                                (int)CHORD_TYPES::Major, nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_ADD_ONS),                                   (int)AddOn_NONE,    nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_INSERT_FREQUENCY),                          (double)432.0,      nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_MANIPULATE_CHOSEN_FREQUENCY),               (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_MULTIPLY_OR_DIVISION),                      (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_MULTIPLY_VALUE),                            (double)1.0,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_DIVISION_VALUE),                            (double)1.0,        nullptr);
    
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_CUSTOM_CHORD),                              (bool)false,        nullptr);
    
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_1),                     (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_2),                     (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_3),                     (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_4),                     (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_5),                     (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_6),                     (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_7),                     (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_8),                     (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_9),                     (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_10),                    (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_11),                    (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_12),                    (bool)false,        nullptr);
    
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_1),                       (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_2),                       (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_3),                       (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_4),                       (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_5),                       (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_6),                       (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_7),                       (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_8),                       (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_9),                       (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_10),                      (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_11),                      (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_12),                      (int)0,             nullptr);
    
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_1),                     (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_2),                     (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_3),                     (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_4),                     (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_5),                     (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_6),                     (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_7),                     (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_8),                     (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_9),                     (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_10),                    (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_11),                    (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_12),                    (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_1_CHORD_PLAYER_CHORDPLAYER_SCALE),                         (SCALES)CHROMATIC_PYTHAGOREAN,             nullptr);

    // Init 2 ChordPlayer Player
    
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_SHORTCUT_IS_ACTIVE),                        (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_SHORTCUT_MUTE),                             (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_SHORTCUT_PLAY_AT_SAME_TIME),                (bool)false,        nullptr);

    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_WAVEFORM_TYPE),                             (int)1,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_INSTRUMENT_TYPE),                           (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_ENV_AMPLITUDE),                             (float)100.0,       nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_ENV_ATTACK),                                (float)10,          nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_ENV_SUSTAIN),                               (double)100.0,      nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_ENV_DECAY),                                 (double)0.5,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_ENV_RELEASE),                               (double)300,        nullptr);
    
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_NUM_REPEATS),                               (int)1,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_NUM_PAUSE),                                 (double)1000,       nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_NUM_DURATION),                              (double)300,        nullptr);
    
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_CHORD_SOURCE),                              (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_KEYNOTE),                                   (int)KEYNOTES::KEY_C, nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_OCTAVE),                                    (int)4,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_CHORD_TYPE),                                (int)CHORD_TYPES::Major, nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_ADD_ONS),                                   (int)AddOn_NONE,    nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_INSERT_FREQUENCY),                          (double)432.0,      nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_MANIPULATE_CHOSEN_FREQUENCY),               (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_MULTIPLY_OR_DIVISION),                      (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_MULTIPLY_VALUE),                            (double)1.0,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_DIVISION_VALUE),                            (double)1.0,        nullptr);
    
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_CUSTOM_CHORD),                              (bool)false,        nullptr);
    
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_1),                     (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_2),                     (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_3),                     (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_4),                     (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_5),                     (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_6),                     (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_7),                     (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_8),                     (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_9),                     (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_10),                    (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_11),                    (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_12),                    (bool)false,        nullptr);
    
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_1),                       (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_2),                       (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_3),                       (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_4),                       (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_5),                       (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_6),                       (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_7),                       (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_8),                       (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_9),                       (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_10),                      (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_11),                      (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_12),                      (int)0,             nullptr);
    
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_1),                     (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_2),                     (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_3),                     (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_4),                     (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_5),                     (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_6),                     (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_7),                     (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_8),                     (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_9),                     (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_10),                    (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_11),                    (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_12),                    (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_2_CHORD_PLAYER_CHORDPLAYER_SCALE),                         (SCALES)CHROMATIC_PYTHAGOREAN,             nullptr);

    // Init 3 ChordPlayer Player
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_SHORTCUT_IS_ACTIVE),                        (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_SHORTCUT_MUTE),                             (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_SHORTCUT_PLAY_AT_SAME_TIME),                (bool)false,        nullptr);

    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_WAVEFORM_TYPE),                             (int)1,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_INSTRUMENT_TYPE),                           (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_ENV_AMPLITUDE),                             (float)100.0,       nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_ENV_ATTACK),                                (float)10,          nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_ENV_SUSTAIN),                               (double)100.0,      nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_ENV_DECAY),                                 (double)0.5,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_ENV_RELEASE),                               (double)300,        nullptr);
    
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_NUM_REPEATS),                               (int)1,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_NUM_PAUSE),                                 (double)1000,       nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_NUM_DURATION),                              (double)300,        nullptr);
    
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_CHORD_SOURCE),                              (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_KEYNOTE),                                   (int)KEYNOTES::KEY_C, nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_OCTAVE),                                    (int)4,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_CHORD_TYPE),                                (int)CHORD_TYPES::Major, nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_ADD_ONS),                                   (int)AddOn_NONE,    nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_INSERT_FREQUENCY),                          (double)432.0,      nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_MANIPULATE_CHOSEN_FREQUENCY),               (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_MULTIPLY_OR_DIVISION),                      (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_MULTIPLY_VALUE),                            (double)1.0,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_DIVISION_VALUE),                            (double)1.0,        nullptr);
    
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_CUSTOM_CHORD),                              (bool)false,        nullptr);

    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_1),                     (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_2),                     (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_3),                     (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_4),                     (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_5),                     (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_6),                     (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_7),                     (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_8),                     (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_9),                     (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_10),                    (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_11),                    (bool)false,        nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_12),                    (bool)false,        nullptr);
    
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_1),                       (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_2),                       (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_3),                       (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_4),                       (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_5),                       (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_6),                       (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_7),                       (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_8),                       (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_9),                       (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_10),                      (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_11),                      (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_12),                      (int)0,             nullptr);
    
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_1),                     (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_2),                     (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_3),                     (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_4),                     (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_5),                     (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_6),                     (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_7),                     (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_8),                     (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_9),                     (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_10),                    (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_11),                    (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_12),                    (int)0,             nullptr);
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(UNIT_3_CHORD_PLAYER_CHORDPLAYER_SCALE),                         (SCALES)CHROMATIC_PYTHAGOREAN,  nullptr);
    

    for (int index = 0; index < TOTAL_NUM_LISSAJOUS_CURVE_PARAMS; index++)
    {
        lissajousProcessor->setParameter(index, lissajousParameters->getProperty(getIdentifierForLissajousParameterIndex(index)));
    }
}

// called from UI to change individual parameters
void ProjectManager::setLissajousParameter(int index, var newValue)
{
    lissajousParameters->setProperty(getIdentifierForLissajousParameterIndex(index), newValue, nullptr);
    
    lissajousProcessor->setParameter(index, newValue);
    // needs to call ui fuction
    uiListeners.call(&UIListener::updateLissajousCurveUIParameter, index);
}

// called from GUI to update controls when neccessary
var ProjectManager::getLissajousParameter(int index)
{
    return lissajousParameters->getProperty(getIdentifierForLissajousParameterIndex(index));
}

//===============================================================================
#pragma mark Chord Player Parameters
//===============================================================================

String ProjectManager::getIdentifierForChordPlayerParameterIndex(int index)
{
    switch (index) {
        case SHORTCUT_IS_ACTIVE:                return "SHORTCUT_IS_ACTIVE"; break;
        case SHORTCUT_MUTE:                     return "SHORTCUT_MUTE"; break;
        case SHORTCUT_PLAY_AT_SAME_TIME:        return "SHORTCUT_PLAY_AT_SAME_TIME"; break;
        case INSTRUMENT_TYPE:                   return "INSTRUMENT_TYPE"; break;
        case WAVEFORM_TYPE:                     return "WAVEFORM_TYPE"; break;
        case ENV_AMPLITUDE:                     return "ENV_AMPLITUDE"; break;
        case ENV_ATTACK:                        return "ENV_ATTACK"; break;
        case ENV_SUSTAIN:                       return "ENV_SUSTAIN"; break;
        case ENV_DECAY:                         return "ENV_DECAY"; break;
        case ENV_RELEASE:                       return "ENV_RELEASE"; break;
        case NUM_REPEATS:                       return "NUM_REPEATS"; break;
        case NUM_PAUSE:                         return "NUM_PAUSE"; break;
        case NUM_DURATION:                      return "NUM_DURATION"; break;
        case CHORD_SOURCE:                      return "CHORD_SOURCE"; break;
        case KEYNOTE:                           return "KEYNOTE"; break;
        case OCTAVE:                            return "OCTAVE"; break;
        case CHORD_TYPE:                        return "CHORD_TYPE"; break;
        case ADD_ONS:                           return "ADD_ONS"; break;
        case INSERT_FREQUENCY:                  return "INSERT_FREQUENCY"; break;
        case MANIPULATE_CHOSEN_FREQUENCY:       return "MANIPULATE_CHOSEN_FREQUENCY"; break;
        case MULTIPLY_OR_DIVISION:              return "MULTIPLY_OR_DIVISION"; break;
        case MULTIPLY_VALUE:                    return "MULTIPLY_VALUE"; break;
        case DIVISION_VALUE:                    return "DIVISION_VALUE"; break;
        case CUSTOM_CHORD:                      return "CUSTOM_CHORD"; break;
            
        case CUSTOM_CHORD_ACTIVE_1:             return "CUSTOM_CHORD_ACTIVE_1"; break;
        case CUSTOM_CHORD_ACTIVE_2:             return "CUSTOM_CHORD_ACTIVE_2"; break;
        case CUSTOM_CHORD_ACTIVE_3:             return "CUSTOM_CHORD_ACTIVE_3"; break;
        case CUSTOM_CHORD_ACTIVE_4:             return "CUSTOM_CHORD_ACTIVE_4"; break;
        case CUSTOM_CHORD_ACTIVE_5:             return "CUSTOM_CHORD_ACTIVE_5"; break;
        case CUSTOM_CHORD_ACTIVE_6:             return "CUSTOM_CHORD_ACTIVE_6"; break;
        case CUSTOM_CHORD_ACTIVE_7:             return "CUSTOM_CHORD_ACTIVE_7"; break;
        case CUSTOM_CHORD_ACTIVE_8:             return "CUSTOM_CHORD_ACTIVE_8"; break;
        case CUSTOM_CHORD_ACTIVE_9:             return "CUSTOM_CHORD_ACTIVE_9"; break;
        case CUSTOM_CHORD_ACTIVE_10:            return "CUSTOM_CHORD_ACTIVE_10"; break;
        case CUSTOM_CHORD_ACTIVE_11:            return "CUSTOM_CHORD_ACTIVE_11"; break;
        case CUSTOM_CHORD_ACTIVE_12:            return "CUSTOM_CHORD_ACTIVE_12"; break;
            
        case CUSTOM_CHORD_NOTE_1:               return "CUSTOM_CHORD_NOTE_1"; break;
        case CUSTOM_CHORD_NOTE_2:               return "CUSTOM_CHORD_NOTE_2"; break;
        case CUSTOM_CHORD_NOTE_3:               return "CUSTOM_CHORD_NOTE_3"; break;
        case CUSTOM_CHORD_NOTE_4:               return "CUSTOM_CHORD_NOTE_4"; break;
        case CUSTOM_CHORD_NOTE_5:               return "CUSTOM_CHORD_NOTE_5"; break;
        case CUSTOM_CHORD_NOTE_6:               return "CUSTOM_CHORD_NOTE_6"; break;
        case CUSTOM_CHORD_NOTE_7:               return "CUSTOM_CHORD_NOTE_7"; break;
        case CUSTOM_CHORD_NOTE_8:               return "CUSTOM_CHORD_NOTE_8"; break;
        case CUSTOM_CHORD_NOTE_9:               return "CUSTOM_CHORD_NOTE_9"; break;
        case CUSTOM_CHORD_NOTE_10:              return "CUSTOM_CHORD_NOTE_10"; break;
        case CUSTOM_CHORD_NOTE_11:              return "CUSTOM_CHORD_NOTE_11"; break;
        case CUSTOM_CHORD_NOTE_12:              return "CUSTOM_CHORD_NOTE_12"; break;
            
        case CUSTOM_CHORD_OCTAVE_1:             return "CUSTOM_CHORD_OCTAVE_1"; break;
        case CUSTOM_CHORD_OCTAVE_2:             return "CUSTOM_CHORD_OCTAVE_2"; break;
        case CUSTOM_CHORD_OCTAVE_3:             return "CUSTOM_CHORD_OCTAVE_3"; break;
        case CUSTOM_CHORD_OCTAVE_4:             return "CUSTOM_CHORD_OCTAVE_4"; break;
        case CUSTOM_CHORD_OCTAVE_5:             return "CUSTOM_CHORD_OCTAVE_5"; break;
        case CUSTOM_CHORD_OCTAVE_6:             return "CUSTOM_CHORD_OCTAVE_6"; break;
        case CUSTOM_CHORD_OCTAVE_7:             return "CUSTOM_CHORD_OCTAVE_7"; break;
        case CUSTOM_CHORD_OCTAVE_8:             return "CUSTOM_CHORD_OCTAVE_8"; break;
        case CUSTOM_CHORD_OCTAVE_9:             return "CUSTOM_CHORD_OCTAVE_9"; break;
        case CUSTOM_CHORD_OCTAVE_10:            return "CUSTOM_CHORD_OCTAVE_10"; break;
        case CUSTOM_CHORD_OCTAVE_11:            return "CUSTOM_CHORD_OCTAVE_11"; break;
        case CUSTOM_CHORD_OCTAVE_12:            return "CUSTOM_CHORD_OCTAVE_12"; break;
            
        case CHORD_PLAYER_OUTPUT_SELECTION:     return "CHORD_PLAYER_OUTPUT_SELECTION"; break;
            
        case CHORDPLAYER_SCALE:                 return "CHORDPLAYER_SCALE"; break;

        case CUSTOM_CHORD_AMPLITUDE_1:          return "CUSTOM_CHORD_AMPLITUDE_1"; break;
        case CUSTOM_CHORD_AMPLITUDE_2:          return "CUSTOM_CHORD_AMPLITUDE_2"; break;
        case CUSTOM_CHORD_AMPLITUDE_3:          return "CUSTOM_CHORD_AMPLITUDE_3"; break;
        case CUSTOM_CHORD_AMPLITUDE_4:          return "CUSTOM_CHORD_AMPLITUDE_4"; break;
        case CUSTOM_CHORD_AMPLITUDE_5:          return "CUSTOM_CHORD_AMPLITUDE_5"; break;
        case CUSTOM_CHORD_AMPLITUDE_6:          return "CUSTOM_CHORD_AMPLITUDE_6"; break;
        case CUSTOM_CHORD_AMPLITUDE_7:          return "CUSTOM_CHORD_AMPLITUDE_7"; break;
        case CUSTOM_CHORD_AMPLITUDE_8:          return "CUSTOM_CHORD_AMPLITUDE_8"; break;
        case CUSTOM_CHORD_AMPLITUDE_9:          return "CUSTOM_CHORD_AMPLITUDE_9"; break;
        case CUSTOM_CHORD_AMPLITUDE_10:         return "CUSTOM_CHORD_AMPLITUDE_10"; break;
        case CUSTOM_CHORD_AMPLITUDE_11:         return "CUSTOM_CHORD_AMPLITUDE_11"; break;
        case CUSTOM_CHORD_AMPLITUDE_12:         return "CUSTOM_CHORD_AMPLITUDE_12"; break;

        case CHORD_PLAYER_NUM_REPEATS:          return "CHORD_PLAYER_NUM_REPEATS"; break;
        case CHORD_PLAYER_NUM_PAUSE:            return "CHORD_PLAYER_NUM_PAUSE"; break;

        default: return ""; break;
    }
}

void ProjectManager::initDefaultChordPlayerParameters()
{
    for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
    {
        // set default properties
        
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(SHORTCUT_IS_ACTIVE),                        (bool)false,        nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(SHORTCUT_MUTE),                             (bool)false,        nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(SHORTCUT_PLAY_AT_SAME_TIME),                (bool)false,        nullptr);
        
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(INSTRUMENT_TYPE),                           (int)-1,             nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(WAVEFORM_TYPE),                             (int)1,             nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(ENV_AMPLITUDE),                             (float)100.0,         nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(ENV_ATTACK),                                (float)10,          nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(ENV_SUSTAIN),                               (double)100.0,      nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(ENV_DECAY),                                 (double)0.5,        nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(ENV_RELEASE),                               (double)300,        nullptr);
        
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(NUM_REPEATS),                               (int)1,             nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(NUM_PAUSE),                                 (double)1000,       nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(NUM_DURATION),                              (double)300,        nullptr);
        
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CHORD_SOURCE),                              (bool)false,        nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(KEYNOTE),                                   (int)1,             nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(OCTAVE),                                    (float)4,           nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CHORD_TYPE),                                (int)1,             nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(ADD_ONS),                                   (int)AddOn_NONE,             nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(INSERT_FREQUENCY),                          (double)432.0,      nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(MANIPULATE_CHOSEN_FREQUENCY),               (bool)false,        nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(MULTIPLY_OR_DIVISION),                      (bool)false,        nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(MULTIPLY_VALUE),                            (double)1.0,        nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(DIVISION_VALUE),                            (double)1.0,        nullptr);
        
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD),                              (bool)false,        nullptr);
        
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_ACTIVE_1),                     (int)0,        nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_ACTIVE_2),                     (int)0,        nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_ACTIVE_3),                     (int)0,        nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_ACTIVE_4),                     (int)0,        nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_ACTIVE_5),                     (int)0,        nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_ACTIVE_6),                     (int)0,        nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_ACTIVE_7),                     (int)0,        nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_ACTIVE_8),                     (int)0,        nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_ACTIVE_9),                     (int)0,        nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_ACTIVE_10),                    (int)0,        nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_ACTIVE_11),                    (int)0,        nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_ACTIVE_12),                    (int)0,        nullptr);
        
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_NOTE_1),                       (int)0,             nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_NOTE_2),                       (int)0,             nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_NOTE_3),                       (int)0,             nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_NOTE_4),                       (int)0,             nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_NOTE_5),                       (int)0,             nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_NOTE_6),                       (int)0,             nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_NOTE_7),                       (int)0,             nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_NOTE_8),                       (int)0,             nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_NOTE_9),                       (int)0,             nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_NOTE_10),                      (int)0,             nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_NOTE_11),                      (int)0,             nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_NOTE_12),                      (int)0,             nullptr);
        
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_OCTAVE_1),                     (int)0,             nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_OCTAVE_2),                     (int)0,             nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_OCTAVE_3),                     (int)0,             nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_OCTAVE_4),                     (int)0,             nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_OCTAVE_5),                     (int)0,             nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_OCTAVE_6),                     (int)0,             nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_OCTAVE_7),                     (int)0,             nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_OCTAVE_8),                     (int)0,             nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_OCTAVE_9),                     (int)0,             nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_OCTAVE_10),                    (int)0,             nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_OCTAVE_11),                    (int)0,             nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_OCTAVE_12),                    (int)0,             nullptr);

        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_AMPLITUDE_1),                 (float)1.0,         nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_AMPLITUDE_2),                 (float)1.0,         nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_AMPLITUDE_3),                 (float)1.0,         nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_AMPLITUDE_4),                 (float)1.0,         nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_AMPLITUDE_5),                 (float)1.0,         nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_AMPLITUDE_6),                 (float)1.0,         nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_AMPLITUDE_7),                 (float)1.0,         nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_AMPLITUDE_8),                 (float)1.0,         nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_AMPLITUDE_9),                 (float)1.0,         nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_AMPLITUDE_10),                (float)1.0,         nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_AMPLITUDE_11),                (float)1.0,         nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_AMPLITUDE_12),                (float)1.0,         nullptr);

        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CHORD_PLAYER_NUM_REPEATS),                 (int)1,             nullptr);
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CHORD_PLAYER_NUM_PAUSE),                   (double)1000,       nullptr);

        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CHORDPLAYER_SCALE),                         (SCALES)CHROMATIC_PYTHAGOREAN, nullptr);
        
        chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CHORD_PLAYER_OUTPUT_SELECTION),                         (AUDIO_OUTPUTS)AUDIO_OUTPUTS::MONO_1, nullptr);
        
        
        // sync to chord player processor / synths
        for (int index = 0; index < TOTAL_NUM_CHORD_PLAYER_SHORTCUT_PARAMS; index++)
        {
            String identifier = getIdentifierForChordPlayerParameterIndex(index);
            if (identifier.isEmpty())
                continue;  // Skip unhandled indices

            if (index == SHORTCUT_IS_ACTIVE)
            {
                chordPlayerProcessor->setActiveShortcutSynth(i, (bool)chordPlayerParameters[i]->getProperty(identifier));
            }
            else
            {
                chordPlayerProcessor->setParameter(i, index, chordPlayerParameters[i]->getProperty(identifier));
            }
        }
    }
    
//    syncChordPlayerGUI();
    
}

void ProjectManager::initDefaultChordPlayerParametersForShortcut(int shortcutRef)
{
    // set default properties
    int i = shortcutRef;

    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(SHORTCUT_MUTE),                             (bool)false,        nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(SHORTCUT_PLAY_AT_SAME_TIME),                (bool)false,        nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(INSTRUMENT_TYPE),                           (int)-1,             nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(WAVEFORM_TYPE),                             (int)1,             nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(ENV_AMPLITUDE),                             (float)100.0,         nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(ENV_ATTACK),                                (float)10,          nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(ENV_SUSTAIN),                               (double)100.0,      nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(ENV_DECAY),                                 (double)0.5,        nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(ENV_RELEASE),                               (double)300,        nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(NUM_REPEATS),                               (int)1,             nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(NUM_PAUSE),                                 (double)1000,       nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(NUM_DURATION),                              (double)300,        nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CHORD_SOURCE),                              (bool)false,        nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(KEYNOTE),                                   (int)1,             nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(OCTAVE),                                    (float)4,           nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CHORD_TYPE),                                (int)1,             nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(ADD_ONS),                                   (int)AddOn_NONE,             nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(INSERT_FREQUENCY),                          (double)432.0,      nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(MANIPULATE_CHOSEN_FREQUENCY),               (bool)false,        nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(MULTIPLY_OR_DIVISION),                      (bool)false,        nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(MULTIPLY_VALUE),                            (double)1.0,        nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(DIVISION_VALUE),                            (double)1.0,        nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD),                              (bool)false,        nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_ACTIVE_1),                     (int)0,        nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_ACTIVE_2),                     (int)0,        nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_ACTIVE_3),                     (int)0,        nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_ACTIVE_4),                     (int)0,        nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_ACTIVE_5),                     (int)0,        nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_ACTIVE_6),                     (int)0,        nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_ACTIVE_7),                     (int)0,        nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_ACTIVE_8),                     (int)0,        nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_ACTIVE_9),                     (int)0,        nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_ACTIVE_10),                    (int)0,        nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_ACTIVE_11),                    (int)0,        nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_ACTIVE_12),                    (int)0,        nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_NOTE_1),                       (int)1,             nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_NOTE_2),                       (int)1,             nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_NOTE_3),                       (int)1,             nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_NOTE_4),                       (int)1,             nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_NOTE_5),                       (int)1,             nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_NOTE_6),                       (int)1,             nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_NOTE_7),                       (int)1,             nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_NOTE_8),                       (int)1,             nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_NOTE_9),                       (int)1,             nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_NOTE_10),                      (int)1,             nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_NOTE_11),                      (int)1,             nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_NOTE_12),                      (int)1,             nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_OCTAVE_1),                     (int)1,             nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_OCTAVE_2),                     (int)1,             nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_OCTAVE_3),                     (int)1,             nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_OCTAVE_4),                     (int)1,             nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_OCTAVE_5),                     (int)1,             nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_OCTAVE_6),                     (int)1,             nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_OCTAVE_7),                     (int)1,             nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_OCTAVE_8),                     (int)1,             nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_OCTAVE_9),                     (int)1,             nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_OCTAVE_10),                    (int)1,             nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_OCTAVE_11),                    (int)1,             nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_OCTAVE_12),                    (int)1,             nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CHORDPLAYER_SCALE),                         (SCALES)getProjectSettingsParameter(DEFAULT_SCALE), nullptr);
    chordPlayerParameters[i]->setProperty(getIdentifierForChordPlayerParameterIndex(CHORD_PLAYER_OUTPUT_SELECTION),             (AUDIO_OUTPUTS)AUDIO_OUTPUTS::MONO_1, nullptr);

    // sync to chord player processor / synths
    for (int index = 0; index < TOTAL_NUM_CHORD_PLAYER_SHORTCUT_PARAMS; index++)
    {
        String identifier = getIdentifierForChordPlayerParameterIndex(index);
        if (identifier.isEmpty())
            continue;  // Skip unhandled indices

        if (index == SHORTCUT_IS_ACTIVE)
        {
            chordPlayerProcessor->setActiveShortcutSynth(i, (bool)chordPlayerParameters[i]->getProperty(identifier));
        }
        else
        {
            chordPlayerProcessor->setParameter(i, index, chordPlayerParameters[i]->getProperty(identifier));
        }
    }
}

void ProjectManager::reorderShortcuts()
{
    // called when a shortcut is deleted, will push all shortcuts adjusted to the left
    
    // first check which is active
    bool active[NUM_SHORTCUT_SYNTHS];
    
    for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
    {
        active[i] = chordPlayerParameters[i]->getProperty(getIdentifierForChordPlayerParameterIndex(SHORTCUT_IS_ACTIVE));
    }
    
    for(int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
    {
        // then shuffle the shortcuts
        if (!active[i])
        {
            for (int next = i+1; next < NUM_SHORTCUT_SYNTHS; next++)
            {
                copyShortcut(next, next-1);
            }
        }
    }
}

void ProjectManager::copyShortcut(int sourceShortcut, int destShortcut)
{
    // sync to chord player processor / synths
    for (int index = 0; index < TOTAL_NUM_CHORD_PLAYER_SHORTCUT_PARAMS; index++)
    {
        String identifier = getIdentifierForChordPlayerParameterIndex(index);
        if (identifier.isEmpty())
            continue;  // Skip unhandled indices

        var paramValueToCopy = chordPlayerParameters[sourceShortcut]->getProperty(identifier);
        setChordPlayerParameter(destShortcut, index, paramValueToCopy);
    }

    // default source Shortcut
    initDefaultChordPlayerParametersForShortcut(sourceShortcut);

    // set active to false
    setChordPlayerParameter(sourceShortcut, SHORTCUT_IS_ACTIVE, false);

    for (int index = 0; index < TOTAL_NUM_CHORD_PLAYER_SHORTCUT_PARAMS; index++)
    {
        String identifier = getIdentifierForChordPlayerParameterIndex(index);
        if (identifier.isEmpty())
            continue;  // Skip unhandled indices

        var paramValueToCopy = chordPlayerParameters[sourceShortcut]->getProperty(identifier);
        setChordPlayerParameter(sourceShortcut, index, paramValueToCopy);
    }
}

// called from UI to change individual parameters
void ProjectManager::setChordPlayerParameter(int synthRef, int index, var newValue)
{
    // store new value in paramater valuetree
    String identifier = getIdentifierForChordPlayerParameterIndex(index);
    if (identifier.isEmpty())
        return;  // Skip unhandled indices
    chordPlayerParameters[synthRef]->setProperty(identifier, newValue, nullptr);

    // check if active param // mmm
    if (index == SHORTCUT_IS_ACTIVE)
    {
        // this basiclly deletes the shortcut,
        chordPlayerProcessor->setActiveShortcutSynth(synthRef, (bool)chordPlayerParameters[synthRef]->getProperty(getIdentifierForChordPlayerParameterIndex(SHORTCUT_IS_ACTIVE)));
        chordPlayerProcessor->setParameter(synthRef, index, newValue);
        
        // so shortcut params should prob be reset to default in this case..
//        initDefaultChordPlayerParametersForShortcut(synthRef); // defaults all parameters EXCEPT SHORTCUT_ACTIVE, which is set above
    }
    else if (index == CUSTOM_CHORD)
    {
        // set to all 12 chord active, note and octave
        chordPlayerProcessor->setParameter(synthRef, index, newValue);
        
        bool should = newValue.operator bool();
        
        if (!should)
        {
            for (int i = 0; i < 12; i++)
            {
                chordPlayerParameters[synthRef]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_ACTIVE_1 + i), 0, nullptr);
                chordPlayerParameters[synthRef]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_NOTE_1 + i), 1, nullptr);
                chordPlayerParameters[synthRef]->setProperty(getIdentifierForChordPlayerParameterIndex(CUSTOM_CHORD_OCTAVE_1 + i), 1, nullptr);
                
                chordPlayerProcessor->setParameter(synthRef, CUSTOM_CHORD_ACTIVE_1 + i, 0);
                chordPlayerProcessor->setParameter(synthRef, CUSTOM_CHORD_NOTE_1 + i, 1);
                chordPlayerProcessor->setParameter(synthRef, CUSTOM_CHORD_OCTAVE_1 + i, 1);
            }
        }
    
    }
    else if (index >= SHORTCUT_MUTE
             && index <= CHORD_PLAYER_OUTPUT_SELECTION)
    {
        chordPlayerProcessor->setParameter(synthRef, index, newValue);
    }
    
    // needs to call ui fuction
    uiListeners.call(&UIListener::updateChordPlayerUIParameter, synthRef, index);
}

// called from GUI to update controls when neccessary
var ProjectManager::getChordPlayerParameter(int synthRef, int index)
{
    String identifier = getIdentifierForChordPlayerParameterIndex(index);
    if (identifier.isEmpty())
        return var();  // Return undefined for unhandled indices
    return chordPlayerParameters[synthRef]->getProperty(identifier);
}

void ProjectManager::syncChordPlayerContainerGUI(int synthRef)
{
    for (int shortcut = 0; shortcut < NUM_SHORTCUT_SYNTHS; shortcut++)
    {
        for (int i = 0; i < TOTAL_NUM_CHORD_PLAYER_SHORTCUT_PARAMS; i++)
        {
//            uiListeners.call(&UIListener::updateChordPlaye, shortcut, i);
        }
    }
}

void ProjectManager::syncChordPlayerSettingsGUI(int synthRef)
{
    // syncs popup settings view
}

//===============================================================================
#pragma mark Chord Scanner Parameters
//===============================================================================

String ProjectManager::getIdentifierForChordScannerParameterIndex(int index)
{
    switch (index) {
        case SPECTRUM_ANALYZER:                 return "SPECTRUM_ANALYZER"; break;
        case CHORD_SCANNER_MODE:                return "CHORD_SCANNER_MODE"; break;
        case CHORD_SCANNER_FREQUENCY_FROM:      return "CHORD_SCANNER_FREQUENCY_FROM"; break;
        case CHORD_SCANNER_FREQUENCY_TO:        return "CHORD_SCANNER_FREQUENCY_TO"; break;
        case CHORD_SCANNER_KEYNOTE_FROM:        return "CHORD_SCANNER_KEYNOTE_FROM"; break;
        case CHORD_SCANNER_KEYNOTE_TO:          return "CHORD_SCANNER_KEYNOTE_TO"; break;
        case CHORD_SCANNER_OCTAVE_FROM:         return "CHORD_SCANNER_OCTAVE_FROM"; break;
        case CHORD_SCANNER_OCTAVE_TO:           return "CHORD_SCANNER_OCTAVE_TO"; break;
        case CHORD_SCANNER_OCTAVE_EXTENDED:     return "CHORD_SCANNER_OCTAVE_EXTENDED"; break;
        case CHORD_SCANNER_WAVEFORM_TYPE:       return "CHORD_SCANNER_WAVEFORM_TYPE"; break;
        case CHORD_SCANNER_INSTRUMENT_TYPE:     return "CHORD_SCANNER_INSTRUMENT_TYPE"; break;
        case CHORD_SCANNER_ENV_AMPLITUDE:       return "CHORD_SCANNER_ENV_AMPLITUDE"; break;
        case CHORD_SCANNER_ENV_ATTACK:          return "CHORD_SCANNER_ENV_ATTACK"; break;
        case CHORD_SCANNER_ENV_SUSTAIN:         return "CHORD_SCANNER_ENV_SUSTAIN"; break;
        case CHORD_SCANNER_ENV_DECAY:           return "CHORD_SCANNER_ENV_DECAY"; break;
        case CHORD_SCANNER_ENV_RELEASE:         return "CHORD_SCANNER_ENV_RELEASE"; break;
        case CHORD_SCANNER_NUM_REPEATS:         return "CHORD_SCANNER_NUM_REPEATS"; break;
        case CHORD_SCANNER_NUM_PAUSE:           return "CHORD_SCANNER_NUM_PAUSE"; break;
        case CHORD_SCANNER_NUM_DURATION:        return "CHORD_SCANNER_NUM_DURATION"; break;
        case CHORD_SCANNER_OUTPUT_SELECTION:    return "CHORD_SCANNER_OUTPUT_SELECTION"; break;

        default: return ""; break;
    }
}

void ProjectManager::initDefaultChordScannerParameters()
{
    // set default properties
    chordScannerParameters->setProperty(getIdentifierForChordScannerParameterIndex(SPECTRUM_ANALYZER),                  (bool)true,         nullptr);
    chordScannerParameters->setProperty(getIdentifierForChordScannerParameterIndex(CHORD_SCANNER_MODE),                 (int)0,             nullptr);
    chordScannerParameters->setProperty(getIdentifierForChordScannerParameterIndex(CHORD_SCANNER_FREQUENCY_FROM),       (double)432.0,       nullptr);
    chordScannerParameters->setProperty(getIdentifierForChordScannerParameterIndex(CHORD_SCANNER_FREQUENCY_TO),         (double)440.0,       nullptr);
    chordScannerParameters->setProperty(getIdentifierForChordScannerParameterIndex(CHORD_SCANNER_KEYNOTE_FROM),         (int)KEYNOTES::KEY_C, nullptr);
    chordScannerParameters->setProperty(getIdentifierForChordScannerParameterIndex(CHORD_SCANNER_KEYNOTE_TO),           (int)KEYNOTES::KEY_B, nullptr);
    chordScannerParameters->setProperty(getIdentifierForChordScannerParameterIndex(CHORD_SCANNER_OCTAVE_FROM),          (int)0,             nullptr);
    chordScannerParameters->setProperty(getIdentifierForChordScannerParameterIndex(CHORD_SCANNER_OCTAVE_TO),            (int)10,             nullptr);
    chordScannerParameters->setProperty(getIdentifierForChordScannerParameterIndex(CHORD_SCANNER_OCTAVE_EXTENDED),      (bool)0,            nullptr);
    chordScannerParameters->setProperty(getIdentifierForChordScannerParameterIndex(CHORD_SCANNER_WAVEFORM_TYPE),        (int)0,             nullptr);
    chordScannerParameters->setProperty(getIdentifierForChordScannerParameterIndex(CHORD_SCANNER_INSTRUMENT_TYPE),      (int)1,             nullptr);
    chordScannerParameters->setProperty(getIdentifierForChordScannerParameterIndex(CHORD_SCANNER_ENV_AMPLITUDE),        (double)100.0,      nullptr);
    chordScannerParameters->setProperty(getIdentifierForChordScannerParameterIndex(CHORD_SCANNER_ENV_ATTACK),           (double)10.0,       nullptr);
    chordScannerParameters->setProperty(getIdentifierForChordScannerParameterIndex(CHORD_SCANNER_ENV_SUSTAIN),          (double)100.0,      nullptr);
    chordScannerParameters->setProperty(getIdentifierForChordScannerParameterIndex(CHORD_SCANNER_ENV_DECAY),            (double)0.5,        nullptr);
    chordScannerParameters->setProperty(getIdentifierForChordScannerParameterIndex(CHORD_SCANNER_ENV_RELEASE),          (double)300.0,      nullptr);
    chordScannerParameters->setProperty(getIdentifierForChordScannerParameterIndex(CHORD_SCANNER_NUM_REPEATS),          (int)1,          nullptr);
    chordScannerParameters->setProperty(getIdentifierForChordScannerParameterIndex(CHORD_SCANNER_NUM_PAUSE),            (int)1000,          nullptr);
    chordScannerParameters->setProperty(getIdentifierForChordScannerParameterIndex(CHORD_SCANNER_NUM_DURATION),         (int)300,           nullptr);
    chordScannerParameters->setProperty(getIdentifierForChordScannerParameterIndex(CHORD_SCANNER_OUTPUT_SELECTION),     (int)AUDIO_OUTPUTS::MONO_1,           nullptr);
    
    for (int i = 0; i < TOTAL_NUM_CHORD_SCANNER_PARAMS; i++)
    {
        chordScannerProcessor->setParameter(i, chordScannerParameters->getProperty(getIdentifierForChordScannerParameterIndex(i)));
    }
}

// called from UI to change individual parameters
void ProjectManager::setChordScannerParameter(int index, var newValue)
{
    // extended hack
    if (index == CHORD_SCANNER_OCTAVE_EXTENDED)
    {
        bool ex = newValue.operator bool();
        
        if (!ex)
        {
            // need to bounds check upper octave
            int octaveTo = chordScannerParameters->getProperty(getIdentifierForChordScannerParameterIndex(CHORD_SCANNER_OCTAVE_TO));
            
            if (octaveTo > 10)
            {
                octaveTo = 10;
                
                chordScannerParameters->setProperty(getIdentifierForChordScannerParameterIndex(CHORD_SCANNER_OCTAVE_TO), octaveTo, nullptr);
                
                chordScannerProcessor->setParameter(CHORD_SCANNER_OCTAVE_TO, octaveTo);

                // needs to call ui fuction
                uiListeners.call(&UIListener::updateChordScannerUIParameter, CHORD_SCANNER_OCTAVE_TO);
            }
            
            int octaveFrom = chordScannerParameters->getProperty(getIdentifierForChordScannerParameterIndex(CHORD_SCANNER_OCTAVE_FROM));
            
            if (octaveFrom > 10)
            {
                octaveFrom = 10;
                
                chordScannerParameters->setProperty(getIdentifierForChordScannerParameterIndex(CHORD_SCANNER_OCTAVE_FROM), octaveFrom, nullptr);
                
                chordScannerProcessor->setParameter(CHORD_SCANNER_OCTAVE_FROM, octaveFrom);

                // needs to call ui fuction
                uiListeners.call(&UIListener::updateChordScannerUIParameter, CHORD_SCANNER_OCTAVE_FROM);
            }
        }
    }
    
    // store new value in paramater valuetree
    chordScannerParameters->setProperty(getIdentifierForChordScannerParameterIndex(index), newValue, nullptr);
    
    chordScannerProcessor->setParameter(index, newValue);

    // needs to call ui fuction
    uiListeners.call(&UIListener::updateChordScannerUIParameter, index);
}

// called from GUI to update controls when neccessary
var ProjectManager::getChordScannerParameter(int index)
{
    return chordScannerParameters->getProperty(getIdentifierForChordScannerParameterIndex(index));
}

//===============================================================================
#pragma mark Frequency Player Parameters
//===============================================================================

String ProjectManager::getIdentifierForFrequencyPlayerParameterIndex(int index)
{
    switch (index) {
        case FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE:               return "FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE"; break;
        case FREQUENCY_PLAYER_SHORTCUT_MUTE:                    return "FREQUENCY_PLAYER_SHORTCUT_MUTE"; break;
        case FREQUENCY_PLAYER_SHORTCUT_PLAY_AT_SAME_TIME:       return "FREQUENCY_PLAYER_SHORTCUT_PLAY_AT_SAME_TIME"; break;
        case FREQUENCY_PLAYER_FREQ_SOURCE:                      return "FREQUENCY_PLAYER_FREQ_SOURCE"; break;
        case FREQUENCY_PLAYER_CHOOSE_FREQ:                      return "FREQUENCY_PLAYER_CHOOSE_FREQ"; break;
        case FREQUENCY_PLAYER_RANGE_MIN:                        return "FREQUENCY_PLAYER_RANGE_MIN"; break;
        case FREQUENCY_PLAYER_RANGE_MAX:                        return "FREQUENCY_PLAYER_RANGE_MAX"; break;
        case FREQUENCY_PLAYER_RANGE_LENGTH:                     return "FREQUENCY_PLAYER_RANGE_LENGTH"; break;
        case FREQUENCY_PLAYER_RANGE_LOG:                        return "FREQUENCY_PLAYER_RANGE_LOG"; break;
        case FREQUENCY_PLAYER_MANIPULATE_CHOSEN_FREQUENCY:      return "FREQUENCY_PLAYER_MANIPULATE_CHOSEN_FREQUENCY"; break;
        case FREQUENCY_PLAYER_MULTIPLY_OR_DIVISION:             return "FREQUENCY_PLAYER_MULTIPLY_OR_DIVISION"; break;
        case FREQUENCY_PLAYER_MULTIPLY_VALUE:                   return "FREQUENCY_PLAYER_MULTIPLY_VALUE"; break;
        case FREQUENCY_PLAYER_DIVISION_VALUE:                   return "FREQUENCY_PLAYER_DIVISION_VALUE"; break;
        case FREQUENCY_PLAYER_WAVEFORM_TYPE:                    return "FREQUENCY_PLAYER_WAVEFORM_TYPE"; break;
        case FREQUENCY_PLAYER_AMPLITUDE:                        return "FREQUENCY_PLAYER_AMPLITUDE"; break;
        case FREQUENCY_PLAYER_ATTACK:                           return "FREQUENCY_PLAYER_ATTACK"; break;
        case FREQUENCY_PLAYER_DECAY:                            return "FREQUENCY_PLAYER_DECAY"; break;
        case FREQUENCY_PLAYER_SUSTAIN:                          return "FREQUENCY_PLAYER_SUSTAIN"; break;
        case FREQUENCY_PLAYER_RELEASE:                          return "FREQUENCY_PLAYER_RELEASE"; break;
        case FREQUENCY_PLAYER_NUM_REPEATS:                      return "FREQUENCY_PLAYER_NUM_REPEATS"; break;
        case FREQUENCY_PLAYER_NUM_PAUSE:                        return "FREQUENCY_PLAYER_NUM_PAUSE"; break;
        case FREQUENCY_PLAYER_NUM_DURATION:                     return "FREQUENCY_PLAYER_NUM_DURATION"; break;
        case FREQUENCY_PLAYER_OUTPUT_SELECTION:                 return "FREQUENCY_PLAYER_OUTPUT_SELECTION"; break;
            
        default: return ""; break;
    }
}

void ProjectManager::initDefaultFrequencyPlayerParameters()
{
    // set default properties
    for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
    {
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE),           (bool)false,         nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_SHORTCUT_MUTE),                (bool)false,        nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_SHORTCUT_PLAY_AT_SAME_TIME),   (bool)false,        nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_FREQ_SOURCE),                  (bool)false,        nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_CHOOSE_FREQ),                  (double)432.0,       nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_RANGE_MIN),                    (double)144.0,       nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_RANGE_MAX),                    (double)432.0,       nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_RANGE_LENGTH),                 (float)1000.0,     nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_RANGE_LOG),                    (bool)false,        nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_MANIPULATE_CHOSEN_FREQUENCY),  (bool)false,        nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_MULTIPLY_OR_DIVISION),         (bool)false,        nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_MULTIPLY_VALUE),               (double)1.0,         nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_DIVISION_VALUE),               (double)1.0,         nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_WAVEFORM_TYPE),                (int)1,             nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_AMPLITUDE),                    (double)100.0,      nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_ATTACK),                       (double)10.0,       nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_SUSTAIN),                      (double)100.0,      nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_DECAY),                        (double)1.0,        nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_RELEASE),                      (double)300.0,      nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_NUM_REPEATS),                  (int)1,             nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_NUM_PAUSE),                    (int)1000,          nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_NUM_DURATION),                 (int)300,           nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_OUTPUT_SELECTION),                 (int)AUDIO_OUTPUTS::MONO_1,           nullptr);

        for (int index = 0; index < TOTAL_NUM_FREQUENCY_PLAYER_SHORTCUT_PARAMS; index++)
        {
            if (index == FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE)
            {
                frequencyPlayerProcessor->setActiveShortcutSynth(i, frequencyPlayerParameters[i]->getProperty( getIdentifierForFrequencyPlayerParameterIndex(index)));
            }
            else
            {
                frequencyPlayerProcessor->setParameter(i, index, frequencyPlayerParameters[i]->getProperty( getIdentifierForFrequencyPlayerParameterIndex(index)) );
            }
        }
    }
}

void ProjectManager::initDefaultFrequencyPlayerParametersForShortcut(int shortcutRef)
{
    int i = shortcutRef;
    // set default properties

    
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_SHORTCUT_MUTE),                (bool)false,        nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_SHORTCUT_PLAY_AT_SAME_TIME),   (bool)false,        nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_FREQ_SOURCE),                  (bool)false,        nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_CHOOSE_FREQ),                  (double)432.0,       nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_RANGE_MIN),                    (double)144.0,       nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_RANGE_MAX),                    (double)432.0,       nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_RANGE_LENGTH),                 (float)1000.0,     nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_RANGE_LOG),                    (bool)false,        nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_MANIPULATE_CHOSEN_FREQUENCY),  (bool)false,        nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_MULTIPLY_OR_DIVISION),         (bool)false,        nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_MULTIPLY_VALUE),               (double)1.0,         nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_DIVISION_VALUE),               (double)1.0,         nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_WAVEFORM_TYPE),                (int)1,             nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_AMPLITUDE),                    (double)100.0,      nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_ATTACK),                       (double)10.0,       nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_SUSTAIN),                      (double)100.0,      nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_DECAY),                            (double)1.0,        nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_RELEASE),                      (double)300.0,      nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_NUM_REPEATS),                  (int)1,             nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_NUM_PAUSE),                    (int)1000,          nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_NUM_DURATION),                 (int)300,           nullptr);
        frequencyPlayerParameters[i]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_OUTPUT_SELECTION),                 (int)AUDIO_OUTPUTS::MONO_1,           nullptr);

        for (int index = 0; index < TOTAL_NUM_FREQUENCY_PLAYER_SHORTCUT_PARAMS; index++)
        {
            if (index == FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE)
            {
                frequencyPlayerProcessor->setActiveShortcutSynth(i, frequencyPlayerParameters[i]->getProperty( getIdentifierForFrequencyPlayerParameterIndex(index)));
            }
            else
            {
                frequencyPlayerProcessor->setParameter(i, index, frequencyPlayerParameters[i]->getProperty( getIdentifierForFrequencyPlayerParameterIndex(index)) );
            }
        }
}

// called from UI to change individual parameters
void ProjectManager::setFrequencyPlayerParameter(int synthRef,int index, var newValue)
{
    frequencyPlayerParameters[synthRef]->setProperty(getIdentifierForFrequencyPlayerParameterIndex(index), newValue, nullptr);
    
    // check if active param // mmm
    if (index == FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE)
    {
        frequencyPlayerProcessor->setActiveShortcutSynth(synthRef, (bool) chordPlayerParameters[synthRef]->getProperty( getIdentifierForFrequencyPlayerParameterIndex(FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE)) );
        frequencyPlayerProcessor->setParameter(synthRef, index, newValue);
        
        // init defaults
//        initDefaultFrequencyPlayerParametersForShortcut(synthRef);
    }
    else //if (index >= FREQUENCY_PLAYER_SHORTCUT_MUTE && index <= FREQUENCY_PLAYER_NUM_DURATION)
    {
        frequencyPlayerProcessor->setParameter(synthRef, index, newValue);
    }
    
    // needs to call ui fuction
    uiListeners.call(&UIListener::updateFrequencyPlayerUIParameter, synthRef, index);
}

// called from GUI to update controls when neccessary
var ProjectManager::getFrequencyPlayerParameter(int synthRef,int index)
{
    return frequencyPlayerParameters[synthRef]->getProperty(getIdentifierForFrequencyPlayerParameterIndex(index));
}

//===============================================================================
#pragma mark Frequency Scanner Parameters
//===============================================================================

String ProjectManager::getIdentifierForFrequencyScannerParameterIndex(int index)
{
    switch (index) {
        case FREQUENCY_SCANNER_SPECTRUM_ANALYZER:               return "FREQUENCY_SCANNER_SPECTRUM_ANALYZER"; break;
        case FREQUENCY_SCANNER_MODE:                            return "FREQUENCY_SCANNER_MODE"; break;
        case FREQUENCY_SCANNER_FREQUENCY_FROM:                  return "FREQUENCY_SCANNER_FREQUENCY_FROM"; break;
        case FREQUENCY_SCANNER_FREQUENCY_TO:                    return "FREQUENCY_SCANNER_FREQUENCY_TO"; break;
        case FREQUENCY_SCANNER_EXTENDED:                        return "FREQUENCY_SCANNER_EXTENDED"; break;
        case FREQUENCY_SCANNER_WAVEFORM_TYPE:                   return "FREQUENCY_SCANNER_WAVEFORM_TYPE"; break;
        case FREQUENCY_SCANNER_ENV_AMPLITUDE:                   return "FREQUENCY_SCANNER_ENV_AMPLITUDE"; break;
        case FREQUENCY_SCANNER_ENV_ATTACK:                      return "FREQUENCY_SCANNER_ENV_ATTACK"; break;
        case FREQUENCY_SCANNER_ENV_SUSTAIN:                     return "FREQUENCY_SCANNER_ENV_SUSTAIN"; break;
        case FREQUENCY_SCANNER_ENV_DECAY:                       return "FREQUENCY_SCANNER_ENV_DECAY"; break;
        case FREQUENCY_SCANNER_ENV_RELEASE:                     return "FREQUENCY_SCANNER_ENV_RELEASE"; break;
        case FREQUENCY_SCANNER_NUM_REPEATS:                     return "FREQUENCY_SCANNER_NUM_REPEATS"; break;
        case FREQUENCY_SCANNER_NUM_PAUSE:                       return "FREQUENCY_SCANNER_NUM_PAUSE"; break;
        case FREQUENCY_SCANNER_NUM_DURATION:                    return "FREQUENCY_SCANNER_NUM_DURATION"; break;
        case FREQUENCY_SCANNER_LOG_LIN:                         return "FREQUENCY_SCANNER_LOG_LIN"; break;
        case FREQUENCY_SCANNER_LOG_VALUE:                       return "FREQUENCY_SCANNER_LOG_VALUE"; break;
        case FREQUENCY_SCANNER_LIN_VALUE:                       return "FREQUENCY_SCANNER_LIN_VALUE"; break;
        case FREQUENCY_SCANNER_OUTPUT_SELECTION:                       return "FREQUENCY_SCANNER_OUTPUT_SELECTION"; break;
            
        default: return ""; break;
    }
}

void ProjectManager::initDefaultFrequencyScannerParameters()
{
    // set default properties

    frequencyScannerParameters->setProperty(getIdentifierForFrequencyScannerParameterIndex(FREQUENCY_SCANNER_SPECTRUM_ANALYZER),    (bool)true,             nullptr);
    frequencyScannerParameters->setProperty(getIdentifierForFrequencyScannerParameterIndex(FREQUENCY_SCANNER_MODE),                 (int)0,                 nullptr);
    frequencyScannerParameters->setProperty(getIdentifierForFrequencyScannerParameterIndex(FREQUENCY_SCANNER_FREQUENCY_FROM),       (double)FREQUENCY_MIN,  nullptr);
    frequencyScannerParameters->setProperty(getIdentifierForFrequencyScannerParameterIndex(FREQUENCY_SCANNER_FREQUENCY_TO),         (double)20000,          nullptr);
    frequencyScannerParameters->setProperty(getIdentifierForFrequencyScannerParameterIndex(FREQUENCY_SCANNER_EXTENDED),             (bool)0,                nullptr);
    frequencyScannerParameters->setProperty(getIdentifierForFrequencyScannerParameterIndex(FREQUENCY_SCANNER_WAVEFORM_TYPE),        (int)0,                 nullptr);
    frequencyScannerParameters->setProperty(getIdentifierForFrequencyScannerParameterIndex(FREQUENCY_SCANNER_ENV_AMPLITUDE),        (double)100.0,          nullptr);
    frequencyScannerParameters->setProperty(getIdentifierForFrequencyScannerParameterIndex(FREQUENCY_SCANNER_ENV_ATTACK),           (double)10.0,           nullptr);
    frequencyScannerParameters->setProperty(getIdentifierForFrequencyScannerParameterIndex(FREQUENCY_SCANNER_ENV_SUSTAIN),          (double)100.0,          nullptr);
    frequencyScannerParameters->setProperty(getIdentifierForFrequencyScannerParameterIndex(FREQUENCY_SCANNER_ENV_DECAY),            (double)0.5,            nullptr);
    frequencyScannerParameters->setProperty(getIdentifierForFrequencyScannerParameterIndex(FREQUENCY_SCANNER_ENV_RELEASE),          (double)300.0,          nullptr);
    frequencyScannerParameters->setProperty(getIdentifierForFrequencyScannerParameterIndex(FREQUENCY_SCANNER_NUM_REPEATS),          (int)1,                 nullptr);
    frequencyScannerParameters->setProperty(getIdentifierForFrequencyScannerParameterIndex(FREQUENCY_SCANNER_NUM_PAUSE),            (int)1000,              nullptr);
    frequencyScannerParameters->setProperty(getIdentifierForFrequencyScannerParameterIndex(FREQUENCY_SCANNER_NUM_DURATION),         (int)300,               nullptr);
    frequencyScannerParameters->setProperty(getIdentifierForFrequencyScannerParameterIndex(FREQUENCY_SCANNER_LOG_LIN),              (int)0,                 nullptr);
    frequencyScannerParameters->setProperty(getIdentifierForFrequencyScannerParameterIndex(FREQUENCY_SCANNER_LOG_VALUE),            (double)0.01,           nullptr);
    frequencyScannerParameters->setProperty(getIdentifierForFrequencyScannerParameterIndex(FREQUENCY_SCANNER_LIN_VALUE),            (double)1,              nullptr);
    frequencyScannerParameters->setProperty(getIdentifierForFrequencyScannerParameterIndex(FREQUENCY_SCANNER_OUTPUT_SELECTION),            (int)AUDIO_OUTPUTS::MONO_1,              nullptr);
    
    for (int index = 0; index < TOTAL_NUM_FREQUENCY_SCANNER_PARAMS; index++)
    {
        frequencyScannerProcessor->setParameter(index, frequencyScannerParameters->getProperty( getIdentifierForFrequencyScannerParameterIndex(index)) );
    }
}

// called from UI to change individual parameters
void ProjectManager::setFrequencyScannerParameter(int index, var newValue)
{
    frequencyScannerParameters->setProperty(getIdentifierForFrequencyScannerParameterIndex(index), newValue, nullptr);
    
    frequencyScannerProcessor->setParameter(index, newValue);

    // needs to call ui fuction
    uiListeners.call(&UIListener::updateFrequencyScannerUIParameter , index);
}

// called from GUI to update controls when neccessary
var ProjectManager::getFrequencyScannerParameter(int index)
{
    return frequencyScannerParameters->getProperty(getIdentifierForFrequencyScannerParameterIndex(index));
}

//===============================================================================
#pragma mark Frequency To Light Parameters
//===============================================================================

String ProjectManager::getIdentifierForFrequencyToLightParameterIndex(int index)
{
    switch (index) {
        case FREQUENCY_LIGHT_CONVERSION_SOURCE:                 return "FREQUENCY_LIGHT_CONVERSION_SOURCE"; break;
        case FREQUENCY_LIGHT_KEYNOTE:                           return "FREQUENCY_LIGHT_KEYNOTE"; break;
        case FREQUENCY_LIGHT_CHORDTYPE:                         return "FREQUENCY_LIGHT_CHORDTYPE"; break;
        case FREQUENCY_LIGHT_FREQUENCY_UNIT:                    return "FREQUENCY_LIGHT_FREQUENCY_UNIT"; break;
        case FREQUENCY_LIGHT_WAVELENGTH_UNIT:                   return "FREQUENCY_LIGHT_WAVELENGTH_UNIT"; break;
        case FREQUENCY_LIGHT_FREQUENCY:                         return "FREQUENCY_LIGHT_FREQUENCY"; break;
        case FREQUENCY_LIGHT_WAVELENGTH:                        return "FREQUENCY_LIGHT_WAVELENGTH"; break;
        case FREQUENCY_LIGHT_PHASESPEED:                        return "FREQUENCY_LIGHT_PHASESPEED"; break;

        case FREQUENCY_LIGHT_MANIPULATE_CHOSEN_FREQUENCY1:      return "FREQUENCY_LIGHT_MANIPULATE_CHOSEN_FREQUENCY1"; break;
        case FREQUENCY_LIGHT_MULTIPLY_OR_DIVISION1:             return "FREQUENCY_LIGHT_MULTIPLY_OR_DIVISION1"; break;
        case FREQUENCY_LIGHT_MULTIPLY_VALUE1:                   return "FREQUENCY_LIGHT_MULTIPLY_VALUE1"; break;
        case FREQUENCY_LIGHT_DIVISION_VALUE1:                   return "FREQUENCY_LIGHT_DIVISION_VALUE1"; break;
            
        case FREQUENCY_LIGHT_MANIPULATE_CHOSEN_FREQUENCY2:      return "FREQUENCY_LIGHT_MANIPULATE_CHOSEN_FREQUENCY2"; break;
        case FREQUENCY_LIGHT_MULTIPLY_OR_DIVISION2:             return "FREQUENCY_LIGHT_MULTIPLY_OR_DIVISION2"; break;
        case FREQUENCY_LIGHT_MULTIPLY_VALUE2:                   return "FREQUENCY_LIGHT_MULTIPLY_VALUE2"; break;
        case FREQUENCY_LIGHT_DIVISION_VALUE2:                   return "FREQUENCY_LIGHT_DIVISION_VALUE2"; break;
            
        case FREQUENCY_LIGHT_MANIPULATE_CHOSEN_FREQUENCY3:      return "FREQUENCY_LIGHT_MANIPULATE_CHOSEN_FREQUENCY3"; break;
        case FREQUENCY_LIGHT_MULTIPLY_OR_DIVISION3:             return "FREQUENCY_LIGHT_MULTIPLY_OR_DIVISION3"; break;
        case FREQUENCY_LIGHT_MULTIPLY_VALUE3:                   return "FREQUENCY_LIGHT_MULTIPLY_VALUE3"; break;
        case FREQUENCY_LIGHT_DIVISION_VALUE3:                   return "FREQUENCY_LIGHT_DIVISION_VALUE3"; break;
        
        case FREQUENCY_LIGHT_MANIPULATE_CHOSEN_FREQUENCY4:      return "FREQUENCY_LIGHT_MANIPULATE_CHOSEN_FREQUENCY4"; break;
        case FREQUENCY_LIGHT_MULTIPLY_OR_DIVISION4:             return "FREQUENCY_LIGHT_MULTIPLY_OR_DIVISION4"; break;
        case FREQUENCY_LIGHT_MULTIPLY_VALUE4:                   return "FREQUENCY_LIGHT_MULTIPLY_VALUE4"; break;
        case FREQUENCY_LIGHT_DIVISION_VALUE4:                   return "FREQUENCY_LIGHT_DIVISION_VALUE4"; break;
            
        case FREQUENCY_LIGHT_MANIPULATE_CHOSEN_FREQUENCY5:      return "FREQUENCY_LIGHT_MANIPULATE_CHOSEN_FREQUENCY5"; break;
        case FREQUENCY_LIGHT_MULTIPLY_OR_DIVISION5:             return "FREQUENCY_LIGHT_MULTIPLY_OR_DIVISION5"; break;
        case FREQUENCY_LIGHT_MULTIPLY_VALUE5:                   return "FREQUENCY_LIGHT_MULTIPLY_VALUE5"; break;
        case FREQUENCY_LIGHT_DIVISION_VALUE5:                   return "FREQUENCY_LIGHT_DIVISION_VALUE5"; break;
            
        case FREQUENCY_LIGHT_COLOUR_SLIDER_VALUE:               return "FREQUENCY_LIGHT_COLOUR_SLIDER_VALUE"; break;
            
        default: return ""; break;
    }
}

void ProjectManager::initDefaultFrequencyToLightParameters()
{
    // set default properties

    frequencyToLightParameters->setProperty(getIdentifierForFrequencyToLightParameterIndex(FREQUENCY_LIGHT_CONVERSION_SOURCE),              (bool)false,     nullptr);
    frequencyToLightParameters->setProperty(getIdentifierForFrequencyToLightParameterIndex(FREQUENCY_LIGHT_KEYNOTE),                        (int)1,         nullptr);
    frequencyToLightParameters->setProperty(getIdentifierForFrequencyToLightParameterIndex(FREQUENCY_LIGHT_CHORDTYPE),                      (int)1,         nullptr);
    frequencyToLightParameters->setProperty(getIdentifierForFrequencyToLightParameterIndex(FREQUENCY_LIGHT_FREQUENCY_UNIT),                 (int)1,         nullptr);
    frequencyToLightParameters->setProperty(getIdentifierForFrequencyToLightParameterIndex(FREQUENCY_LIGHT_WAVELENGTH_UNIT),                (int)1,         nullptr);
    frequencyToLightParameters->setProperty(getIdentifierForFrequencyToLightParameterIndex(FREQUENCY_LIGHT_FREQUENCY),                      (double)432.0,   nullptr);
    frequencyToLightParameters->setProperty(getIdentifierForFrequencyToLightParameterIndex(FREQUENCY_LIGHT_WAVELENGTH),                     (double)222.2,   nullptr);
    frequencyToLightParameters->setProperty(getIdentifierForFrequencyToLightParameterIndex(FREQUENCY_LIGHT_PHASESPEED),                     (int)SPEED_LIGHT_IN_VACUUM ,     nullptr);
    
    frequencyToLightParameters->setProperty(getIdentifierForFrequencyToLightParameterIndex(FREQUENCY_LIGHT_MANIPULATE_CHOSEN_FREQUENCY2),    (bool)false,    nullptr);
    frequencyToLightParameters->setProperty(getIdentifierForFrequencyToLightParameterIndex(FREQUENCY_LIGHT_MULTIPLY_OR_DIVISION2),           (bool)false,    nullptr);
    frequencyToLightParameters->setProperty(getIdentifierForFrequencyToLightParameterIndex(FREQUENCY_LIGHT_MULTIPLY_VALUE2),                 (float)1.0,     nullptr);
    frequencyToLightParameters->setProperty(getIdentifierForFrequencyToLightParameterIndex(FREQUENCY_LIGHT_DIVISION_VALUE2),                 (float)1.0,     nullptr);
    
    frequencyToLightParameters->setProperty(getIdentifierForFrequencyToLightParameterIndex(FREQUENCY_LIGHT_MANIPULATE_CHOSEN_FREQUENCY2),    (bool)false,    nullptr);
    frequencyToLightParameters->setProperty(getIdentifierForFrequencyToLightParameterIndex(FREQUENCY_LIGHT_MULTIPLY_OR_DIVISION2),           (bool)false,    nullptr);
    frequencyToLightParameters->setProperty(getIdentifierForFrequencyToLightParameterIndex(FREQUENCY_LIGHT_MULTIPLY_VALUE2),                 (float)1.0,     nullptr);
    frequencyToLightParameters->setProperty(getIdentifierForFrequencyToLightParameterIndex(FREQUENCY_LIGHT_DIVISION_VALUE2),                 (float)1.0,     nullptr);
    
    frequencyToLightParameters->setProperty(getIdentifierForFrequencyToLightParameterIndex(FREQUENCY_LIGHT_MANIPULATE_CHOSEN_FREQUENCY3),    (bool)false,    nullptr);
    frequencyToLightParameters->setProperty(getIdentifierForFrequencyToLightParameterIndex(FREQUENCY_LIGHT_MULTIPLY_OR_DIVISION3),           (bool)false,    nullptr);
    frequencyToLightParameters->setProperty(getIdentifierForFrequencyToLightParameterIndex(FREQUENCY_LIGHT_MULTIPLY_VALUE3),                 (float)1.0,     nullptr);
    frequencyToLightParameters->setProperty(getIdentifierForFrequencyToLightParameterIndex(FREQUENCY_LIGHT_DIVISION_VALUE3),                 (float)1.0,     nullptr);
    
    frequencyToLightParameters->setProperty(getIdentifierForFrequencyToLightParameterIndex(FREQUENCY_LIGHT_MANIPULATE_CHOSEN_FREQUENCY4),    (bool)false,    nullptr);
    frequencyToLightParameters->setProperty(getIdentifierForFrequencyToLightParameterIndex(FREQUENCY_LIGHT_MULTIPLY_OR_DIVISION4),           (bool)false,    nullptr);
    frequencyToLightParameters->setProperty(getIdentifierForFrequencyToLightParameterIndex(FREQUENCY_LIGHT_MULTIPLY_VALUE4),                 (float)1.0,     nullptr);
    frequencyToLightParameters->setProperty(getIdentifierForFrequencyToLightParameterIndex(FREQUENCY_LIGHT_DIVISION_VALUE4),                 (float)1.0,     nullptr);
    
    frequencyToLightParameters->setProperty(getIdentifierForFrequencyToLightParameterIndex(FREQUENCY_LIGHT_MANIPULATE_CHOSEN_FREQUENCY5),    (bool)false,    nullptr);
    frequencyToLightParameters->setProperty(getIdentifierForFrequencyToLightParameterIndex(FREQUENCY_LIGHT_MULTIPLY_OR_DIVISION5),           (bool)false,    nullptr);
    frequencyToLightParameters->setProperty(getIdentifierForFrequencyToLightParameterIndex(FREQUENCY_LIGHT_MULTIPLY_VALUE5),                 (float)1.0,     nullptr);
    frequencyToLightParameters->setProperty(getIdentifierForFrequencyToLightParameterIndex(FREQUENCY_LIGHT_DIVISION_VALUE5),                 (float)1.0,     nullptr);
    frequencyToLightParameters->setProperty(getIdentifierForFrequencyToLightParameterIndex(FREQUENCY_LIGHT_COLOUR_SLIDER_VALUE),             (float)0.0,     nullptr);

    for (int i = 0; i < FREQUENCY_TO_LIGHT_PARAMS::TOTAL_NUM_FREQUENCY_LIGHT_PARAMS; i++)
    {
        frequencyToLightProcessor->setParameter(i, getFrequencyToLightParameter(i));
    }
}

// called from UI to change individual parameters
void ProjectManager::setFrequencyToLightParameter(int index, var newValue)
{

    frequencyToLightParameters->setProperty(getIdentifierForFrequencyToLightParameterIndex(index), newValue, nullptr);
    
    // do frequency light conversions here if nessary
    frequencyToLightProcessor->setParameter(index, newValue);
    
    uiListeners.call(&UIListener::updateFrequencyToLightUIParameter, index);
}

// called from GUI to update controls when neccessary
var ProjectManager::getFrequencyToLightParameter(int index)
{
    return frequencyToLightParameters->getProperty(getIdentifierForFrequencyToLightParameterIndex(index));
}

//===============================================================================
#pragma mark FFT
//===============================================================================

void ProjectManager::setShouldProcessFFT(bool should)
{
    shouldProcessFFT = should;
}

void ProjectManager::setNewFFTSize(int fftEnum)
{
    fftSizeEnumValue    = fftEnum;

    shouldProcessFFT    = false;
    
    for (int i = 0; i < 8; i++)
    {
        analyser[i].setNewFFTSize(fftSizeEnumValue);
    }

    uiListeners.call(&UIListener::updateSettingsUIParameter, FFT_SIZE);
    
    shouldProcessFFT = true;
}


void ProjectManager::setNewFFTWindowFunction(int windowEnum)
{
    shouldProcessFFT    = false;
    
    for (int i = 0; i < 8; i++)
    {
        analyser[i].setNewFFTWindowFunction(windowEnum);
    }

    shouldProcessFFT    = true;
}

void ProjectManager::initFFT()
{
    outputAnalyser.initFFT();
    
    for (int i = 0; i < 8; i++)
    {
        analyser[i].initFFT();
    }
}


float ProjectManager::getVUMeterValue(int channel)
{
    return vuMeterValues[channel];
}

//=====================================================================================
// new  FFT threader
//=====================================================================================


void ProjectManager::createAnalyserPlot (Path& p, const Rectangle<int> bounds, float minFreq, float maxFreq, bool input)
{
//    outputAnalyser.createPath (p, bounds.toFloat(), minFreq, maxFreq);
//    outputAnalyser.createPathOptimised (p, bounds.toFloat(), minFreq, maxFreq);
    
}

void ProjectManager::createAnalyserPlot (int fftChannel, Path& p, const Rectangle<int> bounds, float minFreq, float maxFreq, bool input)
{
    if (fftChannel >= 0 && fftChannel < 8)
    {
        analyser[fftChannel].createPath (p, bounds.toFloat(), minFreq, maxFreq);
    }
}

void ProjectManager::createAnalyserPlotOptimisedWithRange (Path& p, const Rectangle<int> bounds, float minFreq, float maxFreq, float minDB, float maxDB, bool input)
{
//    outputAnalyser.createPathOptimisedWithRange (p, bounds.toFloat(), minFreq, maxFreq, minDB, maxDB);
}

void ProjectManager::createAnalyserPlotOptimisedWithRange (int fftChannel, Path& p, const Rectangle<int> bounds, float minFreq, float maxFreq, float minDB, float maxDB, bool input)
{
    if (fftChannel >= 0 && fftChannel < 8)
    {
        analyser[fftChannel].createPathOptimisedWithRange (p, bounds.toFloat(), minFreq, maxFreq, minDB, maxDB);
    }
}

void ProjectManager::createOctaveMagnitudes(Array<float> & magnitude, int & numBands, float minFreq, float maxFreq, Array<float> & centralFreqs)
{
//    outputAnalyser.getMagnitudeDataForOctave (magnitude, numBands, minFreq, maxFreq, getSampleRate(), centralFreqs);
}

void ProjectManager::createOctaveMagnitudes(int fftChannel, Array<float> & magnitude, int & numBands, float minFreq, float maxFreq, Array<float> & centralFreqs)
{
    // Always clear output arrays first to ensure clean state
    magnitude.clear();
    centralFreqs.clear();

    // Validate channel index
    if (fftChannel < 0 || fftChannel >= 8)
    {
        numBands = 0;
        return;
    }

    // Validate sample rate
    const float sampleRate = getSampleRate();
    if (sampleRate <= 0.0f)
    {
        numBands = 0;
        return;
    }

    analyser[fftChannel].getMagnitudeDataForOctave(magnitude, numBands, minFreq, maxFreq, sampleRate, centralFreqs);
}

void ProjectManager::createColourSpectrum(Image & imageToRenderTo, float minFreq, float maxFreq, float logScale)
{
//    outputAnalyser.createColourSpectrum(imageToRenderTo, minFreq, maxFreq, logScale);
}

void ProjectManager::createColourSpectrum(int fftChannel, Image & imageToRenderTo, float minFreq, float maxFreq, float logScale)
{
    if (fftChannel >= 0 && fftChannel < 8)
    {
        analyser[fftChannel].createColourSpectrum(imageToRenderTo, minFreq, maxFreq, logScale);
    }
}

void ProjectManager::createFrequencyData(double & peakFrequency, double & peakDB, Array<float> & upperHarmonics, Array<float> & intervals, int & keynote, int & octave, double & ema)
{
//    outputAnalyser.getFrequencyData(peakFrequency, peakDB, upperHarmonics, intervals, ema);
    
    // if its recording, logging should be called from here..
    
    // make copies of all the variables before pushing processLog
    
    double pf                   = peakFrequency;
    double pdb                  = peakDB;
    Array<float> upHarmonics    = upperHarmonics;
    Array<float> newintervals   = intervals;
    int kn                      = keynote;
    int oct                     = octave;
    double e                    = ema;
    
    logFileWriter->processLog(pf, pdb, upHarmonics, intervals, kn, oct, e);
}

void ProjectManager::createFrequencyData(int fftChannel, double & peakFrequency, double & peakDB, Array<float> & upperHarmonics, Array<float> & intervals, int & keynote, int & octave, double & ema)
{
    if (fftChannel >= 0 && fftChannel < 8)
    {
        analyser[fftChannel].getFrequencyData(peakFrequency, peakDB, upperHarmonics, intervals, ema);
        
        // if its recording, logging should be called from here..
        
        // make copies of all the variables before pushing processLog
        
        double pf                   = peakFrequency;
        double pdb                  = peakDB;
        Array<float> upHarmonics    = upperHarmonics;
        Array<float> newintervals   = intervals;
        int kn                      = keynote;
        int oct                     = octave;
        double e                    = ema;
        
        logFileWriter->processLog(pf, pdb, upHarmonics, intervals, kn, oct, e);
    }
}

void ProjectManager::getMovingAveragePeakData(double & _peakFreq, double & _peakDB, double & _movingAvgFreq)
{
//    outputAnalyser.getMovingAveragePeakData(_peakFreq, _peakDB, _movingAvgFreq);
}

void ProjectManager::getMovingAveragePeakData(int fftChannel, double & _peakFreq, double & _peakDB, double & _movingAvgFreq)
{
    if (fftChannel >= 0 && fftChannel < 8)
    {
        analyser[fftChannel].getMovingAveragePeakData(_peakFreq, _peakDB, _movingAvgFreq);
    }
}

//===============================================================================
#pragma mark Recorder
//===============================================================================

uint64 ProjectManager::getRecordCounterInMilliseconds()
{
    recordCounterInMilliseconds = (int64)recordCounterInSamples / 44100.f * 1000.f;
    
    return recordCounterInMilliseconds;
}


void ProjectManager::recordLoopToFile(const File & file)
{
    setupRecording(file);
}




void ProjectManager::createNewFileForRecordingRealtimeAnalysis()
{
    // start recording in projectManager
    Time time = Time::getCurrentTime();
    
    String filename("RealtimeAnalysisAudioRecording-");
    String M(time.getMinutes());
    String HH(time.getHours());
    String DD(time.getDayOfMonth());
    String MM(time.getMonth() + 1);
    String YY(time.getYear());
    
    filename.append(DD, 4); filename.append("-", 3);
    filename.append(MM, 4); filename.append("-", 3);
    filename.append(YY, 4); filename.append("-", 3);
    filename.append(HH, 4); filename.append("-", 3);
    filename.append(M, 4);
    
    
    String url(recordFileDirectory.getFullPathName());
    
    url.append("/", 2);
    url.append(filename, 100);
//    url.append(".wav", 4);
    
    String extension = formatManager.getKnownFormat(recordFileFormatIndex)->getFileExtensions().getReference(0);
    url.append(extension, 6);
    
    File newRecordingFile(url);
    newRecordingFile.create();
    
    setupRecording(newRecordingFile);
}

void ProjectManager::createNewFileForRecordingChordPlayer()
{
    // start recording in projectManager
    Time time = Time::getCurrentTime();
    
    String filename("ChordPlayerAudioRecording-");
    String M(time.getMinutes());
    String HH(time.getHours());
    String DD(time.getDayOfMonth());
    String MM(time.getMonth() + 1);
    String YY(time.getYear());
    
    filename.append(DD, 4); filename.append("-", 3);
    filename.append(MM, 4); filename.append("-", 3);
    filename.append(YY, 4); filename.append("-", 3);
    filename.append(HH, 4); filename.append("-", 3);
    filename.append(M, 4);
    
    
    String url(recordFileDirectory.getFullPathName());
    
    url.append("/", 2);
    url.append(filename, 100);
//    url.append(".wav", 4);
    
    String extension = formatManager.getKnownFormat(recordFileFormatIndex)->getFileExtensions().getReference(0);
    url.append(extension, 6);
    
    File newRecordingFile(url);
    newRecordingFile.create();
    
    setupRecording(newRecordingFile);
}

void ProjectManager::createNewFileForRecordingFrequencyPlayer()
{
    // start recording in projectManager
    Time time = Time::getCurrentTime();
    
    String filename("FrequencyPlayerAudioRecording-");
    String M(time.getMinutes());
    String HH(time.getHours());
    String DD(time.getDayOfMonth());
    String MM(time.getMonth() + 1);
    String YY(time.getYear());
    
    filename.append(DD, 4); filename.append("-", 3);
    filename.append(MM, 4); filename.append("-", 3);
    filename.append(YY, 4); filename.append("-", 3);
    filename.append(HH, 4); filename.append("-", 3);
    filename.append(M, 4);
    
    
    String url(recordFileDirectory.getFullPathName());
    
    url.append("/", 2);
    url.append(filename, 100);
    
    String extension = formatManager.getKnownFormat(recordFileFormatIndex)->getFileExtensions().getReference(0);
    
    url.append(extension, 6);
    
    File newRecordingFile(url);
    newRecordingFile.create();
    
    setupRecording(newRecordingFile);
}

//==============================================================================
void ProjectManager::setupRecording (const File& file)
{
    stopRecording();

    double sampleRate = getSampleRate();
    
    auto currentDev             = deviceManager->getCurrentAudioDevice();
    auto numInputChannels       = currentDev->getActiveInputChannels();
    {
        // Create an OutputStream to write to our destination file...
        file.deleteFile();

        // FIXED: Use smart pointer for FileOutputStream
        auto fileStream = std::make_unique<FileOutputStream>(file);
        
        if (fileStream != nullptr)
        {
            // Now create a WAV writer object that writes to our output stream...
            
            // variable format test
            AudioFormatWriter * writer = formatManager.getKnownFormat(recordFileFormatIndex)->createWriterFor (fileStream.release(), sampleRate, 2, 24, {}, 0);

            if (writer != nullptr)
            {
                // Now we'll create one of these helper objects which will act as a FIFO buffer, and will
                // write the data to disk on our background thread.
                threadedWriter = std::make_unique<AudioFormatWriter::ThreadedWriter>(writer, backgroundThread, 32768);

                // And now, swap over our active writer pointer so that the audio callback will start using it..
                const ScopedLock sl (writerLock);
                activeWriter = threadedWriter.get();
                
                recordCounterInSamples = 0;
            }
        }
    }
}

void ProjectManager::startRecording ()
{
    shouldRecord    = true;
    
//    logFileWriter->startRecordingLog();
}

void ProjectManager::stopRecording()
{
    // First, clear this pointer to stop the audio callback from using our writer object..
    {
        const ScopedLock sl (writerLock);
        activeWriter = nullptr;
    }

    // Now we can delete the writer object. It's done in this order because the deletion could
    // take a little time while remaining data gets flushed to disk, so it's best to avoid blocking
    // the audio callback while this happens.
    threadedWriter.reset();
    
    shouldRecord    = false;
    
    
    
}

bool ProjectManager::isRecording() const
{
    return activeWriter != nullptr;
}


//==============================================================================
#pragma mark Log File Writer
//==============================================================================

void ProjectManager::LogFileWriter::createNewFileForRealtimeAnalysisLogging()
{
//    // start recording in projectManager
//    Time time = Time::getCurrentTime();
//
//    String filename("RealtimeAnalysisLog-");
//    String M(time.getMinutes());
//    String HH(time.getHours());
//    String DD(time.getDayOfMonth());
//    String MM(time.getMonth() + 1);
//    String YY(time.getYear());
//
//    filename.append(DD, 4); filename.append("-", 3);
//    filename.append(MM, 4); filename.append("-", 3);
//    filename.append(YY, 4); filename.append("-", 3);
//    filename.append(HH, 4); filename.append("-", 3);
//    filename.append(M, 4);
//
//    String url(logFileDirectory->getFullPathName());
//
//    url.append("/", 2);
//    url.append(filename, 100);
//    url.append(".txt", 4);
//
//    File newLogFile(url);
//    newLogFile.create();
    
    Time time = Time::getCurrentTime();

    int hours = time.getHours(); String h(hours); String HH;
    if (hours < 10) { HH.append("0", 1);  HH.append(h, 1); }
    else { HH.append(h, 2); }
    
    String M(time.getMinutes());
    String DD(time.getDayOfMonth());
    String MM(time.getMonth() + 1);
    String YY(time.getYear());
    
    String filename("");
    filename.append(DD, 4); filename.append(".", 3);
    filename.append(MM, 4); filename.append(".", 3);
    filename.append(YY, 4); filename.append("-", 3);
    filename.append(HH, 4); filename.append("-", 3);
    filename.append(M, 4);  filename.append("-", 3);
    
    filename.append("Realtime-Analysis", 30);
    
    String url(logFileDirectory->getFullPathName());
    
    url.append("/", 2);
    url.append(filename, 100);
    url.append(".txt", 4);
    
    File newLogFile(url);
    newLogFile.create();
    
    currentLogFile_RealtimeAnalysis = newLogFile;
}

//==============================================================================
void ProjectManager::LogFileWriter::setupLogRecording (const File& file)
{
    currentLogFile_RealtimeAnalysis  = file;
    logString       = "";
}



void ProjectManager::LogFileWriter::processLog(double peakFrequency, double peakDB, Array<float> upperHarmonics, Array<float> intervals, int keynote, int octave, double ema)
{
    if (shouldRecordLog)
    {
        String newEntry("\n\n");
        
        newEntry.append(getDateAndTimeString(), 100); newEntry.append(" | ", 3);
        
        // grab values

        String peakFreqString;
        String f(peakFrequency, 3, false);
        peakFreqString.append(f, 20);
        peakFreqString.append("hz", 2);
        
        String peakDbString;
        String d(peakDB, 3, false);
        peakDbString.append(d, 20);
        peakDbString.append("dB", 2);
        
        int midiNote    = -1;
        float freqDif   = 0.f;
        
        projectManager->frequencyManager->getMIDINoteForFrequency(peakFrequency, midiNote, keynote, octave, freqDif);
        
        String keynoteString;
        
        if (midiNote >= 0)
        {
            String key(ProjectStrings::getKeynoteArray().getReference(keynote));
            String octString(octave-1);
            key.append(octString, 2);
            
            if (freqDif > 0.f)
            {
                key.append(" +", 3);
            }
            else
            {
                key.append(" ", 3);
            }
            
            key.append(String(freqDif, 3, false), 7);
            key.append(" hz", 3);

            keynoteString.append(key, 20);
        }
        
        String emaString;
        String emaRes(ema, 3, false); emaRes.append(" hz", 3);
        emaString.append(emaRes, 20);
        
        newEntry.append(peakFreqString, 100); newEntry.append(" | ", 3);
        newEntry.append(emaString, 100); newEntry.append(" | ", 3);
        newEntry.append(peakDbString, 100); newEntry.append(" | ", 3);
        newEntry.append(keynoteString, 100); newEntry.append(" | ", 3);
        
        // chord
        
        // harmonic 1, 2, 3, 4, 5
        
        String harmonicString1(upperHarmonics.getReference(1), 3, false); harmonicString1.append("hz", 2);
        String harmonicString2(upperHarmonics.getReference(2), 3, false); harmonicString2.append("hz", 2);
        String harmonicString3(upperHarmonics.getReference(3), 3, false); harmonicString3.append("hz", 2);
        String harmonicString4(upperHarmonics.getReference(4), 3, false); harmonicString4.append("hz", 2);
        String harmonicString5(upperHarmonics.getReference(5), 3, false); harmonicString5.append("hz", 2);
        
        newEntry.append(harmonicString1, 20); newEntry.append(" | ", 3);
        newEntry.append(harmonicString2, 20); newEntry.append(" | ", 3);
        newEntry.append(harmonicString3, 20); newEntry.append(" | ", 3);
        newEntry.append(harmonicString4, 20); newEntry.append(" | ", 3);
        newEntry.append(harmonicString5, 20); newEntry.append(" | ", 3);
        
        String intervalString1(intervals.getReference(1), 3, false); intervalString1.append("hz", 2);
        String intervalString2(intervals.getReference(2), 3, false); intervalString2.append("hz", 2);
        String intervalString3(intervals.getReference(3), 3, false); intervalString3.append("hz", 2);
        String intervalString4(intervals.getReference(4), 3, false); intervalString4.append("hz", 2);
        String intervalString5(intervals.getReference(5), 3, false); intervalString5.append("hz", 2);
        
        newEntry.append(intervalString1, 20); newEntry.append(" | ", 3);
        newEntry.append(intervalString2, 20); newEntry.append(" | ", 3);
        newEntry.append(intervalString3, 20); newEntry.append(" | ", 3);
        newEntry.append(intervalString4, 20); newEntry.append(" | ", 3);
        newEntry.append(intervalString5, 20);

        logString.append(newEntry, 500);
    }
}

void ProjectManager::LogFileWriter::startRecordingLog ()
{
    shouldRecordLog = true;
    
    logString.clear();
}

void ProjectManager::LogFileWriter::stopRecordingLog()
{
    shouldRecordLog = false;
    
    currentLogFile_RealtimeAnalysis.appendText(logString);
}


bool ProjectManager::LogFileWriter::isRecordingLog()
{
    return shouldRecordLog;
}


// When audioMode changes, create a new log file timestamped...
void ProjectManager::LogFileWriter::initNewLogFileForAudioMode(AUDIO_MODE newMode)
{
    // delete old file is it contains zero data
    if (logAudioMode != newMode)
    {
        if (logFile[logAudioMode].existsAsFile() && logFile[logAudioMode].getSize() == 0)
        {
            logFile[logAudioMode].deleteFile();
        }

        logAudioMode = newMode;
    }

    const auto createNewFile = [&](const String& identifier)
    {
        Time time = Time::getCurrentTime();

        int hours = time.getHours(); String h(hours); String HH;
        if (hours < 10) { HH.append("0", 1);  HH.append(h, 1); }
        else { HH.append(h, 2); }
        
        String M(time.getMinutes());
        String DD(time.getDayOfMonth());
        String MM(time.getMonth() + 1);
        String YY(time.getYear());
        
        String filename("");
        filename.append(DD, 4); filename.append(".", 3);
        filename.append(MM, 4); filename.append(".", 3);
        filename.append(YY, 4); filename.append("-", 3);
        filename.append(HH, 4); filename.append("-", 3);
        filename.append(M, 4);  filename.append("-", 3);
        
        filename.append(identifier, 30);
        
        String url(logFileDirectory->getFullPathName());
        
        url.append("/", 2);
        url.append(filename, 100);
        url.append(".txt", 4);
        
        File newLogFile(url);
        newLogFile.create();
        return newLogFile;
    };
    
    if (newMode == AUDIO_MODE::MODE_CHORD_PLAYER)
    {
        logFile[logAudioMode] = createNewFile("Chord-Player");
    }
    else if (newMode == AUDIO_MODE::MODE_CHORD_SCANNER)
    {
        logFile[logAudioMode] = createNewFile("Chord-Scanner");
    }
    else if (newMode == AUDIO_MODE::MODE_FREQUENCY_PLAYER)
    {
        logFile[logAudioMode] = createNewFile("Frequency-Player");
    }
    else if (newMode == AUDIO_MODE::MODE_FREQUENCY_SCANNER)
    {
        logFile[logAudioMode] = createNewFile("Frequency-Scanner");
    }
    else if (newMode == AUDIO_MODE::MODE_FREQUENCY_TO_LIGHT)
    {
        logFile[logAudioMode] = createNewFile("Frequency-To-Light");
    }
    else if (newMode == AUDIO_MODE::MODE_REALTIME_ANALYSIS)
    {
        // has own functions for recording...
    }
    else if (newMode == AUDIO_MODE::MODE_LISSAJOUS_CURVES)
    {
        // Doesnt log yet ***
    }
    else if (newMode == AUDIO_MODE::MODE_FUNDAMENTAL_FREQUENCY)
    {
        logFile[logAudioMode] = createNewFile("Fundamental-Frequency");
    }
}

void ProjectManager::LogFileWriter::processLog_ChordPlayer_Parameters()
{
    // called every parameter change
    String outputString;

    // print parameters for each active chord
    for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
    {
        String newEntry("\n\n");
        
        bool active = projectManager->chordPlayerParameters[i]->getProperty(projectManager->getIdentifierForChordPlayerParameterIndex(SHORTCUT_IS_ACTIVE));
        
        if (active)
        {
            newEntry.append(getDateAndTimeString(), 100); newEntry.append(" | ", 3);
            
            String parameterString("CHORD PLAYER SHORTCUT UNIT ");
            String unitString(i); parameterString.append(unitString, 3);
            
            newEntry.append(parameterString, 30); newEntry.append(" | ", 3);
            
            
            // Waveform Type
            String string_Waveform;
            String string_Instrument;
            
            int waveformType = projectManager->getChordPlayerParameter(i, WAVEFORM_TYPE).operator int();
            if (waveformType == 0) // playing instrument
            {
                // PLAYING_INSTRUMENT
                int instrumentType = projectManager->getChordPlayerParameter(i, INSTRUMENT_TYPE).operator int() - 1;
                
                // Use synthesis-based instrument library
                String instString;
                
                // Map instrument types to synthesis-based instruments (no file system access needed)
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
                
                String instName = "Grand Piano"; // Default
                if (instrumentType >= 0 && instrumentType < synthInstruments.size())
                {
                    instName = synthInstruments[instrumentType];
                }
                
                string_Waveform     = "N/A";
                string_Instrument   = instName;
            }
            else if (waveformType == 1) { string_Waveform = "Sinewave";  string_Instrument = "N/A"; }
            else if (waveformType == 2) { string_Waveform = "Triangle";  string_Instrument = "N/A"; }
            else if (waveformType == 3) { string_Waveform = "Square";    string_Instrument = "N/A"; }
            else if (waveformType == 4) { string_Waveform = "Sawtooth";  string_Instrument = "N/A"; }
            else if (waveformType == 5) { string_Waveform = "Wavetable"; string_Instrument = "N/A"; }
            
            
            // KEY NOTE
            int keyNote = projectManager->getChordPlayerParameter(i, KEYNOTE).operator int() - 1;
            
            String string_Keynote(ProjectStrings::getKeynoteArray().getReference(keyNote));
            
            int octave = projectManager->getChordPlayerParameter(i, OCTAVE).operator int();
            
            String octaveString(octave);
            
            // CHORD_TYPE
            int chordType = projectManager->getChordPlayerParameter(i, CHORD_TYPE).operator int() - 1;
            
            String stringChordtype(ProjectStrings::getChordTypeArray().getReference(chordType));

            // notes in chord
            Array<double> noteFreqs     = projectManager->chordPlayerProcessor->chordManager[i]->getFrequenciesForChord();
            Array<int> notes            = projectManager->chordPlayerProcessor->chordManager[i]->getMIDIKeysForChord();
            
            
            // MANIPULATION_MULTIPLICATION
            bool manipulateFreq     = projectManager->getChordPlayerParameter(i, MANIPULATE_CHOSEN_FREQUENCY).operator bool();
            bool multOrDivide       = projectManager->getChordPlayerParameter(i, MULTIPLY_OR_DIVISION).operator bool();

            float multVal = projectManager->getChordPlayerParameter(i, MULTIPLY_VALUE).operator float();
            String multString(multVal);
            
            float divVal = projectManager->getChordPlayerParameter(i, DIVISION_VALUE).operator float();
            String divString(divVal);

            // ADSR
            double amplitude        = projectManager->getChordPlayerParameter(i, ENV_AMPLITUDE).operator double();
            double attack           = projectManager->getChordPlayerParameter(i, ENV_ATTACK).operator double();
            double sustain          = projectManager->getChordPlayerParameter(i, ENV_SUSTAIN).operator double();
            double decay            = projectManager->getChordPlayerParameter(i, ENV_DECAY).operator double();
            double release          = projectManager->getChordPlayerParameter(i, ENV_RELEASE).operator double();
            
            String ampString(amplitude, 1, false);          ampString.append("%", 1);
            String attackString(attack);                    attackString.append("ms", 2);
            String sustainString(sustain, 1, false);        sustainString.append("%", 1);
            String decayString(decay);                      decayString.append("ms", 2);
            String releaseString(release);                  releaseString.append("ms", 2);

            int pauseVal        = projectManager->getChordPlayerParameter(i, NUM_PAUSE).operator int();
            String pauseString(pauseVal); pauseString.append("ms", 2);

            int lengthVal       = projectManager->getChordPlayerParameter(i, NUM_DURATION).operator int();
            String lengthString(lengthVal); lengthString.append("ms", 2);
            
            int numRepeats      = projectManager->getChordPlayerParameter(i, NUM_REPEATS).operator int();
            String repeatsString(numRepeats);
            
            double baseAFrequency = projectManager->frequencyManager->getBaseAFrequency();
            String baseFreqAString(baseAFrequency, 3, false);
            
            String stringBaseA("A4 = ", 6);
            stringBaseA.append(baseFreqAString, 12);
            stringBaseA.append("hz", 3);
            
            int scaleRef = (int)projectManager->getProjectSettingsParameter(TSS_SETTINGS::DEFAULT_SCALE);
            
            String stringScale(getScaleString(scaleRef));
            

            // add to new Entry for individual chordplayer shortcut unit
            
            // chord type
            newEntry.append(string_Keynote, 20); newEntry.append(" | ", 3);
            
            // chord type
            newEntry.append(stringChordtype, 20); newEntry.append(" | ", 3);
            
            if (manipulateFreq)
            {
                if (!multOrDivide)
                {
                    String manValue(multVal, 3);
                    
                    newEntry.append(manValue, 10);
                    
                    newEntry.append(" x | ", 5);
                }
                else
                {
                    double d = 1.f / divVal;
                    
                    String divValueS(d, 3);
                    
                    newEntry.append(divValueS, 10);
                    
                    newEntry.append(" x | ", 5);
                }
            }
            else
            {
                newEntry.append("1.0 x | ", 10);
            }
            
            // octave
            newEntry.append(octaveString, 4); newEntry.append(" | ", 3);
            
            // waveform
            newEntry.append(string_Waveform, 50); newEntry.append(" | ", 3);
            
            // instrument
            newEntry.append(string_Instrument, 50); newEntry.append(" | ", 3);

            // ADSR
            newEntry.append(ampString, 10);     newEntry.append(" | ", 3);
            newEntry.append(attackString, 10);  newEntry.append(" | ", 3);
            newEntry.append(sustainString, 10); newEntry.append(" | ", 3);
            newEntry.append(decayString, 10);   newEntry.append(" | ", 3);
            newEntry.append(releaseString, 10); newEntry.append(" | ", 3);
            
            newEntry.append(lengthString, 10);  newEntry.append(" | ", 3);
            newEntry.append(pauseString, 10);   newEntry.append(" | ", 3);
            newEntry.append(repeatsString, 10); newEntry.append(" | ", 3);
            newEntry.append(stringBaseA, 16); newEntry.append(" | ", 3);
            newEntry.append(stringScale, 100);
            
            // add to output string
            outputString.append(newEntry, 1000);
        }
    }
    
    logFile[AUDIO_MODE::MODE_CHORD_PLAYER].appendText(outputString);
}

void ProjectManager::LogFileWriter::processLog_ChordPlayer_Sequencer(int shortcutRef, const Array<String>& noteStrings, const Array<float>& noteFreqs)
{
    // pushes chord and note frequencis to the log....
    jassert(noteStrings.size() == noteFreqs.size());
    String outputString("\n\n");
    
    outputString.append(getDateAndTimeString(), 100); outputString.append(" | ", 3);

    String chordString("CHORD PLAYER OUTPUT UNIT ");
    String unitString(shortcutRef);
    chordString.append(unitString, 2);
    chordString.append(" | ", 3);
    outputString.append(chordString, 30);
    
    Array<int> notes            = projectManager->chordPlayerProcessor->chordManager[shortcutRef]->getMIDIKeysForChord();


    String stringNoteFrequencies;
    for (int i = 0; i < notes.size(); i++)
    {
        String keyNote = noteStrings[i];
        String keyFreqString(noteFreqs.getReference(i), 3, false); keyFreqString.append("hz", 2);

        stringNoteFrequencies.append(keyNote, 4);
        stringNoteFrequencies.append(" ", 4);
        stringNoteFrequencies.append(keyFreqString, 10);
        
        if (i < notes.size() - 1)
        {
            stringNoteFrequencies.append(" - ", 4);
        }
    }
    
    outputString.append(stringNoteFrequencies, 200);

    Array<float> upperHarmonics;
    Array<float> intervals;

    for (int i = 0; i < 6; i++) {
        float c = 0.f; upperHarmonics.add(c); intervals.add(c);
    }

//    projectManager->outputAnalyser.getFrequencyData(peakFrequency, peakDB, upperHarmonics, intervals, ema);

    String newEntry(" | ");

    // add harmonics
    String harmonicString1(upperHarmonics.getReference(1), 3, false); harmonicString1.append("hz", 2);
    String harmonicString2(upperHarmonics.getReference(2), 3, false); harmonicString2.append("hz", 2);
    String harmonicString3(upperHarmonics.getReference(3), 3, false); harmonicString3.append("hz", 2);
    String harmonicString4(upperHarmonics.getReference(4), 3, false); harmonicString4.append("hz", 2);
    String harmonicString5(upperHarmonics.getReference(5), 3, false); harmonicString5.append("hz", 2);

    newEntry.append(harmonicString1, 20); newEntry.append(" | ", 3);
    newEntry.append(harmonicString2, 20); newEntry.append(" | ", 3);
    newEntry.append(harmonicString3, 20); newEntry.append(" | ", 3);
    newEntry.append(harmonicString4, 20); newEntry.append(" | ", 3);
    newEntry.append(harmonicString5, 20); newEntry.append(" | ", 3);

    String intervalString1(intervals.getReference(1), 3, false); intervalString1.append("hz", 2);
    String intervalString2(intervals.getReference(2), 3, false); intervalString2.append("hz", 2);
    String intervalString3(intervals.getReference(3), 3, false); intervalString3.append("hz", 2);
    String intervalString4(intervals.getReference(4), 3, false); intervalString4.append("hz", 2);
    String intervalString5(intervals.getReference(5), 3, false); intervalString5.append("hz", 2);

    newEntry.append(intervalString1, 20); newEntry.append(" | ", 3);
    newEntry.append(intervalString2, 20); newEntry.append(" | ", 3);
    newEntry.append(intervalString3, 20); newEntry.append(" | ", 3);
    newEntry.append(intervalString4, 20); newEntry.append(" | ", 3);
    newEntry.append(intervalString5, 20);

    outputString.append(newEntry, 200);
    
    
    logFile[AUDIO_MODE::MODE_CHORD_PLAYER].appendText(outputString);
}

// ChordScanner
void ProjectManager::LogFileWriter::processLog_ChordScanner_Parameters()
{
    // Called
    // called every parameter change
    String newEntry("\n\n");
    // add to new Entry for individual chordplayer shortcut unit
    newEntry.append(getDateAndTimeString(), 100); newEntry.append(" | ", 3);
    
    String parameterString("CHORD SCANNER PARAMETERS");
    
    newEntry.append(parameterString, 30); newEntry.append(" | ", 3);
    
    int chordScanType   = projectManager->getChordScannerParameter(CHORD_SCANNER_MODE).operator int();
    String chordScanString;
    
    if (chordScanType == 0)
    {
        chordScanString = "Scan Only Main Chords ";
    }
    else if (chordScanType == 1)
    {
        chordScanString = "Scan All Chords ";
    }
    else if (chordScanType == 2)
    {
        chordScanString = "Scan Specific Range ";
    }
    else if (chordScanType == 3)
    {
        chordScanString = "Scan by Frequency ";
    }

    // Waveform Type
    String string_Waveform;
    String string_Instrument;
    
    int waveformType = projectManager->getChordScannerParameter(CHORD_SCANNER_WAVEFORM_TYPE).operator int();
    if (waveformType == 0) // playing instrument
    {
        // PLAYING_INSTRUMENT
        int instrumentType = projectManager->getChordScannerParameter(CHORD_SCANNER_INSTRUMENT_TYPE).operator int() - 1;
        
        // Use synthesis-based instrument library
        String instString;
        
        // Map instrument types to synthesis-based instruments (no file system access needed)
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
        
        String instName = "Grand Piano"; // Default
        if (instrumentType >= 0 && instrumentType < synthInstruments.size())
        {
            instName = synthInstruments[instrumentType];
        }
        
        string_Waveform     = "N/A";
        string_Instrument   = instName;
    }
    else if (waveformType == 1) { string_Waveform = "Sinewave";  string_Instrument = "N/A"; }
    else if (waveformType == 2) { string_Waveform = "Triangle";  string_Instrument = "N/A"; }
    else if (waveformType == 3) { string_Waveform = "Square";    string_Instrument = "N/A"; }
    else if (waveformType == 4) { string_Waveform = "Sawtooth";  string_Instrument = "N/A"; }
    else if (waveformType == 5) { string_Waveform = "Wavetable"; string_Instrument = "N/A"; }
    

    int keynoteTo       = projectManager->getChordScannerParameter(CHORD_SCANNER_KEYNOTE_TO).operator int() - 1;
    String stringKeynoteTo(ProjectStrings::getKeynoteArray().getReference(keynoteTo));

    int keynoteFrom     = projectManager->getChordScannerParameter(CHORD_SCANNER_KEYNOTE_FROM).operator int() - 1;
    String stringKeynoteFrom(ProjectStrings::getKeynoteArray().getReference(keynoteFrom));
    
    int octaveTo        = projectManager->getChordScannerParameter(CHORD_SCANNER_OCTAVE_TO).operator int();
    String stringOctaveTo(octaveTo);
    
    int octaveFrom      = projectManager->getChordScannerParameter(CHORD_SCANNER_OCTAVE_FROM).operator int();
    String stringOctaveFrom(octaveFrom);
    
    double freqToVal = projectManager->getChordScannerParameter(CHORD_SCANNER_FREQUENCY_TO).operator double();
    String freqToString(freqToVal, 3, false);
    freqToString.append("hz", 2);

    double freqFromVal = projectManager->getChordScannerParameter(CHORD_SCANNER_FREQUENCY_FROM).operator double();
    String freqFromString(freqFromVal, 3, false);
    freqFromString.append("hz", 2);

    int pauseVal = projectManager->getChordScannerParameter(CHORD_SCANNER_NUM_PAUSE).operator int();
    String pauseString(pauseVal);
    pauseString.append("ms", 2);

    int lengthVal = projectManager->getChordScannerParameter(CHORD_SCANNER_NUM_DURATION).operator int();
    String lengthString(lengthVal);
    lengthString.append("ms", 2);
    
    int repeatVal = projectManager->getChordScannerParameter(CHORD_SCANNER_NUM_REPEATS).operator int();
    String repeatString(repeatVal);

    

    double amplitude    = projectManager->getChordScannerParameter(CHORD_SCANNER_ENV_AMPLITUDE).operator double();
    double attack       = projectManager->getChordScannerParameter(CHORD_SCANNER_ENV_ATTACK).operator double();
    double sustain      = projectManager->getChordScannerParameter(CHORD_SCANNER_ENV_SUSTAIN).operator double();
    double decay        = projectManager->getChordScannerParameter(CHORD_SCANNER_ENV_DECAY).operator double();
    double release      = projectManager->getChordScannerParameter(CHORD_SCANNER_ENV_RELEASE).operator double();
    
    double baseAFrequency = projectManager->frequencyManager->getBaseAFrequency();
    String baseFreqAString(baseAFrequency, 3, false);
    
    String stringBaseA("A4 = ", 6);
    stringBaseA.append(baseFreqAString, 12);
    stringBaseA.append("hz", 3);
    
    int scaleRef = (int)projectManager->getProjectSettingsParameter(TSS_SETTINGS::DEFAULT_SCALE);
    
    String stringScale(getScaleString(scaleRef));
    
    String ampString(amplitude, 1, false);          ampString.append("%", 1);
    String attackString(attack);                    attackString.append("ms", 2);
    String sustainString(sustain, 1, false);        sustainString.append("%", 1);
    String decayString(decay);                      decayString.append("ms", 2);
    String releaseString(release);                  releaseString.append("ms", 2);
    
    newEntry.append(" ", 1);
    newEntry.append(chordScanString, 50); newEntry.append(" | ", 3);
    newEntry.append(stringKeynoteFrom, 20);     newEntry.append(" | ", 3);
    newEntry.append(stringKeynoteTo, 20);       newEntry.append(" | ", 3);
    newEntry.append(stringOctaveFrom, 20);      newEntry.append(" | ", 3);
    newEntry.append(stringOctaveTo, 20);        newEntry.append(" | ", 3);
    newEntry.append(string_Waveform, 50);   newEntry.append(" | ", 3);
    newEntry.append(string_Instrument, 50); newEntry.append(" | ", 3);
    newEntry.append(ampString, 10);     newEntry.append(" | ", 3);
    newEntry.append(attackString, 10);  newEntry.append(" | ", 3);
    newEntry.append(sustainString, 10); newEntry.append(" | ", 3);
    newEntry.append(decayString, 10);   newEntry.append(" | ", 3);
    newEntry.append(releaseString, 10); newEntry.append(" | ", 3);
    newEntry.append(repeatString, 10);  newEntry.append(" | ", 3);
    newEntry.append(lengthString, 10);  newEntry.append(" | ", 3);
    newEntry.append(pauseString, 10);   newEntry.append(" | ", 3);
    newEntry.append(stringBaseA, 16); newEntry.append(" | ", 3);
    newEntry.append(stringScale, 30);

    logFile[AUDIO_MODE::MODE_CHORD_SCANNER].appendText(newEntry);
}

void ProjectManager::LogFileWriter::processLog_ChordScanner_Sequencer(bool isAllChords, Array<int> notes, Array<float> noteFreqs)
{
    // only chord and note frequencies
    String newEntry("\n\n");

    newEntry.append(getDateAndTimeString(), 100);
    
    newEntry.append(" | ", 3); newEntry.append("CHORD SCANNER OUTPUT", 15); newEntry.append(" | ", 3);
    
    String stringNoteFrequencies;
    
    int octaveIterator          = projectManager->chordScannerProcessor->repeater->octaveIterator;
    int chordType               = projectManager->chordScannerProcessor->repeater->chordTypeIterator - 1;
    int keynote                 = projectManager->chordScannerProcessor->repeater->keynoteIterator - 1;
   
    for (int i = 0; i < notes.size(); i++)
    {
        // get the note
        int key         = notes.getReference(i);
        int octave      = octaveIterator;
 
        String keyNote(ProjectStrings::getKeynoteArray().getReference(key));
        String oct(octave); keyNote.append(oct, 2);
    
        String keyFreqString(noteFreqs.getReference(i), 3, false); keyFreqString.append("hz", 2);
      
        stringNoteFrequencies.append(keyNote, 4);
        stringNoteFrequencies.append(" ", 4);
        stringNoteFrequencies.append(keyFreqString, 30);
        
        if (i < notes.size() - 1)
        {
            stringNoteFrequencies.append(" - ", 4);
        }
        
    }
    
    String stringKeynote(ProjectStrings::getKeynoteArray().getReference(keynote));
    String stringOctave(octaveIterator);
    String stringChordtype(ProjectStrings::getChordTypeArray().getReference(chordType));
    
    if (!isAllChords)
    {
        newEntry.append(stringKeynote, 10);     newEntry.append(" ", 1);
        newEntry.append(stringChordtype, 20);   newEntry.append(" | ", 3);
    }

    
    // note frequencies
    newEntry.append(stringNoteFrequencies, 300);
    Array<float> upperHarmonics;
    Array<float> intervals;

    for (int i = 0; i < 6; i++)
    {
        float c = 0.f; upperHarmonics.add(c); intervals.add(c);
    }

    newEntry.append(" | ", 3);

    // add harmonics
    String harmonicString1(upperHarmonics.getReference(1), 3, false); harmonicString1.append("hz", 2);
    String harmonicString2(upperHarmonics.getReference(2), 3, false); harmonicString2.append("hz", 2);
    String harmonicString3(upperHarmonics.getReference(3), 3, false); harmonicString3.append("hz", 2);
    String harmonicString4(upperHarmonics.getReference(4), 3, false); harmonicString4.append("hz", 2);
    String harmonicString5(upperHarmonics.getReference(5), 3, false); harmonicString5.append("hz", 2);

    newEntry.append(harmonicString1, 20); newEntry.append(" | ", 3);
    newEntry.append(harmonicString2, 20); newEntry.append(" | ", 3);
    newEntry.append(harmonicString3, 20); newEntry.append(" | ", 3);
    newEntry.append(harmonicString4, 20); newEntry.append(" | ", 3);
    newEntry.append(harmonicString5, 20); newEntry.append(" | ", 3);

    String intervalString1(intervals.getReference(1), 3, false); intervalString1.append("hz", 2);
    String intervalString2(intervals.getReference(2), 3, false); intervalString2.append("hz", 2);
    String intervalString3(intervals.getReference(3), 3, false); intervalString3.append("hz", 2);
    String intervalString4(intervals.getReference(4), 3, false); intervalString4.append("hz", 2);
    String intervalString5(intervals.getReference(5), 3, false); intervalString5.append("hz", 2);

    newEntry.append(intervalString1, 20); newEntry.append(" | ", 3);
    newEntry.append(intervalString2, 20); newEntry.append(" | ", 3);
    newEntry.append(intervalString3, 20); newEntry.append(" | ", 3);
    newEntry.append(intervalString4, 20); newEntry.append(" | ", 3);
    newEntry.append(intervalString5, 20);
    

    logFile[AUDIO_MODE::MODE_CHORD_SCANNER].appendText(newEntry);
}

// FrequencyPlayer
void ProjectManager::LogFileWriter::processLog_FrequencyPlayer_Parameters()
{
    String outputString;
    
    for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
    {
        bool active = projectManager->getFrequencyPlayerParameter(i, FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE).operator bool();
        
        if (active)
        {
            String newEntry("\n\n");
            
            newEntry.append(getDateAndTimeString(), 100); newEntry.append(" | ", 3);
            
            String parameterString("FREQUENCY PLAYER PARAMETERS UNIT "); String unit(i);
            parameterString.append(unit, 2);
            
            newEntry.append(parameterString, 50);
            
            newEntry.append(" | ", 3);
            
            
            bool source = projectManager->getFrequencyPlayerParameter(i, FREQUENCY_PLAYER_FREQ_SOURCE).operator bool();
            
            String sourceString;
            
            if (!source)
            {
                sourceString = "Specific Frequency";
            }
            else
            {
                sourceString = "Range of Frequencies";
            }
            
            
            // Waveform Type
            String string_Waveform;
            
            int waveformType = projectManager->getFrequencyPlayerParameter(i, FREQUENCY_PLAYER_WAVEFORM_TYPE).operator int();
            
            if (waveformType == 0)      { string_Waveform = "Default"; }
            else if (waveformType == 1) { string_Waveform = "Sinewave"; }
            else if (waveformType == 2) { string_Waveform = "Triangle"; }
            else if (waveformType == 3) { string_Waveform = "Square"; }
            else if (waveformType == 4) { string_Waveform = "Sawtooth"; }
            else if (waveformType == 5) { string_Waveform = "Wavetable"; }

            int pauseVal = projectManager->getFrequencyPlayerParameter(i, FREQUENCY_PLAYER_NUM_PAUSE).operator int();
            String pauseString(pauseVal);
            pauseString.append("ms", 2);

            int lengthVal = projectManager->getFrequencyPlayerParameter(i, FREQUENCY_PLAYER_NUM_DURATION).operator int();
            String lengthString(lengthVal);
            lengthString.append("ms", 2);
            
            int numRepeats      = projectManager->getFrequencyPlayerParameter(i, FREQUENCY_PLAYER_NUM_REPEATS).operator int();
            String repeatsString(numRepeats);
            

            double amplitude    = projectManager->getFrequencyPlayerParameter(i, FREQUENCY_PLAYER_AMPLITUDE).operator double();
            double attack       = projectManager->getFrequencyPlayerParameter(i, FREQUENCY_PLAYER_ATTACK).operator double();
            double sustain      = projectManager->getFrequencyPlayerParameter(i, FREQUENCY_PLAYER_SUSTAIN).operator double();
            double decay        = projectManager->getFrequencyPlayerParameter(i, FREQUENCY_PLAYER_DECAY).operator double();
            double release      = projectManager->getFrequencyPlayerParameter(i, FREQUENCY_PLAYER_RELEASE).operator double();
            
            String ampString(amplitude, 1, false);          ampString.append("%", 1);
            String attackString(attack);                    attackString.append("ms", 2);
            String sustainString(sustain, 1, false);        sustainString.append("%", 1);
            String decayString(decay);                      decayString.append("ms", 2);
            String releaseString(release);                  releaseString.append("ms", 2);
            
            
            // MANIPULATION_MULTIPLICATION
            bool manipulateFreq     = projectManager->getFrequencyPlayerParameter(i, FREQUENCY_PLAYER_MANIPULATE_CHOSEN_FREQUENCY).operator bool();
            bool multOrDivide       = projectManager->getFrequencyPlayerParameter(i, FREQUENCY_PLAYER_MULTIPLY_OR_DIVISION).operator bool();

            float multVal = projectManager->getFrequencyPlayerParameter(i, FREQUENCY_PLAYER_MULTIPLY_VALUE).operator float();
            String multString(multVal, 3, false);
            
            float divVal = projectManager->getFrequencyPlayerParameter(i, FREQUENCY_PLAYER_DIVISION_VALUE).operator float();
            String divString(divVal, 3, false);
            
            
            // TextEntryBoxes
            double insertFreqVal = projectManager->getFrequencyPlayerParameter(i, FREQUENCY_PLAYER_CHOOSE_FREQ).operator double();
            String freqString(insertFreqVal, 3, false); freqString.append("Hz", 2);

            double minFreqVal = projectManager->getFrequencyPlayerParameter(i, FREQUENCY_PLAYER_RANGE_MIN).operator double();
            String minFreqString(minFreqVal, 3, false); minFreqString.append("Hz", 2);
            
            double maxFreqVal = projectManager->getFrequencyPlayerParameter(i, FREQUENCY_PLAYER_RANGE_MAX).operator double();
            String maxFreqString(maxFreqVal, 3, false); maxFreqString.append("Hz", 2);

            
            // Specific Frequencies
            newEntry.append(sourceString, 40);  newEntry.append(" | ", 3);
            newEntry.append(freqString, 10);    newEntry.append(" | ", 3);
            newEntry.append(minFreqString, 10); newEntry.append(" - ", 3);
            newEntry.append(maxFreqString, 10); newEntry.append(" | ", 3);
            
            // wavecform
            newEntry.append(string_Waveform, 50); newEntry.append(" | ", 3);
            
            if (manipulateFreq)
            {
                if (!multOrDivide)
                {
                    String manValue(multVal, 3);
                    
                    newEntry.append(manValue, 10);
                    
                    newEntry.append(" x | ", 5);
                }
                else
                {
                    double d = 1.f / divVal;
                    
                    String divValueS(d, 3);
                    
                    newEntry.append(divValueS, 10);
                    
                    newEntry.append(" x | ", 5);
                }
            }
            else
            {
                newEntry.append("1.0 x | ", 10);
            }
            
            // ADSR
            newEntry.append(ampString, 10);     newEntry.append(" | ", 3);
            newEntry.append(attackString, 10);  newEntry.append(" | ", 3);
            newEntry.append(sustainString, 10); newEntry.append(" | ", 3);
            newEntry.append(decayString, 10);   newEntry.append(" | ", 3);
            newEntry.append(releaseString, 10); newEntry.append(" | ", 3);
            newEntry.append(repeatsString, 10); newEntry.append(" | ", 3);
            newEntry.append(lengthString, 10);  newEntry.append(" | ", 3);
            newEntry.append(pauseString, 10);   newEntry.append(" | ", 3);
            
            // add to output string
            outputString.append(newEntry, 1000);
            
        }
    }
    
    logFile[AUDIO_MODE::MODE_FREQUENCY_PLAYER].appendText(outputString);
    
}

void ProjectManager::LogFileWriter::processLog_FrequencyPlayer_Sequencer(int shortcutRef, float freq)
{
    // needs to get frequency from parameters
    
    // only chord and note frequencies
       String newEntry("\n\n");
       
       newEntry.append(getDateAndTimeString(), 100); newEntry.append(" | ", 3);
        newEntry.append("FREQUENCY PLAYER OUTPUT UNIT ", 50);   String unit(shortcutRef); newEntry.append(unit, 3);   newEntry.append(" | ", 3);
       
       String stringFrequency(freq, 3, false);
       stringFrequency.append(" hz", 3);
       newEntry.append(stringFrequency, 40);
    
       logFile[AUDIO_MODE::MODE_FREQUENCY_PLAYER].appendText(newEntry);
}

// FrequencyScanner
void ProjectManager::LogFileWriter::processLog_FrequencyScanner_Parameters()
{
    String outputString;
    
    String newEntry("\n\n");
    
    newEntry.append(getDateAndTimeString(), 100); newEntry.append(" | ", 3);
    
    String parameterString("FREQUENCY SCANNER PARAMETERS");
    
    newEntry.append(parameterString, 30); newEntry.append(" | ", 3);

    int chordScanType   = projectManager->getFrequencyScannerParameter(FREQUENCY_SCANNER_MODE).operator int();
    String chordScanString;
    
    if (chordScanType == 0)         { chordScanString = "Scan All Frequencies"; }
    else if (chordScanType == 1)    { chordScanString = "Scan Specific Range"; }
    
    // Waveform Type
    String string_Waveform;
    
    int waveformType = projectManager->getFrequencyScannerParameter(FREQUENCY_SCANNER_WAVEFORM_TYPE).operator int();
    if (waveformType == 0)      { string_Waveform = "Default"; }
    else if (waveformType == 1) { string_Waveform = "Sinewave"; }
    else if (waveformType == 2) { string_Waveform = "Triangle";  }
    else if (waveformType == 3) { string_Waveform = "Square";  }
    else if (waveformType == 4) { string_Waveform = "Sawtooth"; }
    else if (waveformType == 5) { string_Waveform = "Wavetable"; }
    
    double freqToVal = projectManager->getFrequencyScannerParameter(FREQUENCY_SCANNER_FREQUENCY_TO).operator double();
    String freqToString(freqToVal, 3, false);
    freqToString.append("hz", 2);

    double freqFromVal = projectManager->getFrequencyScannerParameter(FREQUENCY_SCANNER_FREQUENCY_FROM).operator double();
    String freqFromString(freqFromVal, 3, false);
    freqFromString.append("hz", 2);
    
    int intervalType = projectManager->getFrequencyScannerParameter(FREQUENCY_SCANNER_LOG_LIN).operator int();
    String intervalTypeString;
    String intervalValString;
    
    double intervalVal;
    
    if (!intervalType)
    {
        intervalTypeString = "Logarithmic";
        
        intervalVal = projectManager->getFrequencyScannerParameter(FREQUENCY_SCANNER_LOG_VALUE).operator double();
        intervalValString = String(intervalVal, 3, false);
        intervalValString.append(" OCT", 4);
        
        
    }
    else
    {
        intervalTypeString = "Linear";
        
        intervalVal = projectManager->getFrequencyScannerParameter(FREQUENCY_SCANNER_LIN_VALUE).operator double();
        intervalValString = String(intervalVal, 3, false);
        intervalValString.append(" HZ", 4);
    }
    

    int pauseVal = projectManager->getFrequencyScannerParameter(FREQUENCY_SCANNER_NUM_PAUSE).operator int();
    String pauseString(pauseVal);
    pauseString.append("ms", 2);

    int lengthVal = projectManager->getFrequencyScannerParameter(FREQUENCY_SCANNER_NUM_DURATION).operator int();
    String lengthString(lengthVal);
    lengthString.append("ms", 2);
    
    int repeatVal = projectManager->getFrequencyScannerParameter(FREQUENCY_SCANNER_NUM_REPEATS).operator int();
    String repeatString(repeatVal);

    double amplitude    = projectManager->getFrequencyScannerParameter(FREQUENCY_SCANNER_ENV_AMPLITUDE).operator double();
    double attack       = projectManager->getFrequencyScannerParameter(FREQUENCY_SCANNER_ENV_ATTACK).operator double();
    double sustain      = projectManager->getFrequencyScannerParameter(FREQUENCY_SCANNER_ENV_SUSTAIN).operator double();
    double decay        = projectManager->getFrequencyScannerParameter(FREQUENCY_SCANNER_ENV_DECAY).operator double();
    double release      = projectManager->getFrequencyScannerParameter(FREQUENCY_SCANNER_ENV_RELEASE).operator double();
    
    String ampString(amplitude, 1, false);          ampString.append("%", 1);
    String attackString(attack);                    attackString.append("ms", 2);
    String sustainString(sustain, 1, false);        sustainString.append("%", 1);
    String decayString(decay);                      decayString.append("ms", 2);
    String releaseString(release);                  releaseString.append("ms", 2);
    
    
    
    // SCAN TYPE
    newEntry.append(chordScanString, 50); newEntry.append(" | ", 3);
    
    
    newEntry.append(freqFromString, 10); newEntry.append(" | ", 3);
    newEntry.append(freqToString, 10); newEntry.append(" | ", 3);
    newEntry.append(intervalTypeString, 20); newEntry.append(" | ", 3);
    newEntry.append(intervalValString, 20); newEntry.append(" | ", 3);
    
    
    
    newEntry.append(string_Waveform, 50); newEntry.append(" | ", 3);
    
    // ADSR
    newEntry.append(ampString, 10);     newEntry.append(" | ", 3);
    newEntry.append(attackString, 10);  newEntry.append(" | ", 3);
    newEntry.append(sustainString, 10); newEntry.append(" | ", 3);
    newEntry.append(decayString, 10);   newEntry.append(" | ", 3);
    newEntry.append(releaseString, 10); newEntry.append(" | ", 3);
    newEntry.append(repeatString, 10);  newEntry.append(" | ", 3);
    newEntry.append(lengthString, 10);  newEntry.append(" | ", 3);
    newEntry.append(pauseString, 10);   newEntry.append(" | ", 3);

    logFile[AUDIO_MODE::MODE_FREQUENCY_SCANNER].appendText(newEntry);
}

void ProjectManager::LogFileWriter::processLog_FrequencyScanner_Sequencer(float freq)
{
    String newEntry("\n\n");
    
    newEntry.append(getDateAndTimeString(), 100); newEntry.append(" | ", 3);
    newEntry.append("FREQUENCY SCANNER OUTPUT ", 40);      newEntry.append(" | ", 3);
    
    String stringFrequency(freq, 3, false);
    stringFrequency.append(" hz", 3);
    newEntry.append(stringFrequency, 40);
    
    logFile[AUDIO_MODE::MODE_FREQUENCY_SCANNER].appendText(newEntry);
    
}

void ProjectManager::LogFileWriter::processLog_FundamentalFrequency_Sequencer(                                                                const String& fundamental, const juce::String &chord, const Array<String>& harmonics)
{
    String newEntry("\n\n");
    newEntry.append(getDateAndTimeString(), 100);
    newEntry.append(" | ", 3);
    newEntry.append("FUNDAMENTAL FREQUENCY OUTPUT ", 40);
    newEntry.append(" | ", 3);
    
    newEntry.append(fundamental, 15);
    newEntry.append(" | ", 3);
    newEntry.append(chord, 10);
    
    for (const auto& harmonic : harmonics)
    {
        newEntry.append(" | ", 3);
        newEntry.append(harmonic, 15);
    }
    
    logFile[AUDIO_MODE::MODE_FUNDAMENTAL_FREQUENCY].appendText(newEntry);
}

void ProjectManager::LogFileWriter::processLog_FrequencyToLight(String conversionType, String base, String wavelength, String rgbHex, StringArray manipulationStrings)
{
    String newEntry("\n\n");
    
    newEntry.append(getDateAndTimeString(), 100); newEntry.append(" | ", 3);
//    newEntry.append("PARAMETERS", 40);      newEntry.append(" | ", 3);
    
    newEntry.append(conversionType, 50); newEntry.append(" | ", 3);
    newEntry.append(base, 50); newEntry.append(" | ", 3);
    newEntry.append(wavelength, 50); newEntry.append(" | ", 3);
    newEntry.append(rgbHex, 50);
    
    for (int i = 0; i < manipulationStrings.size(); i++)
    {
        newEntry.append(" | ", 3);
        newEntry.append(manipulationStrings.getReference(i), 50);
    }
    
    logFile[AUDIO_MODE::MODE_FREQUENCY_TO_LIGHT].appendText(newEntry);
}

void ProjectManager::LogFileWriter::processLog_PanicButtonPressed(int noise_type)
{
    String newEntry("\n\n");
    
    newEntry.append(getDateAndTimeString(), 130);
    
    if (noise_type == 0) // white
    {
        newEntry.append(" | WHITE NOISE", 30);
    }
    else
    {
        newEntry.append(" | PINK NOISE", 30);
    }
    
    logFile[logAudioMode].appendText(newEntry);
}

void ProjectManager::LogFileWriter::initNewSettingsLogFile()
{
    // create it
    Time time = Time::getCurrentTime();

    int hours = time.getHours(); String h(hours); String HH;
    if (hours < 10) { HH.append("0", 1);  HH.append(h, 1); }
    else { HH.append(h, 2); }
    
    String M(time.getMinutes());
    String DD(time.getDayOfMonth());
    String MM(time.getMonth() + 1);
    String YY(time.getYear());
    
    String filename("");
    filename.append(DD, 4); filename.append(".", 3);
    filename.append(MM, 4); filename.append(".", 3);
    filename.append(YY, 4); filename.append("-", 3);
    filename.append(HH, 4); filename.append("-", 3);
    filename.append(M, 4);  filename.append("-", 3);
    
    filename.append("Settings", 30);
    
    String url(logFileDirectory->getFullPathName());
    
    url.append("/", 2);
    url.append(filename, 100);
    url.append(".txt", 4);
    
    File newLogFile(url);
    newLogFile.create();
    
    logFileSettings = newLogFile;
    
}

void ProjectManager::LogFileWriter::processLog_Settings_Parameters()
{
    // dont forget load and save.. when load, print all new parameters
    
   // |DATE| NOTES FREQUENCIES| SCALE| AMPLITUDE MIN|AMPLITUDE MAX|ATTACK MIN|ATTACK MAX|DECAY MIN| DECAY MAX|SUSTAIN MIN| SUSTAIN MAX|RELEASE MIN| RELEASE MAX|FFT SIZE| FFT WINDOW TYPES|GUI SCALE|SHOW HIGHEST PEAK FREQUENCY|NUMBER OF HIGHEST PEAK FREQUENCIES|SHOW HIGHEST PEAK OCTAVES|NUMBER OF HIGHEST PEAK OCTAVES|SELECTED PLUGIN|
    
    
      // |DATE
    
    String newEntry("\n\n");
    // add to new Entry for individual chordplayer shortcut unit
    newEntry.append(getDateAndTimeString(), 100); newEntry.append(" | ", 3);
    
    //NOTES FREQUENCIES
    double baseAFrequency = projectManager->frequencyManager->getBaseAFrequency();
    String baseFreqAString(baseAFrequency, 3, false);
    String stringBaseA("A4 = ", 6);
    stringBaseA.append(baseFreqAString, 12);
    stringBaseA.append("hz", 3);
    
    newEntry.append(stringBaseA, 16); newEntry.append(" | ", 3);
    
    //SCALE
    int scaleRef = (int)projectManager->getProjectSettingsParameter(TSS_SETTINGS::DEFAULT_SCALE);
    String stringScale(getScaleString(scaleRef));
    
    newEntry.append(stringScale, 100); newEntry.append(" | ", 3);
    
    //AMPLITUDE MIN
    double amplitudeMin     = projectManager->getProjectSettingsParameter(AMPLITUDE_MIN);
    String stringAmplitudeMin(amplitudeMin, 3, false);
    stringAmplitudeMin.append("%", 1);
    
    newEntry.append(stringAmplitudeMin, 100); newEntry.append(" | ", 3);
    
    //AMPLITUDE MAX
    double amplitudeMax     = projectManager->getProjectSettingsParameter(AMPLITUDE_MAX);
    String stringAmplitudeMax(amplitudeMax, 3, false);
    stringAmplitudeMax.append("%", 1);
    
    newEntry.append(stringAmplitudeMax, 100); newEntry.append(" | ", 3);
    
    //ATTACK MIN
    double attackMin     = projectManager->getProjectSettingsParameter(ATTACK_MIN);
    String stringAttackMin(attackMin, 3, false);
    stringAttackMin.append("ms", 2);
    
    newEntry.append(stringAttackMin, 100); newEntry.append(" | ", 3);
    
    //ATTACK MAX
    double attackMax     = projectManager->getProjectSettingsParameter(ATTACK_MAX);
    String stringAttackMax(attackMax, 3, false);
    stringAttackMax.append("ms", 2);
    
    newEntry.append(stringAttackMax, 100); newEntry.append(" | ", 3);
    
    //DECAY MIN
    double decayMin     = projectManager->getProjectSettingsParameter(DECAY_MIN);
    String stringDecayMin(decayMin, 3, false);
    stringDecayMin.append("ms", 2);
    
    newEntry.append(stringDecayMin, 100); newEntry.append(" | ", 3);
    
    //DECAY MAX
    double decayMax     = projectManager->getProjectSettingsParameter(DECAY_MAX);
    String stringDecayMax(decayMax, 3, false);
    stringDecayMax.append("ms", 2);
    
    newEntry.append(stringDecayMax, 100); newEntry.append(" | ", 3);
    
    //SUSTAIN MIN
    double sustainMin     = projectManager->getProjectSettingsParameter(SUSTAIN_MIN);
    String stringSustainMin(sustainMin, 3, false);
    stringSustainMin.append("%", 2);
    
    newEntry.append(stringSustainMin, 100); newEntry.append(" | ", 3);
    
    //SUSTAIN MAX
    double sustainMax     = projectManager->getProjectSettingsParameter(SUSTAIN_MAX);
    String stringSustainMax(sustainMax, 3, false);
    stringSustainMax.append("%", 2);
    
    newEntry.append(stringSustainMax, 100); newEntry.append(" | ", 3);
    
    // RELEASE MIN
    double releaseMin     = projectManager->getProjectSettingsParameter(RELEASE_MIN);
    String stringReleaseMin(releaseMin, 3, false);
    stringReleaseMin.append("ms", 2);
    
    newEntry.append(stringReleaseMin, 100); newEntry.append(" | ", 3);
    
    // RELEASE MAX
    double releaseMax     = projectManager->getProjectSettingsParameter(RELEASE_MAX);
    String stringReleaseMax(releaseMax, 3, false);
    stringReleaseMax.append("ms", 2);
    
    newEntry.append(stringReleaseMax, 100); newEntry.append(" | ", 3);
    
    // FFT SIZE
    int fftSize             = projectManager->getProjectSettingsParameter(FFT_SIZE);
    String stringFFTSize;
    
    if (fftSize == 1)       stringFFTSize = "1024 Samples";
    else if (fftSize == 2)  stringFFTSize = "2048 Samples";
    else if (fftSize == 3)  stringFFTSize = "4096 Samples";
    else if (fftSize == 4)  stringFFTSize = "8192 Samples";
    else if (fftSize == 5)  stringFFTSize = "16384 Samples";
    else if (fftSize == 6)  stringFFTSize = "32768 Samples";
    
    newEntry.append(stringFFTSize, 100); newEntry.append(" | ", 3);
    
    // FFT WINDOW TYPES
    int fftWindow             = projectManager->getProjectSettingsParameter(FFT_WINDOW);
    String stringFFTWindow;
    
    if (fftWindow == 1)       stringFFTWindow = "Rectangular";
    else if (fftWindow == 2)  stringFFTWindow = "Triangular";
    else if (fftWindow == 3)  stringFFTWindow = "Hann";
    else if (fftWindow == 4)  stringFFTWindow = "Hamming";
    else if (fftWindow == 5)  stringFFTWindow = "Blackman";
    else if (fftWindow == 6)  stringFFTWindow = "BlackmanHarris";
    else if (fftWindow == 7)  stringFFTWindow = "Flat Top";
    else if (fftWindow == 8)  stringFFTWindow = "Kaiser";
    
    newEntry.append(stringFFTWindow, 100); newEntry.append(" | ", 3);
    
    // GUI SCALE
    int guiScale             = projectManager->getProjectSettingsParameter(GUI_SCALE);
    String stringGUIScale;
    
    if (guiScale == 0)       stringGUIScale = "25%" ;
    else if (guiScale == 1)  stringGUIScale = "50%";
    else if (guiScale == 2)  stringGUIScale = "75%";
    else if (guiScale == 3)  stringGUIScale = "100%";
    
    newEntry.append(stringGUIScale, 100); newEntry.append(" | ", 3);

    // Fundamental Frequency Algorithm
    {
        const auto index = (int)projectManager->getProjectSettingsParameter(FUNDAMENTAL_FREQUENCY_ALGORITHM);
        const auto algorithmString = ProjectStrings::getFundamentalFrequencyAlgorithms().getReference(index);
        newEntry.append(algorithmString, 100); newEntry.append(" | ", 3);
    }

    // SHOW HIGHEST PEAK FREQUENCY
    bool showHighestPeakFreq    = projectManager->getProjectSettingsParameter(SHOW_HIGHEST_PEAK_FREQUENCY);
    
    String stringShowHighestPeakFreq;
    if (showHighestPeakFreq) stringShowHighestPeakFreq = "TRUE"; else stringShowHighestPeakFreq = "FALSE";
    newEntry.append(stringShowHighestPeakFreq, 10); newEntry.append(" | ", 3);
    
    // NUMBER OF HIGHEST PEAK FREQUENCIES
    int numHighestPeakFreq    = projectManager->getProjectSettingsParameter(NUMBER_HIGHEST_PEAK_FREQUENCIES);
    String stringNumHighestPeakFreq(numHighestPeakFreq);
    newEntry.append(stringNumHighestPeakFreq, 2); newEntry.append(" | ", 3);
    
    // SHOW HIGHEST PEAK OCTAVES
    bool showHighestPeakOctave    = projectManager->getProjectSettingsParameter(SHOW_HIGHEST_PEAK_OCTAVES);
    String stringShowHighestPeakOct;
    if (showHighestPeakOctave) stringShowHighestPeakOct = "TRUE"; else stringShowHighestPeakOct = "FALSE";
    newEntry.append(stringShowHighestPeakOct, 10); newEntry.append(" | ", 3);
    
    // NUMBER OF HIGHEST PEAK OCTAVES
    int numHighestPeakOctaves    = projectManager->getProjectSettingsParameter(NUMBER_HIGHEST_PEAK_OCTAVES);
    String stringNumHighestPeakOct(numHighestPeakOctaves);
    newEntry.append(stringNumHighestPeakOct, 2); newEntry.append(" | ", 3);
    
    // SELECTED PLUGIN
    
    for (int i = 0; i < NUM_PLUGIN_SLOTS;i++)
    {
        int pluginRef = projectManager->getProjectSettingsParameter(PLUGIN_SELECTED_1 + i);
        
        if (pluginRef != -1)
        {
            PluginDescription* plugin = projectManager->pluginAssignProcessor[0]->pluginList->getType(pluginRef);
            
            if (plugin) newEntry.append(plugin->name, 100); newEntry.append(" | ", 3);
        }
    }

    
    // NOISE TYPE
    String stringNoise;
    int noise = projectManager->getProjectSettingsParameter(PANIC_NOISE_TYPE);
    if (noise == 0) stringNoise = "White Noise"; else stringNoise = "Pink Noise";
    
    newEntry.append(" | ", 3);
    
    // Frequency to Chord
    String stringFreqToChord;
    int freqToChord = projectManager->getProjectSettingsParameter(FREQUENCY_TO_CHORD);
    if (freqToChord == 0) stringFreqToChord = "Main Harmonics";
    else if (freqToChord == 1) stringFreqToChord = "Average of main harmonics and Intervals";
    else if (freqToChord == 2) stringFreqToChord = "Exponential moving average";
    
    newEntry.append(stringFreqToChord, 30);
    
    
    logFileSettings.appendText(newEntry);
    
}

// 1. Check age of logging files at start up and delete any older than a week
void ProjectManager::LogFileWriter::checkForOldLoggingFile()
{
    // scan directory
    
    // get current time
    
    // calculate dif between now and file time
    
    // if dif > 7 days, delete file
}

String ProjectManager::LogFileWriter::getDateAndTimeString()
{
    String newEntry;

    Time time = Time::getCurrentTime();
    String DD(time.getDayOfMonth());
    
    int months = time.getMonth() + 1; String mon(months); String MM;
    if (months < 10) { MM.append("0", 1);  MM.append(mon, 1); }
    else { MM.append(mon, 2); }

    String YY(time.getYear());
    String dateString("");
    
    int hours = time.getHours(); String h(hours); String HH;
    if (hours < 10) { HH.append("0", 1);  HH.append(h, 1); }
    else { HH.append(h, 2); }
    
    int mins = time.getMinutes(); String m(mins); String M;
    if (mins < 10) { M.append("0", 1);  M.append(m, 1); }
    else { M.append(m, 2); }
    
    int secs = time.getSeconds(); String s(secs); String SS;
    if (secs < 10) { SS.append("0", 1);  SS.append(s, 1); }
    else { SS.append(s, 2); }
    
    int millis = time.getMilliseconds(); String mils(millis); String MS;
    if (millis < 10) { MS.append("00", 3);  MS.append(mils, 1); }
    else if (millis >= 10 && millis < 100) { MS.append("0", 2);  MS.append(mils, 2); }
    else { MS.append(mils, 4); }
    
    
    dateString.append(DD, 4); dateString.append(".", 3);
    dateString.append(MM, 4); dateString.append(".", 3);
    dateString.append(YY, 4); dateString.append(" ", 3);
    dateString.append(HH, 4); dateString.append(":", 3);
    dateString.append(M, 4);  dateString.append(":", 3);
    dateString.append(SS, 4); dateString.append(":", 3);
    dateString.append(MS, 4);
    
    newEntry.append(dateString, 100);
    
    return newEntry;
}

String ProjectManager::LogFileWriter::getScaleString(int scaleRef)
{
    switch (scaleRef)
    {
        case DIATONIC_PYTHAGOREAN: return "Diatonic Pythgorean"; break;
        case DIATONIC_JUSTLY_TUNED: return "Diatonic Justly Tuned"; break;
        case DIATONIC_ITERATION_FIFTH: return "Diatonic Iteration Fifth"; break;
            
        case CHROMATIC_PYTHAGOREAN: return "Chromatic Pythagorean"; break;
        case CHROMATIC_JUST_INTONATION: return "Chromatic Just Intonation"; break;
        case CHROMATIC_ET: return "Chromatic Equal Temperament"; break;
        
        case HARMONIC_SIMPLE: return "Harmonic Simple"; break;
        case ENHARMONIC: return "Enharmonic"; break;
        case SOLFEGGIO: return "Solfeggio"; break;

        default:
            return "NO Scale"; break;
    }
}





// Profiles

void ProjectManager::saveProfileForMode(AUDIO_MODE mode)
{
   String fileNameString;
   
   switch (mode) {
       case AUDIO_MODE::MODE_CHORD_PLAYER:         { fileNameString = "Chord Player"; } break;
       case AUDIO_MODE::MODE_CHORD_SCANNER:        { fileNameString = "Chord Scanner"; } break;
       case AUDIO_MODE::MODE_FREQUENCY_PLAYER:     { fileNameString = "Frequency Player"; } break;
       case AUDIO_MODE::MODE_FREQUENCY_SCANNER:    { fileNameString = "Frequency Scanner"; } break;
       case AUDIO_MODE::MODE_FREQUENCY_TO_LIGHT:   { fileNameString = "Frequency To Light"; } break;
           
       default: break;
   }
   
   // get date
   Time time = Time::getCurrentTime();
   String DD(time.getDayOfMonth());
   
   int months = time.getMonth() + 1; String mon(months); String MM;
   if (months < 10) { MM.append("0", 1);  MM.append(mon, 1); }
   else { MM.append(mon, 2); }

   String YY(time.getYear());
   String dateString("");
   
   fileNameString.append(" ", 1);
   fileNameString.append(DD, 2); fileNameString.append("-", 2);
   fileNameString.append(MM, 2); fileNameString.append("-", 2);
   fileNameString.append(YY, 2);
   
   // create file
   
   String url(profileDirectory.getFullPathName());
   
   url.append("/", 2);
   url.append(fileNameString, 100);
   url.append(".profile", 10);
   
   File newFile(url); // or chosen url from the filebrowser

   // open file browser at location, put default filename into browser
   FileChooser fileChooser ("Please choose location and name to save",
                      newFile,
                      "*.profile");

   if (fileChooser.browseForFileToSave(false))
   {
       File res = fileChooser.getResult();
       
       if (res.existsAsFile())
       {
           res.deleteFile();
       }
       
       res.create();
       
       // upon Save button,
       FileOutputStream fileOutputStream(res);
       
       
       switch (mode)
       {
           case AUDIO_MODE::MODE_CHORD_PLAYER:
           {
               // wrap all shortcuts paramaters into new Valuetree..
               ValueTree maintree("maintree");
               
               for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
               {
                   ValueTree chordPlayerShortcutParamTree(String::formatted("shortcut%i", i));
                   chordPlayerShortcutParamTree.copyPropertiesFrom(*chordPlayerParameters[i], nullptr);

                   maintree.addChild(chordPlayerShortcutParamTree, i, nullptr);
                   
               }
               
               // DBG(maintree.toXmlString()); // Debug check
               
               maintree.writeToStream(fileOutputStream);
               
           }
               break;
               
           case AUDIO_MODE::MODE_CHORD_SCANNER:
           {
               chordScannerParameters->writeToStream(fileOutputStream);
           }
               break;
               
           case AUDIO_MODE::MODE_FREQUENCY_PLAYER:
           {
               // wrap all shortcuts paramaters into new Valuetree..
               
               ValueTree maintree("maintree");
               
               for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
               {
                   ValueTree frequencyPlayerShortcutParamTree(String::formatted("shortcut%i", i));
                   frequencyPlayerShortcutParamTree.copyPropertiesFrom(*frequencyPlayerParameters[i], nullptr);
                   
                   maintree.addChild(frequencyPlayerShortcutParamTree, i, nullptr);
               }
               
               maintree.writeToStream(fileOutputStream);
           }
               break;
               
           case AUDIO_MODE::MODE_FREQUENCY_SCANNER:
           {
               frequencyScannerParameters->writeToStream(fileOutputStream);
           }
               break;
               
           case AUDIO_MODE::MODE_FREQUENCY_TO_LIGHT:
           {
               frequencyToLightParameters->writeToStream(fileOutputStream);
           }
               break;
               
           case AUDIO_MODE::MODE_LISSAJOUS_CURVES:
           {
               frequencyToLightParameters->writeToStream(fileOutputStream);
           }
               break;
               
           default:
               break;
       }
   }
   else
   {
       // delete temp file
       newFile.deleteFile();
       
   }
}
   
void ProjectManager::loadProfileForMode(AUDIO_MODE mode)
{
   // check nm parameters is equal to selected mode, otherwise throw and exception.
   
   // open file browser at location, put default filename into browser
   FileChooser fileChooser ("Please select the file you want to load",
                     profileDirectory,
                     "*.profile");

   if (fileChooser.browseForFileToOpen())
   {
      File res = fileChooser.getResult();
      
      // check file is .profile
      
      FileInputStream fileInputStream(res);
      ValueTree maintree = ValueTree::readFromStream(fileInputStream);
      
      
      switch (mode)
      {
          case AUDIO_MODE::MODE_CHORD_PLAYER:
          {
              if (maintree.getNumChildren() == NUM_SHORTCUT_SYNTHS)
              {
                  for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
                  {
                      ValueTree chordPlayerShortcutParamTree = maintree.getChildWithName(String::formatted("shortcut%i", i));
                      
                      if (chordPlayerShortcutParamTree.getNumProperties() == TOTAL_NUM_CHORD_PLAYER_SHORTCUT_PARAMS)
                      {
                          // copy valuetree to local params
                          chordPlayerParameters[i]->copyPropertiesFrom(chordPlayerShortcutParamTree, nullptr);

                          // load new value tree into shortcut synth
                          // sync to chord player processor / synths
                          for (int index = 0; index < TOTAL_NUM_CHORD_PLAYER_SHORTCUT_PARAMS; index++)
                          {
                              String identifier = getIdentifierForChordPlayerParameterIndex(index);
                              if (identifier.isEmpty())
                                  continue;  // Skip unhandled indices
                              setChordPlayerParameter(i, index, chordPlayerParameters[i]->getProperty(identifier));
                          }
                      }
                      else
                      {
                          AlertWindow::showNativeDialogBox("Invalid File Type", "Please load a valid Chord Player .profile file", false);
                          
                          break;
                      }

                  }
              }
              else
              {
                  AlertWindow::showNativeDialogBox("Invalid File Type", "Please load a valid Chord Player .profile file", false);
                  
                  break;
              }
              
          }
              break;
              
          case AUDIO_MODE::MODE_CHORD_SCANNER:
          {
              if (maintree.getNumProperties() == TOTAL_NUM_CHORD_SCANNER_PARAMS)
              {
                  // copy valuetree to local params
                  chordScannerParameters->copyPropertiesFrom(maintree, nullptr);

                  for (int index = 0; index < TOTAL_NUM_CHORD_SCANNER_PARAMS; index++)
                  {
                      setChordScannerParameter(index, chordScannerParameters->getProperty(getIdentifierForChordScannerParameterIndex(index)));
                  }
              }
              else
              {
                  AlertWindow::showNativeDialogBox("Invalid File Type", "Please load a valid Chord Scanner .profile file", false);
                  
                  break;
              }

          }
              break;
              
          case AUDIO_MODE::MODE_FREQUENCY_PLAYER:
          {
              if (maintree.getNumChildren() == NUM_SHORTCUT_SYNTHS)
              {
                  for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
                  {
                      ValueTree frequencyPlayerShortcutParamTree = maintree.getChildWithName(String::formatted("shortcut%i", i));
                       
                      if (frequencyPlayerShortcutParamTree.getNumProperties() == TOTAL_NUM_FREQUENCY_PLAYER_SHORTCUT_PARAMS)
                      {
                          if (frequencyPlayerShortcutParamTree.getNumProperties() == TOTAL_NUM_FREQUENCY_PLAYER_SHORTCUT_PARAMS)
                          {
                              // copy valuetree to local params
                              frequencyPlayerParameters[i]->copyPropertiesFrom(frequencyPlayerShortcutParamTree, nullptr);

                              // load new value tree into shortcut synth
                              // sync to frequency player processor / synths
                              for (int index = 0; index < TOTAL_NUM_FREQUENCY_PLAYER_SHORTCUT_PARAMS; index++)
                              {
                                  setFrequencyPlayerParameter(i, index, frequencyPlayerParameters[i]->getProperty(getIdentifierForFrequencyPlayerParameterIndex(index)));
                              }
                          }
                      }
                      else
                      {
                          AlertWindow::showNativeDialogBox("Invalid File Type", "Please load a valid Frequency Player .profile file", false);
                          
                          break;
                      }
                  }
              }
              else
              {
                  AlertWindow::showNativeDialogBox("Invalid File Type", "Please load a valid Frequency Player .profile file", false);
                  
                  break;
              }
              
              
          }
              break;
              
          case AUDIO_MODE::MODE_FREQUENCY_SCANNER:
          {
              if (maintree.getNumProperties() == TOTAL_NUM_FREQUENCY_SCANNER_PARAMS)
              {
                  frequencyScannerParameters->copyPropertiesFrom(maintree, nullptr);

                  for (int index = 0; index < TOTAL_NUM_FREQUENCY_SCANNER_PARAMS; index++)
                  {
                      setFrequencyScannerParameter(index, frequencyScannerParameters->getProperty(getIdentifierForFrequencyScannerParameterIndex(index)));
                  }
              }
              else
              {
                  AlertWindow::showNativeDialogBox("Invalid File Type", "Please load a valid Frequency Scanner .profile file", false);
                  
                  break;
              }
          }
              break;
              
          case AUDIO_MODE::MODE_FREQUENCY_TO_LIGHT:
          {

          }
              break;
              
          case AUDIO_MODE::MODE_LISSAJOUS_CURVES:
          {

          }
              break;
              
          default:
              break;
      }
      
  }
   
}

void ProjectManager::saveSettingsFile()
{
    String fileNameString("Settings");

    
    // get date
    Time time = Time::getCurrentTime();
    String DD(time.getDayOfMonth());
    
    int months = time.getMonth() + 1; String mon(months); String MM;
    if (months < 10) { MM.append("0", 1);  MM.append(mon, 1); }
    else { MM.append(mon, 2); }

    String YY(time.getYear());
    String dateString("");
    
    fileNameString.append(" ", 1);
    fileNameString.append(DD, 2); fileNameString.append("-", 2);
    fileNameString.append(MM, 2); fileNameString.append("-", 2);
    fileNameString.append(YY, 2);
    
    // create file
    
    String url(profileDirectory.getFullPathName());
    
    url.append("/", 2);
    url.append(fileNameString, 100);
    url.append(".profile", 10);
    
    File newFile(url); // or chosen url from the filebrowser

    // open file browser at location, put default filename into browser
    FileChooser fileChooser ("Please choose location and name to save",
                       newFile,
                       "*.profile");

    if (fileChooser.browseForFileToSave(false))
    {
        File res = fileChooser.getResult();
        
        if (res.existsAsFile())
            res.deleteFile();
        
        res.create();
        
        FileOutputStream fileOutputStream(res);
        
        projectSettings->writeToStream(fileOutputStream);
    }
    else
    {
        // delete temp file
        newFile.deleteFile();
    }
    
    logFileWriter->processLog_Settings_Parameters();
    
}

void ProjectManager::loadSettingsFile()
{
    // open file browser at location, put default filename into browser
    FileChooser fileChooser ("Please select the Settings file you want to load",
                      profileDirectory,
                      "*.profile");

    if (fileChooser.browseForFileToOpen())
    {
       File res = fileChooser.getResult();
       
       FileInputStream fileInputStream(res);
       ValueTree maintree = ValueTree::readFromStream(fileInputStream);
    
       if (maintree.getNumProperties() == TOTAL_NUM_SETTINGS)
       {
           projectSettings->copyPropertiesFrom(maintree, nullptr);

           for (int index = 0; index < TOTAL_NUM_SETTINGS; index++)
           {
               setProjectSettingsParameter(index, projectSettings->getProperty(getIdentifierForSettingIndex(index)));
           }
       }
       else
       {
           AlertWindow::showNativeDialogBox("Invalid File Type", "Please load a valid Settings .profile file", false);
       }
    }
    
    logFileWriter->processLog_Settings_Parameters();

}

//===============================================================================
#pragma mark Panic Button / White Noise
//===============================================================================
    
void ProjectManager::setPanicButton()
{
    if (panicButtonIsDown)
    {
        panicButtonIsDown = false; noiseWhite.clear(); noisePink.clear();
    }
    else
    {
        logFileWriter->processLog_PanicButtonPressed(noiseType);
        
        panicButtonIsDown = true; noiseWhite.clear(); noisePink.clear();
    }
}

bool ProjectManager::getIsPanicButtonDown() { return panicButtonIsDown; }


void ProjectManager::processPanicButtonNoise(AudioBuffer<float>& buffer)
{
    if (noiseType == WHITE_NOISE) { processWhiteNoise(buffer); }
    else { processPinkNoise(buffer); }
}

void ProjectManager::processWhiteNoise(AudioBuffer<float>& buffer)
{
    for (int sample = 0; sample < buffer.getNumSamples(); sample++)
    {
        buffer.getWritePointer(0)[sample] = noiseWhite.tick();
        
        if (buffer.getNumChannels() == 2) buffer.getWritePointer(1)[sample] = buffer.getReadPointer(0)[sample];
    }
}

void ProjectManager::processPinkNoise(AudioBuffer<float>& buffer)
{
    for (int sample = 0; sample < buffer.getNumSamples(); sample++)
    {
        buffer.getWritePointer(0)[sample] = noisePink.tick();
        
        if (buffer.getNumChannels() == 2) buffer.getWritePointer(1)[sample] = buffer.getReadPointer(0)[sample];
    }
}

//===============================================================================
#pragma mark CRITICAL BUG FIXES - Thread Safety & Memory Management
//===============================================================================

void ProjectManager::cleanup() noexcept
{
    try 
    {
        // Stop background processing first
        if (backgroundThread.isThreadRunning()) {
            backgroundThread.stopThread(1000);
        }
        
        // Clean up processors in reverse order
        lissajousProcessor.reset();
        frequencyToLightProcessor.reset();
        frequencyScannerProcessor.reset();
        frequencyPlayerProcessor.reset();
        chordScannerProcessor.reset();
        chordPlayerProcessor.reset();
        
        // Clean up managers
        frequencyManager.reset();
        sampleLibraryManager.reset();
        logFileWriter.reset();
        
        // Clean up plugin system
        if (knownPluginList) {
            knownPluginList->removeChangeListener(this);
            knownPluginList.reset();
        }
        
        // Clean up parameter trees
        for (auto& param : chordPlayerParameters) {
            param.reset();
        }
        for (auto& param : frequencyPlayerParameters) {
            param.reset();
        }
        chordScannerParameters.reset();
        frequencyScannerParameters.reset();
        frequencyToLightParameters.reset();
        lissajousParameters.reset();
        projectSettings.reset();
        
        // Clean up plugin processors
        for (auto& processor : pluginAssignProcessor) {
            processor.reset();
        }
        
        appProperties.reset();
    }
    catch (...) 
    {
        // FIXED: Never throw from destructor/cleanup
        DBG("Exception during ProjectManager cleanup - continuing");
    }
}

void ProjectManager::initializeApplicationProperties()
{
    if (!appProperties) {
        PropertiesFile::Options options;
        options.applicationName = "TSS";
        options.filenameSuffix = "settings";
        options.osxLibrarySubFolder = "Preferences";
        
        appProperties = std::make_unique<ApplicationProperties>();
        appProperties->setStorageParameters(options);
    }
}

void ProjectManager::initializePluginSystem()
{
    if (!knownPluginList) {
        knownPluginList = std::make_unique<KnownPluginList>();
    }
    
    deadMansPedalFile = appProperties->getUserSettings()->getFile().getSiblingFile("RecentlyCrashedPluginsList");
    
    pluginFormatManager.addDefaultFormats();
    
    // FIXED: Safe XML loading with error handling
    if (auto pluginListXml = appProperties->getUserSettings()->getXmlValue("pluginList")) {
        try {
            knownPluginList->recreateFromXml(*pluginListXml);
        }
        catch (const std::exception& e) {
            DBG("Failed to load plugin list: " << e.what());
            knownPluginList->createXml(); // Create fresh list
        }
    }
    else {
        knownPluginList->createXml();
    }
    
    knownPluginList->addChangeListener(this);
    
    pluginSortMethod = static_cast<KnownPluginList::SortMethod>(
        appProperties->getUserSettings()->getIntValue("pluginSortMethod", KnownPluginList::sortByManufacturer));
}

void ProjectManager::initializeProcessors()
{
    // FIXED: Exception-safe processor creation
    chordPlayerProcessor = std::make_unique<ChordPlayerProcessor>(
        frequencyManager.get(), sampleLibraryManager.get(), synthesisEngine.get(), this);
    
    chordScannerProcessor = std::make_unique<ChordScannerProcessor>(
        frequencyManager.get(), sampleLibraryManager.get(), this);
    
    frequencyPlayerProcessor = std::make_unique<FrequencyPlayerProcessor>(
        frequencyManager.get(), this);
    
    frequencyScannerProcessor = std::make_unique<FrequencyScannerProcessor>(
        frequencyManager.get(), this);
    
    frequencyToLightProcessor = std::make_unique<FrequencyToLightProcessor>(
        frequencyManager.get());
    
    realtimeAnalysisProcessor = std::make_unique<RealtimeAnalysisProcessor>(*this);
    
    lissajousProcessor = std::make_unique<LissajousProcessor>(
        frequencyManager.get(), sampleLibraryManager.get());
    
    // Initialize other processors...
    initializeAnalysisProcessors();
}

void ProjectManager::initializeAnalysisProcessors()
{
    try {
        // These are already using smart pointers in the header
        if (!frequencyManager) {
            throw std::runtime_error("FrequencyManager is null during analysis processor initialization");
        }
        
        fundamentalFrequencyProcessor = std::make_unique<FundamentalFrequencyProcessor>(*this, *frequencyManager);
        if (!fundamentalFrequencyProcessor) {
            throw std::runtime_error("Failed to create FundamentalFrequencyProcessor");
        }
        
        feedbackModuleProcessor = std::make_unique<FeedbackModuleProcessor>(*this, *frequencyManager);
        if (!feedbackModuleProcessor) {
            throw std::runtime_error("Failed to create FeedbackModuleProcessor");
        }
        
        DBG("Analysis processors initialized successfully");
    }
    catch (const std::exception& e) {
        DBG("Failed to initialize analysis processors: " << e.what());
        throw;
    }
}

// FIXED: Thread-safe mode management
void ProjectManager::setAudioMode(AUDIO_MODE newMode)
{
    std::lock_guard<std::mutex> lock(stateMutex);
    currentMode.store(newMode);
    mode = newMode; // Keep for backward compatibility
    
    // Notify listeners of mode change
    if (logFileWriter) {
        logFileWriter->initNewLogFileForAudioMode(newMode);
    }
}

AUDIO_MODE ProjectManager::getAudioMode() const
{
    return currentMode.load();
}
