/*
  ==============================================================================

    SynthesisProcessor.cpp
    Created: 27 Mar 2019 6:06:58pm
    Author:  Gary Jones

  ==============================================================================
*/

#include "SynthesisProcessor.h"

/*
SynthesisProcessorVoice2::SynthesisProcessorVoice2(FrequencyManager * fm, int ref)
{
    frequencyManager = fm;
    
    oversampleFactor = 1;
    
    osc         = new DigitalVAOSC(44100, oversampleFactor);
    osc         ->reset();
    
    envNew = new ADSR2(44100);
    envNew->reset();
    
    voiceRef    = ref;
}

SynthesisProcessorVoice2::~SynthesisProcessorVoice2()
{
    
}

//void SynthesisProcessorVoice::setCurrentPlaybackSampleRate (double newRate)
//{
//    printf("voice cplaybacksample rate called");
//
//    SynthesiserVoice::setCurrentPlaybackSampleRate(newRate);
//
//    envelope    ->setSampleRate(newRate);
//    osc         ->setSampleRate(44100);
//    osc         ->reset();
//    osc         ->startOscillator();
//
//}

void SynthesisProcessorVoice2::setOversamplingFactor(int newFactor)
{
    oversampleFactor = newFactor;
    
    double oversampledRate = getSampleRate() * oversampleFactor;
//
//    osc         ->setSampleRate(oversampledRate);
//    envelope    ->setSampleRate(oversampledRate);
}

void SynthesisProcessorVoice2::renderNextBlock (AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    float * buffer = outputBuffer.getWritePointer(0);
    
    // if oversampling is set, the voice takes care of processing the correct amount
    // need to downshift before passing back to the
    
    for (int i = 0; i < numSamples; i++)
    {
        float oscSample = (float)osc->doOscillate();
        float envSample = process_adsr();     //envelope->getNextSample();
        
        buffer[i] = oscSample * envSample;
        
        //        if (!envelope->isActive()) {
        //            this->clearCurrentNote();
        //        }
    }
}

float SynthesisProcessorVoice2::process_adsr()
{
    
    float env = 0.f;
    if(envNew->getState() != ADSR2::env_idle)
    {
        float val = envNew->process();
        env =  val;
    }
    else
    {
        env = 0.f;
        osc->stop();
        this->clearCurrentNote();
        
    }
    
    return env;
}

void SynthesisProcessorVoice2::startNote (int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition)
{
    if (const SynthSound* const sound = dynamic_cast<const SynthSound*> (sound))
    {
//        osc->reset();
        osc->set_frequency(frequencyManager->getFrequencyForMIDINote(midiNoteNumber));
        osc->start();
        
        envNew->gate(1);
    }
    else
    {
        jassertfalse; // this object can only play SamplerSounds!
    }
    
    

}

void SynthesisProcessorVoice2::stopNote (float velocity, bool allowTailOff)
{
    envNew->gate(0);
}

void SynthesisProcessorVoice2::debugParameters()
{
//    if (isVoiceActive()) {
//        printf("\n voice %i is active", voiceRef);
//        //            printf("\n frequency = %f", osc->getFrequency());
//    }
//
//    if (envelope->isActive())
//    {
//        printf("\n voice %i ENV is active", voiceRef);
//    }
}

bool SynthesisProcessorVoice2::canPlaySound (SynthesiserSound*sound)
{
//    return true;
        return dynamic_cast <const SynthSound*> (sound) != 0;
}

void SynthesisProcessorVoice2::pitchWheelMoved (int newPitchWheelValue)
{
    
}
void SynthesisProcessorVoice2::controllerMoved (int controllerNumber, int newControllerValue)
{ }

void SynthesisProcessorVoice2::setParameter(int index, var newValue)
{
    switch (index) {
        case INSTRUMENT_TYPE:
        {
            int val = (int)newValue;
            //                instrumentType = (INSTRUMENT_TYPES)val;
            
            // load new instrument into sampler
        }
            break;
        case WAVEFORM_TYPE:
        {
            int val = (int)newValue;
            waveformType = (WAVEFORM_TYPES)val;
            osc->set_osc_type(waveformType);
        }
            break;
        case ENV_AMPLITUDE:
        {
            amplitude = (float)newValue * 0.1;
        }
            break;
        case ENV_ATTACK:
        {
//            envNew->setAttackRate((float)newValue * 0.001);
            envNew->setAttackRate((float)newValue);
        }
            break;
        case ENV_SUSTAIN:
        {
//            envNew->setSustainLevel((float)newValue * 0.1);
            envNew->setSustainLevel((float)newValue);
        }
            break;
        case ENV_DECAY:
        {
//            envNew->setDecayRate((float)newValue * 0.001);
            envNew->setDecayRate((float)newValue);
        }
            break;
        case ENV_RELEASE:
        {
//            envNew->setReleaseRate((float)newValue * 0.001);
            envNew->setReleaseRate((float)newValue);
        }
            break;
        case CHORD_SOURCE:
        {
            if ((bool)newValue != chordSource)
            {
                chordSource = !chordSource;
            }
        }
            break;
        case INSERT_FREQUENCY:
        {
            insertFrequency = (float)newValue;
            
            if (!chordSource)
            {
                // need to face oscillator to override note
            }
        }
            break;
        case OCTAVE:
        {
            octaveShift = (float)newValue;
            osc->set_octave_shift(octaveShift);
        }
            break;
//        case MANIPULATE_CHOSEN_FREQUENCY:
//        {
//            manipulateChoseFrequency = (bool)newValue;
//            osc->setManipulateFrequency(manipulateChoseFrequency);
//        }
//            break;
//        case MULTIPLY_OR_DIVISION:
//        {
//            multiplyOrDivision = (bool)newValue;
//            osc->setMultiplyDivide(multiplyOrDivision);
//        }
//            break;
//        case MULTIPLY_VALUE:
//        {
//            multiplyValue = (float) newValue;
//            osc->setMultiplyValue(multiplyValue);
//        }
//            break;
//        case DIVISION_VALUE:
//        {
//            divisionValue = (float) newValue;
//            osc->setDivideValue(divisionValue);
//        }
//            break;
            
        default: break;
    }
}

*/






































