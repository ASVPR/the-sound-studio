/*
  ==============================================================================

    LissajousCurveProcessor.cpp
    Created: 1 Oct 2019 9:11:57am
    Author:  Gary Jones

  ==============================================================================
*/

#include "LissajousCurveProcessor.h"
#include "ProjectManager.h"

LissajousFrequencyPlayerProcessor::LissajousFrequencyPlayerProcessor(FrequencyManager * fm, SampleLibraryManager * slm)
{
    sampleLibraryManager    = slm;
    frequencyManager        = fm;
    
    synth                   = new FrequencySynthProcessor(44100, frequencyManager);
    synth                   ->setNoteStealingEnabled(true);
    synth                   ->setCurrentPlaybackSampleRate(44100);

    wavetableSynth          = new FrequencyPlayerWavetableSynthProcessor(44100, frequencyManager);
    wavetableSynth          ->setNoteStealingEnabled(true);
    wavetableSynth          ->setCurrentPlaybackSampleRate(44100);
    
    waveformType = DEFAULT;
    
    shouldMute = false;

    repeater = new PlayRepeater(this, 44100);
}

LissajousFrequencyPlayerProcessor::~LissajousFrequencyPlayerProcessor() {}

void LissajousFrequencyPlayerProcessor::prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock)
{

    synth           ->prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);
    wavetableSynth  ->prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);
    
    repeater->prepareToPlay(sampleRate);
}

void LissajousFrequencyPlayerProcessor::setParameter(int synthRef, int index, var newValue)
{
    if (index == FREQUENCY_PLAYER_WAVEFORM_TYPE)
    {
        panic();
        
        waveformType = (WAVEFORM_TYPES)newValue.operator int();
        
        if (waveformType != WAVETABLE)
        {
            synth           ->setParameter(index, newValue);
        }
    }
    else if (index >= FREQUENCY_PLAYER_FREQ_SOURCE && index <= FREQUENCY_PLAYER_RELEASE)
    {
        synth->setParameter(index, newValue);
        wavetableSynth->setParameter(index, newValue);
    }
    else if (index == FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE)
    {
        isActive = newValue.operator bool();
        repeater->setIsActive(synthRef, isActive);
    }
    else if (index == FREQUENCY_PLAYER_SHORTCUT_MUTE)
    {
        shouldMute = newValue.operator bool();
    }
    else if (index == FREQUENCY_PLAYER_NUM_REPEATS)
    {
        repeater->setNumRepeats(synthRef, newValue.operator int());
    }
    else if (index == FREQUENCY_PLAYER_NUM_PAUSE)
    {
        repeater->setPauseMS(synthRef, newValue.operator int());
    }
    else if (index == FREQUENCY_PLAYER_NUM_DURATION)
    {
        repeater->setLengthMS(synthRef, newValue.operator int());
    }
}


void LissajousFrequencyPlayerProcessor::processBlock (AudioBuffer<float>& buffer,
                                         MidiBuffer& midiMessages)
{
    // process repeater first
    if (playState == PLAY_STATE::PLAYING)
    {
        // run repeater looper..
        repeater->tickBuffer(buffer.getNumSamples());
    }
    

    if (isActive && !shouldMute)
    {
        if (waveformType == WAVETABLE)
        {
            wavetableSynth->processBlock(buffer, midiMessages);
        }
        else
        {
            synth->processBlock(buffer, midiMessages);
        }
    }
}

// trigger commands
// temp functions, called when shortcut keys are pressed
// can be evolved to trigger the repeater later, or called by by repeater
void LissajousFrequencyPlayerProcessor::triggerKeyDown()
{
    triggerNoteOn();
}

void LissajousFrequencyPlayerProcessor::triggerKeyUp()
{
    triggerNoteOff();
}

void LissajousFrequencyPlayerProcessor::triggerNoteOn()
{
    float freq = 0;
    if (waveformType == WAVETABLE)
    {
        wavetableSynth->noteOn(0, 0, 1.0);
        
        freq = wavetableSynth->getChosenFrequency();
    }
    else
    {
        synth->noteOn(0, 0, 1.0);
        
        freq = synth->getChosenFrequency();
    }
    
    
//    projectManager->logFileWriter->processLog_FrequencyPlayer_Sequencer(shortcutRef, freq);
    
    
}

void LissajousFrequencyPlayerProcessor::triggerNoteOff()
{
    if (waveformType == WAVETABLE)
    {
        wavetableSynth->noteOff(0, 0, 0.0, true);
    }
    else
    {
        synth->noteOff(0, 0, 0.0, true);
    }
    
}

void LissajousFrequencyPlayerProcessor::panic()
{
    synth               ->allNotesOff(0, true);
    wavetableSynth      ->allNotesOff(0, true);
}

