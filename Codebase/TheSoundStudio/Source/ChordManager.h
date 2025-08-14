/*
  ==============================================================================

    ChordManager.h
    Created: 27 Mar 2019 6:19:55pm
    Author:  Gary Jones

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "Parameters.h"
#include "FrequencyManager.h"

class ChordManager
{
public:
    ChordManager(FrequencyManager * fm);
    ~ChordManager();
    
    void initCustomChordParams();
    void clearCustomChordParams();
    void calculateKeysInChord();
    
    void setCustomChord(bool is);
    void setNoteToCustomChord(int position, int note);
    void setOctaveToCustomChord(int position, int octave);
    void setActiveToCustomChord(int position, bool should);
    void setAddOn(enum ADD_ONS a);
    void setChordType(CHORD_TYPES newChordType);
    void setKeyNote(enum KEYNOTES newKeyNote);
    void setOctave(int newOctave);
    
    Array<double> getFrequenciesForChord();
    int getMIDINoteFor(KEYNOTES noteRef, int octave);
    Array<int> getMIDIKeysForChord();
    int getNumNotesInSelectedChord();
    
    void getKeynoteOctaveForLowestFrequency(float frequencyFrom, int &keynoteFrom, int &octaveFrom, int &midiNoteFromRef)
    {
        // scan midiNote list for closest frequency
        
        int closestNote = 0;
        
        for (int i = 0; i < 128; i++)
        {
            float noteFreq = frequencyManager->getFrequencyForMIDINote(i);
            
            if (noteFreq >= frequencyFrom)
            {
                closestNote = i; break;
            }
        }
        
        keynoteFrom         = (KEYNOTES) (closestNote % 12 ) + 1;
        octaveFrom          = ((closestNote+1) - keynoteFrom) / 12;
        midiNoteFromRef     = closestNote;
        
    }
    
    void getKeynoteOctaveForHighestFrequency(float frequencyTo, int &keynoteTo, int &octaveTo, int &midiNoteToRef)
    {
        int closestNote = 0;
        
        for (int i = 127; i > 0; i--)
        {
            float noteFreq = frequencyManager->getFrequencyForMIDINote(i);
            
            if (noteFreq <= frequencyTo)
            {
                closestNote = i; break;
            }
        }
        
        keynoteTo       = (KEYNOTES) (closestNote % 12 ) + 1;
        octaveTo        = ((closestNote+1) - keynoteTo) / 12;
        midiNoteToRef   = closestNote;
    }
    
    float getFrequencyForMIDINote(int midiNote)
    {
        return frequencyManager->getFrequencyForMIDINote(midiNote);
    }
    
    bool getIsCustomChord()
    {
        return isCustomChord;
    }
    
    
private:
    CHORD_TYPES chord_type;
    enum KEYNOTES  keynote;
    enum ADD_ONS addOns;
    int octave;
    int numNotesInChord = 0;
    bool chordIsReady;
    Array<int> midiKeysInChord;

    int customChordNotes[12];
    int customChordOctave[12];
    int customChordActive[12];
    
private:
    bool isCustomChord;
    
    FrequencyManager * frequencyManager;
    
};
