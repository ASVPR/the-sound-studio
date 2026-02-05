/*
  ==============================================================================

    WaveTableOsc.cpp
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#include "WaveTableOsc.h"

WaveTableOsc::WaveTableOsc(void)
{
    phaseParam = 0.0;
    phasor = 0.0;
    phaseInc = 0.0;
    phaseOfs = 0.5;
    numWaveTables = 0;
    now = false;
    sample_Rate = 44100;
    
    for (int idx = 0; idx < numWaveTableSlots; idx++)
    {
        waveTables[idx].topFreq = 0;
        waveTables[idx].waveTableLen = 0;
        waveTables[idx].waveTable = 0;
    }
    
    // init default wavetable
    
//    setSawtoothOsc(baseFrequency);
    
}


WaveTableOsc::~WaveTableOsc(void)
{

}


float WaveTableOsc::makeWaveTable(WaveTableOsc *osc, int len, myFloat *ar, myFloat *ai, myFloat scale, double topFreq) {
    fft(len, ar, ai);
    
    if (scale == 0.0) {
        // calc normal
        myFloat max = 0;
        for (int idx = 0; idx < len; idx++) {
            myFloat temp = fabs(ai[idx]);
            if (max < temp)
                max = temp;
        }
        scale = 1.0 / max * .999;
    }
    
    // normalize
    float *wave = new float[len];
    
    for (int idx = 0; idx < len; idx++)
        wave[idx] = ai[idx] * scale;
    
    if (osc->addWaveTable(len, wave, topFreq))
        scale = 0.0;

	delete [] wave;
	    
    return scale;
}


//
// fillTables:
//
// The main function of interest here; call this with a pointer to an new, empty oscillator,
// and the real and imaginary arrays and their length. The function fills the oscillator with
// all wavetables necessary for full-bandwidth operation.
//
void WaveTableOsc::fillTables(WaveTableOsc *osc, double *freqWaveRe, double *freqWaveIm, int numSamples)
{
    int idx;
    
    // zero DC offset and Nyquist
    freqWaveRe[0] = freqWaveIm[0] = 0.0;
    freqWaveRe[numSamples >> 1] = freqWaveIm[numSamples >> 1] = 0.0;
    
    // determine maxHarmonic, the highest non-zero harmonic in the wave
    int maxHarmonic = numSamples >> 1;
    const double minVal = 0.000001; // -120 dB
    while ((fabs(freqWaveRe[maxHarmonic]) + fabs(freqWaveIm[maxHarmonic]) > minVal) && maxHarmonic) --maxHarmonic;
    
    // calculate topFreq for the initial wavetable
    // maximum non-aliasing playback rate is 1 / (2 * maxHarmonic), but we allow aliasing up to the
    // point where the aliased harmonic would meet the next octave table, which is an additional 1/3
    double topFreq = 2.0 / 3.0 / maxHarmonic;
    
    // for subsquent tables, double topFreq and remove upper half of harmonics
    double *ar = new double [numSamples];
    double *ai = new double [numSamples];
    double scale = 0.0;
    while (maxHarmonic) {
        // fill the table in with the needed harmonics
        for (idx = 0; idx < numSamples; idx++)
            ar[idx] = ai[idx] = 0.0;
        for (idx = 1; idx <= maxHarmonic; idx++) {
            ar[idx] = freqWaveRe[idx];
            ai[idx] = freqWaveIm[idx];
            ar[numSamples - idx] = freqWaveRe[numSamples - idx];
            ai[numSamples - idx] = freqWaveIm[numSamples - idx];
        }
        
        // make the wavetable
        scale = makeWaveTable(osc, numSamples, ar, ai, scale, topFreq);
        
        // prepare for next table
        topFreq *= 2;
        maxHarmonic >>= 1;
    }
    
    // MEMORY LEAK FIX: The arrays 'ar' and 'ai' were allocated with new[] earlier in this function
    // but were never being freed, causing memory leaks on every oscillator creation.
    // Proper RAII cleanup ensures memory is freed when function exits.
    delete [] ar;
    delete [] ai;
}
//
// example that creates and oscillator from an arbitrary time domain wave
//
WaveTableOsc * WaveTableOsc::waveOsc(double *waveSamples, int tableLen)
{
    int idx;
    double *freqWaveRe = new double [tableLen];
    double *freqWaveIm = new double [tableLen];
    
    // take FFT
    for (idx = 0; idx < tableLen; idx++)
    {
        freqWaveIm[idx] = waveSamples[idx];
        freqWaveRe[idx] = 0.0;
    }
    fft(tableLen, freqWaveRe, freqWaveIm);
    
    // build a wavetable oscillator
    WaveTableOsc *osc = new WaveTableOsc();
    fillTables(osc, freqWaveRe, freqWaveIm, tableLen);
    
    // Fixed memory leak - cleanup allocated arrays
    delete [] freqWaveRe;
    delete [] freqWaveIm;
    
    return osc;
}

void WaveTableOsc::fft(int N, myFloat *ar, myFloat *ai)
{
    int i, j, k, L;            /* indexes */
    int M, TEMP, LE, LE1, ip;  /* M = log N */
    int NV2, NM1;
    myFloat t;               /* temp */
    myFloat Ur, Ui, Wr, Wi, Tr, Ti;
    myFloat Ur_old;
    
    // if ((N > 1) && !(N & (N - 1)))   // make sure we have a power of 2
    
    NV2 = N >> 1;
    NM1 = N - 1;
    TEMP = N; /* get M = log N */
    M = 0;
    while (TEMP >>= 1) ++M;
    
    /* shuffle */
    j = 1;
    for (i = 1; i <= NM1; i++) {
        if(i<j) {             /* swap a[i] and a[j] */
            t = ar[j-1];
            ar[j-1] = ar[i-1];
            ar[i-1] = t;
            t = ai[j-1];
            ai[j-1] = ai[i-1];
            ai[i-1] = t;
        }
        
        k = NV2;             /* bit-reversed counter */
        while(k < j) {
            j -= k;
            k /= 2;
        }
        
        j += k;
    }
    
    LE = 1.;
    for (L = 1; L <= M; L++) {            // stage L
        LE1 = LE;                         // (LE1 = LE/2)
        LE *= 2;                          // (LE = 2^L)
        Ur = 1.0;
        Ui = 0.;
        Wr = cos(juce::double_Pi/(float)LE1);
        Wi = -sin(juce::double_Pi/(float)LE1); // Cooley, Lewis, and Welch have "+" here
        for (j = 1; j <= LE1; j++) {
            for (i = j; i <= N; i += LE) { // butterfly
                ip = i+LE1;
                Tr = ar[ip-1] * Ur - ai[ip-1] * Ui;
                Ti = ar[ip-1] * Ui + ai[ip-1] * Ur;
                ar[ip-1] = ar[i-1] - Tr;
                ai[ip-1] = ai[i-1] - Ti;
                ar[i-1]  = ar[i-1] + Tr;
                ai[i-1]  = ai[i-1] + Ti;
            }
            Ur_old = Ur;
            Ur = Ur_old * Wr - Ui * Wi;
            Ui = Ur_old * Wi + Ui * Wr;
        }
    }
}




