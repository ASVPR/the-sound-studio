/*
  ==============================================================================

    FrequencyToLightProcessor.cpp

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#include "FrequencyToLightProcessor.h"


FrequencyToLightProcessor::FrequencyToLightProcessor(FrequencyManager * fm)
{
    frequencyManager    = fm;
    ownedChordManager   = std::make_unique<ChordManager>(frequencyManager);
    chordManager        = ownedChordManager.get();
        
    // GUI Params
    conversionSource    = false;
    keynote             = 0;
    chordType           = 0;
    frequencyUnit       = HZ; // hz etc
    wavelengthUnit      = NANOMETER; // nm etc
    frequencyParam      = 0;
    waveLengthParam     = 0;
    phaseSpeedParam     = 0;
        
    
    mainOutputColour    = Colours::white;
    mainColourFrequency = 0;
    mainColourWavelength= 0;
    
        
    for (int i = 0; i < 5; i++)
    {
        manipulateChosenFrequency[i]    = false;
        multOrDivide[i]                 = false;
        multiplyValue[i]                = 1.f;
        divisionValue[i]                = 1.f;
            
        // returns
        shortcutFrequency[i]            = 0.f;
        shortcutWaveLength[i]           = 0.f;
            
        manipulatedColour[i]            = Colours::white;
    }

    
    // coloour Slider variables
    colourSlider_Val        = 0; // actual value of slider 0..1
    
    harmonic_Value = 0;
    
    for (int i = 0; i < NUM_HARMONICS; i++)
    {
        harmonics_Multiplier[i]         = 0;
        colourSliderFrequencies[i]      = 0;
        colourSliderWavelengths[i]      = 0;
        colourSliderHarmonicColours[i]  = Colours::white;
    }

}
    
FrequencyToLightProcessor::~FrequencyToLightProcessor(){}
    
void FrequencyToLightProcessor::prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock)
{
        
}
    
    
void FrequencyToLightProcessor::setParameter(int index, var newValue)
{
    if (index == FREQUENCY_LIGHT_CONVERSION_SOURCE)
    {
        conversionSource = newValue.operator bool();
        
        if (conversionSource)
        {
            calculateCoefficients();
        }
        else
        {
            calculateCoefficientsFromChord();
        }
    }
    else if (index == FREQUENCY_LIGHT_KEYNOTE)
    {
        keynote = newValue.operator int(); calculateCoefficientsFromChord();
    }
    else if (index == FREQUENCY_LIGHT_CHORDTYPE)
    {
        chordType = newValue.operator int(); calculateCoefficientsFromChord();
    }
//    else if (index == FREQUENCY_LIGHT_FREQUENCY_UNIT)
//    {
//        frequencyUnit = newValue.operator int(); calculateCoefficients();
//    }
//    else if (index == FREQUENCY_LIGHT_WAVELENGTH_UNIT)
//    {
//        wavelengthUnit = newValue.operator int(); calculateCoefficients();
//    }
    else if (index == FREQUENCY_LIGHT_FREQUENCY)
    {
        frequencyParam = newValue.operator float(); calculateCoefficientsFromFrequency();
    }
    else if (index == FREQUENCY_LIGHT_WAVELENGTH)
    {
        waveLengthParam = newValue.operator float(); calculateCoefficientsFromWavelength();
    }
    else if (index == FREQUENCY_LIGHT_PHASESPEED)
    {
        phaseSpeedParam = newValue.operator float(); calculateCoefficientsFromFrequency();
    }
    else if (index >= FREQUENCY_LIGHT_MANIPULATE_CHOSEN_FREQUENCY1 && index<= FREQUENCY_LIGHT_DIVISION_VALUE5)
    {
        for (int i = 0; i < 5; i++)
        {
            if (index == FREQUENCY_LIGHT_MANIPULATE_CHOSEN_FREQUENCY1 + (i * 4))
            {
                manipulateChosenFrequency[i] = newValue.operator bool();
            }
            else if (index == FREQUENCY_LIGHT_MULTIPLY_OR_DIVISION1 + (i * 4))
            {
                multOrDivide[i] = newValue.operator bool();
            }
            else if (index == FREQUENCY_LIGHT_MULTIPLY_VALUE1 + (i * 4))
            {
                multiplyValue[i] = newValue.operator float();
            }
            else if (index == FREQUENCY_LIGHT_DIVISION_VALUE1 + (i * 4))
            {
                divisionValue[i] = newValue.operator float();
            }
        }
        
        calculateManipulations();
    }
    else if (index == FREQUENCY_LIGHT_COLOUR_SLIDER_VALUE)
    {
        colourSlider_Val = newValue.operator float();
        
        calculateColourSliderHarmonics();
    }
}
    
void FrequencyToLightProcessor::calculateCoefficients()
{
    // reduntant for now
}

void FrequencyToLightProcessor::calculateColourSliderHarmonics()
{
    // caluclate frequency from wavelength
    
    // use harmonic interval to caluclate 4 extra colour
}


    // needs calculator functions
Colour FrequencyToLightProcessor::getMainColourOutput()
{
    return mainOutputColour;
}
    
Colour FrequencyToLightProcessor::getManipulatedColourOutput(int shortcut)
{
    return manipulatedColour[shortcut];
}

float FrequencyToLightProcessor::getWavelengthForShortcut(int shortcut)
{
    return shortcutWaveLength[shortcut];
}

float FrequencyToLightProcessor::getFrequencyForShortcut(int shortcut)
{
    return shortcutFrequency[shortcut];
}
    
    // λ violet=380 to λ red=780 nm
    
    
Colour FrequencyToLightProcessor::getColourFromWavelength(double Wavelength)
{
    const double Gamma        = TSS::FreqToLight::kGamma;
    const double IntensityMax = TSS::FreqToLight::kIntensityMax;

    double factor, Red,Green,Blue;

    if((Wavelength >= 380) && (Wavelength<440))
    {
        Red = -(Wavelength - 440) / (440 - 380);
        Green = 0.0;
        Blue = 1.0;
    }
    else if((Wavelength >= 440) && (Wavelength<490))
    {
        Red = 0.0;
        Green = (Wavelength - 440) / (490 - 440);
        Blue = 1.0;
    }
    else if((Wavelength >= 490) && (Wavelength<510))
    {
        Red = 0.0;
        Green = 1.0;
        Blue = -(Wavelength - 510) / (510 - 490);
    }
    else if((Wavelength >= 510) && (Wavelength<580))
    {
        Red = (Wavelength - 510) / (580 - 510);
        Green = 1.0;
        Blue = 0.0;
    }
    else if((Wavelength >= 580) && (Wavelength<645))
    {
        Red = 1.0;
        Green = -(Wavelength - 645) / (645 - 580);
        Blue = 0.0;
    }
    else if((Wavelength >= 645) && (Wavelength<781))
    {
        Red = 1.0;
        Green = 0.0;
        Blue = 0.0;
    }
    else
    {
        Red = 0.0;
        Green = 0.0;
        Blue = 0.0;
    }

    // Let the intensity fall off near the vision limits

    if((Wavelength >= 380) && (Wavelength<420))
    {
        factor = 0.3 + 0.7*(Wavelength - 380) / (420 - 380);
    }
    else if((Wavelength >= 420) && (Wavelength<701))
    {
        factor = 1.0;
    }
    else if((Wavelength >= 701) && (Wavelength<781))
    {
        factor = 0.3 + 0.7*(780 - Wavelength) / (780 - 700);
    }
    else
    {
        factor = 0.0;
    }


    // Stack allocation — no heap allocation (was: new int[3] with memory leak)
    int rgb[3];

    // Don't want 0^x = 1 for x <> 0
    rgb[0] = Red==0.0 ? 0 : (int) round(IntensityMax * pow(Red * factor, Gamma));
    rgb[1] = Green==0.0 ? 0 : (int) round(IntensityMax * pow(Green * factor, Gamma));
    rgb[2] = Blue==0.0 ? 0 : (int) round(IntensityMax * pow(Blue * factor, Gamma));

    return Colour::fromRGB(static_cast<uint8>(rgb[0]),
                           static_cast<uint8>(rgb[1]),
                           static_cast<uint8>(rgb[2]));
}
