/*
  ==============================================================================

    ProjectManagerFixed.cpp
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#include "ProjectManager.h"
#include "MainComponent.h"

ProjectManager::ProjectManager() : backgroundThread("Audio Recorder Thread")
{
    try 
    {
        // Initialize directories with proper error handling
        logFileDirectory = File::getSpecialLocation(File::userDocumentsDirectory).getChildFile("ASVPR/Logs");
        recordFileDirectory = File::getSpecialLocation(File::userDocumentsDirectory).getChildFile("ASVPR/Recordings");
        profileDirectory = File::getSpecialLocation(File::userDocumentsDirectory).getChildFile("ASVPR/Profiles");
        
        // Create directories if they don't exist
        logFileDirectory.createDirectory();
        recordFileDirectory.createDirectory();
        profileDirectory.createDirectory();
        
        // FIXED: Exception-safe property initialization
        initializeApplicationProperties();
        
        // FIXED: Exception-safe plugin system initialization
        initializePluginSystem();
        
        // FIXED: Exception-safe FFT initialization
        if (!initFFT()) {
            throw std::runtime_error("Failed to initialize FFT system");
        }
        
        // FIXED: Thread-safe mode initialization
        currentMode.store(AudioMode::ChordPlayer);
        
        // FIXED: Smart pointer initialization for better memory safety
        logFileWriter = std::make_unique<LogFileWriter>(this);
        if (logFileWriter) {
            logFileWriter->initNewLogFileForAudioMode(currentMode.load());
            logFileWriter->initNewSettingsLogFile();
        }
        
        sampleLibraryManager = std::make_unique<SampleLibraryManager>();
        
        frequencyManager = std::make_unique<FrequencyManager>();
        frequencyManager->setBaseAFrequency(432);
        
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
}

ProjectManager::~ProjectManager()
{
    // FIXED: Proper cleanup order and thread safety
    cleanup();
}

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
        options.applicationName = "ASVPR";
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
        frequencyManager.get(), sampleLibraryManager.get(), this);
    
    chordScannerProcessor = std::make_unique<ChordScannerProcessor>(
        frequencyManager.get(), sampleLibraryManager.get(), this);
    
    frequencyPlayerProcessor = std::make_unique<FrequencyPlayerProcessor>(
        frequencyManager.get(), this);
    
    frequencyScannerProcessor = std::make_unique<FrequencyScannerProcessor>(
        frequencyManager.get(), this);
    
    frequencyToLightProcessor = std::make_unique<FrequencyToLightProcessor>(
        frequencyManager.get());
    
    lissajousProcessor = std::make_unique<LissajousProcessor>(
        frequencyManager.get(), sampleLibraryManager.get());
    
    // Initialize other processors...
    initializeAnalysisProcessors();
}

void ProjectManager::initializeAnalysisProcessors()
{
    realtimeAnalysisProcessor = std::make_unique<RealtimeAnalysisProcessor>(frequencyManager.get());
    fundamentalFrequencyProcessor = std::make_unique<FundamentalFrequencyProcessor>(frequencyManager.get());
    feedbackModuleProcessor = std::make_unique<FeedbackModuleProcessor>();
}

// FIXED: Thread-safe mode management
void ProjectManager::setAudioMode(AudioMode newMode)
{
    std::lock_guard<std::mutex> lock(stateMutex);
    currentMode.store(newMode);
    
    // Notify listeners of mode change
    if (logFileWriter) {
        logFileWriter->initNewLogFileForAudioMode(newMode);
    }
}

AudioMode ProjectManager::getAudioMode() const
{
    return currentMode.load();
}

// FIXED: Thread-safe audio processing
void ProjectManager::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
    // FIXED: Atomic mode reading for thread safety
    const auto currentAudioMode = currentMode.load();
    
    // Clear buffer first
    bufferToFill.clearActiveBufferRegion();
    
    // Process based on current mode
    switch (currentAudioMode) {
        case AudioMode::ChordPlayer:
            if (chordPlayerProcessor) {
                processChordPlayer(*bufferToFill.buffer);
            }
            break;
            
        case AudioMode::ChordScanner:
            if (chordScannerProcessor) {
                processChordScanner(*bufferToFill.buffer);
            }
            break;
            
        case AudioMode::FrequencyPlayer:
            if (frequencyPlayerProcessor) {
                processFrequencyPlayer(*bufferToFill.buffer);
            }
            break;
            
        case AudioMode::FrequencyScanner:
            if (frequencyScannerProcessor) {
                processFrequencyScanner(*bufferToFill.buffer);
            }
            break;
            
        case AudioMode::LissajousCurve:
            if (lissajousProcessor) {
                processLissajousCurve(*bufferToFill.buffer);
            }
            break;
            
        case AudioMode::RealtimeAnalysis:
            if (realtimeAnalysisProcessor) {
                processRealtimeAnalysis(*bufferToFill.buffer);
            }
            break;
            
        default:
            // Unknown mode - leave buffer cleared
            break;
    }
    
    // Always process fundamental frequency and feedback if enabled
    if (fundamentalFrequencyProcessor) {
        processFundamentalFrequency(*bufferToFill.buffer);
    }
    
    if (feedbackModuleProcessor) {
        processFeedbackModule(*bufferToFill.buffer);
    }
}