SynthesisProcessorVoice::SynthesisProcessorVoice(FrequencyManager * fm, int ref)
{
    frequencyManager = fm;
    
    oversampleFactor = 1;
    
    osc         = new Osc();
    osc         ->setSampleRate(44100);
    osc         ->reset();
    osc         ->setFrequency(432);
    osc         ->startOscillator();
    
    envParams.attack    = 0.f;
    envParams.decay     = 1.f;
    envParams.sustain   = 1.f;
    envParams.release   = 0.5;
    
    envelope    = new ADSR();
    envelope    ->setSampleRate(44100);
    envelope    ->reset();
    envelope    ->setParameters(envParams);
    
    envNew = new ADSR2(44100);
    envNew->reset();
    
    voiceRef    = ref;
}

SynthesisProcessorVoice::~SynthesisProcessorVoice()
{
    
}

//void SynthesisProcessorVoice::setCurrentPlaybackSampleRate (double newRate)
//{
//    printf("voice cplaybacksample rate called");
//    
//    SynthesiserVoice::setCurrentPlaybackSampleRate(newRate);
//    
//    envelope    ->setSampleRate(newRate);
//    osc         ->setSampleRate(44100);
//    osc         ->reset();
//    osc         ->startOscillator();
//    
//}

void SynthesisProcessorVoice::setOversamplingFactor(int newFactor)
{
    oversampleFactor = newFactor;
}

void SynthesisProcessorVoice::renderNextBlock (AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    float * buffer = outputBuffer.getWritePointer(0);
    
    // if oversampling is set, the voice takes care of processing the correct amount
    // need to downshift before passing back to the
    
    for (int i = 0; i < numSamples; i++)
    {
        float oscSample = (float)osc->doOscillate();
        float envSample = process_adsr();     //envelope->getNextSample();
        
        buffer[i] = oscSample * envSample;
        
//        if (!envelope->isActive()) {
//            this->clearCurrentNote();
//        }
    }
}

float SynthesisProcessorVoice::process_adsr()
{

    float env = 0.f;
    if(envNew->getState() != ADSR2::env_idle)
    {
        float val = envNew->process();
        env =  val;
    }
    else
    {
        env = 0.f;
        this->clearCurrentNote();
    }
    
    return env;

//    float env = 0;
//    if (envelope->isActive())
//    {
//        env = envelope->getNextSample();
//    }
//    else
//    {
//        env = 0; this->clearCurrentNote();
//    }
//
//    return env;
}

void SynthesisProcessorVoice::startNote (int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition)
{
    osc->reset();
//    osc->setFrequency(frequencyManager->getFrequencyForMIDINote(midiNoteNumber));
    
//    envelope->reset();
    
    envelope->noteOn();
    
    envNew->gate(0);
}

void SynthesisProcessorVoice::stopNote (float velocity, bool allowTailOff)
{
    envelope->noteOff();
    
    envNew->gate(1);
}