void WaveTableOsc::defineSawtooth(int len, int numHarmonics, myFloat *ar, myFloat *ai)
{
    if (numHarmonics > (len >> 1))
        numHarmonics = (len >> 1);
    
    // clear
    for (int idx = 0; idx < len; idx++)
    {
        ai[idx] = 0;
        ar[idx] = 0;
    }
    
    // sawtooth
    for (int idx = 1, jdx = len - 1; idx <= numHarmonics; idx++, jdx--)
    {
        myFloat temp = -1.0 / idx;
        ar[idx] = -temp;
        ar[jdx] = temp;
    }
    
    // examples of other waves
    /*
     // square
     for (int idx = 1, jdx = len - 1; idx <= numHarmonics; idx++, jdx--) {
     myFloat temp = idx & 0x01 ? 1.0 / idx : 0.0;
     ar[idx] = -temp;
     ar[jdx] = temp;
     }
     */
    /*
     // triangle
     float sign = 1;
     for (int idx = 1, jdx = len - 1; idx <= numHarmonics; idx++, jdx--) {
     myFloat temp = idx & 0x01 ? 1.0 / (idx * idx) * (sign = -sign) : 0.0;
     ar[idx] = -temp;
     ar[jdx] = temp;
     }
     */
}

void WaveTableOsc::defineSquare(int len, int numHarmonics, myFloat *ar, myFloat *ai)
{
    if (numHarmonics > (len >> 1))
        numHarmonics = (len >> 1);
    
    // clear
    for (int idx = 0; idx < len; idx++)
    {
        ai[idx] = 0;
        ar[idx] = 0;
    }
    

     // square
     for (int idx = 1, jdx = len - 1; idx <= numHarmonics; idx++, jdx--)
     {
        myFloat temp = idx & 0x01 ? 1.0 / idx : 0.0;
        ar[idx] = -temp;
        ar[jdx] = temp;
     }
}

