/*
  ==============================================================================

    SynthesisLibraryManager.h
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Parameters.h"
#include "SynthesisEngine.h"

// Synthesis instrument definition
struct SynthesisInstrument
{
    String name;
    String category;
    SynthesisType synthType;
    float defaultParameters[16];
    String description;
};

class SynthesisLibraryManager
{
public:
    SynthesisLibraryManager()
    {
        initializeSynthesisInstruments();
    }
    ~SynthesisLibraryManager() { }
    
    // Main interface methods
    void initializeSynthesisInstruments();
    
    // Instrument access
    String getInstrumentName(int selectedID);
    SynthesisType getInstrumentSynthType(int selectedID);
    SynthesisInstrument getInstrument(int selectedID);
    
    // Category management
    Array<String> getAvailableCategories();
    Array<SynthesisInstrument> getInstrumentsInCategory(const String& category);
    
    // Library information
    int getNumInstruments() const { return synthesisInstruments.size(); }
    int getNumCategories() const { return categoryNames.size(); }
    
private:
    Array<SynthesisInstrument> synthesisInstruments;
    Array<String> categoryNames;
    
    // Helper methods
    void addInstrument(const String& name, const String& category, SynthesisType type, 
                      const String& description = "");
    void initializePianos();
    void initializeGuitars();
    void initializeBells();
    void initializeCinematic();
    void initializeSynthesizers();
    void initializeEthnic();
};
