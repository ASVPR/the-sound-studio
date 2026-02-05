/*
  ==============================================================================

    Oscillators.h
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#pragma once
#include <cmath>
#include "../JuceLibraryCode/JuceHeader.h"
#include "LUTs.h"


// C++17: Replace macros with constexpr constants for better type safety
namespace OscConstants {
    constexpr double FREQUENCY_MIN = 2.0;
    constexpr double FREQUENCY_MAX = 20480.0;
    constexpr double FREQUENCY_DEFAULT = 432.0;
    constexpr double PULSEWIDTH_DEFAULT = 50.0;
}

class Osc
{
public:
    // C++17: Use enum class for better type safety
    enum class OSC_TYPE : int { SINEWAVE = 1, TRIANGLE, SQUARE, SAWTOOTH };
    
private:
    OSC_TYPE osc_type;
    
public:
    void setOscillatorType(OSC_TYPE newType) noexcept { osc_type = newType; }
    constexpr OSC_TYPE getOscillatorType() const noexcept { return osc_type; }
    
    // C++17: Use member initializer list for better performance
    Osc() noexcept
        : osc_type(OSC_TYPE::SINEWAVE)
        , sampleRate(44100.0)
        , isNoteOn(false)
        , midiNote(0)
        , modulo(0.0)
        , inc(0.0)
        , oscfrequency(OscConstants::FREQUENCY_DEFAULT)
        , frequency(OscConstants::FREQUENCY_DEFAULT)
        , pulseWidth(OscConstants::PULSEWIDTH_DEFAULT)
        , squareModulator(-1.0)
        , z1Register(0.0)
        , phaseMod(0.0)
        , ratio(1.0)
        , semitone(0.0f)
        , cents(0.0)
        , octave(0.0)
        , manipulateFrequency(false)
        , multiplyDivide(false)
        , multiplyValue(1.0)
        , divideValue(1.0)
    {
    }
    
    virtual ~Osc(void ) { }
    
    void setSampleRate(double fs)
    {
        sampleRate = fs; update();
    }
    
    // frequency management
    void setFrequency(float newFrequency)
    {
        oscfrequency = newFrequency; update();
    }
    
    constexpr double getFrequency() const noexcept { return frequency; }
    
    
    void setOctave(int octaveShift)
    {
        octave = octaveShift; update();
    }
    
    void setManipulateFrequency(bool should)
    {
        manipulateFrequency = should; update();
    }
    
    void setMultiplyDivide(bool should)
    {
        multiplyDivide = should; update();
    }
    
    void setMultiplyValue(float val)
    {
        multiplyValue = val; update();
    }
    
    void setDivideValue(float val)
    {
        divideValue = val; update();
    }
    
    
    
    void reset()
    {
        modulo              = 0.0;
        squareModulator     = -1.0;
        z1Register          = 0.0;
        
        if (osc_type == OSC_TYPE::SAWTOOTH || osc_type == OSC_TYPE::TRIANGLE)
        {
            modulo = 0.5;
        }
    }
    
    void startOscillator()
    {
        reset(); isNoteOn = true;
    }
    
    void stopOscillator()
    {
        reset(); isNoteOn = false;
    }
    
    void update()
    {
        if (manipulateFrequency)
        {
            if (multiplyDivide)
            {
                // multiply
                frequency = oscfrequency * pitchShiftMultiplier(octave * 12) * multiplyValue;
            }
            else
            {
                // divide // check divide != 0 before...
                frequency = oscfrequency * pitchShiftMultiplier(octave * 12) / divideValue;
            }
        }
        else
        {
            frequency = oscfrequency * pitchShiftMultiplier(octave * 12);
        }
        
        
        
        if (frequency > OscConstants::FREQUENCY_MAX)  { frequency = OscConstants::FREQUENCY_MAX; }
        if (frequency < OscConstants::FREQUENCY_MIN)  { frequency = OscConstants::FREQUENCY_MIN; }
        
        // calc incrememt
        inc = frequency / sampleRate;
    }
    
    
    double doOscillate()
    {
        double oscOutput = 0.0;
        
        bool shouldWrap = checkModulo();
        
        double calcModulo = modulo + phaseMod;
        
        checkWrap(calcModulo);
        
        switch (osc_type)
        {
            case OSC_TYPE::SINEWAVE:
            {
                double angle = calcModulo * 2.0 * (double)M_PI - (double)M_PI;
                
                oscOutput = parabolicSinewave(-1.0 * angle);
            }
                
                break;
                
            case OSC_TYPE::TRIANGLE:
            {
                if (shouldWrap)
                {
                    squareModulator *= -1.0;
                }
                
                oscOutput = generateTriangle(calcModulo, inc, frequency, squareModulator, &z1Register);
            }
                break;
                
            case OSC_TYPE::SQUARE:
            {
                oscOutput = generateSquare(calcModulo, inc);
            }
                break;
                
            case OSC_TYPE::SAWTOOTH:
            {
                oscOutput = generateSawtooth(calcModulo, inc);
            }
                break;
                
            default: break;
        }
        
        incModulo(); if (osc_type == OSC_TYPE::TRIANGLE) { incModulo(); }
        
        return oscOutput;
    }
    
private:
    void incModulo() { modulo += inc; }
    
    bool checkModulo()
    {
        // check for positive frequencies
        if (inc > 0 && modulo >= 1.0) { modulo -= 1.0; return true; }
        
        // check for negative frequencies
        if (inc < 0 && modulo <= 0.0) { modulo += 1.0; return true; }
        
        return false;
    }
    
    void checkWrap(double &_index)
    {
        while(_index < 0.0) { _index += 1.0; }
        
        while (_index >= 1.0) { _index -= 1.0;  }
    }
    
    void resetModulo(double d) {  modulo = d;  }
    
    double pitchShiftMultiplier(double pitchShiftInSemitones)
    {
        if (pitchShiftInSemitones == 0) { return 1.0; }
        
        // PERFORMANCE OPTIMIZATION: std::exp2() is significantly faster than pow(2.0, x)
        // for power-of-2 calculations. Critical for real-time audio processing.
        return std::exp2(pitchShiftInSemitones / 12.0);
    }
    
    double unipolarToBipolar(double value)
    {
        return 2.0 * value - 1.0;
    }
    
    float linearInterpolator(float x1, float x2, float y1, float y2, float x)
    {
        float denominator = x2 - x1;
        
        if (denominator == 0) { return y1; }
        
        // calc decimal position of x
        float dx = (x - x1) / (x2 - x1);
        
        // sum method of interpolator
        float result = dx * y2 + (1 - dx) * y1;
        
        return result;
    }
    
    double lagrangeInterpolator(double * x, double * y, int n, double xbar)
    {
        // C++17: Use auto for variable declarations
        auto fx = 0.0;
        
        for (int i = 0; i < n; i++)
        {
            auto l = 1.0;
            for (int j = 0; j < n; j++)
            {
                if (j != i)
                {
                    l *= ( xbar - x[j] ) / (x[i] - x[j]);
                }
            }
            
            fx += l * y[i];
        }
        
        return fx;
    }
    
    double generateTriangle(double _modulo, double _inc, double _frequency, double _squareMod, double * _register)
    {
        double bipolar = unipolarToBipolar(_modulo);
        
        double square = bipolar*bipolar;
        
        double inv = 1.0 - square;
        
        double squareMod = inv * _squareMod;
        
        double differentiate_squareMod = squareMod - *_register;
        
        *_register = squareMod;
        
        double c = sampleRate / ( 4.0 * 2.0 * _frequency * (1 - _inc));
        
        return differentiate_squareMod * c;
    }
    
    double generateSawtooth (double _modulo, double _inc)
    {
        double trivialSaw = 0.0;
        double sawOut = 0.0;
        
        trivialSaw = unipolarToBipolar(_modulo);
        
        if (frequency <= sampleRate / 8.0)
        {
            sawOut = trivialSaw + doBLEP(&dBLEPTable_8_BLKHAR[0], 4096, _modulo, fabs(_inc), 1.0, false, 4, false);
        }
        else
        {
            sawOut = trivialSaw + doBLEP(&dBLEPTable[0], 4096, _modulo, fabs(_inc), 1.0, false, 1, true);
        }
        
        return sawOut;
    }
    
    double generateSquare (double _modulo, double _inc)
    {
        // do sawtooth first
        double saw1 = generateSawtooth(_modulo, _inc);
        
        // phase shift on 2nd osc
        if (_inc > 0) {  _modulo += pulseWidth/100.0; } else {  _modulo -= pulseWidth/100.0; }
        
        // pos frequencies
        if (_inc > 0 && _modulo >= 1.0) { _modulo -= 1.0; }
        
        // neg frequencies
        if (_inc < 0 && _modulo <= 0.0) { _modulo += 1.0; }
        
        // subtract saw
        double saw2 = generateSawtooth(_modulo, _inc);
        
        // 180 out of phase
        double output = 0.5 * saw1 - 0.5 * saw2;
        
        // DC Correct
        double dcCorrection = 1.0 / (pulseWidth/100.0);
        
        if ((pulseWidth/100.0) < 0.5) {  dcCorrection = 1.0 / (1.0 - (pulseWidth/100));  }
        
        // apply correctiom
        output *= dcCorrection;
        
        return output;
    }
    
    double doBLEP(const double * BLEPTable, double _tableLength, double _modulo, double _inc, double _height, bool risingEdge, double _pointsPerSide, bool _interpolate = false)
    {
        double blep = 0.0;
        
        double t = 0.0;
        
        double tableCentre = _tableLength/2.0 - 1;
        
        // Left side of edge
        for (int i = 1; i <= (uint)_pointsPerSide ; i++)
        {
            if (_modulo > 1.0 - (double) i * _inc )
            {
                //calc distance
                t = (_modulo - 1.0) / (_pointsPerSide * _inc);
                
                // get index
                float index = (1.0 + t) * tableCentre;
                
                // SECURITY FIX: Added bounds checking to prevent buffer overflow attacks
                // Previous code could access memory outside array bounds if index was manipulated
                int intIndex = (int)index;
                if (intIndex < 0 || intIndex >= (int)_tableLength) return 0.0; // Bounds validation
                
                if (_interpolate) 
                {  
                    blep = BLEPTable[intIndex]; 
                }
                else
                {
                    float frac = index - intIndex;
                    // Ensure we don't access beyond array bounds
                    if (intIndex + 1 >= (int)_tableLength)
                    {
                        blep = BLEPTable[intIndex]; // Use current value if next is out of bounds
                    }
                    else
                    {
                        blep = linearInterpolator(0, 1, BLEPTable[intIndex], BLEPTable[intIndex + 1], frac);
                    }
                }
                
                // subtract falling or add rising edge
                if (!risingEdge) {  blep *= -1.0; }
                
                return _height * blep;
            }
        }
        
        // right side of discontinuity
        for (int i = 1; i <= (uint) _pointsPerSide; i++)
        {
            if (_modulo < (double)i * _inc)
            {
                // calc distance
                t = _modulo / (_pointsPerSide * _inc);
                
                float index = t * tableCentre + (tableCentre + 1.0);
                
                // SECURITY FIX: Additional bounds checking for array access protection
                // Prevents potential buffer overflows in audio processing critical path
                int intIndex = (int)index;
                if (intIndex < 0 || intIndex >= (int)_tableLength) return 0.0; // Bounds validation
                
                if (_interpolate) 
                { 
                    blep = BLEPTable[intIndex]; 
                }
                else
                {
                    float frac = index - intIndex;
                    if (intIndex + 1 >= (int)_tableLength)
                    {
                        blep = linearInterpolator(0, 1, BLEPTable[intIndex], BLEPTable[0], frac);
                    }
                    else
                    {
                        blep = linearInterpolator(0, 1, BLEPTable[intIndex], BLEPTable[intIndex + 1], frac);
                    }
                }
                
                if (!risingEdge) { blep *= -1.0; }
                
                return _height * blep;
            }
        }
        return 0.0;
    }
    
    double parabolicSinewave(double x, bool highPrecision = true)
    {
        double y = B1 * x + C1 * x * fabs(x);
        
        if (highPrecision)
        {
            y = P1 * (y * fabs(y) - y) + y;
        }
        
        return y;
    }
    
    

protected:
    bool isNoteOn;
    double oscfrequency;
    double sampleRate;
    double frequency;
    double pulseWidth;
    double squareModulator;
    double z1Register;
    double phaseMod;
    double ratio;
    int octave;
    int semitone;
    int cents;
    double modulo;
    double inc;
    int midiNote;
    
    const double B1 = 4.0/(float)M_PI;
    const double C1 = -4.0/((float)M_PI*(float)M_PI);
    const double P1 = 0.225;
    
    // extra params
    bool manipulateFrequency;
    bool multiplyDivide;
    float multiplyValue;
    float divideValue;
};
