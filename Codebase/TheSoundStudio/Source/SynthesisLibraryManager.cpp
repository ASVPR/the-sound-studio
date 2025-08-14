/*
  ==============================================================================

    SynthesisLibraryManager.cpp
    Created: 14 Aug 2025 (Adapted from SampleLibraryManager)
    Author:  The Sound Studio Team

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
    
    addInstrument("Grand Classic", category, SynthesisType::PHYSICAL_MODELING_PIANO, 
                 "Classic grand piano with rich harmonics");
    addInstrument("Studio EP", category, SynthesisType::PHYSICAL_MODELING_PIANO,
                 "Electric piano with bell-like tones");
    addInstrument("Church Organ", category, SynthesisType::WAVETABLE_SYNTH,
                 "Majestic pipe organ simulation");
    addInstrument("Rhodes EP", category, SynthesisType::PHYSICAL_MODELING_PIANO,
                 "Vintage electric piano with bark");
    addInstrument("DX Supreme", category, SynthesisType::WAVETABLE_SYNTH,
                 "FM electric piano synthesis");
}

void SynthesisLibraryManager::initializeGuitars()
{
    const String category = "Guitars";
    if (!categoryNames.contains(category))
        categoryNames.add(category);
    
    addInstrument("Steel Guitar", category, SynthesisType::KARPLUS_STRONG_GUITAR,
                 "Bright acoustic steel string guitar");
    addInstrument("Nylon Guitar", category, SynthesisType::KARPLUS_STRONG_GUITAR,
                 "Warm classical nylon string guitar");
    addInstrument("Popular Guitar", category, SynthesisType::KARPLUS_STRONG_GUITAR,
                 "Modern acoustic guitar sound");
}

void SynthesisLibraryManager::initializeBells()
{
    const String category = "Bells & Vibes";
    if (!categoryNames.contains(category))
        categoryNames.add(category);
    
    addInstrument("Kingdom Bell", category, SynthesisType::PHYSICAL_MODELING_STRINGS,
                 "Majestic bell with long decay");
    addInstrument("Tubular Bell", category, SynthesisType::PHYSICAL_MODELING_STRINGS,
                 "Large orchestral tubular bells");
    addInstrument("Xylophone", category, SynthesisType::KARPLUS_STRONG_GUITAR,
                 "Bright wooden bar percussion");
    addInstrument("Marimba", category, SynthesisType::KARPLUS_STRONG_GUITAR,
                 "Warm wooden mallet instrument");
}

void SynthesisLibraryManager::initializeCinematic()
{
    const String category = "Cinematic";
    if (!categoryNames.contains(category))
        categoryNames.add(category);
    
    addInstrument("Epic Strings", category, SynthesisType::PHYSICAL_MODELING_STRINGS,
                 "Lush orchestral string section");
    addInstrument("Brass Section", category, SynthesisType::WAVETABLE_SYNTH,
                 "Powerful orchestral brass");
    addInstrument("Concert Harp", category, SynthesisType::KARPLUS_STRONG_HARP,
                 "Elegant orchestral harp");
    addInstrument("Choir Pad", category, SynthesisType::WAVETABLE_SYNTH,
                 "Ethereal vocal pad");
}

void SynthesisLibraryManager::initializeSynthesizers()
{
    const String category = "Synthesizer";
    if (!categoryNames.contains(category))
        categoryNames.add(category);
    
    addInstrument("Analog Lead", category, SynthesisType::VA_ANALOG_MODELING,
                 "Classic analog synthesizer lead");
    addInstrument("Poly Synth", category, SynthesisType::WAVETABLE_SYNTH,
                 "Rich polyphonic synthesizer");
    addInstrument("Bass Synth", category, SynthesisType::VA_ANALOG_MODELING,
                 "Deep analog bass synthesizer");
    addInstrument("Pad Synth", category, SynthesisType::WAVETABLE_SYNTH,
                 "Ambient atmospheric pad");
}

void SynthesisLibraryManager::initializeEthnic()
{
    const String category = "Ethnic";
    if (!categoryNames.contains(category))
        categoryNames.add(category);
    
    addInstrument("Sitar", category, SynthesisType::KARPLUS_STRONG_GUITAR,
                 "Indian classical string instrument");
    addInstrument("Koto", category, SynthesisType::KARPLUS_STRONG_GUITAR,
                 "Japanese traditional string instrument");
    addInstrument("Steel Drum", category, SynthesisType::PHYSICAL_MODELING_STRINGS,
                 "Caribbean steel drum");
    addInstrument("Ethnic Flute", category, SynthesisType::WAVETABLE_SYNTH,
                 "World wind instrument");
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