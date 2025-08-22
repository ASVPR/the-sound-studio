/*
  ==============================================================================

    ScalesManager.cpp
    Created: 17 Dec 2019 8:36:56pm
    Author:  Gary Jones

  ==============================================================================
*/

#include "Scales.h"

void ScalesBase::initNoteAvailables()
{
    switch (thisScale)
    {
        case SCALES::DIATONIC_PYTHAGOREAN:
        case SCALES::DIATONIC_JUSTLY_TUNED:
        case SCALES::DIATONIC_ITERATION_FIFTH:
        {
            for (int i = 0; i < 128; i++) {  noteIsAvailable[i] = false; }
            
            for (int oct = 0; oct < 10; oct ++)
            {
                int base = oct * 12;
                
                noteIsAvailable[base + 0] = true;   // C
                noteIsAvailable[base + 2] = true;   // D
                noteIsAvailable[base + 4] = true;   // E
                noteIsAvailable[base + 5] = true;   // F
                noteIsAvailable[base + 7] = true;   // G
                noteIsAvailable[base + 9] = true;   // A
                noteIsAvailable[base + 11] = true;  // B
            }
            
            noteIsAvailable[120] = true;
            noteIsAvailable[122] = true;
            noteIsAvailable[124] = true;
            noteIsAvailable[125] = true;
            noteIsAvailable[127] = true;
        }
            
            break;
            
        case SCALES::CHROMATIC_ET:
        case SCALES::CHROMATIC_PYTHAGOREAN:
        case SCALES::CHROMATIC_JUST_INTONATION:
        {
            for (int i = 0; i < 128; i++) { noteIsAvailable[i] = true; }
        }
            break;
        
        case SCALES::ENHARMONIC:
        {
             for (int i = 0; i < 128; i++) { noteIsAvailable[i] = true; }
        }
            
            break;
            
//        case SCALES::HARMONIC_ET:
        case SCALES::HARMONIC_SIMPLE:
//        case SCALES::HARMONIC_MODERN:
        {
             for (int i = 0; i < 128; i++) { noteIsAvailable[i] = true; }
        }
            
            break;
            
        case SCALES::SOLFEGGIO:
        {
            for (int i = 0; i < 128; i++) {  noteIsAvailable[i] = false; }
            
            for (int oct = 0; oct < 10; oct ++)
            {
                int base = oct * 12;
                
                noteIsAvailable[base + 0] = true;   // C
                noteIsAvailable[base + 2] = true;   // D
                noteIsAvailable[base + 4] = true;   // E
                noteIsAvailable[base + 5] = true;   // F
                noteIsAvailable[base + 7] = true;   // G
                noteIsAvailable[base + 9] = true;   // A
            }
            
            noteIsAvailable[120] = true;
            noteIsAvailable[122] = true;
            noteIsAvailable[124] = true;
            noteIsAvailable[125] = true;
            noteIsAvailable[127] = true;
        }
            
            break;
            
        default: break;
    }

}


Diatonic_Pythagorean::Diatonic_Pythagorean()
{
    thisScale           = SCALES::DIATONIC_PYTHAGOREAN;
    doesHaveInharmonics = false;
    
    // add compatible keynotes
    keynotesArray.add(KEY_C);
    keynotesArray.add(KEY_D);
    keynotesArray.add(KEY_E);
    keynotesArray.add(KEY_F);
    keynotesArray.add(KEY_G);
    keynotesArray.add(KEY_A);
    keynotesArray.add(KEY_B);
    
    // compatible chord types..
    for (int type = 1; type < NUM_CHORD_TYPES; type++)
    {
        if (type != Augmented || type != Normal7th_5 || type != Normal7th_9 || type != Normal7th_11
            || type != Major7th_11)
        {
            chordTypesArray.add((CHORD_TYPES)type);
        }
    }

    
    initNoteAvailables();
    
}

