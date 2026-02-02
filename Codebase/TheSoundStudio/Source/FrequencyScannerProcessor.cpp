/*
  ==============================================================================

    FrequencyScannerProcessor.cpp

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#include "FrequencyScannerProcessor.h"
#include "ProjectManager.h"
#include "AudioRouting.h"

FrequencyScannerProcessor::FrequencyScannerProcessor(FrequencyManager * fm, ProjectManager * pm)
{
    projectManager      = pm;
    frequencyManager    = fm;

    synth           = new FrequencyScannerSynthProcessor(44100, frequencyManager);
    synth           ->setNoteStealingEnabled(true);
    synth           ->setCurrentPlaybackSampleRate(44100);
    
    wavetableSynth   = new FrequencyScannerWavetableSynthProcessor(44100, frequencyManager);
    wavetableSynth   ->setNoteStealingEnabled(true);
    wavetableSynth  ->setCurrentPlaybackSampleRate(44100);
        
    shouldMute      = false;
    waveformType    = DEFAULT;
    
    repeater        = new PlayRepeater(this, 44100);
}

FrequencyScannerProcessor::~FrequencyScannerProcessor()
{
    // Properly clean up allocated resources
    if (synth)
    {
        delete synth;
        synth = nullptr;
    }
    
    if (wavetableSynth)
    {
        delete wavetableSynth;
        wavetableSynth = nullptr;
    }
    
    if (repeater)
    {
        delete repeater;
        repeater = nullptr;
    }
}

void FrequencyScannerProcessor::prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock)
{
    synth       ->prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);
    repeater    ->prepareToPlay(sampleRate);
    
    outputBuffer.clear();
    
    outputBuffer.setSize(1, maximumExpectedSamplesPerBlock);
}

void FrequencyScannerProcessor::setParameter(int index, var newValue)
{
    if (index == FREQUENCY_SCANNER_WAVEFORM_TYPE)
    {
        panic();
        
        waveformType = (WAVEFORM_TYPES)newValue.operator int();
        
        if (waveformType != WAVETABLE)
        {
            synth           ->setParameter(index, newValue);
        }

    }
    else if (index > FREQUENCY_SCANNER_WAVEFORM_TYPE && index <= FREQUENCY_SCANNER_ENV_RELEASE)
    {
        synth           ->setParameter(index, newValue);
        wavetableSynth  ->setParameter(index, newValue);
    }
    else if (index == FREQUENCY_SCANNER_NUM_PAUSE)
    {
        repeater->setPauseMS(newValue.operator int());  
    }
    else if (index == FREQUENCY_SCANNER_NUM_DURATION)
    {
        repeater->setLengthMS(newValue.operator int());
    }
    else if (index == FREQUENCY_SCANNER_NUM_REPEATS)
    {
        repeater->setNumRepeats(newValue.operator int());
    }
    else if (index == FREQUENCY_SCANNER_MODE)
    {
        repeater->setScanningMode(newValue.operator int());
    }
    else if (index == FREQUENCY_SCANNER_FREQUENCY_FROM)
    {
        repeater->setFrequencyFrom(newValue.operator float());
    }
    else if (index == FREQUENCY_SCANNER_FREQUENCY_TO)
    {
        repeater->setFrequencyTo(newValue.operator float());
    }
    else if (index == FREQUENCY_SCANNER_EXTENDED)
    {
        repeater->setExtendedRange(newValue.operator bool());
    }
    else if (index == FREQUENCY_SCANNER_LOG_LIN)
    {
        repeater->setLogOrLinear(newValue.operator int());
    }
    else if (index == FREQUENCY_SCANNER_LIN_VALUE)
    {
        repeater->setLinIntervalInHz(newValue.operator float());
    }
    else if (index == FREQUENCY_SCANNER_LOG_VALUE)
    {
        repeater->setLogIntervalInOctaves(newValue.operator float());
    }
    else if (index == FREQUENCY_SCANNER_OUTPUT_SELECTION)
    {
        output = (AUDIO_OUTPUTS)newValue.operator int();
    }
}

void FrequencyScannerProcessor::setOversamplingFactor(int newFactor)
{
    // ** NOT WORKING YET
    suspendProcessing(true);
    
    for (int i = 0; i < 1; i++)
    {
        //        synth[i]->setOversamplingFactor(newFactor);
        
    }
    
    suspendProcessing(false);
}

void FrequencyScannerProcessor::processBlock (AudioBuffer<float>& buffer,
                                             MidiBuffer& midiMessages)
{
    // process repeater first
    if (playState == PLAY_STATE::PLAYING)
    {
        // run repeater looper..
        repeater->tickBuffer(buffer.getNumSamples());
    }
    
    outputBuffer.clear();

    if (output != AUDIO_OUTPUTS::NO_OUTPUT)
    {
        if (waveformType == WAVETABLE)
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
void FrequencyScannerProcessor::triggerKeyDown()
{
//    triggerNoteOn();
}

void FrequencyScannerProcessor::triggerKeyUp()
{
//    triggerNoteOff();
}

void FrequencyScannerProcessor::triggerNoteOn(float freq) // need to add arg for frequency of triggered note
{
    if (waveformType == WAVETABLE)
    {
        wavetableSynth->noteOn(0, 0, freq);
    }
    else
    {
        synth->noteOn(0, 0, freq);
    }
    
    projectManager->logFileWriter->processLog_FrequencyScanner_Sequencer(freq);
}

void FrequencyScannerProcessor::triggerNoteOff()
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

void FrequencyScannerProcessor::panic()
{
    synth    ->allNotesOff(0, true);
    wavetableSynth->allNotesOff(0, true);
}

void FrequencyScannerProcessor::setActiveShortcutSynth(int synthRef, bool shouldBeActive)
{
    isActive = shouldBeActive;
}

void FrequencyScannerProcessor::setPlayerCommand(PLAYER_COMMANDS command)
{
    switch (command)
    {
        case COMMAND_PLAYER_PLAYPAUSE:
        {
//            playState = PLAY_STATE::PLAYING;
            
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

void FrequencyScannerProcessor::setPlayerPlayMode(PLAY_MODE mode)
{
    playMode = mode;
    
    repeater->setPlayMode(mode);
}

// Play Repeater Embedded class

FrequencyScannerProcessor::PlayRepeater::PlayRepeater(FrequencyScannerProcessor * cp, double sr)
{
    sampleRate                  = sr;
    proc                        = cp;
    sampleCounter               = 0;
    numSamplesPerMS             = sampleRate / 1000;
    playMode                    = PLAY_MODE::NORMAL;
    playState                   = PLAY_STATE::OFF;
    currentMS                   = 0;
    totalMSOfLoop               = 0;
    
    isActive                    = false;
    totalNumRepeats             = 0;
    numRepeats                  = 0;
    lengthMS                    = 0;
    lengthInSamples             = 0;
    pauseMS                     = 0;
    pauseInSamples              = 0;
    nextNoteOnEvent             = 0;
    nextNoteOffEvent            = 0;
    currentRepeat               = 0;
    isPaused                    = false;
    
    scanningMode                = 0; // all ferequency / selected range
    frequencyFrom               = FREQUENCY_MIN;
    frequencyTo                 = 20000;
    frequencyToExtended         = FREQUENCY_MAX;
    extendedRange               = false;
    logOrLinear                 = 0;
    logIntervalInOctaves        = 0.5f; // = 1 semitone
    linIntervalInHz             = 1.f; // = 1 hz
    
    currentFrequency            = frequencyFrom;
    nextFrequency               = currentFrequency;
    numRepeatIterator           = 0;
}

FrequencyScannerProcessor::PlayRepeater::~PlayRepeater()
{
    
}

void FrequencyScannerProcessor::PlayRepeater::prepareToPlay(double newSampleRate)
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

void FrequencyScannerProcessor::PlayRepeater::setIsActive(bool should)
{
    isActive = should;
    
    calculatePlaybackTimers();
}

void FrequencyScannerProcessor::PlayRepeater::calculateLengths()
{
    float oneMSInSamples = sampleRate / 1000;
    
    lengthInSamples = (int)lengthMS * oneMSInSamples;
    pauseInSamples  = (int)pauseMS  * oneMSInSamples;
    
    calculatePlaybackTimers();
}

void FrequencyScannerProcessor::PlayRepeater::setPauseMS(int ms)
{
    pauseMS = ms; calculateLengths();
}

void FrequencyScannerProcessor::PlayRepeater::setLengthMS(int ms)
{
    lengthMS = ms; calculateLengths();
}

void FrequencyScannerProcessor::PlayRepeater::setNumRepeats(int num)
{
    numRepeats = num; calculateLengths();
}

void FrequencyScannerProcessor::PlayRepeater::setPlayMode(PLAY_MODE mode)
{
    playMode = mode;
}

int FrequencyScannerProcessor::PlayRepeater::getTotalMSOfLoop()
{
    return totalMSOfLoop;
}

int FrequencyScannerProcessor::PlayRepeater::getCurrentMSInLoop()
{
    return currentMS;
}

float FrequencyScannerProcessor::PlayRepeater::getProgressBarValue()
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

String FrequencyScannerProcessor::PlayRepeater::getTimeRemainingInSecondsString()
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
void FrequencyScannerProcessor::PlayRepeater::triggerOnEvent(float f)
{
    proc->triggerNoteOn(f); // need to grab  urrent Frequency from the scanner
}

void FrequencyScannerProcessor::PlayRepeater::triggerOffEvent()
{
    proc->triggerNoteOff();
}

void FrequencyScannerProcessor::PlayRepeater::allNotesOff()
{
    proc->panic();
}

void FrequencyScannerProcessor::PlayRepeater::clearOpenRepeats()
{
    currentRepeat           = 0;
    nextNoteOnEvent         = 0;
}

//=================================================================
// Transport Start / Stop etc
//=================================================================
void FrequencyScannerProcessor::PlayRepeater::resetTick()
{
    calculatePlaybackTimers();
    
    sampleCounter           = 0;
    
    if (scanningMode == 0) {
        currentFrequency        = FREQUENCY_MIN;
    }
    else {
        currentFrequency        = frequencyFrom;
    }
    
    nextFrequency = currentFrequency;
    
    
    clearOpenRepeats();
}

void FrequencyScannerProcessor::PlayRepeater::play()
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
        
        proc->projectManager->logFileWriter->processLog_FrequencyScanner_Parameters();
        
        shouldProcess = true;
        sampleCounter = 0;
        
        proc->playState = PLAY_STATE::PLAYING;
        
        // Start from 0..
        
        // call log for Parameters..
    }
    

}

void FrequencyScannerProcessor::PlayRepeater::stop()
{
    shouldProcess   = false;
    
    allNotesOff();
    
    proc->playState = PLAY_STATE::OFF;
}
//=================================================================
// Calculators
//=================================================================
void FrequencyScannerProcessor::PlayRepeater::calculatePlaybackTimers()
{
    totalNumSamplesOfLoop   = 0;
    int totalNumIntervals   = 0;
    float minFrequency      = FREQUENCY_MIN;
    
    if (scanningMode == 0)
    {
        //==================================================================
        // Scan all frequencies
        //==================================================================
        
        // work out total repeats needed for both log and linear intervals
        // total frequency range min - max freq or extended to 100khz
        
        if (extendedRange)
        {
            if (logOrLinear == 0) // log
            {
                // need to find number of intervals @ x octave increments..
                int num = 0; float f = minFrequency;
                while (f < frequencyToExtended) {  f *= (1 + logIntervalInOctaves); num++; }
                
                totalNumIntervals       = num+1;
                totalNumSamplesOfLoop   = totalNumIntervals * (lengthInSamples + pauseInSamples) * numRepeats;
            }
            else            // linear
            {
                float range             = frequencyToExtended - minFrequency; // min frequency
                totalNumIntervals       = (int)range / linIntervalInHz;
                totalNumSamplesOfLoop   = totalNumIntervals * (lengthInSamples + pauseInSamples) * numRepeats;
            }
        }
        else
        {
            
            if (logOrLinear == 0) // log
            {
                // need to find number of intervals @ x octave increments..
                int num = 0;
                float f = minFrequency;
                while (f < frequencyTo) {  f *= (1 + logIntervalInOctaves); num++; }
                
                totalNumIntervals       = num+1;
                totalNumSamplesOfLoop   = totalNumIntervals * (lengthInSamples + pauseInSamples) * numRepeats;
            }
            else            // linear
            {
                float range             = frequencyTo - minFrequency;
                totalNumIntervals       = (int)range / linIntervalInHz;
                totalNumIntervals       += 1;
                totalNumSamplesOfLoop   = totalNumIntervals * (lengthInSamples + pauseInSamples) * numRepeats ;
            }
        }
    }
    else
    {
        //==================================================================
        // scan specific frequencies
        //==================================================================
        
        // work out total repeats needed for both log and linear intervals
        // total frequency range min - max freq or extended to 100khz
        
        if (logOrLinear == 0) // log
        {
            // need to find number of intervals @ x octave increments..
            int num = 0; float f = frequencyFrom;
            while (f < frequencyTo) {  f *= (1 + logIntervalInOctaves); num++; }
                
            totalNumIntervals       = num+1;
            totalNumSamplesOfLoop   = totalNumIntervals * (lengthInSamples + pauseInSamples) * numRepeats;
        }
        else            // linear
        {
            float range             = frequencyTo - frequencyFrom; // min frequency
            totalNumIntervals       = (int)range / linIntervalInHz;
            totalNumSamplesOfLoop   = totalNumIntervals * (lengthInSamples + pauseInSamples) * numRepeats;
        }
    }
    
    totalMSOfLoop           = (totalNumSamplesOfLoop / sampleRate * 1000);
    totalNumRepeats         = totalNumIntervals * numRepeats;

}

//=================================================================
// Tick & event sequencing
//=================================================================
void FrequencyScannerProcessor::PlayRepeater::processSecondsClock()
{
    currentMS = sampleCounter / ( sampleRate / 1000) ;
}

void FrequencyScannerProcessor::PlayRepeater::tickBuffer(int numSamples)
{
    if (shouldProcess)
    {
        for (int i = 0; i < numSamples; i++)
        {
            processSecondsClock();


            if (logOrLinear == 0) // log
            {
                processLog(sampleCounter);
            }
            else
            {
                processLin(sampleCounter);
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

void FrequencyScannerProcessor::PlayRepeater::processLog(int sampleRef)
{
    if (sampleRef == nextNoteOnEvent)
    {
        currentFrequency = nextFrequency;
        
        triggerOnEvent(currentFrequency);
        
        
        if (numRepeatIterator >= numRepeats-1)
        {
            numRepeatIterator = 0;
            
            nextFrequency *= (1 + logIntervalInOctaves);
            
            if (extendedRange)
            {
                if (nextFrequency >= frequencyToExtended) { nextFrequency = frequencyToExtended; }
            }
            else
            {
                if (nextFrequency >= frequencyTo) { nextFrequency = frequencyTo; }
            }
            
        }
        else
        {
            numRepeatIterator++;
        }
        
        nextNoteOffEvent = sampleRef + lengthInSamples;

        currentRepeat++;
    }
    else if (sampleRef == nextNoteOffEvent)
    {
        triggerOffEvent();

        // set next note on event after paus
        if (currentRepeat < totalNumRepeats)
        {
            nextNoteOnEvent = sampleRef + pauseInSamples;
        }
    }
}


void FrequencyScannerProcessor::PlayRepeater::processLin(int sampleRef)
{
    if (sampleRef == nextNoteOnEvent)
    {
        currentFrequency = nextFrequency;
        
        triggerOnEvent(currentFrequency);
        
        if (numRepeatIterator >= numRepeats-1)
        {
            numRepeatIterator = 0;
            
            nextFrequency += linIntervalInHz;
            
            if (extendedRange)
            {
                if (nextFrequency >= frequencyToExtended) { nextFrequency = frequencyToExtended; }
            }
            else
            {
                if (nextFrequency >= frequencyTo) { nextFrequency = frequencyTo; }
            }
        }
        else
        {
            numRepeatIterator++;
        }
        
        nextNoteOffEvent = sampleRef + lengthInSamples;

        currentRepeat++;
    }
    else if (sampleRef == nextNoteOffEvent)
    {
        triggerOffEvent();

        // set next note on event after paus
        if (currentRepeat < totalNumRepeats)
        {
            nextNoteOnEvent = sampleRef + pauseInSamples;
        }
    }
}
