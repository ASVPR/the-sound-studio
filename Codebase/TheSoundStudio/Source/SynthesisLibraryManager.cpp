/*
  ==============================================================================

    SynthesisLibraryManager.cpp
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#include "SynthesisLibraryManager.h"

void SynthesisLibraryManager::initializeSynthesisInstruments()
{
    // Clear existing instruments
    synthesisInstruments.clear();
    categoryNames.clear();
    
    // Initialize all instrument categories
    initializePianos();
    initializeGuitars();
    initializeBells();
    initializeCinematic();
    initializeSynthesizers();
    initializeEthnic();
}

void SynthesisLibraryManager::initializePianos()
{
    const String category = "Keys & Pianos";
    if (!categoryNames.contains(category))
        categoryNames.add(category);
    
    addInstrument("Grand Piano", category, SynthesisType::PHYSICAL_MODELING_PIANO, 
                 "Realistic grand piano with inharmonic partials");
    addInstrument("Electric Piano", category, SynthesisType::PHYSICAL_MODELING_PIANO,
                 "Electric piano with bell-like attack");
    addInstrument("Church Organ", category, SynthesisType::WAVETABLE_ELECTRONIC,
                 "Hammond-style organ with drawbar simulation");
    addInstrument("Pipe Organ", category, SynthesisType::WAVETABLE_ELECTRONIC,
                 "Classical pipe organ with rich harmonics");
}

void SynthesisLibraryManager::initializeGuitars()
{
    const String category = "Guitars";
    if (!categoryNames.contains(category))
        categoryNames.add(category);
    
    addInstrument("Acoustic Guitar", category, SynthesisType::KARPLUS_STRONG_GUITAR,
                 "Steel string acoustic guitar with body resonance");
    addInstrument("Classical Guitar", category, SynthesisType::KARPLUS_STRONG_GUITAR,
                 "Nylon string classical guitar");
    addInstrument("Electric Guitar", category, SynthesisType::KARPLUS_STRONG_GUITAR,
                 "Clean electric guitar tone");
}

void SynthesisLibraryManager::initializeBells()
{
    const String category = "Bells & Vibes";
    if (!categoryNames.contains(category))
        categoryNames.add(category);
    
    addInstrument("Bell", category, SynthesisType::KARPLUS_STRONG_GUITAR,
                 "Church bell with long resonant decay");
    addInstrument("Chimes", category, SynthesisType::KARPLUS_STRONG_GUITAR,
                 "Tubular chimes with bright attack");
    addInstrument("Xylophone", category, SynthesisType::KARPLUS_STRONG_GUITAR,
                 "Wooden mallet percussion");
    addInstrument("Vibraphone", category, SynthesisType::KARPLUS_STRONG_GUITAR,
                 "Metal bar vibraphone");
}

void SynthesisLibraryManager::initializeCinematic()
{
    const String category = "Cinematic";
    if (!categoryNames.contains(category))
        categoryNames.add(category);
    
    addInstrument("Strings", category, SynthesisType::PHYSICAL_MODELING_STRINGS,
                 "Rich orchestral string ensemble");
    addInstrument("Brass", category, SynthesisType::WAVETABLE_SYNTH,
                 "Powerful brass section");
    addInstrument("Harp", category, SynthesisType::KARPLUS_STRONG_HARP,
                 "Concert harp with plucked resonance");
    addInstrument("Flute", category, SynthesisType::WAVETABLE_SYNTH,
                 "Woodwind flute with breath modeling");
}

void SynthesisLibraryManager::initializeSynthesizers()
{
    const String category = "Synthesizer";
    if (!categoryNames.contains(category))
        categoryNames.add(category);
    
    addInstrument("Lead Synth", category, SynthesisType::WAVETABLE_SYNTH,
                 "Bright synthesizer lead sound");
    addInstrument("Pad Synth", category, SynthesisType::WAVETABLE_SYNTH,
                 "Warm synthesizer pad");
    addInstrument("Bass Synth", category, SynthesisType::WAVETABLE_SYNTH,
                 "Deep synthesizer bass");
    addInstrument("Pluck Synth", category, SynthesisType::WAVETABLE_SYNTH,
                 "Percussive synthesizer pluck");
}

void SynthesisLibraryManager::initializeEthnic()
{
    const String category = "Ethnic";
    if (!categoryNames.contains(category))
        categoryNames.add(category);
    
    addInstrument("Sitar", category, SynthesisType::KARPLUS_STRONG_GUITAR,
                 "Indian string instrument with sympathetic strings");
    addInstrument("Koto", category, SynthesisType::KARPLUS_STRONG_GUITAR,
                 "Japanese plucked string instrument");
    addInstrument("Steel Drum", category, SynthesisType::KARPLUS_STRONG_GUITAR,
                 "Caribbean steel percussion");
    addInstrument("Shakuhachi", category, SynthesisType::WAVETABLE_SYNTH,
                 "Japanese bamboo flute");
}

void SynthesisLibraryManager::addInstrument(const String& name, const String& category, 
                                          SynthesisType type, const String& description)
{
    SynthesisInstrument instrument;
    instrument.name = name;
    instrument.category = category;
    instrument.synthType = type;
    instrument.description = description;
    
    // Initialize default parameters based on synthesis type
    for (int i = 0; i < 16; ++i)
        instrument.defaultParameters[i] = 0.5f; // Default middle values
    
    // Set type-specific defaults
    switch (type)
    {
        case SynthesisType::PHYSICAL_MODELING_PIANO:
            instrument.defaultParameters[0] = 0.8f; // String tension
            instrument.defaultParameters[1] = 0.6f; // Hammer hardness
            instrument.defaultParameters[2] = 0.4f; // Damping
            break;
            
        case SynthesisType::KARPLUS_STRONG_GUITAR:
            instrument.defaultParameters[0] = 0.7f; // Body resonance
            instrument.defaultParameters[1] = 0.3f; // String damping
            instrument.defaultParameters[2] = 0.5f; // Pluck position
            break;
            
        case SynthesisType::WAVETABLE_SYNTH:
            instrument.defaultParameters[0] = 0.5f; // Wavetable position
            instrument.defaultParameters[1] = 0.3f; // Filter cutoff
            instrument.defaultParameters[2] = 0.2f; // Filter resonance
            break;
            
        default:
            break;
    }
    
    synthesisInstruments.add(instrument);
}

String SynthesisLibraryManager::getInstrumentName(int selectedID)
{
    if (selectedID >= 0 && selectedID < synthesisInstruments.size())
        return synthesisInstruments[selectedID].name;
    
    return "Unknown Instrument";
}

SynthesisType SynthesisLibraryManager::getInstrumentSynthType(int selectedID)
{
    if (selectedID >= 0 && selectedID < synthesisInstruments.size())
        return synthesisInstruments[selectedID].synthType;
    
    return SynthesisType::WAVETABLE_SYNTH; // Default fallback
}

SynthesisInstrument SynthesisLibraryManager::getInstrument(int selectedID)
{
    if (selectedID >= 0 && selectedID < synthesisInstruments.size())
        return synthesisInstruments[selectedID];
    
    // Return default instrument
    SynthesisInstrument defaultInstrument;
    defaultInstrument.name = "Default";
    defaultInstrument.category = "Synthesizer";
    defaultInstrument.synthType = SynthesisType::WAVETABLE_SYNTH;
    for (int i = 0; i < 16; ++i)
        defaultInstrument.defaultParameters[i] = 0.5f;
    
    return defaultInstrument;
}

Array<String> SynthesisLibraryManager::getAvailableCategories()
{
    return categoryNames;
}

Array<SynthesisInstrument> SynthesisLibraryManager::getInstrumentsInCategory(const String& category)
{
    Array<SynthesisInstrument> filtered;
    
    for (const auto& instrument : synthesisInstruments)
    {
        if (instrument.category == category)
            filtered.add(instrument);
    }
    
    return filtered;
}