void Diatonic_Pythagorean::initFrequencies()
{
    for (int i = 0 ; i < 128; i++)
    {
        noteFrequencies[i] = 0.f;
    }
    
    double AMinus1 = central_frequency / 32.f;
    double CMinus1 = AMinus1 / (27.f / 16.f);
    
    noteFrequencies[0] = CMinus1;                                   // C
    noteFrequencies[2] = CMinus1 * ( 9.f / 8.f);                    // D
    noteFrequencies[4] = CMinus1 * ( 81.f / 64.f);                  // E
    noteFrequencies[5] = CMinus1 * ( 4.f / 3.f);                    // F
    noteFrequencies[7] = CMinus1 * ( 3.f / 2.f);                    // G
    noteFrequencies[9] = CMinus1 * ( 27.f / 16.f);                  // A
    noteFrequencies[11] = CMinus1 * ( 243.f / 128.f);               // B

    for (int i = 12; i < 128; i++)
    {
        noteFrequencies[i] = noteFrequencies[i-12] * 2.f;
    }
    
//    debugFrequenciesDiatonic();
    
}

Diatonic_IterationFifth::Diatonic_IterationFifth()
{
    thisScale           = SCALES::DIATONIC_ITERATION_FIFTH;
    doesHaveInharmonics = false;
    
    // add compatible keynotes

    keynotesArray.add(KEY_C);
    keynotesArray.add(KEY_D);
    keynotesArray.add(KEY_E);
    keynotesArray.add(KEY_F);
    keynotesArray.add(KEY_G);
    keynotesArray.add(KEY_A);
    keynotesArray.add(KEY_B);
    
    // add compatible chord types
    // compatible chord types..
    for (int type = 1; type < NUM_CHORD_TYPES; type++)
    {
        if (type != Augmented || type != Normal7th_5 || type != Normal7th_9 || type != Normal7th_11
            || type != Major7th_11)
        {
            chordTypesArray.add((CHORD_TYPES)type);
        }
    }
    
    initNoteAvailables();
    
}

void Diatonic_IterationFifth::initFrequencies()
{
    // start at A0 frequency and build from there...
    // user can opt for upto -2 octaves, which will reduce the frequency by half each unit.. , maybe more even
    // for diatonic gui offer C to B, then shift octave based on
    for (int i = 0 ; i < 128; i++) { noteFrequencies[i] = 0.f; }
    
    double A0 = central_frequency / 16.f;
    double AMinus1 = A0 / 2.f;
    double CMinus1 = AMinus1 / ( 27.f / 16.f );
    double DMinus1 = CMinus1 * (9.f / 8.f);
    double EMinus1 = CMinus1 * (81.f / 64.f);
    double FMinus1 = CMinus1 * (4.f / 3.f);
    double GMinus1 = CMinus1 * (3.f / 2.f);
    double BMinus1 = CMinus1 * (243.f / 128.f);
    
    
    
    noteFrequencies[0]   = CMinus1;      // C
    noteFrequencies[2]   = DMinus1;      // D
    noteFrequencies[4]   = EMinus1;      // E
    noteFrequencies[5]   = FMinus1;      // F
    noteFrequencies[7]   = GMinus1;      // G
    noteFrequencies[9]   = AMinus1;      // A
    noteFrequencies[11]  = BMinus1;      // B
    
    for (int i = 12; i < 128; i++)
    {
        noteFrequencies[i] = noteFrequencies[i-12] * 2.f;
    }
    
//    debugFrequenciesDiatonic();
}


Diatonic_JustIntonation::Diatonic_JustIntonation()
{
    thisScale           = SCALES::DIATONIC_JUSTLY_TUNED;
    doesHaveInharmonics = false;
    
    // add compatible keynotes
    // add compatible keynotes
    keynotesArray.add(KEY_C);
    keynotesArray.add(KEY_D);
    keynotesArray.add(KEY_E);
    keynotesArray.add(KEY_F);
    keynotesArray.add(KEY_G);
    keynotesArray.add(KEY_A);
    keynotesArray.add(KEY_B);
    
    // add compatible chord types
    for (int type = 1; type < NUM_CHORD_TYPES; type++)
    {
        if (type != Augmented || type != Normal7th_5 || type != Normal7th_9 || type != Normal7th_11
            || type != Major7th_11)
        {
            chordTypesArray.add((CHORD_TYPES)type);
        }
    }
    
    initNoteAvailables();
}


