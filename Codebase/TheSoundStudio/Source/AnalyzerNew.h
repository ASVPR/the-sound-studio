/*
  ==============================================================================

    AnalyzerNew.h
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#pragma once

#include  <JuceHeader.h>

template<typename Type>
class AnalyserNew : public juce::Thread
{
public:
    AnalyserNew() : Thread ("FFT Analyser New")
    {
        averager.clear();

        initMovingAverage();

        initGradient();
    }

    AnalyserNew(int instance) : Thread (String::formatted("FFT Analyser%i", instance))
    {
        averager.clear();

        initMovingAverage();

        initGradient();
    }

    virtual ~AnalyserNew() = default;

    void initFFT()
    {
        forwardFFT1024      = std::make_unique<dsp::FFT>(10);
        forwardFFT2048      = std::make_unique<dsp::FFT>(11);
        forwardFFT4096      = std::make_unique<dsp::FFT>(12);
        forwardFFT8192      = std::make_unique<dsp::FFT>(13);
        forwardFFT16384     = std::make_unique<dsp::FFT>(14);
        forwardFFT32768     = std::make_unique<dsp::FFT>(15);
        forwardFFT65536     = std::make_unique<dsp::FFT>(16);

        fftSize             = forwardFFT1024->getSize();
        fft                 = forwardFFT1024.get();

        averager            .setSize (5, forwardFFT65536->getSize() / 2, false, false, true);
        fftBuffer           .setSize (1, forwardFFT65536->getSize(), false, false, true );

        shouldProcess       = true;
    }

    void addAudioData (const AudioBuffer<Type>& buffer, int startChannel, int numChannels)
    {
        if (shouldProcess)
        {
            if (abstractFifo.getFreeSpace() < buffer.getNumSamples())
                return;

            int start1, block1, start2, block2;
            abstractFifo.prepareToWrite (buffer.getNumSamples(), start1, block1, start2, block2);
            audioFifo.copyFrom (0, start1, buffer.getReadPointer (startChannel), block1);
            if (block2 > 0)
                audioFifo.copyFrom (0, start2, buffer.getReadPointer (startChannel, block1), block2);

            for (int channel = startChannel + 1; channel < startChannel + numChannels; ++channel)
            {
                if (block1 > 0) audioFifo.addFrom (0, start1, buffer.getReadPointer (channel), block1);
                if (block2 > 0) audioFifo.addFrom (0, start2, buffer.getReadPointer (channel, block1), block2);
            }
            abstractFifo.finishedWrite (block1 + block2);
            waitForData.signal();
        }
    }

    void setupAnalyser (int audioFifoSize, Type sampleRateToUse)
    {
        sampleRate = sampleRateToUse;
        audioFifo.setSize (1, audioFifoSize);
        abstractFifo.setTotalSize (audioFifoSize);

        startThread (juce::Thread::Priority::normal);
    }

    void run() override
    {
        while (! threadShouldExit())
        {
            if (shouldProcess)
            {
                if (abstractFifo.getNumReady() >= fft->getSize())
                {
                    fftBuffer.clear();

                    int start1, block1, start2, block2;
                    abstractFifo.prepareToRead (fft->getSize(), start1, block1, start2, block2);
                    if (block1 > 0) fftBuffer.copyFrom (0, 0, audioFifo.getReadPointer (0, start1), block1);
                    if (block2 > 0) fftBuffer.copyFrom (0, block1, audioFifo.getReadPointer (0, start2), block2);
                    abstractFifo.finishedRead ((block1 + block2) / 2);

                    windowing->multiplyWithWindowingTable (fftBuffer.getWritePointer (0), size_t (fft->getSize()));

                    fft->performFrequencyOnlyForwardTransform (fftBuffer.getWritePointer (0));

                    ScopedLock lockedForWriting (pathCreationLock);
                    averager.addFrom (0, 0, averager.getReadPointer (averagerPtr), averager.getNumSamples(), -1.0f);
                    averager.copyFrom (averagerPtr, 0, fftBuffer.getReadPointer (0), averager.getNumSamples(), 1.0f / (averager.getNumSamples() * (averager.getNumChannels() - 1)));
                    averager.addFrom (0, 0, averager.getReadPointer (averagerPtr), averager.getNumSamples());
                    if (++averagerPtr == averager.getNumChannels()) averagerPtr = 1;

                    // here to calculate plots etc

                    newDataAvailable = true;
                }

                if (abstractFifo.getNumReady() < fft->getSize())
                    waitForData.wait (100);
            }
        }
    }

    void createPath (Path& p, const Rectangle<float> bounds, float minFreq, float maxFreq) // half sample rate
    {
        p.clear();


        //        ScopedLock lockedForReading (pathCreationLock);
        const auto* fftData = averager.getReadPointer (0);
        float octaves =  (log2(maxFreq) - log2(minFreq));
        const auto  factor  = bounds.getWidth() / octaves;
        p.startNewSubPath (bounds.getX() + factor * indexToX (0, minFreq), binToY (fftData [0], bounds));

        // Peak freq vars
        float highest   = 0.f;
        int highestBin  = 0;

        for (int i = 0; i < averager.getNumSamples(); ++i)
        {
            float x = bounds.getX() + factor * indexToX (i, minFreq);
            p.lineTo (x, binToY (fftData [i], bounds));

            // peak freq
            if (shouldProcessMovingAvg)
            {
                if (fftData[i] > highest)
                {
                    highest         = fftData[i];
                    highestBin      = i;
                }
            }
        }

        peakFreq    = (sampleRate * highestBin) / fftSize;

        peakDB      = 20.0 * log10(sqrt(fftData[highestBin])); // * might not need the sqrt **

        calculateMovingAverage();

        p.lineTo(bounds.getWidth(), bounds.getHeight());
        p.lineTo(0, bounds.getHeight());
        p.closeSubPath();

    }



    void createPathOptimised (Path& p, const Rectangle<float> bounds, float minFreq, float maxFreq) // half sample rate
    {
        p.clear();
        //        p.preallocateSpace (8 + averager.getNumSamples() * 3);

        //        ScopedLock lockedForReading (pathCreationLock);
        const auto* fftData = averager.getReadPointer (0);
        float octaves =  (log2(maxFreq) - log2(minFreq));
        const auto  factor  = bounds.getWidth() / octaves;
        p.startNewSubPath (bounds.getX() + factor * indexToX (0, minFreq), binToY (fftData [0], bounds));

        // Peak freq vars
        float highest   = 0.f;
        int highestBin  = 0;

        // only for actual pixels

        for (int x = 0; x < bounds.getWidth(); x++)
        {
            int index = xToBinIndex(x, bounds.getWidth(), minFreq, maxFreq, sampleRate, fftSize);

            // need lin interpolation here for a smooth plot....

            p.lineTo (x, binToY (fftData [index], bounds));
        }

        for (int i = 0; i < averager.getNumSamples(); ++i)
        {
            //            float x = bounds.getX() + factor * indexToX (i, minFreq);
            //            p.lineTo (x, binToY (fftData [i], bounds));

            // peak freq
            if (shouldProcessMovingAvg)
            {
                if (fftData[i] > highest)
                {
                    highest         = fftData[i];
                    highestBin      = i;
                }
            }
        }

        peakFreq    = (sampleRate * highestBin) / fftSize;
        peakDB      = 20.0 * log10(sqrt(fftData[highestBin])); // * might not need the sqrt **

        calculateMovingAverage();

        p.lineTo(bounds.getWidth(), bounds.getHeight());
        p.lineTo(0, bounds.getHeight());
        p.closeSubPath();

    }

    void createPathOptimisedWithRange (Path& p, const Rectangle<float> bounds, float minFreq, float maxFreq, float minDB, float maxDB)
    {
        p.clear();
        //        p.preallocateSpace (8 + averager.getNumSamples() * 3);

        //        ScopedLock lockedForReading (pathCreationLock);
        const auto* fftData = averager.getReadPointer (0);

        float octaves =  (log2(maxFreq) - log2(minFreq)); // could be permanent, recalulated on resize..
        const auto  factor  = bounds.getWidth() / octaves; // as above

        p.startNewSubPath (bounds.getX() + factor * indexToX (0, minFreq), binToYWithRange (fftData [0], bounds, minDB, maxDB));

        // Peak freq vars
        float highest   = 0.f;
        int highestBin  = 0;

        // only for actual pixels

        for (int x = 0; x < bounds.getWidth(); x++)
        {
            int index = xToBinIndex(x, bounds.getWidth(), minFreq, maxFreq, sampleRate, fftSize);

            p.lineTo (x, binToYWithRange (fftData [index], bounds, minDB, maxDB));
        }

        for (int i = 0; i < averager.getNumSamples(); ++i)
        {
            if (shouldProcessMovingAvg)
            {
                if (fftData[i] > highest)
                {
                    highest         = fftData[i];
                    highestBin      = i;
                }
            }
        }

        peakFreq    = (sampleRate * highestBin) / fftSize;
        peakDB      = 20.0 * log10(sqrt(fftData[highestBin])); // * might not need the sqrt **

        calculateMovingAverage();

        p.lineTo(bounds.getWidth(), bounds.getHeight());
        p.lineTo(0, bounds.getHeight());
        p.closeSubPath();

    }

    inline float dLinTerp(float x1, float x2, float y1, float y2, float x)
    {
        float denom = x2 - x1;
        if(denom == 0)
            return y1; // should not ever happen

        // calculate decimal position of x
        float dx = (x - x1)/(x2 - x1);

        // use weighted sum method of interpolating
        float result = dx*y2 + (1-dx)*y1;

        return result;

    }

    void getPeakFrequency(float & freq, float & db)
    {
        ScopedLock lockedForReading (pathCreationLock);
        const auto* fftData = averager.getReadPointer (0);

        float highest   = 0.f;
        int highestBin  = 0;

        for (int i = 0; i < averager.getNumSamples(); ++i)
        {
            if (fftData[i] > highest)
            {
                highest         = fftData[i];
                highestBin      = i;
            }
        }

        freq    = (sampleRate * highestBin) / fftSize;
        db      = 20.0 * log10(sqrt(fftData[highestBin])); // * might not need the sqrt **
    }

    void getHarmonics(Array<float>& frequencies, Array<float>& db, int numHarmonicsToFind, bool upper)
    {
        if (numHarmonicsToFind <= 0) return;

        const auto* fftData = averager.getReadPointer(0);
        int numSamples = averager.getNumSamples();

        std::vector<float> highest(numHarmonicsToFind, 0.0f);
        std::vector<int> highestBin(numHarmonicsToFind, 0);

        for (int harmonic = 0; harmonic < numHarmonicsToFind; harmonic++)
        {
            for (int i = 0; i < numSamples; ++i)
            {
                if (upper)
                {
                    bool alreadyFound = false;
                    for (int h = 0; h < harmonic; h++)
                    {
                        if (i == highestBin[h])
                        {
                            alreadyFound = true;
                            break;
                        }
                    }

                    if (!alreadyFound && fftData[i] > highest[harmonic])
                    {
                        highest[harmonic] = fftData[i];
                        highestBin[harmonic] = i;
                    }
                }
            }

            if (harmonic < frequencies.size())
                frequencies.set(harmonic, (sampleRate * (float)highestBin[harmonic]) / (float)fftSize);
            else
                frequencies.add((sampleRate * (float)highestBin[harmonic]) / (float)fftSize);

            float dbVal = (fftData[highestBin[harmonic]] > 0) ? 20.0f * std::log10(std::sqrt(fftData[highestBin[harmonic]])) : -100.0f;
            if (harmonic < db.size())
                db.set(harmonic, dbVal);
            else
                db.add(dbVal);
        }
    }

    // Octave Visualiser
    //===================================
    Array<float>    centralFrequencies;
    Array<float>    lowerFrequencies;
    Array<float>    upperFrequencies;
    Array<int>      binBandStarts; // bins start on this index.. e.g binBandStarts[0] = minFreq

    int calculateBands(float minFreq, float maxFreq, float sampleRate, int fftSize)
    {
        centralFrequencies.clear();
        lowerFrequencies.clear();
        upperFrequencies.clear();
        binBandStarts.clear();

        if (minFreq <= 0 || maxFreq <= minFreq || sampleRate <= 0 || fftSize <= 0)
            return 0;

        int   num           = 0;
        float currentFreq   = minFreq;

        // find count bands
        while (currentFreq < maxFreq)
        {
            currentFreq *= powf(10.f, 0.1f);
            if (currentFreq > maxFreq) break;
            num++;
        }

        if (num <= 0) return 0;

        // then calculate lower, central and uppfreqs
        currentFreq = minFreq;
        for (int i = 0; i < num; i++)
        {
            lowerFrequencies.add(currentFreq);
            currentFreq *= powf(10.f, 0.05f);
            centralFrequencies.add(currentFreq);
            currentFreq *= powf(10.f, 0.05f);
            upperFrequencies.add(currentFreq);
            binBandStarts.add(0);

            if (currentFreq > maxFreq)
                break;
        }

        // Adjust num if loop broke early
        num = lowerFrequencies.size();

        // calculate bands
        octaveFFTSize = fftSize;
        int halfFFT = octaveFFTSize / 2;
        if (halfFFT <= 0) return 0;

        std::vector<float> indexFreq(halfFFT);
        for (int index = 0; index < halfFFT; index++)
        {
            indexFreq[index] = (sampleRate * (float)index) / (float)octaveFFTSize;
        }

        // calculate binBandStarts.getReference(0)
        if (binBandStarts.size() > 0)
        {
            for (int index = 0; index < halfFFT; index++)
            {
                if (indexFreq[index] >= minFreq)
                {
                    binBandStarts.set(0, index);
                    break;
                }
            }
        }

        // set last band limit
        binBandStarts.add(halfFFT); 

        for (int b = 0; b < num && (b + 1) < binBandStarts.size(); b++)
        {
            for (int index = binBandStarts[b]; index < halfFFT; index++)
            {
                if (indexFreq[index] > upperFrequencies[b])
                {
                    binBandStarts.set(b + 1, index);
                    break;
                }
            }
            // Ensure next start is at least current start
            if (binBandStarts[b+1] < binBandStarts[b])
                binBandStarts.set(b+1, binBandStarts[b]);
        }

        return num;
    }

    int octaveNumBands = 32;
    float octaveMinFreq = 0;
    float octaveMaxFreq = 0;
    int octaveFFTSize   = 0;
    bool isFirstRun = true;
    int startBin = 0; // set as minFreqBin

    void getMagnitudeDataForOctave(Array<float>& magnitude, int& numBands, float minFreq, float maxFreq, float sr, Array<float>& centralFreqs)
    {
        magnitude.clear();
        
        if (averager.getNumChannels() == 0)
        {
            numBands = 0;
            return;
        }

        const auto* fftData = averager.getReadPointer(0);

        if (minFreq != octaveMinFreq || maxFreq != octaveMaxFreq || sr != sampleRate || octaveFFTSize != fftSize || isFirstRun)
        {
            isFirstRun      = false;
            octaveFFTSize   = fftSize;
            sampleRate      = sr;
            octaveMinFreq   = minFreq;
            octaveMaxFreq   = maxFreq;
            octaveNumBands  = calculateBands(octaveMinFreq, octaveMaxFreq, sr, fftSize);
        }

        centralFreqs = centralFrequencies;
        numBands = octaveNumBands;

        if (numBands <= 0) return;

        std::vector<float> bandValues(numBands, 0.0f);
        int bandRef = 0;

        int firstBin = (binBandStarts.size() > 0) ? binBandStarts[0] : 0;
        int maxBin = averager.getNumSamples();

        for (int bin = firstBin; bin < maxBin; ++bin)
        {
            while (bandRef < numBands && (bandRef + 1) < binBandStarts.size() && bin >= binBandStarts[bandRef + 1])
            {
                bandRef++;
            }

            if (bandRef < numBands)
            {
                bandValues[bandRef] += fftData[bin];
            }
            else
            {
                break;
            }
        }

        for (int i = 0; i < numBands; i++)
        {
            if (std::isnan(bandValues[i]) || std::isinf(bandValues[i]))
            {
                bandValues[i] = 0.f;
            }

            int start = (i < binBandStarts.size()) ? binBandStarts[i] : 0;
            int end = ((i + 1) < binBandStarts.size()) ? binBandStarts[i + 1] : start;
            int numBinsToDivide = jmax(1, end - start);

            bandValues[i] /= (float)numBinsToDivide;
            magnitude.add(bandValues[i]);
        }
    }

    ColourGradient gradientDecibels;

    void initGradient()
    {
        gradientDecibels.addColour(0.f, Colours::black);
        gradientDecibels.addColour(0.2f, Colours::purple);
        gradientDecibels.addColour(0.4f, Colours::red);
        gradientDecibels.addColour(0.6f, Colours::yellow);
        gradientDecibels.addColour(0.8f, Colours::white);
    }

    void createColourSpectrum(Image & imageToRenderTo, float minFreq, float maxFreq, float logScale)
    {
        ScopedLock lockedForReading (pathCreationLock);
        if (averager.getNumChannels() == 0 || fft == nullptr || sampleRate <= 0)
            return;

        if (imageToRenderTo.getWidth() <= 0 || imageToRenderTo.getHeight() <= 0)
            return;

        const auto* fftData = averager.getReadPointer (0);

        auto rightHandEdge = imageToRenderTo.getWidth() - 1;
        auto imageHeight   = imageToRenderTo.getHeight();

        imageToRenderTo.moveImageSection (0, 0, 1, 0, rightHandEdge, imageHeight);

        for (auto y = 0; y < imageHeight; ++y)
        {
            float y2                = (float)imageHeight - y;

            auto fftDataIndex       = xToBinIndex(y2, (float)imageHeight, minFreq, maxFreq, (float)sampleRate, fftSize);

            if (fftDataIndex >= 0 && fftDataIndex < averager.getNumSamples())
            {
                float infinity          = -80.f;
                auto level              = jmap (Decibels::gainToDecibels (fftData[fftDataIndex], infinity), infinity, 0.0f, 0.f, 1.f);
                imageToRenderTo.setPixelAt (rightHandEdge, y, gradientDecibels.getColourAtPosition(level));
            }
        }
    }

    void getFrequencyData(double& peakFrequency, double& peakDB, Array<float>& harmonics, Array<float>& intervals, double& ema)
    {
        const auto* fftData = averager.getReadPointer(0);
        int numSamples = averager.getNumSamples();

        if (numSamples <= 0) return;

        // Peak Frequency
        float highest = 0.f;
        int highestBin = 0;
        for (int i = 0; i < numSamples; ++i)
        {
            if (fftData[i] > highest)
            {
                highest = fftData[i];
                highestBin = i;
            }
        }

        std::vector<float> harmonicFreq(6, 0.0f);
        std::vector<float> peakDBBin(6, -100.0f);
        std::vector<float> interval(6, 0.0f);
        std::vector<int> bin(6, 0);

        bin[0] = highestBin;
        harmonicFreq[0] = (sampleRate * (float)bin[0]) / (float)fftSize;
        peakDBBin[0] = (fftData[bin[0]] > 0) ? 20.0f * std::log10(std::sqrt(fftData[bin[0]])) : -100.0f;

        for (int harmonic = 1; harmonic < 6; harmonic++)
        {
            highest = 0.f;
            int lastBin = bin[harmonic - 1];
            float lastFreq = (sampleRate * (float)lastBin) / (float)fftSize;
            float nextFreq = lastFreq * powf(2.f, 0.25f);
            int nextBin = (int)((nextFreq / sampleRate) * (float)fftSize);

            if (nextBin >= numSamples) break;

            for (int i = nextBin; i < numSamples; ++i)
            {
                if (fftData[i] > highest)
                {
                    highest = fftData[i];
                    bin[harmonic] = i;
                }
            }

            harmonicFreq[harmonic] = (sampleRate * (float)bin[harmonic]) / (float)fftSize;
            peakDBBin[harmonic] = (fftData[bin[harmonic]] > 0) ? 20.0f * std::log10(std::sqrt(fftData[bin[harmonic]])) : -100.0f;

            if (harmonic < harmonics.size())
                harmonics.set(harmonic, harmonicFreq[harmonic]);
            else
                harmonics.add(harmonicFreq[harmonic]);

            interval[harmonic] = harmonicFreq[harmonic] - harmonicFreq[harmonic - 1];
            if (harmonic < intervals.size())
                intervals.set(harmonic, interval[harmonic]);
            else
                intervals.add(interval[harmonic]);
        }

        peakFrequency = harmonicFreq[0];
        peakDB = peakDBBin[0];

        calculateMovingAverage();
        ema = movingAvgFreq;
    }

    bool checkForNewData()
    {
        auto available = newDataAvailable.load();
        newDataAvailable.store (false);
        return available;
    }

    void setNewFFTWindowFunction(int windowEnum)
    {
        if (fft == nullptr) return;

        shouldProcess   = false;

        windowingEnum   = windowEnum;

        windowMethod    = (dsp::WindowingFunction<float>::WindowingMethod) windowingEnum ;

        windowing       = std::make_unique<dsp::WindowingFunction<float>> (fft->getSize(),  windowMethod, true, 4 );

        shouldProcess   = true;
    }

    void setNewFFTSize(int fftEnum)
    {
        shouldProcess = false;

        averager.clear();
        fftBuffer.clear();

        abstractFifo.reset();

        if      (fftEnum == 1 || fftEnum == 0) { fftSize = 1024; fft = forwardFFT1024.get(); }
        else if (fftEnum == 2) { fftSize = 2048; fft = forwardFFT2048.get(); }
        else if (fftEnum == 3) { fftSize = 4096; fft = forwardFFT4096.get(); }
        else if (fftEnum == 4) { fftSize = 8192; fft = forwardFFT8192.get(); }
        else if (fftEnum == 5) { fftSize = 16384; fft = forwardFFT16384.get(); }
        else if (fftEnum == 6) { fftSize = 32768; fft = forwardFFT32768.get(); }
        else if (fftEnum == 7) { fftSize = 65536; fft = forwardFFT65536.get(); }

        if (fft != nullptr)
        {
            fftBuffer.setSize(1, fftSize, false, false, true); 
            averager.setSize(5, fftSize / 2, false, false, true);
            audioFifo.setSize (1, fftSize + 1);
            abstractFifo.setTotalSize (fftSize + 1);

            windowMethod    = (dsp::WindowingFunction<float>::WindowingMethod) windowingEnum ;
            windowing       = std::make_unique<dsp::WindowingFunction<float>> (fft->getSize(),  windowMethod, true, 4 );
        }

        shouldProcess   = true;
    }

private:

    inline float indexToX (float index, float minFreq) const
    {
        const auto freq = (sampleRate * index) / fft->getSize();
        return (freq > 0.01f) ? std::log (freq / minFreq) / std::log (2.0f) : 0.0f;
    }

    inline int xToBinIndex (float x, float w, float minFreq, float maxFreq, float sr, int fftSize) const
    {
        float xScale                = x / w;
        float freq                  = powf(10.f, xScale * (log10f(maxFreq)-log10f(minFreq)) + log10f(minFreq));
        int binIndex                = (int)freq / sampleRate  * fft->getSize();

        return binIndex;
    }

    inline float binToY (float bin, const Rectangle<float> bounds) const
    {
        const float maxDB = 12.f;
        const float infinity = -80.0f;
        return jmap (Decibels::gainToDecibels (bin, infinity),
                     infinity, maxDB, bounds.getBottom(), bounds.getY());
    }

    inline float binToYWithRange (float bin, const Rectangle<float> bounds, float minDB, float maxDB) const
    {
        return jmap (Decibels::gainToDecibels (bin, minDB),
                     minDB, maxDB, bounds.getBottom(), bounds.getY());
    }

    WaitableEvent waitForData;
    CriticalSection pathCreationLock;

    Type sampleRate {};

    int fftSize = 0;
    int windowingEnum = 0;

    // mine
    dsp::FFT * fft { nullptr };
    std::unique_ptr<dsp::FFT> forwardFFT1024;
    std::unique_ptr<dsp::FFT> forwardFFT2048;
    std::unique_ptr<dsp::FFT> forwardFFT4096;
    std::unique_ptr<dsp::FFT> forwardFFT8192;
    std::unique_ptr<dsp::FFT> forwardFFT16384;
    std::unique_ptr<dsp::FFT> forwardFFT32768;
    std::unique_ptr<dsp::FFT> forwardFFT65536;

    std::unique_ptr<dsp::WindowingFunction<float>> windowing;
    dsp::WindowingFunction<float>::WindowingMethod  windowMethod;
    AudioBuffer<float> fftBuffer;
    AudioBuffer<float> averager;
    int averagerPtr = 1;
    AbstractFifo abstractFifo              { 96000 };
    AudioBuffer<Type> audioFifo;

    std::atomic<bool> newDataAvailable { false };

    bool shouldProcess = false;

#define NUM_AVG 30

    bool shouldProcessMovingAvg     = true;
    double peakFreq                  = 0.f;
    double peakDB                    = 0.f;
    double movingAvgFreq             = 0.f;
    double movingAvgFreqs[NUM_AVG];
    int movingAVGIterator           = 0;
    bool isFull                     = false;


    void initMovingAverage()
    {
        movingAVGIterator   = 0;
        isFull              = false;
        movingAvgFreq       = 0.f;

        for (int i = 0; i < NUM_AVG; i++)
        {
            movingAvgFreqs[i] = 0.f;
        }
    }

    void calculateMovingAverage()
    {
        movingAvgFreqs[movingAVGIterator] = peakFreq;

        float total = 0.f;

        if (isFull)
        {
            for (int i = 0; i < NUM_AVG; i++)
            {
                total += movingAvgFreqs[i];
            }

            movingAvgFreq = total / NUM_AVG;
        }
        else
        {
            for (int i = 0; i < movingAVGIterator; i++) {
                total += movingAvgFreqs[i];
            }

            //            movingAvgFreq = total / NUM_AVG;

            movingAvgFreq = total / (movingAVGIterator + 1);
        }

        // iterate
        movingAVGIterator++;

        if (movingAVGIterator >= NUM_AVG)
        {
            if (!isFull) isFull = true;

            movingAVGIterator = 0;
        }
    }
public:
    void getMovingAveragePeakData(double & _peakFreq, double & _peakDB, double & _movingAvgFreq)
    {
        _peakFreq       = peakFreq;
        _peakDB         = peakDB;
        _movingAvgFreq  = movingAvgFreq;
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnalyserNew)
};
