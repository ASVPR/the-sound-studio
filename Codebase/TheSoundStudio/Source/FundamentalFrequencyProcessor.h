/*
  ==============================================================================

    FundamentalFrequencyProcessor.h
    Created: 17 Mar 2021 9:48:01pm
    Author:  Gary Jones

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Analyzer.h"
#include "VotanSynthProcessor.h"
#include "SamplerProcessor.h"
#include "WavetableSynthProcessor.h"

class ProjectManager;

class FundamentalFrequencyProcessor : public juce::AudioProcessor,
public juce::AudioProcessorValueTreeState::Listener
{
public:
    FundamentalFrequencyProcessor(ProjectManager& pm, FrequencyManager& fm);

    ~FundamentalFrequencyProcessor() override
    {
        analyser.stopThread(1000);
    }
    
    void prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock) override;
    
    void processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override;
    
    void clearAlgorithm();
    
    double getTailLengthSeconds() const override { return 0.0; }

    bool acceptsMidi() const override { return true; }

    bool producesMidi() const override { return false; }

    void releaseResources() override { }

    juce::AudioProcessorEditor * createEditor() override;

    bool hasEditor() const override {  return false; }

    const juce::String getName() const  override { return "FundamentalFrequencyProcessor"; }

    int getNumPrograms()override { return 0; }

    int getCurrentProgram() override { return 0; }

    void setCurrentProgram (int index) override { }

    const juce::String getProgramName (int index) override { return ""; }

    void changeProgramName (int index, const String& newName) override { }

    void getStateInformation (juce::MemoryBlock& destData) override {}

    void setStateInformation (const void* data, int sizeInBytes) override {}

    float getCurrentFrequency() { return analyser.getCurrentFrequency(); };

    bool isPlaying()
    {
        return playing.load();
    }

    void startAnalyser()
    {
        clearAlgorithm();

        if (!playing.load())
        {
            analyser.startProcess();
            playing.store(true);
        }
    }

    void stopAnalyser()
    {
        if (playing.load())
        {
            playing.store(false);
            analyser.stopProcess();
        }
    }

    bool is_doing_analysis()
    {
        return analyser.is_doing_analysis();
    }
    
    Analyser& getAnalyser() { return analyser; }

    enum class Parameter
    {
        fft_size,
        fft_window,
        iteration_length,
        iterations,
        input_channel,
        algorithm,
        harmonics,
        threshold,
        min_interval,
        max_interval,
        min_frequency,
        max_frequency,
        frequency_range
    };

    static constexpr const char * get_parameter_name(Parameter p)
    {
        
        switch (p)
        {
            case Parameter::fft_size:
                return "fft_size";
                break;

            case Parameter::fft_window:
                return "fft_window";
                break;

            case Parameter::iteration_length:
                return "iteration_length";
                break;

            case Parameter::iterations:
                return "iterations";
                break;

            case Parameter::input_channel:
                return "input_channel";
                break;

            case Parameter::algorithm:
                return "algorithm";
                break;

            case Parameter::harmonics:
                return "harmonics";
                break;
            case Parameter::threshold:
                return "threshold";
            break;
            case Parameter::min_interval:
                return "min_interval";
            break;
            case Parameter::max_interval:
                return "max_interval";
            break;
            case Parameter::min_frequency:
                return "min_frequency";
            break;
            case Parameter::max_frequency:
                return "max_frequency";
            break;
            case Parameter::frequency_range:
                return "frequency_range";
            break;
        }
    }


    int get_iterations()
    {
        return analyser.getIterations();
    }

    void set_fft_size(Analyser::fft_size size);

    Analyser::fft_size get_fft_size()
    {
        auto value = apvts.getRawParameterValue(get_parameter_name(Parameter::fft_size))->load();

        return get_fft_size_from_index(value);
    }

    std::optional<float> set_iteration_length(float sm)
    {
        return analyser.set_iteration_length(sm);
    }

    float get_iteration_length()
    {
        return analyser.get_iteration_length();
    }

    void set_iterations(int num)
    {
        analyser.setIterationNumber(num);
    }

    int get_num_of_harmonics()
    {
        return apvts.getRawParameterValue(get_parameter_name(Parameter::harmonics))->load();
    }


    static constexpr Analyser::fft_size get_fft_size_from_index(int index)
    {
        switch (index)
        {
            case 0:
                return Analyser::fft_size::size_10;
                break;
            case 1:
                return Analyser::fft_size::size_11;
                break;
            case 2:
                return Analyser::fft_size::size_12;
                break;
            case 3:
                return Analyser::fft_size::size_13;
                break;
            case 4:
                return Analyser::fft_size::size_14;
                break;
            case 5:
                return Analyser::fft_size::size_15;
                break;
            case 6:
                return Analyser::fft_size::size_16;
                break;
            case 7:
                return Analyser::fft_size::size_17;
                break;
            case 8:
                return Analyser::fft_size::size_18;
                break;
            case 9:
                return Analyser::fft_size::size_19;
                break;
            case 10:
                return Analyser::fft_size::size_20;
                break;
        }

        return Analyser::fft_size::size_10;
    }

    static constexpr int get_fft_size_as_index(Analyser::fft_size size)
    {
        switch (size)
        {
            case Analyser::fft_size::size_10:
                return 0;
                break;
            case Analyser::fft_size::size_11:
                return 1;
                break;
            case Analyser::fft_size::size_12:
                return 2;
                break;
            case Analyser::fft_size::size_13:
                return 3;
                break;
            case Analyser::fft_size::size_14:
                return 4;
                break;
            case Analyser::fft_size::size_15:
                return 5;
                break;
            case Analyser::fft_size::size_16:
                return 6;
                break;
            case Analyser::fft_size::size_17:
                return 7;
                break;
            case Analyser::fft_size::size_18:
                return 8;
                break;
            case Analyser::fft_size::size_19:
                return 9;
                break;
            case Analyser::fft_size::size_20:
                return 10;
                break;
        }
    }

    juce::AudioProcessorValueTreeState& getApvts()
    {
        return apvts;
    };

    void parameterChanged(const juce::String& parameterID, float newValue) override;

    bool should_wait_for_harmonics()
    {
        return analyser.should_wait_for_harmonics();
    }

    bool is_fft_process_enable()
    {
        return analyser.is_fft_process_enable();
    }

    void enable_fft_process(bool enable)
    {
        return analyser.enable_fft_process(enable);
    }

    bool is_custom_range_enable()
    {
        return apvts.getRawParameterValue(get_parameter_name(Parameter::frequency_range))->load();
    }

    void enable_custom_range(bool enable)
    {
        auto value = apvts.getParameterAsValue(get_parameter_name(Parameter::frequency_range));

        value.setValue(enable);
    }

    juce::StringArray getInputNames();

    juce::StringArray getOutputNames();

private:

    juce::AudioProcessorValueTreeState apvts;

    auto * getCurrentEditor();

    void setNewFFTWindowFunction(int windowEnum);

    ProjectManager& projectManager;

    FrequencyManager& frequencyManager;

    Analyser analyser;

    std::atomic<bool> playing { false };
};