void Diatonic_JustIntonation::initFrequencies()
{
    for (int i = 0 ; i < 128; i++) { noteFrequencies[i] = 0.f; }
    
    double A0 = central_frequency / 16.f;
    double AMinus1 = A0 / 2.f;
//    double CMinus1 = AMinus1 / ( 5.f / 3.f );
    double CMinus1 = AMinus1 * ( 16.f / 27.f );
    double DMinus1 = CMinus1 * (9.f / 8.f);
//    double EMinus1 = CMinus1 * (5.f / 4.f);
    double EMinus1 = CMinus1 * (81.f / 64.f);
    double FMinus1 = CMinus1 * (4.f / 3.f);
    double GMinus1 = CMinus1 * (3.f / 2.f);
//    double BMinus1 = CMinus1 * (15.f / 8.f);
    double BMinus1 = CMinus1 * (234.f / 128.f);
    
    noteFrequencies[0]   = CMinus1;      // C
    noteFrequencies[2]   = DMinus1;      // D
    noteFrequencies[4]   = EMinus1;      // E
    noteFrequencies[5]   = FMinus1;      // F
    noteFrequencies[7]   = GMinus1;      // G
    noteFrequencies[9]   = AMinus1;      // A
    noteFrequencies[11]  = BMinus1;      // B
    
    for (int i = 12; i < 128; i++)
    {
        noteFrequencies[i] = noteFrequencies[i-12] * 2.f;
    }
    
//    debugFrequenciesDiatonic();
}


Chromatic_EqualTemperament::Chromatic_EqualTemperament()
{
    thisScale           = SCALES::CHROMATIC_ET;
    doesHaveInharmonics = false;
    
    // add compatible keynotes
    // add compatible keynotes
    keynotesArray.add(KEY_C);
    keynotesArray.add(KEY_C_SHARP);
    keynotesArray.add(KEY_D);
    keynotesArray.add(KEY_D_SHARP);
    keynotesArray.add(KEY_E);
    keynotesArray.add(KEY_F);
    keynotesArray.add(KEY_F_SHARP);
    keynotesArray.add(KEY_G);
    keynotesArray.add(KEY_G_SHARP);
    keynotesArray.add(KEY_A);
    keynotesArray.add(KEY_A_SHARP);
    keynotesArray.add(KEY_B);
    
    // add compatible chord types
    for (int type = 1; type < NUM_CHORD_TYPES; type++)
    {
        chordTypesArray.add((CHORD_TYPES)type);
    }
    
    initNoteAvailables();
}


void Chromatic_EqualTemperament::initFrequencies()
{
//    double AMinus1 = central_frequency / 32.f;
//
//    double CMinus1 = AMinus1 / pow(2.0, 9 / 12.0);
    
    
    double CMinus1 = central_frequency / pow(2.0, 69 / 12.0);

    noteFrequencies[0] = CMinus1;

    for (int i = 1; i < 128; i++) // 3 octaves + 4 semitones from A0...
    {
//        noteFrequencies[i] = noteFrequencies[i - 1] * pow(2.0, 1 / 12.0);
        
        noteFrequencies[i] = noteFrequencies[0] * pow(2.0, i / 12.0);
    }
    
//    debugFrequenciesChromatic();
}


Chromatic_Pythagorean::Chromatic_Pythagorean()
{
    thisScale           = SCALES::CHROMATIC_PYTHAGOREAN;
    doesHaveInharmonics = false;
    
    // add compatible keynotes
    keynotesArray.add(KEY_C);
    keynotesArray.add(KEY_C_SHARP);
    keynotesArray.add(KEY_D);
    keynotesArray.add(KEY_D_SHARP);
    keynotesArray.add(KEY_E);
    keynotesArray.add(KEY_F);
    keynotesArray.add(KEY_F_SHARP);
    keynotesArray.add(KEY_G);
    keynotesArray.add(KEY_G_SHARP);
    keynotesArray.add(KEY_A);
    keynotesArray.add(KEY_A_SHARP);
    keynotesArray.add(KEY_B);
    
    // add compatible chord types
    for (int type = 1; type < NUM_CHORD_TYPES; type++)
    {
        chordTypesArray.add((CHORD_TYPES)type);
    }
    
    initNoteAvailables();
}

