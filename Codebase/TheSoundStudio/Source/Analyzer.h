/*
  ==============================================================================

    Analyzer.h
    Created: 27 May 2020 11:09:07am
    Author:  Gary Jones

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "Parameters.h"
#include "FrequencyManager.h"

//==============================================================================
/*
*/

class Analyser : public juce::Thread
{
public:

    enum class Algorithm
    {
        TrackHarmonics = 0,
        Yin,
        MPM
    };

    enum class fft_size
    {
        size_10 = 1024,
        size_11 = 2048,
        size_12 = 4096,
        size_13 = 8192,
        size_14 = 16384,
        size_15 = 32768,
        size_16 = 65536,
        size_17 = 131072,
        size_18 = 262144,
        size_19 = 524288,
        size_20 = 1048576
    };

    Analyser(FrequencyManager& freqManager);
    
    Analyser (FrequencyManager& freqManager, int instance);
    
    void init_fft();

    void addAudioData (const AudioBuffer<float>& buffer, int startChannel, int numChannels);

    void setupAnalyser (float sampleRateToUse, int numChannels);

    void run() override;
    
    const float * getFFTReadPointer();
    
    int getFFTSize();
    
    void setNewFFTWindowFunction(int windowEnum);
    
    void setNewFFTSize(fft_size fftEnum);

    void setNumHarmonicsToTrack(int num);
    
    void setAlgorithm(Algorithm newAlgorithm);
    
    void setCustomRange(bool should);
    
    void setCustomRangeMinFrequency(double f);
    
    void setCustomRangeMaxFrequency(double f);
    
    void setInputThreshold(double db);
    
    void setMinInterval(double min);
    
    void setMaxInterval(double max);
    
    void initHarmonics();

    void setIterationNumber(int iterationsNum)
    {
        shouldProcess = false;
        iterations = iterationsNum;
        iterationsData.reserve(iterations);

        stopProcess();

        clearAlgorithm();

        shouldProcess = true;
    }

    float get_iteration_length() const
    {
        return get_samples_as_time(iterationLength);
    }

    void startProcess()
    {
        iterationMedianData.clear();
        iterationsData.clear();
        startThread (juce::Thread::Priority::normal);
        shouldProcess.store(true,std::memory_order_relaxed);
        isPlaying.store(true,std::memory_order_relaxed);
    }

    void stopProcess()
    {
        isPlaying.store(false, std::memory_order_release);
        shouldProcess.store(false, std::memory_order_release);
        signalThreadShouldExit();
        notify();

        if (waitForThreadToExit(100))
        {

        }
    }

    bool is_doing_analysis()
    {
        return isPlaying.load();
    }

    static constexpr int get_maximum_fft_size() { return static_cast<int>( fft_size::size_20);  };

    struct Harmonic
    {
        void initialize() noexcept
        {
            isActive         = false;
            binRef           = -1;
            freq             = 432.f;
            db               = -80.f;
            velocity         = 127;
            freqEMA          = 0.f;
            nearestMidiNote  = 0;
            percentDif       = 0.f;
            interval         = 0.f;
        }

        bool    isActive            = false;
        int     binRef              = 0;
        double  freq                = 0.f;
        double  db                  = -80.f;
        int     velocity            = 127;
        double  freqEMA             = 0.f;
        int     nearestMidiNote     = 0;
        float   percentDif          = 0.f; // +/-
        float   interval            = 0.f;
    };

    struct Harmonics
    {
        constexpr static size_t max_num_harmonics = 10;

        void initialize() noexcept
        {
            // C++17: Range-based for loop with auto (already modern)
            for (auto& h : harmonic)
            {
                h.initialize();
            }
        }

        auto& getHarmonic(int index)
        {
            return harmonic[index];
        }

        auto& get_harmonics_array()
        {
            return harmonic;
        }

        void set_tonic_frequency(float tonic)
        {
            auto multiplier = (tonic / getHarmonic(0).freq);

            for (auto& h : get_harmonics_array())
            {
                h.freq = h.freq * multiplier;
            }
        }

        std::array<Harmonic, max_num_harmonics> harmonic;
    };

    float calculateHarmonics();

    void clearAlgorithm();


    Harmonics& getHarmonics()
    {
        return harmonics;
    }

