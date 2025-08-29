/*
  ==============================================================================

    WavetableSynthProcessor.cpp
    Created: 31 Mar 2020 9:27:07pm
    Author:  Gary Jones

  ==============================================================================
*/

#include "WavetableSynthProcessor.h"
#include "Parameters.h"
#include <cstdlib>


WavetableSynthVoice::WavetableSynthVoice(double newSampleRate, FrequencyManager * fm)
{
    frequencyManager    = fm;
    sample_rate         = newSampleRate;
    oscillator          = new WaveTableOsc();
    env[0]              = new ADSR2(sample_rate);

    
 
    for (int i = 0; i < 1; i++)
    {
        osc_volume[i] = 1.f;
    }
    
    env[0]->reset();
    
    // Initialize instrument type to Piano
    instrumentType = PIANO;
}

WavetableSynthVoice::~WavetableSynthVoice()  { }

void WavetableSynthVoice::setCurrentPlaybackSampleRate (double newRate)
{
    sample_rate = newRate;
    SynthesiserVoice::setCurrentPlaybackSampleRate(newRate);
    
    double newSampleRate = sample_rate;
    
    // set new samplerate to OSCs with newSampleRate
    for (int i = 0; i < 1; i++)
    {
        oscillator->setSampleRate(newSampleRate);
        env[i]->setNewSamplerate(newSampleRate);
    }
}

bool WavetableSynthVoice::canPlaySound (SynthesiserSound* sound)
{
    return dynamic_cast <WavetableSynthSound*> (sound) != 0;
}

void WavetableSynthVoice::startNote (int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition)
{
//    for (int i =0; i<1; i++)
//    {
//        oscillator->reset();
//        oscillator->set_frequency(frequencyManager->getFrequencyForMIDINote(midiNoteNumber));
//        oscillator->triggerNote();
//    }
    
    for (int i = 0; i < 1; i++) env[i]->gate(1);
}

void WavetableSynthVoice::stopNote (float velocity, bool allowTailOff)
{
    // call release
    for(int i = 0; i < 1; i++) env[i]->gate(0);
}

void WavetableSynthVoice::renderNextBlock (AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
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
    
    
//    float * buf_left = outputBuffer.getWritePointer(0);
//    float * buf_right = outputBuffer.getWritePointer(1);
//
//    float voiceBufferLeft[numSamples]; for (int i = 0; i < numSamples; i++) voiceBufferLeft[i] = 0.f;
//    float voiceBufferRight[numSamples]; for (int i = 0; i < numSamples; i++) voiceBufferRight[i] = 0.f;
//
//    ProcessBuffer(voiceBufferLeft, voiceBufferRight, numSamples);
//
//    for (int i = 0; i < numSamples; i++)
//    {
//        buf_left[i]     += voiceBufferLeft[i];
//        buf_right[i]    += voiceBufferRight[i];
//    }
}

void WavetableSynthVoice::ProcessBuffer(float *bufferLeft, float * bufferRight, int buffersize)
{
    // check
    process_oscs(bufferLeft, bufferRight, buffersize);
    process_adsr(bufferLeft, bufferRight, buffersize);
}

void WavetableSynthVoice::process_oscs(float *bufferLeft, float * bufferRight, int buffersize)
{
    // Use instrument-specific synthesis based on selected instrument
    switch (instrumentType)
    {
        case PIANO:
            synthesizePiano(bufferLeft, bufferRight, buffersize);
            break;
        case FLUTE:
            synthesizeFlute(bufferLeft, bufferRight, buffersize);
            break;
        case GUITAR:
            synthesizeGuitar(bufferLeft, bufferRight, buffersize);
            break;
        case STRINGS:
            synthesizeStrings(bufferLeft, bufferRight, buffersize);
            break;
        case HARP:
            synthesizeHarp(bufferLeft, bufferRight, buffersize);
            break;
        default:
            // Default to wavetable synthesis for other instruments
            if (oscillator != nullptr)
            {
                oscillator->processBuffer(bufferLeft, buffersize, true);
                
                for (int i = 0; i < buffersize; i++)
                {
                    bufferLeft[i] *= amplitude * osc_volume[0];
                    bufferRight[i] = bufferLeft[i];
                }
            }
            break;
    }
}


void WavetableSynthVoice::process_adsr(float *bufferLeft, float * bufferRight, int buffersize)
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