void Chromatic_Pythagorean::initFrequencies()
{
    for (int i = 0 ; i < 128; i++)
    {
        noteFrequencies[i] = 0.f;
    }
    
    // calc A0-E1-B1-F2-C3-G3

    double AMinus1 = central_frequency / 32.f;
    double CMinus1 = AMinus1 / (27.f / 16.f);
    
    noteFrequencies[0] = CMinus1;                                   // C
    noteFrequencies[1] = CMinus1 * ( 2187.f / 2048.f);              // C#
    noteFrequencies[2] = CMinus1 * ( 9.f / 8.f);                    // D
    noteFrequencies[3] = CMinus1 * ( 19683.f / 16384.f);            // D#
    noteFrequencies[4] = CMinus1 * ( 81.f / 64.f);                  // E
    noteFrequencies[5] = CMinus1 * ( 4.f / 3.f);                    // F
    noteFrequencies[6] = CMinus1 * ( 729.f / 512.f);                // F#
    noteFrequencies[7] = CMinus1 * ( 3.f / 2.f);                    // G
    noteFrequencies[8] = CMinus1 * ( 6561.f / 4096.f);              // G#
    noteFrequencies[9] = CMinus1 * ( 27.f / 16.f);                  // A
    noteFrequencies[10] = CMinus1 * ( 59049.f / 32768.f);           // A#
    noteFrequencies[11] = CMinus1 * ( 243.f / 128.f);               // B

    for (int i = 12; i < 128; i++)
    {
        noteFrequencies[i] = noteFrequencies[i-12] * 2.f;
    }
    
//    debugFrequenciesChromatic();
    
}


Chromatic_JustItonation::Chromatic_JustItonation()
{
    thisScale           = SCALES::CHROMATIC_JUST_INTONATION;
    doesHaveInharmonics = false;
    
    // add compatible keynotes
    keynotesArray.add(KEY_C);
    keynotesArray.add(KEY_C_SHARP);
    keynotesArray.add(KEY_D);
    keynotesArray.add(KEY_D_SHARP);
    keynotesArray.add(KEY_E);
    keynotesArray.add(KEY_F);
    keynotesArray.add(KEY_F_SHARP);
    keynotesArray.add(KEY_G);
    keynotesArray.add(KEY_G_SHARP);
    keynotesArray.add(KEY_A);
    keynotesArray.add(KEY_A_SHARP);
    keynotesArray.add(KEY_B);
    
    // add compatible chord types
    // compatible chord types..
    for (int type = 1; type < NUM_CHORD_TYPES; type++)
    {
        chordTypesArray.add((CHORD_TYPES)type);
    }
    
    initNoteAvailables();
}


void Chromatic_JustItonation::initFrequencies()
{
    // start at A0 frequency and build from there...
    // user can opt for upto -2 octaves, which will reduce the frequency by half each unit.. , maybe more even
    // for diatonic gui offer C to B, then shift octave based on
    
    for (int i = 0 ; i < 128; i++)
    {
        noteFrequencies[i] = 0.f;
    }
    
    // calc A0-E1-B1-F2-C3-G3

    double AMinus1 = central_frequency / 32.f;
    double CMinus1 = AMinus1 / (5.f / 3.f);
    
    noteFrequencies[0] = CMinus1;                                   // C
    noteFrequencies[1] = CMinus1 * ( 25.f / 24.f);                  // C#
    noteFrequencies[2] = CMinus1 * ( 9.f / 8.f);                    // D
    noteFrequencies[3] = CMinus1 * ( 75.f / 64.f);                  // D#
    noteFrequencies[4] = CMinus1 * ( 5.f / 4.f);                  // E
    noteFrequencies[5] = CMinus1 * ( 4.f / 3.f);                    // F
    noteFrequencies[6] = CMinus1 * ( 25.f / 18.f);                // F#
    noteFrequencies[7] = CMinus1 * ( 3.f / 2.f);                    // G
    noteFrequencies[8] = CMinus1 * ( 25.f / 16.f);              // G#
    noteFrequencies[9] = CMinus1 * ( 5.f / 3.f);                  // A
    noteFrequencies[10] = CMinus1 * ( 125.f / 72.f);           // A#
    noteFrequencies[11] = CMinus1 * ( 15.f / 8.f);               // B

    
    for (int i = 12; i < 128; i++)
    {
        noteFrequencies[i] = noteFrequencies[i-12] * 2.f;
    }
    
//    debugFrequenciesChromatic();
}

