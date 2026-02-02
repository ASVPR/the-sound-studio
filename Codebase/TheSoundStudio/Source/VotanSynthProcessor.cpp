/*
  ==============================================================================

    VotanSynthProcessor.cpp

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#include "VotanSynthProcessor.h"

ChordSynthVoice::ChordSynthVoice(double newSampleRate, FrequencyManager * fm)
{
    frequencyManager    = fm;
    sample_rate         = newSampleRate;
    oscillator[0]       = new OSCManager2(sample_rate);
    env[0]              = new ADSR2(sample_rate);

    for(int i = 0; i < 12; i++)
    {
        m_bufferAlloc[i].alloc(FRAMESIZE * oversample_factor);
    }
    
    for (int i = 0; i < 1; i++)
    {
        oscillator[i]->reset();
        osc_volume[i] = 1.f;
    }
    
    env[0]->reset();
}

ChordSynthVoice::~ChordSynthVoice()  { }

void ChordSynthVoice::setCurrentPlaybackSampleRate (double newRate)
{
    sample_rate = newRate;
    SynthesiserVoice::setCurrentPlaybackSampleRate(newRate);
    
    double newSampleRate = sample_rate;
    
    // set new samplerate to OSCs with newSampleRate
    for (int i = 0; i < 1; i++)
    {
        oscillator[i]->set_new_samplerate(newSampleRate);
        env[i]->setNewSamplerate(newSampleRate);
    }
}

bool ChordSynthVoice::canPlaySound (SynthesiserSound* sound)
{
    return dynamic_cast <ChordSynthSound*> (sound) != 0;
}

void ChordSynthVoice::startNote (int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition)
{
    for (int i =0; i<1; i++)
    {
        oscillator[i]->reset();
//        oscillator[i]->set_frequency(frequencyManager->getFrequencyForMIDINote(midiNoteNumber));
        oscillator[i]->triggerNote();
    }
    
    for (int i = 0; i < 1; i++) env[i]->gate(1);
}

void ChordSynthVoice::stopNote (float velocity, bool allowTailOff)
{
    // call release
    for(int i = 0; i < 1; i++) env[i]->gate(0);
}

void ChordSynthVoice::renderNextBlock (AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (outputBuffer.getNumChannels() == 1)
    {
        float * buf_left = outputBuffer.getWritePointer(0);
        
        float voiceBufferLeft[numSamples]; for (int i = 0; i < numSamples; i++) voiceBufferLeft[i] = 0.f;
        float voiceBufferRight[numSamples]; for (int i = 0; i < numSamples; i++) voiceBufferRight[i] = 0.f;
        
        ProcessBuffer(voiceBufferLeft, voiceBufferRight, numSamples);
        
        for (int i = 0; i < numSamples; i++)
        {
            buf_left[i]     += voiceBufferLeft[i];
        }
    }
    else
    {
        float * buf_left = outputBuffer.getWritePointer(0);
        float * buf_right = outputBuffer.getWritePointer(1);
        
        float voiceBufferLeft[numSamples]; for (int i = 0; i < numSamples; i++) voiceBufferLeft[i] = 0.f;
        float voiceBufferRight[numSamples]; for (int i = 0; i < numSamples; i++) voiceBufferRight[i] = 0.f;
        
        ProcessBuffer(voiceBufferLeft, voiceBufferRight, numSamples);
        
        for (int i = 0; i < numSamples; i++)
        {
            buf_left[i]     += voiceBufferLeft[i];
            buf_right[i]    += voiceBufferRight[i];
        }
    }
    
}

void ChordSynthVoice::ProcessBuffer(float *bufferLeft, float * bufferRight, int buffersize)
{
    // check
    process_oscs(bufferLeft, bufferRight, buffersize);
    process_adsr(bufferLeft, bufferRight, buffersize);
}

void ChordSynthVoice::process_oscs(float *bufferLeft, float * bufferRight, int buffersize)
{
    float* osc_buf[1];

    osc_buf[0] = m_bufferAlloc[0].m_buffer;
    
    // batch process by split factor
    int split_factor = 8;
    int blockSize = buffersize / split_factor;
    
    // process oscilators & mods
    float osc1_small_buf[blockSize];
    
    for (int s = 0; s < split_factor; s++)
    {
        oscillator[0]->processBuffer(osc1_small_buf, blockSize);

        for (int i = 0; i < blockSize; i++)
        {
            float osc_1_final =  osc1_small_buf[i] * osc_volume[0] * amplitude;
            
            bufferLeft[i+(blockSize * s)]  = (osc_1_final);
            bufferRight[i+(blockSize * s)] = (osc_1_final);
        }
    }
}


void ChordSynthVoice::process_adsr(float *bufferLeft, float * bufferRight, int buffersize)
{
    for (int i = 0; i < buffersize; i++)
    {
        if(env[0]->getState() != ADSR2::env_idle)
        {
            float val = env[0]->process();
            bufferLeft[i] *= val;
            bufferRight[i] *= val;
        }
        else
        {
            bufferLeft[i] = 0;
            bufferRight[i] = 0.f;
            this->clearCurrentNote();
        }
    }
}

void ChordSynthVoice::setParameter(int index, var newValue)
{
    switch (index) {
        case INSTRUMENT_TYPE:
        {

        }
            break;
        case WAVEFORM_TYPE:
        {
            int val = (int)newValue;
            waveformType = (WAVEFORM_TYPES)val;
            oscillator[0]->set_subtype(waveformType);
        }
            break;
            
        case ENV_AMPLITUDE:
        {
            amplitude = (float)newValue * 0.01;
        }
            break;
            
        case ENV_ATTACK:
        {
            float val = newValue.operator float();
            env[0]->SetAttack(val);
        }
            break;
            
        case ENV_SUSTAIN:
        {
            float val = newValue.operator float();
            env[0]->SetSustain((float)val * 0.01);
        }
            break;
        case ENV_DECAY:
        {
            float val = newValue.operator float();
            env[0]->SetDecay(val);
        }
            break;
        case ENV_RELEASE:
        {
            float val = newValue.operator float();
            env[0]->SetRelease(val);
        }
            break;
//        case CHORD_SOURCE:
//        {
//            if ((bool)newValue != chordSource)
//            {
//                chordSource = !chordSource;
//            }
//        }
//            break;
//        case INSERT_FREQUENCY:
//        {
//            insertFrequency = (float)newValue;
//
//            if (!chordSource)
//            {
//                // need to face oscillator to override note
//            }
//        }
//            break;
//        case OCTAVE:
//        {
//            octaveShift = (float)newValue;
//            oscillator[0]->setOctaveShift(octaveShift);
//        }
//            break;
//        case MANIPULATE_CHOSEN_FREQUENCY:
//        {
//            manipulateChoseFrequency = (bool)newValue;
//            oscillator[0]->setManipulateFrequency(manipulateChoseFrequency);
//        }
//            break;
//        case MULTIPLY_OR_DIVISION:
//        {
//            multiplyOrDivision = (bool)newValue;
//            oscillator[0]->setMultiplyDivide(multiplyOrDivision);
//        }
//            break;
//        case MULTIPLY_VALUE:
//        {
//            multiplyValue = (float) newValue;
//            oscillator[0]->setMultiplyValue(multiplyValue);
//        }
//            break;
//        case DIVISION_VALUE:
//        {
//            divisionValue = (float) newValue;
//            oscillator[0]->setDivideValue(divisionValue);
//        }
//            break;
            
        default: break;
    }
}

//=================================================================================
// Frequency Synth Voice
//=================================================================================

FrequencySynthVoice::FrequencySynthVoice(double newSampleRate, FrequencyManager * fm)
{
    frequencyManager    = fm;
    sample_rate         = newSampleRate;
    oscillator[0]       = new OSCManager2(sample_rate);
    env[0]              = new ADSR2(sample_rate);
    c_chosenFrequency   = 432.f;
    p_chosenFrequency   = 432.f;
    
    for(int i = 0; i < 12; i++)
    {
        m_bufferAlloc[i].alloc(FRAMESIZE * oversample_factor);
    }
    
    for (int i = 0; i < 1; i++)
    {
        oscillator[i]->reset();
        osc_volume[i] = 1.f;
    }
    
    env[0]->reset();
}

FrequencySynthVoice::~FrequencySynthVoice()  { }

void FrequencySynthVoice::setCurrentPlaybackSampleRate (double newRate)
{
    sample_rate = newRate;
    SynthesiserVoice::setCurrentPlaybackSampleRate(newRate);
    
    double newSampleRate = sample_rate;
    
    // set new samplerate to OSCs with newSampleRate
    for (int i = 0; i < 1; i++)
    {
        oscillator[i]->set_new_samplerate(newSampleRate);
        env[i]->setNewSamplerate(newSampleRate);
    }
}

bool FrequencySynthVoice::canPlaySound (SynthesiserSound* sound)
{
    return dynamic_cast <ChordSynthSound*> (sound) != 0;
}

void FrequencySynthVoice::startNote (int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition)
{
    calcManipulateFrequencies(); // check manipulate frequency bool, and sets parameter variables accordingly
    
    for (int i =0; i<1; i++)
    {
        oscillator[i]->reset();
        oscillator[i]->triggerNote();
        
        if (!frequencySource)
        {
            oscillator[0]->set_frequency(c_chosenFrequency);
        }
        else
        {
            calcRangeSweep();
        }
    }
    
    for (int i = 0; i < 1; i++)
        env[i]->gate(1);
}


void FrequencySynthVoice::stopNote (float velocity, bool allowTailOff)
{
    // call release
    for(int i = 0; i < 1; i++) env[i]->gate(0);
}

void FrequencySynthVoice::renderNextBlock (AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (outputBuffer.getNumChannels() == 1)
    {
        float * buf_left = outputBuffer.getWritePointer(0);
        
        float voiceBufferLeft[numSamples]; for (int i = 0; i < numSamples; i++) voiceBufferLeft[i] = 0.f;
        float voiceBufferRight[numSamples]; for (int i = 0; i < numSamples; i++) voiceBufferRight[i] = 0.f;
        
        ProcessBuffer(voiceBufferLeft, voiceBufferRight, numSamples);
        
        for (int i = 0; i < numSamples; i++)
        {
            buf_left[i]     += voiceBufferLeft[i];
        }
    }
    else
    {
        float * buf_left = outputBuffer.getWritePointer(0);
        float * buf_right = outputBuffer.getWritePointer(1);
        
        float voiceBufferLeft[numSamples]; for (int i = 0; i < numSamples; i++) voiceBufferLeft[i] = 0.f;
        float voiceBufferRight[numSamples]; for (int i = 0; i < numSamples; i++) voiceBufferRight[i] = 0.f;
        
        ProcessBuffer(voiceBufferLeft, voiceBufferRight, numSamples);
        
        for (int i = 0; i < numSamples; i++)
        {
            buf_left[i]     += voiceBufferLeft[i];
            buf_right[i]    += voiceBufferRight[i];
        }
    }
    

}

void FrequencySynthVoice::ProcessBuffer(float *bufferLeft, float * bufferRight, int buffersize)
{
    if (frequencySource && shouldProcessRangeSweep)
    {
        processRangeSweep(buffersize);
    }

    process_oscs(bufferLeft, bufferRight, buffersize);
    process_adsr(bufferLeft, bufferRight, buffersize);
}

void FrequencySynthVoice::process_oscs(float *bufferLeft, float * bufferRight, int buffersize)
{
    float* osc_buf[1];
    
    osc_buf[0] = m_bufferAlloc[0].m_buffer;
    
    // batch process by split factor
    int split_factor = 8;
    int blockSize = buffersize / split_factor;
    
    // process oscilators & mods
    float osc1_small_buf[blockSize];
    
    for (int s = 0; s < split_factor; s++)
    {
        oscillator[0]->processBuffer(osc1_small_buf, blockSize);
        
        for (int i = 0; i < blockSize; i++)
        {
            float osc_1_final =  osc1_small_buf[i] * osc_volume[0] * amplitude;
            
            bufferLeft[i+(blockSize * s)]  = (osc_1_final);
            bufferRight[i+(blockSize * s)] = (osc_1_final);
        }
    }
}

void FrequencySynthVoice::process_adsr(float *bufferLeft, float * bufferRight, int buffersize)
{
    for (int i = 0; i < buffersize; i++)
    {
        if(env[0]->getState() != ADSR2::env_idle)
        {
            float val = env[0]->process();
            bufferLeft[i] *= val;
            bufferRight[i] *= val;
        }
        else
        {
            bufferLeft[i] = 0;
            bufferRight[i] = 0.f;
            this->clearCurrentNote();
        }
    }
}

void FrequencySynthVoice::setParameter(int index, var newValue)
{
    switch (index) {
        case FREQUENCY_PLAYER_WAVEFORM_TYPE:
        {
            int val = (int)newValue;
            waveformType = (WAVEFORM_TYPES)val;
            oscillator[0]->set_subtype(waveformType);
        }
            break;
            
        case FREQUENCY_PLAYER_AMPLITUDE:
        {
            float val = (float)newValue * 0.01;
            amplitude = val;
        }
            break;
            
        case FREQUENCY_PLAYER_ATTACK:
        {
            float val = newValue.operator float();
            env[0]->SetAttack(val);
        }
            break;
            
        case FREQUENCY_PLAYER_DECAY:
        {
            float val = newValue.operator float();
            env[0]->SetDecay(val);
        }
            break;
            
        case FREQUENCY_PLAYER_SUSTAIN:
        {
            float val = newValue.operator float();
            env[0]->SetSustain((float)val * 0.01);
        }
            break;
            
        case FREQUENCY_PLAYER_RELEASE:
        {
            float val = newValue.operator float();
            env[0]->SetRelease(val);
        }
            break;

        case FREQUENCY_PLAYER_FREQ_SOURCE:
        {
//            bool boolVal = newValue.operator bool();
//
//            if (boolVal != frequencySource)
//            {
//                frequencySource = !frequencySource;
//            }
            

            frequencySource = newValue.operator bool();
//
        }
            break;
        case FREQUENCY_PLAYER_CHOOSE_FREQ:
        {
            p_chosenFrequency = newValue.operator float();
            calcManipulateFrequencies();
            oscillator[0]->set_frequency(c_chosenFrequency);
            
        }
            break;
        case FREQUENCY_PLAYER_RANGE_MIN:
        {
            p_rangeMin = newValue.operator float();
            calcManipulateFrequencies();
        }
            break;
        case FREQUENCY_PLAYER_RANGE_MAX:
        {
            p_rangeMax = newValue.operator float();
            calcManipulateFrequencies();
        }
            break;
            
        case FREQUENCY_PLAYER_RANGE_LENGTH:
        {
            rangeLengthMS = newValue.operator int();
        }
            break;
        case FREQUENCY_PLAYER_RANGE_LOG:
        {
            rangeLog = newValue.operator bool();
        }
            break;

        case FREQUENCY_PLAYER_MANIPULATE_CHOSEN_FREQUENCY:
        {
            manipulateChoseFrequency = (bool)newValue;
            calcManipulateFrequencies();
        }
            break;
            
        case FREQUENCY_PLAYER_MULTIPLY_OR_DIVISION:
        {
            multiplyOrDivision = (bool)newValue;
            calcManipulateFrequencies();
        }
            break;
            
        case FREQUENCY_PLAYER_MULTIPLY_VALUE:
        {
            multiplyValue = (float) newValue;
            calcManipulateFrequencies();
        }
            break;
            
        case FREQUENCY_PLAYER_DIVISION_VALUE:
        {
            divisionValue = (float) newValue;
            calcManipulateFrequencies();
        }
            break;
            
        default: break;
    }
}

void FrequencySynthVoice::calcRangeSweep()
{
    sampleInc = 0;
    
    // set Min Freq to begin
    currentFrequency = c_rangeMin;
    oscillator[0]->set_frequency(currentFrequency);
    
    rangeLengthInSamples = (int)(rangeLengthMS / 1000) * sample_rate;
    
    // work out frequency incremement for lin / log
    if (rangeLog)
    {
        minMaxDifference = c_rangeMax - c_rangeMin;
    }
    else
    {
        // dif between min and max
        minMaxDifference = c_rangeMax - c_rangeMin;
        
        // spread dif between number of samples
        frequencyIncrementPerSample = minMaxDifference / rangeLengthInSamples;
    }
    
    shouldProcessRangeSweep = true;
}

void FrequencySynthVoice::processRangeSweep(int bufferLength)
{
    if (shouldProcessRangeSweep)
    {
        if (rangeLog)
        {
            float pos = (float)sampleInc / rangeLengthInSamples;
            currentFrequency = powf(10.f, pos * (log10f(c_rangeMax) - log10f(c_rangeMin)) + log10f(c_rangeMin));
            oscillator[0] ->set_frequency(currentFrequency);
        }
        else
        {
            currentFrequency += (frequencyIncrementPerSample * bufferLength);
            oscillator[0]->set_frequency(currentFrequency);
        }
        
        sampleInc += bufferLength;
        
        if (currentFrequency == c_rangeMax || sampleInc >= rangeLengthInSamples)
        {
            shouldProcessRangeSweep = false;
        }
    }
}

void FrequencySynthVoice::calcManipulateFrequencies()
{
    if (manipulateChoseFrequency)
    {
        if (!multiplyOrDivision)
        {
            // mult
            c_rangeMin          = p_rangeMin * multiplyValue;
            c_rangeMax          = p_rangeMax * multiplyValue;
            c_chosenFrequency   = p_chosenFrequency * multiplyValue;
        }
        else
        {
            // div
            c_rangeMin          = p_rangeMin / divisionValue;
            c_rangeMax          = p_rangeMax / divisionValue;
            c_chosenFrequency   = p_chosenFrequency / divisionValue;
        }
    }
    else
    {
        c_rangeMin          = p_rangeMin;
        c_rangeMax          = p_rangeMax;
        c_chosenFrequency   = p_chosenFrequency;
    }
}


// ========================================================================
// Frequency Scanner Synth Voice
//=========================================================================


//=================================================================================
// Frequency Synth Voice
//=================================================================================

FrequencyScannerSynthVoice::FrequencyScannerSynthVoice(double newSampleRate, FrequencyManager * fm)
{
    frequencyManager    = fm;
    sample_rate         = newSampleRate;
    oscillator[0]       = new OSCManager2(sample_rate);
    env[0]              = new ADSR2(sample_rate);
    
    for(int i = 0; i < 12; i++)
    {
        m_bufferAlloc[i].alloc(FRAMESIZE * oversample_factor);
    }
    
    for (int i = 0; i < 1; i++)
    {
        oscillator[i]->reset();
        osc_volume[i] = 1.f;
    }
    
    env[0]->reset();
}

FrequencyScannerSynthVoice::~FrequencyScannerSynthVoice()  { }

void FrequencyScannerSynthVoice::setCurrentPlaybackSampleRate (double newRate)
{
    sample_rate = newRate;
    SynthesiserVoice::setCurrentPlaybackSampleRate(newRate);
    
    double newSampleRate = sample_rate;
    
    // set new samplerate to OSCs with newSampleRate
    for (int i = 0; i < 1; i++)
    {
        oscillator[i]->set_new_samplerate(newSampleRate);
        env[i]->setNewSamplerate(newSampleRate);
    }
}

bool FrequencyScannerSynthVoice::canPlaySound (SynthesiserSound* sound)
{
    return dynamic_cast <ChordSynthSound*> (sound) != 0;
}

void FrequencyScannerSynthVoice::startNote (int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition)
{
    for (int i =0; i<1; i++)
    {
        oscillator[i]->reset();
        oscillator[i]->triggerNote();
    }
    
    for (int i = 0; i < 1; i++)
        env[i]->gate(1);
}

void FrequencyScannerSynthVoice::stopNote (float velocity, bool allowTailOff)
{
    // call release
    for(int i = 0; i < 1; i++) env[i]->gate(0);
}

void FrequencyScannerSynthVoice::renderNextBlock (AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (outputBuffer.getNumChannels() == 1)
    {
        float * buf_left = outputBuffer.getWritePointer(0);
        
        float voiceBufferLeft[numSamples]; for (int i = 0; i < numSamples; i++) voiceBufferLeft[i] = 0.f;
        float voiceBufferRight[numSamples]; for (int i = 0; i < numSamples; i++) voiceBufferRight[i] = 0.f;
        
        ProcessBuffer(voiceBufferLeft, voiceBufferRight, numSamples);
        
        for (int i = 0; i < numSamples; i++)
        {
            buf_left[i]     += voiceBufferLeft[i];
        }
    }
    else
    {
        float * buf_left = outputBuffer.getWritePointer(0);
        float * buf_right = outputBuffer.getWritePointer(1);
        
        float voiceBufferLeft[numSamples]; for (int i = 0; i < numSamples; i++) voiceBufferLeft[i] = 0.f;
        float voiceBufferRight[numSamples]; for (int i = 0; i < numSamples; i++) voiceBufferRight[i] = 0.f;
        
        ProcessBuffer(voiceBufferLeft, voiceBufferRight, numSamples);
        
        for (int i = 0; i < numSamples; i++)
        {
            buf_left[i]     += voiceBufferLeft[i];
            buf_right[i]    += voiceBufferRight[i];
        }
    }
}

void FrequencyScannerSynthVoice::ProcessBuffer(float *bufferLeft, float * bufferRight, int buffersize)
{
//    if (frequencySource && shouldProcessRangeSweep)
//    {
//        processRangeSweep(buffersize);
//    }

    process_oscs(bufferLeft, bufferRight, buffersize);
    process_adsr(bufferLeft, bufferRight, buffersize);
}

void FrequencyScannerSynthVoice::process_oscs(float *bufferLeft, float * bufferRight, int buffersize)
{
    float* osc_buf[1];
    
    osc_buf[0] = m_bufferAlloc[0].m_buffer;
    
    // batch process by split factor
    int split_factor = 8;
    int blockSize = buffersize / split_factor;
    
    // process oscilators & mods
    float osc1_small_buf[blockSize];
    
    for (int s = 0; s < split_factor; s++)
    {
        oscillator[0]->processBuffer(osc1_small_buf, blockSize);
        
        for (int i = 0; i < blockSize; i++)
        {
            float osc_1_final =  osc1_small_buf[i] * osc_volume[0] * amplitude;
            
            bufferLeft[i+(blockSize * s)]  = (osc_1_final);
            bufferRight[i+(blockSize * s)] = (osc_1_final);
        }
    }
}

void FrequencyScannerSynthVoice::process_adsr(float *bufferLeft, float * bufferRight, int buffersize)
{
    for (int i = 0; i < buffersize; i++)
    {
        if(env[0]->getState() != ADSR2::env_idle)
        {
            float val = env[0]->process();
            bufferLeft[i] *= val;
            bufferRight[i] *= val;
        }
        else
        {
            bufferLeft[i] = 0;
            bufferRight[i] = 0.f;
            this->clearCurrentNote();
        }
    }
}

void FrequencyScannerSynthVoice::setParameter(int index, var newValue)
{
    switch (index) {
        case FREQUENCY_SCANNER_WAVEFORM_TYPE:
        {
            int val = (int)newValue;
            waveformType = (WAVEFORM_TYPES)val;
            oscillator[0]->set_subtype(waveformType);
        }
            break;
            
        case FREQUENCY_SCANNER_ENV_AMPLITUDE:
        {
            float val = (float)newValue * 0.01;
            amplitude = val;
        }
            break;
            
        case FREQUENCY_SCANNER_ENV_ATTACK:
        {
            float val = newValue.operator float();
            env[0]->SetAttack(val);
        }
            break;
            
        case FREQUENCY_SCANNER_ENV_DECAY:
        {
            float val = newValue.operator float();
            env[0]->SetDecay(val);
        }
            break;
            
        case FREQUENCY_SCANNER_ENV_SUSTAIN:
        {
            float val = newValue.operator float();
            env[0]->SetSustain((float)val * 0.01);
        }
            break;
            
        case FREQUENCY_SCANNER_ENV_RELEASE:
        {
            float val = newValue.operator float();
            env[0]->SetRelease(val);
        }
            break;

        default: break;
    }
}
