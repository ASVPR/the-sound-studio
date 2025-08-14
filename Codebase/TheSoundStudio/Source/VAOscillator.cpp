/*
  ============================================================================
    VAOscillator.
    Created: 2 Jul 2019 9:16:3
    Author:  Gary Jon
  ============================================================================
*/
#include "VAOscillator.h"


// --- construction
QOscillator::QOscillator(void)
{
	// --- initialize variables
	m_dSampleRate = 44100;
	m_bNoteOn = false;
	m_uMIDINoteNumber = 0;
	m_dModulo = 0.0;
	m_dInc = 0.0;
	m_dOscFo = OSC_FO_DEFAULT; // GUI
	m_dAmplitude = 1.0; // default ON
	m_dPulseWidth = OSC_PULSEWIDTH_DEFAULT;
	m_dPulseWidthControl = OSC_PULSEWIDTH_DEFAULT; // GUI
	m_dFo = OSC_FO_DEFAULT;
    
	// --- seed the random number generator
	srand((int)time(NULL));
	m_uPNRegister = rand();
    
	// --- continue inits
	m_nRSHCounter = -1; // flag for reset condition
	m_dRSHValue = 0.0;
	m_dAmpMod = 1.0; // note default to 1 to avoid silent osc
	m_dFoModLin = 0.0;
	m_dPhaseMod = 0.0;
	m_dFoMod = 0.0;
	m_dPitchBendMod = 0.0;
	m_dPWMod = 0.0;
	m_nOctave = 0.0;
	m_nSemitones = 0.0;
	m_nCents = 0.0;
	m_dFoRatio = 1.0;
	m_uLFOMode = 0;
    
	// --- pitched
	m_uWaveform = SINE;    
    manipulateFrequency = false;
    multiplyDivide      = false;
    multiplyValue       = 1.0;
    divideValue         = 1.0;
    
    m_Phase             = 0.f;

}

// --- destruction
QOscillator::~QOscillator(void)
{
}

// --- VIRTUAL FUNCTION; base class implementations
void QOscillator::reset()
{
	// --- Pitched modulos, wavetables start at 0.0
	m_dModulo = 0.0;
    
	// --- needed fror triangle algorithm, DPW
	m_dDPWSquareModulator = -1.0;
    
	// --- flush DPW registers
	m_dDPW_z1 = 0.0;
    
	// --- for random stuff
	srand((int)time(NULL));
	m_uPNRegister = rand();
	m_nRSHCounter = -1; // flag for reset condition
	m_dRSHValue = 0.0;
    
	// --- modulation variables
	m_dAmpMod = 1.0; // note default to 1 to avoid silent osc
	m_dPWMod = 0.0;
	m_dPitchBendMod = 0.0;
	m_dFoMod = 0.0;
	m_dFoModLin = 0.0;
	m_dPhaseMod = 0.0;
}
CQBLimitedOscillator::CQBLimitedOscillator(void)
{
}

CQBLimitedOscillator::~CQBLimitedOscillator(void)
{
}

void CQBLimitedOscillator::reset()
{
	QOscillator::reset();

	// --- saw/tri starts at 0.5
	if(m_uWaveform == SAW1 || m_uWaveform == SAW2 ||
	   m_uWaveform == SAW3 || m_uWaveform == TRI)
	{
		m_dModulo = 0.5;
	}
}

void CQBLimitedOscillator::startOscillator()
{
	reset();
	m_bNoteOn = true;
}

void CQBLimitedOscillator::stopOscillator()
{
	m_bNoteOn = false;



}
