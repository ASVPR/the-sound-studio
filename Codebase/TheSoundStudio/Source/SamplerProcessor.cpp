/*
  ==============================================================================

    SamplerProcessor.cpp
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#include "SamplerProcessor.h"

// ==================================================================
// Sampler Sound
#pragma mark SamplerSound
// ==================================================================

SynthSamplerSound::SynthSamplerSound (const String& name, AudioFormatReader& source, const BigInteger& midiNotes, int midiNoteForNormalPitch, float centralPitch, Range<float> range, double maxSampleLengthSeconds)
: name (name), midiNotes (midiNotes), midiRootNote (midiNoteForNormalPitch), noteFrequency(centralPitch), frequencyRange(range)
{
    sourceSampleRate = source.sampleRate;

    if (sourceSampleRate <= 0 || source.lengthInSamples <= 0)
    {
        length          = 0;
        attackSamples   = 0;
        releaseSamples  = 0;
    }
    else
    {
        length = jmin ((int) source.lengthInSamples, (int) (maxSampleLengthSeconds * sourceSampleRate));

        data = std::make_unique<AudioBuffer<float>> (jmin (2, (int) source.numChannels), length + 4);

        source.read (data.get(), 0, length + 4, 0, true, true);

        attackSamples   = 10;
        releaseSamples  = roundToInt (7.0 * sourceSampleRate);
    }
}
//SynthSamplerSound::SynthSamplerSound (const String& name,
//                   AudioFormatReader& source,
//                   const BigInteger& midiNotes,
//                   int midiNoteForNormalPitch,
//                   double attackTimeSecs,
//                   double releaseTimeSecs,
//                   double maxSampleLengthSeconds)
//: name (name),
//midiNotes (midiNotes),
//midiRootNote (midiNoteForNormalPitch)
//{
//    sourceSampleRate = source.sampleRate;
//
//    if (sourceSampleRate <= 0 || source.lengthInSamples <= 0)
//    {
//        length          = 0;
//        attackSamples   = 0;
//        releaseSamples  = 0;
//    }
//    else
//    {
//        length = jmin ((int) source.lengthInSamples, (int) (maxSampleLengthSeconds * sourceSampleRate));
//
//        data = std::make_unique<AudioBuffer<float>> (jmin (2, (int) source.numChannels), length + 4);
//
//        source.read (data.get(), 0, length + 4, 0, true, true);
//
//        attackSamples   = roundToInt (attackTimeSecs * sourceSampleRate);
//        releaseSamples  = roundToInt (releaseTimeSecs * sourceSampleRate);
//    }
//}

SynthSamplerSound::~SynthSamplerSound()
{
    
}

const String& SynthSamplerSound::getName() const noexcept
{
    return name;
}

AudioBuffer<float>* SynthSamplerSound::getAudioData() const noexcept
{
    return data.get();

}
//==============================================================================
bool SynthSamplerSound::appliesToNote (int midiNoteNumber)
{
    // should prob hack it here and run checks on frequency.
    return true;
}

bool SynthSamplerSound::appliesToFrequency(float frequency)
{
    if (frequencyRange.contains(frequency))
    {
        return true;
    }
    else
        return false;
}

bool SynthSamplerSound::appliesToChannel (int midiChannel)
{
    return true;
}


// ==================================================================
// Sampler Voice
#pragma mark SamplerVoice
// ==================================================================

SamplerProcessorVoice::SamplerProcessorVoice(FrequencyManager * fm, int ref, int shortcut) : pitchRatio (0.0),
sourceSamplePosition (0.0),
lgain (0.0f), rgain (0.0f),
attackReleaseLevel (0), attackDelta (0), releaseDelta (0),
isInAttack (false), isInRelease (false)
{
    shortcutRef                 = shortcut;
    voiceRef                    = ref;
    frequencyManager            = fm;
    envelope                    = new ADSR2(44100);
    oscFrequency                = 432;
    sampleRate                  = 44100;

    envelope->reset();
}

SamplerProcessorVoice::~SamplerProcessorVoice()
{
    
}

void SamplerProcessorVoice::setCurrentPlaybackSampleRate (double newRate)
{
    SynthesiserVoice::setCurrentPlaybackSampleRate(newRate);
    
    envelope->setNewSamplerate(newRate);
    
}

//==============================================================================
bool SamplerProcessorVoice::canPlaySound (SynthesiserSound*sound)
{
    return dynamic_cast<const SynthSamplerSound*> (sound) != nullptr;
}


//void SamplerProcessorVoice::calculatePitchRatio(int midiNoteNumber, const SynthSamplerSound*  sound)
//{
//    // work works fine until here :
//
//    double base = frequencyManager->getBaseAFrequency() / 440;
//    double shiftRate = 1.0;
//    double rootNoteFrequency;
//
//    if (shortcutRef == -1) /* normal */  {
//        rootNoteFrequency = frequencyManager->scalesManager->getFrequencyForMIDINote(sound->midiRootNote);
//    }
//    else if (shortcutRef == 11) /* lissajous */ {
//        rootNoteFrequency = frequencyManager->scalesManager->getFrequencyForMIDINoteLissajous(sound->midiRootNote);
//    }
//    else /* shortcut */ {
//        rootNoteFrequency = frequencyManager->scalesManager->getFrequencyForMIDINoteShortcut(sound->midiRootNote, shortcutRef);
//    }
//
//
//    if (manipulateChoseFrequency)
//    {
//        if (!multiplyOrDivision)
//        {
//            shiftRate = (base * ((currentFrequency / rootNoteFrequency) * pow(2.0, octaveShift))) * multiplyValue;
//        }
//        else
//        {
//            shiftRate = (base * ((currentFrequency / rootNoteFrequency) * pow(2.0, octaveShift))) / divisionValue;
//        }
//    }
//    else
//    {
//        shiftRate = base * (currentFrequency / rootNoteFrequency) * pow(2.0, octaveShift);
//    }
//
//    pitchRatio = shiftRate * sound->sourceSampleRate / getSampleRate();
//}

void SamplerProcessorVoice::calculatePitchRatio(int midiNoteNumber, const SynthSamplerSound*  sound)
{
    double shiftRate            = 1.0;
    double rootNoteFrequency    = sound->noteFrequency;
    
    shiftRate = (currentFrequency / rootNoteFrequency);
    
    pitchRatio = shiftRate * sound->sourceSampleRate / getSampleRate();
}

//void SamplerProcessorVoice::startNote (int midiNoteNumber, float velocity, SynthesiserSound*s, int pitchWheel)
//{
//    if (const SynthSamplerSound* const sound = dynamic_cast<const SynthSamplerSound*> (s))
//    {
//        calculatePitchRatio(midiNoteNumber, sound);
//
//        sourceSamplePosition = 0.0;
//        lgain = velocity;
//        rgain = velocity;
//
//        envelope->gate(1);
//    }
//    else
//    {
//        jassertfalse; // this object can only play SamplerSounds!
//    }
//}

void SamplerProcessorVoice::startNote (int midiNoteNumber, float velocity, SynthesiserSound*s, int pitchWheel)
{
    if (const SynthSamplerSound* const sound = dynamic_cast<const SynthSamplerSound*> (s))
    {
        calculatePitchRatio(midiNoteNumber, sound);
        
        float volScale = 3.f * 4.f;
        
        sourceSamplePosition = 0.0;
        lgain = velocity * volScale;
        rgain = velocity * volScale;
        
        envelope->gate(1);
    }
    else
    {
        jassertfalse; // this object can only play SamplerSounds!
    }
}

void SamplerProcessorVoice::stopNote (float velocity, bool allowTailOff)
{
    envelope->gate(0);
}

void SamplerProcessorVoice::pitchWheelMoved (int newValue) {}
void SamplerProcessorVoice::controllerMoved (int controllerNumber, int newValue){}


void SamplerProcessorVoice::renderNextBlock (AudioBuffer<float>&outputBuffer, int startSample, int numSamples)
{
    if (const SynthSamplerSound* const playingSound = static_cast<SynthSamplerSound*> (getCurrentlyPlayingSound().get()))
    {
        const float* const inL = playingSound->data->getReadPointer (0);
        const float* const inR = playingSound->data->getNumChannels() > 1
        ? playingSound->data->getReadPointer (1) : nullptr;
        
        float* outL = outputBuffer.getWritePointer (0, startSample);
        float* outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getWritePointer (1, startSample) : nullptr;
        
        while (--numSamples >= 0)
        {
            // printf("numsamples = %i", numSamples);
            
            int pos         = 0;
            float alpha     = 0;
            float invAlpha  = 0;
            float l         = 0;
            float r         = 0;
            
            
            pos = (int) sourceSamplePosition;
            alpha = (float) (sourceSamplePosition - pos);
            invAlpha = 1.0f - alpha;
            l = (inL [pos] * invAlpha + inL [pos + 1] * alpha);
            r = (inR != nullptr) ? (inR [pos] * invAlpha + inR [pos + 1] * alpha): l;
            
            l *= lgain;
            r *= rgain;
            
//            float envVal = envelope->getNextSample();
            l *= amplitude;
            r *= amplitude;
            
            process_adsr(&l, &r, 1);
            
            
            
            if (outR != nullptr)
            {
                *outL++ += l;
                *outR++ += r;
            }
            else
            {
                *outL++ += (l + r) * 0.5f;
            }
            
            sourceSamplePosition += pitchRatio;
            
//            if (!envelope->isActive()) {
//                this->clearCurrentNote();
//            }
            
            if (sourceSamplePosition >= (playingSound->length / pitchRatio))
            {
                stopNote (0.0f, false);
                envelope->reset();
                
                clearCurrentNote();
                break;
            }
        }
    }
}

void SamplerProcessorVoice::process_adsr(float *bufferLeft, float * bufferRight, int buffersize)
{
    for (int i = 0; i < buffersize; i++)
    {
        if(envelope->getState() != ADSR2::env_idle)
        {
            float val = envelope->process();
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

void SamplerProcessorVoice::setParameter(int index, var newValue)
{
    switch (index)
    {
        case ENV_AMPLITUDE:
        {
            amplitude = (float)newValue * 0.01;
        }
            break;
        case ENV_ATTACK:
        {
            float val = newValue.operator float();
            envelope->SetAttack(val);
        }
            break;
        case ENV_SUSTAIN:
        {
            envelope->SetSustain((float)newValue * 0.01);
        }
            break;
        case ENV_DECAY:
        {
            float val = newValue.operator float();
            envelope->SetDecay(val);
        }
            break;
        case ENV_RELEASE:
        {
            float val = newValue.operator float();
            envelope->SetRelease(val);
        }
            break;
            
        default: break;
    }
}


// ==================================================================
// Sampler Processor
#pragma mark SamplerProcessor
// ==================================================================

SamplerProcessor::SamplerProcessor(FrequencyManager *fm, SynthesisLibraryManager * slm, double initialSampleRate, int shortcut)
{
    shortcutRef             = shortcut;
    frequencyManager        = fm;
    sampleLibraryManager    = slm;
    
    // initialize other stuff (not related to buses)
    formatManager.registerBasicFormats();
    
    for (int i = 0; i < maxNumberOfVoices; i++)
    {
        this->addVoice (new SamplerProcessorVoice(frequencyManager, i, shortcutRef));
    }
    
    this->setNoteStealingEnabled(true);
    
//    loadNewSamplerInstrument(69);
}

SamplerProcessor::~SamplerProcessor(){}

void SamplerProcessor::processBlock(AudioBuffer<float>& buffer,
                  MidiBuffer& midiMessages)
{
    renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

//===============================================================
// in order for new chord system to work, we need to override this method pushing frequencies direct to the oscillator
//---------------------------------------------------------------
void SamplerProcessor::noteOn (const int midiChannel, const int midiNoteNumber, const float frequencyDirect)
{
    const ScopedLock sl (lock);

    for (auto* sound : sounds)
    {
        SynthSamplerSound * s = (SynthSamplerSound*)sound;
        
        if (s->appliesToFrequency(frequencyDirect))
        {
            // If hitting a note that's still ringing, stop it first (it could be
            // still playing because of the sustain or sostenuto pedal).
            for (auto* voice : voices)
                if (voice->getCurrentlyPlayingNote() == midiNoteNumber && voice->isPlayingChannel (midiChannel))
                    stopVoice (voice, 1.0f, true);

            SamplerProcessorVoice * v = (SamplerProcessorVoice*)findFreeVoice (sound, midiChannel, midiNoteNumber, false);
            
            if (v != nullptr)
            {
                v->setFrequencyDirect(frequencyDirect);
                startVoice (v, sound, midiChannel, midiNoteNumber, 1.0);
            }
            
        }
    }
}

//void SamplerProcessor::noteOn (const int midiChannel, const int midiNoteNumber, const float frequencyDirect)
//{
//    const ScopedLock sl (lock);
//
//    for (auto* sound : sounds)
//    {
//        if (sound->appliesToNote (midiNoteNumber) && sound->appliesToChannel (midiChannel))
//        {
//            // If hitting a note that's still ringing, stop it first (it could be
//            // still playing because of the sustain or sostenuto pedal).
//            for (auto* voice : voices)
//                if (voice->getCurrentlyPlayingNote() == midiNoteNumber && voice->isPlayingChannel (midiChannel))
//                    stopVoice (voice, 1.0f, true);
//
//            SamplerProcessorVoice * v = (SamplerProcessorVoice*)findFreeVoice (sound, midiChannel, midiNoteNumber, false);
//
//            if (v != nullptr)
//            {
//                v->setFrequencyDirect(frequencyDirect);
//                startVoice (v, sound, midiChannel, midiNoteNumber, 1.0);
//            }
//
//        }
//    }
//}


void SamplerProcessor::setParameter(int index, var newValue)
{
    if (index == INSTRUMENT_TYPE)
    {
//        loadSampleIntoPlayer(newValue.operator int());
        loadInstrument(newValue.operator int());
    }
    else
    {
        for (int v = 0; v < getNumVoices(); v++)
        {
            SamplerProcessorVoice* voice = (SamplerProcessorVoice*)getVoice(v);
            voice->setParameter(index, newValue);
        }
    }
}

void SamplerProcessor::loadInstrument(int filePathIndex) // FIXED: Now completely bypassed
{
    // IMPORTANT: This method is now bypassed because SAMPLER calls are redirected
    // to use WavetableSynthProcessor in the main processors (ChordPlayerProcessor, etc.)
    
    // The SamplerProcessor is no longer used for playing instruments.
    // Instead, when waveformType == SAMPLER, the system automatically uses
    // wavetableSynth->processBlock() which generates synthesis-based audio.
    
    // This method still exists for compatibility, but does nothing.
    // All "playing instrument" audio generation now happens through synthesis.
    
    // Clear any existing sounds to be safe (though they won't be used)
    clearSounds();
}
//void SamplerProcessor::loadNewSamplerInstrument(int midiNote)
//{
//    // should always be 69 @ 440hz..
//    
//    double attackTimeSecs           = 0.005;
//    double releaseTimeSecs          = 7.0;
//    double maxSampleLengthSeconds   = 7.0;
//    
//    BigInteger midiNotes; midiNotes.setRange (0, 127, true);
//
//    std::unique_ptr<AudioFormatReader> formatReader (formatManager.findFormatForFileExtension ("wav")->createReaderFor (new MemoryInputStream(BinaryData::ClassicGrandPiano_wav, BinaryData::ClassicGrandPiano_wavSize, false), true));
//
//    midiNotes.setBit(midiNote);
//    SynthSamplerSound::Ptr newSound = new SynthSamplerSound ("Voice", *formatReader, midiNotes, midiNote, attackTimeSecs, releaseTimeSecs, maxSampleLengthSeconds);
//
//    this->addSound (newSound);
//}
//
//void SamplerProcessor::loadSampleIntoPlayerNew (String filePath)
//{
//    int midiNote                    = 60;
//    double attackTimeSecs           = 0.005;
//    double maxSampleLengthSeconds   = 7.0;
//    
//    BigInteger midiNotes;
//    
//    File samplerFile(filePath);
//    
//    if (samplerFile.existsAsFile())
//    {
//        std::unique_ptr<AudioFormatReader> formatReader (formatManager.findFormatForFileExtension ("wav")->createReaderFor (new FileInputStream(samplerFile), true));
//        
//        midiNotes.setBit(midiNote);
//        
//        maxSampleLengthSeconds = (double)formatReader->lengthInSamples / formatReader->sampleRate;
//        
//        SynthSamplerSound::Ptr newSound = new SynthSamplerSound ("Voice", *formatReader, midiNotes, midiNote, attackTimeSecs, maxSampleLengthSeconds, maxSampleLengthSeconds);
//        
//        this->addSound (newSound);
//    }
//}
//
//void SamplerProcessor::loadSampleIntoPlayer (int filePathIndex)
//{
//    int midiNote                    = 60;
//    double attackTimeSecs           = 0.005;
//    double maxSampleLengthSeconds   = 7.0;
//    
//    BigInteger midiNotes;
//    
//    File samplerFile(sampleLibraryManager->getInstrumentFilePath(filePathIndex));
//    
//    if (samplerFile.existsAsFile())
//    {
//        std::unique_ptr<AudioFormatReader> formatReader (formatManager.findFormatForFileExtension ("wav")->createReaderFor (new FileInputStream(samplerFile), true));
//        
//        midiNotes.setBit(midiNote);
//        
//        maxSampleLengthSeconds = (double)formatReader->lengthInSamples / formatReader->sampleRate;
//        
//        SynthSamplerSound::Ptr newSound = new SynthSamplerSound ("Voice", *formatReader, midiNotes, midiNote, attackTimeSecs, maxSampleLengthSeconds, maxSampleLengthSeconds);
//        
//        this->addSound (newSound);
//    }
//}

void SamplerProcessor::removeCurrentInstrument()
{
    for (int i = 0; i < this->getNumSounds(); i++)
    {
        this->removeSound(i);
    }
}

void SamplerProcessor::setCentralFrequencyToSounds()
{
    // In order to preserve the highest quality of playback
    // we need the samples to be played that are closest to the origina frequency
    // thus reducing the distrotion of the pitch shift
    // to do this each sound must know which MIDInote it is best to correspond to
    // this can only be done by implemented an algorithm that checks in the appliesToNote(midinote) function
    // each sound also need to know the central pitch A in order to decide this
    
    for (int i = 0; i < getNumSounds(); i++)
    {
        SynthSamplerSound * sound = (SynthSamplerSound*)getSound(i).get();
        
        sound->midiRootNote = 0;
        
    }
}


//void SamplerProcessor::noteOn (const int midiChannel, const int midiNoteNumber, const float velocity)
//{
//    const ScopedLock sl (lock);
//
//    float midiNoteFrequency = frequencyManager->getFrequencyForMIDINote(midiNoteNumber);
//
//    // next need to check for sound with closest frequency
//
//    // once found sound need to inform voice of difference in pitch required (sound.frequency - midiNote.frequency)
//
//    // set to voice before calling startVoice
//
//    for (auto* sound : sounds)
//    {
//        if ( sound->appliesToNote(midiNoteNumber))
//        {
//            // If hitting a note that's still ringing, stop it first (it could be
//            // still playing because of the sustain or sostenuto pedal).
//            for (auto* voice : voices)
//                if (voice->getCurrentlyPlayingNote() == midiNoteNumber && voice->isPlayingChannel (midiChannel))
//                    stopVoice (voice, 1.0f, true);
//
//            startVoice (findFreeVoice (sound, midiChannel, midiNoteNumber, true),
//                        sound, midiChannel, midiNoteNumber, velocity);
//        }
//    }
//
//    //        for (auto* sound : sounds)
//    //        {
//    //            if (sound->appliesToNote (midiNoteNumber) && sound->appliesToChannel (midiChannel))
//    //            {
//    //                // If hitting a note that's still ringing, stop it first (it could be
//    //                // still playing because of the sustain or sostenuto pedal).
//    //                for (auto* voice : voices)
//    //                    if (voice->getCurrentlyPlayingNote() == midiNoteNumber && voice->isPlayingChannel (midiChannel))
//    //                        stopVoice (voice, 1.0f, true);
//    //
//    //                startVoice (findFreeVoice (sound, midiChannel, midiNoteNumber, true),
//    //                                sound, midiChannel, midiNoteNumber, velocity);
//    //            }
//    //        }
//}
