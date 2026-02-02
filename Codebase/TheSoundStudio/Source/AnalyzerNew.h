/*
  ==============================================================================

    AnalyzerNew.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

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
        forwardFFT1024      = new dsp::FFT(10);
        forwardFFT2048      = new dsp::FFT(11);
        forwardFFT4096      = new dsp::FFT(12);
        forwardFFT8192      = new dsp::FFT(13);
        forwardFFT16384     = new dsp::FFT(14);
        forwardFFT32768     = new dsp::FFT(15);
        forwardFFT65536     = new dsp::FFT(16);

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

    void getHarmonics(Array<float> & frequencies, Array<float> &db, int numHarmonicsToFind, bool upper)
    {
        // thpght it would be easy, but alas, maybe not
        // attaining the higherst 5 peaks wont be sufficient
        // because, the 5 highest bins, might be in the same harmonic..
        // need to scan the fft and check the position of the peaks
        // if they are within a x bins of the highest peak bin, they hsould be ignored..
        // find


        //        ScopedLock lockedForReading (pathCreationLock);
        const auto* fftData = averager.getReadPointer (0);

        float highest[numHarmonicsToFind];
        int highestBin[numHarmonicsToFind];

        for (int i = 0; i < numHarmonicsToFind; i++) { highest[i] = 0; highestBin[i] = 0; }

        for (int harmonic = 0; harmonic < numHarmonicsToFind; harmonic++)
        {
            for (int i = 0; i < averager.getNumSamples(); ++i)
            {
                if (upper)
                {
                    if (fftData[i] > highest[harmonic] && i != highestBin[harmonic] )
                    {
                        highest[harmonic]       = fftData[i];
                        highestBin[harmonic]    = i;
                    }
                }
            }

            frequencies.getReference(harmonic)  = (sampleRate * highestBin[harmonic]) / fftSize;
            db.getReference(harmonic)           = 20.0 * log10(fftData[highestBin[harmonic]]);
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
        binBandStarts.clear();

        int   num           = 0;
        float currentFreq   = minFreq;

        // find count bands
        while (currentFreq < maxFreq)
        {
            currentFreq *= powf(10.f, 0.1);

            if (currentFreq > maxFreq) { break; }
            else { num++; }

        }

        // then calculate lower, central and uppfreqs
        currentFreq = minFreq;

        for (int i = 0; i < num; i++)
        {
            lowerFrequencies.add(currentFreq);
            centralFrequencies.add(currentFreq *=powf(10.f, 0.05));
            upperFrequencies.add(currentFreq *=powf(10.f, 0.05));
            binBandStarts.add(0);

            //            printf("\n Lower Freq Band %i : %f", i, lowerFrequencies.getReference(i));
            //            printf("\n Centr Freq Band %i : %f", i, centralFrequencies.getReference(i));
            //            printf("\n Upper Freq Band %i : %f", i, upperFrequencies.getReference(i));


            if (currentFreq > maxFreq)
            {
                break;
            }

        }



        // calculate bands
        octaveFFTSize   = fftSize;

        float indexFreq[octaveFFTSize/2];
        for (int index = 0; index < (octaveFFTSize/2); index++)
        {
            indexFreq[index] = (sampleRate * index) / octaveFFTSize;
        }


        // calculate binBandStarts.getReference(0)
        for (int index = 0; index < (octaveFFTSize/2); index++)
        {
            if (indexFreq[index] >= minFreq)
            {
                binBandStarts.getReference(0) = index;

                //                printf("\n bin 0 : freq %f", indexFreq[index]);
                break;
            }
        }

        // set last band limit
        binBandStarts.add(octaveFFTSize/2); // add one extra to the end which is @ bin half fftsize

        for (int b = 0; b < num-1; b++)
        {
            // get range of band and check indexFeeq if they are within the range
            for (int index = binBandStarts.getReference(b); index < (octaveFFTSize/2); index++)
            {
                if (indexFreq[index] > upperFrequencies.getReference(b))
                {
                    //                    printf("\n bin %i : index %i : freq %f", b+1, index, indexFreq[index]);

                    binBandStarts.getReference(b+1) = index;

                    break;
                }
            }
        }

        return num;
    }

    int octaveNumBands = 32;
    float octaveMinFreq = 0;
    float octaveMaxFreq = 0;
    int octaveFFTSize   = 0;
    bool isFirstRun = true;
    int startBin = 0; // set as minFreqBin

    void getMagnitudeDataForOctave(Array<float> & magnitude, int & numBands, float  minFreq, float maxFreq, float sr, Array<float> & centralFreqs)
    {
        //        ScopedLock lockedForReading (pathCreationLock);
        const auto* fftData = averager.getReadPointer (0);


        // if min or max freq has changed, recalculate the number of octave bands
        if (minFreq != octaveMinFreq || maxFreq != octaveMaxFreq || sr != sampleRate || octaveFFTSize != fftSize || isFirstRun)
        {
            isFirstRun      = false;
            octaveFFTSize   = fftSize;
            sampleRate      = sr;
            octaveMinFreq   = minFreq;
            octaveMaxFreq   = maxFreq;
            octaveNumBands  = calculateBands(octaveMinFreq, octaveMaxFreq, sr, fftSize);
        }

        centralFreqs    = centralFrequencies;

        numBands = octaveNumBands;

        int bandRef     = 0;

        float bandValues[numBands];

        for (int i = 0; i < numBands; i++) { bandValues[i] = 0.f; }



        bandRef = 1;
        for (int bin = binBandStarts.getReference(0); bin < averager.getNumSamples(); ++bin)
        {
            if (bin < binBandStarts.getReference(bandRef))
            {
                bandValues[bandRef] += fftData[bin];
            }
            else
            {
                if (bandRef < numBands)
                {
                    bandRef++;
                }
                else
                {
                    break;
                }
            }
        }

        // scale accumlated values by num bins
        for (int i = 0; i < numBands; i++)
        {
            // Come back and test this again once we are using higher res fft
            // it will give us more bins and a more precise
            if (std::isnan(bandValues[i]))
            {
                bandValues[i] = 0.f;
            }

            int numBinsToDivide = binBandStarts.getReference(i+1) - binBandStarts.getReference(i);

            bandValues[i] /= numBinsToDivide;

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
        const auto* fftData = averager.getReadPointer (0);

        auto rightHandEdge = imageToRenderTo.getWidth() - 1;
        auto imageHeight   = imageToRenderTo.getHeight();

        imageToRenderTo.moveImageSection (0, 0, 1, 0, rightHandEdge, imageHeight);

        for (auto y = 0; y < imageHeight; ++y)
        {
            float y2                = imageHeight - y;

            auto fftDataIndex       = xToBinIndex(y2, imageHeight, minFreq, maxFreq, sampleRate, fftSize);

            float infinity          = -80.f;

            auto level              = jmap (Decibels::gainToDecibels (fftData[fftDataIndex], infinity), infinity, 0.0f, 0.f, 1.f);

            imageToRenderTo.setPixelAt (rightHandEdge, y, gradientDecibels.getColourAtPosition(level));
        }
    }

    void getFrequencyData(double & peakFrequency, double & peakDB, Array<float> &harmonics, Array<float> &intervals, double & ema)
    {
        //        ScopedLock lockedForReading (pathCreationLock);
        const auto* fftData = averager.getReadPointer (0);

        // Peak Frequency
        float highest   = 0.f;
        int highestBin  = 0;
        //
        for (int i = 0; i < averager.getNumSamples(); ++i)
        {
            if (fftData[i] > highest)
            {
                highest         = fftData[i];
                highestBin      = i;
            }
        }
        //
        // harmonics
        float harmonicFreq[6];
        float peakDBBin[6];
        float interval[6];
        int bin[6];

        for (int i = 0;i < 6; i++) {
            harmonicFreq[i] = 0; peakDBBin[i] = 0; interval[i] = 0; bin[i] = 0;
        }

        bin[0]              = highestBin;
        harmonicFreq[0]     = (sampleRate * bin[0]) / fftSize;
        peakDBBin[0]        = 20.0 * log10(fftData[bin[0]]);

        for (int harmonic = 1; harmonic < 6; harmonic++)
        {
            highest = 0.f;

            // need to define the next bin from the last highest peak + a deteremined interval, 1 semitone (@current scale) etc.. 1/4 octave , 4/12..

            //            int nextBin = bin[harmonic-1]+1;

            int lastBin         = bin[harmonic-1];
            float lastFreq      = (sampleRate * lastBin) / fftSize;
            float nextFreq      = lastFreq * powf(2.f, 0.25);
            int nextBin         = (int)nextFreq / sampleRate  * fft->getSize();

            for (int i = nextBin; i < averager.getNumSamples(); ++i)
            {
                if (fftData[i] > highest)
                {
                    highest         = fftData[i];
                    bin[harmonic]   = i;
                }
            }

            harmonicFreq[harmonic]              = (sampleRate * bin[harmonic]) / fftSize;
            peakDBBin[harmonic]                 = 20.0 * log10(fftData[bin[harmonic]]);

            harmonics.getReference(harmonic)    = harmonicFreq[harmonic];

            // calculate interval from last to this
            if (harmonic != 0)
            {
                interval[harmonic] = harmonicFreq[harmonic] - harmonicFreq[harmonic-1];

                intervals.getReference(harmonic) = interval[harmonic];
            }
        }

        peakFrequency       = harmonicFreq[0];
        peakDB              = peakDBBin[0]; // * might not need the sqrt

        calculateMovingAverage();

        ema                 = movingAvgFreq;
    }

    bool checkForNewData()
    {
        auto available = newDataAvailable.load();
        newDataAvailable.store (false);
        return available;
    }

    void setNewFFTWindowFunction(int windowEnum)
    {
        shouldProcess   = false;

        windowingEnum   = windowEnum;

        windowMethod    = (dsp::WindowingFunction<float>::WindowingMethod) windowingEnum ;

        windowing       = new dsp::WindowingFunction<float> (fft->getSize(),  windowMethod, true, 4 );

        shouldProcess   = true;
    }

    void setNewFFTSize(int fftEnum)
    {
        shouldProcess = false;

        averager.clear();
        fftBuffer.clear();

        abstractFifo.reset();

        if      (fftEnum == 1) { fftSize = 1024; fft = forwardFFT1024; fftBuffer.setSize(1, fftSize, false, false, true); averager.setSize(5, fftSize / 2, false, false, true); }
        else if (fftEnum == 2) { fftSize = 2048; fft = forwardFFT2048; fftBuffer.setSize(1, fftSize, false, false, true); averager.setSize(5, fftSize / 2, false, false, true); }
        else if (fftEnum == 3) { fftSize = 4096; fft = forwardFFT4096; fftBuffer.setSize(1, fftSize, false, false, true); averager.setSize(5, fftSize / 2, false, false, true);  }
        else if (fftEnum == 4) { fftSize = 8192; fft = forwardFFT8192; fftBuffer.setSize(1, fftSize, false, false, true); averager.setSize(5, fftSize / 2, false, false, true);  }
        else if (fftEnum == 5) { fftSize = 16384; fft = forwardFFT16384; fftBuffer.setSize(1, fftSize, false, false, true); averager.setSize(5, fftSize / 2, false, false, true); }
        else if (fftEnum == 6) { fftSize = 32768; fft = forwardFFT32768; fftBuffer.setSize(1, fftSize, false, false, true); averager.setSize(5, fftSize / 2, false, false, true); }
        else if (fftEnum == 7) { fftSize = 65536; fft = forwardFFT65536; fftBuffer.setSize(1, fftSize, false, false, true); averager.setSize(5, fftSize / 2, false, false, true); }

        audioFifo.setSize (1, fftSize + 1);

        abstractFifo.setTotalSize (fftSize + 1);

        windowMethod    = (dsp::WindowingFunction<float>::WindowingMethod) windowingEnum ;

        windowing       = new dsp::WindowingFunction<float> (fft->getSize(),  windowMethod, true, 4 );

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

    int fftSize;
    int windowingEnum = 0;

    // mine
    dsp::FFT * fft;
    dsp::FFT * forwardFFT1024;
    dsp::FFT * forwardFFT2048;
    dsp::FFT * forwardFFT4096;
    dsp::FFT * forwardFFT8192;
    dsp::FFT * forwardFFT16384;
    dsp::FFT * forwardFFT32768;
    dsp::FFT * forwardFFT65536;

    dsp::WindowingFunction<float> * windowing;
    dsp::WindowingFunction<float>::WindowingMethod  windowMethod;
    AudioBuffer<float> fftBuffer;
    AudioBuffer<float> averager;
    int averagerPtr = 1;
    AbstractFifo abstractFifo              { 96000 };
    AudioBuffer<Type> audioFifo;

    std::atomic<bool> newDataAvailable;

    bool shouldProcess = true;

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
    const float* getFFTData() const
    {
        return averager.getReadPointer(0);
    }

    int getFFTDataSize() const
    {
        return averager.getNumSamples();
    }

    void getMovingAveragePeakData(double & _peakFreq, double & _peakDB, double & _movingAvgFreq)
    {
        _peakFreq       = peakFreq;
        _peakDB         = peakDB;
        _movingAvgFreq  = movingAvgFreq;
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnalyserNew)
};