void SynthesisProcessorVoice::debugParameters()
{
    if (isVoiceActive()) {
        printf("\n voice %i is active", voiceRef);
        //            printf("\n frequency = %f", osc->getFrequency());
    }
    
    if (envelope->isActive())
    {
        printf("\n voice %i ENV is active", voiceRef);
    }
}

bool SynthesisProcessorVoice::canPlaySound (SynthesiserSound*sound)
{
    return true;
//    return dynamic_cast <SynthSound*> (sound) != 0;
}

void SynthesisProcessorVoice::pitchWheelMoved (int newPitchWheelValue)
{
    
}
void SynthesisProcessorVoice::controllerMoved (int controllerNumber, int newControllerValue)
{ }

void SynthesisProcessorVoice::setParameter(int index, var newValue)
{
    switch (index) {
        case INSTRUMENT_TYPE:
        {

        }
            break;
        case WAVEFORM_TYPE:
        {
            auto val = (int) newValue;
            waveformType = static_cast<WAVEFORM_TYPES>(val);
            osc->setOscillatorType( (Osc::OSC_TYPE) val);
        }
            break;
        case ENV_AMPLITUDE:
        {
            amplitude = (float)newValue * 0.1;
        }
            break;
        case ENV_ATTACK:
        {
            envParams.attack = (float)newValue * 0.001;
            envelope->setParameters(envParams);
        }
            break;
        case ENV_SUSTAIN:
        {
            envParams.sustain = (float)newValue * 0.1;
            envelope->setParameters(envParams);
        }
            break;
        case ENV_DECAY:
        {
            envParams.decay = (float)newValue * 0.001;
            envelope->setParameters(envParams);
        }
            break;
        case ENV_RELEASE:
        {
            envParams.release = (float)newValue * 0.001;
            envelope->setParameters(envParams);
        }
            break;
        case CHORD_SOURCE:
        {
            if ((bool)newValue != chordSource)
            {
                chordSource = !chordSource;
            }
        }
            break;
        case INSERT_FREQUENCY:
        {
            insertFrequency = (float)newValue;
            
            if (!chordSource)
            {
                // need to face oscillator to override note frequency
                // do chord finder thing.
            }
        }
            break;
        case OCTAVE:
        {
            octaveShift = (float)newValue;
            osc->setOctave(octaveShift);
        }
            break;
        case MANIPULATE_CHOSEN_FREQUENCY:
        {
            manipulateChoseFrequency = (bool)newValue;
            osc->setManipulateFrequency(manipulateChoseFrequency);
        }
            break;
        case MULTIPLY_OR_DIVISION:
        {
            multiplyOrDivision = (bool)newValue;
            osc->setMultiplyDivide(multiplyOrDivision);
        }
            break;
        case MULTIPLY_VALUE:
        {
            multiplyValue = (float) newValue;
            osc->setMultiplyValue(multiplyValue);
        }
            break;
        case DIVISION_VALUE:
        {
            divisionValue = (float) newValue;
            osc->setDivideValue(divisionValue);
        }
            break;
            
        default: break;
    }
}


SynthesisProcessor::SynthesisProcessor(FrequencyManager * fm, double initialSampleRate)
{
//    setCurrentPlaybackSampleRate(initialSampleRate);
    
    frequencyManager = fm;

//    clearVoices();
//    clearSounds();
    
    for (int voice = 0; voice < MAX_NUM_VOICES; voice++)
    {
        voices[voice] = new SynthesisProcessorVoice(frequencyManager, voice);
        this->addVoice(voices[voice]);
    }
    
    addSound(new SynthSound());
    
    setNoteStealingEnabled(true);
}

SynthesisProcessor::~SynthesisProcessor(){}

void SynthesisProcessor::processBlock(AudioBuffer<float>& buffer,
                  MidiBuffer& midiMessages)
{
    // check mute here
    renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    

}

//void SynthesisProcessor::prepareToPlay(float sampleR, int blockSize)
//{
//    setCurrentPlaybackSampleRate(sampleR);
//}

void SynthesisProcessor::setOversamplingFactor(int newFactor)
{
    //        clearVoices();
    
    //        for (int voice = 0; voice < MAX_NUM_VOICES; voice++)
    //        {
    //            voices[voice]->setOversamplingFactor(newFactor);
    //        }
}

void SynthesisProcessor::setParameter(int index, var newValue)
{
    for (int v = 0; v < getNumVoices(); v++)
    {
        voices[v]->setParameter(index, newValue);
    }
}