void LissajousFrequencyPlayerProcessor::setPlayerCommand(PLAYER_COMMANDS command, bool freeFlow)
{
    switch (command)
    {
        case COMMAND_PLAYER_PLAYPAUSE:
        {
            if (!freeFlow)
            {
                if (playState == PLAY_STATE::PLAYING)
                {
                    repeater->stop();
                }
                else
                {
                    triggerNoteOn();
                }
            }
            else
            {
                repeater->play();
            }
        }
            break;
            
        case COMMAND_PLAYER_STOP:
        {
            playState = PLAY_STATE::OFF;
            
            if (!freeFlow)
            {
                triggerNoteOff();
            }
            else
            {
                repeater->stop();
            }
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

void LissajousFrequencyPlayerProcessor::setPlayerPlayMode(PLAY_MODE mode)
{
    playMode = mode;
    
    repeater->setPlayMode(mode);
}

// Play Repeater Embedded class

LissajousFrequencyPlayerProcessor::PlayRepeater::PlayRepeater(LissajousFrequencyPlayerProcessor * cp, double sr)
{
    sampleRate                  = sr;
    proc                        = cp;
    sampleCounter               = 0;
    numSamplesPerMS             = sampleRate / 1000;
    playMode                    = PLAY_MODE::NORMAL;
    playState                   = PLAY_STATE::OFF;
    currentMS                   = 0;
    totalMSOfLoop               = 0;
    playSimultaneous            = false;

    isActive                    = false;
    numRepeats                  = 0;
    lengthMS                    = 0;
    lengthInSamples             = 0;
    pauseMS                     = 0;
    pauseInSamples              = 0;
    nextNoteOnEvent             = 0;
    nextNoteOffEvent            = 0;
    currentRepeat               = 0;
    isPaused                    = false;
}

LissajousFrequencyPlayerProcessor::PlayRepeater::~PlayRepeater()
{
    
}

void LissajousFrequencyPlayerProcessor::PlayRepeater::prepareToPlay(double newSampleRate)
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

void LissajousFrequencyPlayerProcessor::PlayRepeater::setIsActive(int shortcutRef, bool should)
{
    isActive = should;
    
    calculatePlaybackTimers();
}

void LissajousFrequencyPlayerProcessor::PlayRepeater::calculateLengths(int shortcutRef)
{
    float oneMSInSamples = sampleRate / 1000;
    
    lengthInSamples = (int)lengthMS * oneMSInSamples;
    pauseInSamples  = (int)pauseMS  * oneMSInSamples;
    
    calculatePlaybackTimers();
}

void LissajousFrequencyPlayerProcessor::PlayRepeater::setNumRepeats(int shortcutRef, int num)
{
    numRepeats = num; calculateLengths(shortcutRef);
}

void LissajousFrequencyPlayerProcessor::PlayRepeater::setPauseMS(int shortcutRef, int ms)
{
    pauseMS = ms; calculateLengths(shortcutRef);
}

void LissajousFrequencyPlayerProcessor::PlayRepeater::setLengthMS(int shortcutRef, int ms)
{
    lengthMS = ms; calculateLengths(shortcutRef);
}

void LissajousFrequencyPlayerProcessor::PlayRepeater::setPlayMode(PLAY_MODE mode)
{
    playMode = mode;
}

void LissajousFrequencyPlayerProcessor::PlayRepeater::setPlaySimultaneous(bool should)
{
    playSimultaneous = should;
    resetTick();
}

int LissajousFrequencyPlayerProcessor::PlayRepeater::getTotalMSOfLoop()
{
    return totalMSOfLoop;
}

int LissajousFrequencyPlayerProcessor::PlayRepeater::getCurrentMSInLoop()
{
    return currentMS;
}

float LissajousFrequencyPlayerProcessor::PlayRepeater::getProgressBarValue()
{
    if (totalMSOfLoop == 0)
    {
        return 0;
    }
    else
    {
        return (float)currentMS / totalMSOfLoop;
    }
}

String LissajousFrequencyPlayerProcessor::PlayRepeater::getTimeRemainingInSecondsString()
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
void LissajousFrequencyPlayerProcessor::PlayRepeater::triggerOnEvent(int shortcutRef)
{
    proc->triggerNoteOn();
}

void LissajousFrequencyPlayerProcessor::PlayRepeater::triggerOffEvent(int shortcutRef)
{
    proc->triggerNoteOff();
}

void LissajousFrequencyPlayerProcessor::PlayRepeater::allNotesOff()
{
    proc->panic();
}

void LissajousFrequencyPlayerProcessor::PlayRepeater::clearOpenRepeats()
{
    currentRepeat        = 0;
    currentPlayingShortcut  = 0;
    nextNoteOnEvent      = 0;

}

//=================================================================
// Transport Start / Stop etc
//=================================================================
void LissajousFrequencyPlayerProcessor::PlayRepeater::resetTick()
{
    calculatePlaybackTimers();
    
    sampleCounter           = 0;
    
    clearOpenRepeats();
    
//    if (playSimultaneous)
//    {
//        currentPlayingShortcut  = 0;
//    }
//    else
//    {
//        currentPlayingShortcut  = 0;
//
//        for (int c = currentPlayingShortcut; c < NUM_SHORTCUT_SYNTHS; c++)
//        {
//            if (isActive[c] && !proc->shouldMute[c])
//            {
//                currentPlayingShortcut = c; break;
//            }
//        }
//    }
}

void LissajousFrequencyPlayerProcessor::PlayRepeater::play()
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
        resetTick();
        
        shouldProcess = true;
        sampleCounter = 0;
        
        proc->playState = PLAY_STATE::PLAYING;
    }
}

void LissajousFrequencyPlayerProcessor::PlayRepeater::stop()
{
    
    shouldProcess   = false;
    
    allNotesOff();
    
    proc->playState = PLAY_STATE::OFF;
}
//=================================================================
// Calculators
//=================================================================
void LissajousFrequencyPlayerProcessor::PlayRepeater::calculatePlaybackTimers()
{
    totalNumSamplesOfLoop       = 0;
    
    // first check for shoudPlays,
    // find longest shouldPlay Loop and add to val
    if (playSimultaneous)
    {
        int shortcutValInSamps  = 0;
        
        // simply find the longest loop

        if (isActive & !proc->shouldMute)
        {
            shortcutValInSamps = numRepeats * (lengthInSamples + pauseInSamples);
        }
        
        if (shortcutValInSamps > totalNumSamplesOfLoop)
        {
            totalNumSamplesOfLoop = shortcutValInSamps;
        }
    }
    else
    {
        if (isActive && !proc->shouldMute)
        {
            int shortcutValInSamps = numRepeats * (lengthInSamples + pauseInSamples);
            
            totalNumSamplesOfLoop += shortcutValInSamps;
        }
    }
    
    totalMSOfLoop           = totalNumSamplesOfLoop / sampleRate * 1000;
    
//    printf("\n Total samps of sequence = %i", totalNumSamplesOfLoop);
//    printf("\n Total MS of sequence = %i", totalMSOfLoop);
    
}

//=================================================================
// Tick & event sequencing
//=================================================================
void LissajousFrequencyPlayerProcessor::PlayRepeater::processSecondsClock()
{
    currentMS = sampleCounter / ( sampleRate / 1000) ;
}

void LissajousFrequencyPlayerProcessor::PlayRepeater::tickBuffer(int numSamples)
{
    if (shouldProcess)
    {
        for (int i = 0; i < numSamples; i++)
        {
            processSecondsClock();
            
            // just sequence through active shortcuts
            if (isActive)
            {
                processShortcut(0, sampleCounter);
            }

            sampleCounter++;
            
            if (playMode == PLAY_MODE::NORMAL)
            {
                if (sampleCounter >= (totalNumSamplesOfLoop)) stop();
            }
            else if (playMode == PLAY_MODE::LOOP)
            {
                if (sampleCounter >= totalNumSamplesOfLoop) resetTick();
            }
        }
    }
}

void LissajousFrequencyPlayerProcessor::PlayRepeater::processShortcut(int shortcutRef, int sampleRef)
{
    if (sampleRef == nextNoteOnEvent)
    {
        triggerOnEvent(shortcutRef);
        
        nextNoteOffEvent = sampleRef + lengthInSamples;
        
        currentRepeat++;
    }
    else if (sampleRef == nextNoteOffEvent)
    {
        triggerOffEvent(shortcutRef);
        
        // set next note on event after paus
        if (currentRepeat < numRepeats)
        {
            nextNoteOnEvent = sampleRef + pauseInSamples;
        }
        else
        {
            if (isActive && !proc->shouldMute)
            {
                currentPlayingShortcut = 0;
//                prepareNextShortcut();
            }
        }
    }
}

void LissajousFrequencyPlayerProcessor::PlayRepeater::prepareNextShortcut()
{
//    nextNoteOnEvent[currentPlayingShortcut] = sampleCounter + pauseInSamples[currentPlayingShortcut-1];
}

void LissajousFrequencyPlayerProcessor::PlayRepeater::processSimultaneousShortcuts(int firstShortcutRef, int sampleRef)
{
//    for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
//    {
//        if (isActive[i])
//        {
//            if (sampleRef == nextNoteOnEvent[i])
//            {
//                triggerOnEvent(i);
//
//                nextNoteOffEvent[i] = sampleRef + lengthInSamples[i];
//
//                currentRepeat[i]++;
//            }
//            else if (sampleRef == nextNoteOffEvent[i])
//            {
//                triggerOffEvent(i);
//
//                if (currentRepeat[i] < numRepeats[i])
//                {
//                    nextNoteOnEvent[i] = sampleRef + pauseInSamples[i];
//                }
//            }
//        }
//    }
}


//LissajousChordPlayerProcessor::LissajousChordPlayerProcessor(FrequencyManager * fm, SampleLibraryManager * slm)
//{
//    frequencyManager        = fm;
//    sampleLibraryManager    = slm;
//
//
//    synth            = new ChordSynthProcessor(44100, frequencyManager);
//    synth            ->setNoteStealingEnabled(true);
//
//    sampler          = new SamplerProcessor(frequencyManager, sampleLibraryManager, 44100, 11);
//    sampler          ->setNoteStealingEnabled(true);
//
//    wavetableSynth   = new WavetableSynthProcessor(44100, frequencyManager);
//    wavetableSynth   ->setNoteStealingEnabled(true);
//
//    synth            ->setCurrentPlaybackSampleRate(44100);
//    sampler          ->setCurrentPlaybackSampleRate(44100);
//    wavetableSynth   ->setCurrentPlaybackSampleRate(44100);
//
//
//    chordManager     = new ChordManager(frequencyManager);
//    shouldMute       = false;
//    waveformType     = SAMPLER;
//
//
//    repeater = new PlayRepeater(this, 44100);
//}
//
//LissajousChordPlayerProcessor::~LissajousChordPlayerProcessor()
//{
//
//}
//
//void LissajousChordPlayerProcessor::prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock)
//{
//
//    synth    ->prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);
//    sampler  ->setCurrentPlaybackSampleRate(sampleRate);
//    wavetableSynth   ->prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);
//
//    repeater->prepareToPlay(sampleRate);
//}
//
//void LissajousChordPlayerProcessor::setParameter(int index, var newValue)
//{
//    if (index >= INSTRUMENT_TYPE && index <= DIVISION_VALUE)
//    {
//        synth     ->setParameter(index, newValue);
//        sampler   ->setParameter(index, newValue);
//        wavetableSynth->setParameter(index, newValue);
//    }
//    else if (index == WAVEFORM_TYPE)
//    {
//        panic();
//
//        waveformType = (WAVEFORM_TYPES)newValue.operator int();
//
//        synth->setParameter(index, newValue);
//    }
//    else if (index == SHORTCUT_IS_ACTIVE)
//    {
//        isActive = newValue.operator bool();
//        repeater->setIsActive(isActive);
//    }
//    else if (index == SHORTCUT_MUTE)
//    {
//        shouldMute = newValue.operator bool();
//    }
//    else if (index == NUM_REPEATS)
//    {
//        repeater->setNumRepeats(newValue.operator int());
//    }
//    else if (index == NUM_PAUSE)
//    {
//        repeater->setPauseMS(newValue.operator int());
//    }
//    else if (index == NUM_DURATION)
//    {
//        repeater->setLengthMS(newValue.operator int());
//    }
//    else if (index == SHORTCUT_PLAY_AT_SAME_TIME)
//    {
//        // should delete
//    }
//    else if (index == CHORD_SOURCE)
//    {
//        // not implemented yet
//    }
//    else if (index == KEYNOTE)
//    {
//        KEYNOTES keyNote = (KEYNOTES)newValue.operator int();
//
//        chordManager->setKeyNote(keyNote);
//    }
//    else if (index == CHORD_TYPE)
//    {
//        CHORD_TYPES type = (CHORD_TYPES)newValue.operator int();
//
//        chordManager->setChordType(type);
//    }
//    else if (index == ADD_ONS)
//    {
//        // needs implementing
//        enum ADD_ONS addOnType = (enum ADD_ONS)newValue.operator int();
//        chordManager->setAddOn(addOnType);
//    }
//    else if (index == CUSTOM_CHORD)
//    {
//        chordManager->setCustomChord(newValue.operator bool());
//    }
//    else if (index >= CUSTOM_CHORD_ACTIVE_1 && index <= CUSTOM_CHORD_ACTIVE_12)
//    {
//        int pos = index - CUSTOM_CHORD_ACTIVE_1;
//        chordManager->setActiveToCustomChord(pos, newValue.operator bool());
//    }
//    else if (index >= CUSTOM_CHORD_NOTE_1 && index <= CUSTOM_CHORD_NOTE_12)
//    {
//        int pos = index - CUSTOM_CHORD_NOTE_1;
//        chordManager->setNoteToCustomChord(pos, newValue.operator int());
//    }
//    else if (index >= CUSTOM_CHORD_OCTAVE_1 && index <= CUSTOM_CHORD_OCTAVE_12)
//    {
//        int pos = index - CUSTOM_CHORD_OCTAVE_1;
//        chordManager->setOctaveToCustomChord(pos, newValue.operator int());
//    }
//    else if (index == CHORDPLAYER_SCALE)
//    {
//        frequencyManager->setLissajousPlayerScale(newValue.operator int());
//    }
//}
//
//void LissajousChordPlayerProcessor::setOversamplingFactor(int newFactor)
//{
//    // ** NOT WORKING YET
//    suspendProcessing(true);
//
//
////        synth[i]->setOversamplingFactor(newFactor);
//
//
//    suspendProcessing(false);
//}
//
//void LissajousChordPlayerProcessor::processBlock (AudioBuffer<float>& buffer,
//                           MidiBuffer& midiMessages)
//{
//    if (playState == PLAY_STATE::PLAYING)
//    {
//        repeater->tickBuffer(buffer.getNumSamples());
//    }
//
//
//    if (isActive && !shouldMute)
//    {
//        if (waveformType == SAMPLER)
//        {
//            sampler->processBlock(buffer, midiMessages);
//        }
//        else if (waveformType == WAVETABLE)
//        {
//            wavetableSynth->processBlock(buffer, midiMessages);
//        }
//        else
//        {
//            synth->processBlock(buffer, midiMessages);
//        }
//    }
//}
//
//void LissajousChordPlayerProcessor::triggerNoteOn()
//{
//    // CHECK CHECK CHECK !!!!!
//
//    // get chord notes from parameters *** needs to refactor ChordManager to derive the chords specifically from Lissajous..
//
//    Array<int> notes(chordManager->getMIDIKeysForChord());
//
//    for (int i = 0; i < notes.size(); i++)
//    {
//        int midiNote = notes.getReference(i);
//
//        if (midiNote >= 0 && midiNote <= 127)
//        {
//            if (waveformType == SAMPLER)
//            {
//                sampler->noteOn(0, midiNote, 1.0);
//            }
//            else if (waveformType == WAVETABLE)
//            {
//                wavetableSynth->noteOn(0, midiNote, 1.0);
//            }
//            else
//            {
//                synth->noteOn(0, midiNote, 1.0);
//            }
//        }
//    }
//}
//
//void LissajousChordPlayerProcessor::triggerNoteOff()
//{
//    Array<int> notes = chordManager->getMIDIKeysForChord();
//
//    for (int i = 0; i < notes.size(); i++)
//    {
//        int midiNote = notes.getReference(i);
//
//        if (midiNote >= 0 && midiNote <= 127)
//        {
//            if (waveformType == SAMPLER)
//            {
//                sampler->noteOff(0, midiNote, 1.0, true);
//            }
//            else if (waveformType == WAVETABLE)
//            {
//                wavetableSynth->noteOff(0, midiNote, 1.0, true);
//            }
//            else
//            {
//                synth->noteOff(0, midiNote, 1.0, true);
//            }
//        }
//    }
//}
//
//void LissajousChordPlayerProcessor::panic()
//{
//    synth    ->allNotesOff(0, true);
//    sampler  ->allNotesOff(0, true);
//    wavetableSynth->allNotesOff(0, true);
//}
//
//void LissajousChordPlayerProcessor::setActiveShortcutSynth(bool shouldBeActive)
//{
//    isActive = shouldBeActive;
//}
//
//void LissajousChordPlayerProcessor::setPlayerCommand(PLAYER_COMMANDS command)
//{
//    switch (command)
//    {
//        case COMMAND_PLAYER_PLAYPAUSE:
//        {
//            playState = PLAY_STATE::PLAYING;
//
//            repeater->play();
//        }
//            break;
//
//        case COMMAND_PLAYER_STOP:
//        {
//            playState = PLAY_STATE::OFF;
//
//            repeater->stop();
//        }
//            break;
//
//        case COMMAND_PLAYER_RECORD:
//        {
//            shouldRecord = !shouldRecord;
//        }
//            break;
//
//        case COMMAND_PLAYER_PANIC:
//        {
//            panic();
//        }
//            break;
//
//
//        default:
//            break;
//    }
//
//}
//
//void LissajousChordPlayerProcessor::setPlayerPlayMode(PLAY_MODE mode)
//{
//    playMode = mode;
//
//    repeater->setPlayMode(mode);
//}
//
////=================================================================
//// Play Repeater Embedded Class
////=================================================================
//LissajousChordPlayerProcessor::PlayRepeater::PlayRepeater(LissajousChordPlayerProcessor * cp, double sr)
//{
//    sampleRate                  = sr;
//    proc                        = cp;
//    sampleCounter               = 0;
//    numSamplesPerMS             = sampleRate / 1000;
//    playMode                    = PLAY_MODE::NORMAL;
//    playState                   = PLAY_STATE::OFF;
//    currentMS                   = 0;
//    totalMSOfLoop               = 0;
//
//
//    isActive             = false;
//    numRepeats           = 0;
//    lengthMS             = 0;
//    lengthInSamples      = 0;
//    pauseMS              = 0;
//    pauseInSamples       = 0;
//    nextNoteOnEvent      = 0;
//    nextNoteOffEvent     = 0;
//    currentRepeat        = 0;
//    isPaused             = false;
//
//}
//
//LissajousChordPlayerProcessor::PlayRepeater::~PlayRepeater()
//{
//
//}
//
//void LissajousChordPlayerProcessor::PlayRepeater::prepareToPlay(double newSampleRate)
//{
//    shouldProcess = false;
//
//    sampleRate = newSampleRate;
//
//    float oneMSInSamples = sampleRate / 1000;
//
//    lengthInSamples = (int)lengthMS * oneMSInSamples;
//    pauseInSamples  = (int)pauseMS  * oneMSInSamples;
//
//    calculatePlaybackTimers();
//
//    shouldProcess = true;
//}
//
////=================================================================
//// Setters and Getters
////=================================================================
//
//void LissajousChordPlayerProcessor::PlayRepeater::setIsActive(bool should)
//{
//    isActive = should;
//
//    calculatePlaybackTimers();
//}
//
//void LissajousChordPlayerProcessor::PlayRepeater::calculateLengths()
//{
//    float oneMSInSamples = sampleRate / 1000;
//
//    lengthInSamples = (int)lengthMS * oneMSInSamples;
//    pauseInSamples  = (int)pauseMS  * oneMSInSamples;
//
//    calculatePlaybackTimers();
//}
//
//void LissajousChordPlayerProcessor::PlayRepeater::setNumRepeats(int num)
//{
//    numRepeats = num; calculateLengths();
//}
//
//void LissajousChordPlayerProcessor::PlayRepeater::setPauseMS(int ms)
//{
//    pauseMS = ms; calculateLengths();
//}
//
//void LissajousChordPlayerProcessor::PlayRepeater::setLengthMS(int ms)
//{
//    lengthMS = ms; calculateLengths();
//}
//
//void LissajousChordPlayerProcessor::PlayRepeater::setPlayMode(PLAY_MODE mode)
//{
//    playMode = mode;
//}
//
//int LissajousChordPlayerProcessor::PlayRepeater::getTotalMSOfLoop()
//{
//    return totalMSOfLoop;
//}
//
//int LissajousChordPlayerProcessor::PlayRepeater::getCurrentMSInLoop()
//{
//    return currentMS;
//}
//
//float LissajousChordPlayerProcessor::PlayRepeater::getProgressBarValue()
//{
//    if (totalMSOfLoop == 0)
//    {
//        return 0;
//    }
//    else
//    {
//        return (float)currentMS / totalMSOfLoop;
//    }
//}
//
//String LissajousChordPlayerProcessor::PlayRepeater::getTimeRemainingInSecondsString()
//{
//    String timeRemainingString("Playing (");
//
//    int totalMSRemaining    = totalMSOfLoop - currentMS;
//    int totalSeconds        = (int)totalMSRemaining / 1000;
//    int minutes             = (int)totalSeconds / 60;
//    int secs                = (int)totalSeconds % 60;
//
//    String m(String::formatted("%.2i", minutes));
//    String mins(m); timeRemainingString.append(mins, 10); timeRemainingString.append(":",2);
//
//    String c(String::formatted("%.2i", secs));  String secon(c); timeRemainingString.append(secon, 2);
//
//    String close(")"); timeRemainingString.append(close, 1);
//
//    return timeRemainingString;
//}
//
////=================================================================
//// Triggers & Event Management
////=================================================================
//void LissajousChordPlayerProcessor::PlayRepeater::triggerOnEvent()
//{
//    proc->triggerNoteOn();
//}
//
//void LissajousChordPlayerProcessor::PlayRepeater::triggerOffEvent()
//{
//    proc->triggerNoteOff();
//}
//
//void LissajousChordPlayerProcessor::PlayRepeater::allNotesOff()
//{
//    proc->panic();
//}
//
//void LissajousChordPlayerProcessor::PlayRepeater::clearOpenRepeats()
//{
//    currentRepeat        = 0;
//    nextNoteOnEvent      = 0;
//}
//
////=================================================================
//// Transport Start / Stop etc
////=================================================================
//void LissajousChordPlayerProcessor::PlayRepeater::resetTick()
//{
//    calculatePlaybackTimers();
//
//    sampleCounter           = 0;
//
//    clearOpenRepeats();
//}
//
//void LissajousChordPlayerProcessor::PlayRepeater::play()
//{
//    clearOpenRepeats();
//
//    calculatePlaybackTimers();
//
//    shouldProcess = true;
//    sampleCounter = 0;
//
//    proc->playState = PLAY_STATE::PLAYING;
//}
//
//void LissajousChordPlayerProcessor::PlayRepeater::stop()
//{
//    shouldProcess   = false;
//
//    allNotesOff();
//
//    proc->playState = PLAY_STATE::OFF;
//}
////=================================================================
//// Calculators
////=================================================================
//void LissajousChordPlayerProcessor::PlayRepeater::calculatePlaybackTimers()
//{
//    totalNumSamplesOfLoop       = 0;
//
//    if (isActive)
//    {
//        int shortcutValInSamps = numRepeats * (lengthInSamples + pauseInSamples);
//
//        totalNumSamplesOfLoop += shortcutValInSamps;
//    }
//
//    totalMSOfLoop           = totalNumSamplesOfLoop / sampleRate * 1000;
//
////    printf("\n Total samps of sequence = %i", totalNumSamplesOfLoop);
////    printf("\n Total MS of sequence = %i", totalMSOfLoop);
//
//}
//
////=================================================================
//// Tick & event sequencing
////=================================================================
//void LissajousChordPlayerProcessor::PlayRepeater::processSecondsClock()
//{
//    currentMS = sampleCounter / ( sampleRate / 1000) ;
//}
//
//void LissajousChordPlayerProcessor::PlayRepeater::tickBuffer(int numSamples)
//{
//    if (shouldProcess)
//    {
//        for (int i = 0; i < numSamples; i++)
//        {
//            processSecondsClock();
//
//            // just sequence through active shortcuts
//            if (isActive)
//            {
//                processShortcut(sampleCounter);
//            }
//
//            sampleCounter++;
//
//            if (playMode == PLAY_MODE::NORMAL)
//            {
//                if (sampleCounter >= (totalNumSamplesOfLoop)) stop();
//            }
//            else if (playMode == PLAY_MODE::LOOP)
//            {
//                if (sampleCounter >= totalNumSamplesOfLoop) resetTick();
//            }
//        }
//    }
//}
//
//void LissajousChordPlayerProcessor::PlayRepeater::processShortcut(int sampleRef)
//{
//    if (sampleRef == nextNoteOnEvent)
//    {
//        triggerOnEvent();
//
//        nextNoteOffEvent = sampleRef + lengthInSamples;
//
//        currentRepeat++;
//    }
//    else if (sampleRef == nextNoteOffEvent)
//    {
//        triggerOffEvent();
//
//        // set next note on event after paus
//        if (currentRepeat < numRepeats)
//        {
//            nextNoteOnEvent = sampleRef + pauseInSamples;
//        }
//        else
//        {
////            for (int c = currentPlayingShortcut+1; c < 1; c++)
////            {
////                if (isActive[c])
////                {
////                    currentPlayingShortcut = c;
////                    prepareNextShortcut();
////                    break;
////                }
////            }
//
//        }
//    }
//}


LissajousChordPlayerProcessor::LissajousChordPlayerProcessor(FrequencyManager * fm, SampleLibraryManager * slm)
{
    frequencyManager        = fm;
    sampleLibraryManager    = slm;
    

    synth            = new ChordSynthProcessor(44100, frequencyManager);
    synth            ->setNoteStealingEnabled(true);
    
    sampler          = new SamplerProcessor(frequencyManager, sampleLibraryManager, 44100, 0);
    sampler          ->setNoteStealingEnabled(true);
    
    wavetableSynth   = new WavetableSynthProcessor(44100, frequencyManager);
    wavetableSynth   ->setNoteStealingEnabled(true);
    
    synth            ->setCurrentPlaybackSampleRate(44100);
    sampler          ->setCurrentPlaybackSampleRate(44100);
    wavetableSynth   ->setCurrentPlaybackSampleRate(44100);
    
    chordManager     = new ChordManager(frequencyManager);
    shouldMute       = false;
    waveformType     = SAMPLER;
    
    chordSource                 = false;
    octaveShift                 = 0.0;
    manipulateChoseFrequency    = false;
    multiplyOrDivision          = false;
    multiplyValue               = 1.0;
    divisionValue               = 1.0;

    repeater = new PlayRepeater(this, 44100);
    
}

LissajousChordPlayerProcessor::~LissajousChordPlayerProcessor()
{
    
}

void LissajousChordPlayerProcessor::prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock)
{
    synth            ->prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);
    sampler          ->setCurrentPlaybackSampleRate(sampleRate);
    wavetableSynth   ->prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);
    
    repeater->prepareToPlay(sampleRate);
}