//Harmonic_ET::Harmonic_ET()
//{
//    thisScale           = SCALES::HARMONIC_ET;
//    doesHaveInharmonics = false;
//
//    // add compatible keynotes
//    keynotesArray.add(KEY_C);
//    keynotesArray.add(KEY_C_SHARP);
//    keynotesArray.add(KEY_D);
//    keynotesArray.add(KEY_D_SHARP);
//    keynotesArray.add(KEY_E);
//    keynotesArray.add(KEY_F);
//    keynotesArray.add(KEY_F_SHARP);
//    keynotesArray.add(KEY_G);
//    keynotesArray.add(KEY_G_SHARP);
//    keynotesArray.add(KEY_A);
//    keynotesArray.add(KEY_A_SHARP);
//    keynotesArray.add(KEY_B);
//
//    // add compatible chord types
//
//    // compatible chord types..
//    for (int type = 1; type < NUM_CHORD_TYPES; type++)
//    {
//        chordTypesArray.add((CHORD_TYPES)type);
//    }
//
//    initNoteAvailables();
//}
//
//void Harmonic_ET::initFrequencies()
//{
//    // shift central A down
//
//    for (int i = 0 ; i < 128; i++) { noteFrequencies[i] = 0.f; }
//
//    double AMinus1 = central_frequency / 32;              // A0
//    double CMinus1 = AMinus1 / (27.f / 16.f);
//
//    noteFrequencies[0] = CMinus1;
//
//    for (int i = 1; i < 127; i++)
//    {
//        noteFrequencies[i] = noteFrequencies[i - 1] * (7.f / 4.f);
//    }
//
////    debugFrequenciesChromatic();
//}


//Harmonic_Modern::Harmonic_Modern()
//{
//    thisScale           = SCALES::HARMONIC_ET;
//    doesHaveInharmonics = false;
//
//    // add compatible keynotes
//    keynotesArray.add(KEY_C);
//    keynotesArray.add(KEY_C_SHARP);
//    keynotesArray.add(KEY_D);
//    keynotesArray.add(KEY_D_SHARP);
//    keynotesArray.add(KEY_E);
//    keynotesArray.add(KEY_F);
//    keynotesArray.add(KEY_F_SHARP);
//    keynotesArray.add(KEY_G);
//    keynotesArray.add(KEY_G_SHARP);
//    keynotesArray.add(KEY_A);
//    keynotesArray.add(KEY_A_SHARP);
//    keynotesArray.add(KEY_B);
//
//    // add compatible chord types
//    // compatible chord types..
//    for (int type = 1; type < NUM_CHORD_TYPES; type++)
//    {
//        if (type != Augmented || type != Normal7th_5 || type != Major7th_11)
//        {
//            chordTypesArray.add((CHORD_TYPES)type);
//        }
//    }
//
//    initNoteAvailables();
//}
//
//
//void Harmonic_Modern::initFrequencies()
//{
//    // start at A0 frequency and build from there...
//    // user can opt for upto -2 octaves, which will reduce the frequency by half each unit.. , maybe more even
//    // for diatonic gui offer C to B, then shift octave based on
//    for (int i = 0 ; i < 128; i++) { noteFrequencies[i] = 0.f; }
//
//    double AMinus1 = central_frequency / 32;              // A0
//    double CMinus1 = AMinus1 / (27.f / 16.f);
//
//    noteFrequencies[0] = CMinus1;
//
//    for (int i = 1; i < 127; i++)
//    {
//        noteFrequencies[i] = noteFrequencies[i - 1] * (7.f / 4.f);
//    }
//
////    debugFrequenciesChromatic();
//}


Harmonic_Simple::Harmonic_Simple()
{
    thisScale           = SCALES::HARMONIC_SIMPLE;
    doesHaveInharmonics = false;
    
    // add compatible keynotes
    keynotesArray.add(KEY_C);
    keynotesArray.add(KEY_C_SHARP);
    keynotesArray.add(KEY_D);
    keynotesArray.add(KEY_D_SHARP);
    keynotesArray.add(KEY_E);
    keynotesArray.add(KEY_F);
    keynotesArray.add(KEY_F_SHARP);
    keynotesArray.add(KEY_G);
    keynotesArray.add(KEY_G_SHARP);
    keynotesArray.add(KEY_A);
    keynotesArray.add(KEY_A_SHARP);
    keynotesArray.add(KEY_B);
    
    // add compatible chord types
    for (int type = 1; type < NUM_CHORD_TYPES; type++)
    {
        chordTypesArray.add((CHORD_TYPES)type);
    }
    
    initNoteAvailables();
}