//void WaveTableOsc::defineCustom(int len, int numHarmonics, myFloat *ar, myFloat *ai)
//{
//    if (numHarmonics > (len >> 1))
//        numHarmonics = (len >> 1);
//
//    // clear
//    for (int idx = 0; idx < len; idx++)
//    {
//        ai[idx] = 0;
//        ar[idx] = 0;
//    }
//
//
//    // square // need to mod this... to read file...
//    for (int idx = 1, jdx = len - 1; idx <= numHarmonics; idx++, jdx--)
//    {
//        myFloat temp = idx & 0x01 ? 1.0 / idx : 0.0;
//        ar[idx] = -temp;
//        ar[jdx] = temp;
//    }
//}

//void WaveTableOsc::setCustomOsc(WaveTableOsc *osc, float baseFreq, int stackRef) {
//    // calc number of harmonics where the highest harmonic baseFreq and lowest alias an octave higher would meet
//
//    float sample_Rate = osc->getSampleRate();
//    
//    int maxHarms = sample_Rate / (3.0 * baseFreq) + 0.5;
//
//    // round up to nearest power of two
//    unsigned int v = maxHarms;
//    v--;            // so we don't go up if already a power of 2
//    v |= v >> 1;    // roll the highest bit into all lower bits...
//    v |= v >> 2;
//    v |= v >> 4;
//    v |= v >> 8;
//    v |= v >> 16;
//    v++;            // and increment to power of 2
//
//    int tableLen = v * 2 * overSamp;  // double for the sample rate, then oversampling
//
//        //myFloat ar[tableLen], ai[tableLen];   // for ifft
//
//	double *ar = new double[tableLen];
//	double *ai = new double[tableLen];
//
//    double topFreq = baseFreq * 2.0 / sample_Rate;
//
//    myFloat scale = 0.0;
//
//    for (; maxHarms >= 1; maxHarms >>= 1)
//    {
//        defineCustom(tableLen, maxHarms, ar, ai);
//
//        scale = makeWaveTable(osc, tableLen, ar, ai, scale, topFreq);
//
//        topFreq *= 2;
//        if (tableLen > constantRatioLimit) // variable table size (constant oversampling but with minimum table size)
//            tableLen >>= 1;
//    }
//
//    delete [] ar;
//	delete [] ai;
//}


void WaveTableOsc::setSawtoothOsc(float baseFreq)
{
    float sample_Rate = getSampleRate();
    
    // calc number of harmonics where the highest harmonic baseFreq and lowest alias an octave higher would meet
    int maxHarms = sample_Rate / (3.0 * baseFreq) + 0.5;
    
    // round up to nearest power of two
    unsigned int v = maxHarms;
    v--;            // so we don't go up if already a power of 2
    v |= v >> 1;    // roll the highest bit into all lower bits...
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;            // and increment to power of 2
    int tableLen = v * 2 * overSamp;  // double for the sample rate, then oversampling
    
        //myFloat ar[tableLen], ai[tableLen];   // for ifft

    double *ar = new double[tableLen];
    double *ai = new double[tableLen];
    
    double topFreq = baseFreq * 2.0 / sample_Rate;
    myFloat scale = 0.0;
    
    for (; maxHarms >= 1; maxHarms >>= 1)
    {
        defineSawtooth(tableLen, maxHarms, ar, ai);
        
        scale = makeWaveTable(this, tableLen, ar, ai, scale, topFreq);
        
        topFreq *= 2;
        if (tableLen > constantRatioLimit) // variable table size (constant oversampling but with minimum table size)
            tableLen >>= 1;
    }
    
    delete [] ar;
    delete [] ai;
}

void WaveTableOsc::setSquareOsc(float baseFreq)
{
    float sample_Rate = getSampleRate();
    
    // calc number of harmonics where the highest harmonic baseFreq and lowest alias an octave higher would meet
    int maxHarms = sample_Rate / (3.0 * baseFreq) + 0.5;
    
    // round up to nearest power of two
    unsigned int v = maxHarms;
    v--;            // so we don't go up if already a power of 2
    v |= v >> 1;    // roll the highest bit into all lower bits...
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;            // and increment to power of 2
    int tableLen = v * 2 * overSamp;  // double for the sample rate, then oversampling
    
        //myFloat ar[tableLen], ai[tableLen];   // for ifft

    double *ar = new double[tableLen];
    double *ai = new double[tableLen];
    
    double topFreq = baseFreq * 2.0 / sample_Rate;
    myFloat scale = 0.0;
    
    for (; maxHarms >= 1; maxHarms >>= 1)
    {
        defineSquare(tableLen, maxHarms, ar, ai);
        
        scale = makeWaveTable(this, tableLen, ar, ai, scale, topFreq);
        
        topFreq *= 2;
        if (tableLen > constantRatioLimit) // variable table size (constant oversampling but with minimum table size)
            tableLen >>= 1;
    }
    
    delete [] ar;
    delete [] ai;
}





