/*
  ==============================================================================

    LogFileWriter.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Parameters.h"

class ProjectManager;
class FrequencyManager;

class LogFileWriter
{
public:
    AUDIO_MODE logAudioMode;

    LogFileWriter(ProjectManager* pm);
    ~LogFileWriter();

    void createNewFileForRealtimeAnalysisLogging();

    void setupLogRecording(const File& file);

    void processLog(double peakFrequency, double peakDB, Array<float> upperHarmonics,
                    Array<float> intervals, int keynote, int octave, double ema);

    void startRecordingLog();
    void stopRecordingLog();
    bool isRecordingLog();

    void initNewLogFileForAudioMode(AUDIO_MODE newMode);

    void processLog_ChordPlayer_Parameters();
    void processLog_ChordPlayer_Sequencer(int shortcutRef, const Array<String>& noteStrings,
                                          const Array<float>& noteFreqs);

    void processLog_ChordScanner_Parameters();
    void processLog_ChordScanner_Sequencer(bool isAllChords, Array<int> notes,
                                           Array<float> noteFreqs);

    void processLog_FrequencyPlayer_Parameters();
    void processLog_FrequencyPlayer_Sequencer(int shortcutRef, float freq);

    void processLog_FrequencyScanner_Parameters();
    void processLog_FrequencyScanner_Sequencer(float freq);

    void processLog_FundamentalFrequency_Sequencer(const String& fundamental,
                                                   const String& chord,
                                                   const Array<String>& harmonics);

    void processLog_FrequencyToLight(String conversionType, String base,
                                     String wavelength, String rgbHex,
                                     StringArray manipulationStrings);

    void processLog_PanicButtonPressed(int noise_type);

    void initNewSettingsLogFile();
    void processLog_Settings_Parameters();

    void checkForOldLoggingFile();

    String getDateAndTimeString();
    String getScaleString(int scaleRef);

private:
    bool shouldRecordLog = false;

    String logString;
    uint64 recordLogSamples;
    uint64 loggingSampleRate;
    uint64 loggingCounter;

    File currentLogFile_RealtimeAnalysis;
    File logFile[AUDIO_MODE::NUM_MODES];
    File logFileSettings;

    ProjectManager*  projectManager;
    FrequencyManager* frequencyManager;
    File*            logFileDirectory;
};
