/*
  ==============================================================================

    FrequencyManager.h
    Created: 17 Dec 2019 8:36:48pm
    Author:  Gary Jones

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "Parameters.h"
#include "Scales.h"


class ScalesManager
{
public:
    ScalesManager();
    ~ScalesManager();
    
    void    setBaseAFrequency(double newBaseA);
    double  getBaseAFrequency();
    void    setScale(int scale);
    void    setChordPlayerScale(int shortcut, int scale);
    void    setLissajousScale(int newScale);
    void    initialiseNoteFrequencies();    // all scales
    
    void getMIDINoteForFrequency(float frequency, int &midiNote, int & keynote, int & octave, float & frequencyDifference )
    {
        // scan main scale midiNote list for closest midinote / keynote
        
        int closestMidiNote = -1; float freqDif = 0.f;
        // first check if frequency is within range of hte midiNotes
        
        if (frequency <= mainScale->getNoteFrequencyFor(127) && frequency >= mainScale->getNoteFrequencyFor(0))
        {
            for (int i = 0; i < 127; i++)
            {
                if (frequency >= mainScale->getNoteFrequencyFor(i) && frequency <=  mainScale->getNoteFrequencyFor(i+1))
                {
                    closestMidiNote = i;
                    freqDif         = mainScale->getNoteFrequencyFor(i) - frequency;
                }
            }

        }
        else if (frequency > mainScale->getNoteFrequencyFor(127))
        {
            
        }
        
        midiNote            = closestMidiNote;
        keynote             = midiNote % 12;
        octave              = (midiNote - keynote) / 12;
        frequencyDifference = freqDif;
    }

    double  getFrequencyForMIDINote(int midiNote)
    {
        return  mainScale->getNoteFrequencyFor(midiNote);
    }
    
    double  getFrequencyForMIDINoteShortcut(int midiNote, int shortcut)
    {
        return  chordPlayerShortcutScale[shortcut]->getNoteFrequencyFor(midiNote);
    }
    
    double  getFrequencyForMIDINoteLissajous(int midiNote)
    {
        return  lissajousScale->getNoteFrequencyFor(midiNote);
    }
    
    void    getComboBoxPopupMenuForScales(PopupMenu & scalesPopupMenu);
    void    getComboBoxPopupMenuForChords(PopupMenu & chordsPopupMenu, SCALES_UNIT unit);
    void    getComboBoxPopupMenuForKeynotes(PopupMenu & keynotesPopupMenu, SCALES_UNIT unit);

private:
    
    void    changeMainScaleTo(SCALES newScale);
    void    changeChordPlayerScaleTo(int shortcut, SCALES newScale);
    void    changeLissajousScaleTo(SCALES newScale);
    
    double                          central_frequency;
    int                             octave;
    SCALES                          currentScale;
    
    ScalesBase *                    mainScale;
    ScalesBase *                    chordPlayerShortcutScale[NUM_SHORTCUT_SYNTHS]; // for non default scales for shortcuts..
    ScalesBase *                    lissajousScale;
    
    Diatonic_Pythagorean            diatonic_Pythagorean;
    Diatonic_JustIntonation         diatonic_JustItonation;
    Diatonic_IterationFifth         diatonic_IterationFifth;
    Chromatic_Pythagorean           chromatic_Pythagorean;
    Chromatic_JustItonation         chromatic_JustItonation;
    Chromatic_EqualTemperament      chromatic_EqualTemperament;
//    Harmonic_ET                     harmonic_EqualTemperament;
//    Harmonic_Modern                 harmonic_Modern;
    Harmonic_Simple                 harmonic_Simple;
    Enharmonic                      enharmonic;
    Solfeggio                       solfeggio;
    
    // have array of other scales for chord player / scanner, use defulat unless user overrides it
};



class FrequencyManager
{
public:
    FrequencyManager();
    ~FrequencyManager();
    
    void  setBaseAFrequency(double newBaseA);
    float getBaseAFrequency();
    float getFrequencyForBaseNote(KEYNOTES note); // called from ChordManager or other.. requests frequency of note from lower region. synth add octave shifts // returns from scalesManager, which in turn holds all scales and note frequencies..
    
    int gettMIDINoteFor(int note, int octave){ return 0; }
    double getFrequencyForMIDINote(int midiNote)
    {
        return scalesManager->getFrequencyForMIDINote(midiNote);
    }
    
    void getMIDINoteForFrequency(float frequency, int &midiNote, int & keynote, int & octave, float & frequencyDifference)
    {
        scalesManager->getMIDINoteForFrequency(frequency, midiNote, keynote, octave, frequencyDifference);
    }
    
    void setScale(int newScale)
    {
        scalesManager->setScale(newScale);
    }
    
    void setChordPlayerScale(int shortcut, int newScale)
    {
        scalesManager->setChordPlayerScale(shortcut, newScale);
    }
    
    void setLissajousPlayerScale(int newScale)
    {
        scalesManager->setLissajousScale(newScale);
    }
    
    ScalesManager * scalesManager;
    
private:
    double central_frequency = 432.0;

};