void WavetableSynthVoice::setParameter(int index, var newValue)
{
    switch (index) {
        case INSTRUMENT_TYPE:
        {
            int val = (int)newValue;
            instrumentType = val;
        }
            break;
        case WAVEFORM_TYPE:
        {
            int val = (int)newValue;
            waveformType = (WAVEFORM_TYPES)val;
//            oscillator[0]->set_subtype(waveformType);
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
        default: break;
    }
}



WavetableSynthProcessor::WavetableSynthProcessor(float hwsamplerate, FrequencyManager * fm)
{
    internal_sample_rate    = hwsamplerate * OVERSAMPLE_FACTOR;
    frequencyManager        = fm;
    
    clearVoices();

    for (int i = 0; i < MAX_NUM_VOICES; i++)
    {
        cVoices[i] = new WavetableSynthVoice(internal_sample_rate, frequencyManager);
        cVoices[i]->setVoiceRef(i);
        this->addVoice(cVoices[i]);
    }
    
    clearSounds();
    
    addSound (new WavetableSynthSound());
    setNoteStealingEnabled(true);
    
    initBaseWaveType(0);
    
}
WavetableSynthProcessor::~WavetableSynthProcessor() { }

void WavetableSynthProcessor::prepareToPlay(float hwSampleRate, int blockSize)
{
    internal_sample_rate = hwSampleRate * OVERSAMPLE_FACTOR;
    
    setCurrentPlaybackSampleRate(internal_sample_rate);
}

void WavetableSynthProcessor::processBlock(AudioBuffer<float>& buffer,
                  MidiBuffer& midiMessages)
{
    renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

void WavetableSynthProcessor::setParameter(int index, var newValue)
{
    for (int i = 0; i < MAX_NUM_VOICES; i++) {
        cVoices[i]->setParameter(index, newValue);
    }
}

void WavetableSynthProcessor::initBaseWaveType(int type) // 0= tr
{
    for (int i =0 ; i < MAX_NUM_VOICES; i++)
    {
        cVoices[i]->oscillator = WaveTableOsc::waveOsc(getWavetableBuffer(type, 2048), 2048);
    }
}


double * WavetableSynthProcessor::getWavetableBuffer(int type, int length)
{
    double * buffer = new double[length];
    
    if (type == 0)
    {
        // generate sinewave
        double frequency = 1.f;

        for (int i = 0; i < length; i++)
        {
            buffer[i] = sin(2 * M_PI * frequency / length * i);
        }
    }
    else if (type == 1)
    {
        // generate tri
        double samp = -1.f;
        double acc  = 2.f / (length / 2.f);
        
        buffer[0] = samp;
        
        for (int i = 1; i < length; i++)
        {
            if (i < (length / 2)) { buffer[i] = buffer[i-1] + acc; }
            else { buffer[i] = buffer[i-1] - acc; }
        }
    }
    else if (type == 2)
    {
        // generate sqr
        for (int i = 0; i < length; i++)
        {
            if (i < (length / 2)) { buffer[i] = 1.f; } else { buffer[i] = -1.f; }
        }
    }
    else if (type == 3)
    {
        // generate saw
        double samp = 1.f;
        double acc  = 2.f / length;
        
        buffer[0] = samp;
        
        for (int i = 1; i < length; i++)
        {
            buffer[i] = buffer[i-1] - acc;
        }
    }
    else if (type == 4)
    {
        for (int i = 0; i < length; i++)
        {
            buffer[i] = 0.f;
        }
    }
    
    baseWavetable = buffer;
    
    return buffer;
}

double * WavetableSynthProcessor::getBaseWavetableBuffer()
{
    return baseWavetable;
}

void WavetableSynthProcessor::setWavetableBuffer(double * table)
{
    baseWavetable = table;
    
    for (int i =0 ; i < MAX_NUM_VOICES; i++)
    {
        cVoices[i]->oscillator = WaveTableOsc::waveOsc(baseWavetable, 2048);
    }
}


//===============================================================
// in order for new chord system to work, we need to override this method pushing frequencies direct to the oscillator
//---------------------------------------------------------------
void WavetableSynthProcessor::noteOn (const int midiChannel, const int midiNoteNumber, const float frequencyDirect)
{
    const ScopedLock sl (lock);

    for (auto* sound : sounds)
    {
        if (sound->appliesToNote (midiNoteNumber) && sound->appliesToChannel (midiChannel))
        {
            // If hitting a note that's still ringing, stop it first (it could be
            // still playing because of the sustain or sostenuto pedal).
            for (auto* voice : voices)
                if (voice->getCurrentlyPlayingNote() == midiNoteNumber && voice->isPlayingChannel (midiChannel))
                    stopVoice (voice, 1.0f, true);

            WavetableSynthVoice * v = (WavetableSynthVoice*)findFreeVoice (sound, midiChannel, midiNoteNumber, false);
            
            if (v != nullptr)
            {
                v->setFrequencyDirect(frequencyDirect);
                startVoice (v, sound, midiChannel, midiNoteNumber, 1.0);
            }
            
        }
    }
}

// FrequencyPlayer Wavetable Synth

FrequencyPlayerWavetableSynthVoice::FrequencyPlayerWavetableSynthVoice(double newSampleRate, FrequencyManager * fm)
{
    frequencyManager    = fm;
    sample_rate         = newSampleRate;
    oscillator          = new WaveTableOsc();
    env[0]              = new ADSR2(sample_rate);

    
 
    for (int i = 0; i < 1; i++)
    {
        osc_volume[i] = 1.f;
    }
    
    env[0]->reset();
}

FrequencyPlayerWavetableSynthVoice::~FrequencyPlayerWavetableSynthVoice()  { }

void FrequencyPlayerWavetableSynthVoice::setCurrentPlaybackSampleRate (double newRate)
{
    sample_rate = newRate;
    SynthesiserVoice::setCurrentPlaybackSampleRate(newRate);
    
    double newSampleRate = sample_rate;
    
    // set new samplerate to OSCs with newSampleRate
    for (int i = 0; i < 1; i++)
    {
        oscillator->setSampleRate(newSampleRate);
        env[i]->setNewSamplerate(newSampleRate);
    }
}

bool FrequencyPlayerWavetableSynthVoice::canPlaySound (SynthesiserSound* sound)
{
    return dynamic_cast <WavetableSynthSound*> (sound) != 0;
}

void FrequencyPlayerWavetableSynthVoice::startNote (int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition)
{
    calcManipulateFrequencies(); // check manipulate frequency bool, and sets parameter variables accordingly
    
    for (int i =0; i<1; i++)
    {
//        oscillator[i].reset();
//        oscillator[i].triggerNote();
        
        if (!frequencySource)
        {
            oscillator[0].setFrequency(c_chosenFrequency / sample_rate);
        }
        else
        {
            calcRangeSweep();
        }
    }
    
    for (int i = 0; i < 1; i++)
        env[i]->gate(1);
    
//    for (int i =0; i<1; i++)
//    {
//        oscillator->reset();
//        oscillator->set_frequency(frequencyManager->getFrequencyForMIDINote(midiNoteNumber));
//        oscillator->triggerNote();
//    }
    
//    for (int i = 0; i < 1; i++) env[i]->gate(1);
}

void FrequencyPlayerWavetableSynthVoice::stopNote (float velocity, bool allowTailOff)
{
    // call release
    for(int i = 0; i < 1; i++) env[i]->gate(0);
}

void FrequencyPlayerWavetableSynthVoice::renderNextBlock (AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
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

void FrequencyPlayerWavetableSynthVoice::ProcessBuffer(float *bufferLeft, float * bufferRight, int buffersize)
{
    if (frequencySource && shouldProcessRangeSweep)
    {
        processRangeSweep(buffersize);
    }
    
    // check
    process_oscs(bufferLeft, bufferRight, buffersize);
    process_adsr(bufferLeft, bufferRight, buffersize);
}

void FrequencyPlayerWavetableSynthVoice::process_oscs(float *bufferLeft, float * bufferRight, int buffersize)
{
    if (oscillator != nullptr)
    {
        oscillator->processBuffer(bufferLeft, buffersize, true);
        
        for (int i = 0; i < buffersize; i++)
        {
            bufferLeft[i] *= amplitude * osc_volume[0];
            bufferRight[i] = bufferLeft[i];
        }
    }
}


void FrequencyPlayerWavetableSynthVoice::process_adsr(float *bufferLeft, float * bufferRight, int buffersize)
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

void FrequencyPlayerWavetableSynthVoice::setParameter(int index, var newValue)
{
    switch (index) {
        case FREQUENCY_PLAYER_WAVEFORM_TYPE:
        {
            int val = (int)newValue;
            waveformType = (WAVEFORM_TYPES)val;
//            oscillator[0]->set_subtype(waveformType);
        }
            break;
            
        case FREQUENCY_PLAYER_AMPLITUDE:
        {
            amplitude = (float)newValue * 0.01;
        }
            break;
            
        case FREQUENCY_PLAYER_ATTACK:
        {
            float val = newValue.operator float();
            env[0]->SetAttack(val);
        }
            break;
            
        case FREQUENCY_PLAYER_SUSTAIN:
        {
            float val = newValue.operator float();
            env[0]->SetSustain((float)val * 0.01);
        }
            break;
        case FREQUENCY_PLAYER_DECAY:
        {
            float val = newValue.operator float();
            env[0]->SetDecay(val);
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
            bool boolVal = newValue.operator bool();
            
            if (boolVal != frequencySource)
            {
                frequencySource = !frequencySource;
            }
        }
            break;
        case FREQUENCY_PLAYER_CHOOSE_FREQ:
        {
            p_chosenFrequency = newValue.operator float();
            calcManipulateFrequencies();
            
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

void FrequencyPlayerWavetableSynthVoice::calcRangeSweep()
{
    sampleInc = 0;
    
    // set Min Freq to begin
    currentFrequency = c_rangeMin;
    oscillator[0].setFrequency(currentFrequency / sample_rate);
    
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

void FrequencyPlayerWavetableSynthVoice::processRangeSweep(int bufferLength)
{
    if (shouldProcessRangeSweep)
    {
        if (rangeLog)
        {
            float pos = (float)sampleInc / rangeLengthInSamples;
            currentFrequency = powf(10.f, pos * (log10f(c_rangeMax) - log10f(c_rangeMin)) + log10f(c_rangeMin));
            oscillator[0].setFrequency(currentFrequency / sample_rate);
        }
        else
        {
            currentFrequency += (frequencyIncrementPerSample * bufferLength);
            oscillator[0].setFrequency(currentFrequency / sample_rate);
        }
        
        sampleInc += bufferLength;
        
        if (currentFrequency == c_rangeMax || sampleInc >= rangeLengthInSamples)
        {
            shouldProcessRangeSweep = false;
        }
    }
}

void FrequencyPlayerWavetableSynthVoice::calcManipulateFrequencies()
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



FrequencyPlayerWavetableSynthProcessor::FrequencyPlayerWavetableSynthProcessor(float hwsamplerate, FrequencyManager * fm)
{
    internal_sample_rate    = hwsamplerate * OVERSAMPLE_FACTOR;
    frequencyManager        = fm;
    
    clearVoices();

    for (int i = 0; i < MAX_NUM_VOICES; i++)
    {
        cVoices[i] = new FrequencyPlayerWavetableSynthVoice(internal_sample_rate, frequencyManager);
        cVoices[i]->setVoiceRef(i);
        this->addVoice(cVoices[i]);
    }
    
    clearSounds();
    
    addSound (new WavetableSynthSound());
    setNoteStealingEnabled(true);
    
    initBaseWaveType(0);
    
}
FrequencyPlayerWavetableSynthProcessor::~FrequencyPlayerWavetableSynthProcessor() { }

void FrequencyPlayerWavetableSynthProcessor::prepareToPlay(float hwSampleRate, int blockSize)
{
    internal_sample_rate = hwSampleRate * OVERSAMPLE_FACTOR;
    
    setCurrentPlaybackSampleRate(internal_sample_rate);
}

void FrequencyPlayerWavetableSynthProcessor::processBlock(AudioBuffer<float>& buffer,
                  MidiBuffer& midiMessages)
{
    renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

void FrequencyPlayerWavetableSynthProcessor::setParameter(int index, var newValue)
{
    for (int i = 0; i < MAX_NUM_VOICES; i++) {
        cVoices[i]->setParameter(index, newValue);
    }
}

void FrequencyPlayerWavetableSynthProcessor::initBaseWaveType(int type) // 0= tr
{
    for (int i =0 ; i < MAX_NUM_VOICES; i++)
    {
        cVoices[i]->oscillator = WaveTableOsc::waveOsc(getWavetableBuffer(type, 2048), 2048);
    }
}


double * FrequencyPlayerWavetableSynthProcessor::getWavetableBuffer(int type, int length)
{
    double * buffer = new double[length];
    
    if (type == 0)
    {
        // generate sinewave
        double frequency = 1.f;

        for (int i = 0; i < length; i++)
        {
            buffer[i] = sin(2 * M_PI * frequency / length * i);
        }
    }
    else if (type == 1)
    {
        // generate tri
        double samp = -1.f;
        double acc  = 2.f / (length / 2.f);
        
        buffer[0] = samp;
        
        for (int i = 1; i < length; i++)
        {
            if (i < (length / 2)) { buffer[i] = buffer[i-1] + acc; }
            else { buffer[i] = buffer[i-1] - acc; }
        }
    }
    else if (type == 2)
    {
        // generate sqr
        for (int i = 0; i < length; i++)
        {
            if (i < (length / 2)) { buffer[i] = 1.f; } else { buffer[i] = -1.f; }
        }
    }
    else if (type == 3)
    {
        // generate saw
        double samp = 1.f;
        double acc  = 2.f / length;
        
        buffer[0] = samp;
        
        for (int i = 1; i < length; i++)
        {
            buffer[i] = buffer[i-1] - acc;
        }
    }
    else if (type == 4)
    {
        for (int i = 0; i < length; i++)
        {
            buffer[i] = 0.f;
        }
    }
    
    baseWavetable = buffer;
    
    return buffer;
}

double * FrequencyPlayerWavetableSynthProcessor::getBaseWavetableBuffer()
{
    return baseWavetable;
}

void FrequencyPlayerWavetableSynthProcessor::setWavetableBuffer(double * table)
{
    baseWavetable = table;
    
    for (int i =0 ; i < MAX_NUM_VOICES; i++)
    {
        cVoices[i]->oscillator = WaveTableOsc::waveOsc(baseWavetable, 2048);
    }
}


FrequencyScannerWavetableSynthVoice::FrequencyScannerWavetableSynthVoice(double newSampleRate, FrequencyManager * fm)
{
    frequencyManager    = fm;
    sample_rate         = newSampleRate;
    oscillator          = new WaveTableOsc();
    env[0]              = new ADSR2(sample_rate);

    for (int i = 0; i < 1; i++)
    {
        osc_volume[i] = 1.f;
    }
    
    env[0]->reset();
}

FrequencyScannerWavetableSynthVoice::~FrequencyScannerWavetableSynthVoice()  { }

void FrequencyScannerWavetableSynthVoice::setCurrentPlaybackSampleRate (double newRate)
{
    sample_rate = newRate;
    SynthesiserVoice::setCurrentPlaybackSampleRate(newRate);
    
    double newSampleRate = sample_rate;
    
    // set new samplerate to OSCs with newSampleRate
    for (int i = 0; i < 1; i++)
    {
        oscillator->setSampleRate(newSampleRate);
        env[i]->setNewSamplerate(newSampleRate);
    }
}

bool FrequencyScannerWavetableSynthVoice::canPlaySound (SynthesiserSound* sound)
{
    return dynamic_cast <WavetableSynthSound*> (sound) != 0;
}

void FrequencyScannerWavetableSynthVoice::startNote (int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition)
{
//    for (int i =0; i<1; i++)
//    {
//        oscillator->reset();
//        oscillator->set_frequency(frequencyManager->getFrequencyForMIDINote(midiNoteNumber));
//        oscillator->triggerNote();
//    }
    
    for (int i = 0; i < 1; i++) env[i]->gate(1);
}

void FrequencyScannerWavetableSynthVoice::stopNote (float velocity, bool allowTailOff)
{
    // call release
    for(int i = 0; i < 1; i++) env[i]->gate(0);
}

void FrequencyScannerWavetableSynthVoice::renderNextBlock (AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
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
    
//    float * buf_left = outputBuffer.getWritePointer(0);
//    float * buf_right = outputBuffer.getWritePointer(1);
//
//    float voiceBufferLeft[numSamples]; for (int i = 0; i < numSamples; i++) voiceBufferLeft[i] = 0.f;
//    float voiceBufferRight[numSamples]; for (int i = 0; i < numSamples; i++) voiceBufferRight[i] = 0.f;
//
//    ProcessBuffer(voiceBufferLeft, voiceBufferRight, numSamples);
//
//    for (int i = 0; i < numSamples; i++)
//    {
//        buf_left[i]     += voiceBufferLeft[i];
//        buf_right[i]    += voiceBufferRight[i];
//    }
}

void FrequencyScannerWavetableSynthVoice::ProcessBuffer(float *bufferLeft, float * bufferRight, int buffersize)
{
    // check
    process_oscs(bufferLeft, bufferRight, buffersize);
    process_adsr(bufferLeft, bufferRight, buffersize);
}

void FrequencyScannerWavetableSynthVoice::process_oscs(float *bufferLeft, float * bufferRight, int buffersize)
{
    if (oscillator != nullptr)
    {
        oscillator->processBuffer(bufferLeft, buffersize, true);
        
        for (int i = 0; i < buffersize; i++)
        {
            bufferLeft[i] *= amplitude * osc_volume[0];
            bufferRight[i] = bufferLeft[i];
        }
    }
}


void FrequencyScannerWavetableSynthVoice::process_adsr(float *bufferLeft, float * bufferRight, int buffersize)
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

void FrequencyScannerWavetableSynthVoice::setParameter(int index, var newValue)
{
    switch (index) {

        case FREQUENCY_SCANNER_WAVEFORM_TYPE:
        {
            int val = (int)newValue;
            waveformType = (WAVEFORM_TYPES)val;
//            oscillator[0]->set_subtype(waveformType);
        }
            break;
            
        case FREQUENCY_SCANNER_ENV_AMPLITUDE:
        {
            amplitude = (float)newValue * 0.01;
        }
            break;
            
        case FREQUENCY_SCANNER_ENV_ATTACK:
        {
            float val = newValue.operator float();
            env[0]->SetAttack(val);
        }
            break;
            
        case FREQUENCY_SCANNER_ENV_SUSTAIN:
        {
            float val = newValue.operator float();
            env[0]->SetSustain((float)val * 0.01);
        }
            break;
        case FREQUENCY_SCANNER_ENV_DECAY:
        {
            float val = newValue.operator float();
            env[0]->SetDecay(val);
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



FrequencyScannerWavetableSynthProcessor::FrequencyScannerWavetableSynthProcessor(float hwsamplerate, FrequencyManager * fm)
{
    internal_sample_rate    = hwsamplerate * OVERSAMPLE_FACTOR;
    frequencyManager        = fm;
    
    clearVoices();

    for (int i = 0; i < MAX_NUM_VOICES; i++)
    {
        cVoices[i] = new FrequencyScannerWavetableSynthVoice(internal_sample_rate, frequencyManager);
        cVoices[i]->setVoiceRef(i);
        this->addVoice(cVoices[i]);
    }
    
    clearSounds();
    
    addSound (new WavetableSynthSound());
    setNoteStealingEnabled(true);
    
    initBaseWaveType(0);
    
}
FrequencyScannerWavetableSynthProcessor::~FrequencyScannerWavetableSynthProcessor() { }

void FrequencyScannerWavetableSynthProcessor::prepareToPlay(float hwSampleRate, int blockSize)
{
    internal_sample_rate = hwSampleRate * OVERSAMPLE_FACTOR;
    
    setCurrentPlaybackSampleRate(internal_sample_rate);
}

void FrequencyScannerWavetableSynthProcessor::processBlock(AudioBuffer<float>& buffer,
                  MidiBuffer& midiMessages)
{
    renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

void FrequencyScannerWavetableSynthProcessor::setParameter(int index, var newValue)
{
    for (int i = 0; i < MAX_NUM_VOICES; i++) {
        cVoices[i]->setParameter(index, newValue);
    }
}

void FrequencyScannerWavetableSynthProcessor::initBaseWaveType(int type) // 0= tr
{
    for (int i =0 ; i < MAX_NUM_VOICES; i++)
    {
        cVoices[i]->oscillator = WaveTableOsc::waveOsc(getWavetableBuffer(type, 2048), 2048);
    }
}


double * FrequencyScannerWavetableSynthProcessor::getWavetableBuffer(int type, int length)
{
    double * buffer = new double[length];
    
    if (type == 0)
    {
        // generate sinewave
        double frequency = 1.f;

        for (int i = 0; i < length; i++)
        {
            buffer[i] = sin(2 * M_PI * frequency / length * i);
        }
    }
    else if (type == 1)
    {
        // generate tri
        double samp = -1.f;
        double acc  = 2.f / (length / 2.f);
        
        buffer[0] = samp;
        
        for (int i = 1; i < length; i++)
        {
            if (i < (length / 2)) { buffer[i] = buffer[i-1] + acc; }
            else { buffer[i] = buffer[i-1] - acc; }
        }
    }
    else if (type == 2)
    {
        // generate sqr
        for (int i = 0; i < length; i++)
        {
            if (i < (length / 2)) { buffer[i] = 1.f; } else { buffer[i] = -1.f; }
        }
    }
    else if (type == 3)
    {
        // generate saw
        double samp = 1.f;
        double acc  = 2.f / length;
        
        buffer[0] = samp;
        
        for (int i = 1; i < length; i++)
        {
            buffer[i] = buffer[i-1] - acc;
        }
    }
    else if (type == 4)
    {
        for (int i = 0; i < length; i++)
        {
            buffer[i] = 0.f;
        }
    }
    
    baseWavetable = buffer;
    
    return buffer;
}

double * FrequencyScannerWavetableSynthProcessor::getBaseWavetableBuffer()
{
    return baseWavetable;
}

void FrequencyScannerWavetableSynthProcessor::setWavetableBuffer(double * table)
{
    baseWavetable = table;
    
    for (int i =0 ; i < MAX_NUM_VOICES; i++)
    {
        cVoices[i]->oscillator = WaveTableOsc::waveOsc(baseWavetable, 2048);
    }
}


//===============================================================
// in order for new chord system to work, we need to override this method pushing frequencies direct to the oscillator
//---------------------------------------------------------------
void FrequencyScannerWavetableSynthProcessor::noteOn (const int midiChannel, const int midiNoteNumber, const float frequencyDirect)
{
    const ScopedLock sl (lock);

    for (auto* sound : sounds)
    {
        if (sound->appliesToNote (midiNoteNumber) && sound->appliesToChannel (midiChannel))
        {
            // If hitting a note that's still ringing, stop it first (it could be
            // still playing because of the sustain or sostenuto pedal).
            for (auto* voice : voices)
                if (voice->getCurrentlyPlayingNote() == midiNoteNumber && voice->isPlayingChannel (midiChannel))
                    stopVoice (voice, 1.0f, true);

            FrequencyScannerWavetableSynthVoice * v = (FrequencyScannerWavetableSynthVoice*)findFreeVoice (sound, midiChannel, midiNoteNumber, false);
            v->setFrequencyDirect(frequencyDirect);
            startVoice (v, sound, midiChannel, midiNoteNumber, 1.0);
        }
    }
}

// =================================================================================
// Instrument-specific synthesis methods for WavetableSynthVoice
// =================================================================================

void WavetableSynthVoice::synthesizePiano(float* bufferLeft, float* bufferRight, int numSamples)
{
    // Piano synthesis using additive synthesis with harmonic content
    const float phase_increment = currentFrequency * 2.0f * juce::MathConstants<float>::pi / (float)sample_rate;
    static float phase = 0.0f;
    
    for (int i = 0; i < numSamples; ++i)
    {
        // Generate piano-like sound with multiple harmonics
        float sample = 0.0f;
        
        // Fundamental frequency
        sample += 0.8f * std::sin(phase);
        
        // Second harmonic (softer for piano)
        sample += 0.4f * std::sin(phase * 2.0f);
        
        // Third harmonic 
        sample += 0.2f * std::sin(phase * 3.0f);
        
        // Fourth harmonic 
        sample += 0.1f * std::sin(phase * 4.0f);
        
        // Fifth harmonic (slight)
        sample += 0.05f * std::sin(phase * 5.0f);
        
        // Add slight inharmonic content for realism
        sample += 0.02f * std::sin(phase * 2.1f);
        sample += 0.01f * std::sin(phase * 3.1f);
        
        // Apply amplitude and volume
        sample *= amplitude * osc_volume[0] * 0.3f; // Scale down to prevent clipping
        
        bufferLeft[i] = sample;
        bufferRight[i] = sample;
        
        // Update phase
        phase += phase_increment;
        if (phase >= 2.0f * juce::MathConstants<float>::pi)
            phase -= 2.0f * juce::MathConstants<float>::pi;
    }
}

void WavetableSynthVoice::synthesizeFlute(float* bufferLeft, float* bufferRight, int numSamples)
{
    // Flute synthesis using filtered noise and sine waves
    const float phase_increment = currentFrequency * 2.0f * juce::MathConstants<float>::pi / (float)sample_rate;
    static float phase = 0.0f;
    
    for (int i = 0; i < numSamples; ++i)
    {
        // Generate flute-like sound (mostly fundamental with some breath noise)
        float sample = 0.0f;
        
        // Strong fundamental
        sample += 0.9f * std::sin(phase);
        
        // Weak second harmonic
        sample += 0.1f * std::sin(phase * 2.0f);
        
        // Very weak third harmonic
        sample += 0.05f * std::sin(phase * 3.0f);
        
        // Add breath noise (random component)
        float noise = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
        sample += noise * 0.02f;
        
        // Apply amplitude and volume
        sample *= amplitude * osc_volume[0] * 0.4f;
        
        bufferLeft[i] = sample;
        bufferRight[i] = sample;
        
        // Update phase
        phase += phase_increment;
        if (phase >= 2.0f * juce::MathConstants<float>::pi)
            phase -= 2.0f * juce::MathConstants<float>::pi;
    }
}

void WavetableSynthVoice::synthesizeGuitar(float* bufferLeft, float* bufferRight, int numSamples)
{
    // Guitar synthesis using Karplus-Strong-like plucked string model
    const float phase_increment = currentFrequency * 2.0f * juce::MathConstants<float>::pi / (float)sample_rate;
    static float phase = 0.0f;
    
    for (int i = 0; i < numSamples; ++i)
    {
        // Generate guitar-like plucked sound
        float sample = 0.0f;
        
        // Generate sawtooth-like waveform (rich in harmonics)
        float sawWave = 2.0f * (phase / (2.0f * juce::MathConstants<float>::pi)) - 1.0f;
        if (sawWave > 1.0f) sawWave = 2.0f - sawWave;
        if (sawWave < -1.0f) sawWave = -2.0f - sawWave;
        
        sample += 0.7f * sawWave;
        
        // Add some harmonics for brightness
        sample += 0.3f * std::sin(phase * 2.0f);
        sample += 0.15f * std::sin(phase * 3.0f);
        sample += 0.08f * std::sin(phase * 4.0f);
        
        // Apply amplitude and volume
        sample *= amplitude * osc_volume[0] * 0.35f;
        
        bufferLeft[i] = sample;
        bufferRight[i] = sample;
        
        // Update phase
        phase += phase_increment;
        if (phase >= 2.0f * juce::MathConstants<float>::pi)
            phase -= 2.0f * juce::MathConstants<float>::pi;
    }
}

void WavetableSynthVoice::synthesizeStrings(float* bufferLeft, float* bufferRight, int numSamples)
{
    // String synthesis using additive synthesis with rich harmonics
    const float phase_increment = currentFrequency * 2.0f * juce::MathConstants<float>::pi / (float)sample_rate;
    static float phase = 0.0f;
    
    for (int i = 0; i < numSamples; ++i)
    {
        // Generate string-like sound with many harmonics
        float sample = 0.0f;
        
        // Fundamental
        sample += 0.6f * std::sin(phase);
        
        // Strong harmonics for string character
        sample += 0.4f * std::sin(phase * 2.0f);
        sample += 0.3f * std::sin(phase * 3.0f);
        sample += 0.2f * std::sin(phase * 4.0f);
        sample += 0.15f * std::sin(phase * 5.0f);
        sample += 0.1f * std::sin(phase * 6.0f);
        sample += 0.08f * std::sin(phase * 7.0f);
        sample += 0.06f * std::sin(phase * 8.0f);
        
        // Apply amplitude and volume
        sample *= amplitude * osc_volume[0] * 0.25f; // Scale down due to many harmonics
        
        bufferLeft[i] = sample;
        bufferRight[i] = sample;
        
        // Update phase
        phase += phase_increment;
        if (phase >= 2.0f * juce::MathConstants<float>::pi)
            phase -= 2.0f * juce::MathConstants<float>::pi;
    }
}

void WavetableSynthVoice::synthesizeHarp(float* bufferLeft, float* bufferRight, int numSamples)
{
    // Harp synthesis using decaying harmonics
    const float phase_increment = currentFrequency * 2.0f * juce::MathConstants<float>::pi / (float)sample_rate;
    static float phase = 0.0f;
    static float decay = 1.0f;
    
    for (int i = 0; i < numSamples; ++i)
    {
        // Generate harp-like plucked sound with quick decay
        float sample = 0.0f;
        
        // Fundamental with harmonics
        sample += 0.8f * std::sin(phase) * decay;
        sample += 0.4f * std::sin(phase * 2.0f) * decay;
        sample += 0.2f * std::sin(phase * 3.0f) * decay;
        sample += 0.1f * std::sin(phase * 4.0f) * decay;
        
        // Add bell-like metallic harmonics
        sample += 0.1f * std::sin(phase * 2.4f) * decay;
        sample += 0.05f * std::sin(phase * 3.2f) * decay;
        
        // Apply amplitude and volume
        sample *= amplitude * osc_volume[0] * 0.4f;
        
        bufferLeft[i] = sample;
        bufferRight[i] = sample;
        
        // Update phase and decay
        phase += phase_increment;
        if (phase >= 2.0f * juce::MathConstants<float>::pi)
            phase -= 2.0f * juce::MathConstants<float>::pi;
            
        // Slow decay for harp-like envelope
        decay *= 0.9999f;
    }
}
