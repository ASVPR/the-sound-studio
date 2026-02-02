/*
  ==============================================================================

    ChordPlayerProcessor.cpp

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#include "ChordPlayerProcessor.h"
#include "ProjectManager.h"
#include "AudioRouting.h"

ChordPlayerProcessor::ChordPlayerProcessor(FrequencyManager * fm, SynthesisLibraryManager * slm, SynthesisEngine * se, ProjectManager * pm)
{
    projectManager          = pm;
    frequencyManager        = fm;
    sampleLibraryManager    = slm;
    synthesisEngine         = se;
    
    for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
    {
        synth[i]            = new ChordSynthProcessor(44100, frequencyManager);
        synth[i]            ->setNoteStealingEnabled(true);
        
        sampler[i]          = new SamplerProcessor(frequencyManager, sampleLibraryManager, 44100, i);
        sampler[i]          ->setNoteStealingEnabled(true);
        
        wavetableSynth[i]   = new WavetableSynthProcessor(44100, frequencyManager);
        wavetableSynth[i]   ->setNoteStealingEnabled(true);
        
        synth[i]            ->setCurrentPlaybackSampleRate(44100);
        sampler[i]          ->setCurrentPlaybackSampleRate(44100);
        wavetableSynth[i]   ->setCurrentPlaybackSampleRate(44100);
        
        chordManager[i]     = new ChordManager(frequencyManager);
        shouldMute[i]       = false;
        waveformType[i]     = SAMPLER; // Keep original - will redirect SAMPLER to use synthesis
        
        chordSource[i]                 = false;
        octaveShift[i]                 = 0.0;
        manipulateChoseFrequency[i]    = false;
        multiplyOrDivision[i]          = false;
        multiplyValue[i]               = 1.0;
        divisionValue[i]               = 1.0;
        
        hasCustomChord[i]               = false;
    }

    repeater = new PlayRepeater(this, 44100);
    
}

ChordPlayerProcessor::~ChordPlayerProcessor()
{
    // Properly clean up allocated resources
    for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
    {
        if (synth[i])
        {
            delete synth[i];
            synth[i] = nullptr;
        }
        
        if (sampler[i])
        {
            delete sampler[i];
            sampler[i] = nullptr;
        }
        
        if (wavetableSynth[i])
        {
            delete wavetableSynth[i];
            wavetableSynth[i] = nullptr;
        }
        
        if (chordManager[i])
        {
            delete chordManager[i];
            chordManager[i] = nullptr;
        }
    }
    
    if (repeater)
    {
        delete repeater;
        repeater = nullptr;
    }
}

void ChordPlayerProcessor::prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock)
{
    for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
    {
        synth[i]            ->prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);
        sampler[i]          ->setCurrentPlaybackSampleRate(sampleRate);
        wavetableSynth[i]   ->prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);
    }
    
    outputBuffer.clear();
    
    outputBuffer.setSize(1, maximumExpectedSamplesPerBlock);
    
    repeater->prepareToPlay(sampleRate);
}

void ChordPlayerProcessor::setParameter(int synthRef, int index, var newValue)
{
    if (index >= INSTRUMENT_TYPE && index <= ENV_RELEASE)
    {
        synth[synthRef]         ->setParameter(index, newValue);
        sampler[synthRef]       ->setParameter(index, newValue);
        wavetableSynth[synthRef]->setParameter(index, newValue);
    }
    else if (index == WAVEFORM_TYPE)
    {
        panic();
        
        waveformType[synthRef] = (WAVEFORM_TYPES)newValue.operator int();
        
        synth[synthRef]->setParameter(index, newValue);
//        sampler[synthRef]->setParameter(index, newValue);
    }
    
    else if (index == CHORD_SOURCE)
    {
        if ((bool)newValue != chordSource[synthRef])
        {
            chordSource[synthRef] = !chordSource[synthRef];
        }
    }
    else if (index == INSERT_FREQUENCY)
    {
        insertFrequency[synthRef] = (float)newValue;
        
        if (!chordSource[synthRef])
        {
            // need to face oscillator to override note frequency
            // do chord finder thing.
        }
    }
    else if (index == OCTAVE)
    {
        octaveShift[synthRef] = (float)newValue + 1;
    }
    else if (index == MANIPULATE_CHOSEN_FREQUENCY)
    {
        manipulateChoseFrequency[synthRef] = (bool)newValue;
    }
    else if (index == MULTIPLY_OR_DIVISION)
    {
        multiplyOrDivision[synthRef] = (bool)newValue;
    }
    else if (index == MULTIPLY_VALUE)
    {
        multiplyValue[synthRef] = (float) newValue;
    }
    else if (index == DIVISION_VALUE)
    {
        divisionValue[synthRef] = (float) newValue;
    }
    else if (index == SHORTCUT_IS_ACTIVE)
    {
        isActive[synthRef] = newValue.operator bool();
        repeater->setIsActive(synthRef, isActive[synthRef]);
    }
    else if (index == SHORTCUT_MUTE)
    {
        shouldMute[synthRef] = newValue.operator bool();
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
        
        chordManager[synthRef]->setKeyNote(keyNote);
    }
    else if (index == CHORD_TYPE)
    {
        panic();
        
        CHORD_TYPES type = (CHORD_TYPES)newValue.operator int();
        
        chordManager[synthRef]->setChordType(type);
    }
    else if (index == ADD_ONS)
    {
        panic();
        
        enum ADD_ONS addOnType = (enum ADD_ONS)newValue.operator int();
        chordManager[synthRef]->setAddOn(addOnType);
    }
    else if (index == CUSTOM_CHORD)
    {
        panic();
        
        hasCustomChord[synthRef] = newValue.operator bool();
        chordManager[synthRef]->setCustomChord(hasCustomChord[synthRef]);
    }
    else if (index >= CUSTOM_CHORD_ACTIVE_1 && index <= CUSTOM_CHORD_ACTIVE_12)
    {
        panic();
        
        int pos = index - CUSTOM_CHORD_ACTIVE_1;
        chordManager[synthRef]->setActiveToCustomChord(pos, newValue.operator bool());
    }
    else if (index >= CUSTOM_CHORD_NOTE_1 && index <= CUSTOM_CHORD_NOTE_12)
    {
        panic();
        
        int pos = index - CUSTOM_CHORD_NOTE_1;
        chordManager[synthRef]->setNoteToCustomChord(pos, newValue.operator int());
    }
    else if (index >= CUSTOM_CHORD_OCTAVE_1 && index <= CUSTOM_CHORD_OCTAVE_12)
    {
        int pos = index - CUSTOM_CHORD_OCTAVE_1;
        chordManager[synthRef]->setOctaveToCustomChord(pos, newValue.operator int());
    }
    else if (index == CHORDPLAYER_SCALE)
    {
        panic();
        
        frequencyManager->setChordPlayerScale(synthRef, newValue.operator int());
    }
    else if (index == CHORD_PLAYER_OUTPUT_SELECTION)
    {
        output[synthRef] = (AUDIO_OUTPUTS)newValue.operator int();
    }
}

void ChordPlayerProcessor::setOversamplingFactor(int newFactor)
{
    // ** NOT WORKING YET
    suspendProcessing(true);
    
    for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
    {
//        synth[i]->setOversamplingFactor(newFactor);
    }
    
    suspendProcessing(false);
}

void ChordPlayerProcessor::processBlock (AudioBuffer<float>& buffer,
                           MidiBuffer& midiMessages)
{
    
    
    
    if (playState == PLAY_STATE::PLAYING)
    {
        repeater->tickBuffer(buffer.getNumSamples());
    }
    
    for (int s = 0; s < NUM_SHORTCUT_SYNTHS; s++)
    {
        if (isActive[s] && !shouldMute[s])
        {
            outputBuffer.clear();
            
            if (output[s] != AUDIO_OUTPUTS::NO_OUTPUT)
            {
                // Use per-voice processors for continuity and realism
                if (waveformType[s] == SAMPLER)
                {
                    wavetableSynth[s]->processBlock(outputBuffer, midiMessages);
                }
                else if (waveformType[s] == WAVETABLE)
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
void ChordPlayerProcessor::triggerKeyDown(int shortcutRef)
{
    triggerNoteOn(shortcutRef);
}

void ChordPlayerProcessor::triggerKeyUp(int shortcutRef)
{
    triggerNoteOff(shortcutRef);
}

void ChordPlayerProcessor::triggerNoteOn(int shortcutRef)
{
    // get chord notes from parameters
    Array<int>notes(chordManager[shortcutRef]->getMIDIKeysForChord());

    Array<String> noteStrings;
    Array<float> noteFreqs;
    
    for (int i = 0; i < notes.size(); i++)
    {
        int midiNote = notes.getReference(i);
        String noteString = ProjectStrings::getKeynoteArray().getReference(midiNote % 12);
        
        if (midiNote >= 0 && midiNote <= 127)
        {
            double baseFreq = frequencyManager->scalesManager->getFrequencyForMIDINoteShortcut(midiNote, shortcutRef);
            double freq     = baseFreq;
            
            double octShift = 0.f;
            
            if (!hasCustomChord[shortcutRef])
            {
                octShift = octaveShift[shortcutRef];
                noteString.append(String{(int)projectManager->getChordPlayerParameter(i, OCTAVE)}, 2);
            }
            else
            {
                noteString.append(String{std::floor(midiNote / 12.0) - 1}, 2);
            }

            if (manipulateChoseFrequency[shortcutRef])
            {
                if (!multiplyOrDivision[shortcutRef])
                {
                    freq = baseFreq * pow(2.0, octShift) * multiplyValue[shortcutRef];
                }
                else
                {
                    freq = baseFreq * pow(2.0, octShift) / divisionValue[shortcutRef];
                }
            }
            else
            {
//                freq = baseFreq * pow(2.0, octaveShift[shortcutRef]);
                freq = baseFreq * pow(2.0, octShift);
            }

            noteStrings.add(noteString);
            noteFreqs.add(freq);

            if (freq) // check freq does not = 0hz
            {
                if (waveformType[shortcutRef] == SAMPLER)
                {
                    // Use synthesis instead of samples for playing instruments
                    wavetableSynth[shortcutRef]->noteOn(0, midiNote, freq);
                }
                else if (waveformType[shortcutRef] == WAVETABLE)
                {
                    wavetableSynth[shortcutRef]->noteOn(0, midiNote, freq);
                }
                else
                {
                    synth[shortcutRef]->noteOn(0, midiNote, freq);
                }
            }
        }
    }
    
    //** Crash ***
    // Crashes because note on is pushed at 0hz.. baseFreq returns from Scales Manager as 0, because note is not part of scale..
    // possible solutions are, simply dont trigger a note on when freq is 0.. should do this anyway
    // but really need not to offer the chord to the user

    if (notes.size() > 0)
        projectManager->logFileWriter->processLog_ChordPlayer_Sequencer(shortcutRef, noteStrings, noteFreqs);

}

void ChordPlayerProcessor::triggerNoteOff(int shortcutRef)
{
    Array<int> notes = chordManager[shortcutRef]->getMIDIKeysForChord();
    
    for (int i = 0; i < notes.size(); i++)
    {
        int midiNote = notes.getReference(i);
        
        if (midiNote >= 0 && midiNote <= 127)
        {
            if (waveformType[shortcutRef] == SAMPLER)
            {
                // Use synthesis instead of samples for playing instruments
                wavetableSynth[shortcutRef]->noteOff(0, midiNote, 1.0, true);
            }
            else if (waveformType[shortcutRef] == WAVETABLE)
            {
                wavetableSynth[shortcutRef]->noteOff(0, midiNote, 1.0, true);
            }
            else
            {
                synth[shortcutRef]->noteOff(0, midiNote, 1.0, true);
            }
        }
    }
}

void ChordPlayerProcessor::panic()
{
    for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++)
    {
        synth[i]    ->allNotesOff(0, true);
        sampler[i]  ->allNotesOff(0, true);
        wavetableSynth[i]->allNotesOff(0, true);
    }
}

void ChordPlayerProcessor::setActiveShortcutSynth(int synthRef, bool shouldBeActive)
{
    isActive[synthRef] = shouldBeActive;
}

void ChordPlayerProcessor::setPlayerCommand(PLAYER_COMMANDS command)
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

void ChordPlayerProcessor::setPlayerPlayMode(PLAY_MODE mode)
{
    playMode = mode;
    
    repeater->setPlayMode(mode);
}

// PlayRepeater implementation now lives in PlayRepeaterBase.h
// ChordPlayerProcessor::PlayRepeater only overrides the virtual hooks (defined inline in header)
// NEW: Helper methods for high-quality synthesis engine integration
String ChordPlayerProcessor::getCurrentInstrumentName(int shortcutRef)
{
    // Get current instrument type from project manager parameters (INSTRUMENTS enum)
    int instrumentType = projectManager->getChordPlayerParameter(shortcutRef, INSTRUMENT_TYPE);
    switch (instrumentType)
    {
        case 1: return "Grand Piano";      // PIANO
        case 4: return "Acoustic Guitar";  // GUITAR
        case 5: return "Harp";             // HARP
        case 7: return "Strings";          // STRINGS
        case 3: return "Flute";            // FLUTE
        default: return "Grand Piano";
    }
}

SynthesisType ChordPlayerProcessor::getSynthesisTypeForInstrument(const String& instrumentName)
{
    // Map supported instruments to synthesis types
    if (instrumentName == "Grand Piano")
        return SynthesisType::PHYSICAL_MODELING_PIANO;
    else if (instrumentName == "Acoustic Guitar")
        return SynthesisType::KARPLUS_STRONG_GUITAR;
    else if (instrumentName == "Harp")
        return SynthesisType::KARPLUS_STRONG_HARP;
    else if (instrumentName == "Strings")
        return SynthesisType::PHYSICAL_MODELING_STRINGS;
    else if (instrumentName == "Flute")
        return SynthesisType::WAVETABLE_SYNTH; // simple wavetable flute
    else
        return SynthesisType::PHYSICAL_MODELING_PIANO;
}

void ChordPlayerProcessor::generateSynthesisAudio(AudioBuffer<float>& buffer, int shortcutRef, SynthesisType synthType)
{
    if (!synthesisEngine) return;
    
    // Get active chord notes for this shortcut
    Array<int> notes = chordManager[shortcutRef]->getMIDIKeysForChord();
    
    if (notes.size() == 0) return; // No active chord
    
    buffer.clear();
    
    for (int i = 0; i < notes.size(); ++i)
    {
        int midiNote = notes[i];
        if (midiNote >= 0 && midiNote <= 127)
        {
            // Calculate frequency for this note
            double frequency = frequencyManager->scalesManager->getFrequencyForMIDINoteShortcut(midiNote, shortcutRef);
            
            if (frequency > 0) // Valid frequency
            {
                // Apply octave shift and manipulation
                double octShift = octaveShift[shortcutRef];
                if (manipulateChoseFrequency[shortcutRef])
                {
                    if (!multiplyOrDivision[shortcutRef])
                        frequency = frequency * pow(2.0, octShift) * multiplyValue[shortcutRef];
                    else
                        frequency = frequency * pow(2.0, octShift) / divisionValue[shortcutRef];
                }
                else
                {
                    frequency = frequency * pow(2.0, octShift);
                }
                
                // Generate audio using high-quality synthesis engine
                AudioBuffer<float> noteBuffer = synthesisEngine->generateInstrument(
                    synthType, 
                    (float)frequency, 
                    0.7f, // velocity 
                    buffer.getNumSamples()
                );
                
                // Mix this note into the output buffer
                if (noteBuffer.getNumChannels() > 0 && noteBuffer.getNumSamples() >= buffer.getNumSamples())
                {
                    for (int channel = 0; channel < std::min(buffer.getNumChannels(), noteBuffer.getNumChannels()); ++channel)
                    {
                        buffer.addFrom(channel, 0, noteBuffer, channel, 0, buffer.getNumSamples());
                    }
                }
            }
        }
    }
}