void LissajousChordPlayerProcessor::setParameter(int synthRef, int index, var newValue)
{
    if (index >= INSTRUMENT_TYPE && index <= ENV_RELEASE)
    {
        synth         ->setParameter(index, newValue);
        sampler       ->setParameter(index, newValue);
        wavetableSynth->setParameter(index, newValue);
    }
    else if (index == WAVEFORM_TYPE)
    {
        panic();
        
        waveformType = (WAVEFORM_TYPES)newValue.operator int();
        
        synth->setParameter(index, newValue);
//        sampler[synthRef]->setParameter(index, newValue);
    }
    
    else if (index == CHORD_SOURCE)
    {
        if ((bool)newValue != chordSource)
        {
            chordSource = !chordSource;
        }
    }
    else if (index == INSERT_FREQUENCY)
    {
        insertFrequency = (float)newValue;
        
        if (!chordSource)
        {
            // need to face oscillator to override note frequency
            // do chord finder thing.
        }
    }
    else if (index == OCTAVE)
    {
        octaveShift = (float)newValue + 1;
    }
    else if (index == MANIPULATE_CHOSEN_FREQUENCY)
    {
        manipulateChoseFrequency = (bool)newValue;
    }
    else if (index == MULTIPLY_OR_DIVISION)
    {
        multiplyOrDivision = (bool)newValue;
    }
    else if (index == MULTIPLY_VALUE)
    {
        multiplyValue = (float) newValue;
    }
    else if (index == DIVISION_VALUE)
    {
        divisionValue = (float) newValue;
    }
    else if (index == SHORTCUT_IS_ACTIVE)
    {
        isActive = newValue.operator bool();
        repeater->setIsActive(synthRef, isActive);
    }
    else if (index == SHORTCUT_MUTE)
    {
        shouldMute = newValue.operator bool();
    }
    else if (index == NUM_REPEATS)
    {
        repeater->setNumRepeats(synthRef, newValue.operator int());
    }
    else if (index == NUM_PAUSE)
    {
        repeater->setPauseMS(synthRef, newValue.operator int());
    }
    else if (index == NUM_DURATION)
    {
        repeater->setLengthMS(synthRef, newValue.operator int());
    }
    else if (index == SHORTCUT_PLAY_AT_SAME_TIME)
    {
        // should delete
    }
    else if (index == KEYNOTE)
    {
        panic();
        
        enum KEYNOTES keyNote = (enum KEYNOTES)newValue.operator int();
        
        chordManager->setKeyNote(keyNote);
    }
    else if (index == CHORD_TYPE)
    {
        panic();
        
        CHORD_TYPES type = (CHORD_TYPES)newValue.operator int();
        
        chordManager->setChordType(type);
    }
    else if (index == ADD_ONS)
    {
        panic();
        
        enum ADD_ONS addOnType = (enum ADD_ONS)newValue.operator int();
        chordManager->setAddOn(addOnType);
    }
    else if (index == CUSTOM_CHORD)
    {
        panic();
        
        bool hasCustomChord = newValue.operator bool();
        chordManager->setCustomChord(hasCustomChord);
    }
    else if (index >= CUSTOM_CHORD_ACTIVE_1 && index <= CUSTOM_CHORD_ACTIVE_12)
    {
        panic();
        
        int pos = index - CUSTOM_CHORD_ACTIVE_1;
        chordManager->setActiveToCustomChord(pos, newValue.operator bool());
    }
    else if (index >= CUSTOM_CHORD_NOTE_1 && index <= CUSTOM_CHORD_NOTE_12)
    {
        panic();
        
        int pos = index - CUSTOM_CHORD_NOTE_1;
        chordManager->setNoteToCustomChord(pos, newValue.operator int());
    }
    else if (index >= CUSTOM_CHORD_OCTAVE_1 && index <= CUSTOM_CHORD_OCTAVE_12)
    {
        int pos = index - CUSTOM_CHORD_OCTAVE_1;
        chordManager->setOctaveToCustomChord(pos, newValue.operator int());
    }
    else if (index == CHORDPLAYER_SCALE)
    {
        panic();
        
        frequencyManager->setLissajousPlayerScale(newValue.operator int());
    }
}