    float getCurrentFrequency() { return currentFrequency.load(); };

    Harmonics harmonics;

   [[nodiscard]] std::optional<float> set_iteration_length(float length_in_ms);

    double get_sample_rate() const { return sampleRate; }

    int getIterations()
    {
        return iterations;
    }

    bool should_wait_for_harmonics()
    {
        return shouldWaitforHarmonics.load();
    }

    struct Listener
    {
        virtual ~Listener() {};

        virtual void fft_updated() = 0;
    };

    void add_listener(Listener& listener)
    {
        listeners.add(&listener);
    }

    void remove_listener(Listener& listener)
    {
        listeners.remove(&listener);
    }

    bool is_fft_process_enable()
    {
        return enable_fft.load();
    }

    void enable_fft_process(bool enable)
    {
        enable_fft.store(enable, std::memory_order_release);

        if (!enable)
        {
            stopProcess();
        }
    }

private:

    float get_minimum_iteration_length() const;

    float get_samples_as_time(int numSamples) const;

    int get_time_as_samples(float ms) const;

    constexpr float calculate_median(std::vector<float>& d)
    {
        auto s = static_cast<int>(d.size());

        if (s % 2 == 0 )
        {
            std::nth_element(d.begin(),
                             d.begin() + s / 2,
                             d.end());

            std::nth_element(d.begin(),
                             d.begin() + (s - 1) / 2,
                             d.end());

            return  (d[(s - 1) / 2] + d[s / 2]) / 2.f;
        }
        else
        {
            std::nth_element(d.begin(),
                             d.begin() + s / 2,
                             d.end());

            return d[s / 2];
        }
    }

    constexpr float calculate_emm(std::vector<float>& data, float current)
    {
        const auto multiplier = 2.f / (static_cast<int>(data.size()) + 1);
        
        auto median = calculate_median(data);

        return (current * multiplier) + (median * (1.f - multiplier));
    };
    
    FrequencyManager& frequencyManager;

    std::unique_ptr<adamski::PitchMPM> pitchPMP;

    std::unique_ptr<adamski::PitchYIN> pitchYIN;

    std::atomic<float> currentFrequency { 0.f };

    WaitableEvent   waitForData;

    CriticalSection pathCreationLock;

    float sampleRate { 44100 };
    
    int fftSize;

    std::unique_ptr<dsp::FFT> fft;
    
    std::unique_ptr<dsp::WindowingFunction<float>> windowing;

    dsp::WindowingFunction<float>::WindowingMethod  windowMethod{dsp::WindowingFunction<float>::WindowingMethod::blackmanHarris};

    int averagerPtr = 1;

    juce::AbstractFifo abstractFifo { 44100 };

    juce::AbstractFifo harmonicsAbstractFifo { 44100 };

    juce::AudioBuffer<float> audioFifo;
    juce::AudioBuffer<float> secondAudioFifo;
    juce::AudioBuffer<float> fftBuffer;
    juce::AudioBuffer<float> averager;
    juce::AudioBuffer<float> pitchDetectorBuffer;

    /* ITERATION */
    
    std::vector<float> iterationMedianData;
    std::vector<float> iterationsData;

    int iterations { 3 };
    int iterationLength { 32768 };
    int currentIteration { 0 };

    double maxNumberOfSamplesToRead { 0 };
    double numberOfSamplesRead { 0 };
    bool retryAnalysis { false };

    /*=========================*/
    
    std::atomic<bool> shouldProcess = true;

    std::atomic<bool> isProcessing { false };

    std::atomic<bool> isPlaying { false };

    std::atomic<bool> shouldWaitforHarmonics { false };

    std::atomic<bool> enable_fft { true };
    
    std::atomic<int> numHarmonicsToTrack { 10 };
    Algorithm algorithm { Algorithm::TrackHarmonics };

    bool customRange                = false;
    double customRangeMinFrequency  = 1.f;
    double customRangeMaxFrequency  = 22000.0;
    double inputThreshold           = -80.0;
    double minIntervalInOctaves     = 0.2;
    double maxIntervalInOctaves     = 4.0;

    int customRangeMinBin = 0;
    int customRangeMaxBin = 512;

    juce::ListenerList<Listener> listeners;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Analyser)
};
