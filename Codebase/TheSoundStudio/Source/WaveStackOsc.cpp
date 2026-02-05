/*
  ==============================================================================

    WaveStackOsc.cpp
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#include "WaveStackOsc.h"

WaveStackOsc::WaveStackOsc()
{
    refTemp1 = 0; 
	refTemp2 = 1;
    mixTemp = 0.0;
    phasorVal = 0;
	wetDryMix = 0.0;
	filterLPFrequency = 12000.0;
    filterHPFrequency = 200.0;
	
	
    // initilizse stack Arrays first
    convertAudioFileToSampleArray();
    
    osc1 = 0;
    osc2 = 1;
    
    // initalise
    for (int i = 0; i < numWaveStacks; i++)
    {
        _stackOsc[i] = WaveTableOsc::waveOsc(_waveStackArray[i], 2048);
        _stackOsc[i]->setFrequency(432.0 / sample_Rate);
    }
    
    oscVolume = 0.0;
    m_semitone = 1.0;
    m_detune = 1.0;
}

WaveStackOsc::~WaveStackOsc()
{
    
}

void WaveStackOsc::processBuffer(float * buffer, int numSamples)
{

	float **oscBuf = new float* [numWaveStacks];
	for(int i=0; i<numWaveStacks; i++)
		oscBuf[i] = new float[numSamples];

    float * oscMix_buf;
    int osc_buffersize = numSamples;
    
    mixBufferAlloc.alloc(osc_buffersize);
    oscMix_buf = mixBufferAlloc.m_buffer;
    
    setBufferReference(morph);
    
    int ref1 = osc1;
    int ref2 = osc2;
    float mix = wetDryMix;
    
    // process each waveStack
    for (int i = 0; i < numWaveStacks; i++)
    {
        // optimsation, should check if stack is required to render, but alwasy update the phase acc
        // could also make faster subloops of 32 samples to get higher resolution on the mix algo and buffer changes
        
        if (i == ref1 || i == ref2)
        {
            _stackOsc[i]->processBuffer(oscBuf[i], numSamples, true);
        }
        else
        {
            _stackOsc[i]->processBuffer(oscBuf[i], numSamples, false);
        }
        
    }
    
    // now mix algorithm
    for (int i = 0; i < numSamples; i++)
    {
        //  setBufferReference(morph);
        oscMix_buf[i] = (oscBuf[ref2][i] * mix) + (1.0 - mix) * oscBuf[ref1][i];
        // oscMix_buf[i] = oscBuf[ref1][i]; // use this test integrity of individual buffers
        buffer[i] = oscMix_buf[i] * oscVolume;
    }
    
    
	for(int i=0; i<numWaveStacks; i++) delete [] oscBuf[i];
	delete [] oscBuf;

}

void WaveStackOsc::runTestOnBuffer()
{
    frequency = 22;
    
    for (int i = 0; i < 2048; i++)
    {
        float val = 0;
        _stackOsc[0]->processBuffer2(&val, 1, true);
        printf("sample %i = %f", i, val);
    }
}

void WaveStackOsc::processBuffer2(float * buffer, int numSamples)
{
    // initialise buffers etc
	// Ivan : same remark than before
	//float oscBuf[numWaveStacks][numSamples];

	float** oscBuf = new float*[numWaveStacks];
	for(int i=0; i<numWaveStacks; i++)
		oscBuf[i] = new float[numSamples];
    
    float * oscMix_buf;
    int osc_buffersize = numSamples;
    
    mixBufferAlloc.alloc(osc_buffersize);
    oscMix_buf = mixBufferAlloc.m_buffer;
    
    // now mix algorithm
    for (int i = 0; i < numSamples; i++)
    {
        if (_stackOsc[0]->getNow())
        {
            setBufferReference(morph);
        }
        
        int ref1 = osc1;
        int ref2 = osc2;
        
        float sampbuf1 = 0;
        float sampbuf2 = 0;
        
        // process each waveStack
        for (int j = 0; j < numWaveStacks; j++)
        {
            if ((j == ref1 || j == ref2))
            {
                _stackOsc[j]->processBuffer2(&sampbuf1, 1, true);
                oscBuf[j][i] = sampbuf1;
            }
            else
            {
                _stackOsc[j]->processBuffer2(&sampbuf2, 1, false);
                oscBuf[j][i] = sampbuf2;
            }
        }
        oscMix_buf[i] = (oscBuf[osc2][i] * wetDryMix) + (1.0 - wetDryMix) * oscBuf[osc1][i];

        buffer[i] = oscMix_buf[i] * oscVolume;
    }

	for(int i=0; i<numWaveStacks; i++) delete [] oscBuf[i];
	delete [] oscBuf;
}

double WaveStackOsc::getPhasorVal()
{
    phasorVal = _stackOsc[0]->getPhasorValue();
    return phasorVal;
}

void WaveStackOsc::setBufferReference(float morphValue)
{
    float boundary = 3100 / numWaveStacks-1;
    
    for (int i = 0; i < numWaveStacks-1; i++)
    {
        if (morphValue >= boundary * i && morphValue < boundary * (i + 1))
        {
            osc1 = i;
            osc2 = i + 1;
            wetDryMix = (morphValue - boundary * i - 1 ) / 100.0;
        }
    }
}

bool WaveStackOsc::AreSame(double a, double b)
{
    if (fabs(a - b) < 0.000001) { return true; }
    else return false;
}

void WaveStackOsc::setFrequency(float f)
{
    frequency = f;
    applyFrequency();
}

void WaveStackOsc::applyFrequency()
{
    float f = frequency * m_semitone * m_detune;
    
    for (int i = 0; i < numWaveStacks; i++)
    {
        _stackOsc[i]->setFrequency(f / sample_Rate);
    }
   
    
//    _stackOsc[0]->setFrequency(f / sample_Rate);
//    _stackOsc[1]->setFrequency(f / sample_Rate);
//    _stackOsc[2]->setFrequency(f / sample_Rate);
//    _stackOsc[3]->setFrequency(f / sample_Rate);
//    _stackOsc[4]->setFrequency(f / sample_Rate);
//    _stackOsc[5]->setFrequency(f / sample_Rate);
//    _stackOsc[6]->setFrequency(f / sample_Rate);
//    _stackOsc[7]->setFrequency(f / sample_Rate);
//    _stackOsc[8]->setFrequency(f / sample_Rate);
//    _stackOsc[9]->setFrequency(f / sample_Rate);
//    _stackOsc[10]->setFrequency(f / sample_Rate);
//    _stackOsc[11]->setFrequency(f / sample_Rate);
//    _stackOsc[12]->setFrequency(f / sample_Rate);
//    _stackOsc[13]->setFrequency(f / sample_Rate);
//    _stackOsc[14]->setFrequency(f / sample_Rate);
//    _stackOsc[15]->setFrequency(f / sample_Rate);
//    _stackOsc[16]->setFrequency(f / sample_Rate);
//    _stackOsc[17]->setFrequency(f / sample_Rate);
//    _stackOsc[18]->setFrequency(f / sample_Rate);
//    _stackOsc[19]->setFrequency(f / sample_Rate);
//    _stackOsc[20]->setFrequency(f / sample_Rate);
//    _stackOsc[21]->setFrequency(f / sample_Rate);
//    _stackOsc[22]->setFrequency(f / sample_Rate);
//    _stackOsc[23]->setFrequency(f / sample_Rate);
//    _stackOsc[24]->setFrequency(f / sample_Rate);
//    _stackOsc[25]->setFrequency(f / sample_Rate);
//    _stackOsc[26]->setFrequency(f / sample_Rate);
//    _stackOsc[27]->setFrequency(f / sample_Rate);
//    _stackOsc[28]->setFrequency(f / sample_Rate);
//    _stackOsc[29]->setFrequency(f / sample_Rate);
//    _stackOsc[30]->setFrequency(f / sample_Rate);
//    _stackOsc[31]->setFrequency(f / sample_Rate);

}

void WaveStackOsc::setMorph(float m)
{
    morph = m;
}

void WaveStackOsc::SetZeroPhase()
{
    
}

void WaveStackOsc::setTableRefIndex(int ref)
{
    reinitialiseTable(ref);
}

void WaveStackOsc::updatePhase()
{
    for (int i = 0; i < numWaveStacks; i++)
    {
        _stackOsc[i]->updatePhase(); // remember to switch off update phase in the waveosc class
    }
}

void WaveStackOsc::setPhase(float value)
{
    for (int i = 0; i < numWaveStacks; i++)
    {
        _stackOsc[i]->setPhaseOffset2(value);
    }
}

void WaveStackOsc::setIndex(int idx)
{
    index = idx;
}

void WaveStackOsc::setDetuneValue(float d)
{
    m_detune = convertDetuneToHz(d);
    applyFrequency();
}

void WaveStackOsc::setSemiToneValue(float st)
{
    m_semitone = convertSemiToneToHz(st);
    applyFrequency();
}

void WaveStackOsc::setFilterCoefs(int filter)
{
    switch (filter)
    {
        case 0: filterLP.setCoefficients(IIRCoefficients::makeLowPass(44100, filterLPFrequency)); break;
        case 1: filterHP.setCoefficients(IIRCoefficients::makeHighPass(44100, filterHPFrequency)); break;
            
        default:
            break;
    }
}

//helper functions
float WaveStackOsc::convertSemiToneToHz(float semitone)
{
    if(semitone > 36.0) semitone = 36.0;
    if(semitone < -36.0) semitone = -36.0;
    float aSemitone = pow(2.0, (semitone/12.0));
    return aSemitone;
}

float WaveStackOsc::convertDetuneToHz(float cents)
{
    if(cents > 1200.0) cents = 1200.0;
    
    float aCent = pow(2.0, (cents/1200.0));
    return aCent;
}

void WaveStackOsc::loadTableFromFileChooser(File * file)
{
    printf("load file clicked and arrived to osc");
    AiffAudioFormat aiffFormat;
    
    std::unique_ptr<AudioFormatReader> audioReader (aiffFormat.createReaderFor (new FileInputStream (*file),true));
    
    int numSamples =  static_cast<int>(audioReader->lengthInSamples);
    printf("numSamples = %i", numSamples);
    
    // Create Temporary Sample Buffer
    AudioBuffer<float> floatbuf(1, numSamples);
    
    // read samples from aiff object and place temp sample buffer
    audioReader->read(&floatbuf, 0, numSamples, 0, false, false);
    
    // get float array from temp buffer
    float * floatData = floatbuf.getWritePointer(0);//   .getSampleData(0);
    
    // next step. get aiff with multiple stacks, parse the
    double stack[numWaveStacks][2048];
    
    // envelope code
    
    double attackRate = 10.0;
    double envelopeDuration = 1, startingValue = 0, endingValue=1, exponent=3;
    if(exponent) startingValue = pow(startingValue, 1/exponent);
    if(exponent) endingValue = pow(endingValue, 1/exponent);
    envelopeDuration *= attackRate;
    double valueIncrement = ((endingValue - startingValue)/envelopeDuration);
    
    // create float buffers for each stack
    
    int j = 0;
    
    for (int i = 0; i < numSamples; i++)
    {
        float divisor =  i / (numSamples / numWaveStacks);
        j = (int)floorf(divisor);
        //   printf("j = %i", j);
        stack[j][i-(2048*j)] = (double)floatData[i];
    }
    
    // run env on buffer
    
    for (int j = 0; j < numWaveStacks; j++)
    {
        double currentVolume = startingValue;
        
        for (int i = 0; i < 2048; i++)
        {
            if (i < envelopeDuration)
            {
                stack[j][i] *= pow(currentVolume, exponent);
            }
            else
            {
                //  stack[j][i] *= 1.0;
            }
            
            if(i < envelopeDuration) currentVolume += valueIncrement;
            
        }
        
    }
    
    //copy stacks to WaveStacks
    for (int j = 0; j < numWaveStacks; j++)
    {
        _waveStackArray[j] = nullptr;
        _waveStackArray[j] = stack[j];
    }
    
    // ok got stacks, debug stacks
    for (int j = 0; j < numWaveStacks; j++)
    {
        for (int i = 0; i < 2048; i++)
        {
          //  if (j == 0) printf("stack %i- sample %i , val = %f", j , i , _waveStackArray[j][i]);
        }
    }
    
    // recreate osccilators
    // initalise
    for (int i = 0; i < numWaveStacks; i++)
    {
        _stackOsc[i] = nullptr;
        _stackOsc[i] = WaveTableOsc::waveOsc(_waveStackArray[i], 2048);
        _stackOsc[i]->setFrequency(432.0 / sample_Rate);
    }
 //   runTestOnBuffer();
}

void WaveStackOsc::reinitialiseTable(int ref)
{
    MemoryInputStream * testAiffMemStream {nullptr};

    AiffAudioFormat aiffFormat;
    
    // create reader for mem stream
    std::unique_ptr<AudioFormatReader> audioReader(aiffFormat.createReaderFor(testAiffMemStream, false));
    int numSamples =  static_cast<int>(audioReader->lengthInSamples);
    printf("numSamples = %i", numSamples);
    
    // Create Temporary Sample Buffer
    AudioBuffer<float> floatbuf(1, numSamples);
    
    // read samples from aiff object and place temp sample buffer
    audioReader->read(&floatbuf, 0, numSamples, 0, false, false);
    
    // get float array from temp buffer
    float * floatData = floatbuf.getWritePointer(0); //.getSampleData(0);
    
    // next step. get aiff with multiple stacks, parse the
    double stack[numWaveStacks][2048];
    
    // create float buffers for each stack
    
    int j = 0;
    for (int i = 0; i < numSamples; i++)
    {
        // every 2048 samples needs to accumulate j++
        float divisor =  i / (numSamples / numWaveStacks);
        j = (int)floorf(divisor);
        //   printf("j = %i", j);
        stack[j][i-(2048*j)] = (double)floatData[i];
    }
    
    //copy stacks to WaveStacks
    for (int j = 0; j < numWaveStacks; j++)
    {
        _waveStackArray[j] = nullptr;
        _waveStackArray[j] = stack[j];
    }
    
    // ok got stacks, debug stacks
    for (int j = 0; j < numWaveStacks; j++)
    {
        for (int i = 0; i < 2048; i++)
        {
            //  printf("stack %i- sample %i , val = %f", j , i , _waveStackArray[j][i]);
        }
    }
    
    // delet current oscs
    
    // recreate osccilators
    // initalise
    for (int i = 0; i < numWaveStacks; i++)
    {
        _stackOsc[i] = nullptr;
        _stackOsc[i] = WaveTableOsc::waveOsc(_waveStackArray[i], 2048);
        _stackOsc[i]->setFrequency(432.0 / sample_Rate);
    }
}

void WaveStackOsc::convertAudioFileToSampleArray()
{
    // initialize with sinewave from binary resources
    /*
    MemoryInputStream * testAiffMemStream = new MemoryInputStream(BinaryData::Sine_aif, BinaryData::Sine_aifSize, true); // good
    
    // create aiff format
    AiffAudioFormat aiffFormat;
    
    // create reader for mem stream
    std::unique_ptr<AudioFormatReader> audioReader(aiffFormat.createReaderFor(testAiffMemStream, false));
    int numSamples =  audioReader->lengthInSamples;
    printf("numSamples = %i", numSamples);
    
    // Create Temporary Sample Buffer
    AudioBuffer<float> floatbuf(1, numSamples);
    
    // read samples from aiff object and place temp sample buffer
    audioReader->read(&floatbuf, 0, numSamples, 0, false, false);
    
    // get float array from temp buffer
    float * floatData = floatbuf.getWritePointer(0); //.getSampleData(0);
    
    // next step. get aiff with multiple stacks, parse the
    double stack[numWaveStacks][2048];
    
    
    // envelope code
    
    
    
    double attackRate = 10.0;
    double envelopeDuration = 1, startingValue = 0, endingValue=1, exponent=3;
    if(exponent) startingValue = pow(startingValue, 1/exponent);
    if(exponent) endingValue = pow(endingValue, 1/exponent);
    envelopeDuration *= attackRate;
    double valueIncrement = ((endingValue - startingValue)/envelopeDuration);
    
    // create float buffers for each stack
    
    int j = 0;
    for (int i = 0; i < numSamples; i++)
    {
        // every 2048 samples needs to accumulate j++
        float divisor =  i / (numSamples / numWaveStacks);
        j = (int)floorf(divisor);
        stack[j][i-(2048*j)] = (double)floatData[i];
    }
    
    // run env on buffer
    
    
    for (int j = 0; j < numWaveStacks; j++)
    {
        double currentVolume = startingValue;
        
        for (int i = 0; i < 2048; i++)
        {
            if (i < envelopeDuration)
            {
                stack[j][i] *= pow(currentVolume, exponent);
            }
            else
            {
                //  stack[j][i] *= 1.0;
            }
            
            if(i < envelopeDuration) currentVolume += valueIncrement;
        }
        
    }
    
    //copy stacks to WaveStacks
    for (int j = 0; j < numWaveStacks; j++)
    {
        _waveStackArray[j] = nullptr;
        // set stacks to ewaveStacks
        
        _waveStackArray[j] = stack[j];
    }
    
    // ok got stacks, debug stacks
    for (int j = 0; j < numWaveStacks; j++)
    {
        for (int i = 0; i < 2048; i++)
        {
            //  printf("stack %i- sample %i , val = %f", j , i , _waveStackArray[j][i]);
        }
    }
     
     */
}

