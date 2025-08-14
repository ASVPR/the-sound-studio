/*
  ==============================================================================

    VAOscillator.h
    Created: 2 Jul 2019 9:16:39am
    Author:  Gary Jones

  ==============================================================================
*/

#pragma once

#include "pluginconstants.h"
#include "synthfunctions.h"
#include "Parameters.h"

#define OSC_FO_MOD_RANGE 2			//2 semitone default
#define OSC_HARD_SYNC_RATIO_RANGE 4	//
#define OSC_PITCHBEND_MOD_RANGE 12	//12 semitone default
#define OSC_FO_DEFAULT 440.0		//A5
#define OSC_PULSEWIDTH_MIN 2		//2%
#define OSC_PULSEWIDTH_MAX 98		//98%
#define OSC_PULSEWIDTH_DEFAULT 50	//50%



class QOscillator
{
public:
	QOscillator(void);
	virtual ~QOscillator(void);
    
	// --- ATTRIBUTES
	// --- PUBLIC: these variables may be get/set
	//             you may make get/set functions for them
	//             if you like, but will add function call layer
	// --- oscillator run flag
	bool m_bNoteOn;
    
	// --- user controls or MIDI
	double m_dOscFo;		// oscillator frequency from MIDI note number
	double m_dFoRatio;	    // FM Synth Modulator OR Hard Sync ratio
	double m_dAmplitude;	// 0->1 from GUI
	
	// ---  pulse width in % (sqr only) from GUI
	double m_dPulseWidthControl;
    
	// --- modulo counter and inc for timebase
	double m_dModulo;		// modulo counter 0->1
	double m_dInc;			// phase inc = fo/fs
    
    double m_Phase;         // -1 .. 1 phase shift to modulo, added by GJ..
    
	// --- more pitch mods
	double m_nOctave;			// octave tweak
	int m_nSemitones;		// semitones tweak
	int m_nCents;			// cents tweak
	
	// --- for PITCHED Oscillators
	enum {SINE=0,SAW1,SAW2,SAW3,TRI,SQUARE,NOISE,PNOISE,SIMPLESAW, SIMPLESQUARE, SIMPLETRIANGLE};
	UINT m_uWaveform;	// to store type
	
	// --- for LFOs
	enum {sine,usaw,dsaw,tri,square,expo,rsh,qrsh};
    
	// --- for LFOs - MODE
	enum {sync,shot,free};
	UINT m_uLFOMode;	// to store MODE
    
	// --- MIDI note that is being played
	UINT m_uMIDINoteNumber;
    
    
    bool manipulateFrequency;
    bool multiplyDivide;
    float multiplyValue;
    float divideValue;
	
protected:
	// --- PROTECTED: generally these are either basic calc variables
	//                and modulation stuff
	// --- calculation variables
	double m_dSampleRate;	// fs
	double m_dFo;			// current (actual) frequency of oscillator
	double m_dPulseWidth;	// pulse width in % for calculation
	
	// --- for noise and random sample/hold
	UINT   m_uPNRegister;	// for PN Noise sequence
	int    m_nRSHCounter;	// random sample/hold counter
	double m_dRSHValue;		// currnet rsh output
	
	// --- for DPW
	double m_dDPWSquareModulator;	// square toggle
	double m_dDPW_z1; // memory register for differentiator
	
	// --- mondulation inputs
	double m_dFoMod;			/* modulation input -1 to +1 */
	double m_dPitchBendMod;	    /* modulation input -1 to +1 */
	double m_dFoModLin;			/* FM modulation input -1 to +1 (not actually used in Yamaha FM!) */
	double m_dPhaseMod;			/* Phase modulation input -1 to +1 (used for DX synth) */
	double m_dPWMod;			/* modulation input for PWM -1 to +1 */
	double m_dAmpMod;			/* output amplitude modulation for AM 0 to +1 (not dB)*/
    
public:
	// --- FUNCTIONS: all public
	//
	// --- modulo functions for master/slave operation
	// --- increment the modulo counters
	inline void incModulo(){m_dModulo += m_dInc;}
    
