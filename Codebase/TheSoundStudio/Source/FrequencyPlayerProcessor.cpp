/*
  ==============================================================================

    FrequencyPlayerProcessor.cpp

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#include "FrequencyPlayerProcessor.h"
#include "ProjectManager.h"
#include "AudioRouting.h"

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
    // Properly clean up allocated resources
    for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
    {
        if (synth[i])
        {
            delete synth[i];
            synth[i] = nullptr;
        }
        
        if (wavetableSynth[i])
        {
            delete wavetableSynth[i];
            wavetableSynth[i] = nullptr;
        }
    }
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
                
                TSS::Audio::routeToOutput(buffer, outputBuffer, output[s]);
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

// PlayRepeater implementation now lives in PlayRepeaterBase.h
// FrequencyPlayerProcessor::PlayRepeater only overrides the virtual hooks (defined inline in header)
