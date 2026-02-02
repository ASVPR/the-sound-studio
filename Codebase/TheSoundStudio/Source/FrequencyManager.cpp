/*
  ==============================================================================

    FrequencyManager.cpp

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#include "FrequencyManager.h"
#include "ProjectManager.h"

FrequencyManager::FrequencyManager()
{
    ownedScalesManager = std::make_unique<ScalesManager>();
    scalesManager = ownedScalesManager.get();
}

FrequencyManager::~FrequencyManager()
{
    
}

void FrequencyManager::setBaseAFrequency(double newBaseA)
{
    if (newBaseA <= MIN_BASE_A_FREQUENCY) newBaseA = MIN_BASE_A_FREQUENCY;
    
    if (newBaseA >= MAX_BASE_A_FREQUENCY) newBaseA = MAX_BASE_A_FREQUENCY;
    
    central_frequency = newBaseA;
    
    scalesManager->setBaseAFrequency(central_frequency);
    
    scalesManager->initialiseNoteFrequencies();
}

float FrequencyManager::getBaseAFrequency()
{
    return central_frequency;
}

ScalesManager::ScalesManager()
{
    central_frequency   = TSS::Audio::kDefaultA4Frequency;
    currentScale        = SCALES::DIATONIC_PYTHAGOREAN;
    changeMainScaleTo(currentScale);
    changeLissajousScaleTo(currentScale);  // Initialize lissajousScale to prevent null pointer
    
    // Initialize chordPlayerShortcutScale array
    for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++) {
        changeChordPlayerScaleTo(i, currentScale);
    }
}

ScalesManager::~ScalesManager(){
    
}

void  ScalesManager::setBaseAFrequency(double newBaseA)
{
    if (newBaseA <= MIN_BASE_A_FREQUENCY) newBaseA = MIN_BASE_A_FREQUENCY;
    if (newBaseA >= MAX_BASE_A_FREQUENCY) newBaseA = MAX_BASE_A_FREQUENCY;
    
    central_frequency = newBaseA;
    
    mainScale->setBaseAFrequency(central_frequency);
    
    diatonic_Pythagorean        .setBaseAFrequency(central_frequency);
    diatonic_JustItonation      .setBaseAFrequency(central_frequency);
    diatonic_IterationFifth     .setBaseAFrequency(central_frequency);
    chromatic_Pythagorean       .setBaseAFrequency(central_frequency);
    chromatic_JustItonation     .setBaseAFrequency(central_frequency);
    chromatic_EqualTemperament  .setBaseAFrequency(central_frequency);
//    harmonic_EqualTemperament   .setBaseAFrequency(central_frequency);
//    harmonic_Modern             .setBaseAFrequency(central_frequency);
    harmonic_Simple             .setBaseAFrequency(central_frequency);
    enharmonic                  .setBaseAFrequency(central_frequency);
    solfeggio                   .setBaseAFrequency(central_frequency);
}

double ScalesManager::getBaseAFrequency()
{
    return central_frequency;
}


void ScalesManager::initialiseNoteFrequencies()
{
    
}

void ScalesManager::setScale(int scale)
{
    currentScale = (SCALES)scale;
    changeMainScaleTo(currentScale);
    mainScale->initFrequencies();
    
    // if default is reset, it should also rest Chordplayer and lissajous
    
    for (int i = 0; i < NUM_SHORTCUT_SYNTHS; i++) {
        setChordPlayerScale(i, currentScale);
    }
    
    setLissajousScale(currentScale);
    
}

void ScalesManager::setChordPlayerScale(int shortcut, int scale)
{
    changeChordPlayerScaleTo(shortcut, (SCALES)scale);
    chordPlayerShortcutScale[shortcut]->initFrequencies();
}

void ScalesManager::setLissajousScale(int newScale)
{
    changeLissajousScaleTo((SCALES)newScale);
    lissajousScale->initFrequencies();
}

void ScalesManager::changeMainScaleTo(SCALES newScale)
{
    switch (newScale)
    {
        case DIATONIC_PYTHAGOREAN:          mainScale = &diatonic_Pythagorean;          break;
        case DIATONIC_JUSTLY_TUNED:         mainScale = &diatonic_JustItonation;        break;
        case DIATONIC_ITERATION_FIFTH:      mainScale = &diatonic_IterationFifth;       break;
        case CHROMATIC_PYTHAGOREAN:         mainScale = &chromatic_Pythagorean;         break;
        case CHROMATIC_JUST_INTONATION:     mainScale = &chromatic_JustItonation;       break;
        case CHROMATIC_ET:                  mainScale = &chromatic_EqualTemperament;    break;
//        case HARMONIC_ET:                   mainScale = &harmonic_EqualTemperament;     break;
//        case HARMONIC_MODERN:               mainScale = &harmonic_Modern;               break;
        case HARMONIC_SIMPLE:               mainScale = &harmonic_Simple;               break;
        case ENHARMONIC:                    mainScale = &enharmonic;                    break;
        case SOLFEGGIO:                     mainScale = &solfeggio;                     break;
        default:                            mainScale = &diatonic_Pythagorean;          break;
    }
}

void ScalesManager::changeChordPlayerScaleTo(int shortcut, SCALES newScale)
{
    switch (newScale)
    {
        case DIATONIC_PYTHAGOREAN:          chordPlayerShortcutScale[shortcut] = &diatonic_Pythagorean;          break;
        case DIATONIC_JUSTLY_TUNED:         chordPlayerShortcutScale[shortcut] = &diatonic_JustItonation;        break;
        case DIATONIC_ITERATION_FIFTH:      chordPlayerShortcutScale[shortcut] = &diatonic_IterationFifth;       break;
        case CHROMATIC_PYTHAGOREAN:         chordPlayerShortcutScale[shortcut] = &chromatic_Pythagorean;         break;
        case CHROMATIC_JUST_INTONATION:     chordPlayerShortcutScale[shortcut] = &chromatic_JustItonation;       break;
        case CHROMATIC_ET:                  chordPlayerShortcutScale[shortcut] = &chromatic_EqualTemperament;    break;
//        case HARMONIC_ET:                   chordPlayerShortcutScale[shortcut] = &harmonic_EqualTemperament;     break;
//        case HARMONIC_MODERN:               chordPlayerShortcutScale[shortcut] = &harmonic_Modern;               break;
        case HARMONIC_SIMPLE:               chordPlayerShortcutScale[shortcut] = &harmonic_Simple;               break;
        case ENHARMONIC:                    chordPlayerShortcutScale[shortcut] = &enharmonic;                    break;
        case SOLFEGGIO:                     chordPlayerShortcutScale[shortcut] = &solfeggio;                     break;
        default:                            chordPlayerShortcutScale[shortcut] = &diatonic_Pythagorean;          break;
    }
}

void ScalesManager::changeLissajousScaleTo(SCALES newScale)
{
    switch (newScale)
    {
        case DIATONIC_PYTHAGOREAN:          lissajousScale = &diatonic_Pythagorean;          break;
        case DIATONIC_JUSTLY_TUNED:         lissajousScale = &diatonic_JustItonation;        break;
        case DIATONIC_ITERATION_FIFTH:      lissajousScale = &diatonic_IterationFifth;       break;
        case CHROMATIC_PYTHAGOREAN:         lissajousScale = &chromatic_Pythagorean;         break;
        case CHROMATIC_JUST_INTONATION:     lissajousScale = &chromatic_JustItonation;       break;
        case CHROMATIC_ET:                  lissajousScale = &chromatic_EqualTemperament;    break;
//        case HARMONIC_ET:                   lissajousScale = &harmonic_EqualTemperament;     break;
//        case HARMONIC_MODERN:               lissajousScale = &harmonic_Modern;               break;
        case HARMONIC_SIMPLE:               lissajousScale = &harmonic_Simple;               break;
        case ENHARMONIC:                    lissajousScale = &enharmonic;                    break;
        case SOLFEGGIO:                     lissajousScale = &solfeggio;                     break;
        default:                            lissajousScale = &diatonic_Pythagorean;          break;
    }
}


void ScalesManager::getComboBoxPopupMenuForScales(PopupMenu & scalesPopupMenu)
{
    // return full popup menu system for dropdown in Settings / ChordPlayer
    PopupMenu diatonicMenu;
    diatonicMenu.addItem(DIATONIC_PYTHAGOREAN, "Pythagorean");
    diatonicMenu.addItem(DIATONIC_ITERATION_FIFTH, "Iteration Fifth");
    diatonicMenu.addItem(DIATONIC_JUSTLY_TUNED, "Justly Tuned");

    scalesPopupMenu.addSubMenu("Diatonic", diatonicMenu);
    
    PopupMenu chromaticMenu;
    chromaticMenu.addItem(CHROMATIC_PYTHAGOREAN, "Pythagorean");
    chromaticMenu.addItem(CHROMATIC_JUST_INTONATION, "Just Itonation");
    chromaticMenu.addItem(CHROMATIC_ET, "Equal Temperament");
    
    scalesPopupMenu.addSubMenu("Chromatic", chromaticMenu);
    
    PopupMenu harmonicMenu;
//    harmonicMenu.addItem(HARMONIC_ET, "Equal Temperament");
//    harmonicMenu.addItem(HARMONIC_MODERN, "Modern");
    harmonicMenu.addItem(HARMONIC_SIMPLE, "Simple");
    
    scalesPopupMenu.addSubMenu("Harmonic", harmonicMenu);
    
    scalesPopupMenu.addItem(ENHARMONIC, "Enharmonic");
    scalesPopupMenu.addItem(SOLFEGGIO, "Solfeggio");
}

void ScalesManager::getComboBoxPopupMenuForChords(PopupMenu & chordsPopupMenu, SCALES_UNIT unit)
{
    chordsPopupMenu.clear();
    
    if (unit == MAIN_SCALE)
    {
        if (mainScale != nullptr)  // Add null check
        {
            for (int num = 0; num < mainScale->getAvailableChordTypes().size(); num++)
            {
                for (int i = 1; i < NUM_CHORD_TYPES; i++)
                {
                    if ((CHORD_TYPES)i == mainScale->getAvailableChordTypes().getReference(num))
                    {
                        chordsPopupMenu.addItem(i, ProjectStrings::getChordTypeArray().getReference(i-1));
                    }
                }
            }
        }
    }
    else if (unit == LISSAJOUS_SCALE)
    {
        if (lissajousScale != nullptr)  // Add null check
        {
            for (int num = 0; num < lissajousScale->getAvailableChordTypes().size(); num++)
            {
                for (int i = 1; i < NUM_CHORD_TYPES; i++)
                {
                    if ((CHORD_TYPES)i == lissajousScale->getAvailableChordTypes().getReference(num))
                    {
                        chordsPopupMenu.addItem(i, ProjectStrings::getChordTypeArray().getReference(i-1));
                    }
                }
            }
        }
    }
    else
    {
        int shortcutUnit = (int)unit;
        
        if (shortcutUnit >= 0 && shortcutUnit < NUM_SHORTCUT_SYNTHS && chordPlayerShortcutScale[shortcutUnit] != nullptr)  // Add bounds and null check
        {
            for (int num = 0; num < chordPlayerShortcutScale[shortcutUnit]->getAvailableChordTypes().size(); num++)
            {
                for (int i = 1; i < NUM_CHORD_TYPES; i++)
                {
                    if ((CHORD_TYPES)i == chordPlayerShortcutScale[shortcutUnit]->getAvailableChordTypes().getReference(num))
                {
                    chordsPopupMenu.addItem(i, ProjectStrings::getChordTypeArray().getReference(i-1));
                }
            }
        }
        }
    }
}

void ScalesManager::getComboBoxPopupMenuForKeynotes(PopupMenu & keynotesPopupMenu, SCALES_UNIT unit)
{
    keynotesPopupMenu.clear();
    
    if (unit == MAIN_SCALE)
    {
        if (mainScale != nullptr)  // Add null check
        {
            for (int num = 0; num < mainScale->getAvailableKeynotes().size(); num++)
            {
                for (int i = 1; i < TOTAL_NUM_KEYNOTES; i++)
                {
                    if ((KEYNOTES)i == mainScale->getAvailableKeynotes().getReference(num))
                    {
                        keynotesPopupMenu.addItem(i, ProjectStrings::getKeynoteArray().getReference(i-1));
                    }
                }
            }
        }
    }
    else if (unit == LISSAJOUS_SCALE)
    {
        if (lissajousScale != nullptr)  // Add null check
        {
            for (int num = 0; num < lissajousScale->getAvailableKeynotes().size(); num++)
            {
                for (int i = 1; i < TOTAL_NUM_KEYNOTES; i++)
                {
                    if ((KEYNOTES)i == lissajousScale->getAvailableKeynotes().getReference(num))
                    {
                        keynotesPopupMenu.addItem(i, ProjectStrings::getKeynoteArray().getReference(i-1));
                    }
                }
            }
        }
    }
    else
    {
        int shortcutUnit = (int)unit;
        
        if (shortcutUnit >= 0 && shortcutUnit < NUM_SHORTCUT_SYNTHS && chordPlayerShortcutScale[shortcutUnit] != nullptr)  // Add bounds and null check
        {
            for (int num = 0; num < chordPlayerShortcutScale[shortcutUnit]->getAvailableKeynotes().size(); num++)
            {
                for (int i = 1; i < TOTAL_NUM_KEYNOTES; i++)
                {
                    if ((KEYNOTES)i == chordPlayerShortcutScale[shortcutUnit]->getAvailableKeynotes().getReference(num))
                {
                    keynotesPopupMenu.addItem(i, ProjectStrings::getKeynoteArray().getReference(i-1));
                }
            }
        }
        }
    }
}