	// --- check and wrap the modulo
	//     returns true if modulo wrapped
	inline bool checkWrapModulo()
	{
		// --- for positive frequencies
		if(m_dInc > 0 && m_dModulo >= 1.0)
		{
			m_dModulo -= 1.0;
			return true;
		}
		// --- for negative frequencies
		if(m_dInc < 0 && m_dModulo <= 0.0)
		{
			m_dModulo += 1.0;
			return true;
		}
		return false;
	}
    
    inline bool checkWrapModuloWithPhase() // GJ *** .. phase wrap over...
    {
        
        // --- for positive frequencies
        if(m_dInc > 0 && m_dModulo >= 1.0)
        {
            m_dModulo -= 1.0;
            return true;
        }
        // --- for negative frequencies
        if(m_dInc < 0 && m_dModulo <= 0.0)
        {
            m_dModulo += 1.0;
            return true;
        }
        return false;
    }
	
	// --- reset the modulo (required for master->slave operations)
	inline void resetModulo(double d = 0.0){m_dModulo = d;}
    
	// --- modulation functions - NOT needed/used if you implement the Modulation Matrix!
	//
	// --- output amplitude modulation (AM, not tremolo (dB); 0->1, NOT dB
	inline void setAmplitudeMod(double dAmp){m_dAmpMod = dAmp;}
    
	// --- modulation, exponential
	inline void setFoModExp(double dMod){m_dFoMod = dMod;}
	inline void setPitchBendMod(double dMod){m_dPitchBendMod = dMod;}
	
	// --- for FM only (not used in Yamaha or my DX synths!)
	inline void setFoModLin(double dMod){m_dFoModLin = dMod;}
    
	// --- for Yamaha and my DX Synth
	inline void setPhaseMod(double dMod){m_dPhaseMod = dMod;}
    
	// --- PWM for square waves only
	inline void setPWMod(double dMod){m_dPWMod = dMod;}
    
	virtual void startOscillator() = 0;
	virtual void stopOscillator() = 0;
	
	virtual double doOscillate(double* pAuxOutput = NULL) = 0;
    
	virtual void setSampleRate(double dFs){m_dSampleRate = dFs;}
	
	virtual void reset();
    
	inline virtual void update()
	{
		if(m_uWaveform == rsh || m_uWaveform == qrsh)
            m_uLFOMode = free;
        
//        if (manipulateFrequency)
//        {
//            if (multiplyDivide)
//            {
//                // multiply
//                m_dFo = m_dOscFo * pitchShiftMultiplierOctaves(m_nOctave) * multiplyValue;
//            }
//            else
//            {
//                // divide // check divide != 0 before...
//                 m_dFo = m_dOscFo * pitchShiftMultiplierOctaves(m_nOctave) / divideValue;
//            }
//        }
//        else
//        {
//            m_dFo = m_dOscFo * pitchShiftMultiplierOctaves(m_nOctave);
//        }
        
        m_dFo = m_dOscFo;
        

		if(m_dFo > FREQUENCY_MAX)
			m_dFo = FREQUENCY_MAX;
		if(m_dFo < -FREQUENCY_MAX)
			m_dFo = -FREQUENCY_MAX;
        
		m_dInc = m_dFo/m_dSampleRate;
        
		m_dPulseWidth = m_dPulseWidthControl + m_dPWMod*(OSC_PULSEWIDTH_MAX - OSC_PULSEWIDTH_MIN)/OSC_PULSEWIDTH_MIN;
        
		m_dPulseWidth = fmin(m_dPulseWidth, OSC_PULSEWIDTH_MAX);
		m_dPulseWidth = fmax(m_dPulseWidth, OSC_PULSEWIDTH_MIN);
	}
};



class CQBLimitedOscillator : public QOscillator
{
public:
	CQBLimitedOscillator(void);
	~CQBLimitedOscillator(void);
    
