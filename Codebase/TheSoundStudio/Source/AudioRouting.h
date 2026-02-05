/*
  ==============================================================================

    AudioRouting.h
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Parameters.h"

namespace TSS { namespace Audio {

inline void routeToOutput(juce::AudioBuffer<float>& buffer,
                          const juce::AudioBuffer<float>& outputBuffer,
                          AUDIO_OUTPUTS output)
{
    const int numSamples  = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    switch (output)
    {
        case AUDIO_OUTPUTS::MONO_1:
            buffer.addFrom(0, 0, outputBuffer, 0, 0, numSamples);
            break;

        case AUDIO_OUTPUTS::MONO_2:
            if (numChannels > 1)
                buffer.addFrom(1, 0, outputBuffer, 0, 0, numSamples);
            break;

        case AUDIO_OUTPUTS::MONO_3:
            if (numChannels > 2)
                buffer.addFrom(2, 0, outputBuffer, 0, 0, numSamples);
            break;

        case AUDIO_OUTPUTS::MONO_4:
            if (numChannels > 3)
                buffer.addFrom(3, 0, outputBuffer, 0, 0, numSamples);
            break;

        case AUDIO_OUTPUTS::MONO_5:
            if (numChannels > 4)
                buffer.addFrom(4, 0, outputBuffer, 0, 0, numSamples);
            break;

        case AUDIO_OUTPUTS::MONO_6:
            if (numChannels > 5)
                buffer.addFrom(5, 0, outputBuffer, 0, 0, numSamples);
            break;

        case AUDIO_OUTPUTS::MONO_7:
            if (numChannels > 6)
                buffer.addFrom(6, 0, outputBuffer, 0, 0, numSamples);
            break;

        case AUDIO_OUTPUTS::MONO_8:
            if (numChannels > 7)
                buffer.addFrom(7, 0, outputBuffer, 0, 0, numSamples);
            break;

        case AUDIO_OUTPUTS::STEREO_1_2:
            if (numChannels > 1)
            {
                buffer.addFrom(0, 0, outputBuffer, 0, 0, numSamples);
                buffer.addFrom(1, 0, outputBuffer, 0, 0, numSamples);
            }
            break;

        case AUDIO_OUTPUTS::STEREO_3_4:
            if (numChannels > 2)
            {
                buffer.addFrom(2, 0, outputBuffer, 0, 0, numSamples);
                buffer.addFrom(3, 0, outputBuffer, 0, 0, numSamples);
            }
            break;

        case AUDIO_OUTPUTS::STEREO_5_6:
            if (numChannels > 5)
            {
                buffer.addFrom(4, 0, outputBuffer, 0, 0, numSamples);
                buffer.addFrom(5, 0, outputBuffer, 0, 0, numSamples);
            }
            break;

        case AUDIO_OUTPUTS::STEREO_7_8:
            if (numChannels > 7)
            {
                buffer.addFrom(6, 0, outputBuffer, 0, 0, numSamples);
                buffer.addFrom(7, 0, outputBuffer, 0, 0, numSamples);
            }
            break;

        default:
            break;
    }
}

}} // namespace TSS::Audio
