/*
  ==============================================================================

    Analyzer.cpp
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#include <numeric>
#include <thread>
#include <chrono>
#include <vector>
#include "Analyzer.h"



Analyser::Analyser(FrequencyManager& freqManager) :
juce::Thread ("FFT Analyser"),
frequencyManager(freqManager)
{
    init_fft();
}

Analyser::Analyser (FrequencyManager& freqManager, int instance) :
juce::Thread (String::formatted("FFT Analyser%i", instance)),
frequencyManager(freqManager)
{
    init_fft();
}

void Analyser::init_fft()
{
    averager.setSize (1, get_maximum_fft_size() / 2, false, false, true);

    fftBuffer.setSize (1, get_maximum_fft_size(), false, false, true );

    iterationMedianData.reserve(20);
}

void Analyser::addAudioData (const AudioBuffer<float>& buffer, int startChannel, int numChannels)
{
    if (shouldProcess && isPlaying.load() && enable_fft.load())
    {
        const auto numSamples = buffer.getNumSamples();

        auto addToFifoBuffer = [&] (juce::AudioBuffer<float>& destionationBuffer, juce::AbstractFifo& fifo)
        {
            auto fifoWrite = fifo.write(numSamples);

            for (auto channel = startChannel; channel < startChannel + numChannels; ++channel)
            {
                if (fifoWrite.blockSize1 > 0)
                {
                    destionationBuffer.copyFrom (channel,
                                    fifoWrite.startIndex1,
                                    buffer.getReadPointer (channel),
                                    fifoWrite.blockSize1);
                }

                if (fifoWrite.blockSize2 > 0)
                {
                    destionationBuffer.copyFrom (0,
                                    fifoWrite.startIndex2,
                                    buffer.getReadPointer (channel, fifoWrite.blockSize1),
                                    fifoWrite.blockSize2);
                }
            }
        };

        if (algorithm !=  Algorithm::TrackHarmonics)
        {
            if (harmonicsAbstractFifo.getFreeSpace() >= numSamples)
            {
                addToFifoBuffer(secondAudioFifo, harmonicsAbstractFifo);
            }
        }

        if (abstractFifo.getFreeSpace() >= numSamples)
        {
            addToFifoBuffer(audioFifo, abstractFifo);
        }

        
    }
}

void Analyser::setupAnalyser (float sampleRateToUse, int maxNumOfChannels)
{
    sampleRate = sampleRateToUse;

    clearAlgorithm();
}

void Analyser::run()
{

    if (!enable_fft.load())
    {
        return;
    }

    while (! threadShouldExit())
    {
        if (shouldProcess && isPlaying.load())
        {
            isProcessing.store(true);

            auto write_to_buffer = [this] (juce::AbstractFifo& fifo,
                                           juce::AudioBuffer<float>& inputBuffer,
                                           juce::AudioBuffer<float>& buffer,
                                           int numSamples)
            {
                auto scopedFifoRead = fifo.read(numSamples);

                if (scopedFifoRead.blockSize1 > 0)
                {
                    buffer.copyFrom (0,
                                     0,
                                     inputBuffer.getReadPointer (0, scopedFifoRead.startIndex1),
                                     scopedFifoRead.blockSize1);
                }

                if (scopedFifoRead.blockSize2 > 0)
                {
                    buffer.copyFrom (0,
                                     scopedFifoRead.blockSize1,
                                     inputBuffer.getReadPointer (0, scopedFifoRead.startIndex2),
                                     scopedFifoRead.blockSize2);
                }
            };

            auto size = (algorithm ==  Algorithm::TrackHarmonics) ? fftSize : fftSize / 2;

            auto pitch {0.f};

            if (abstractFifo.getNumReady() < size)
            {
                waitForData.wait(50);
            }
            else
            {
                fftBuffer.clear();

                shouldWaitforHarmonics.store(false);

                if (algorithm ==  Algorithm::TrackHarmonics)
                {

                    write_to_buffer(abstractFifo, audioFifo, fftBuffer, size);

                    pitch = calculateHarmonics();
                }
                else
                {
                    if (iterations == 0)
                    {
                        shouldWaitforHarmonics.store(true);
                    }

                    pitchDetectorBuffer.clear();

                    write_to_buffer(abstractFifo, audioFifo, pitchDetectorBuffer, size);

                    if (harmonicsAbstractFifo.getNumReady() == fftSize)
                    {
                        write_to_buffer(harmonicsAbstractFifo, secondAudioFifo, fftBuffer, size);

                        calculateHarmonics();
                    }


                    if (algorithm == Algorithm::MPM)
                    {
                        pitch = pitchPMP->getPitch(pitchDetectorBuffer.getReadPointer(0));

                        if (pitch < 0)
                        {
                            pitch = 0;
                        }

                    }
                    else if (algorithm == Algorithm::Yin)
                    {
                        pitch = pitchYIN->getPitchInHz(pitchDetectorBuffer.getReadPointer(0));
                    }

                    if (iterationMedianData.size() > 10)
                    {
                        iterationMedianData.clear();
                    }

                    iterationMedianData.emplace_back(pitch);

                    pitch = calculate_emm(iterationMedianData, pitch);
                }

                if (iterations == 0)
                {
                    currentFrequency.store(pitch, std::memory_order_release);

                    harmonics.set_tonic_frequency(pitch);
                    shouldWaitforHarmonics.store(false);

                }
                else
                {
                    if (iterationLength < numberOfSamplesRead + size)
                    {
                        numberOfSamplesRead = size;
                        iterationMedianData.clear();
                        iterationsData.push_back(pitch);
                        ++currentIteration;
                    }
                    else
                    {
                        numberOfSamplesRead += size;
                }

                    if (currentIteration >= iterations)
                    {
                        pitch = calculate_emm(iterationsData, pitch);
                        currentIteration = 0;

                        // THREAD SAFETY FIX: Previous code used std::thread(...).detach() which created
                        // orphaned threads that could cause crashes on shutdown. Now calling directly
                        // within the existing thread context for safe, synchronous execution.
                        stopProcess();

                        isProcessing.store(false);

                        harmonics.set_tonic_frequency(pitch);

                        currentFrequency.store(pitch, std::memory_order_release);

                        return;
                    }
                }
            }
        }


        isProcessing.store(false);
    }
}

void Analyser::clearAlgorithm()
{
    // CRITICAL PERFORMANCE FIX: Wait for processing to complete before clearing
    // Previous implementation had a spinning loop that consumed 100% CPU
    // Now using std::this_thread::sleep_for for efficient waiting with minimal CPU usage
    while (isProcessing.load())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    averager.clear();

    fftBuffer.clear();

    abstractFifo.reset();

    harmonicsAbstractFifo.reset();

    initHarmonics();

    currentFrequency = 0.f;

    iterationsData.clear();

    iterationsData.reserve(iterations);

    currentIteration =  0;

    numberOfSamplesRead = 0;
    
    iterationMedianData.clear();

    initHarmonics();

    pitchPMP = std::make_unique<adamski::PitchMPM>(sampleRate, fftSize / 2);

    pitchYIN = std::make_unique<adamski::PitchYIN>(sampleRate, fftSize / 2);

    averager.setSize(2, fftSize / 2, false, false, true);

    if (algorithm == Algorithm::TrackHarmonics)
    {
        abstractFifo.setTotalSize (fftSize + 1);

        audioFifo.setSize(1, fftSize + 1);
    }
    else
    {
        harmonicsAbstractFifo.setTotalSize (fftSize + 1);

        secondAudioFifo.setSize(1, fftSize + 1);

        abstractFifo.setTotalSize ((fftSize / 2) + 1);

        audioFifo.setSize(1, (fftSize / 2) + 1);
    }

    pitchDetectorBuffer.setSize(1, (fftSize / 2) + 1);

    fftBuffer.setSize(1, fftSize, false, false, true);

    windowing = std::make_unique<dsp::WindowingFunction<float>>(fft->getSize(),  windowMethod, true, 4 );
}

const float * Analyser::getFFTReadPointer()
{
    return averager.getReadPointer (0);
}

int Analyser::getFFTSize()
{
    return fftSize;
}


void Analyser::setNewFFTWindowFunction(int windowEnum)
{
    shouldProcess   = false;
    
    windowMethod = static_cast<dsp::WindowingFunction<float>::WindowingMethod>(windowEnum);

    clearAlgorithm();
    
    shouldProcess   = true;
}

void Analyser::setNewFFTSize(fft_size newSize)
{
    shouldProcess = false;

    auto size = static_cast<float>(static_cast<int>(newSize));

    fft = std::make_unique<dsp::FFT>(std::log2(size));
    
    fftSize = fft->getSize();

    if (iterationLength < fftSize)
    {
       [[maybe_unused]] auto a =  set_iteration_length(get_samples_as_time(fftSize));

        listeners.call([] (Listener& l) { l.fft_updated(); });
    }

    clearAlgorithm();
    
    shouldProcess   = true;
}

void Analyser::setNumHarmonicsToTrack(int num)
{
    numHarmonicsToTrack = num;
}

void Analyser::setAlgorithm(Algorithm newAlgorithm)
{
    shouldProcess = false;

    algorithm = newAlgorithm;

    clearAlgorithm();

    shouldProcess = true;
}

void Analyser::setCustomRange(bool should)
{
    customRange = should;
}

void Analyser::setCustomRangeMinFrequency(double f)
{
    customRangeMinFrequency = f;
}

void Analyser::setCustomRangeMaxFrequency(double f)
{
    customRangeMaxFrequency = f;
}

void Analyser::setInputThreshold(double db)
{
    inputThreshold = db;
}

void Analyser::setMinInterval(double min)
{
    minIntervalInOctaves = min;
}

void Analyser::setMaxInterval(double max)
{
    maxIntervalInOctaves = max;
}

void Analyser::initHarmonics()
{
    harmonics.initialize();
}

float Analyser::calculateHarmonics()
{
    windowing->multiplyWithWindowingTable (fftBuffer.getWritePointer (0), size_t (fft->getSize()));

    fft->performFrequencyOnlyForwardTransform (fftBuffer.getWritePointer (0));

    ScopedLock lockedForWriting (pathCreationLock);

    averager.addFrom (0,
                      0,
                      averager.getReadPointer (averagerPtr),
                      averager.getNumSamples(),
                      -1.0f);

    averager.copyFrom (averagerPtr,
                       0,
                       fftBuffer.getReadPointer (0),
                       averager.getNumSamples(),
                       1.0f / (averager.getNumSamples() * (averager.getNumChannels() - 1)));

    averager.addFrom (0, 0, averager.getReadPointer (averagerPtr), averager.getNumSamples());

    if (++averagerPtr == averager.getNumChannels())
    {
        averagerPtr = 1;
    }
    const auto* fftData = averager.getReadPointer (0);
    
    // Peak Frequency
    float highest   = 0.f;
    int highestBin  = 0;
    
    // calc fft bin scan range from min/mx frequencies...
    
    auto binRangeMin = 0;
    auto binRangeMax = (sampleRate / 2) / sampleRate * fft->getSize();
    
    if (customRange)
    {
        binRangeMin = static_cast<int>(customRangeMinFrequency / sampleRate * fft->getSize());
        binRangeMax = static_cast<int>(customRangeMaxFrequency / sampleRate * fft->getSize());
    }

    for (int i = binRangeMin; i < binRangeMax; ++i)
    {
        if (fftData[i] > highest)
        {
            highest = fftData[i];
            highestBin = i;
        }
    }

    auto numHarmonics = numHarmonicsToTrack.load();
    
    // harmonics - replaced VLAs with vectors for safety
    std::vector<float> harmonicFreq(numHarmonics);
    std::vector<float> peakDBBin(numHarmonics);
    std::vector<float> interval(numHarmonics);
    std::vector<int> bin(numHarmonics);
    
    for (int i = 0; i < numHarmonics; i++)
    {
        harmonicFreq[i]     = 0;
        peakDBBin[i]        = 0;
        interval[i]         = 0;
        bin[i]              = 0;
    }
    
    bin[0] = highestBin;

    // is highest over input threshold ?
    double binDB = 20.0 * log10(fftData[bin[0]]);

    auto& harmonicData = harmonics.get_harmonics_array();

    auto& tonicHarmonic = harmonics.getHarmonic(0);

    float pitch { 0.f };

    if (binDB > inputThreshold)
    {
        tonicHarmonic.isActive    = true;
        tonicHarmonic.binRef      = bin[0];

        harmonicFreq[0]             = (sampleRate * bin[0]) / fftSize;
        tonicHarmonic.freq        = harmonicFreq[0];

        peakDBBin[0]                = 20.0 * log10(fftData[bin[0]]);
        tonicHarmonic.db          = peakDBBin[0];
    }
    else
    {
        tonicHarmonic.isActive = false;
    }

    if (tonicHarmonic.isActive)
    {
        pitch = tonicHarmonic.freq;

        for (int harmonic = 1; harmonic < harmonics.get_harmonics_array().size(); harmonic++)
        {
            if (harmonic >= numHarmonics)
            {
                harmonicData[harmonic].isActive = false;
            }
            else
            {
                highest = 0.f;

                int lastBin          = bin[harmonic - 1];
                float lastFreq       = (sampleRate * lastBin) / fftSize;
                float minFreq        = lastFreq * powf(2.f, minIntervalInOctaves);
                float maxFreq        = lastFreq * powf(2.f, maxIntervalInOctaves);
                
                // Safe calculation to prevent integer overflow
                double minBinCalc = (double)minFreq / sampleRate * fft->getSize();
                double maxBinCalc = (double)maxFreq / sampleRate * fft->getSize();
                
                // Clamp to safe integer range
                int minIntervalBin = juce::jmax(0, juce::jmin((int)minBinCalc, fft->getSize() - 1));
                int maxIntervalBin = juce::jmax(0, juce::jmin((int)maxBinCalc, fft->getSize() - 1));
                bool didFindHarmonic = false;

                for (int i = minIntervalBin; i < maxIntervalBin && i < binRangeMax; ++i)
                {
                    if (fftData[i] > highest)
                    {
                        highest             = fftData[i];
                        bin[harmonic]       = i;
                    }
                }

                if (highest != 0)
                {
                    double db = 20.0 * log10(fftData[bin[harmonic]]);

                    if (db > inputThreshold)
                    {
                        didFindHarmonic                     = true;

                        harmonicData[harmonic].isActive     = true;
                        harmonicData[harmonic].binRef       = bin[harmonic];
                        harmonicData[harmonic].freq         = (sampleRate * bin[harmonic]) / fftSize;
                        harmonicData[harmonic].db           = 20.0 * log10(fftData[bin[harmonic]]);
                        int midiNote    = -1;
                        float freqDif   = 0.f;
                        int octave      = 0;
                        int keynote     = 0;

                        frequencyManager.getMIDINoteForFrequency(harmonicData[harmonic].freq , midiNote, keynote, octave, freqDif);

                        harmonicData[harmonic].nearestMidiNote = midiNote;

                        // calculate interval from last to this
                        if (harmonic != 0)
                        {
                            harmonicData[harmonic].interval = harmonicData[harmonic].freq - harmonicData[harmonic - 1].freq;
                        }
                    }
                    else
                    {
                        harmonicData[harmonic].isActive    = false;
                    }
                }
                else
                {
                    harmonicData[harmonic].isActive    = false;
                }
            }
        }
    }
    else
    {
        for (auto& h : harmonicData)
        {
            h.isActive = false;
            h.freq = 0;
        }

        if (algorithm == Algorithm::TrackHarmonics)
        {
            return 0.f;
        }
    }

    return pitch;
}


std::optional<float> Analyser::set_iteration_length(float length_in_ms)
{
    shouldProcess = false;

    auto minLength = get_minimum_iteration_length();

    std::optional<float> result {std::nullopt};

    if (length_in_ms < minLength)
    {
        length_in_ms = minLength;

        result = length_in_ms;
    }

    iterationLength = get_time_as_samples(length_in_ms);

    stopProcess();

    clearAlgorithm();

    shouldProcess = true;

    return result;
}

float Analyser::get_samples_as_time(int numSamples) const
{
    return static_cast<float>(numSamples) * 1000.f / static_cast<float>(sampleRate);
}

int Analyser::get_time_as_samples(float time) const
{
    return static_cast<int>(time * static_cast<float>(sampleRate) / 1000.f);
}

float Analyser::get_minimum_iteration_length() const
{
    return get_samples_as_time(static_cast<float>(fftSize));
}
