/*
  ==============================================================================

    WaveStackOsc.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/


#pragma once
#ifndef __WaveTableSynthPrototype__WaveStackOsc__
#define __WaveTableSynthPrototype__WaveStackOsc__

#include <iostream>
#include "JuceHeader.h"
#include "WaveTableOsc.h"
#include "VotanBuffer.h"
#include "math.h"

#endif /* defined(__WaveTableSynthPrototype__WaveStackOsc__) */

// 1. try to keep wavegeneration in shared place, otherwise we'll be 32 x numvoices of tables


#define tableSize 2048
const static int numWaveStacks = 32;

class WaveStackOsc {
    
public:
    
    WaveStackOsc();
    ~WaveStackOsc();
    
    // table loader functions
    void loadTableFromFileChooser(File * file);
    void reinitialiseTable(int ref);
    void convertAudioFileToSampleArray();
    
    void processBuffer(float * buffer, int numSamples);
    void processBuffer2(float * buffer, int numSamples);
    
    void setParameter(int type, float value);
    double getPhasorVal();
    void setBufferReference(float morphValue);
    bool AreSame(double a, double b);
    void setFrequency(float f);
    void applyFrequency();
    void setMorph(float m);
    void SetZeroPhase();
    void setTableRefIndex(int ref);
    void updatePhase();
    void setPhase(float value);
    void setIndex(int idx);
    
    void setDetuneValue(float d);
    void setSemiToneValue(float st);
    
    int refTemp1, refTemp2;
    float mixTemp;
    double phasorVal;

    int index;
    float oscVolume;

    float   m_semitone;
    float   m_detune;
    
    #define CLOSE(a,b,e) (fabs(a,b)< e)
    
    
    void runTestOnBuffer();
    
    void setSampleRate(float sr)
    {
        sample_Rate = sr;
        
        for (int i = 0; i < numWaveStacks; i++) {
            _stackOsc[i]->setSampleRate(sample_Rate);
        }
    }
    
    void reset()
    {
        // set acc to 0;
        setPhase(0);
        
    }

private:
    
    int osc1, osc2; // buffer reference;
    float frequency;
    float morph;
    float wetDryMix;
    
    float sample_Rate;
    
    WaveTableOsc * _stackOsc[numWaveStacks];
    double * _waveStackArray[numWaveStacks];
    
    CVotanBuffer stackBufferAlloc[numWaveStacks];
    CVotanBuffer mixBufferAlloc;
    
    // filters
    IIRFilter filterLP;
    IIRFilter filterHP;
    
    double filterLPFrequency;
    double filterHPFrequency;
    
    void setFilterCoefs(int filter);

    //helper functions
    float convertSemiToneToHz(float semitone);
    float convertDetuneToHz(float cents);
};
