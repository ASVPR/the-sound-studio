/*
  ==============================================================================

    ProjectManager.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Parameters.h"
#include "ChordPlayerProcessor.h"
#include "ChordScannerProcessor.h"
#include "FrequencyPlayerProcessor.h"
#include "FrequencyScannerProcessor.h"
#include "FrequencyToLightProcessor.h"
#include "LissajousCurveProcessor.h"
#include "ChordManager.h"
#include "SynthesisLibraryManager.h"
#include "SynthesisEngine.h"
#include "FrequencyManager.h"
#include "PluginAssignProcessor.h"
#include "RingBuffer.h"
#include "NoiseOscillator.h"
#include "FundamentalFrequencyProcessor.h"
#include "AnalyzerPool.h"
#include "RecordingManager.h"
#include "LogFileWriter.h"
#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <array>

class MainComponent;

class ProjectManager : public ChangeListener
{
public:
    // Thread-safe atomic mode management (deprecated non-atomic `mode` removed)
    std::atomic<AUDIO_MODE> currentMode { AUDIO_MODE::MODE_CHORD_PLAYER };
    mutable std::mutex stateMutex;  // For thread safety
    
    ProjectManager();
    
    ~ProjectManager();
    
    MainComponent * mainComponent;
    
    void setMainComponent(MainComponent * mc) { mainComponent = mc; }
    
    void changeListenerCallback (ChangeBroadcaster*);
    
    AudioDeviceManager * deviceManager;
    
    AudioDeviceManager * getDeviceManager() { return deviceManager; }
    
    void setDeviceManager(AudioDeviceManager * dm) { deviceManager = dm; }
    
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate);
    
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill);
    
    void processChordPlayer(AudioBuffer<float>& buffer);
    
    void processChordScanner(AudioBuffer<float>& buffer);
    
    void processFrequencyPlayer(AudioBuffer<float>& buffer);
    
    void processFrequencyScanner(AudioBuffer<float>& buffer);
    
    void processLissajousCurve(AudioBuffer<float>& buffer);
    
    void processRealtimeAnalysis(AudioBuffer<float>& buffer);
    
    void processFundamentalFrequency(AudioBuffer<float>& buffer);
    
    void releaseResources();
    
    void setOversampingFactor(int newFactor);
    
    double getSampleRate() { return sample_rate; };
    
    void setMode(AUDIO_MODE newMode); // called from menu
    
    // FIXED: Thread-safe mode management
    void setAudioMode(AUDIO_MODE newMode);
    AUDIO_MODE getAudioMode() const;
    
    void initGUISync();
    
    void initGUISync(AUDIO_MODE mode);
    
    void shortcutKeyPressed(CHORD_PLAYER_SHORTCUT_KEY keyRef);

    enum class FontType
    {
        Light,
        Bold,
        SemiBold
    };

    static juce::Font getAssistantFont(FontType fontType)
    {
        if (fontType == FontType::Light)
        {
            return juce::Font(Typeface::createSystemTypefaceFor(BinaryData::AssistantLight_ttf,
                                                                BinaryData::AssistantLight_ttfSize));
        }
        else if (fontType == FontType::Bold)
        {
            return juce::Font(Typeface::createSystemTypefaceFor(BinaryData::AssistantBold_ttf,
                                                                BinaryData::AssistantBold_ttfSize));
        }
        else if (fontType == FontType::SemiBold)
        {
            return juce::Font(Typeface::createSystemTypefaceFor(BinaryData::AssistantSemiBold_ttf,
                                                                BinaryData::AssistantSemiBold_ttfSize));
        }
        
        // Default fallback
        return juce::Font(Typeface::createSystemTypefaceFor(BinaryData::AssistantLight_ttf,
                                                            BinaryData::AssistantLight_ttfSize));
    }
    
//===============================================================================
#pragma mark Frequency Manager
//===============================================================================
    
    std::unique_ptr<FrequencyManager> frequencyManager;

//===============================================================================
#pragma mark Project Settings
//===============================================================================
    
    
    // FIXED: Smart pointer for memory safety
    std::unique_ptr<ValueTree> projectSettings;
    
    AudioFormatManager formatManager;
    
    // local varibales
    double sample_rate;
    int oversample_factor;
    File logFileDirectory;
    File recordFileDirectory;
    File profileDirectory;
    double baseAFrequency; // 432
    SCALES  defaultScale;
    int recordFileFormatIndex;
    
    double amplitude_min, amplitude_max, attack_min, attack_max, decay_min, decay_max,
    sustain_min, sustain_max, release_min, release_max;
    
    double inputGain[4];
    double outputGain[4];
    bool inputFFT[4];
    bool outputFFT[4];
    
    String getIdentifierForSettingIndex(int index);

    void initDefaultProjectSettings();
    
    // called from gui controls
    void setProjectSettingsParameter(int index, double newVal);
    
    double getProjectSettingsParameter(int index);
    
    void setDirectoryFileForSettingParameter(int index, File newDirectory);
    
    File getDirectoryFileForSettingParameter(int index);
    
    void syncSettingsGUI();
    
    void setSettingsColorParameter(int index, int32 argb)
    {
        projectSettings->setProperty(getIdentifierForSettingIndex(FFT_COLOR_SPEC_MAIN + index), argb, nullptr);
        
        uiListeners.call(&::ProjectManager::UIListener::updateSettingsUIParameter, FFT_COLOR_SPEC_MAIN + index);
    }
    
    int32 getSettingsColorParameter(int index)
    {
        return projectSettings->getProperty(getIdentifierForSettingIndex(FFT_COLOR_SPEC_MAIN +  index)).operator int32();
    }
    
    //===============================================================================
#pragma mark Commands
    //===============================================================================
    void setPlayerCommand(PLAYER_COMMANDS command);

    void setPlayerPlayMode(PLAY_MODE pmode);
    
    void shortcutKeyDown(int shortcutRef);
    
    void shortcutKeyUp(int shortcutRef);
    
    //===============================================================================
#pragma mark Sampler Instruments Directories
    //===============================================================================
    
    // FIXED: Smart pointer for memory safety
    std::unique_ptr<SynthesisLibraryManager> sampleLibraryManager;
    std::unique_ptr<SynthesisEngine> synthesisEngine;
    
    SynthesisLibraryManager * getSampleLibraryManager() { return sampleLibraryManager.get(); }
    SynthesisEngine * getSynthesisEngine() { return synthesisEngine.get(); }
    
    //===============================================================================
#pragma mark Wavetable interface
    //===============================================================================
    
    double * getWavetableBufferForCurrentEditor(AUDIO_MODE mode, int shortcut);
    
    void * setWavetableBuffer(AUDIO_MODE mode, int shortcut, double * tableArray);
    
    void initBaseWavetable(AUDIO_MODE mode, int shortcutRef, int newType);
    
    //===============================================================================
#pragma mark Chord Player Parameters
    //===============================================================================

    // FIXED: Smart pointer for memory safety
    std::unique_ptr<ChordPlayerProcessor> chordPlayerProcessor;
    // FIXED: Smart pointer array for memory safety
    std::array<std::unique_ptr<ValueTree>, NUM_SHORTCUT_SYNTHS> chordPlayerParameters;
    
    String getIdentifierForChordPlayerParameterIndex(int index);
    
    void initDefaultChordPlayerParameters();
    
    void initDefaultChordPlayerParametersForShortcut(int shortcutRef);
    // called from UI to change individual parameters
    void setChordPlayerParameter(int synthRef, int index, var newValue);
    
    // called from GUI to update controls when neccessary
    var getChordPlayerParameter(int synthRef, int index);
    
    void syncChordPlayerContainerGUI(int synthRef);
    
    void syncChordPlayerSettingsGUI(int synthRef);
    
    void reorderShortcuts();
    
    void copyShortcut(int sourceShortcut, int destShortcut);
    
    //===============================================================================
#pragma mark Chord Scanner Parameters
    //===============================================================================
    
    // FIXED: Smart pointer for memory safety
    std::unique_ptr<ChordScannerProcessor> chordScannerProcessor;
    // FIXED: Smart pointer for memory safety
    std::unique_ptr<ValueTree> chordScannerParameters;
    
    String getIdentifierForChordScannerParameterIndex(int index);
    
    void initDefaultChordScannerParameters();
    // called from UI to change individual parameters
    void setChordScannerParameter(int index, var newValue);
    
    // called from GUI to update controls when neccessary
    var getChordScannerParameter(int index);
    
    //===============================================================================
#pragma mark Fundamental Frequency Parameters
    //===============================================================================
    
    std::unique_ptr<FundamentalFrequencyProcessor> fundamentalFrequencyProcessor;

    //===============================================================================
#pragma mark Frequency Player Parameters
    //===============================================================================
    
    // FIXED: Smart pointer for memory safety
    std::unique_ptr<FrequencyPlayerProcessor> frequencyPlayerProcessor;
    // FIXED: Smart pointer array for memory safety
    std::array<std::unique_ptr<ValueTree>, NUM_SHORTCUT_SYNTHS> frequencyPlayerParameters;
    
    String getIdentifierForFrequencyPlayerParameterIndex(int index);
    
    void initDefaultFrequencyPlayerParameters();
    
    void initDefaultFrequencyPlayerParametersForShortcut(int shortcutRef);
    
    // called from UI to change individual parameters
    void setFrequencyPlayerParameter(int synthRef, int index, var newValue);
    
    // called from GUI to update controls when neccessary
    var getFrequencyPlayerParameter(int synthRef, int index);
    
    //===============================================================================
#pragma mark Frequency Scanner Parameters
    //===============================================================================
    
    // FIXED: Smart pointer for memory safety
    std::unique_ptr<FrequencyScannerProcessor> frequencyScannerProcessor;
    // FIXED: Smart pointer for memory safety
    std::unique_ptr<ValueTree> frequencyScannerParameters;
    
    String getIdentifierForFrequencyScannerParameterIndex(int index);
    
    void initDefaultFrequencyScannerParameters();
    
    // called from UI to change individual parameters
    void setFrequencyScannerParameter(int index, var newValue);
    
    // called from GUI to update controls when neccessary
    var getFrequencyScannerParameter(int index);
    
    
    //===============================================================================
#pragma mark Frequency To Light Parameters
    //===============================================================================
    
    // FIXED: Smart pointer for memory safety
    std::unique_ptr<FrequencyToLightProcessor> frequencyToLightProcessor;
    // FIXED: Smart pointer for memory safety
    std::unique_ptr<ValueTree> frequencyToLightParameters;
    
    String getIdentifierForFrequencyToLightParameterIndex(int index);
    
    void initDefaultFrequencyToLightParameters();
    
    // called from UI to change individual parameters
    void setFrequencyToLightParameter(int index, var newValue);
    
    // called from GUI to update controls when neccessary
    var getFrequencyToLightParameter(int index);
    
    //===============================================================================
#pragma mark Lissajous Curve Parameters
    //===============================================================================
        
    // FIXED: Smart pointer for memory safety
    std::unique_ptr<LissajousProcessor> lissajousProcessor;
    // FIXED: Smart pointer for memory safety
    std::unique_ptr<ValueTree> lissajousParameters;
    
    String getIdentifierForLissajousParameterIndex(int index);
    
    void initDefaultLissajousParameters();
        
    // called from UI to change individual parameters
    void setLissajousParameter(int index, var newValue);
        
    // called from GUI to update controls when neccessary
    var getLissajousParameter(int index);
    
    
    //===============================================================================
#pragma mark To be done !
    //===============================================================================
    ValueTree realtimeAnalysisParameters;
    
    
    // MainProcessor holds all synth objects
    // Manages mixing between synths and audio states
    // returns all data for visualisers (FFT, meters etc)
    
    //===============================================================================
#pragma mark Plugin Assign Processor
    //===============================================================================
    
    ValueTree pluginAssignParameters; // not done yet
    
    // FIXED: Smart pointer array for memory safety
    std::array<std::unique_ptr<PluginAssignProcessor>, NUM_PLUGIN_SLOTS> pluginAssignProcessor;
    
    // FIXED: Smart pointers for memory safety
    std::unique_ptr<ApplicationProperties> appProperties;
    std::unique_ptr<KnownPluginList> knownPluginList;
    AudioPluginFormatManager pluginFormatManager;
    
    OwnedArray <PluginDescription> internalTypes;
    KnownPluginList::SortMethod pluginSortMethod;
    File deadMansPedalFile;
    
    
    //===============================================================================
#pragma mark UI Listener Class
    //===============================================================================
    class UIListener
    {
    public:
        virtual ~UIListener() = default;
        
        virtual void updateSettingsUIParameter(int settingIndex) {}
        
        virtual void updateChordPlayerUIParameter(int shortcutRef, int paramIndex) {}
        
        virtual void updateChordScannerUIParameter(int paramIndex) {}
        
        virtual void updateFundamentalFrequencyUIParameter(int paramIndex) {}
        
        virtual void updateFrequencyPlayerUIParameter(int shortcutRef, int paramIndex) {}
        
        virtual void updateFrequencyScannerUIParameter(int paramIndex) {}
        
        virtual void updateLissajousCurveUIParameter(int paramIndex) {}
        
        virtual void updateFrequencyToLightUIParameter(int paramIndex) {}

        virtual void updateMinMaxSettings(int paramIndex) {}
        
//        virtual void updateFFTSize(int newFFTSize) {}
        
        virtual void freezeFFTProcessing(bool shouldFreeze) {}
        
        virtual void updateDeviceIO() {} // for device inout/output changes to propogate to GUI and sub processors
        
        virtual void updateAudioMode(AUDIO_MODE mode) {}
    };
    
    void addUIListener(ProjectManager::UIListener* toAdd) { uiListeners.add(toAdd); }
    
    void removeUIListener(ProjectManager::UIListener* toRemove) { uiListeners.remove(toRemove); }
    
    ListenerList<UIListener> uiListeners;

    //===============================================================================
#pragma mark FFT  / Visual Stuff
    //===============================================================================
    

    float * fifo;
    float * fftData;
    int fftOrder = 10;
    int fftSize = 1 << fftOrder;
    int fftSizeEnumValue    = 0;
    int fifoIndex           = 0;
    bool shouldProcessFFT   = true;
    Range<float> maxLevel;

    void setShouldProcessFFT(bool should);
    
    int getFFTSize() { return fftSize; }
    void setNewFFTSize(int fftEnum);
    
    void setNewFFTWindowFunction(int windowEnum);
    
    void initFFT();

    
    //=====================================================================================
    // new  FFT threader
    //=====================================================================================
    
    AnalyzerPool analyzerPool;

    
    void createAnalyserPlot (Path& p, const Rectangle<int> bounds, float minFreq, float maxFreq, bool input);
    void createAnalyserPlot (int fftChannel, Path& p, const Rectangle<int> bounds, float minFreq, float maxFreq, bool input);
    
    void createAnalyserPlotOptimisedWithRange (Path& p, const Rectangle<int> bounds, float minFreq, float maxFreq, float minDB, float maxDB, bool input);
    void createAnalyserPlotOptimisedWithRange (int fftChannel, Path& p, const Rectangle<int> bounds, float minFreq, float maxFreq, float minDB, float maxDB, bool input);
    
    void createOctaveMagnitudes(Array<float> & magnitude, int & numBands, float minFreq, float maxFreq, Array<float> & centralFreqs);
    void createOctaveMagnitudes(int fftChannel, Array<float> & magnitude, int & numBands, float minFreq, float maxFreq, Array<float> & centralFreqs);
    
    void createColourSpectrum(Image & imageToRenderTo, float minFreq, float maxFreq, float logScale);
    void createColourSpectrum(int fftChannel, Image & imageToRenderTo, float minFreq, float maxFreq, float logScale);
    
    void createFrequencyData(double & peakFrequency, double & peakDB, Array<float> & upperHarmonics, Array<float> & lowerHarmonics, int & keynote, int & octave, double & ema);
    void createFrequencyData(int fftChannel, double & peakFrequency, double & peakDB, Array<float> & upperHarmonics, Array<float> & lowerHarmonics, int & keynote, int & octave, double & ema);
    
    void getMovingAveragePeakData(double & _peakFreq, double & _peakDB, double & _movingAvgFreq);
    void getMovingAveragePeakData(int fftChannel, double & _peakFreq, double & _peakDB, double & _movingAvgFreq);

    //=====================================================================================
    // VU Meters
    //=====================================================================================
    
    float vuMeterValues[2];
    float getVUMeterValue(int channel);
    
    // New FFT Visualiser Stuff
    RingBuffer<float> * visualisersRingBuffer;
    
    
    //=====================================================================================
    // Oscilloscope
    //=====================================================================================
    
    int refreshRate = 33;
    int visualiserBufferSize = 44100 + 1;
    int visualiserBufferCounter = 0;
    
    void setOscilloscopeRefreshRate(int newRate)
    {
        refreshRate = newRate;
        
        visualiserBufferSize = (int)(sample_rate / refreshRate) + 1;
        
        visualiserRingBuffer.clear();
        visualiserRingBuffer.setSize(2, visualiserBufferSize);
        
        visualiserBufferCounter = 0;
    }
    
    AudioBuffer<float> visualiserRingBuffer;
    
    AudioBuffer<float> getOscillscopeBuffer()
    {
        return AudioBuffer<float>(visualiserRingBuffer);
    }
    
    
    
    
    //=====================================================================================
    // Recorder (delegated to RecordingManager)
    //=====================================================================================

    #pragma mark Record Functions

    RecordingManager recordingManager;

    void createNewFileForRecordingRealtimeAnalysis();
    void createNewFileForRecordingChordPlayer();
    void createNewFileForRecordingFrequencyPlayer();

    void startRecording();
    void stopRecording();
    bool isRecording() const;
    uint64 getRecordCounterInMilliseconds();
    
    //==============================================================================
    // Log File Writer (extracted to LogFileWriter.h)
    //==============================================================================
    std::unique_ptr<LogFileWriter> logFileWriter;
    
    String getProjectVersionString()
    {
      return String(ProjectInfo::versionString);
    }
    
    String getProjectName()
    {
        return String(ProjectInfo::projectName);
    }
    
    void setPanicButton();
    bool getIsPanicButtonDown();
    
    void saveProfileForMode(AUDIO_MODE mode);
    void loadProfileForMode(AUDIO_MODE mode);
    
    void getOutputsPopupMenu(PopupMenu & outputMenu)
    {
        outputMenu.addItem(AUDIO_OUTPUTS::NO_OUTPUT, "NO OUTPUT");
        outputMenu.addItem(AUDIO_OUTPUTS::MONO_1, "MONO 1");
        outputMenu.addItem(AUDIO_OUTPUTS::MONO_2, "MONO 2");
        outputMenu.addItem(AUDIO_OUTPUTS::MONO_3, "MONO 3");
        outputMenu.addItem(AUDIO_OUTPUTS::MONO_4, "MONO 4");
        outputMenu.addItem(AUDIO_OUTPUTS::MONO_5, "MONO 5");
        outputMenu.addItem(AUDIO_OUTPUTS::MONO_6, "MONO 6");
        outputMenu.addItem(AUDIO_OUTPUTS::MONO_7, "MONO 7");
        outputMenu.addItem(AUDIO_OUTPUTS::MONO_8, "MONO 8");
        outputMenu.addItem(AUDIO_OUTPUTS::STEREO_1_2, "STEREO 1 & 2");
        outputMenu.addItem(AUDIO_OUTPUTS::STEREO_3_4, "STEREO 3 & 4");
        outputMenu.addItem(AUDIO_OUTPUTS::STEREO_5_6, "STEREO 5 & 6");
        outputMenu.addItem(AUDIO_OUTPUTS::STEREO_7_8, "STEREO 7 & 8");
    }
    
private:
    // FIXED: Exception-safe helper methods
    void cleanup() noexcept;
    void initializeApplicationProperties();
    void initializePluginSystem();
    void initializeProcessors();
    void initializeAnalysisProcessors();

//===============================================================================
#pragma mark Profiles / Presets Save - Load
//===============================================================================
    
    void saveSettingsFile();
    void loadSettingsFile();
    
    //===============================================================================
    #pragma mark Multichannel audio
    //===============================================================================





    //===============================================================================
    #pragma mark Panic Button White Noise Generator
    //===============================================================================

    enum NOISE_TYPE { WHITE_NOISE = 0, PINK_NOISE } noiseType;

    WhiteNoise     noiseWhite;
    PinkNoise      noisePink;

    bool panicButtonIsDown = false;

    void processPanicButtonNoise(AudioBuffer<float>& buffer);
    void processWhiteNoise(AudioBuffer<float>& buffer);
    void processPinkNoise(AudioBuffer<float>& buffer);
    
    void getInputsPopupMenu(PopupMenu & inputMenu)
    {
        inputMenu.addItem(AUDIO_OUTPUTS::NO_OUTPUT, "NO INPUT");
        inputMenu.addItem(AUDIO_OUTPUTS::MONO_1, "MONO 1");
        inputMenu.addItem(AUDIO_OUTPUTS::MONO_2, "MONO 2");
        inputMenu.addItem(AUDIO_OUTPUTS::MONO_3, "MONO 3");
        inputMenu.addItem(AUDIO_OUTPUTS::MONO_4, "MONO 4");
        
        inputMenu.addItem(AUDIO_OUTPUTS::STEREO_1_2, "STEREO 1 & 2");
        inputMenu.addItem(AUDIO_OUTPUTS::STEREO_3_4, "STEREO 3 & 4");
    }
};



 
 
