/*
  ==============================================================================

    OSCManager.h
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#pragma once
#ifndef OSCMANAGER_H_INCLUDED
#define OSCMANAGER_H_INCLUDED
#include "VAOscillator.h"


// base class for oscillators, handles triggers, samplerates, frequencies etc..
class OscillatorBase
{
public:
    OscillatorBase(double newSampleRate, int oversample)
    {
        sample_rate         = newSampleRate;
        oversample_factor   = oversample;
    }
    
    virtual ~OscillatorBase(){}
    
    // virtual funcs to be overriden by oscillators
    virtual void process_block(float * buffer, int numSamples) = 0;
    virtual void reset() = 0;
    virtual void update_coefficients() = 0;
    virtual void set_sub_type(int newSubType) = 0;
    virtual void set_defaults() = 0;
    virtual void start() = 0; // trigger phase
    virtual void stop() = 0; // stop osc processing, called on release/end note..
    
    void set_midi_note_frequency(int midiNote)
    {
//        midiNote += (octave_shift * 12);
        
        if (midiNote <= 0) midiNote = 0;
        if (midiNote >= 127) midiNote = 127;
        
        double fr = 440 * powf (2.0, (midiNote - 69) / 12.0); // add + glide here before its sent to set_freq...
        
        set_frequency(fr);
    }
    
    virtual void set_frequency(float  f)
    {
        frequency   = f;
        cur_freq    = frequency;
        update_coefficients();
    }
    
    void set_semitone(float  s)
    {
        semitones = s;
        update_coefficients();
    }
    
    void set_detune(float  s)
    {
        detune_cents = s;
        update_coefficients();
    }

    
    void set_octave_shift(float shift)
    {
        octave_shift = shift;
        update_coefficients();
    }
    
    void set_osc_type(int new_type)
    {
        osc_type = new_type;
        switch_to_new_oscillator(osc_type);
    }
    
    void switch_to_new_oscillator(int new_osc_type)
    {
        // force defaults onto new osc
        // set to main params somehow..
    }
    
    void setShouldFM(bool s)
    {
        shouldFM = s;
    }
    void setFreqModBuffer(float * modBuf)
    {
        freqMod_buf = modBuf;
    }

    void set_fm_amount(float fm)
    {
        fmAmount = fm;
    }
    
    void set_sample_rate(float newSampleRate)
    {
        sample_rate = newSampleRate;
        
    }
    

protected:
    double  sample_rate;
    int     oversample_factor;
    int     osc_type;
    float   frequency;
    float   cur_freq;
    float   detune_cents;
    float   semitones;
    float     octave_shift;
    
    bool shouldFM;
    float fmAmount;
    float * freqMod_buf;
};


class DigitalVAOSC : public OscillatorBase
{
public:
    DigitalVAOSC(double newSampleRate, int oversample) : OscillatorBase(newSampleRate, oversample)
    {
        osc.m_uWaveform = 9;
        osc.setSampleRate(sample_rate * oversample_factor);
        osc.reset();
    }
    
    ~DigitalVAOSC() { }
    
    void set_new_samplerate(double newRate)
    {
        sample_rate = newRate; set_sub_type(sub_type);
    }
    
    virtual void process_block(float * buffer, int numSamples) override
    {
        for (int s = 0; s < numSamples; s++)
        {
            buffer[s] = (float)osc.doOscillate();
        }
    }
    
    void setPhase(double newPhase)
    {
        osc.m_Phase = newPhase;
    }
    
    virtual void set_sub_type(int newSubType) override
    {
        sub_type = (WAVEFORM_TYPES) newSubType;
        
        switch (sub_type) {
            case SINE:
            {
                osc.m_uWaveform = 0;
                osc.setSampleRate(sample_rate * oversample_factor);
                osc.reset();
            }
                break;
            case TRIANGLE:
            {
                osc.m_uWaveform = 4;
                osc.setSampleRate(sample_rate * oversample_factor);
                osc.reset();
            }
                break;
            case SAWTOOTH:
            {
                osc.m_uWaveform = 1;
                osc.setSampleRate(sample_rate * oversample_factor);
                osc.reset();
            }
                break;
            case SQUARE:
            {
                osc.m_uWaveform = 5;
                osc.setSampleRate(sample_rate * oversample_factor);
                osc.reset();
            }
                break;
                
            default:
            {
                osc.m_uWaveform = 0;
                osc.setSampleRate(sample_rate * oversample_factor);
                osc.reset();
            }
                break;
        }
    }
    
    virtual void reset() override
    {
        osc.reset();
    }
    virtual void update_coefficients() override
    {
        osc.m_nSemitones    = semitones;
        osc.m_nOctave       = octave_shift;
        osc.m_dOscFo        = frequency;
        osc.m_nCents        = detune_cents;
        
        osc.update();
    }
    
    virtual void set_defaults() override
    {
        
    }
    
    virtual void start() override  // call on trigger note
    {
        osc.startOscillator();
    }
    
    virtual void stop() override  // call on total release of envelope / voice
    {
        osc.stopOscillator();
    }
    
    void setOctave(int octaveShift)
    {
        osc.setOctave(octaveShift);
    }
    
    void setManipulateFrequency(bool should)
    {
        osc.setManipulateFrequency(should);
    }
    
    void setMultiplyDivide(bool should)
    {
        osc.setMultiplyDivide(should);
    }
    
    void setMultiplyValue(float val)
    {
        osc.setMultiplyValue(val);
    }
    
    void setDivideValue(float val)
    {
        osc.setDivideValue(val);
    }
    
//    void set_midi_note_frequency(int midiNote);
    
    void setShouldFM(bool s) { shouldFM = s; }
    void setFreqModBuffer(float * modBuf) { freqMod_buf = modBuf; }
    
    enum WAVEFORM_TYPES
    {
        DEFAULT = 0, SINE,
        TRIANGLE, SQUARE, SAWTOOTH,
        TOTAL_NUM_WAVEFORM_TYPES
    } sub_type;
    
private:
    bool shouldFM;
    float pulse_width;
    
    float fm_amount;
    float * freqMod_buf;
    CQBLimitedOscillator osc;

};

 
class OSCManager2
{
public:
    OSCManager2(double newSampleRate) :  digitalVAOsc(newSampleRate, 1) { }
    
    ~OSCManager2() {  }
    
    void processBuffer(float * buffer, int numSamples)
    {
        digitalVAOsc.process_block(buffer, numSamples);
    }
    
    void setParameter(int type, float value) { }
    
    void reset()
    {
        digitalVAOsc.reset();
    }
    
    void setPhase(double newPhase)
    {
        digitalVAOsc.setPhase(newPhase);
    }
    
    void set_new_samplerate(double newRate)
    {
        sample_rate = newRate;
        digitalVAOsc.set_new_samplerate(sample_rate);
        reset();
        
        digitalVAOsc.update_coefficients();
    }
    
    void SetMIDINoteFrequency(int midiNote)
    {
        digitalVAOsc.set_midi_note_frequency(midiNote);
    }
    void SetPitchMultiplier(float pitchMultiplier);
    void setOctaveShift(float shift)
    {
        octave_shift = shift;

        digitalVAOsc.set_octave_shift(octave_shift);
    }
    
    void set_type(int type)
    {
        osc_type = type;
        // do bounds check
        // force defaults
    }
    
    void set_subtype(int newSubType)
    {
        osc_subtype = newSubType;

        digitalVAOsc.set_sub_type(osc_subtype);
    }

    void set_frequency(float  f)
    {
        frequency = f;
        
        digitalVAOsc.set_frequency(f);
    }
    
    void set_semitone(float  s)
    {
        semitone = s;
        
        digitalVAOsc.set_semitone(semitone);
    }
    void set_detune(float  s)
    {
        detune = s;
        
        digitalVAOsc.set_detune(detune);
    }
    
    void setOctave(int octaveShift)
    {
        digitalVAOsc.setOctave(octaveShift);
    }
    
    void setManipulateFrequency(bool should)
    {
        digitalVAOsc.setManipulateFrequency(should);
    }
    
    void setMultiplyDivide(bool should)
    {
        digitalVAOsc.setMultiplyDivide(should);
    }
    
    void setMultiplyValue(float val)
    {
        digitalVAOsc.setMultiplyValue(val);
    }
    
    void setDivideValue(float val)
    {
        digitalVAOsc.setDivideValue(val);
    }
    

    
    void trigger_random_phase(); // sets random phase for each osc upon note trigger.
    
    void triggerNote()
    {
        digitalVAOsc.start();

    }

    // public so can query type for sync parameter.
    int     osc_type;
    int     osc_subtype;
    float   semitone;
    float     octave_shift;
    
private:
    int     over_sample_factor = 1;
    double  sample_rate;

    float   frequency;
    float   detune;
    
    bool    shouldFM;
    float   fmAmount;
    

private:
    DigitalVAOSC    digitalVAOsc;
};


#endif  // OSCMANAGER_H_INCLUDED
