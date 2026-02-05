/*
  ==============================================================================

    FrequencyToLightProcessor.h
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#pragma once

#include "ChordManager.h"
#include "Parameters.h"
#include "FrequencyManager.h"

#define NUM_HARMONICS 5

class FrequencyToLightProcessor {
    
    // speed of light in air vacuum     = 299792458 m/s
    // speed of light n water           = 225000000 m/s
    // speed of sound in air            = 331.5 m/s
    // speed of sound in water          = 1498 m/s
    
    // refractive index parameter ?
public:
    FrequencyToLightProcessor(FrequencyManager * fm);
    ~FrequencyToLightProcessor();
    void prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock);
    void setParameter(int index, var newValue);
    
    
    // getters // main
    Colour getMainColourOutput();
    Colour getManipulatedColourOutput(int shortcut);
    float getWavelengthForShortcut(int shortcut);
    float getFrequencyForShortcut(int shortcut);
    
    // getters Colour Slider
    Colour getColourForColourSlider(int harmonic);
    float getFrequencyForColourSlider(int harmonic);
    float getWavelengthForColiurSlider(int harmonic);
    
    // calculators
    
    
    // hwne setParam is called, we need to
    // 1. check chord source
    // check if frequency or wavelength is updating
    // dun calc to shift f -> w or reverse
    // once done.. calculate manipulations
    // set f / w
    // set colours of all
    
    void calculateCoefficients();
    
    // called when freq/wavelength is already set, use for phase speed change for now
    void calculateCoefficientsFromPhaseSpeed()
    {
        
    }
    
    // called when chord is selected
    void calculateCoefficientsFromChord()
    {
//        // get frequency of Base note
////        double frequency = frequencyManager->getFrequencyForMIDINote(keyNote);
////        // set wavelength
////        double waveLength =
////        // set Main Colour
////        mainOutputColour = getWavelengthFromFrequency(<#float freqVal#>)
//        // set manipulations
//        calculateManipulations();
    }
    
    // called when frequency parameter is changed
    void calculateCoefficientsFromFrequency()
    {
        waveLengthParam     = getWavelengthFromFrequency(frequencyParam);
        mainOutputColour    = getColourFromWavelength(waveLengthParam);
        
        calculateManipulations();
    }
    
    // callend when wavelength is changed
    void calculateCoefficientsFromWavelength()
    {
        frequencyParam      = getFrequencyFromWavelength(waveLengthParam);
        mainOutputColour    = getColourFromWavelength(waveLengthParam);
    
        calculateManipulations();
    }
   
    // called by all the above when changed, called when manipulations are called
//    void calculateManipulations()
//    {
//        // use manipulation values for create colours
//        for (int i = 0; i < 5; i++)
//        {
//            if (manipulateChosenFrequency[i])
//            {
//                if (!multOrDivide[i]) // mult
//                {
//                    shortcutFrequency[i]    = frequencyParam / multiplyValue[i];
//                    shortcutWaveLength[i]   = getWavelengthFromFrequency(shortcutFrequency[i]);
//                }
//                else // divide
//                {
//                    shortcutFrequency[i]    = frequencyParam * divisionValue[i];
//                    shortcutWaveLength[i]   = getWavelengthFromFrequency(shortcutFrequency[i]);
//                }
//
//                manipulatedColour[i] = getColourFromWavelength(shortcutWaveLength[i]);
//            }
//            else
//            {
//                // set to white
//                manipulatedColour[i] = Colours::white;
//            }
//        }
//    }
    
    void calculateManipulations()
    {
        // use manipulation values for create colours
        for (int i = 0; i < 5; i++)
        {
            if (manipulateChosenFrequency[i])
            {
                if (!multOrDivide[i]) // mult
                {
                    shortcutWaveLength[i]    = waveLengthParam * multiplyValue[i];
                    shortcutFrequency[i]     = getFrequencyFromWavelength(shortcutWaveLength[i]);
                }
                else // divide
                {
                    shortcutWaveLength[i]    = waveLengthParam / divisionValue[i];
                    shortcutFrequency[i]     = getFrequencyFromWavelength(shortcutWaveLength[i]);
                }
                
                manipulatedColour[i] = getColourFromWavelength(shortcutWaveLength[i]);
            }
            else
            {
                // set to white
                manipulatedColour[i] = Colours::white;
            }
        }
    }
    
    
    double getWavelengthFromFrequency(double f)
    {
        float c = 0;
        if (phaseSpeedParam == PHASE_SPEED_UNITS::SPEED_LIGHT_IN_VACUUM)
        { c = SPEEDLIGHTINVACUUM; }
        else if (phaseSpeedParam == PHASE_SPEED_UNITS::SPEED_LIGHT_IN_WATER)
        { c = SPEEDLIGHTINWATER; }
        else if (phaseSpeedParam == PHASE_SPEED_UNITS::SPEED_SOUND_IN_VACUUM)
        { c = SPEEDSOUNDINVACUUM; }
        else if (phaseSpeedParam == PHASE_SPEED_UNITS::SPEED_SOUND_IN_WATER)
        { c = SPEEDSOUNDINWATER; }
        
        return (c / f) * METER_FACTOR;
    }
    
    double getFrequencyFromWavelength(double w)
    {
        float c = 0;
        if (phaseSpeedParam == PHASE_SPEED_UNITS::SPEED_LIGHT_IN_VACUUM)
        { c = SPEEDLIGHTINVACUUM; }
        else if (phaseSpeedParam == PHASE_SPEED_UNITS::SPEED_LIGHT_IN_WATER)
        { c = SPEEDLIGHTINWATER; }
        else if (phaseSpeedParam == PHASE_SPEED_UNITS::SPEED_SOUND_IN_VACUUM)
        { c = SPEEDSOUNDINVACUUM; }
        else if (phaseSpeedParam == PHASE_SPEED_UNITS::SPEED_SOUND_IN_WATER)
        { c = SPEEDSOUNDINWATER; }
        
        return c / (w * METER_FACTOR);
    }
    
    
    Colour getColourFromWavelength(double Wavelength);
    
private:
    
    FrequencyManager * frequencyManager;
    ChordManager * chordManager;
    
    // GUI parameters
    bool conversionSource = false;
    int keynote;
    int chordType;
    int frequencyUnit; // hz etc
    int wavelengthUnit; // nm etc
    double frequencyParam; // in hz
    double waveLengthParam; // in nm
    int  phaseSpeedParam;
    double phaseSpeedValue;
    bool manipulateChosenFrequency[5];
    bool multOrDivide[5];
    float multiplyValue[5];
    float divisionValue[5];
    
    // returns
    float mainColourFrequency;
    float mainColourWavelength;
    float shortcutFrequency[5];
    float shortcutWaveLength[5];
    
    Colour mainOutputColour;
    Colour manipulatedColour[5];
    
    // colourslider
    float colourSlider_Val;
    float harmonic_Value;
    
    float harmonics_Multiplier[NUM_HARMONICS];
    
    void calculateColourSliderHarmonics();
    
    float colourSliderFrequencies[NUM_HARMONICS];
    float colourSliderWavelengths[NUM_HARMONICS];
    Colour colourSliderHarmonicColours[NUM_HARMONICS];
    
};