void Harmonic_Simple::initFrequencies()
{
    for (int i = 0 ; i < 128; i++) { noteFrequencies[i] = 0.f; }
    
//    double AMinus1 = central_frequency / 32;              // A0
//    double CMinus1 = AMinus1 / (27.f / 16.f);
//
//    noteFrequencies[0] = CMinus1;
//
//    for (int i = 1; i < 127; i++)
//    {
//        noteFrequencies[i] = noteFrequencies[i - 1] * (7.f / 4.f);
//    }
    
//    double CMinus1 = central_frequency / 69.f;
//
//    noteFrequencies[0] = CMinus1;
//
//    for (int i = 1; i < 127; i++)
//    {
//        noteFrequencies[i] = noteFrequencies[i - 1] * 2.f;
//    }
//
//    debugFrequenciesChromatic();
}


Enharmonic::Enharmonic()
{
    thisScale           = SCALES::ENHARMONIC;
    doesHaveInharmonics = false;
    
    // add compatible keynotes
    keynotesArray.add(KEY_C);
    keynotesArray.add(KEY_C_SHARP);
    keynotesArray.add(KEY_D);
    keynotesArray.add(KEY_D_SHARP);
    keynotesArray.add(KEY_E);
    keynotesArray.add(KEY_F);
    keynotesArray.add(KEY_F_SHARP);
    keynotesArray.add(KEY_G);
    keynotesArray.add(KEY_G_SHARP);
    keynotesArray.add(KEY_A);
    keynotesArray.add(KEY_A_SHARP);
    keynotesArray.add(KEY_B);
    
    chordTypesArray.add(Major);
    chordTypesArray.add(Major13th);
    
    initNoteAvailables();
}

void Enharmonic::initFrequencies()
{
    double AMinus1 = central_frequency / 32.f;
    double CMinus1 = AMinus1 * (16.f / 27.f);
    
    noteFrequencies[0] = CMinus1;                                   // C
    noteFrequencies[1] = CMinus1 * ( 2187.f / 2048.f);              // C#
    noteFrequencies[2] = CMinus1 * ( 9.f / 8.f);                    // D
    noteFrequencies[3] = CMinus1 * ( 19683.f / 16384.f);            // D#
    noteFrequencies[4] = CMinus1 * ( 81.f / 64.f);                  // E
    noteFrequencies[5] = CMinus1 * ( 4.f / 3.f);                    // F
    noteFrequencies[6] = CMinus1 * ( 729.f / 512.f);                // F#
    noteFrequencies[7] = CMinus1 * ( 3.f / 2.f);                    // G
    noteFrequencies[8] = CMinus1 * ( 6561.f / 4096.f);              // G#
    noteFrequencies[9] = CMinus1 * ( 27.f / 16.f);                  // A
    noteFrequencies[10] = CMinus1 * ( 59049.f / 32768.f);           // A#
    noteFrequencies[11] = CMinus1 * ( 243.f / 128.f);               // B

    for (int i = 12; i < 128; i++)
    {
        noteFrequencies[i] = noteFrequencies[i-12] * 2.f;
    }
    
//    debugFrequenciesChromatic();
}


Solfeggio::Solfeggio()
{
    thisScale           = SCALES::SOLFEGGIO;
    doesHaveInharmonics = false;
    
    // add compatible keynotes
    keynotesArray.add(KEY_C);
    keynotesArray.add(KEY_D);
    keynotesArray.add(KEY_E);
    keynotesArray.add(KEY_F);
    keynotesArray.add(KEY_G);
    keynotesArray.add(KEY_A);

    
    // add compatible chord types
    for (int type = 1; type < NUM_CHORD_TYPES; type++)
    {
        chordTypesArray.add((CHORD_TYPES)type);
    }
    
    initNoteAvailables();
}

