/*
  ==============================================================================

    ChordManager.cpp
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#include "ChordManager.h"

ChordManager::ChordManager(FrequencyManager * fm)
{
    frequencyManager    = fm;
    
    keynote             = KEYNOTES::KEY_C;
    chord_type          = CHORD_TYPES::Major;
    octave              = 0;
    chordIsReady        = false;
    isCustomChord       = false;
    addOns              = ADD_ONS::AddOn_NONE;
    
    initCustomChordParams();
    
}

ChordManager::~ChordManager()
{
    
}

void ChordManager::setChordType(CHORD_TYPES newChordType)
{
    isCustomChord = false; clearCustomChordParams();
    
    chord_type = newChordType;
    
    calculateKeysInChord();
}

void ChordManager::setKeyNote(KEYNOTES newKeyNote)
{
    isCustomChord = false; clearCustomChordParams();
    
    keynote = newKeyNote;
    
    calculateKeysInChord();
}

void ChordManager::setOctave(int newOctave)
{
    octave = newOctave; calculateKeysInChord();
}

void ChordManager::setAddOn(enum ADD_ONS a)
{
    addOns = a; calculateKeysInChord();
}

void ChordManager::calculateKeysInChord()
{
    // called after any changes in the Chord Player settings
    chordIsReady = false;
    
    midiKeysInChord.clear();
    numNotesInChord =  midiKeysInChord.size();
    
    if (isCustomChord)
    {
        for (int i = 0; i < 12; i++)
        {
            if (customChordActive[i])
            {
                int note = getMIDINoteFor((KEYNOTES)customChordNotes[i], 0);
                
                note += (customChordOctave[i] * 12);
                
                midiKeysInChord.add(note);
            }
        }
    }
    else
    {
        switch (chord_type)
        {
            case Major:
            {
                // add 3 notes to
                int firstNote   = getMIDINoteFor(keynote, octave);
                midiKeysInChord.add(firstNote);
                int secondNote  = firstNote + 4;
                midiKeysInChord.add(secondNote);
                int thirdNote   = firstNote + 7;
                midiKeysInChord.add(thirdNote);
                
            }
                break;
            case Minor:
            {
                // add 3 notes to
                int firstNote   = getMIDINoteFor(keynote, octave); midiKeysInChord.add(firstNote);
                int secondNote  = firstNote + 3; midiKeysInChord.add(secondNote);
                int thirdNote   = firstNote + 7; midiKeysInChord.add(thirdNote);
                
            }
                break;
            case Augmented:
            {
                int firstNote   = getMIDINoteFor(keynote, octave); midiKeysInChord.add(firstNote);
                int secondNote  = firstNote + 4; midiKeysInChord.add(secondNote);
                int thirdNote   = firstNote + 8; midiKeysInChord.add(thirdNote);
                
            }
                break;
            case Diminished:
            {
                int firstNote   = getMIDINoteFor(keynote, octave); midiKeysInChord.add(firstNote);
                int secondNote  = firstNote + 3; midiKeysInChord.add(secondNote);
                int thirdNote   = firstNote + 6; midiKeysInChord.add(thirdNote);
                
            }
                break;
            case Suspended4th:
            {
                int firstNote   = getMIDINoteFor(keynote, octave); midiKeysInChord.add(firstNote);
                int secondNote  = firstNote + 5; midiKeysInChord.add(secondNote);
                int thirdNote   = firstNote + 7; midiKeysInChord.add(thirdNote);
                
            }
                break;
            case Suspended2nd:
            {
                int firstNote   = getMIDINoteFor(keynote, octave); midiKeysInChord.add(firstNote);
                int secondNote  = firstNote + 2; midiKeysInChord.add(secondNote);
                int thirdNote   = firstNote + 7; midiKeysInChord.add(thirdNote);
                
            }
                break;
            case Normal5th:
            {
                int firstNote   = getMIDINoteFor(keynote, octave); midiKeysInChord.add(firstNote);
                int secondNote  = firstNote + 7; midiKeysInChord.add(secondNote);
                
            }
                break;
            case Normal6th:
            {
                int firstNote   = getMIDINoteFor(keynote, octave); midiKeysInChord.add(firstNote);
                int secondNote  = firstNote + 4; midiKeysInChord.add(secondNote);
                int thirdNote   = firstNote + 7; midiKeysInChord.add(thirdNote);
                int fourthNote  = firstNote + 9; midiKeysInChord.add(fourthNote);
                
            }
                break;
            case Normal6thMinor:
            {
                int firstNote   = getMIDINoteFor(keynote, octave); midiKeysInChord.add(firstNote);
                int secondNote  = firstNote + 3; midiKeysInChord.add(secondNote);
                int thirdNote   = firstNote + 7; midiKeysInChord.add(thirdNote);
                int fourthNote  = firstNote + 9; midiKeysInChord.add(fourthNote);
                
            }
                break;
            case Normal7th:
            {
                int firstNote   = getMIDINoteFor(keynote, octave); midiKeysInChord.add(firstNote);
                int secondNote  = firstNote + 4; midiKeysInChord.add(secondNote);
                int thirdNote   = firstNote + 7; midiKeysInChord.add(thirdNote);
                int fourthNote  = firstNote + 10; midiKeysInChord.add(fourthNote);
                
            }
                break;
                
            case Normal7thMajor:
            {
                int firstNote   = getMIDINoteFor(keynote, octave); midiKeysInChord.add(firstNote);
                int secondNote  = firstNote + 4; midiKeysInChord.add(secondNote);
                int thirdNote   = firstNote + 7; midiKeysInChord.add(thirdNote);
                int fourthNote  = firstNote + 11; midiKeysInChord.add(fourthNote);
                
            }
                break;
            case Normal7thMinor:
            {
                int firstNote   = getMIDINoteFor(keynote, octave); midiKeysInChord.add(firstNote);
                int secondNote  = firstNote + 3; midiKeysInChord.add(secondNote);
                int thirdNote   = firstNote + 7; midiKeysInChord.add(thirdNote);
                int fourthNote  = firstNote + 10; midiKeysInChord.add(fourthNote);
                
            }
                break;
            case Normal7thDiminished:
            {
                int firstNote   = getMIDINoteFor(keynote, octave); midiKeysInChord.add(firstNote);
                int secondNote  = firstNote + 3; midiKeysInChord.add(secondNote);
                int thirdNote   = firstNote + 6; midiKeysInChord.add(thirdNote);
                int fourthNote  = firstNote + 10; midiKeysInChord.add(fourthNote);
                
            }
                break;
            case Normal7thHalfdiminished:
            {
                int firstNote   = getMIDINoteFor(keynote, octave); midiKeysInChord.add(firstNote);
                int secondNote  = firstNote + 3; midiKeysInChord.add(secondNote);
                int thirdNote   = firstNote + 6; midiKeysInChord.add(thirdNote);
                int fourthNote  = firstNote + 10; midiKeysInChord.add(fourthNote);
                
            }
                break;
            case Normal7th_5:
            {
                int firstNote   = getMIDINoteFor(keynote, octave); midiKeysInChord.add(firstNote);
                int secondNote  = firstNote + 4; midiKeysInChord.add(secondNote);
                int thirdNote   = firstNote + 8; midiKeysInChord.add(thirdNote);
                int fourthNote  = firstNote + 10; midiKeysInChord.add(fourthNote);
                
            }
                break;
            case Normal9th:
            {
                int firstNote   = getMIDINoteFor(keynote, octave); midiKeysInChord.add(firstNote);
                int secondNote  = firstNote + 4; midiKeysInChord.add(secondNote);
                int thirdNote   = firstNote + 7; midiKeysInChord.add(thirdNote);
                int fourthNote  = firstNote + 10; midiKeysInChord.add(fourthNote);
                int fifthNote   = firstNote + 14; midiKeysInChord.add(fifthNote);
                
            }
                break;
            case Normal7th_9:
            {
                int firstNote   = getMIDINoteFor(keynote, octave); midiKeysInChord.add(firstNote);
                int secondNote  = firstNote + 4; midiKeysInChord.add(secondNote);
                int thirdNote   = firstNote + 7; midiKeysInChord.add(thirdNote);
                int fourthNote  = firstNote + 10; midiKeysInChord.add(fourthNote);
                int fifthNote   = firstNote + 15; midiKeysInChord.add(fifthNote);
                
            }
                break;
            case Normal9thMajor:
            {
                int firstNote   = getMIDINoteFor(keynote, octave); midiKeysInChord.add(firstNote);
                int secondNote  = firstNote + 4; midiKeysInChord.add(secondNote);
                int thirdNote   = firstNote + 7; midiKeysInChord.add(thirdNote);
                int fourthNote  = firstNote + 11; midiKeysInChord.add(fourthNote);
                int fifthNote   = firstNote + 14; midiKeysInChord.add(fifthNote);
                
            }
                break;
            case Normal9thMinor:
            {
                int firstNote   = getMIDINoteFor(keynote, octave); midiKeysInChord.add(firstNote);
                int secondNote  = firstNote + 3; midiKeysInChord.add(secondNote);
                int thirdNote   = firstNote + 7; midiKeysInChord.add(thirdNote);
                int fourthNote  = firstNote + 10; midiKeysInChord.add(fourthNote);
                int fifthNote   = firstNote + 14; midiKeysInChord.add(fifthNote);
                
            }
                break;
            case Normal9thAdded:
            {
                int firstNote   = getMIDINoteFor(keynote, octave); midiKeysInChord.add(firstNote);
                int secondNote  = firstNote + 4; midiKeysInChord.add(secondNote);
                int thirdNote   = firstNote + 7; midiKeysInChord.add(thirdNote);
                int fourthNote  = firstNote + 14; midiKeysInChord.add(fourthNote);
                
            }
                break;
                
            case Normal9thMinorAdded:
            {
                int firstNote   = getMIDINoteFor(keynote, octave); midiKeysInChord.add(firstNote);
                int secondNote  = firstNote + 4; midiKeysInChord.add(secondNote);
                int thirdNote   = firstNote + 7; midiKeysInChord.add(thirdNote);
                int fourthNote  = firstNote + 14; midiKeysInChord.add(fourthNote);
                
            }
                break;
            case Normal11th:
            {
                int firstNote   = getMIDINoteFor(keynote, octave); midiKeysInChord.add(firstNote);
                int secondNote  = firstNote + 4; midiKeysInChord.add(secondNote);
                int thirdNote   = firstNote + 7; midiKeysInChord.add(thirdNote);
                int fourthNote  = firstNote + 10; midiKeysInChord.add(fourthNote);
                int fifthNote   = firstNote + 14; midiKeysInChord.add(fifthNote);
                int sixthNote   = firstNote + 17; midiKeysInChord.add(sixthNote);
                
            }
                break;
            case Normal11thMinor:
            {
                int firstNote   = getMIDINoteFor(keynote, octave); midiKeysInChord.add(firstNote);
                int secondNote  = firstNote + 3; midiKeysInChord.add(secondNote);
                int thirdNote   = firstNote + 7; midiKeysInChord.add(thirdNote);
                int fourthNote  = firstNote + 10; midiKeysInChord.add(fourthNote);
                int fifthNote   = firstNote + 14; midiKeysInChord.add(fifthNote);
                int sixthNote   = firstNote + 17; midiKeysInChord.add(sixthNote);
                
            }
                break;
            case Normal7th_11:
            {
                int firstNote   = getMIDINoteFor(keynote, octave); midiKeysInChord.add(firstNote);
                int secondNote  = firstNote + 4; midiKeysInChord.add(secondNote);
                int thirdNote   = firstNote + 7; midiKeysInChord.add(thirdNote);
                int fourthNote  = firstNote + 10; midiKeysInChord.add(fourthNote);
                int fifthNote   = firstNote + 14; midiKeysInChord.add(fifthNote);
                int sixthNote   = firstNote + 18; midiKeysInChord.add(sixthNote);
                
            }
                break;
            case Major7th_11:
            {
                int firstNote   = getMIDINoteFor(keynote, octave); midiKeysInChord.add(firstNote);
                int secondNote  = firstNote + 4; midiKeysInChord.add(secondNote);
                int thirdNote   = firstNote + 7; midiKeysInChord.add(thirdNote);
                int fourthNote  = firstNote + 11; midiKeysInChord.add(fourthNote);
                int fifthNote   = firstNote + 14; midiKeysInChord.add(fifthNote);
                int sixthNote   = firstNote + 18; midiKeysInChord.add(sixthNote);
                
            }
                break;
            case Normal13th:
            {
                // might be error on 4th note **** //
                int firstNote   = getMIDINoteFor(keynote, octave); midiKeysInChord.add(firstNote);
                int secondNote  = firstNote + 4; midiKeysInChord.add(secondNote);
                int thirdNote   = firstNote + 7; midiKeysInChord.add(thirdNote);
                int fourthNote  = firstNote + 11; midiKeysInChord.add(fourthNote);
                int fifthNote   = firstNote + 14; midiKeysInChord.add(fifthNote);
                int sixthNote   = firstNote + 17; midiKeysInChord.add(sixthNote);
                int seventhNote = firstNote + 21; midiKeysInChord.add(seventhNote);
                
            }
                break;
            case Major13th:
            {
                int firstNote   = getMIDINoteFor(keynote, octave); midiKeysInChord.add(firstNote);
                int secondNote  = firstNote + 4; midiKeysInChord.add(secondNote);
                int thirdNote   = firstNote + 7; midiKeysInChord.add(thirdNote);
                int fourthNote  = firstNote + 11; midiKeysInChord.add(fourthNote);
                int fifthNote   = firstNote + 14; midiKeysInChord.add(fifthNote);
                int sixthNote   = firstNote + 17; midiKeysInChord.add(sixthNote);
                int seventhNote = firstNote + 21; midiKeysInChord.add(seventhNote);
                
            }
                break;
            case Minor13th:
            {
                int firstNote   = getMIDINoteFor(keynote, octave); midiKeysInChord.add(firstNote);
                int secondNote  = firstNote + 3; midiKeysInChord.add(secondNote);
                int thirdNote   = firstNote + 7; midiKeysInChord.add(thirdNote);
                int fourthNote  = firstNote + 11; midiKeysInChord.add(fourthNote);
                int fifthNote   = firstNote + 14; midiKeysInChord.add(fifthNote);
                int sixthNote   = firstNote + 17; midiKeysInChord.add(sixthNote);
                int seventhNote = firstNote + 21; midiKeysInChord.add(seventhNote);
                
            }
                break;
                
            default: break;
        }
    }

    // addOns
    if (addOns != AddOn_NONE)
    {
        switch (addOns)
        {
            case AddOn_6:
            {
                int addOnNote  = getMIDINoteFor(keynote, octave) + 9;
                midiKeysInChord.add(addOnNote);

            }
                break;
                
            case AddOn_7:
            {
                int addOnNote  = getMIDINoteFor(keynote, octave) + 10;
                midiKeysInChord.add(addOnNote);
                
            }
                break;
                
            case AddOn_7Major:
            {
                int addOnNote  = getMIDINoteFor(keynote, octave) + 11;
                midiKeysInChord.add(addOnNote);
                
            }
                break;
                
            case AddOn_9:
            {
                int addOnNote  = getMIDINoteFor(keynote, octave) + 14;
                midiKeysInChord.add(addOnNote);
                
            }
                break;
                
            case AddOn_9flat:
            {
                int addOnNote  = getMIDINoteFor(keynote, octave) + 13;
                midiKeysInChord.add(addOnNote);
                
            }
                break;
                
            case AddOn_9sharp:
            {
                int addOnNote  = getMIDINoteFor(keynote, octave) + 15;
                midiKeysInChord.add(addOnNote);
                
            }
                break;
                
            case AddOn_11:
            {
                int addOnNote  = getMIDINoteFor(keynote, octave) + 17;
                midiKeysInChord.add(addOnNote);
                
            }
                break;
                
            case AddOn_11sharp:
            {
                int addOnNote  = getMIDINoteFor(keynote, octave) + 18;
                midiKeysInChord.add(addOnNote);
                
            }
                break;
                
            case AddOn_13:
            {
                int addOnNote  = getMIDINoteFor(keynote, octave) + 21;
                midiKeysInChord.add(addOnNote);
                
            }
                break;
                
            case AddOn_13flat:
            {
                int addOnNote  = getMIDINoteFor(keynote, octave) + 20;
                midiKeysInChord.add(addOnNote);
                
            }
                break;
                
            case AddOn_Inverted:
            {
                    // not sure....
                
            }
                break;
                
            default: break;
        }
        
    }
    
    numNotesInChord = midiKeysInChord.size();
    
    chordIsReady = true;
}

int ChordManager::getMIDINoteFor(KEYNOTES key, int octave)
{
//    noteRef-=1;
    int result = 0;
    
    result = (int)key-1;

    return result;

}




void ChordManager::initCustomChordParams()
{
    for (int i = 0; i < 12; i++)
    {
        customChordNotes[i]     = 0;;
        customChordOctave[i]    = 0;
        customChordActive[i]    = false;
    }
}

Array<int> ChordManager::getMIDIKeysForChord()
{
    return midiKeysInChord;
}

int ChordManager::getNumNotesInSelectedChord()
{
    return numNotesInChord;
}

void ChordManager::clearCustomChordParams()
{
    initCustomChordParams();
}


void ChordManager::setCustomChord(bool is)
{

    isCustomChord = is;
    
    if (isCustomChord)
    {
         calculateKeysInChord();
    }
    else
    {
        clearCustomChordParams();
        calculateKeysInChord();
    }
   
}

void ChordManager::setNoteToCustomChord(int position, int note)
{
    customChordNotes[position] = note;
//    calculateKeysInChord();
}

void ChordManager::setOctaveToCustomChord(int position, int octave)
{
    customChordOctave[position] = octave;
//    calculateKeysInChord();
}

void ChordManager::setActiveToCustomChord(int position, bool should)
{
    customChordActive[position] = should;
//    calculateKeysInChord();
}

Array<double> ChordManager::getFrequenciesForChord()
{
    Array<double> frequencies;

    for (int num = 0; num < getNumNotesInSelectedChord(); num++)
    {
        frequencies.add(frequencyManager->scalesManager->getFrequencyForMIDINote(midiKeysInChord[num]));
    }
    
    return frequencies;
    
}
