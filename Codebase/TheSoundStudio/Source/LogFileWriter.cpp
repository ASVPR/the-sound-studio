/*
  ==============================================================================

    LogFileWriter.cpp
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

//
//#include "LogFileWriter.h"
//
//
//void LogFileWriter::createNewFileForRealtimeAnalysisLogging()
//{
//    // start recording in LogFileWriter
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
//    
//    currentLogFile_RealtimeAnalysis = newLogFile;
//}
//
////==============================================================================
//void LogFileWriter::setupLogRecording (const File& file)
//{
//    currentLogFile_RealtimeAnalysis  = file;
//    logString       = "";
//}
//
//
//
//void LogFileWriter::processLog(double peakFrequency, double peakDB, Array<float> upperHarmonics, Array<float> lowerHarmonics, int keynote, int octave, double ema)
//{
//    if (shouldRecordLog)
//    {
//        // called when its time to grab the logs.
//        
//        // start recording in LogFileWriter
//        Time time = Time::getCurrentTime();
//        
//        String logEntry("LogTime : ");
//        String MS(time.getMilliseconds());
//        String S(time.getSeconds());
//        String M(time.getMinutes());
//        String HH(time.getHours());
//        
//        logEntry.append(HH, 4); logEntry.append(":", 3);
//        logEntry.append(M, 4); logEntry.append(":", 3);
//        logEntry.append(S, 4); logEntry.append(":", 3);
//        logEntry.append(MS, 4);
//        
//        // grab values
//
//        String peakFreqString("\nPeak Frequency : ", 20);
//        String f(peakFrequency, 3, false);
//        peakFreqString.append(f, 20);
//        peakFreqString.append("hz", 2);
//        
//        String peakDbString("\nPeak dB : ", 20);
//        String d(peakDB, 3, false);
//        peakDbString.append(d, 20);
//        peakDbString.append("dB", 2);
//        
//        
//        int midiNote    = -1;
//        float freqDif   = 0.f;
//        
//        frequencyManager->getMIDINoteForFrequency(peakFrequency, midiNote, keynote, octave, freqDif);
//        
//        String keynoteString("\nKeynote : ", 20);
//        
//        if (midiNote >= 0)
//        {
//            String key(ProjectStrings::getKeynoteArray().getReference(keynote));
//            
//            if (freqDif > 0.f)
//            {
//                key.append(" +", 3);
//            }
//            else
//            {
//                key.append(" ", 3);
//            }
//            
//            key.append(String(freqDif, 3, false), 7);
//            key.append(" hz", 3);
//            
//            keynoteString.append(key, 20);
//        }
//        String emaString("\nEMA : ", 7);
//        String emaRes(ema, 3, false); emaRes.append("hz", 3);
//        emaString.append(emaRes, 20);
//        
//        
//        logString.append("\n", 3);
//        
//        logString.append(logEntry, 60);
//        logString.append(peakFreqString, 60);
//        logString.append(peakDbString, 60);
//        logString.append(keynoteString, 60);
//        logString.append(emaString, 60);
//        logString.append("\n ======================================", 30);
//        // add string to main string
//        
//    }
//    
//    
//}
//
//void LogFileWriter::startRecordingLog ()
//{
//    
//    shouldRecordLog = true;
//}
//
//void LogFileWriter::stopRecordingLog()
//{
//    shouldRecordLog = false;
//    
//    // add string to file
//    currentLogFile_RealtimeAnalysis.replaceWithText(logString);
//    
//    // close file
//}
//
//
//bool LogFileWriter::isRecordingLog()
//{
//    return shouldRecordLog;
//}
//
//
//
//
//
//// When audioMode changes, create a new log file timestamped...
//void LogFileWriter::initNewLogFileForAudioMode(AUDIO_MODE newMode)
//{
//    if (newMode == AUDIO_MODE::MODE_CHORD_PLAYER)
//    {
//        Time time = Time::getCurrentTime();
//
//        String M(time.getMinutes());
//        String HH(time.getHours());
//        String DD(time.getDayOfMonth());
//        String MM(time.getMonth() + 1);
//        String YY(time.getYear());
//        
//        String filename("");
//        filename.append(DD, 4); filename.append(".", 3);
//        filename.append(MM, 4); filename.append(".", 3);
//        filename.append(YY, 4); filename.append("-", 3);
//        filename.append(HH, 4); filename.append("-", 3);
//        filename.append(M, 4); filename.append("-", 3);
//        
//        filename.append("Chord-Player", 30);
//        
//        String url(logFileDirectory->getFullPathName());
//        
//        url.append("/", 2);
//        url.append(filename, 100);
//        url.append(".txt", 4);
//        
//        File newLogFile(url);
//        newLogFile.create();
//        
//        logFileChordPlayer = newLogFile;
//    }
//    else if (newMode == AUDIO_MODE::MODE_CHORD_SCANNER)
//    {
//        Time time = Time::getCurrentTime();
//        
//        String M(time.getMinutes());
//        String HH(time.getHours());
//        String DD(time.getDayOfMonth());
//        String MM(time.getMonth() + 1);
//        String YY(time.getYear());
//        
//        String filename("");
//        filename.append(DD, 4); filename.append(".", 3);
//        filename.append(MM, 4); filename.append(".", 3);
//        filename.append(YY, 4); filename.append("-", 3);
//        filename.append(HH, 4); filename.append("-", 3);
//        filename.append(M, 4); filename.append("-", 3);
//        
//        filename.append("Chord-Scanner", 30);
//        
//        String url(logFileDirectory->getFullPathName());
//        
//        url.append("/", 2);
//        url.append(filename, 100);
//        url.append(".txt", 4);
//        
//        File newLogFile(url);
//        newLogFile.create();
//        
//        logFileChordScanner = newLogFile;
//    }
//    else if (newMode == AUDIO_MODE::MODE_FREQUENCY_PLAYER)
//    {
//        Time time = Time::getCurrentTime();
//        
//        String M(time.getMinutes());
//        String HH(time.getHours());
//        String DD(time.getDayOfMonth());
//        String MM(time.getMonth() + 1);
//        String YY(time.getYear());
//        
//        String filename("");
//        filename.append(DD, 4); filename.append(".", 3);
//        filename.append(MM, 4); filename.append(".", 3);
//        filename.append(YY, 4); filename.append("-", 3);
//        filename.append(HH, 4); filename.append("-", 3);
//        filename.append(M, 4); filename.append("-", 3);
//        
//        filename.append("Frequency-Player", 30);
//        
//        String url(logFileDirectory->getFullPathName());
//        
//        url.append("/", 2);
//        url.append(filename, 100);
//        url.append(".txt", 4);
//        
//        File newLogFile(url);
//        newLogFile.create();
//        
//        logFileFrequencyPlayer = newLogFile;
//    }
//    else if (newMode == AUDIO_MODE::MODE_FREQUENCY_SCANNER)
//    {
//        Time time = Time::getCurrentTime();
//        
//        String M(time.getMinutes());
//        String HH(time.getHours());
//        String DD(time.getDayOfMonth());
//        String MM(time.getMonth() + 1);
//        String YY(time.getYear());
//        
//        String filename("");
//        filename.append(DD, 4); filename.append(".", 3);
//        filename.append(MM, 4); filename.append(".", 3);
//        filename.append(YY, 4); filename.append("-", 3);
//        filename.append(HH, 4); filename.append("-", 3);
//        filename.append(M, 4); filename.append("-", 3);
//        
//        filename.append("Frequency-Scanner", 30);
//        
//        String url(logFileDirectory->getFullPathName());
//        
//        url.append("/", 2);
//        url.append(filename, 100);
//        url.append(".txt", 4);
//        
//        File newLogFile(url);
//        newLogFile.create();
//        
//        logFileFrequencyScanner = newLogFile;
//    }
//    else if (newMode == AUDIO_MODE::MODE_FREQUENCY_TO_LIGHT)
//    {
//        Time time = Time::getCurrentTime();
//        
//        String M(time.getMinutes());
//        String HH(time.getHours());
//        String DD(time.getDayOfMonth());
//        String MM(time.getMonth() + 1);
//        String YY(time.getYear());
//        
//        String filename("");
//        filename.append(DD, 4); filename.append(".", 3);
//        filename.append(MM, 4); filename.append(".", 3);
//        filename.append(YY, 4); filename.append("-", 3);
//        filename.append(HH, 4); filename.append("-", 3);
//        filename.append(M, 4); filename.append("-", 3);
//        
//        filename.append("Frequency-To-Light", 30);
//        
//        String url(logFileDirectory->getFullPathName());
//        
//        url.append("/", 2);
//        url.append(filename, 100);
//        url.append(".txt", 4);
//        
//        File newLogFile(url);
//        newLogFile.create();
//        
//        logFileFrequencyToLight = newLogFile;
//    }
//    else if (newMode == AUDIO_MODE::MODE_REALTIME_ANALYSIS)
//    {
//        // has own functions for recording...
//    }
//    else if (newMode == AUDIO_MODE::MODE_LISSAJOUS_CURVES)  {} // Doesnt log yet ***
//}
//
//// chordPlayer
//// Called every time th user changes settings and presses play !
////
//void LogFileWriter::processLog_ChordPlayer()
//{
//    // called every parameter change
//    String outputString;
//    
//    // print parameters for each active chord
//    for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
//    {
//        String newEntry("\n");;
//        
//        // check if its active
//        
//        bool active = chordPlayerParameters[i]->getProperty(getIdentifierForChordPlayerParameterIndex(SHORTCUT_IS_ACTIVE));
//        
//        if (active)
//        {
//            // Waveform Type
//            String string_Waveform;
//            String string_Instrument;
//            
//            int waveformType = getChordPlayerParameter(i, WAVEFORM_TYPE).operator int();
//            if (waveformType == 0) // playing instrument
//            {
//                // PLAYING_INSTRUMENT
//                int instrumentType = getChordPlayerParameter(i, INSTRUMENT_TYPE).operator int();
//                
//                // get string for type
//                String instString;
//                
//                File dir = File::getSpecialLocation (File::userDocumentsDirectory).getChildFile ("ASVPR/SampleLibrary/Playing Instruments");
//                
//                Array<juce::File> results; dir.findChildFiles(results, juce::File::TypesOfFileToFind::findDirectories , false);
//                
//                File instrumentDir(results[instrumentType]); String instName(instrumentDir.getFileName());
//                
//                string_Waveform     = "Playing Instrument";
//                string_Instrument   = instName;
//            }
//            else if (waveformType == 1) { string_Waveform = "Sinewave"; }
//            else if (waveformType == 2) { string_Waveform = "Triangle"; }
//            else if (waveformType == 3) { string_Waveform = "Square";   }
//            else if (waveformType == 4) { string_Waveform = "Sawtooth"; }
//            else if (waveformType == 5) { string_Waveform = "Wavetable";}
//            
//            
//            // KEY NOTE
//            int keyNote = getChordPlayerParameter(i, KEYNOTE).operator int();
//            
//            String string_Keynote(ProjectStrings::getKeynoteArray().getReference(keyNote));
//            
//            // CHORD_TYPE
//            int chordType = getChordPlayerParameter(i, CHORD_TYPE).operator int();
//            
//            String stringChordtype(ProjectStrings::getChordTypeArray().getReference(chordType));
//
//            // notes in chord
//            Array<double> noteFreqs     = chordPlayerProcessor->chordManager[i]->getFrequenciesForChord();
//            Array<int> notes            = chordPlayerProcessor->chordManager[i]->getMIDIKeysForChord();
//            
//            
//            String stringNoteFrequencies;
//            for (int i = 0; i < notes.size(); i++)
//            {
//                // get the note
//                int key         = notes.getReference(i);
//                int octave      = getChordPlayerParameter(i, OCTAVE).operator int();
//                int midiNote    = (octave * 12) + key;
////                    double noteFreq = chordPlayerProcessor->chordManager[i]->getFrequencyForMIDINote(midiNote);
//
//                String keyNote(ProjectStrings::getKeynoteArray().getReference(key));
//                String oct(octave); keyNote.append(oct, 2);
//                
//                String keyFreqString(noteFreqs.getReference(i), 3, false); keyFreqString.append("hz", 2);
//                
//                stringNoteFrequencies.append(keyNote, 4);
//                stringNoteFrequencies.append(" ", 4);
//                stringNoteFrequencies.append(keyFreqString, 10);
//                stringNoteFrequencies.append(" - ", 4);
//                
//                
//            }
//            
//            // MANIPULATION_MULTIPLICATION
//            bool manipulateFreq     = getChordPlayerParameter(i, MANIPULATE_CHOSEN_FREQUENCY).operator bool();
//            bool multOrDivide       = getChordPlayerParameter(i, MULTIPLY_OR_DIVISION).operator bool();
//
//            float multVal = getChordPlayerParameter(i, MULTIPLY_VALUE).operator float();
//            String multString(multVal);
//            
//            float divVal = getChordPlayerParameter(i, DIVISION_VALUE).operator float();
//            String divString(divVal);
//
//            // ADSR
//            double adsr_amp     = getChordPlayerParameter(i, ENV_AMPLITUDE).operator double();
//            double adsr_a       = getChordPlayerParameter(i, ENV_ATTACK).operator double();
//            double adsr_s       = getChordPlayerParameter(i, ENV_SUSTAIN).operator double();
//            double adsr_d       = getChordPlayerParameter(i, ENV_DECAY).operator double();
//            double adsr_r       = getChordPlayerParameter(i, ENV_RELEASE).operator double();
//            
//            String ampString(adsr_amp);     ampString.append("%", 1);
//            String attackString(adsr_a);    attackString.append("ms", 2);
//            String sustainString(adsr_s);   sustainString.append("%", 1);
//            String decayString(adsr_d);     decayString.append("ms", 2);
//            String releaseString(adsr_r);   releaseString.append("ms", 2);
//
//            int pauseVal        = getChordPlayerParameter(i, NUM_PAUSE).operator int();
//            String pauseString(pauseVal); pauseString.append("ms", 2);
//
//            int lengthVal       = getChordPlayerParameter(i, NUM_DURATION).operator int();
//            String lengthString(lengthVal); lengthString.append("ms", 2);
//            
//            int numRepeats      = getChordPlayerParameter(i, NUM_REPEATS).operator int();
//            String repeatsString(numRepeats);
//
//            // add to new Entry for individual chordplayer shortcut unit
//            String chordPlayerString("Chord Player Unit "); String unit(i); chordPlayerString.append(unit, 2);
//            
//            newEntry.append(chordPlayerString, 20); newEntry.append(" | ", 3);
//            
//            // chord type
//            newEntry.append(stringChordtype, 20); newEntry.append(" | ", 3);
//            
//            // note frequencies
//            newEntry.append(stringNoteFrequencies, 300); newEntry.append(" | ", 3);
//            
//            // instrument
//            newEntry.append(string_Waveform, 50); newEntry.append(" | ", 3);
//            
//            if (waveformType == 0) // playing instrument
//            {
//                newEntry.append(string_Instrument, 50); newEntry.append(" | ", 3);
//            }
//            
//            if (manipulateFreq)
//            {
//                newEntry.append("Manipulate ", 50);
//                
//                if (multOrDivide)
//                {
//                    newEntry.append("Multiplication ", 50);
//                    
//                    String manValue(multVal, 3);
//                    
//                    newEntry.append(manValue, 10);
//                    
//                    newEntry.append(" | ", 3);
//                }
//                else
//                {
//                    newEntry.append("Division ", 50);
//                    
//                    String manValue(divVal, 3);
//                    
//                    newEntry.append(manValue, 10);
//                    
//                    newEntry.append(" | ", 3);
//                }
//            }
//            
//            // ADSR
//            newEntry.append(ampString, 10);     newEntry.append(" | ", 3);
//            newEntry.append(attackString, 10);  newEntry.append(" | ", 3);
//            newEntry.append(sustainString, 10); newEntry.append(" | ", 3);
//            newEntry.append(decayString, 10);   newEntry.append(" | ", 3);
//            newEntry.append(releaseString, 10); newEntry.append(" | ", 3);
//            
//            newEntry.append(lengthString, 10);  newEntry.append(" | ", 3);
//            newEntry.append(pauseString, 10);   newEntry.append(" | ", 3);
//            newEntry.append(repeatsString, 10);
//            
//            // add to output string
//            outputString.append(newEntry, 1000);
//        }
//    }
//    
//    logFileChordPlayer.appendText(outputString);
//    
//}
//
//// ChordScanner
//void LogFileWriter::processLog_ChordScanner_init()
//{
//    // Called
//    
//    int chordScanType   = getChordScannerParameter(CHORD_SCANNER_MODE).operator int();
//    String chordScanString;
//    
//    if (chordScanType == 0)
//    {
//        chordScanString = "Scan Only Main Chords ";
//    }
//    else if (chordScanType == 1)
//    {
//        chordScanString = "Scan All Chords ";
//    }
//    else if (chordScanType == 2)
//    {
//        chordScanString = "Scan Specific Range ";
//    }
//    else if (chordScanType == 3)
//    {
//        chordScanString = "Scan by Frequency ";
//    }
//
//    // Waveform Type
//    String string_Waveform;
//    String string_Instrument;
//    
//    int waveformType = getChordScannerParameter(CHORD_SCANNER_WAVEFORM_TYPE).operator int();
//    if (waveformType == 0) // playing instrument
//    {
//        // PLAYING_INSTRUMENT
//        int instrumentType = getChordScannerParameter(CHORD_SCANNER_INSTRUMENT_TYPE).operator int();
//        
//        // get string for type
//        String instString;
//        
//        File dir = File::getSpecialLocation (File::userDocumentsDirectory).getChildFile ("ASVPR/SampleLibrary/Playing Instruments");
//        
//        Array<juce::File> results; dir.findChildFiles(results, juce::File::TypesOfFileToFind::findDirectories , false);
//        
//        File instrumentDir(results[instrumentType]); String instName(instrumentDir.getFileName());
//        
//        string_Waveform     = "Playing Instrument";
//        string_Instrument   = instName;
//    }
//    else if (waveformType == 1) { string_Waveform = "Sinewave";  string_Instrument = "N/A"; }
//    else if (waveformType == 2) { string_Waveform = "Triangle";  string_Instrument = "N/A"; }
//    else if (waveformType == 3) { string_Waveform = "Square";    string_Instrument = "N/A"; }
//    else if (waveformType == 4) { string_Waveform = "Sawtooth";  string_Instrument = "N/A"; }
//    else if (waveformType == 5) { string_Waveform = "Wavetable"; string_Instrument = "N/A"; }
//    
//
//    int keynoteTo       = getChordScannerParameter(CHORD_SCANNER_KEYNOTE_TO).operator int();
//    String stringKeynoteTo(ProjectStrings::getKeynoteArray().getReference(keynoteTo));
//
//    int keynoteFrom     = getChordScannerParameter(CHORD_SCANNER_KEYNOTE_FROM).operator int();
//    String stringKeynoteFrom(ProjectStrings::getKeynoteArray().getReference(keynoteFrom));
//    
//    int octaveTo        = getChordScannerParameter(CHORD_SCANNER_OCTAVE_TO).operator int();
//    String stringOctaveTo(octaveTo);
//    
//    int octaveFrom      = getChordScannerParameter(CHORD_SCANNER_OCTAVE_FROM).operator int();
//    String stringOctaveFrom(octaveFrom);
//    
//    double freqToVal = getChordScannerParameter(CHORD_SCANNER_FREQUENCY_TO).operator double();
//    String freqToString(freqToVal, 3, false);
//    freqToString.append("hz", 2);
//
//    double freqFromVal = getChordScannerParameter(CHORD_SCANNER_FREQUENCY_FROM).operator double();
//    String freqFromString(freqFromVal, 3, false);
//    freqFromString.append("hz", 2);
//
//    int pauseVal = getChordScannerParameter(CHORD_SCANNER_NUM_PAUSE).operator int();
//    String pauseString(pauseVal);
//    pauseString.append("ms", 2);
//
//    int lengthVal = getChordScannerParameter(CHORD_SCANNER_NUM_DURATION).operator int();
//    String lengthString(lengthVal);
//    lengthString.append("ms", 2);
//    
//
//    double amplitude    = getChordScannerParameter(CHORD_SCANNER_ENV_AMPLITUDE).operator double();
//    double attack       = getChordScannerParameter(CHORD_SCANNER_ENV_ATTACK).operator double();
//    double sustain      = getChordScannerParameter(CHORD_SCANNER_ENV_SUSTAIN).operator double();
//    double decay        = getChordScannerParameter(CHORD_SCANNER_ENV_DECAY).operator double();
//    double release      = getChordScannerParameter(CHORD_SCANNER_ENV_RELEASE).operator double();
//    
//    String ampString(amplitude);        ampString.append("%", 1);
//    String attackString(attack);        attackString.append("ms", 2);
//    String sustainString(sustain);      sustainString.append("%", 1);
//    String decayString(decay);          decayString.append("ms", 2);
//    String releaseString(release);      releaseString.append("ms", 2);
//    
//
//    String newEntry("\n");
//    // add to new Entry for individual chordplayer shortcut unit
//    String chordPlayerString("Chord Scannner ");
//    
//    // SCAN TYPE
//    newEntry.append(chordScanString, 50); newEntry.append(" | ", 3);
//    
//    if (chordScanType == 2)
//    {
//        newEntry.append(stringKeynoteFrom, 20);     newEntry.append(" | ", 3);
//        newEntry.append(stringKeynoteTo, 20);       newEntry.append(" | ", 3);
//        newEntry.append(stringOctaveFrom, 20);      newEntry.append(" | ", 3);
//        newEntry.append(stringOctaveTo, 20);        newEntry.append(" | ", 3);
//    }
//    else
//    {
//        newEntry.append("N/A", 20);         newEntry.append(" | ", 3);
//        newEntry.append("N/A", 20);         newEntry.append(" | ", 3);
//        newEntry.append("N/A", 20);         newEntry.append(" | ", 3);
//        newEntry.append("N/A", 20);         newEntry.append(" | ", 3);
//    }
//    
//    newEntry.append(string_Waveform, 50);   newEntry.append(" | ", 3);
//    newEntry.append(string_Instrument, 50); newEntry.append(" | ", 3);
//    
//    // ADSR
//    newEntry.append(ampString, 10);     newEntry.append(" | ", 3);
//    newEntry.append(attackString, 10);  newEntry.append(" | ", 3);
//    newEntry.append(sustainString, 10); newEntry.append(" | ", 3);
//    newEntry.append(decayString, 10);   newEntry.append(" | ", 3);
//    newEntry.append(releaseString, 10); newEntry.append(" | ", 3);
//    
//    newEntry.append(lengthString, 10);  newEntry.append(" | ", 3);
//    newEntry.append(pauseString, 10);   newEntry.append(" | ", 3);
//
//    logFileChordScanner.appendText(newEntry);
//}
//
//void LogFileWriter::processLog_ChordScanner_addition()
//{
//    // only chord and note frequencies
//    String newEntry("\n");
//    
//    // Date
//    Time time = Time::getCurrentTime();
//    
//    String dateString("");
//    String M(time.getMinutes());
//    String HH(time.getHours());
//    String DD(time.getDayOfMonth());
//    String MM(time.getMonth() + 1);
//    String YY(time.getYear());
//    String SS(time.getSeconds());
//    
//    dateString.append(DD, 4); dateString.append(".", 3);
//    dateString.append(MM, 4); dateString.append(".", 3);
//    dateString.append(YY, 4); dateString.append(" ", 3);
//    dateString.append(HH, 4); dateString.append(":", 3);
//    dateString.append(M, 4); dateString.append(":", 3);
//    dateString.append(SS, 4); dateString.append(":", 3);
//    
//    String stringNoteFrequencies;
//       
//    Array<double> noteFreqs     = chordScannerProcessor->chordManager->getFrequenciesForChord();
//    Array<int> notes            = chordScannerProcessor->chordManager->getMIDIKeysForChord();
//    int octaveIterator          = chordScannerProcessor->repeater->octaveIterator;
//    int chordType               = chordScannerProcessor->repeater->chordTypeIterator;
//    int keynote                 = chordScannerProcessor->repeater->keynoteIterator;
//   
//    for (int i = 0; i < notes.size(); i++)
//    {
//        // get the note
//        int key         = notes.getReference(i);
//        int octave      = octaveIterator;
//        int midiNote    = (octave * 12) + key;
// 
//        String keyNote(ProjectStrings::getKeynoteArray().getReference(key));
//        String oct(octave); keyNote.append(oct, 2);
//    
//        String keyFreqString(noteFreqs.getReference(i), 3, false); keyFreqString.append("hz", 2);
//      
//        stringNoteFrequencies.append(keyNote, 4);
//        stringNoteFrequencies.append(" ", 4);
//        stringNoteFrequencies.append(keyFreqString, 10);
//        stringNoteFrequencies.append(" - ", 4);
//    }
//    
//    String stringKeynote(ProjectStrings::getKeynoteArray().getReference(keynote));
//    String stringOctave(octaveIterator);
//    String stringChordtype(ProjectStrings::getChordTypeArray().getReference(chordType));
//    
//    newEntry.append(dateString, 30);        newEntry.append(" | ", 3);
//    newEntry.append(stringKeynote, 10);     newEntry.append(" | ", 3);
//    newEntry.append(stringChordtype, 20);   newEntry.append(" | ", 3);
//    
//    // note frequencies
//    newEntry.append(stringNoteFrequencies, 300); newEntry.append(" | ", 3);
//
//    logFileChordScanner.appendText(newEntry);
//    
//}
//
//// FrequencyPlayer
//void LogFileWriter::processLog_FrequencyPlayer()
//{
//    String outputString;
//    
//    for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
//    {
//        String newEntry("\n");;
//        
//        // check if its active
//        
//        bool active = getFrequencyPlayerParameter(i, FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE).operator bool();
//        
//        if (active)
//        {
//            bool source = getFrequencyPlayerParameter(i, FREQUENCY_PLAYER_FREQ_SOURCE).operator bool();
//            
//            String sourceString;
//            
//            if (source)
//            {
//                sourceString = "Specific Frequency ";
//            }
//            else
//            {
//                sourceString = "Range of Frequencies ";
//            }
//            
//            
//            // Waveform Type
//            String string_Waveform;
//            
//            int waveformType = getFrequencyPlayerParameter(i, FREQUENCY_PLAYER_WAVEFORM_TYPE).operator int();
//            
//            if (waveformType == 0)      { string_Waveform = "Default"; }
//            else if (waveformType == 1) { string_Waveform = "Sinewave"; }
//            else if (waveformType == 2) { string_Waveform = "Triangle"; }
//            else if (waveformType == 3) { string_Waveform = "Square"; }
//            else if (waveformType == 4) { string_Waveform = "Sawtooth"; }
//            else if (waveformType == 5) { string_Waveform = "Wavetable"; }
//
//            int pauseVal = getFrequencyPlayerParameter(i, FREQUENCY_PLAYER_NUM_PAUSE).operator int();
//            String pauseString(pauseVal);
//            pauseString.append("ms", 2);
//
//            int lengthVal = getFrequencyPlayerParameter(i, FREQUENCY_PLAYER_NUM_DURATION).operator int();
//            String lengthString(lengthVal);
//            lengthString.append("ms", 2);
//            
//            int numRepeats      = getFrequencyPlayerParameter(i, NUM_REPEATS).operator int();
//            String repeatsString(numRepeats);
//            
//
//            double amplitude    = getFrequencyPlayerParameter(i, FREQUENCY_PLAYER_AMPLITUDE).operator double();
//            double attack       = getFrequencyPlayerParameter(i, FREQUENCY_PLAYER_ATTACK).operator double();
//            double sustain      = getFrequencyPlayerParameter(i, FREQUENCY_PLAYER_SUSTAIN).operator double();
//            double decay        = getFrequencyPlayerParameter(i, FREQUENCY_PLAYER_DECAY).operator double();
//            double release      = getFrequencyPlayerParameter(i, FREQUENCY_PLAYER_RELEASE).operator double();
//            
//            String ampString(amplitude);        ampString.append("%", 1);
//            String attackString(attack);        attackString.append("ms", 2);
//            String sustainString(sustain);      sustainString.append("%", 1);
//            String decayString(decay);          decayString.append("ms", 2);
//            String releaseString(release);      releaseString.append("ms", 2);
//            
//            
//            // MANIPULATION_MULTIPLICATION
//            bool manipulateFreq     = getFrequencyPlayerParameter(i, FREQUENCY_PLAYER_MANIPULATE_CHOSEN_FREQUENCY).operator bool();
//            bool multOrDivide       = getFrequencyPlayerParameter(i, FREQUENCY_PLAYER_MULTIPLY_OR_DIVISION).operator bool();
//
//            float multVal = getFrequencyPlayerParameter(i, FREQUENCY_PLAYER_MULTIPLY_VALUE).operator float();
//            String multString(multVal, 3, false);
//            
//            float divVal = getFrequencyPlayerParameter(i, FREQUENCY_PLAYER_DIVISION_VALUE).operator float();
//            String divString(divVal, 3, false);
//            
//            
//            // TextEntryBoxes
//            double insertFreqVal = getFrequencyPlayerParameter(i, FREQUENCY_PLAYER_CHOOSE_FREQ).operator double();
//            String freqString(insertFreqVal, 3, false); freqString.append("Hz", 2);
//
//            double minFreqVal = getFrequencyPlayerParameter(i, FREQUENCY_PLAYER_RANGE_MIN).operator double();
//            String minFreqString(minFreqVal, 3, false); minFreqString.append("Hz", 2);
//            
//            double maxFreqVal = getFrequencyPlayerParameter(i, FREQUENCY_PLAYER_RANGE_MAX).operator double();
//            String maxFreqString(maxFreqVal, 3, false); maxFreqString.append("Hz", 2);
//            
//            
//            
//                           // add to new Entry for individual chordplayer shortcut unit
//            String chordPlayerString("Frequency Player Unit "); String unit(i); chordPlayerString.append(unit, 2);
//            
//            newEntry.append(chordPlayerString, 20); newEntry.append(" | ", 3);
//
//            // wavecform
//            newEntry.append(string_Waveform, 50); newEntry.append(" | ", 3);
//            
//
//            if (manipulateFreq)
//            {
//                newEntry.append("Manipulate ", 50);
//                
//                if (multOrDivide)
//                {
//                    newEntry.append("Multiplication ", 50);
//                    
//                    String manValue(multVal, 3);
//                    
//                    newEntry.append(manValue, 10);
//                    
//                    newEntry.append(" | ", 3);
//                }
//                else
//                {
//                    newEntry.append("Division ", 50);
//                    
//                    String manValue(divVal, 3);
//                    
//                    newEntry.append(manValue, 10);
//                    
//                    newEntry.append(" | ", 3);
//                }
//            }
//            
//            // ADSR
//            newEntry.append(ampString, 10);     newEntry.append(" | ", 3);
//            newEntry.append(attackString, 10);  newEntry.append(" | ", 3);
//            newEntry.append(sustainString, 10); newEntry.append(" | ", 3);
//            newEntry.append(decayString, 10);   newEntry.append(" | ", 3);
//            newEntry.append(releaseString, 10); newEntry.append(" | ", 3);
//            
//            newEntry.append(lengthString, 10);  newEntry.append(" | ", 3);
//            newEntry.append(pauseString, 10);   newEntry.append(" | ", 3);
//            newEntry.append(repeatsString, 10); newEntry.append(" | ", 3);
//            
//            // Specific Frequencies
//            newEntry.append(sourceString, 40);  newEntry.append(" | ", 3);
//            newEntry.append(freqString, 10);    newEntry.append(" | ", 3);
//            newEntry.append(minFreqString, 10); newEntry.append(" | ", 3);
//            newEntry.append(maxFreqString, 10); newEntry.append(" | ", 3);
//            
//
//            // add to output string
//            outputString.append(newEntry, 1000);
//            
//        }
//        
//        logFileFrequencyPlayer.appendText(outputString);
//    }
//    
//}
//
//// FrequencyScanner
//void LogFileWriter::processLog_FrequencyScanner_init()
//{
//    String outputString;
//
//    int chordScanType   = getFrequencyScannerParameter(FREQUENCY_SCANNER_MODE).operator int();
//    String chordScanString;
//    
//    if (chordScanType == 0)         { chordScanString = "Scan All Frequencies "; }
//    else if (chordScanType == 1)    { chordScanString = "Scan Specific "; }
//    
//    // Waveform Type
//    String string_Waveform;
//    
//    int waveformType = getFrequencyScannerParameter(FREQUENCY_SCANNER_WAVEFORM_TYPE).operator int();
//    if (waveformType == 0)      { string_Waveform = "Default"; }
//    else if (waveformType == 1) { string_Waveform = "Sinewave"; }
//    else if (waveformType == 2) { string_Waveform = "Triangle";  }
//    else if (waveformType == 3) { string_Waveform = "Square";  }
//    else if (waveformType == 4) { string_Waveform = "Sawtooth"; }
//    else if (waveformType == 5) { string_Waveform = "Wavetable"; }
//    
//    double freqToVal = getFrequencyScannerParameter(FREQUENCY_SCANNER_FREQUENCY_TO).operator double();
//    String freqToString(freqToVal, 3, false);
//    freqToString.append("hz", 2);
//
//    double freqFromVal = getFrequencyScannerParameter(FREQUENCY_SCANNER_FREQUENCY_FROM).operator double();
//    String freqFromString(freqFromVal, 3, false);
//    freqFromString.append("hz", 2);
//
//    int pauseVal = getFrequencyScannerParameter(FREQUENCY_SCANNER_NUM_PAUSE).operator int();
//    String pauseString(pauseVal);
//    pauseString.append("ms", 2);
//
//    int lengthVal = getFrequencyScannerParameter(FREQUENCY_SCANNER_NUM_DURATION).operator int();
//    String lengthString(lengthVal);
//    lengthString.append("ms", 2);
//
//    double amplitude    = getFrequencyScannerParameter(FREQUENCY_SCANNER_ENV_AMPLITUDE).operator double();
//    double attack       = getFrequencyScannerParameter(FREQUENCY_SCANNER_ENV_ATTACK).operator double();
//    double sustain      = getFrequencyScannerParameter(FREQUENCY_SCANNER_ENV_SUSTAIN).operator double();
//    double decay        = getFrequencyScannerParameter(FREQUENCY_SCANNER_ENV_DECAY).operator double();
//    double release      = getFrequencyScannerParameter(FREQUENCY_SCANNER_ENV_RELEASE).operator double();
//    
//    String ampString(amplitude);        ampString.append("%", 1);
//    String attackString(attack);        attackString.append("ms", 2);
//    String sustainString(sustain);      sustainString.append("%", 1);
//    String decayString(decay);          decayString.append("ms", 2);
//    String releaseString(release);      releaseString.append("ms", 2);
//
//    String newEntry("\n");
//    // add to new Entry for individual chordplayer shortcut unit
//    String chordPlayerString("Frequency Scannner ");
//    
//    // SCAN TYPE
//    newEntry.append(chordScanString, 50); newEntry.append(" | ", 3);
//    newEntry.append(string_Waveform, 50); newEntry.append(" | ", 3);
//    
//    // ADSR
//    newEntry.append(ampString, 10);     newEntry.append(" | ", 3);
//    newEntry.append(attackString, 10);  newEntry.append(" | ", 3);
//    newEntry.append(sustainString, 10); newEntry.append(" | ", 3);
//    newEntry.append(decayString, 10);   newEntry.append(" | ", 3);
//    newEntry.append(releaseString, 10); newEntry.append(" | ", 3);
//    
//    newEntry.append(lengthString, 10);  newEntry.append(" | ", 3);
//    newEntry.append(pauseString, 10);   newEntry.append(" | ", 3);
//
//    logFileFrequencyScanner.appendText(newEntry);
//    
//}
//
//void LogFileWriter::processLog_FrequencyScanner_addition()
//{
//    String outputString;
//    
//}
//
//void LogFileWriter::processLog_FrequencyToLight()
//{
//    // but more difficult because the algorithm is contained within the UI component, not the processing / project manager.. can make a listener callback, but complicated and convoluted..
//    
//    
//    // basically needs refactoring into a processing unit...
//    
//    //
//
//}
//    
//// Other Logging functions
//
//// 1. Check age of logging files at start up and delete any older than a week
//void LogFileWriter::checkForOldLoggingFile()
//{
//    // scan directory
//    
//    // get current time
//    
//    // calculate dif between now and file time
//    
//    // if dif > 7 days, delete file
//}