void Solfeggio::initFrequencies()
{

    for (int i = 0 ; i < 128; i++) { noteFrequencies[i] = 0.f; }

    int base = 0 * 12;
    
    noteFrequencies[base + 0] = 396.f / 32.f;       // C
    noteFrequencies[base + 2] = 417.f / 32.f;       // D
    noteFrequencies[base + 4] = 528.f / 32.f;       // E
    noteFrequencies[base + 5] = 639.f / 32.f;       // F
    noteFrequencies[base + 7] = 741.f / 32.f;       // G
    noteFrequencies[base + 9] = 852.f / 32.f;       // A
    
    base = 1 * 12;
    
    noteFrequencies[base + 0] = 396.f / 16.f;       // C
    noteFrequencies[base + 2] = 417.f / 16.f;       // D
    noteFrequencies[base + 4] = 528.f / 16.f;       // E
    noteFrequencies[base + 5] = 639.f / 16.f;       // F
    noteFrequencies[base + 7] = 741.f / 16.f;       // G
    noteFrequencies[base + 9] = 852.f / 16.f;       // A
    
    base = 2 * 12;
    
    noteFrequencies[base + 0] = 396.f / 8.f;       // C
    noteFrequencies[base + 2] = 417.f / 8.f;       // D
    noteFrequencies[base + 4] = 528.f / 8.f;       // E
    noteFrequencies[base + 5] = 639.f / 8.f;       // F
    noteFrequencies[base + 7] = 741.f / 8.f;       // G
    noteFrequencies[base + 9] = 852.f / 8.f;       // A
    
    base = 3 * 12;
    
    noteFrequencies[base + 0] = 396.f / 4.f;       // C
    noteFrequencies[base + 2] = 417.f / 4.f;       // D
    noteFrequencies[base + 4] = 528.f / 4.f;       // E
    noteFrequencies[base + 5] = 639.f / 4.f;       // F
    noteFrequencies[base + 7] = 741.f / 4.f;       // G
    noteFrequencies[base + 9] = 852.f / 4.f;       // A
    
    base = 4 * 12;
    
    noteFrequencies[base + 0] = 396.f / 2.f;       // C
    noteFrequencies[base + 2] = 417.f / 2.f;       // D
    noteFrequencies[base + 4] = 528.f / 2.f;       // E
    noteFrequencies[base + 5] = 639.f / 2.f;       // F
    noteFrequencies[base + 7] = 741.f / 2.f;       // G
    noteFrequencies[base + 9] = 852.f / 2.f;       // A
    
    base = 5 * 12;
    
    noteFrequencies[base + 0] = 396.f;       // C
    noteFrequencies[base + 2] = 417.f;       // D
    noteFrequencies[base + 4] = 528.f;       // E
    noteFrequencies[base + 5] = 639.f;       // F
    noteFrequencies[base + 7] = 741.f;       // G
    noteFrequencies[base + 9] = 852.f;       // A
    
    base = 6 * 12;
    
    noteFrequencies[base + 0] = 396.f * 2.f;       // C
    noteFrequencies[base + 2] = 417.f * 2.f;       // D
    noteFrequencies[base + 4] = 528.f * 2.f;       // E
    noteFrequencies[base + 5] = 639.f * 2.f;       // F
    noteFrequencies[base + 7] = 741.f * 2.f;       // G
    noteFrequencies[base + 9] = 852.f * 2.f;       // A
    
    base = 7 * 12;
    
    noteFrequencies[base + 0] = 396.f * 4.f;       // C
    noteFrequencies[base + 2] = 417.f * 4.f;       // D
    noteFrequencies[base + 4] = 528.f * 4.f;       // E
    noteFrequencies[base + 5] = 639.f * 4.f;       // F
    noteFrequencies[base + 7] = 741.f * 4.f;       // G
    noteFrequencies[base + 9] = 852.f * 4.f;       // A
    
    base = 8 * 12;
    
    noteFrequencies[base + 0] = 396.f * 8.f;       // C
    noteFrequencies[base + 2] = 417.f * 8.f;       // D
    noteFrequencies[base + 4] = 528.f * 8.f;       // E
    noteFrequencies[base + 5] = 639.f * 8.f;       // F
    noteFrequencies[base + 7] = 741.f * 8.f;       // G
    noteFrequencies[base + 9] = 852.f * 8.f;       // A
    
    base = 9 * 12;
    
    noteFrequencies[base + 0] = 396.f * 16.f;       // C
    noteFrequencies[base + 2] = 417.f * 16.f;       // D
    noteFrequencies[base + 4] = 528.f * 16.f;       // E
    noteFrequencies[base + 5] = 639.f * 16.f;       // F
    noteFrequencies[base + 7] = 741.f * 16.f;       // G
    noteFrequencies[base + 9] = 852.f * 16.f;       // A
    
    noteFrequencies[120] = 396.f * 32.f;      // C
    noteFrequencies[122] = 417.f * 32.f;      // D
    noteFrequencies[124] = 528.f * 32.f;      // E
    noteFrequencies[125] = 639.f * 32.f;      // F
    noteFrequencies[127] = 741.f * 32.f;      // G
    
//    debugFrequenciesDiatonic();
}
