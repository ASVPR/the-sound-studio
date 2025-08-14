/*
  ==============================================================================

    LogFileWriter.h
    Created: 7 Sep 2020 9:27:57pm
    Author:  Gary Jones

  ==============================================================================
*/
//
//#pragma once
//#include "../JuceLibraryCode/JuceHeader.h"
//#include "Parameters.h"
//
//// Basically, need to embed this class into ProjectManager
//// then pass it through to ChordPlayerProcessor/ScannerProcessor etc...
////
//
//class LogFileWriter
//{
//public:
//    
//    LogFileWriter(File * logFileDir)
//    {
//        logFileDirectory = logFileDir;
//    }
//    
//    File * logFileDirectory;
//    
//    ~LogFileWriter()
//    {
//        
//    }
//    
//    void createNewFileForRealtimeAnalysisLogging();
//
//    void setupLogRecording (const File& file);
//    
//    void processLog(double peakFrequency, double peakDB, Array<float> upperHarmonics, Array<float> lowerHarmonics, int keynote, int octave, double ema);
//    
//    void startRecordingLog();
//
//    void stopRecordingLog();
//
//    bool isRecordingLog();
//    
//    
//    // When audioMode changes, create a new log file timestamped...
//    void initNewLogFileForAudioMode(AUDIO_MODE newMode);
//    
//    void processLog_ChordPlayer();
//
//    void processLog_ChordScanner_init();
//    void processLog_ChordScanner_addition();
//    
//    void processLog_FrequencyPlayer();
//
//    void processLog_FrequencyScanner_init();
//    void processLog_FrequencyScanner_addition();
//    
//    void processLog_FrequencyToLight();
//        
//    void checkForOldLoggingFile();
//    
//private:
//    
//    bool shouldRecordLog = false;
//    
//    String logString;               // String where logs are set, to be written to file later
//    uint64 recordLogSamples;        // number of samples taken thus far
//    uint64 loggingSampleRate;       // grabs log sample every x samples
//    uint64 loggingCounter;          // counts samples to trigger log
//    
//    File currentLogFile_RealtimeAnalysis;            //
//    // Other Logs for parameter changes
//    File logFileChordPlayer;
//    File logFileChordScanner;
//    File logFileFrequencyPlayer;
//    File logFileFrequencyScanner;
//    File logFileLissajous;
//    File logFileFrequencyToLight;
//    
//    
//};