    void setOctave(int octaveShift)
    {
        m_nOctave = octaveShift; update();
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
    

	// --- inline functions for realtime remdering stuff
	//
    inline double doSimpleSawtooth(double dModulo, double dInc)
	{
		double dTrivialSaw = 0.0;
		double dOut = 0.0;
        
		if(m_uWaveform == SAW1)			// SAW1 = normal sawtooth (ramp)
			dTrivialSaw = unipolarToBipolar(dModulo);
		else if(m_uWaveform == SAW2)	// SAW2 = one sided wave shaper
			dTrivialSaw = 2.0*(tanh(1.5*dModulo)/tanh(1.5)) - 1.0;
		else if(m_uWaveform == SAW3)	// SAW3 = double sided wave shaper
		{
			dTrivialSaw = unipolarToBipolar(dModulo);
			dTrivialSaw = tanh(1.5*dTrivialSaw)/tanh(1.5);
		}
        else if(m_uWaveform == SIMPLESAW)			// SAW1 = normal sawtooth (ramp)
			dTrivialSaw = unipolarToBipolar(dModulo);
        
		// --- NOTE: Fs/8 = Nyquist/4
		if(m_dFo <= m_dSampleRate/8.0)
		{
			dOut = dTrivialSaw;
		}
		else // to prevent overlapping BLEPs, default back to 2-point for f > Nyquist/4
		{
			dOut = dTrivialSaw;
		}
        
		return dOut;
	}
    
    // square with polyBLEP
	inline double doSimpleSquare(double dModulo, double dInc)
	{
		// --- sum-of-saws method
		//
		// --- pretend to be SAW1 type
		m_uWaveform = SIMPLESAW;
        
		// --- get first sawtooth output
		double dSaw1 = doSimpleSawtooth(dModulo, dInc);
        
		// --- phase shift on second oscillator
		if(dInc > 0)
			dModulo += m_dPulseWidth/100.0;
		else
			dModulo -= m_dPulseWidth/100.0;
        
		// --- for positive frequencies
		if(dInc > 0 && dModulo >= 1.0)
			dModulo -= 1.0;
        
		// --- for negative frequencies
		if(dInc < 0 && dModulo <= 0.0)
			dModulo += 1.0;
        
		// --- subtract saw method
		double dSaw2 = doSimpleSawtooth(dModulo, dInc);
        
		// --- subtract = 180 out of phase
		double dOut = 0.5*dSaw1 - 0.5*dSaw2;
		
		// --- apply DC correction
		double dCorr = 1.0/(m_dPulseWidth/100.0);
        
		// --- modfiy for less than 50%
		if((m_dPulseWidth/100.0) < 0.5)
			dCorr = 1.0/(1.0-(m_dPulseWidth/100.0));
        
		// --- apply correction
		dOut *= dCorr;
        
		// --- reset back to SQUARE
		m_uWaveform = SIMPLESQUARE;
        
		return dOut;
	}
    
	inline double doSawtooth(double dModulo, double dInc)
	{
		double dTrivialSaw = 0.0;
		double dOut = 0.0;

		if(m_uWaveform == SAW1)			// SAW1 = normal sawtooth (ramp)
			dTrivialSaw = unipolarToBipolar(dModulo);
		else if(m_uWaveform == SAW2)	// SAW2 = one sided wave shaper
			dTrivialSaw = 2.0*(tanh(1.5*dModulo)/tanh(1.5)) - 1.0; 
		else if(m_uWaveform == SAW3)	// SAW3 = double sided wave shaper
		{
			dTrivialSaw = unipolarToBipolar(dModulo);
			dTrivialSaw = tanh(1.5*dTrivialSaw)/tanh(1.5);
		}

		// --- NOTE: Fs/8 = Nyquist/4
		if(m_dFo <= m_dSampleRate/8.0)
		{
			dOut = dTrivialSaw + doBLEP_N(&dBLEPTable_8_BLKHAR[0], /* BLEP table */
										4096,			/* BLEP table length */
										dModulo,		/* current phase value */
										fabs(dInc),	/* abs(dInc) is for FM synthesis with negative frequencies */
										1.0,			/* sawtooth edge height = 1.0 */
										false,		/* falling edge */
										4,			/* 1 point per side */
										false);		/* no interpolation */
		}
		else // to prevent overlapping BLEPs, default back to 2-point for f > Nyquist/4
		{
			dOut = dTrivialSaw + doBLEP_N(&dBLEPTable[0], /* BLEP table */
										4096,			/* BLEP table length */
										dModulo,		/* current phase value */
										fabs(dInc),	/* abs(dInc) is for FM synthesis with negative frequencies */
										1.0,			/* sawtooth edge height = 1.0 */
										false,		/* falling edge */
										1,			/* 1 point per side */
										true);		/* no interpolation */
		}
	
		// --- or do PolyBLEP
		//dOut = dTrivialSaw + doPolyBLEP_2(dModulo, 
		//								  abs(dInc),/* abs(dInc) is for FM synthesis with negative frequencies */
		//								  1.0,		/* sawtooth edge = 1.0 */
		//								  false);	/* falling edge */
										
		return dOut;
	}

	// square with polyBLEP
	inline double doSquare(double dModulo, double dInc)
	{
		// --- sum-of-saws method
		//
		// --- pretend to be SAW1 type
		m_uWaveform = SAW1;

		// --- get first sawtooth output
		double dSaw1 = doSawtooth(dModulo, dInc);

		// --- phase shift on second oscillator
		if(dInc > 0)
			dModulo += m_dPulseWidth/100.0;
		else
			dModulo -= m_dPulseWidth/100.0;

		// --- for positive frequencies
		if(dInc > 0 && dModulo >= 1.0) 
			dModulo -= 1.0; 

		// --- for negative frequencies
		if(dInc < 0 && dModulo <= 0.0) 
			dModulo += 1.0; 

		// --- subtract saw method
		double dSaw2 = doSawtooth(dModulo, dInc);

		// --- subtract = 180 out of phase
		double dOut = 0.5*dSaw1 - 0.5*dSaw2;
		
		// --- apply DC correction
		double dCorr = 1.0/(m_dPulseWidth/100.0);

		// --- modfiy for less than 50%
		if((m_dPulseWidth/100.0) < 0.5)
			dCorr = 1.0/(1.0-(m_dPulseWidth/100.0)); 

		// --- apply correction
		dOut *= dCorr;

		// --- reset back to SQUARE
		m_uWaveform = SQUARE;

		return dOut;
	}

	// DPW
	inline double doTriangle(double dModulo, double dInc, double dFo, double dSquareModulator, double* pZ_register)
	{
		//double dOut = 0.0;
		//bool bDone = false;

		// bipolar conversion and squaring
		double dBipolar = unipolarToBipolar(dModulo);
		double dSq = dBipolar*dBipolar;

		// inversion
		double dInv = 1.0 - dSq;

		// modulation with square modulo
		double dSqMod = dInv*dSquareModulator;

		// original
		double dDifferentiatedSqMod = dSqMod - *pZ_register;
		*pZ_register = dSqMod;

		// c = fs/[4fo(1-2fo/fs)]
		double c = m_dSampleRate/(4.0*2.0*dFo*(1-dInc));

		return dDifferentiatedSqMod*c;
	}
    
    // DPW
	inline double doSimpleTriangle(double dModulo, double dInc, double dFo, double dSquareModulator, double* pZ_register)
	{
		//double dOut = 0.0;
		//bool bDone = false;
        
		// bipolar conversion and squaring
		double dBipolar = unipolarToBipolar(dModulo);
		double dSq = dBipolar*dBipolar;
        
		// inversion
		double dInv = 1.0 - dSq;
        
		// modulation with square modulo
		double dSqMod = dInv*dSquareModulator;
        
		// original
		double dDifferentiatedSqMod = dSqMod - *pZ_register;
		*pZ_register = dSqMod;
        
		// c = fs/[4fo(1-2fo/fs)]
		double c = m_dSampleRate/(4.0*2.0*dFo*(1-dInc));
        
		return dDifferentiatedSqMod*c;
	}

	
	// -- virtual overrides
	virtual void reset();
	virtual void startOscillator();
	virtual void stopOscillator();

	// the rendering function
	virtual inline double doOscillate(double* pAuxOutput = NULL)
	{
		if(!m_bNoteOn)
			return 0.0;

		double dOut = 0.0;

		// always first
		bool bWrap = checkWrapModulo();

		// added for PHASE MODULATION
		double dCalcModulo = m_dModulo + m_dPhaseMod + m_Phase; // ******
			checkWrapIndex(dCalcModulo);

		switch(m_uWaveform)
		{
			case SINE:
			{
				// calculate angle
				double dAngle = dCalcModulo*2.0*(double)M_PI - (double)M_PI;

				// call the parabolicSine approximator
				dOut = parabolicSine(-1.0*dAngle);

				break;
			}

			case SAW1:
			case SAW2:
			case SAW3:
			{
				// do first waveform
				dOut = doSawtooth(dCalcModulo, m_dInc); 

				break;
			}

			case SQUARE:
			{
				dOut = doSquare(dCalcModulo, m_dInc);

				break;
			}

			case TRI:
			{
				// do first waveform
				if(bWrap)
					m_dDPWSquareModulator *= -1.0;

				dOut = doTriangle(dCalcModulo, m_dInc, m_dFo, m_dDPWSquareModulator, &m_dDPW_z1);

				break;
			}

			case NOISE:
			{
				// use helper function
				dOut = doWhiteNoise();

				break;
			}

			case PNOISE:
			{
				// use helper function
				dOut = doPNSequence(m_uPNRegister);

				break;
			}
            case SIMPLESAW:
			{
				// use helper function
				dOut = doSimpleSawtooth(dCalcModulo, m_dInc);
                
				break;
			}
                
            case SIMPLESQUARE:
			{
				// use helper function
				dOut = doSimpleSquare(dCalcModulo, m_dInc);
				break;
			}
            case SIMPLETRIANGLE:
			{
				// do first waveform
				if(bWrap)
					m_dDPWSquareModulator *= -1.0;
                
				dOut = doSimpleTriangle(dCalcModulo, m_dInc, m_dFo, m_dDPWSquareModulator, &m_dDPW_z1);
                
				break;
			}
			default:
				break;
		}

		// --- ok to inc modulo now
		incModulo();
		if(m_uWaveform == TRI)
			incModulo();
		
		// --- m_dAmpMod is set in update()
		if(pAuxOutput)
			*pAuxOutput = dOut*m_dAmplitude*m_dAmpMod;;

		// --- m_dAmpMod is set in update()
		return dOut*m_dAmplitude*m_dAmpMod;
	}
};

/*
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
    
    void set_new_samplerate(double newRate)
    {
        sample_rate = newRate;
        update_coefficients();
    }
    
    void set_octave_shift(int shift)
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
    
    
protected:
    double  sample_rate;
    int     oversample_factor;
    int     osc_type;
    float   frequency;
    float   cur_freq;
    float   detune_cents;
    float   semitones;
    int     octave_shift;
    
    bool shouldFM;
    float fmAmount;
    float * freqMod_buf;
};
 
 */

/*
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
    
    virtual void process_block(float * buffer, int numSamples) override
    {
        for (int s = 0; s < numSamples; s++)
        {
            if (shouldFM)
            {
                // qOsc[3].setPhaseMod(freqMod_buf[i]*fmAmount); // not nice
                // qOsc[3].setFoModExp(freqMod_buf[i]*(fmAmount / 99 ));   // 1111 / 111 // nice
                
                // commented for now.... ***
                if (freqMod_buf && fm_amount)
                {
                    //                    osc.setFoModLin(freqMod_buf[s]*fm_amount); // nice
                    //                    osc.update();
                }
                
            }
            
            buffer[s] = (float)osc.doOscillate();
        }
    }
    
    double doOscillate()
    {
        return osc.doOscillate();
    }
    
    
    virtual void set_sub_type(int newSubType) override
    {
        sub_type = newSubType;
        
        switch (sub_type) {
            case 0:
            {
                osc.m_uWaveform = 0;
                osc.setSampleRate(sample_rate * oversample_factor);
                osc.reset();
            }
                break;
            case 1:
            {
                osc.m_uWaveform = 4;
                osc.setSampleRate(sample_rate * oversample_factor);
                osc.reset();
            }
                break;
            case 2:
            {
                osc.m_uWaveform = 1;
                osc.setSampleRate(sample_rate * oversample_factor);
                osc.reset();
            }
                break;
            case 3:
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
    
    //    void set_midi_note_frequency(int midiNote);
    
    void setShouldFM(bool s) { shouldFM = s; }
    void setFreqModBuffer(float * modBuf) { freqMod_buf = modBuf; }
    
private:
    int sub_type;
    bool shouldFM;
    float pulse_width;
    
    float fm_amount;
    float * freqMod_buf;
    CQBLimitedOscillator osc;
};

*/
