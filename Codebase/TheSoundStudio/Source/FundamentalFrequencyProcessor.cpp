/*
  ==============================================================================

    FundamentalFrequencyProcessor.cpp

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#include "FundamentalFrequencyProcessor.h"
#include "FundamentalFrequencyComponent.h"
#include "ProjectManager.h"

static const juce::StringArray get_fft_sizes_as_strings()
{
    return juce::StringArray(juce::String( static_cast<int>( Analyser::fft_size::size_10 ) ),
                             juce::String( static_cast<int>( Analyser::fft_size::size_11 ) ),
                             juce::String( static_cast<int>( Analyser::fft_size::size_12 ) ),
                             juce::String( static_cast<int>( Analyser::fft_size::size_13) ),
                             juce::String( static_cast<int>( Analyser::fft_size::size_14) ),
                             juce::String( static_cast<int>( Analyser::fft_size::size_15) ),
                             juce::String( static_cast<int>( Analyser::fft_size::size_16) ),
                             juce::String( static_cast<int>( Analyser::fft_size::size_17) ),
                             juce::String( static_cast<int>( Analyser::fft_size::size_18) ),
                             juce::String( static_cast<int>( Analyser::fft_size::size_19) ),
                             juce::String( static_cast<int>( Analyser::fft_size::size_20) ));
}

FundamentalFrequencyProcessor::FundamentalFrequencyProcessor(ProjectManager& pm, FrequencyManager& fm) :
apvts(*this,
      nullptr,
      "FundamentalFrequencyProcessor",
{
    std::make_unique<juce::AudioParameterChoice>(get_parameter_name(Parameter::fft_size),
                                                 get_parameter_name(Parameter::fft_size),
                                                 get_fft_sizes_as_strings(),
                                                 5),
    std::make_unique<juce::AudioParameterInt>(get_parameter_name(Parameter::fft_window),
                                              get_parameter_name(Parameter::fft_window),
                                              1, 8, 1),
    std::make_unique<juce::AudioParameterFloat>(get_parameter_name(Parameter::iteration_length),
                                              get_parameter_name(Parameter::iteration_length),
                                                0.f, 80000.f, 300.f),

    std::make_unique<juce::AudioParameterInt>(get_parameter_name(Parameter::iterations),
                                                get_parameter_name(Parameter::iterations),
                                                0, 10, 3),

    std::make_unique<juce::AudioParameterInt>(get_parameter_name(Parameter::input_channel),
                                              get_parameter_name(Parameter::input_channel),
                                              0, 15, 0),

    std::make_unique<juce::AudioParameterInt>(get_parameter_name(Parameter::algorithm),
                                              get_parameter_name(Parameter::algorithm),
                                              0, 2, 0),

    std::make_unique<juce::AudioParameterInt>(get_parameter_name(Parameter::harmonics),
                                              get_parameter_name(Parameter::harmonics),
                                              1, 10, 10),

    std::make_unique<juce::AudioParameterFloat>(get_parameter_name(Parameter::threshold),
                                                get_parameter_name(Parameter::threshold),
                                                -80.f, 6.f, -80.f),

    std::make_unique<juce::AudioParameterFloat>(get_parameter_name(Parameter::min_interval),
                                                get_parameter_name(Parameter::min_interval),
                                                0.05f, 4, 0.2f),

    std::make_unique<juce::AudioParameterFloat>(get_parameter_name(Parameter::max_interval),
                                                get_parameter_name(Parameter::max_interval),
                                                0.05f, 4.f, 4.f),

    std::make_unique<juce::AudioParameterInt>(get_parameter_name(Parameter::max_frequency),
                                                get_parameter_name(Parameter::max_frequency),
                                                1, 22000, 22000),

    std::make_unique<juce::AudioParameterInt>(get_parameter_name(Parameter::min_frequency),
                                                get_parameter_name(Parameter::min_frequency),
                                                1, 22000, 1),

    std::make_unique<juce::AudioParameterBool>(get_parameter_name(Parameter::frequency_range),
                                               get_parameter_name(Parameter::frequency_range),
                                               false)
}),
projectManager(pm),
frequencyManager(fm),
analyser(frequencyManager)
{
    set_fft_size(get_fft_size());

    apvts.addParameterListener(get_parameter_name(Parameter::fft_size), this);
    apvts.addParameterListener(get_parameter_name(Parameter::iteration_length), this);
    apvts.addParameterListener(get_parameter_name(Parameter::fft_window), this);
    apvts.addParameterListener(get_parameter_name(Parameter::algorithm), this);
    apvts.addParameterListener(get_parameter_name(Parameter::harmonics), this);
    apvts.addParameterListener(get_parameter_name(Parameter::threshold), this);
    apvts.addParameterListener(get_parameter_name(Parameter::min_interval), this);
    apvts.addParameterListener(get_parameter_name(Parameter::max_interval), this);
    apvts.addParameterListener(get_parameter_name(Parameter::min_frequency), this);
    apvts.addParameterListener(get_parameter_name(Parameter::max_frequency), this);

    // Initialise default algorithm from Project Settings if available
    {
        int defaultAlg = static_cast<int>(projectManager.getProjectSettingsParameter(TSS_SETTINGS::FUNDAMENTAL_FREQUENCY_ALGORITHM));
        if (defaultAlg < 0) defaultAlg = 0;
        if (defaultAlg > 2) defaultAlg = 2;
        apvts.getParameterAsValue(get_parameter_name(Parameter::algorithm)).setValue(defaultAlg);
    }
}

auto * FundamentalFrequencyProcessor::getCurrentEditor()
{
    return dynamic_cast<FundamentalFrequencyComponent*>(getActiveEditor());
}

void FundamentalFrequencyProcessor::prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock) 
{
    auto inputChannels = static_cast<int>(projectManager.deviceManager->getCurrentAudioDevice()->getActiveInputChannels().toInt64());

    analyser.setupAnalyser(static_cast<float>(sampleRate), inputChannels);

    // Prepare IR convolution for current stream format
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = (juce::uint32) maximumExpectedSamplesPerBlock;
    spec.numChannels = 1; // analysis path uses a single selected input channel
    irConvolution.prepare(spec);
    irPrepared = true;
}

void FundamentalFrequencyProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    if (!analyser.is_doing_analysis())
    {
        playing.store(false, std::memory_order_release);
    }
        
    if (playing.load())
    {
        auto inputChannel = apvts.getRawParameterValue(get_parameter_name(Parameter::input_channel))->load();

        if (inputChannel < buffer.getNumChannels())
        {
            // Optionally pass a convolved (IR) version of the selected input channel into the analyser
            if (irEnabled.load(std::memory_order_acquire) && irPrepared)
            {
                // Make a mono copy of selected channel
                juce::AudioBuffer<float> mono(1, buffer.getNumSamples());
                mono.copyFrom(0, 0, buffer, (int) inputChannel, 0, buffer.getNumSamples());

                juce::dsp::AudioBlock<float> blk(mono);
                juce::dsp::ProcessContextReplacing<float> ctx(blk);
                irConvolution.process(ctx);

                // Wet/dry mix for analysis
                const float wet = irWet.load();
                if (wet < 1.0f)
                {
                    // mix dry input into mono
                    mono.applyGain(wet);
                    mono.addFrom(0, 0, buffer, (int) inputChannel, 0, buffer.getNumSamples(), 1.0f - wet);
                }

                analyser.addAudioData(mono, 0, 1);
            }
            else
            {
                analyser.addAudioData(buffer, inputChannel, 1);
            }
        }
    }

    buffer.clear();

}

juce::Result FundamentalFrequencyProcessor::loadIRFromWav(const juce::File& wavFile)
{
    if (! wavFile.existsAsFile())
        return juce::Result::fail("IR file not found: " + wavFile.getFullPathName());

    // JUCE dsp::Convolution handles WAV files via loadImpulseResponse
    try
    {
        irConvolution.loadImpulseResponse(wavFile,
                                          juce::dsp::Convolution::Stereo::no,
                                          juce::dsp::Convolution::Trim::yes,
                                          0);
    }
    catch (const std::exception& e)
    {
        return juce::Result::fail("Failed to load IR: " + juce::String(e.what()));
    }

    return juce::Result::ok();
}

AudioProcessorEditor * FundamentalFrequencyProcessor::createEditor()
{
    //return new FundamentalFrequencyComponent(projectManager, *this);
}

void FundamentalFrequencyProcessor::clearAlgorithm()
{
    analyser.clearAlgorithm();
}

void FundamentalFrequencyProcessor::set_fft_size(Analyser::fft_size newSize)
{
    analyser.setNewFFTSize(newSize);
}

void FundamentalFrequencyProcessor::setNewFFTWindowFunction(int windowEnum)
{
    analyser.setNewFFTWindowFunction(windowEnum);
}

void FundamentalFrequencyProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID == get_parameter_name(Parameter::fft_size))
    {
        set_fft_size(get_fft_size_from_index(static_cast<int>(newValue)));

        if (auto e = getCurrentEditor())
        {
            e->updateIterationUI();
        }
    }

    else if (parameterID == get_parameter_name(Parameter::iteration_length))
    {
        auto updatedValue = analyser.set_iteration_length(newValue);

        if (updatedValue.has_value())
        {
            apvts.getParameterAsValue(get_parameter_name(Parameter::iteration_length)).setValue(updatedValue.value());
        }

        if (auto e = getCurrentEditor())
        {
            e->updateIterationUI();
        }
    }

    else if (parameterID == get_parameter_name(Parameter::algorithm))
    {
        analyser.setAlgorithm(static_cast<Analyser::Algorithm>(static_cast<int>(newValue)));
        // Persist selection as the default in Project Settings
        projectManager.setProjectSettingsParameter(TSS_SETTINGS::FUNDAMENTAL_FREQUENCY_ALGORITHM,
                                                  static_cast<double>(newValue));
    }

    if (parameterID == get_parameter_name(Parameter::harmonics))
    {
        analyser.setNumHarmonicsToTrack(static_cast<int>(newValue));
    }

    else if (parameterID == get_parameter_name(Parameter::fft_window))
    {
        analyser.setNewFFTWindowFunction(static_cast<int>(newValue));
    }
    else if (parameterID == get_parameter_name(Parameter::frequency_range))
    {
        analyser.setCustomRange(static_cast<bool>(newValue));
    }
    else if (parameterID == get_parameter_name(Parameter::min_frequency))
    {
        analyser.setCustomRangeMinFrequency(static_cast<double>(newValue));
    }
    else if (parameterID == get_parameter_name(Parameter::max_frequency))
    {
        analyser.setCustomRangeMaxFrequency(static_cast<double>(newValue));
    }
    else if (parameterID == get_parameter_name(Parameter::threshold))
    {
        analyser.setInputThreshold(static_cast<double>(newValue));
    }
    else if (parameterID == get_parameter_name(Parameter::min_interval))
    {
        analyser.setMinInterval(static_cast<double>(newValue));
    }
    else if (parameterID == get_parameter_name(Parameter::min_interval))
    {
        analyser.setMinInterval(static_cast<double>(newValue));
    }
    else if (parameterID == get_parameter_name(Parameter::max_interval))
    {
        analyser.setMaxInterval(static_cast<double>(newValue));
    }
}

juce::StringArray FundamentalFrequencyProcessor::getInputNames()
{
    auto device = projectManager.getDeviceManager()->getCurrentAudioDevice();

    return device->getInputChannelNames();
}

juce::StringArray FundamentalFrequencyProcessor::getOutputNames()
{
    auto device = projectManager.getDeviceManager()->getCurrentAudioDevice();

    return device->getOutputChannelNames();
}