void WaveTableOsc::processBuffer(float * buffer, int numSamples, bool shouldProcess)
{
    for (int i = 0 ; i < numSamples; i++)
    {
        if (shouldProcess)
        {
            buffer[i] = this->getOutput();
            this->updatePhase();
        }
        else
        {
            this->updatePhase();
        }
    }
}

void WaveTableOsc::processBuffer2(float * buffer, int numSamples, bool shouldProcess)
{
        if (shouldProcess)
        {
            *buffer = this->getOutput();
            this->updatePhase();
        }
        else
        {
            this->updatePhase();
        }
}


//
// addWaveTable
//
// add wavetables in order of lowest frequency to highest
// topFreq is the highest frequency supported by a wavetable
// wavetables within an oscillator can be different lengths
//
// returns 0 upon success, or the number of wavetables if no more room is available
//
int WaveTableOsc::addWaveTable(int len, float *waveTableIn, double topFreq)
{
    if (this->numWaveTables < numWaveTableSlots)
    {
        float *waveTable = this->waveTables[this->numWaveTables].waveTable = new float[len];
        this->waveTables[this->numWaveTables].waveTableLen = len;
        this->waveTables[this->numWaveTables].topFreq = topFreq;
        ++this->numWaveTables;
        
        // fill in wave
        for (long idx = 0; idx < len; idx++)
            waveTable[idx] = waveTableIn[idx];
        
        return 0;
    }
    return this->numWaveTables;
}


//
// getOutput
//
// returns the current oscillator output
//
float WaveTableOsc::getOutput()
{
    // grab the appropriate wavetable
    int waveTableIdx = 0;
    
    while ((this->phaseInc >= this->waveTables[waveTableIdx].topFreq) && (waveTableIdx < (this->numWaveTables - 1)))
    {
        ++waveTableIdx;
    }
    
    waveTable *waveTable = &this->waveTables[waveTableIdx];
    
#if !doLinearInterp
    
    // truncate
    return waveTable->waveTable[int(this->phasor * waveTable->waveTableLen)];

#else
    
    // linear interpolation

    double temp = this->phasor * waveTable->waveTableLen;
    
    // phase param code
    double tempPhaseShift = this->phaseParam * waveTable->waveTableLen;
    
    temp+=tempPhaseShift;
    
    if (temp > waveTable->waveTableLen)
    {
        temp-= waveTable->waveTableLen;
    }

    
    int intPart = temp;
    
    double fracPart = temp - intPart;
    
    //:::: crash bug here caused by intPart value wrong at launch :::::*** needs investigation
    float samp0 = waveTable->waveTable[intPart];
    
    if (++intPart >= waveTable->waveTableLen)
        intPart = 0;
    
    float samp1 = waveTable->waveTable[intPart];
    
    return samp0 + (samp1 - samp0) * fracPart;
    
#endif
}


//
// getOutputMinusOffset
//
// for variable pulse width: initialize to sawtooth,
// set phaseOfs to duty cycle, use this for osc output
//
// returns the current oscillator output
//
float WaveTableOsc::getOutputMinusOffset()
{
    // grab the appropriate wavetable
    int waveTableIdx = 0;
    
    while ((this->phaseInc >= this->waveTables[waveTableIdx].topFreq) && (waveTableIdx < (this->numWaveTables - 1))) {
        ++waveTableIdx;
    }
    
    waveTable *waveTable = &this->waveTables[waveTableIdx];
    
#if !doLinearInterp
    // truncate
    double offsetPhasor = this->phasor + this->phaseOfs;
    
    if (offsetPhasor >= 1.0)
        offsetPhasor -= 1.0;
    
    return waveTable->waveTable[int(this->phasor * waveTable->waveTableLen)] - waveTable->waveTable[int(offsetPhasor * waveTable->waveTableLen)];
#else
    // linear
    double temp = this->phasor * waveTable->waveTableLen;
    int intPart = temp;
    double fracPart = temp - intPart;
    float samp0 = waveTable->waveTable[intPart];
    
    if (++intPart >= waveTable->waveTableLen)
        intPart = 0;
    
    float samp1 = waveTable->waveTable[intPart];
    float samp = samp0 + (samp1 - samp0) * fracPart;
    
    // and linear again for the offset part
    double offsetPhasor = this->phasor + this->phaseOfs;
    
    if (offsetPhasor > 1.0)
        offsetPhasor -= 1.0;
    
    temp = offsetPhasor * waveTable->waveTableLen;
    intPart = temp;
    fracPart = temp - intPart;
    samp0 = waveTable->waveTable[intPart];
    
    if (++intPart >= waveTable->waveTableLen)
        intPart = 0;
    samp1 = waveTable->waveTable[intPart];
    
    return samp - (samp0 + (samp1 - samp0) * fracPart);
#endif
}




