/*
  ==============================================================================

    TSSConstants.h
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#pragma once

#include <cstdint>
#include <cmath>
#include <array>

namespace TSS {

// ============================================================
// Audio Constants
// ============================================================
namespace Audio {
    constexpr double kDefaultA4Frequency    = 432.0;
    constexpr double kStandardA4Frequency   = 440.0;
    constexpr double kMinA4Frequency        = 333.0;
    constexpr double kMaxA4Frequency        = 666.0;

    constexpr double kDefaultSampleRate     = 44100.0;
    constexpr double kSampleRate48K         = 48000.0;
    constexpr double kSampleRate96K         = 96000.0;

    constexpr int    kDefaultFFTOrder       = 10;
    constexpr int    kDefaultFFTSize        = 1 << kDefaultFFTOrder;

    constexpr int    kMaxInputChannels      = 4;
    constexpr int    kMaxOutputChannels     = 4;
    constexpr int    kMaxAnalyzers          = 8;

    constexpr int    kWavetableSize         = 2048;
    constexpr int    kRecordingBufferSize   = 32768;

    constexpr int    kMaxShortcutSynths     = 20;
    constexpr int    kMaxPluginSlots        = 4;
    constexpr int    kMaxCustomChordNotes   = 12;
    constexpr int    kMIDINoteCount         = 128;
    constexpr int    kMaxNumVoices          = 16;
    constexpr int    kMaxVoicesPerEngine    = 32;

    constexpr float  kDefaultDecayRate      = 0.0008f;
    constexpr float  kStringDecayRate       = 0.0003f;

    constexpr double kFrequencyMax          = 384000.0;
    constexpr double kFrequencyMin          = 0.01;

    constexpr int    kTimerUpdateRate       = 20;
    constexpr int    kFrameSize             = 256;

    constexpr double kSemitonesPerOctave    = 12.0;
    constexpr double kOctaveRatio           = 2.0;
}

// ============================================================
// ADSR Defaults
// ============================================================
namespace ADSR {
    constexpr double kMinAmplitude = 0.0;
    constexpr double kMaxAmplitude = 100.0;
    constexpr double kMinAttack    = 0.0;
    constexpr double kMaxAttack    = 10000.0;
    constexpr double kMinDecay     = 0.0;
    constexpr double kMaxDecay     = 10000.0;
    constexpr double kMinSustain   = 0.0;
    constexpr double kMaxSustain   = 100.0;
    constexpr double kMinRelease   = 0.0;
    constexpr double kMaxRelease   = 10000.0;
}

// ============================================================
// Frequency Manipulation Limits
// ============================================================
namespace FreqManip {
    constexpr double kMultiplyMin  = 0.001;
    constexpr double kMultiplyMax  = 999.0;
    constexpr double kDivisionMin  = 0.001;
    constexpr double kDivisionMax  = 999.0;
    constexpr int    kRepeatMin    = 1;
    constexpr int    kRepeatMax    = 99;
    constexpr int    kPauseMin     = 1;
    constexpr int    kPauseMax     = 10000;
    constexpr int    kLengthMin    = 1;
    constexpr int    kLengthMax    = 10000;
    constexpr double kInsertFreqMin = 8.0;
    constexpr double kInsertFreqMax = 20000.0;
    constexpr int    kOctaveMin    = 0;
    constexpr int    kOctaveMax    = 16;
}

// ============================================================
// UI Constants
// ============================================================
namespace UI {
    constexpr float  kDefaultScaleFactor    = 0.5f;
    constexpr float  kMinScaleFactor        = 0.3f;
    constexpr float  kMaxScaleFactor        = 2.0f;

    constexpr int    kOscilloscopeRefreshHz = 33;

    constexpr int    kBaseWindowWidth       = 1920;
    constexpr int    kBaseWindowHeight      = 1740;

    constexpr float  kMenuSideBarWidthRatio = 0.18f;
    constexpr float  kMinSidebarWidth       = 280.0f;
    constexpr float  kMaxSidebarWidth       = 400.0f;

    namespace ScaleBreakpoints {
        constexpr int kSmall  = 1280;
        constexpr int kMedium = 1920;
        constexpr int kLarge  = 2560;
    }

    namespace Colors {
        constexpr uint32_t kSliderBackground    = 0xFF626672;  // Colour(98, 103, 114)
        constexpr uint32_t kSliderFill          = 0xFF007ACD;  // Colour(0, 122, 205)
        constexpr uint32_t kSliderOutline       = 0xFF2A303C;  // Colour(42, 48, 60)
        constexpr uint32_t kComboBoxText        = 0xFF333333;  // {51, 51, 51}
        constexpr uint32_t kComboBoxBackground  = 0xFFF9FAFA;  // Colour::fromRGB(249, 250, 250)
        constexpr uint32_t kPopupMenuBackground = 0xFFB7B7B7;  // Colour::fromRGB(183, 183, 183)
        constexpr uint32_t kTextEditorBackground = 0xFF626672; // Colour(98, 103, 114)
    }
}

// ============================================================
// Spectrogram Constants
// ============================================================
namespace Spectrogram {
    constexpr float  kDefaultMinHertz = 11.7185f;
    constexpr float  kDefaultMaxHertz = 22050.0f;
    constexpr float  kDefaultMinDbFS  = -80.0f;
    constexpr float  kDefaultMaxDbFS  = 12.0f;
    constexpr int    kNumFreqLines    = 10;
    constexpr int    kNumDBLines      = 8;
    constexpr float  kDBLogFactor     = 2.4f;
    constexpr int    kDBAxisGap       = 40;
    constexpr int    kFreqAxisGap     = 17;
}

// ============================================================
// Physical Modeling Constants
// ============================================================
namespace Synthesis {
    constexpr float kPianoInharmonicityCoeff = 0.0001f;
    constexpr float kC4ReferenceFreq         = 261.63f;
    constexpr float kInharmonicityExponent   = 1.8f;
    constexpr int   kMaxHarmonics            = 12;
    constexpr float kGoldenRatio             = 0.618f;
    constexpr float kDefaultResonance        = 0.7f;
    constexpr float kDefaultFeedback         = 0.95f;
    constexpr float kDefaultStringDamping    = 0.3f;

    // Bowed string harmonic amplitude ratios
    constexpr float kBowedStringHarmonics[] = {
        1.0f, 0.75f, 0.55f, 0.42f, 0.32f, 0.24f, 0.18f, 0.14f, 0.10f
    };

    // Hammond organ drawbar ratios
    constexpr float kOrganDrawbarRatios[] = {
        0.8f, 0.6f, 0.4f, 0.5f, 0.3f, 0.4f
    };

    // Guitar body resonance frequencies and scaling
    constexpr float kGuitarBodyFrequencies[] = { 100.0f, 200.0f, 400.0f };
    constexpr float kGuitarBodyScaling[]     = { 50.0f, 80.0f, 100.0f };
}

// ============================================================
// Frequency-to-Light Constants
// ============================================================
namespace FreqToLight {
    constexpr double kSpeedOfLightVacuum  = 299792458.0;
    constexpr double kSpeedOfLightWater   = 225000000.0;
    constexpr double kSpeedOfSoundVacuum  = 331.5;
    constexpr double kSpeedOfSoundWater   = 1498.0;
    constexpr double kGamma               = 0.80;
    constexpr double kIntensityMax        = 255.0;
    constexpr double kMinWavelengthNm     = 380.0;
    constexpr double kMaxWavelengthNm     = 781.0;

    constexpr double kMeterFactor         = 0.000000001;
    constexpr double kCentimeterFactor    = 0.0000001;
    constexpr double kMillimeterFactor    = 0.000001;
    constexpr double kMicrometerFactor    = 0.001;

    constexpr double kWavelengthMax       = 1000.0;
    constexpr double kWavelengthMin       = 1.0;
    constexpr double kPhaseSpeedMax       = 20.0;
    constexpr double kPhaseSpeedMin       = 1.0;

    constexpr int    kNumHarmonics        = 5;
    constexpr int    kMaxHarmonicsToTrack = 10;
}

// ============================================================
// Input Detection
// ============================================================
namespace Detection {
    constexpr double kInputThresholdMin = -80.0;
    constexpr double kInputThresholdMax = 6.0;
}

// ============================================================
// File System Paths (relative to user documents)
// ============================================================
namespace Paths {
    constexpr const char* kAppDataDir     = "TSS";
    constexpr const char* kLogsSubDir     = "Logs";
    constexpr const char* kRecordingsDir  = "Recordings";
    constexpr const char* kProfilesDir    = "Profiles";
    constexpr const char* kLogExtension   = ".txt";
}

// ============================================================
// Waveform Type Mapping
// ============================================================
namespace Waveforms {
    enum Type { Instrument = 0, Sine, Triangle, Square, Sawtooth, Wavetable, Count };

    constexpr const char* kNames[] = {
        "Instrument", "Sinewave", "Triangle", "Square", "Sawtooth", "Wavetable"
    };

    inline const char* getName(int type) {
        if (type >= 0 && type < Count) return kNames[type];
        return "Unknown";
    }
}

} // namespace TSS
