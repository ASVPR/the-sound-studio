/*
  ==============================================================================

    FrequencyPlayerProcessor.cpp
    Created: 30 May 2019 2:46:52pm
    Author:  Gary Jones

  ==============================================================================
*/

#include "FrequencyPlayerProcessor.h"
#include "ProjectManager.h"

FrequencyPlayerProcessor::FrequencyPlayerProcessor(FrequencyManager * fm, ProjectManager * pm)
{
    projectManager      = pm;
    frequencyManager    = fm;
    
    for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
    {
        synth[i]            = new FrequencySynthProcessor(44100, frequencyManager);
        synth[i]            ->setNoteStealingEnabled(true);
        synth[i]            ->setCurrentPlaybackSampleRate(44100);

        wavetableSynth[i]   = new FrequencyPlayerWavetableSynthProcessor(44100, frequencyManager);
        wavetableSynth[i]   ->setNoteStealingEnabled(true);
        wavetableSynth[i]   ->setCurrentPlaybackSampleRate(44100);
        
        waveformType[i] = DEFAULT;
        
        shouldMute[i] = false;
    }

    repeater = new PlayRepeater(this, 44100);
    
}

FrequencyPlayerProcessor::~FrequencyPlayerProcessor()
{
    
}

void FrequencyPlayerProcessor::prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock)
{
    for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
    {
        synth[i]    ->prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);
        wavetableSynth[i]   ->prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);
    }
    
    repeater->prepareToPlay(sampleRate);
    
    outputBuffer.clear();
    
    outputBuffer.setSize(1, maximumExpectedSamplesPerBlock);
}

