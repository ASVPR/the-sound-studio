/*
  ==============================================================================

    ScalesManager.h
    Created: 17 Dec 2019 8:36:56pm
    Author:  Gary Jones

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "Parameters.h"


class ScalesBase
{
public:
    ScalesBase(){}
    virtual ~ScalesBase(){}
    virtual void initFrequencies(){}
    
    void debugFrequenciesDiatonic()
    {
        printf("\n A4 = %f", noteFrequencies[69]);
        printf("\n B4 = %f", noteFrequencies[71]);
        printf("\n C4 = %f", noteFrequencies[60]);
        printf("\n D4 = %f", noteFrequencies[62]);
        printf("\n E4 = %f", noteFrequencies[64]);
        printf("\n F4 = %f", noteFrequencies[65]);
        printf("\n G4 = %f", noteFrequencies[67]);
        
    }
    
    void debugFrequenciesChromatic()
    {
        printf("\n A4 = %f", noteFrequencies[69]);
        printf("\n A#4 = %f", noteFrequencies[70]);
        printf("\n B4 = %f", noteFrequencies[71]);
        printf("\n C4 = %f", noteFrequencies[60]);
        printf("\n C#4 = %f", noteFrequencies[61]);
        printf("\n D4 = %f", noteFrequencies[62]);
        printf("\n D#4 = %f", noteFrequencies[63]);
        printf("\n E4 = %f", noteFrequencies[64]);
        printf("\n F4 = %f", noteFrequencies[65]);
        printf("\n F#4 = %f", noteFrequencies[66]);
        printf("\n G4 = %f", noteFrequencies[67]);
        printf("\n G#4 = %f", noteFrequencies[68]);
        
    }
    
    void setBaseAFrequency(double f)                { central_frequency = f; initFrequencies(); }
    Array<CHORD_TYPES> getAvailableChordTypes()     { return chordTypesArray; }
    Array<KEYNOTES> getAvailableKeynotes()          { return keynotesArray; }
    bool getDoesHaveInharmonics()                   { return doesHaveInharmonics; }
    double getNoteFrequencyFor(int keynote)         { return noteFrequencies[keynote]; }
    bool getNoteAvailable(int note)                 { return noteIsAvailable[note]; }
    
    SCALES thisScale;

protected:
    double              central_frequency;
    int                 numNotes;
    double              noteFrequencies[128];
    bool                noteIsAvailable[128];   // used to block off hidden notes in diatonic and solfeggio
    bool                doesHaveInharmonics;
    Array<CHORD_TYPES>  chordTypesArray;
    Array<KEYNOTES>     keynotesArray;
    
    void initNoteAvailables();
    
};


// Scales
class Diatonic_Pythagorean : public ScalesBase
{
public:
    
    Diatonic_Pythagorean();
    ~Diatonic_Pythagorean(){}
    
    virtual void initFrequencies() override;
};


class Diatonic_IterationFifth : public ScalesBase
{
public:
    Diatonic_IterationFifth();
    ~Diatonic_IterationFifth(){}
    
    virtual void initFrequencies() override;
};

class Diatonic_JustIntonation : public ScalesBase
{
public:
    
    Diatonic_JustIntonation();
    ~Diatonic_JustIntonation() { }
    
    virtual void initFrequencies() override;
};

class Chromatic_EqualTemperament : public ScalesBase
{
public:
    Chromatic_EqualTemperament();
    ~Chromatic_EqualTemperament() { }
    
    virtual void initFrequencies() override;
    
};

class Chromatic_Pythagorean : public ScalesBase
{
public:
    Chromatic_Pythagorean();
    ~Chromatic_Pythagorean() { }
    
    virtual void initFrequencies() override;
};

class Chromatic_JustItonation : public ScalesBase
{
public:
    Chromatic_JustItonation();
    ~Chromatic_JustItonation() { }
    
    virtual void initFrequencies() override;
};

//class Harmonic_ET : public ScalesBase
//{
//public:
//    Harmonic_ET();
//    ~Harmonic_ET() { }
//    
//    virtual void initFrequencies() override;
//};
//
//class Harmonic_Modern : public ScalesBase
//{
//public:
//    Harmonic_Modern();
//    ~Harmonic_Modern() { }
//    
//    virtual void initFrequencies() override;
//};

class Harmonic_Simple : public ScalesBase
{
public:
    Harmonic_Simple();
    ~Harmonic_Simple() { }
    
    virtual void initFrequencies() override;
};

class Enharmonic : public ScalesBase
{
public:
    Enharmonic();
    ~Enharmonic() { }
    
    virtual void initFrequencies() override;
};

class Solfeggio : public ScalesBase
{
public:
    Solfeggio();
    ~Solfeggio() { }
    
    virtual void initFrequencies() override;
};



// Further notes :
/*
 
 1. Solfeggio
 - wont work with chords, does not double over octaves, only chord will be cMajor, or individual notes via custom chord
 
 2. Harmonic
 - ET. strange sounding each note has a temperment of 1.75, 3/4 of an octave
 - Modern - 144 notes per octave ? not sure i understand this one
 - Simple
 
 How do this lot relate to standard notes and chords ? or should we disable chords/notes and only allow on custom chords..
 
 3. Enharmonic
 - up and down ? I can have 2 tables and we choose the flat/sharp basedo n the chord, but requires alot more logic..
 
 recommendations
 
 1. The infrasture is built, and it works mostly..but harmonic/enharmonic/solfeggio require more thought
 2. diatonic scales need to remove chord that dont fit
 3. for chromatic scales to fully work I think we need to define the root scale (A Major etc, then we can define the use of flats/sharps etc). this would require an extra parameter and more though on the overall system..
 4. still need to fix the sampler frequncy override function.. not too difficult..
 
 
 Next phase : adding Root Note for Scales.
 
 - All Scales are built from a root note.
 - This is demonstrated in musical notation when the stave defines the sharps/flats
 - The root note can be calculated from this, they usually have a term such as C Major scale
 - As such the flat/sharps of scales that have intonation change depending on the scale
 
 */



