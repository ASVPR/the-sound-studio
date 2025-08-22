/*
  =============================================================================

    WaveTableOsc.
    Created: 13 Mar 2020 11:26:45p
    Author:  Gary Jone

  =============================================================================
*/

#pragma once

#include <iostream>
#include <math.h>
#include "JuceHeader.h"

#define doLinearInterp 1
// oscillator

//#define sample_Rate (44100)
#define overSamp (2)        /* oversampling factor (positive integer) */
#define baseFrequency (6)  /* starting frequency of first table */
#define constantRatioLimit (99999)    /* set to a large number (greater than or equal to the length of the lowest octave table) for constant table size; set to 0 for a constant oversampling ratio (each higher ocatave table length reduced by half); set somewhere between (64, for instance) for constant oversampling but with a minimum limit */

#define myFloat double      /* float or double, to set the resolution of the FFT, etc. (the resulting wavetables are always float) */


typedef struct
{
    double topFreq;
    int waveTableLen;
    float *waveTable;
} waveTable;

const int numWaveTableSlots = 32;

class WaveTableOsc
{
public:
    WaveTableOsc(void);
    ~WaveTableOsc(void);
    
    void setFrequency(double inc);
    void setSampleRate(double sr) { sample_Rate = sr; }
    void setPhaseOffset(double offset);
    
    float getSampleRate() { return sample_Rate; }
    
    void updatePhase(void);
    
    float getOutput(void);
    float getOutputMinusOffset(void);
    
    // for our phase parameter // set phaseParam var..
    void setPhaseOffset2(double offset);
    
    
    void processBuffer(float * buffer, int numSamples, bool shouldProcess);
    void processBuffer2(float * buffer, int numSamples, bool shouldProcess);
    
    bool now;
    bool getNow() { return now; }
    
    double getPhasorValue() { return phasor; }
    
    bool phasorIsAt0() {
        if (phasor == 0.000) { return true; }
        else return false;
    }
    
    
    void deleteCurrentTable()
    {
        for (int idx = 0; idx < numWaveTableSlots; idx++)
        {
            float *temp = waveTables[idx].waveTable;
            if (temp != 0) temp = 0;
//            if (temp != 0) delete [] temp;
        }
    }
    
    void renderCustomWave(float * waveBuffer)
    {
        // receives new wave buffer, reprocesses custom
    }
    
    
    
    // 1. check frequency range, decrease base freq to 6/7 hz
    // 2. sample rate changes
    
    // 3. fix process buffer
    // 4. fix external functions
    // 5. switch default waveforms from view
    // 6. create new custom waveform from view, with interpolation
    // 7. load table from file
    // 8. save table to file
    int addWaveTable(int len, float *waveTableIn, double topFreq);
    
    static void fillTables(WaveTableOsc *osc, double *freqWaveRe, double *freqWaveIm, int numSamples);
    static void fft(int N, myFloat *ar, myFloat *ai);
    static WaveTableOsc *waveOsc(double *waveSamples, int tableLen);
    static float makeWaveTable(WaveTableOsc *osc, int len, myFloat *ar, myFloat *ai, myFloat scale, double topFreq);
    
    
    void defineSawtooth(int len, int numHarmonics, myFloat *ar, myFloat *ai);
    void defineSquare(int len, int numHarmonics, myFloat *ar, myFloat *ai);

    
    void setSawtoothOsc(float baseFreq);
    void setSquareOsc(float baseFreq);

private:
    float sample_Rate;
    
protected:
    double phasor;      // phase accumulator
    double phaseInc;    // phase increment
    double phaseOfs;    // phase offset for PWM
    double phaseParam; // our phase parameter
    
    // list of wavetables
    int numWaveTables;
    waveTable waveTables[numWaveTableSlots];
    
    float * customWaveTable; // synchronises with gui object,
    
    void processLinearInterpForCustomWaveTable()
    {
        
    }
};

//
// if scale is 0, auto-scales
// returns scaling factor (0.0 if failure), and wavetable in ai array
//


// note: if you don't keep this in the range of 0-1, you'll need to make changes elsewhere
inline void WaveTableOsc::setFrequency(double inc) {
    phaseInc = inc;
}

// note: if you don't keep this in the range of 0-1, you'll need to make changes elsewhere
inline void WaveTableOsc::setPhaseOffset(double offset) {
    phaseOfs = offset;
}

inline void WaveTableOsc::setPhaseOffset2(double offset)
{
    phaseParam = offset;
}

inline void WaveTableOsc::updatePhase()
{
    phasor += phaseInc;
    
    now = false;
    
    if (phasor >= 1.0)
    {
         phasor -= 1.0;
         now = true;
    }
}

