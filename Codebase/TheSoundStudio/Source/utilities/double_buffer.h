/*
  ==============================================================================

    double_buffer.h
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

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