void LissajousChordPlayerProcessor::setOversamplingFactor(int newFactor)
{
    // ** NOT WORKING YET
    suspendProcessing(true);
    
    for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
    {
//        synth[i]->setOversamplingFactor(newFactor);
    }
    
    suspendProcessing(false);
}

void LissajousChordPlayerProcessor::processBlock (AudioBuffer<float>& buffer,
                           MidiBuffer& midiMessages)
{
    if (playState == PLAY_STATE::PLAYING)
    {
        repeater->tickBuffer(buffer.getNumSamples());
    }
    

    if (isActive && !shouldMute)
    {
        if (waveformType == SAMPLER)
        {
            sampler->processBlock(buffer, midiMessages);
        }
        else if (waveformType == WAVETABLE)
        {
            wavetableSynth->processBlock(buffer, midiMessages);
        }
        else
        {
            synth->processBlock(buffer, midiMessages);
        }
    }

}

// trigger commands
// temp functions, called when shortcut keys are pressed
// can be evolved to trigger the repeater later, or called by by repeater
void LissajousChordPlayerProcessor::triggerKeyDown(int shortcutRef)
{
    triggerNoteOn(shortcutRef);
}

void LissajousChordPlayerProcessor::triggerKeyUp(int shortcutRef)
{
    triggerNoteOff(shortcutRef);
}