void FrequencyPlayerProcessor::setParameter(int synthRef, int index, var newValue)
{
    if (index == FREQUENCY_PLAYER_WAVEFORM_TYPE)
    {
        panic();
        
        waveformType[synthRef] = (WAVEFORM_TYPES)newValue.operator int();
        
        if (waveformType[synthRef] != WAVETABLE)
        {
            synth[synthRef]           ->setParameter(index, newValue);
        }
    }
    else if (index >= FREQUENCY_PLAYER_FREQ_SOURCE && index <= FREQUENCY_PLAYER_RELEASE)
    {
        synth[synthRef]->setParameter(index, newValue);
        wavetableSynth[synthRef]->setParameter(index, newValue);
    }
    else if (index == FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE)
    {
        isActive[synthRef] = newValue.operator bool();
        repeater->setIsActive(synthRef, isActive[synthRef]);
    }
    else if (index == FREQUENCY_PLAYER_SHORTCUT_MUTE)
    {
        shouldMute[synthRef] = newValue.operator bool();
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
    else if (index == FREQUENCY_PLAYER_OUTPUT_SELECTION)
    {
        output[synthRef] = (AUDIO_OUTPUTS)newValue.operator int();
    }
}

void FrequencyPlayerProcessor::setOversamplingFactor(int newFactor)
{
    // ** NOT WORKING YET
    suspendProcessing(true);
    
    for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
    {
//        synth[i]->setOversamplingFactor(newFactor);
        
    }
    
    suspendProcessing(false);
}

void FrequencyPlayerProcessor::processBlock (AudioBuffer<float>& buffer,
                                         MidiBuffer& midiMessages)
{
    // process repeater first
    if (playState == PLAY_STATE::PLAYING)
    {
        // run repeater looper..
        repeater->tickBuffer(buffer.getNumSamples());
    }
    
    for (int s = 0; s < NUM_SHORTCUT_SYNTHS; s++)
    {
        if (isActive[s] && !shouldMute[s])
        {
            outputBuffer.clear();
            
            if (output[s] != AUDIO_OUTPUTS::NO_OUTPUT)
            {
                if (waveformType[s] == WAVETABLE)
                {
                    wavetableSynth[s]->processBlock(outputBuffer, midiMessages);
                }
                else
                {
                    synth[s]->processBlock(outputBuffer, midiMessages);
                }
                
                if      (output[s] == AUDIO_OUTPUTS::MONO_1) { buffer.addFrom(0, 0, outputBuffer, 0, 0, buffer.getNumSamples()); }
                else if (output[s] == AUDIO_OUTPUTS::MONO_2 && buffer.getNumChannels() > 1) { buffer.addFrom(1, 0, outputBuffer, 0, 0, buffer.getNumSamples()); }
                else if (output[s] == AUDIO_OUTPUTS::MONO_3 && buffer.getNumChannels() > 2) { buffer.addFrom(2, 0, outputBuffer, 0, 0, buffer.getNumSamples()); }
                else if (output[s] == AUDIO_OUTPUTS::MONO_4 && buffer.getNumChannels() > 3) { buffer.addFrom(3, 0, outputBuffer, 0, 0, buffer.getNumSamples()); }
                else if (output[s] == AUDIO_OUTPUTS::STEREO_1_2 && buffer.getNumChannels() > 1)
                {
                    buffer.addFrom(0, 0, outputBuffer, 0, 0, buffer.getNumSamples());
                    buffer.addFrom(1, 0, outputBuffer, 0, 0, buffer.getNumSamples());
                }
                else if (output[s] == AUDIO_OUTPUTS::STEREO_3_4 && buffer.getNumChannels() > 2)
                {
                    buffer.addFrom(2, 0, outputBuffer, 0, 0, buffer.getNumSamples());
                    buffer.addFrom(3, 0, outputBuffer, 0, 0, buffer.getNumSamples());
                }
            }
        }
    }
}

// trigger commands
// temp functions, called when shortcut keys are pressed
// can be evolved to trigger the repeater later, or called by by repeater
void FrequencyPlayerProcessor::triggerKeyDown(int shortcutRef)
{
    triggerNoteOn(shortcutRef);
}

void FrequencyPlayerProcessor::triggerKeyUp(int shortcutRef)
{
    triggerNoteOff(shortcutRef);
}

void FrequencyPlayerProcessor::triggerNoteOn(int shortcutRef)
{
    float freq = 0;
    if (waveformType[shortcutRef] == WAVETABLE)
    {
        wavetableSynth[shortcutRef]->noteOn(0, 0, 1.0);
        
        freq = wavetableSynth[shortcutRef]->getChosenFrequency();
    }
    else
    {
        synth[shortcutRef]->noteOn(0, 0, 1.0);
        
        freq = synth[shortcutRef]->getChosenFrequency();
    }
    
    
    projectManager->logFileWriter->processLog_FrequencyPlayer_Sequencer(shortcutRef, freq);
    
    
}

void FrequencyPlayerProcessor::triggerNoteOff(int shortcutRef)
{
    if (waveformType[shortcutRef] == WAVETABLE)
    {
        wavetableSynth[shortcutRef]->noteOff(0, 0, 0.0, true);
    }
    else
    {
        synth[shortcutRef]->noteOff(0, 0, 0.0, true);
    }
    
}

void FrequencyPlayerProcessor::panic()
{
    for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
    {
        synth[i]    ->allNotesOff(0, true);
        wavetableSynth[i]    ->allNotesOff(0, true);
    }
}

void FrequencyPlayerProcessor::setActiveShortcutSynth(int synthRef, bool shouldBeActive)
{
    isActive[synthRef] = shouldBeActive;
}

void FrequencyPlayerProcessor::setPlayerCommand(PLAYER_COMMANDS command)
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

void FrequencyPlayerProcessor::setPlayerPlayMode(PLAY_MODE mode)
{
    playMode = mode;
    
    repeater->setPlayMode(mode);
}

// Play Repeater Embedded class

FrequencyPlayerProcessor::PlayRepeater::PlayRepeater(FrequencyPlayerProcessor * cp, double sr)
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
    
    
    for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
    {
        isActive[i]             = false;
        numRepeats[i]           = 0;
        lengthMS[i]             = 0;
        lengthInSamples[i]      = 0;
        pauseMS[i]              = 0;
        pauseInSamples[i]       = 0;
        nextNoteOnEvent[i]      = 0;
        nextNoteOffEvent[i]     = 0;
        currentRepeat[i]        = 0;
        isPaused[i]             = false;
    }
}

