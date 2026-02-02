/*
  ==============================================================================

    ChordScannerProcessor.cpp

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#include "ChordScannerProcessor.h"
#include "ProjectManager.h"
#include "AudioRouting.h"

ChordScannerProcessor::ChordScannerProcessor(FrequencyManager * fm, SynthesisLibraryManager * slm, ProjectManager * pm)
{
    projectManager          = pm;
    frequencyManager        = fm;
    sampleLibraryManager    = slm;

    synth           = new ChordSynthProcessor(44100, frequencyManager);
    synth           ->setNoteStealingEnabled(true);
        
    sampler         = new SamplerProcessor(frequencyManager, sampleLibraryManager, 44100, -1);
    sampler         ->setNoteStealingEnabled(true);
    
    wavetableSynth   = new WavetableSynthProcessor(44100, frequencyManager);
    wavetableSynth   ->setNoteStealingEnabled(true);
        
    chordManager    = new ChordManager(frequencyManager);
        
    shouldMute      = false;
    waveformType    = SAMPLER; // Keep original - will redirect SAMPLER to use synthesis
    
    repeater        = new PlayRepeater(this, 44100);
}

ChordScannerProcessor::~ChordScannerProcessor()
{
    // Properly clean up allocated resources
    if (synth)
    {
        delete synth;
        synth = nullptr;
    }
    
    if (sampler)
    {
        delete sampler;
        sampler = nullptr;
    }
    
    if (wavetableSynth)
    {
        delete wavetableSynth;
        wavetableSynth = nullptr;
    }
    
    if (chordManager)
    {
        delete chordManager;
        chordManager = nullptr;
    }
    
    if (repeater)
    {
        delete repeater;
        repeater = nullptr;
    }
}

void ChordScannerProcessor::prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock)
{

    synth       ->setCurrentPlaybackSampleRate(sampleRate);
    sampler     ->setCurrentPlaybackSampleRate(sampleRate);
    wavetableSynth   ->prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);
    
    repeater    ->prepareToPlay(sampleRate);
    
    outputBuffer.clear();
    
    outputBuffer.setSize(1, maximumExpectedSamplesPerBlock);
}

void ChordScannerProcessor::setParameter(int index, var newValue)
{
    if (index == CHORD_SCANNER_WAVEFORM_TYPE)
    {
        index = WAVEFORM_TYPE;
        
        waveformType = (WAVEFORM_TYPES)newValue.operator int();
        synth->setParameter(index, newValue);
    }
    else if (index == CHORD_SCANNER_INSTRUMENT_TYPE)
    {
        index = INSTRUMENT_TYPE;
        synth->setParameter(index, newValue);
        sampler->setParameter(index, newValue);
        wavetableSynth->setParameter(index, newValue);
    }
    else if (index == CHORD_SCANNER_ENV_AMPLITUDE)
    {
        index = ENV_AMPLITUDE;
        synth->setParameter(index, newValue);
        sampler->setParameter(index, newValue);
        wavetableSynth->setParameter(index, newValue);
    }
    else if (index == CHORD_SCANNER_ENV_ATTACK)
    {
        index = ENV_ATTACK;
        synth->setParameter(index, newValue);
        sampler->setParameter(index, newValue);
        wavetableSynth->setParameter(index, newValue);
    }
    else if (index == CHORD_SCANNER_ENV_DECAY)
    {
        index = ENV_DECAY;
        synth->setParameter(index, newValue);
        sampler->setParameter(index, newValue);
        wavetableSynth->setParameter(index, newValue);
    }
    else if (index == CHORD_SCANNER_ENV_SUSTAIN)
    {
        index = ENV_SUSTAIN;
        synth->setParameter(index, newValue);
        sampler->setParameter(index, newValue);
        wavetableSynth->setParameter(index, newValue);
    }
    else if (index == CHORD_SCANNER_ENV_RELEASE)
    {
        index = ENV_RELEASE;
        synth->setParameter(index, newValue);
        sampler->setParameter(index, newValue);
        wavetableSynth->setParameter(index, newValue);
        
    }
    else if (index == CHORD_SCANNER_MODE)
    {
        setScanMode((SCANNER_MODE)newValue.operator int());
    }
    else if (index ==CHORD_SCANNER_NUM_REPEATS)
    {
        repeater->setNumRepeats(newValue.operator int());
    }
    else if (index == CHORD_SCANNER_NUM_PAUSE)
    {
        repeater->setPauseMS(newValue.operator int());
    }
    else if (index == CHORD_SCANNER_NUM_DURATION)
    {
        repeater->setLengthMS(newValue.operator int());
    }
    
    else if (index == CHORD_SCANNER_FREQUENCY_FROM)
    {
        repeater->setFrequencyFrom(newValue.operator float());
    }
    else if (index == CHORD_SCANNER_FREQUENCY_TO)
    {
        repeater->setFrequencyTo(newValue.operator float());
    }
    else if (index == CHORD_SCANNER_KEYNOTE_FROM)
    {
        repeater->setKeynoteFrom(newValue.operator int());
    }
    else if (index == CHORD_SCANNER_KEYNOTE_TO)
    {
        repeater->setKeynoteTo(newValue.operator int());
    }
    else if (index == CHORD_SCANNER_OCTAVE_FROM)
    {
        repeater->setOctaveFrom(newValue.operator int() + 1);
    }
    else if (index == CHORD_SCANNER_OCTAVE_TO)
    {
        repeater->setOctaveTo(newValue.operator int() + 1);
    }
    else if (index == CHORD_SCANNER_OCTAVE_EXTENDED)
    {
        // not done yet
        
        bool should = newValue.operator bool();
        
        if(should) repeater->maxOctaves = 16 + 1;
        else repeater->maxOctaves = 10 + 1;
    }
    else if (index == CHORD_SCANNER_OUTPUT_SELECTION)
    {
        output = (AUDIO_OUTPUTS)newValue.operator int();
    }
}

void ChordScannerProcessor::setOversamplingFactor(int newFactor)
{
    // ** NOT WORKING YET
    suspendProcessing(true);
    
    for (int i = 0; i < 1; i++)
    {
//        synth->setOversamplingFactor(newFactor);
        
    }
    
    suspendProcessing(false);
}

void ChordScannerProcessor::processBlock (AudioBuffer<float>& buffer,
                                         MidiBuffer& midiMessages)
{
    
    
    // process repeater first
    if (playState == PLAY_STATE::PLAYING)
    {
        // run repeater looper..
        repeater->tickBuffer(buffer.getNumSamples());
    }
    
    if (output != AUDIO_OUTPUTS::NO_OUTPUT)
    {
        outputBuffer.clear();
        
        if (waveformType == SAMPLER)
        {
            // FIXED: Redirect playing instruments to use synthesis instead of samples
            wavetableSynth->processBlock(outputBuffer, midiMessages);
        }
        else if (waveformType == WAVETABLE)
        {
            wavetableSynth->processBlock(outputBuffer, midiMessages);
        }
        else
        {
            synth->processBlock(outputBuffer, midiMessages);
        }
        
        TSS::Audio::routeToOutput(buffer, outputBuffer, output);
    }
    

}

// trigger commands
// temp functions, called when shortcut keys are pressed
// can be evolved to trigger the repeater later, or called by by repeater
void ChordScannerProcessor::triggerKeyDown(int shortcutRef) { }
void ChordScannerProcessor::triggerKeyUp(int shortcutRef) { }

void ChordScannerProcessor::triggerNoteOn(KEYNOTES keynote, CHORD_TYPES chordType, int octave)
{
    chordManager->setChordType(chordType);
    chordManager->setKeyNote(keynote);
    
    // get chord notes from parameters
    Array<int> notes(chordManager->getMIDIKeysForChord());
    
    Array<float> noteFreqs;
    
    for (int i = 0; i < notes.size(); i++)
    {
        int midiNote = notes.getReference(i);
        
        if (midiNote >= 0 && midiNote <= 127)
        {
            double baseFreq = frequencyManager->scalesManager->getFrequencyForMIDINote(midiNote);
            double freq = baseFreq * pow(2.0, octave);
            
            
            noteFreqs.add(freq);

            if (freq) // check freq does not = 0hz
            {
                if (waveformType == SAMPLER)
                {
                    // FIXED: Redirect playing instruments to use synthesis instead of samples
                    wavetableSynth->noteOn(0, midiNote, freq);
                }
                else if (waveformType == WAVETABLE)
                {
                    wavetableSynth->noteOn(0, midiNote, freq);
                }
                else
                {
                    synth->noteOn(0, midiNote, freq);
                }
            }
        }
    }
    
    projectManager->logFileWriter->processLog_ChordScanner_Sequencer(false, notes, noteFreqs);
    
}

void ChordScannerProcessor::triggerNoteOff(KEYNOTES keynote, CHORD_TYPES chordType, int octave)
{
    chordManager->setChordType(chordType);
    chordManager->setKeyNote(keynote);
    
    Array<int> notes = chordManager->getMIDIKeysForChord();

    for (int i = 0; i < notes.size(); i++)
    {
        int midiNote = notes.getReference(i);

        if (midiNote >= 0 && midiNote <= 127)
        {

            if (waveformType == SAMPLER)
            {
                // FIXED: Redirect playing instruments to use synthesis instead of samples
                wavetableSynth->noteOff(0, midiNote, 1.0, true);
            }
            else if (waveformType == WAVETABLE)
            {
                wavetableSynth->noteOff(0, midiNote, 1.0, true);
            }
            else
            {
                synth->noteOff(0, midiNote, 1.0, true);
            }
        }
    }
}

void ChordScannerProcessor::triggerNoteOnDirect(KEYNOTES keynote, int octave)
{
    int midiNote = keynote;
    
    if (midiNote >= 0 && midiNote <= 127)
    {
        double baseFreq = frequencyManager->scalesManager->getFrequencyForMIDINote(midiNote);
        double freq = baseFreq * pow(2.0, octave);
        
        if (waveformType == SAMPLER)
        {
            // FIXED: Redirect playing instruments to use synthesis instead of samples
            wavetableSynth->noteOn(0, midiNote, freq);
        }
        else if (waveformType == WAVETABLE)
        {
            wavetableSynth->noteOn(0, midiNote, freq);
        }
        else
        {
            synth->noteOn(0, midiNote, freq);
        }
    }
    
}

void ChordScannerProcessor::triggerNoteOffDirect(KEYNOTES keynote, int octave)
{
    int midiNote = keynote;// check this ******

    if (midiNote >= 0 && midiNote <= 127)
    {

        if (waveformType == SAMPLER)
        {
            sampler->noteOff(0, midiNote, 1.0, true);
        }
        else if (waveformType == WAVETABLE)
        {
            wavetableSynth->noteOff(0, midiNote, 1.0, true);
        }
        else
        {
            synth->noteOff(0, midiNote, 1.0, true);
        }
    }
}



void ChordScannerProcessor::panic()
{
    synth    ->allNotesOff(0, true);
    sampler  ->allNotesOff(0, true);
    wavetableSynth->allNotesOff(0, true);
}

void ChordScannerProcessor::setActiveShortcutSynth(int synthRef, bool shouldBeActive)
{
    isActive = shouldBeActive;
}

void ChordScannerProcessor::setPlayerCommand(PLAYER_COMMANDS command)
{
    switch (command)
    {
        case COMMAND_PLAYER_PLAYPAUSE:
        {
            repeater->play();
        }
            break;
            
        case COMMAND_PLAYER_STOP:
        {
            playState = PLAY_STATE::OFF;
            
            repeater->stop();
        }
            break;
            
        case COMMAND_PLAYER_RECORD:
        {
            shouldRecord = !shouldRecord;
        }
            break;
            
        case COMMAND_PLAYER_PANIC:
        {
            panic();
        }
            break;
            
            
        default:
            break;
    }
    
}

void ChordScannerProcessor::setPlayerPlayMode(PLAY_MODE mode)
{
    playMode = mode; repeater->setPlayMode(mode);
}

//=================================================================
// Play Repeater Embedded Class
//=================================================================
ChordScannerProcessor::PlayRepeater::PlayRepeater(ChordScannerProcessor * cp, double sr)
{
    sampleRate              = sr;
    proc                    = cp;
    sampleCounter           = 0;
    numSamplesPerMS         = sampleRate / 1000;
    playMode                = PLAY_MODE::NORMAL;
    playState               = PLAY_STATE::OFF;
    currentMS               = 0;
    totalMSOfLoop           = 0;
    playSimultaneous        = false;
    
    
    isActive                = true; // only one shortcut so always true for ChordScanner
    totalNumRepeats         = 0;
    lengthMS                = 0;
    lengthInSamples         = 0;
    pauseMS                 = 0;
    pauseInSamples          = 0;
    nextNoteOnEvent         = 0;
    nextNoteOffEvent        = 0;
    currentRepeat           = 0;
    isPaused                = false;
    numRepeats              = 0;
    numRepeatIterator       = 0;
    
    keynoteFrom             = KEY_C;
    keynoteTo               = KEY_B;
    octaveFrom              = 0;
    octaveTo                = 10;
    frequencyFrom           = FREQUENCY_MIN;
    frequencyTo             = FREQUENCY_MAX;
    
    currentPlayingChord     = chordTypeIterator;
    
}

ChordScannerProcessor::PlayRepeater::~PlayRepeater()
{
    
}

void ChordScannerProcessor::PlayRepeater::prepareToPlay(double newSampleRate)
{
    shouldProcess = false;
    
    sampleRate = newSampleRate;
    
    float oneMSInSamples = sampleRate / 1000;
    
    lengthInSamples = (int)lengthMS * oneMSInSamples;
    pauseInSamples  = (int)pauseMS  * oneMSInSamples;
    
    calculatePlaybackTimers();
    
    shouldProcess = true;
}

//=================================================================
// Setters and Getters
//=================================================================

void ChordScannerProcessor::PlayRepeater::setIsActive(bool should)
{
    isActive = should;
    
    calculatePlaybackTimers();
}

void ChordScannerProcessor::PlayRepeater::calculateLengths()
{
    float oneMSInSamples = sampleRate / 1000;
    
    lengthInSamples = (int)lengthMS * oneMSInSamples;
    pauseInSamples  = (int)pauseMS  * oneMSInSamples;
    
    calculatePlaybackTimers();
}

void ChordScannerProcessor::PlayRepeater::setNumRepeats(int num)
{
    numRepeats = num; calculateLengths();
}

void ChordScannerProcessor::PlayRepeater::setPauseMS(int ms)
{
    pauseMS = ms; calculateLengths();
}

void ChordScannerProcessor::PlayRepeater::setLengthMS(int ms)
{
    lengthMS = ms; calculateLengths();
}

void ChordScannerProcessor::PlayRepeater::setPlayMode(PLAY_MODE mode)
{
    playMode = mode;
}

void ChordScannerProcessor::PlayRepeater::setPlaySimultaneous(bool should)
{
    playSimultaneous = should;
    resetTick();
}

int ChordScannerProcessor::PlayRepeater::getTotalMSOfLoop()
{
    return totalMSOfLoop;
}

int ChordScannerProcessor::PlayRepeater::getCurrentMSInLoop()
{
    return currentMS;
}

float ChordScannerProcessor::PlayRepeater::getProgressBarValue()
{
    if (totalMSOfLoop == 0)
    {
        return 0;
    }
    else
    {
        return (float) currentMS / totalMSOfLoop;
    }
}

String ChordScannerProcessor::PlayRepeater::getTimeRemainingInSecondsString()
{
    String timeRemainingString("Playing (");
    
    int totalMSRemaining    = totalMSOfLoop - currentMS;
    int totalSeconds        = (int)totalMSRemaining / 1000;
    int minutes             = (int)totalSeconds / 60;
    int secs                = (int)totalSeconds % 60;
    
    String m(String::formatted("%.2i", minutes));
    String mins(m); timeRemainingString.append(mins, 10); timeRemainingString.append(":",2);
    
    String c(String::formatted("%.2i", secs));  String secon(c); timeRemainingString.append(secon, 2);
    
    String close(")"); timeRemainingString.append(close, 1);
    
    return timeRemainingString;
}



//=================================================================
// Triggers & Event Management
//=================================================================
void ChordScannerProcessor::PlayRepeater::triggerOnEvent(KEYNOTES keynote, CHORD_TYPES chordType, int octave)
{
    proc->triggerNoteOn(keynote, chordType, octave);
}

void ChordScannerProcessor::PlayRepeater::triggerOffEvent(KEYNOTES keynote, CHORD_TYPES chordType, int octave)
{
    proc->triggerNoteOff(keynote, chordType, octave);
}

void ChordScannerProcessor::PlayRepeater::allNotesOff()
{
    proc->panic();
}

void ChordScannerProcessor::PlayRepeater::clearOpenRepeats()
{
    currentRepeat           = 0;
    currentPlayingShortcut  = 0;
    nextNoteOnEvent         = 0;
    
}

//=================================================================
// Transport Start / Stop etc
//=================================================================
void ChordScannerProcessor::PlayRepeater::resetTick()
{
    calculatePlaybackTimers();
    
    sampleCounter           = 0;
    currentPlayingShortcut  = 0;
    
    clearOpenRepeats();
}

void ChordScannerProcessor::PlayRepeater::play()
{
    if (proc->playState == PLAY_STATE::PLAYING)
    {
        shouldProcess = false;
        
        proc->playState = PLAY_STATE::PAUSED;
        
    }
    else if (proc->playState == PLAY_STATE::PAUSED)
    {
        shouldProcess = true;
        
        proc->playState = PLAY_STATE::PLAYING;
    }
    else
    {
        proc->projectManager->logFileWriter->processLog_ChordScanner_Parameters();
        
        clearOpenRepeats();
        
        calculatePlaybackTimers();
        
        shouldProcess = true;
        sampleCounter = 0;
        
        proc->playState = PLAY_STATE::PLAYING;
        

        
    }

}

void ChordScannerProcessor::PlayRepeater::stop()
{
    shouldProcess   = false;
    
    allNotesOff();
    
    proc->playState = PLAY_STATE::OFF;
    
    resetTick();
}
//=================================================================
// Calculators
//=================================================================
void ChordScannerProcessor::PlayRepeater::calculatePlaybackTimers()
{
    totalNumSamplesOfLoop       = 0;
    totalNumRepeats             = 0;
    
    octaveIterator              = 0;
    keynoteIterator             = KEY_C;
    chordTypeIterator           = Major;
    
    numRepeatIterator           = 0;
    
    currentPlayingChord         = chordTypeIterator;

    // each mode uses an unique algorithm so needs to caluclate playback length independently
    
    switch (scanMode)
    {
        case SCAN_MAIN_CHORDS:
        {
            for (int repeat = 0; repeat < numRepeats; repeat++)
            {
                for (int keynote = KEY_C; keynote < TOTAL_NUM_KEYNOTES; keynote++)
                {
                    for (int chordType = Major; chordType < NUM_CHORD_TYPES; chordType++)
                    {
                        for (int octave = 0; octave <= maxOctaves; octave++)
                        {
                            totalNumSamplesOfLoop += lengthInSamples + pauseInSamples;

                            totalNumRepeats++;
                        }
                    }
                }
            }

        }
            break;
            
        case SCAN_ALL_CHORDS:
        {
            for (int repeat = 0; repeat < numRepeats; repeat++)
            {
                for (int octave = octaveFrom; octave <= octaveTo; octave++)
                {
                    for (int keynote = KEY_C; keynote < TOTAL_NUM_KEYNOTES; keynote++)
                    {
                        for (int numNotes = MinNumNotes; numNotes < MaxNumNotes; numNotes++)
                        {
                            for (int keynote2 = keynote; keynote2 < TOTAL_NUM_KEYNOTES; keynote2++)
                            {

                                totalNumSamplesOfLoop += lengthInSamples + pauseInSamples;
                                totalNumRepeats++;
                            }
                        }
                    }
                }
            }

            
            numNotesIterator                = 2;
            octaveIterator                  = octaveFrom;

            keynoteIteratorAllChords[0]     = 0;
            keynoteIteratorAllChords[1]     = 1;
            keynoteIteratorAllChords[2]     = 2;
            keynoteIteratorAllChords[3]     = 3;
            keynoteIteratorAllChords[4]     = 4;
            keynoteIteratorAllChords[5]     = 5;
            keynoteIteratorAllChords[6]     = 6;
            keynoteIteratorAllChords[7]     = 7;
            keynoteIteratorAllChords[8]     = 8;
            keynoteIteratorAllChords[9]     = 9;
            keynoteIteratorAllChords[10]    = 10;
            keynoteIteratorAllChords[11]    = 11;

        }
            break;
            
        case SCAN_SPECIFIC_RANGE:
        {

            octaveIterator  = octaveFrom;
            keynoteIterator = keynoteFrom;
            
            for (int repeat = 0; repeat < numRepeats; repeat++)
            {
                for (int octave = octaveFrom; octave <= octaveTo; octave++)
                {
                    for (int keynote = keynoteFrom; keynote <= keynoteTo; keynote++)
                    {
                        for (int chordType = Major; chordType < NUM_CHORD_TYPES; chordType++)
                        {
                            totalNumSamplesOfLoop += lengthInSamples + pauseInSamples;
                            totalNumRepeats++;
                        }
                    }
                }
            }

        }
            break;
            
        case SCAN_BY_FREQUENCY:
        {
            // take min frequency, find corresponding base note,
            
            // need to acquire keynote/octave of lowest chord from frequency / chord manager...

            int octaveFromRef   = 0;
            int octaveToRef     = 0;
            int keynoteToRef    = 0;
            int keynoteFromRef  = 0;
            int midiNoteFromRef = 0;
            int midiNoteToRef   = 0;
            
            proc->chordManager->getKeynoteOctaveForLowestFrequency(frequencyFrom, keynoteFromRef, octaveFromRef, midiNoteFromRef);
            proc->chordManager->getKeynoteOctaveForHighestFrequency(frequencyTo, keynoteToRef, octaveToRef, midiNoteToRef);
            
            octaveFrom      = octaveFromRef;
            octaveTo        = octaveToRef;
            
            keynoteTo       = keynoteToRef;
            keynoteFrom     = keynoteFromRef;
            
            octaveIterator  = octaveFrom;
            keynoteIterator = keynoteFrom;
            
            // needs to take into account stunted start, and early end on the keynotes !
            int numKeynotesToScan   = (midiNoteToRef - midiNoteFromRef) + 1;
            totalNumSamplesOfLoop   = (lengthInSamples + pauseInSamples) * (numKeynotesToScan * (NUM_CHORD_TYPES - 1)) * numRepeats;
            totalNumRepeats         = (numKeynotesToScan * (NUM_CHORD_TYPES-1)) * numRepeats;

        }
            break;
            
        default:  break;
    }

    totalMSOfLoop           = totalNumSamplesOfLoop / sampleRate * 1000;
    
    // NUM_REPEATS : simply multiply by numRepeats.. dont forget totalNumSamplesOfLoop
    
}

//=================================================================
// Tick & event sequencing
//=================================================================
void ChordScannerProcessor::PlayRepeater::processSecondsClock()
{
    currentMS = sampleCounter / ( sampleRate / 1000) ;
}

void ChordScannerProcessor::PlayRepeater::tickBuffer(int numSamples)
{
    if (shouldProcess)
    {
        for (int i = 0; i < numSamples; i++)
        {
            processSecondsClock();
            
            switch (scanMode)
            {
                case SCAN_MAIN_CHORDS: {    process_MainChords(sampleCounter); }        break;
                case SCAN_ALL_CHORDS: {     process_AllChords(sampleCounter); }         break;
                case SCAN_SPECIFIC_RANGE: { process_SpecificRange(sampleCounter); }     break;
                case SCAN_BY_FREQUENCY: {   process_ByFrequency(sampleCounter); }       break;
                    
                default: break;
            }
            
            sampleCounter++;
            
            if (playMode == PLAY_MODE::NORMAL)
            {
                if (sampleCounter >= (totalNumSamplesOfLoop))
                    stop();
            }
            else if (playMode == PLAY_MODE::LOOP)
            {
                if (sampleCounter >= totalNumSamplesOfLoop)
                    resetTick();
            }
        }
    }
}

void ChordScannerProcessor::PlayRepeater::process_MainChords(int sampleRef)
{
    if (sampleRef == nextNoteOnEvent)
    {
        
        triggerOnEvent((KEYNOTES)keynoteIterator,
                       (CHORD_TYPES)chordTypeIterator,
                       octaveIterator);
        
        nextNoteOffEvent = sampleRef + lengthInSamples;
        
        currentRepeat++;
        
        currentPlayingChord = chordTypeIterator;
        
    }
    else if (sampleRef == nextNoteOffEvent)
    {
        triggerOffEvent((KEYNOTES)keynoteIterator,
                        (CHORD_TYPES)chordTypeIterator,
                        octaveIterator);
        
        
        // set next note on event after paus
        if (currentRepeat < totalNumRepeats)
        {
            nextNoteOnEvent = sampleRef + pauseInSamples;
        }
        else
        {
            stop();
        }

        // iterators..
        
        if (numRepeatIterator >= (numRepeats-1))
        {
            numRepeatIterator = 0;
            
            chordTypeIterator++;
            
            if (chordTypeIterator >= (NUM_CHORD_TYPES))
            {
                chordTypeIterator = Major;
                keynoteIterator++;
            }
            
            if (keynoteIterator >= (TOTAL_NUM_KEYNOTES))
            {
                keynoteIterator = KEY_C;
                octaveIterator++;
            }

            
            if (octaveIterator > maxOctaves)
            {
                stop();
            }
        }
        else
        {
            numRepeatIterator++;
        }
    }
}

void ChordScannerProcessor::PlayRepeater::process_AllChords(int sampleRef)
{
    if (sampleRef == nextNoteOnEvent)
    {
        
        Array<int> notes;
        Array<float> noteFreqs;
        
        for (int numNotes = 0; numNotes < numNotesIterator; numNotes++)
        {
            proc->triggerNoteOnDirect((KEYNOTES) keynoteIteratorAllChords[numNotes], octaveIterator);
            
            notes.add(keynoteIteratorAllChords[numNotes]);
            
            double baseFreq = proc->frequencyManager->scalesManager->getFrequencyForMIDINote(keynoteIteratorAllChords[numNotes]);
            double freq = baseFreq * pow(2.0, octaveIterator);
            
            
            noteFreqs.add(freq);
        }
        
        // get all notes above..
        
        // get frequencies..
        
        // call log writer
        proc->projectManager->logFileWriter->processLog_ChordScanner_Sequencer(true, notes, noteFreqs);

        nextNoteOffEvent = sampleRef + lengthInSamples;
        
        currentRepeat++;

    }
    else if (sampleRef == nextNoteOffEvent)
    {
        for (int numNotes = 0; numNotes < numNotesIterator; numNotes++)
        {
            proc->triggerNoteOffDirect((KEYNOTES) keynoteIteratorAllChords[numNotes], octaveIterator);
        }
        
        
        // set next note on event after paus
        if (currentRepeat < totalNumRepeats)
        {
            nextNoteOnEvent = sampleRef + pauseInSamples;
        }
        else
        {
            stop();
        }
        
        if (numRepeatIterator >= (numRepeats-1))
        {
            numRepeatIterator = 0;
            
            for (int i = 1; i < 12; i++)
            {
                if (i == (numNotesIterator - 1))
                {
                    if (keynoteIteratorAllChords[i] < (keynoteIteratorAllChords[0] + 10))
                    {
                        keynoteIteratorAllChords[i]++;
                    }
                    else
                    {
                        if (numNotesIterator < (MaxNumNotes))
                        {
                            numNotesIterator++;
                            
                            keynoteIteratorAllChords[i]     = keynoteIteratorAllChords[i - 1] + 1;
                            keynoteIteratorAllChords[i+1]   = keynoteIteratorAllChords[i];
                        }
                        else
                        {
                            numNotesIterator = MinNumNotes;
                            
                            keynoteIteratorAllChords[0]++;
                            
                            if (keynoteIteratorAllChords[0] > 11)
                            {
                                octaveIterator++;
                                
                                keynoteIteratorAllChords[0]     = 0;
                                keynoteIteratorAllChords[1]     = 1;
                                keynoteIteratorAllChords[2]     = 2;
                                keynoteIteratorAllChords[3]     = 3;
                                keynoteIteratorAllChords[4]     = 4;
                                keynoteIteratorAllChords[5]     = 5;
                                keynoteIteratorAllChords[6]     = 6;
                                keynoteIteratorAllChords[7]     = 7;
                                keynoteIteratorAllChords[8]     = 8;
                                keynoteIteratorAllChords[9]     = 9;
                                keynoteIteratorAllChords[10]    = 10;
                                keynoteIteratorAllChords[11]    = 11;
                                
                                if (octaveIterator > octaveTo || octaveIterator > maxOctaves)
                                {
                                    stop();
                                }

                            }
                        }
                    }
                }
            }
        }
        else
        {
            numRepeatIterator++;
        }
    }
}

void ChordScannerProcessor::PlayRepeater::process_SpecificRange(int sampleRef)
{
    if (sampleRef == nextNoteOnEvent)
    {
        
        triggerOnEvent((KEYNOTES)keynoteIterator,
                       (CHORD_TYPES)chordTypeIterator,
                       octaveIterator);
        
        nextNoteOffEvent = sampleRef + lengthInSamples;
        
        currentRepeat++;
        
        currentPlayingChord = chordTypeIterator;
        
    }
    else if (sampleRef == nextNoteOffEvent)
    {
        triggerOffEvent((KEYNOTES)keynoteIterator,
                        (CHORD_TYPES)chordTypeIterator,
                        octaveIterator);
        
        
        // set next note on event after paus
        if (currentRepeat < totalNumRepeats)
        {
            nextNoteOnEvent = sampleRef + pauseInSamples;
        }
        else
        {
            stop();
        }
        
        
        if (numRepeatIterator >= (numRepeats-1))
        {
            numRepeatIterator = 0;
            
            // iterators..
            chordTypeIterator++;
            
            if (chordTypeIterator >= (NUM_CHORD_TYPES))
            {
                chordTypeIterator = Major;
                keynoteIterator++;
            }
            
            if (keynoteIterator > keynoteTo)
            {
                keynoteIterator = keynoteFrom;
                octaveIterator++;
            }
            
            if (octaveIterator > maxOctaves || octaveIterator > octaveTo)
            {
                stop();
            }
        }
        else
        {
            numRepeatIterator++;
        }
        
        
    }
    
}

void ChordScannerProcessor::PlayRepeater::process_ByFrequency(int sampleRef)
{
    if (sampleRef == nextNoteOnEvent)
    {
        
        triggerOnEvent((KEYNOTES)keynoteIterator,
                       (CHORD_TYPES)chordTypeIterator,
                       octaveIterator);
        
        nextNoteOffEvent = sampleRef + lengthInSamples;
        
        currentRepeat++;
        
        currentPlayingChord = chordTypeIterator;
        
    }
    else if (sampleRef == nextNoteOffEvent)
    {
        triggerOffEvent((KEYNOTES)keynoteIterator,
                        (CHORD_TYPES)chordTypeIterator,
                        octaveIterator);
        
        
        // set next note on event after paus
        if (currentRepeat < totalNumRepeats)
        {
            nextNoteOnEvent = sampleRef + pauseInSamples;
        }
        else
        {
            stop();
        }
        
        if (numRepeatIterator >= (numRepeats-1))
        {
            numRepeatIterator = 0;
            
            // iterators..
            chordTypeIterator++;
            
            if (chordTypeIterator >= (NUM_CHORD_TYPES))
            {
                chordTypeIterator = Major;
                keynoteIterator++;
            }
            
            if (keynoteIterator >= (TOTAL_NUM_KEYNOTES))
            {
                keynoteIterator = KEY_C;
                octaveIterator++;
            }
            
            if (octaveIterator > maxOctaves || octaveIterator > octaveTo)
            {
                stop();
            }
        }
        else
        {
            numRepeatIterator++;
        }
        
    }
}





void ChordScannerProcessor::PlayRepeater::processShortcut(int shortcutRef, int sampleRef) { }
void ChordScannerProcessor::PlayRepeater::prepareNextShortcut() { }
void ChordScannerProcessor::PlayRepeater::processSimultaneousShortcuts(int firstShortcutRef, int sampleRef) { }
