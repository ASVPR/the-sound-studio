/*
  ==============================================================================

    LogFileWriter.cpp

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#include "LogFileWriter.h"
#include "ProjectManager.h"
#include "TSSPaths.h"
#include "TSSConstants.h"

LogFileWriter::LogFileWriter(ProjectManager* pm)
{
    projectManager   = pm;
    logFileDirectory = &projectManager->logFileDirectory;
    frequencyManager = projectManager->frequencyManager.get();
    logAudioMode     = AUDIO_MODE::MODE_CHORD_PLAYER;
}

LogFileWriter::~LogFileWriter() {}

void LogFileWriter::createNewFileForRealtimeAnalysisLogging()
{
    auto filename = TSS::TSSPaths::buildTimestampedFilename("Realtime-Analysis");
    File newLogFile(logFileDirectory->getChildFile(filename));
    newLogFile.create();

    currentLogFile_RealtimeAnalysis = newLogFile;
}

//==============================================================================
void LogFileWriter::setupLogRecording (const File& file)
{
    currentLogFile_RealtimeAnalysis  = file;
    logString       = "";
}



void LogFileWriter::processLog(double peakFrequency, double peakDB, Array<float> upperHarmonics, Array<float> intervals, int keynote, int octave, double ema)
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

void LogFileWriter::startRecordingLog ()
{
    shouldRecordLog = true;
    
    logString.clear();
}

void LogFileWriter::stopRecordingLog()
{
    shouldRecordLog = false;
    
    currentLogFile_RealtimeAnalysis.appendText(logString);
}


bool LogFileWriter::isRecordingLog()
{
    return shouldRecordLog;
}


// When audioMode changes, create a new log file timestamped...
void LogFileWriter::initNewLogFileForAudioMode(AUDIO_MODE newMode)
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
        auto filename = TSS::TSSPaths::buildTimestampedFilename(identifier);
        File newLogFile(logFileDirectory->getChildFile(filename));
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

void LogFileWriter::processLog_ChordPlayer_Parameters()
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
            else { string_Waveform = TSS::Waveforms::getName(waveformType);  string_Instrument = "N/A"; }


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

void LogFileWriter::processLog_ChordPlayer_Sequencer(int shortcutRef, const Array<String>& noteStrings, const Array<float>& noteFreqs)
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
void LogFileWriter::processLog_ChordScanner_Parameters()
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
    else { string_Waveform = TSS::Waveforms::getName(waveformType);  string_Instrument = "N/A"; }

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

void LogFileWriter::processLog_ChordScanner_Sequencer(bool isAllChords, Array<int> notes, Array<float> noteFreqs)
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
void LogFileWriter::processLog_FrequencyPlayer_Parameters()
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
            
            string_Waveform = TSS::Waveforms::getName(waveformType);

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

void LogFileWriter::processLog_FrequencyPlayer_Sequencer(int shortcutRef, float freq)
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
void LogFileWriter::processLog_FrequencyScanner_Parameters()
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
    string_Waveform = TSS::Waveforms::getName(waveformType);

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

void LogFileWriter::processLog_FrequencyScanner_Sequencer(float freq)
{
    String newEntry("\n\n");
    
    newEntry.append(getDateAndTimeString(), 100); newEntry.append(" | ", 3);
    newEntry.append("FREQUENCY SCANNER OUTPUT ", 40);      newEntry.append(" | ", 3);
    
    String stringFrequency(freq, 3, false);
    stringFrequency.append(" hz", 3);
    newEntry.append(stringFrequency, 40);
    
    logFile[AUDIO_MODE::MODE_FREQUENCY_SCANNER].appendText(newEntry);
    
}

void LogFileWriter::processLog_FundamentalFrequency_Sequencer(                                                                const String& fundamental, const juce::String &chord, const Array<String>& harmonics)
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

void LogFileWriter::processLog_FrequencyToLight(String conversionType, String base, String wavelength, String rgbHex, StringArray manipulationStrings)
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

void LogFileWriter::processLog_PanicButtonPressed(int noise_type)
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

void LogFileWriter::initNewSettingsLogFile()
{
    auto filename = TSS::TSSPaths::buildTimestampedFilename("Settings");
    File newLogFile(logFileDirectory->getChildFile(filename));
    newLogFile.create();

    logFileSettings = newLogFile;
    
}

void LogFileWriter::processLog_Settings_Parameters()
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
void LogFileWriter::checkForOldLoggingFile()
{
    // scan directory
    
    // get current time
    
    // calculate dif between now and file time
    
    // if dif > 7 days, delete file
}

String LogFileWriter::getDateAndTimeString()
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

String LogFileWriter::getScaleString(int scaleRef)
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