void WaveStackOsc::setParameter(int type, float value)
{
//    switch (index) {
//        case 0:
//        {
//            if (type == OSC1_MORPH)
//            {
//                setMorph(value);
//            }
//            else if (type == OSC1_PHASE)
//            {
//                setPhase(value);
//            }
//            else if (type == OSC1_SEMITONE)
//            {
//                setSemiToneValue(value);
//            }
//            else if (type == OSC1_FINETUNE)
//            {
//                setDetuneValue(value);
//            }
//            else if (type == OSC1_RETRIGGER)
//            {
//
//            }
//            else if (type == OSC1_WAVE)
//            {
//                int ref = (int)value;
//                // reinitialiseTable(ref);
//            }
//            else if (type == OSC1_LPFILTER)
//            {
//                filterLPFrequency = value;
//                setFilterCoefs(0);
//            }
//            else if (type == OSC1_HPFILTER)
//            {
//                filterHPFrequency = value;
//                setFilterCoefs(1);
//            }
//            else if (type == OSC1_VOLUME)
//            {
//                oscVolume = value;
//            }
//
//        }
//            break;
//
//        case 1:
//        {
//            if (type == OSC2_MORPH)
//            {
//                setMorph(value);
//            }
//            else if (type == OSC2_PHASE)
//            {
//                setPhase(value);
//            }
//            else if (type == OSC2_SEMITONE)
//            {
//                setSemiToneValue(value);
//            }
//            else if (type == OSC2_FINETUNE)
//            {
//                setDetuneValue(value);
//            }
//            else if (type == OSC2_RETRIGGER)
//            {
//
//            }
//            else if (type == OSC2_WAVE)
//            {
//                int ref = (int)value;
//                // reinitialiseTable(ref);
//            }
//            else if (type == OSC2_LPFILTER)
//            {
//                filterLPFrequency = value;
//                setFilterCoefs(0);
//            }
//            else if (type == OSC2_HPFILTER)
//            {
//                filterHPFrequency = value;
//                setFilterCoefs(1);
//            }
//            else if (type == OSC2_VOLUME)
//            {
//                oscVolume = value;
//            }
//        }
//            break;
//
//        case 2:
//        {
//            if (type == OSC3_MORPH)
//            {
//                setMorph(value);
//            }
//            else if (type == OSC3_PHASE)
//            {
//                setPhase(value);
//            }
//            else if (type == OSC3_SEMITONE)
//            {
//                setSemiToneValue(value);
//            }
//            else if (type == OSC3_FINETUNE)
//            {
//                setDetuneValue(value);
//            }
//            else if (type == OSC3_RETRIGGER)
//            {
//
//            }
//            else if (type == OSC3_WAVE)
//            {
//                int ref = (int)value;
//                // reinitialiseTable(ref);
//            }
//            else if (type == OSC3_LPFILTER)
//            {
//                filterLPFrequency = value;
//                setFilterCoefs(0);
//            }
//            else if (type == OSC3_HPFILTER)
//            {
//                filterHPFrequency = value;
//                setFilterCoefs(1);
//            }
//            else if (type == OSC3_VOLUME)
//            {
//                oscVolume = value;
//            }
//        }
//            break;
//
//        case 3:
//        {
//            if (type == OSC4_MORPH)
//            {
//                setMorph(value);
//            }
//            else if (type == OSC4_PHASE)
//            {
//                setPhase(value);
//            }
//            else if (type == OSC4_SEMITONE)
//            {
//                setSemiToneValue(value);
//            }
//            else if (type == OSC4_FINETUNE)
//            {
//                setDetuneValue(value);
//            }
//            else if (type == OSC4_RETRIGGER)
//            {
//
//            }
//            else if (type == OSC4_WAVE)
//            {
//                int ref = (int)value;
//                // reinitialiseTable(ref);
//            }
//            else if (type == OSC4_LPFILTER)
//            {
//                filterLPFrequency = value;
//                setFilterCoefs(0);
//            }
//            else if (type == OSC4_HPFILTER)
//            {
//                filterHPFrequency = value;
//                setFilterCoefs(1);
//            }
//            else if (type == OSC4_VOLUME)
//            {
//                oscVolume = value;
//            }
//        }
//            break;
//
//        default:
//            break;
//    }
}