void LissajousChordPlayerProcessor::triggerNoteOn(int shortcutRef)
{
    // get chord notes from parameters
    Array<int>notes(chordManager->getMIDIKeysForChord());
    
    Array<float> noteFreqs;
    
    for (int i = 0; i < notes.size(); i++)
    {
        int midiNote = notes.getReference(i);
        
        if (midiNote >= 0 && midiNote <= 127)
        {
            double baseFreq = frequencyManager->scalesManager->getFrequencyForMIDINoteShortcut(midiNote, shortcutRef);
            double freq     = baseFreq;

            if (manipulateChoseFrequency)
            {
                if (!multiplyOrDivision)
                {
                    freq = baseFreq * pow(2.0, octaveShift) * multiplyValue;
                }
                else
                {
                    freq = baseFreq * pow(2.0, octaveShift) / divisionValue;
                }
            }
            else
            {
                freq = baseFreq * pow(2.0, octaveShift);
            }
            
            noteFreqs.add(freq);

            if (freq) // check freq does not = 0hz
            {
                if (waveformType == SAMPLER)
                {
                    sampler->noteOn(0, midiNote, freq);
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
    
    //** Crash ***
    // Crashes because note on is pushed at 0hz.. baseFreq returns from Scales Manager as 0, because note is not part of scale..
    // possible solutions are, simply dont trigger a note on when freq is 0.. should do this anyway
    // but really need not to offer the chord to the user
    
//    projectManager->logFileWriter->processLog_ChordPlayer_Sequencer(shortcutRef, noteFreqs);

}

void LissajousChordPlayerProcessor::triggerNoteOff(int shortcutRef)
{
    Array<int> notes = chordManager->getMIDIKeysForChord();
    
    for (int i = 0; i < notes.size(); i++)
    {
        int midiNote = notes.getReference(i);
        
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
}

void LissajousChordPlayerProcessor::panic()
{

        synth    ->allNotesOff(0, true);
        sampler  ->allNotesOff(0, true);
        wavetableSynth->allNotesOff(0, true);
}

void LissajousChordPlayerProcessor::setActiveShortcutSynth(int synthRef, bool shouldBeActive)
{
    isActive = shouldBeActive;
}

void LissajousChordPlayerProcessor::setPlayerCommand(PLAYER_COMMANDS command, bool freeFlow)
{
    switch (command)
    {
        case COMMAND_PLAYER_PLAYPAUSE:
        {
            if (!freeFlow)
            {
                if (playState == PLAY_STATE::PLAYING)
                {
                    repeater->stop();
                }
                else
                {
                    triggerNoteOn(0);
                }
            }
            else
            {
                repeater->play();
            }
        }
            break;
            
        case COMMAND_PLAYER_STOP:
        {
            playState = PLAY_STATE::OFF;
            
            if (!freeFlow)
            {
                triggerNoteOff(0);
            }
            else
            {
                repeater->stop();
            } 
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

void LissajousChordPlayerProcessor::setPlayerPlayMode(PLAY_MODE mode)
{
    playMode = mode;
    
    repeater->setPlayMode(mode);
}

//=================================================================
// Play Repeater Embedded Class
//=================================================================
LissajousChordPlayerProcessor::PlayRepeater::PlayRepeater(LissajousChordPlayerProcessor * cp, double sr)
{
    sampleRate                  = sr;
    proc                        = cp;
    sampleCounter               = 0;
    numSamplesPerMS             = sampleRate / 1000;
    playMode                    = PLAY_MODE::NORMAL;
    playState                   = PLAY_STATE::OFF;
    currentMS                   = 0;
    totalMSOfLoop               = 0;
    playSimultaneous           = false;
    
    
        isActive             = false;
        numRepeats           = 0;
        lengthMS             = 0;
        lengthInSamples      = 0;
        pauseMS              = 0;
        pauseInSamples       = 0;
        nextNoteOnEvent      = 0;
        nextNoteOffEvent     = 0;
        currentRepeat        = 0;
        isPaused             = false;
}

LissajousChordPlayerProcessor::PlayRepeater::~PlayRepeater()
{
    
}

void LissajousChordPlayerProcessor::PlayRepeater::prepareToPlay(double newSampleRate)
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

void LissajousChordPlayerProcessor::PlayRepeater::setIsActive(int shortcutRef, bool should)
{
    isActive = should;
    
    calculatePlaybackTimers();
}

void LissajousChordPlayerProcessor::PlayRepeater::calculateLengths(int shortcutRef)
{
    float oneMSInSamples = sampleRate / 1000;
    
    lengthInSamples = (int)lengthMS * oneMSInSamples;
    pauseInSamples  = (int)pauseMS  * oneMSInSamples;
    
    calculatePlaybackTimers();
}

void LissajousChordPlayerProcessor::PlayRepeater::setNumRepeats(int shortcutRef, int num)
{
    numRepeats = num; calculateLengths(shortcutRef);
}

void LissajousChordPlayerProcessor::PlayRepeater::setPauseMS(int shortcutRef, int ms)
{
    pauseMS = ms; calculateLengths(shortcutRef);
}

void LissajousChordPlayerProcessor::PlayRepeater::setLengthMS(int shortcutRef, int ms)
{
    lengthMS = ms; calculateLengths(shortcutRef);
}

void LissajousChordPlayerProcessor::PlayRepeater::setPlayMode(PLAY_MODE mode)
{
    playMode = mode;
}

void LissajousChordPlayerProcessor::PlayRepeater::setPlaySimultaneous(bool should)
{
    playSimultaneous = should;
    resetTick();
}

int LissajousChordPlayerProcessor::PlayRepeater::getTotalMSOfLoop()
{
    return totalMSOfLoop;
}

int LissajousChordPlayerProcessor::PlayRepeater::getCurrentMSInLoop()
{
    return currentMS;
}

float LissajousChordPlayerProcessor::PlayRepeater::getProgressBarValue()
{
    if (totalMSOfLoop == 0)
    {
        return 0;
    }
    else
    {
        return (float)currentMS / totalMSOfLoop;
    }
}

String LissajousChordPlayerProcessor::PlayRepeater::getTimeRemainingInSecondsString()
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
void LissajousChordPlayerProcessor::PlayRepeater::triggerOnEvent(int shortcutRef)
{
    proc->triggerNoteOn(shortcutRef);
}

void LissajousChordPlayerProcessor::PlayRepeater::triggerOffEvent(int shortcutRef)
{
    proc->triggerNoteOff(shortcutRef);
}

void LissajousChordPlayerProcessor::PlayRepeater::allNotesOff()
{
    proc->panic();
}

void LissajousChordPlayerProcessor::PlayRepeater::clearOpenRepeats()
{

        currentRepeat           = 0;
        currentPlayingShortcut  = 0;
        nextNoteOnEvent         = 0;
}

//=================================================================
// Transport Start / Stop etc
//=================================================================
void LissajousChordPlayerProcessor::PlayRepeater::resetTick()
{
//    calculatePlaybackTimers();
//
//    sampleCounter           = 0;
//    currentPlayingShortcut  = 0;
//
//    clearOpenRepeats();
    
    calculatePlaybackTimers();
    
    sampleCounter           = 0;
    
    clearOpenRepeats();
    
    if (playSimultaneous)
    {
        currentPlayingShortcut  = 0;
    }
    else
    {
        currentPlayingShortcut  = 0;
        
        for (int c = 0; c < 1; c++)
        {
            if (isActive && !proc->shouldMute)
            {
                currentPlayingShortcut = c; break;
            }
        }
    }
    
}

void LissajousChordPlayerProcessor::PlayRepeater::play()
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
        resetTick();

        shouldProcess   = true;
        sampleCounter   = 0;
        proc->playState = PLAY_STATE::PLAYING;
    }
}

void LissajousChordPlayerProcessor::PlayRepeater::stop()
{
    shouldProcess   = false;
    
    allNotesOff();
    
    proc->playState = PLAY_STATE::OFF;
}
//=================================================================
// Calculators
//=================================================================
void LissajousChordPlayerProcessor::PlayRepeater::calculatePlaybackTimers()
{
    totalNumSamplesOfLoop       = 0;
    int lastPauseInSamples      = 0;
    
    // first check for shoudPlays,
    // find longest shouldPlay Loop and add to val
    if (playSimultaneous)
    {
        int shortcutValInSamps  = 0;
        
        // simply find the longest loop
        for (int i = 0; i < 1; i++)
        {
            if (isActive & !proc->shouldMute)
            {
                shortcutValInSamps = numRepeats * (lengthInSamples + pauseInSamples);
            }
            
            if (shortcutValInSamps > totalNumSamplesOfLoop)
            {
                totalNumSamplesOfLoop = shortcutValInSamps;
            }
        }
        
        
    }
    else
    {
        
       
        
        // simply acc++  active shortcuts
        for (int i = 0; i < 1; i++)
        {
            if (isActive && !proc->shouldMute)
            {
                int shortcutValInSamps = numRepeats * (lengthInSamples + pauseInSamples);
                
                totalNumSamplesOfLoop += shortcutValInSamps;
                
                lastPauseInSamples = pauseInSamples;
            }
        }
    }
    

    
    totalMSOfLoop           = totalNumSamplesOfLoop / sampleRate * 1000;
    
//    printf("\n Total samps of sequence = %i", totalNumSamplesOfLoop);
//    printf("\n Total MS of sequence = %i", totalMSOfLoop);

}

//=================================================================
// Tick & event sequencing
//=================================================================
void LissajousChordPlayerProcessor::PlayRepeater::processSecondsClock()
{
    currentMS = sampleCounter / ( sampleRate / 1000) ;
}

void LissajousChordPlayerProcessor::PlayRepeater::tickBuffer(int numSamples)
{
    if (shouldProcess)
    {
        for (int i = 0; i < numSamples; i++)
        {
            processSecondsClock();
            
            if (playSimultaneous)
            {
                processSimultaneousShortcuts(currentPlayingShortcut, sampleCounter);
            }
            else
            {
                // just sequence through active shortcuts
                if (isActive)
                {
                    processShortcut(currentPlayingShortcut, sampleCounter);
                }
            }
            
            sampleCounter++;
            
            if (playMode == PLAY_MODE::NORMAL)
            {
                if (sampleCounter >= (totalNumSamplesOfLoop))
                {
                    currentMS = totalMSOfLoop;
                    
                    stop();
                }
            }
            else if (playMode == PLAY_MODE::LOOP)
            {
                if (sampleCounter >= totalNumSamplesOfLoop) resetTick();
            }
        }
    }
}

void LissajousChordPlayerProcessor::PlayRepeater::processShortcut(int shortcutRef, int sampleRef)
{
    if (sampleRef == nextNoteOnEvent)
    {
        triggerOnEvent(shortcutRef);
        
        nextNoteOffEvent = sampleRef + lengthInSamples;
        
        currentRepeat++;
    }
    else if (sampleRef == nextNoteOffEvent)
    {
        triggerOffEvent(shortcutRef);
        
        // set next note on event after paus
        if (currentRepeat < numRepeats)
        {
            nextNoteOnEvent = sampleRef + pauseInSamples;
        }
        else
        {
            for (int c = 0; c < 1; c++)
            {
                if (isActive && !proc->shouldMute)
                {
                    currentPlayingShortcut = c;
                    prepareNextShortcut();
                    break;
                }
            }
            
        }
    }
}

void LissajousChordPlayerProcessor::PlayRepeater::prepareNextShortcut()
{
    nextNoteOnEvent = sampleCounter + pauseInSamples;
}

void LissajousChordPlayerProcessor::PlayRepeater::processSimultaneousShortcuts(int firstShortcutRef, int sampleRef)
{
    for (int i = 0; i < 1; i++)
    {
        if (isActive)
        {
            if (sampleRef == nextNoteOnEvent)
            {
                triggerOnEvent(i);
                
                nextNoteOffEvent = sampleRef + lengthInSamples;
                
                currentRepeat++;
            }
            else if (sampleRef == nextNoteOffEvent)
            {
                triggerOffEvent(i);
                
                if (currentRepeat < numRepeats)
                {
                    nextNoteOnEvent = sampleRef + pauseInSamples;
                }
            }
        }
    }
}
