/*
  ==============================================================================

    FundamentalFrequencyProcessor.cpp
    Created: 17 Mar 2021 9:48:01pm
    Author:  Gary Jones

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
}

auto * FundamentalFrequencyProcessor::getCurrentEditor()
{
    return dynamic_cast<FundamentalFrequencyComponent*>(getActiveEditor());
}

void FundamentalFrequencyProcessor::prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock) 
{
    auto inputChannels = static_cast<int>(projectManager.deviceManager->getCurrentAudioDevice()->getActiveInputChannels().toInt64());

    analyser.setupAnalyser(static_cast<float>(sampleRate), inputChannels);
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
            analyser.addAudioData(buffer, inputChannel, 1);
        }
    }

    buffer.clear();

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