FrequencyPlayerProcessor::PlayRepeater::~PlayRepeater()
{
    
}

void FrequencyPlayerProcessor::PlayRepeater::prepareToPlay(double newSampleRate)
{
    shouldProcess = false;
    
    sampleRate = newSampleRate;
    
    float oneMSInSamples = sampleRate / 1000;
    
    for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
    {
        
        lengthInSamples[i] = (int)lengthMS[i] * oneMSInSamples;
        pauseInSamples[i]  = (int)pauseMS[i]  * oneMSInSamples;
    }
    
    calculatePlaybackTimers();
    
    shouldProcess = true;
}

//=================================================================
// Setters and Getters
//=================================================================

void FrequencyPlayerProcessor::PlayRepeater::setIsActive(int shortcutRef, bool should)
{
    isActive[shortcutRef] = should;
    
    calculatePlaybackTimers();
}

void FrequencyPlayerProcessor::PlayRepeater::calculateLengths(int shortcutRef)
{
    float oneMSInSamples = sampleRate / 1000;
    
    lengthInSamples[shortcutRef] = (int)lengthMS[shortcutRef] * oneMSInSamples;
    pauseInSamples[shortcutRef]  = (int)pauseMS[shortcutRef]  * oneMSInSamples;
    
    calculatePlaybackTimers();
}

void FrequencyPlayerProcessor::PlayRepeater::setNumRepeats(int shortcutRef, int num)
{
    numRepeats[shortcutRef] = num; calculateLengths(shortcutRef);
}

void FrequencyPlayerProcessor::PlayRepeater::setPauseMS(int shortcutRef, int ms)
{
    pauseMS[shortcutRef] = ms; calculateLengths(shortcutRef);
}

void FrequencyPlayerProcessor::PlayRepeater::setLengthMS(int shortcutRef, int ms)
{
    lengthMS[shortcutRef] = ms; calculateLengths(shortcutRef);
}

void FrequencyPlayerProcessor::PlayRepeater::setPlayMode(PLAY_MODE mode)
{
    playMode = mode;
}

void FrequencyPlayerProcessor::PlayRepeater::setPlaySimultaneous(bool should)
{
    playSimultaneous = should;
    resetTick();
}

int FrequencyPlayerProcessor::PlayRepeater::getTotalMSOfLoop()
{
    return totalMSOfLoop;
}

int FrequencyPlayerProcessor::PlayRepeater::getCurrentMSInLoop()
{
    return currentMS;
}

float FrequencyPlayerProcessor::PlayRepeater::getProgressBarValue()
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

String FrequencyPlayerProcessor::PlayRepeater::getTimeRemainingInSecondsString()
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
void FrequencyPlayerProcessor::PlayRepeater::triggerOnEvent(int shortcutRef)
{
    proc->triggerNoteOn(shortcutRef);
}

void FrequencyPlayerProcessor::PlayRepeater::triggerOffEvent(int shortcutRef)
{
    proc->triggerNoteOff(shortcutRef);
}

void FrequencyPlayerProcessor::PlayRepeater::allNotesOff()
{
    proc->panic();
}

void FrequencyPlayerProcessor::PlayRepeater::clearOpenRepeats()
{
    for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
    {
        currentRepeat[i]        = 0;
        currentPlayingShortcut  = 0;
        nextNoteOnEvent[i]      = 0;
    }
}

