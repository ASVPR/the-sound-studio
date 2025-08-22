/*
  ==============================================================================

    double_buffer.h
    Created: 2 Feb 2022 12:03:37pm
    Author:  Javier Andoni Aizpuru Rodríguez

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace audio_devs
{

template<typename FloatType>
class double_buffer
{
public:

    void set_size(int numChannels, int numSamples)
    {
        write_audio_fifo.setSize(numChannels, numSamples);
        read_audio_fifo.setSize(numChannels, numSamples);
    }

    void add_samples(const juce::AudioBuffer<float>& buffer)
    {
        const auto numSamples = buffer.getNumSamples();

        auto fifoWrite = abstract_fifo->write(numSamples);

        for (auto channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            if (fifoWrite.blockSize1 > 0)
            {
                write_audio_fifo.copyFrom(channel,
                                          fifoWrite.startIndex1,
                                          buffer.getReadPointer (channel),
                                          fifoWrite.blockSize1);
            }

            if (abstract_fifo->getFreeSpace() <= numSamples)
            {
                read_audio_fifo.copyFrom(channel,
                                         0,
                                         write_audio_fifo.getReadPointer (channel),
                                         write_audio_fifo.getNumSamples());

                dataReady.store(true);
//                dataReady.notify_one();
            }

            if (fifoWrite.blockSize2 > 0)
            {

            }
        }
    }

    void read_samples(const juce::AudioBuffer<float>& buffer)
    {
        dataReady.wait(false);
    }

private:

    std::atomic<bool> dataReady { false };

    juce::AudioBuffer<float> write_audio_fifo;

    juce::AudioBuffer<float> read_audio_fifo;

    std::unique_ptr<juce::AbstractFifo> abstract_fifo;
};


}