//=================================================================
// Transport Start / Stop etc
//=================================================================
void FrequencyPlayerProcessor::PlayRepeater::resetTick()
{
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
        
        for (int c = currentPlayingShortcut; c < NUM_SHORTCUT_SYNTHS; c++)
        {
            if (isActive[c] && !proc->shouldMute[c])
            {
                currentPlayingShortcut = c; break;
            }
        }
    }
}

void FrequencyPlayerProcessor::PlayRepeater::play()
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

void FrequencyPlayerProcessor::PlayRepeater::stop()
{
    
    shouldProcess   = false;
    
    allNotesOff();
    
    proc->playState = PLAY_STATE::OFF;
}
//=================================================================
// Calculators
//=================================================================
void FrequencyPlayerProcessor::PlayRepeater::calculatePlaybackTimers()
{
    totalNumSamplesOfLoop       = 0;
    
    // first check for shoudPlays,
    // find longest shouldPlay Loop and add to val
    if (playSimultaneous)
    {
        int shortcutValInSamps  = 0;
        
        // simply find the longest loop
        for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
        {
            if (isActive[i] & !proc->shouldMute[i])
            {
                shortcutValInSamps = numRepeats[i] * (lengthInSamples[i] + pauseInSamples[i]);
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
        for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
        {
            if (isActive[i] && !proc->shouldMute[i])
            {
                int shortcutValInSamps = numRepeats[i] * (lengthInSamples[i] + pauseInSamples[i]);
                
                totalNumSamplesOfLoop += shortcutValInSamps;
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
void FrequencyPlayerProcessor::PlayRepeater::processSecondsClock()
{
    currentMS = sampleCounter / ( sampleRate / 1000) ;
}

void FrequencyPlayerProcessor::PlayRepeater::tickBuffer(int numSamples)
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
                if (isActive[currentPlayingShortcut])
                {
                    processShortcut(currentPlayingShortcut, sampleCounter);
                }
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

void FrequencyPlayerProcessor::PlayRepeater::processShortcut(int shortcutRef, int sampleRef)
{
    if (sampleRef == nextNoteOnEvent[shortcutRef])
    {
        triggerOnEvent(shortcutRef);
        
        nextNoteOffEvent[shortcutRef] = sampleRef + lengthInSamples[shortcutRef];
        
        currentRepeat[shortcutRef]++;
    }
    else if (sampleRef == nextNoteOffEvent[shortcutRef])
    {
        triggerOffEvent(shortcutRef);
        
        // set next note on event after paus
        if (currentRepeat[shortcutRef] < numRepeats[shortcutRef])
        {
            nextNoteOnEvent[shortcutRef] = sampleRef + pauseInSamples[shortcutRef];
        }
        else
        {
            for (int c = currentPlayingShortcut+1; c < NUM_SHORTCUT_SYNTHS; c++)
            {
                if (isActive[c] && !proc->shouldMute[c])
                {
                    currentPlayingShortcut = c;
                    prepareNextShortcut();
                    break;
                }
            }
            
        }
    }
}

void FrequencyPlayerProcessor::PlayRepeater::prepareNextShortcut()
{
    nextNoteOnEvent[currentPlayingShortcut] = sampleCounter + pauseInSamples[currentPlayingShortcut-1];
}

void FrequencyPlayerProcessor::PlayRepeater::processSimultaneousShortcuts(int firstShortcutRef, int sampleRef)
{
    for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
    {
        if (isActive[i])
        {
            if (sampleRef == nextNoteOnEvent[i])
            {
                triggerOnEvent(i);
                
                nextNoteOffEvent[i] = sampleRef + lengthInSamples[i];
                
                currentRepeat[i]++;
            }
            else if (sampleRef == nextNoteOffEvent[i])
            {
                triggerOffEvent(i);
                
                if (currentRepeat[i] < numRepeats[i])
                {
                    nextNoteOnEvent[i] = sampleRef + pauseInSamples[i];
                }
            }
        }
    }
}
