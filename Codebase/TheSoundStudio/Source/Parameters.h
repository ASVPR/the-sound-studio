/*
  ==============================================================================

    Parameters.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#define DEFAULT_SAMPLER_FILEPATH   ""
#define NUM_SHORTCUT_SYNTHS 20
#define NUM_PLUGIN_SLOTS 4
#define TIMER_UPDATE_RATE 20
#define FRAMESIZE 256
#define FFT_ORDER 10
#define FFT_BUFFER_SIZE 2048
// Synth params / defines
#define MAX_NUM_VOICES 16
#define MIN_BASE_A_FREQUENCY 333
#define MAX_BASE_A_FREQUENCY 666
#define MIN_AMPLITUDE 0
#define MAX_AMPLITUDE 100
#define MIN_ATTACK 0
#define MAX_ATTACK 10000
#define MIN_DECAY 0
#define MAX_DECAY 10000
#define MIN_SUSTAIN 0
#define MAX_SUSTAIN 100
#define MIN_RELEASE 0
#define MAX_RELEASE 10000
#define MULTIPLY_MINIMUM 0.001
#define MULTIPLY_MAXIMUM 999
#define DIVISION_MINIMUM 0.001
#define DIVISION_MAXIMUM 999
#define REPEAT_MIN 1
#define REPEAT_MAX 99
#define PAUSE_MAX 10000
#define PAUSE_MIN 1
#define LENGTH_MIN 1
#define LENGTH_MAX 10000
#define INSERT_FREQ_MIN 8
#define INSERT_FREQ_MAX 20000

#define FREQUENCY_MAX 384000
#define FREQUENCY_MIN 0.01

#define WAVELENGTH_MAX 1000
#define WAVELENGTH_MIN 1
#define PHASESPEED_MAX 20
#define PHASESPEED_MIN 1

#define OCTAVE_MIN 0
#define OCTAVE_MAX 16

#define NUM_COLOUR_HARMONICS

#define MAX_HARMONICS_TO_TRACK 10

#define INPUT_THRESHOLD_DETECT_MIN -80
#define INPUT_THRESHOLD_DETECT_MAX 6



enum AUDIO_MODE
{
    MODE_NONE = 0,
    MODE_CHORD_PLAYER,
    MODE_CHORD_SCANNER,
    MODE_FUNDAMENTAL_FREQUENCY,
    MODE_FREQUENCY_PLAYER,
    MODE_FREQUENCY_SCANNER,
    MODE_FREQUENCY_TO_LIGHT,
    MODE_REALTIME_ANALYSIS,
    MODE_LISSAJOUS_CURVES,
    MODE_FEEDBACK_MODULE,
    MODE_FEEDBACK_MODULE_HARDWARE,
    MODE_CALCULATORS,
    NUM_MODES
};

enum TSS_SETTINGS
{
    SAMPLE_RATE = 0,        // double
    OVERSAMPLE_FACTOR,      // int
    
    DEFAULT_SCALE,
    
    //Strings
    LOG_FILE_LOCATION,
    RECORD_FILE_LOCATION,
    RECORD_FILE_FORMAT,     // int / enum
    
    // double
    BASE_FREQUENCY_A,       // double // central pitch
    
    // doubles
    AMPLITUDE_MIN,          // max 100 / min -100 ????
    AMPLITUDE_MAX,
    ATTACK_MIN,             // Min 0 ms
    ATTACK_MAX,             // Max 10000ms
    DECAY_MIN,              // 0ms
    DECAY_MAX,              // max = 10000ms
    SUSTAIN_MIN,            // 10dB ??
    SUSTAIN_MAX,
    RELEASE_MIN,
    RELEASE_MAX,
    
    SHOW_HIGHEST_PEAK_FREQUENCY,        // bool
    NUMBER_HIGHEST_PEAK_FREQUENCIES,    // int
    SHOW_HIGHEST_PEAK_OCTAVES,          // bool
    NUMBER_HIGHEST_PEAK_OCTAVES,        // int
    
    PLAYER_PLAY_IN_LOOP,
    PLAYER_PLAY_SIMULTANEOUS,
    
    FFT_SIZE,                           // enum 1024, 2048, 4096, 8192, 16384, 32762
    
    FFT_WINDOW,
    
    FFT_DELAY_MS,
    
    FFT_COLOR_SPEC_MAIN,
    FFT_COLOR_SPEC_SEC,
    
    FFT_COLOR_OCTAVE_MAIN,
    FFT_COLOR_OCTAVE_SEC,
    
    FFT_COLOR_COLOR_MAIN,
    FFT_COLOR_COLOR_SEC,
    
    FFT_COLOR_3D_MAIN,
    FFT_COLOR_3D_SEC,
    
    FFT_COLOR_FREQUENCY_MAIN,
    FFT_COLOR_FREQUENCY_SEC,
    
    LISSAJOUS_COLOR_MAIN,
    LISSAJOUS_COLOR_SEC,

    GUI_SCALE,
    
    PANIC_NOISE_TYPE,

    FUNDAMENTAL_FREQUENCY_ALGORITHM,
    
    PLUGIN_SELECTED_1, // index from Known Plugin List | -1 = NO_PLUGIN
    PLUGIN_SELECTED_2,
    PLUGIN_SELECTED_3,
    PLUGIN_SELECTED_4,
    
    FREQUENCY_TO_CHORD, // Radio button 3
    
    MIXER_INPUT_GAIN_1,
    MIXER_INPUT_GAIN_2,
    MIXER_INPUT_GAIN_3,
    MIXER_INPUT_GAIN_4,
    
    MIXER_OUTPUT_GAIN_1,
    MIXER_OUTPUT_GAIN_2,
    MIXER_OUTPUT_GAIN_3,
    MIXER_OUTPUT_GAIN_4,
    
    MIXER_INPUT_FFT_1,
    MIXER_INPUT_FFT_2,
    MIXER_INPUT_FFT_3,
    MIXER_INPUT_FFT_4,
    
    MIXER_OUTPUT_FFT_1,
    MIXER_OUTPUT_FFT_2,
    MIXER_OUTPUT_FFT_3,
    MIXER_OUTPUT_FFT_4,

    TOTAL_NUM_SETTINGS
};

enum VISUALISER_SOURCE
{
    INPUT_1 = 1,
    INPUT_2,
    INPUT_3,
    INPUT_4,
    OUTPUT_1,
    OUTPUT_2,
    OUTPUT_3,
    OUTPUT_4,

    TOTAL_NUM_SOURCES
};




// To shift to this I need to :
// 1. FrequencyManager/ChordManager
// 2. Synth Objects need to focus on octave shifts on individual notes
// 2.1 Check Custom Chord notes, may call midiNotes
// 3. GUI, all widgets that use Chord root notes, need multiple otpions

// should prob just be player command, then each unit (chordplayer, freq player etc, can reuse without confusion..
enum PLAYER_COMMANDS
{
    // play chord,
    COMMAND_PLAYER_PLAYPAUSE = 0, //
    COMMAND_PLAYER_STOP,           // return to start
    COMMAND_PLAYER_RECORD,         // record arm, starts recording upon pressing play

    // panic
    COMMAND_PLAYER_PANIC,           // notes off to all instruments
    
    COMMAND_PLAYER_PLAY_IN_LOOP,
    COMMAND_PLAYER_PLAY_SIMULTANEOUS,
    
    TOTAL_NUM_COMMANDS
};

enum PLAY_STATE
{
    OFF = 0,
    PLAYING,
    PAUSED,
    NUM_PLAY_STATES
};

enum PLAY_MODE
{
    NORMAL = 0, // sequenced
    LOOP,       // loop individual except plat at same times
//    TRIGGER,
//    LOOP_SIMULTANEOUS,
    
    NUM_PLAY_MODES
};




// Instruments

enum INSTRUMENTS
{
    PIANO = 1,
    ELEC_PIANO,
    FLUTE,
    GUITAR,
    HARP,
    BELL,
    STRINGS,
    VIOLIN,
    
    TOTAL_NUM_INSTRUMENTS
};



enum CHORD_PLAYER_SHORTCUT_KEY
{
    SHORTCUT_A = 0, SHORTCUT_S, SHORTCUT_D, SHORTCUT_F, SHORTCUT_G, SHORTCUT_H, SHORTCUT_J, SHORTCUT_K, SHORTCUT_L, SHORTCUT_Z, SHORTCUT_X, SHORTCUT_C, SHORTCUT_V, SHORTCUT_B, SHORTCUT_N, SHORTCUT_M,  SHORTCUT_Q,  SHORTCUT_W,  SHORTCUT_E,  SHORTCUT_R, NUM_CHORD_PLAYER_SHORTCUT_KEYS
};

enum SCALES
{
    DIATONIC_PYTHAGOREAN = 1,
    DIATONIC_ITERATION_FIFTH,
    DIATONIC_JUSTLY_TUNED,
    CHROMATIC_PYTHAGOREAN,
    CHROMATIC_JUST_INTONATION,
    CHROMATIC_ET,
//    HARMONIC_ET,
//    HARMONIC_MODERN,
    HARMONIC_SIMPLE,
    ENHARMONIC,
    SOLFEGGIO,

    TOTAL_NUM_SCALES
};

enum AUDIO_OUTPUTS
{
    NO_OUTPUT = 1,
    MONO_1,
    MONO_2,
    MONO_3,
    MONO_4,
    MONO_5,
    MONO_6,
    MONO_7,
    MONO_8,
    STEREO_1_2,
    STEREO_3_4,
    STEREO_5_6,
    STEREO_7_8,
    
    TOTAL_OUTPUT_TYPES
};



//enum KEYNOTE
//{
//    C = 1, D, E, F, G, A, B,
//    NUM_KEYNOTES
//
//};

enum KEYNOTES
{
    KEY_C = 1,
    KEY_C_SHARP,
    KEY_D,
    KEY_D_SHARP,
    KEY_E,
    KEY_F,
    KEY_F_SHARP,
    KEY_G,
    KEY_G_SHARP,
    KEY_A,
    KEY_A_SHARP,
    KEY_B,
    
    TOTAL_NUM_KEYNOTES
};

enum CHORD_TYPES
{
    Major = 1,
    Minor,
    Augmented,
    Diminished,
    Suspended4th,
    Suspended2nd,
    Normal5th,
    Normal6th,
    Normal6thMinor,
    Normal7th,
    Normal7thMajor,
    Normal7thMinor,
    Normal7thDiminished,
    Normal7thHalfdiminished,
    Normal7th_5,
    Normal9th,
    Normal7th_9,
    Normal9thMajor,
    Normal9thMinor,
    Normal9thAdded,
    Normal9thMinorAdded,
    Normal11th,
    Normal11thMinor,
    Normal7th_11,
    Major7th_11,
    Normal13th,
    Major13th,
    Minor13th,
    
    NUM_CHORD_TYPES
};

enum ADD_ONS
{
    AddOn_NONE = -1,
    AddOn_6,
    AddOn_7,
    AddOn_7Major,
    AddOn_9,
    AddOn_9flat,
    AddOn_9sharp,
    AddOn_11,
    AddOn_11sharp,
    AddOn_13,
    AddOn_13flat,
    AddOn_Inverted,
    
    NUM_ADDONS
};

enum CHORD_PLAYER_SHORTCUT_PARAMS
{
    // internal params for each shortcut
    SHORTCUT_IS_ACTIVE  = 0,
    SHORTCUT_MUTE,
    SHORTCUT_PLAY_AT_SAME_TIME,     // not in use, but will leave
    
    // synth params
    WAVEFORM_TYPE,                  // osc type {default, sine, triangle, squ, saw}
    
    INSTRUMENT_TYPE,                // sampler enum
    ENV_AMPLITUDE,                  // double
    ENV_ATTACK,                     // double
    ENV_SUSTAIN,                    // double
    ENV_DECAY,                      // double
    ENV_RELEASE,                    // double
    
    CHORD_SOURCE,                   // boolean - list or frequency
    // frequency
    INSERT_FREQUENCY,               // double frequency
    OCTAVE,
    
    // Manipulate Chosen Frequency
    MANIPULATE_CHOSEN_FREQUENCY,    // boolean
    MULTIPLY_OR_DIVISION,           // boolean
    MULTIPLY_VALUE,                 // double
    DIVISION_VALUE,                 // double
    
    // repeats
    NUM_REPEATS,                    // enum of millisecond values
    NUM_PAUSE,                      // enum of millisecond values
    NUM_DURATION,                   // duration ms of note downs
    
    // chord params
    
    // list
    KEYNOTE,                        // enum// enum
    CHORD_TYPE,                     // enum
    ADD_ONS,                        // enum
    
    CUSTOM_CHORD,                   // boolean
    
    CUSTOM_CHORD_ACTIVE_1,
    CUSTOM_CHORD_ACTIVE_2,
    CUSTOM_CHORD_ACTIVE_3,
    CUSTOM_CHORD_ACTIVE_4,
    CUSTOM_CHORD_ACTIVE_5,
    CUSTOM_CHORD_ACTIVE_6,
    CUSTOM_CHORD_ACTIVE_7,
    CUSTOM_CHORD_ACTIVE_8,
    CUSTOM_CHORD_ACTIVE_9,
    CUSTOM_CHORD_ACTIVE_10,
    CUSTOM_CHORD_ACTIVE_11,
    CUSTOM_CHORD_ACTIVE_12,
    
    CUSTOM_CHORD_NOTE_1,
    CUSTOM_CHORD_NOTE_2,
    CUSTOM_CHORD_NOTE_3,
    CUSTOM_CHORD_NOTE_4,
    CUSTOM_CHORD_NOTE_5,
    CUSTOM_CHORD_NOTE_6,
    CUSTOM_CHORD_NOTE_7,
    CUSTOM_CHORD_NOTE_8,
    CUSTOM_CHORD_NOTE_9,
    CUSTOM_CHORD_NOTE_10,
    CUSTOM_CHORD_NOTE_11,
    CUSTOM_CHORD_NOTE_12,
    
    CUSTOM_CHORD_OCTAVE_1,
    CUSTOM_CHORD_OCTAVE_2,
    CUSTOM_CHORD_OCTAVE_3,
    CUSTOM_CHORD_OCTAVE_4,
    CUSTOM_CHORD_OCTAVE_5,
    CUSTOM_CHORD_OCTAVE_6,
    CUSTOM_CHORD_OCTAVE_7,
    CUSTOM_CHORD_OCTAVE_8,
    CUSTOM_CHORD_OCTAVE_9,
    CUSTOM_CHORD_OCTAVE_10,
    CUSTOM_CHORD_OCTAVE_11,
    CUSTOM_CHORD_OCTAVE_12,
    
    // Amplitude for each custom chord note (0.0 to 1.0)
    CUSTOM_CHORD_AMPLITUDE_1,
    CUSTOM_CHORD_AMPLITUDE_2,
    CUSTOM_CHORD_AMPLITUDE_3,
    CUSTOM_CHORD_AMPLITUDE_4,
    CUSTOM_CHORD_AMPLITUDE_5,
    CUSTOM_CHORD_AMPLITUDE_6,
    CUSTOM_CHORD_AMPLITUDE_7,
    CUSTOM_CHORD_AMPLITUDE_8,
    CUSTOM_CHORD_AMPLITUDE_9,
    CUSTOM_CHORD_AMPLITUDE_10,
    CUSTOM_CHORD_AMPLITUDE_11,
    CUSTOM_CHORD_AMPLITUDE_12,
    
    CHORDPLAYER_SCALE, // normally default but can be changed
    
    CHORD_PLAYER_OUTPUT_SELECTION, // MAX 8 mono, 4 stereo
    
    // Play Configuration parameters
    CHORD_PLAYER_NUM_REPEATS,      // Number of times to repeat each chord (default: 1)
    CHORD_PLAYER_NUM_PAUSE,         // Pause in milliseconds between chords (default: 0)

    TOTAL_NUM_CHORD_PLAYER_SHORTCUT_PARAMS
};

// Chord Scanner Parameters 20
enum CHORD_SCANNER_PARAMS
{
    // internal params for each shortcut
    SPECTRUM_ANALYZER  = 0,
    
    CHORD_SCANNER_MODE, // scan only etc
    
    CHORD_SCANNER_FREQUENCY_FROM,
    CHORD_SCANNER_FREQUENCY_TO,
    
    CHORD_SCANNER_KEYNOTE_FROM,
    CHORD_SCANNER_KEYNOTE_TO,
    
    CHORD_SCANNER_OCTAVE_FROM,
    CHORD_SCANNER_OCTAVE_TO,
    
    CHORD_SCANNER_OCTAVE_EXTENDED,
    
    // synth paramsnumber
    CHORD_SCANNER_WAVEFORM_TYPE,                  // osc type {default, sine, triangle, squ, saw}
    
    CHORD_SCANNER_INSTRUMENT_TYPE,                // sampler enum
    CHORD_SCANNER_ENV_AMPLITUDE,                  // double
    CHORD_SCANNER_ENV_ATTACK,                     // double
    CHORD_SCANNER_ENV_SUSTAIN,                    // double
    CHORD_SCANNER_ENV_DECAY,                      // double
    CHORD_SCANNER_ENV_RELEASE,                    // doubl
    
    // repeats
    CHORD_SCANNER_NUM_REPEATS,                    // num repeats
    CHORD_SCANNER_NUM_PAUSE,                      // enum of millisecond values
    CHORD_SCANNER_NUM_DURATION,                   // duration ms of note downs
    
    CHORD_SCANNER_OUTPUT_SELECTION,
    
    TOTAL_NUM_CHORD_SCANNER_PARAMS
};

// Fundamental Frequency
enum FF_ALGORITHM
{
    TRACK_HARMONICS = 0,           // tracks exact harmonics, equal amplitude
    NEAREST_KEYNOTES,              // transposes harmonic freq to closest note frequency
    NEAREST_CHORD                  // nearest chord based on numHarmonics
};

enum FUNDAMENTAL_FREQUENCY_PARAMS
{
    // internal params for each shortcut
    FUNDAMENTAL_FREQUENCY_FFT_SIZE  = 0,
    
    FUNDAMENTAL_FREQUENCY_FFT_WINDOW,
    
    FUNDAMENTAL_FREQUENCY_INPUT_CHANNEL,
    FUNDAMENTAL_FREQUENCY_OUTPUT_CHANNEL,
    
    NUM_HARMONICS_TO_TRACK,
    
    ALGORITHM, // Freq or Chord, up/down, highest decending, noise reduction,
    
    FUNDAMENTAL_FREQUENCY_CUSTOM_RANGE, // brickwall filter for optimisation and focus
    FUNDAMENTAL_FREQUENCY_MIN_FREQ,
    FUNDAMENTAL_FREQUENCY_MAX_FREQ,

    FUNDAMENTAL_FREQUENCY_INPUT_THRESHOLD, // main harmonic above certain db
    FUNDAMENTAL_FREQUENCY_KEYNOTE_TOLERANCE, // % (in semitones) from keynote
    FUNDAMENTAL_FREQUENCY_MIN_INTERVAL, //
    FUNDAMENTAL_FREQUENCY_MAX_INTERVAL,
    
    // Synth params
    FUNDAMENTAL_FREQUENCY_FREQ_SOURCE,
    FUNDAMENTAL_FREQUENCY_CHOOSE_FREQ,
    FUNDAMENTAL_FREQUENCY_RANGE_MIN,
    FUNDAMENTAL_FREQUENCY_RANGE_MAX,
    FUNDAMENTAL_FREQUENCY_RANGE_LENGTH,
    FUNDAMENTAL_FREQUENCY_RANGE_LOG,
    
    // Manipulate Chosen Frequency
    FUNDAMENTAL_FREQUENCY_MANIPULATE_CHOSEN_FREQUENCY,    // boolean
    FUNDAMENTAL_FREQUENCY_MULTIPLY_OR_DIVISION,           // boolean
    FUNDAMENTAL_FREQUENCY_MULTIPLY_VALUE,                 // double
    FUNDAMENTAL_FREQUENCY_DIVISION_VALUE,
    FUNDAMENTAL_FREQUENCY_WAVEFORM_TYPE,                  // osc type {default, sine, triangle, squ, saw}
    FUNDAMENTAL_FREQUENCY_AMPLITUDE,
    FUNDAMENTAL_FREQUENCY_ATTACK,
    FUNDAMENTAL_FREQUENCY_DECAY,
    FUNDAMENTAL_FREQUENCY_SUSTAIN,
    FUNDAMENTAL_FREQUENCY_RELEASE,
    
    FUNDAMENTAL_FREQUENCY_REPEATER,
    FUNDAMENTAL_FREQUENCY_NUM_PAUSE,
    FUNDAMENTAL_FREQUENCY_NUM_DURATION,
    
    FUNDAMENTAL_FREQUENCY_FFT_ACTIVE,
    FUNDAMENTAL_FREQUENCY_SYNTH_ACTIVE,
    
    FUNDAMENTAL_FREQUENCY_PHASE,
    
    TOTAL_NUM_FUNDAMENTAL_FREQUENCY_PARAMS
};

enum FUNDAMENTAL_FEEDBACK_PARAMS
{
    // internal params for each shortcut
    FUNDAMENTAL_FEEDBACK_FFT_SIZE  = 0,
    
    FUNDAMENTAL_FEEDBACK_FFT_WINDOW,
    
    FUNDAMENTAL_FEEDBACK_INPUT_CHANNEL,
    FUNDAMENTAL_FEEDBACK_OUTPUT_CHANNEL,
    
    FUNDAMENTAL_FEEDBACK_NUM_HARMONICS_TO_TRACK,
    
    FUNDAMENTAL_FEEDBACK_ALGORITHM, // Freq or Chord, up/down, highest decending, noise reduction,
    
    FUNDAMENTAL_FEEDBACK_CUSTOM_RANGE, // brickwall filter for optimisation and focus
    FUNDAMENTAL_FEEDBACK_MIN_FREQ,
    FUNDAMENTAL_FEEDBACK_MAX_FREQ,

    FUNDAMENTAL_FEEDBACK_INPUT_THRESHOLD, // main harmonic above certain db
    FUNDAMENTAL_FEEDBACK_KEYNOTE_TOLERANCE, // % (in semitones) from keynote
    FUNDAMENTAL_FEEDBACK_MIN_INTERVAL, //
    FUNDAMENTAL_FEEDBACK_MAX_INTERVAL,
    
    // Synth params
    FUNDAMENTAL_FEEDBACK_FREQ_SOURCE,
    FUNDAMENTAL_FEEDBACK_CHOOSE_FREQ,
    FUNDAMENTAL_FEEDBACK_RANGE_MIN,
    FUNDAMENTAL_FEEDBACK_RANGE_MAX,
    FUNDAMENTAL_FEEDBACK_RANGE_LENGTH,
    FUNDAMENTAL_FEEDBACK_RANGE_LOG,
    
    // Manipulate Chosen Frequency
    FUNDAMENTAL_FEEDBACK_MANIPULATE_CHOSEN_FREQUENCY,    // boolean
    FUNDAMENTAL_FEEDBACK_MULTIPLY_OR_DIVISION,           // boolean
    FUNDAMENTAL_FEEDBACK_MULTIPLY_VALUE,                 // double
    FUNDAMENTAL_FEEDBACK_DIVISION_VALUE,
    FUNDAMENTAL_FEEDBACK_WAVEFORM_TYPE,                  // osc type {default, sine, triangle, squ, saw}
    FUNDAMENTAL_FEEDBACK_AMPLITUDE,
    FUNDAMENTAL_FEEDBACK_ATTACK,
    FUNDAMENTAL_FEEDBACK_DECAY,
    FUNDAMENTAL_FEEDBACK_SUSTAIN,
    FUNDAMENTAL_FEEDBACK_RELEASE,
    
    FUNDAMENTAL_FEEDBACK_REPEATER,
    FUNDAMENTAL_FEEDBACK_NUM_PAUSE,
    FUNDAMENTAL_FEEDBACK_NUM_DURATION,
    
    FUNDAMENTAL_FEEDBACK_FFT_ACTIVE,
    FUNDAMENTAL_FEEDBACK_SYNTH_ACTIVE,
    
    FUNDAMENTAL_FEEDBACK__PHASE,
    
    TOTAL_NUM_FUNDAMENTAL_FEEDBACK_PARAMS
};

enum SCANNER_MODE
{
    SCAN_MAIN_CHORDS = 0,
    SCAN_ALL_CHORDS,
    SCAN_SPECIFIC_RANGE,
    SCAN_BY_FREQUENCY
};

// Chord Scanner Parameters 18
enum FREQUENCY_SCANNER_PARAMS
{
    // internal params for each shortcut
    FREQUENCY_SCANNER_SPECTRUM_ANALYZER  = 0,
    
    FREQUENCY_SCANNER_MODE, // scan only etc
    
    FREQUENCY_SCANNER_FREQUENCY_FROM,
    FREQUENCY_SCANNER_FREQUENCY_TO,
    
    FREQUENCY_SCANNER_EXTENDED,
    
    // synth params
    FREQUENCY_SCANNER_WAVEFORM_TYPE,                  // osc type {default, sine, triangle, squ, saw}
    
    FREQUENCY_SCANNER_ENV_AMPLITUDE,                  // double
    FREQUENCY_SCANNER_ENV_ATTACK,                     // double
    FREQUENCY_SCANNER_ENV_SUSTAIN,                    // double
    FREQUENCY_SCANNER_ENV_DECAY,                      // double
    FREQUENCY_SCANNER_ENV_RELEASE,                    // doubl
    
    // repeats
    FREQUENCY_SCANNER_NUM_REPEATS,
    FREQUENCY_SCANNER_NUM_PAUSE,                      // enum of millisecond values
    FREQUENCY_SCANNER_NUM_DURATION,                   // duration ms of note downs
    
    FREQUENCY_SCANNER_LOG_LIN,
    FREQUENCY_SCANNER_LOG_VALUE,
    FREQUENCY_SCANNER_LIN_VALUE,
    
    FREQUENCY_SCANNER_OUTPUT_SELECTION,
    
    TOTAL_NUM_FREQUENCY_SCANNER_PARAMS
};

enum FREQUENCY_SCANNER_MODE
{
    FREQUENCY_SCAN_ALL_FREQUENCIES = 0,
    FREQUENCY_SCAN_SPECIFIC_RANGE
};

// Chord Scanner Parameters 23
enum FREQUENCY_PLAYER_SHORTCUT_PARAMS
{
    // internal params for each shortcut// internal params for each shortcut
    FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE  = 0,
    FREQUENCY_PLAYER_SHORTCUT_MUTE,
    FREQUENCY_PLAYER_SHORTCUT_PLAY_AT_SAME_TIME,
    
    FREQUENCY_PLAYER_FREQ_SOURCE,
    FREQUENCY_PLAYER_CHOOSE_FREQ,
    FREQUENCY_PLAYER_RANGE_MIN,
    FREQUENCY_PLAYER_RANGE_MAX,
    FREQUENCY_PLAYER_RANGE_LENGTH,
    FREQUENCY_PLAYER_RANGE_LOG,
    
    // Manipulate Chosen Frequency
    FREQUENCY_PLAYER_MANIPULATE_CHOSEN_FREQUENCY,    // boolean
    FREQUENCY_PLAYER_MULTIPLY_OR_DIVISION,           // boolean
    FREQUENCY_PLAYER_MULTIPLY_VALUE,                 // double
    FREQUENCY_PLAYER_DIVISION_VALUE,                 // double
    
    // synth params
    FREQUENCY_PLAYER_WAVEFORM_TYPE,                  // osc type {default, sine, triangle, squ, saw}
    FREQUENCY_PLAYER_AMPLITUDE,
    FREQUENCY_PLAYER_ATTACK,
    FREQUENCY_PLAYER_DECAY,
    FREQUENCY_PLAYER_SUSTAIN,
    FREQUENCY_PLAYER_RELEASE,
    // repeats
    FREQUENCY_PLAYER_NUM_REPEATS,                    // enum of millisecond values
    FREQUENCY_PLAYER_NUM_PAUSE,                      // enum of millisecond values
    FREQUENCY_PLAYER_NUM_DURATION,                   // duration ms of note downs
    
    FREQUENCY_PLAYER_OUTPUT_SELECTION,
    
    TOTAL_NUM_FREQUENCY_PLAYER_SHORTCUT_PARAMS
};

enum LISSAJOUSE_CURVE_PARAMS
{
    UNIT_1_PROCESSOR_TYPE = 0, // Frequency Player || ChordPlayer
    UNIT_2_PROCESSOR_TYPE,
    UNIT_3_PROCESSOR_TYPE,
    
    UNIT_1_FREE_FLOW, // Open Env or sequencer trigger boolean
    UNIT_2_FREE_FLOW,
    UNIT_3_FREE_FLOW,
    
    UNIT_1_PHASE, // Open Env or sequencer trigger boolean
    UNIT_2_PHASE,
    UNIT_3_PHASE,
    
    AMPLITUDE_X,
    AMPLITUDE_Y,
    AMPLITUDE_Z,
    
    UNIT_1_FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE,
    UNIT_1_FREQUENCY_PLAYER_SHORTCUT_MUTE,
    UNIT_1_FREQUENCY_PLAYER_SHORTCUT_PLAY_AT_SAME_TIME,
    
    UNIT_1_FREQUENCY_PLAYER_FREQ_SOURCE,
    UNIT_1_FREQUENCY_PLAYER_CHOOSE_FREQ,
    UNIT_1_FREQUENCY_PLAYER_RANGE_MIN,
    UNIT_1_FREQUENCY_PLAYER_RANGE_MAX,
    UNIT_1_FREQUENCY_PLAYER_RANGE_LENGTH,
    UNIT_1_FREQUENCY_PLAYER_RANGE_LOG,
    
    // Manipulate Chosen Frequency
    UNIT_1_FREQUENCY_PLAYER_MANIPULATE_CHOSEN_FREQUENCY,    // boolean
    UNIT_1_FREQUENCY_PLAYER_MULTIPLY_OR_DIVISION,           // boolean
    UNIT_1_FREQUENCY_PLAYER_MULTIPLY_VALUE,                 // double
    UNIT_1_FREQUENCY_PLAYER_DIVISION_VALUE,                 // double
    
    // synth params
    UNIT_1_FREQUENCY_PLAYER_WAVEFORM_TYPE,                  // osc type {default, sine, triangle, squ, saw}
    UNIT_1_FREQUENCY_PLAYER_AMPLITUDE,
    UNIT_1_FREQUENCY_PLAYER_ATTACK,
    UNIT_1_FREQUENCY_PLAYER_DECAY,
    UNIT_1_FREQUENCY_PLAYER_SUSTAIN,
    UNIT_1_FREQUENCY_PLAYER_RELEASE,
    // repeats
    UNIT_1_FREQUENCY_PLAYER_NUM_REPEATS,                    // enum of millisecond values
    UNIT_1_FREQUENCY_PLAYER_NUM_PAUSE,                      // enum of millisecond values
    UNIT_1_FREQUENCY_PLAYER_NUM_DURATION,                   // duration ms of note downs
    
    // Unit 2
    
    UNIT_2_FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE,
    UNIT_2_FREQUENCY_PLAYER_SHORTCUT_MUTE,
    UNIT_2_FREQUENCY_PLAYER_SHORTCUT_PLAY_AT_SAME_TIME,
    
    UNIT_2_FREQUENCY_PLAYER_FREQ_SOURCE,
    UNIT_2_FREQUENCY_PLAYER_CHOOSE_FREQ,
    UNIT_2_FREQUENCY_PLAYER_RANGE_MIN,
    UNIT_2_FREQUENCY_PLAYER_RANGE_MAX,
    UNIT_2_FREQUENCY_PLAYER_RANGE_LENGTH,
    UNIT_2_FREQUENCY_PLAYER_RANGE_LOG,
    
    // Manipulate Chosen Frequency
    UNIT_2_FREQUENCY_PLAYER_MANIPULATE_CHOSEN_FREQUENCY,    // boolean
    UNIT_2_FREQUENCY_PLAYER_MULTIPLY_OR_DIVISION,           // boolean
    UNIT_2_FREQUENCY_PLAYER_MULTIPLY_VALUE,                 // double
    UNIT_2_FREQUENCY_PLAYER_DIVISION_VALUE,                 // double
    
    // synth params
    UNIT_2_FREQUENCY_PLAYER_WAVEFORM_TYPE,                  // osc type {default, sine, triangle, squ, saw}
    UNIT_2_FREQUENCY_PLAYER_AMPLITUDE,
    UNIT_2_FREQUENCY_PLAYER_ATTACK,
    UNIT_2_FREQUENCY_PLAYER_DECAY,
    UNIT_2_FREQUENCY_PLAYER_SUSTAIN,
    UNIT_2_FREQUENCY_PLAYER_RELEASE,
    // repeats
    UNIT_2_FREQUENCY_PLAYER_NUM_REPEATS,                    // enum of millisecond values
    UNIT_2_FREQUENCY_PLAYER_NUM_PAUSE,                      // enum of millisecond values
    UNIT_2_FREQUENCY_PLAYER_NUM_DURATION,                   // duration ms of note downs
    
    // Unit 3
    
    UNIT_3_FREQUENCY_PLAYER_SHORTCUT_IS_ACTIVE,
    UNIT_3_FREQUENCY_PLAYER_SHORTCUT_MUTE,
    UNIT_3_FREQUENCY_PLAYER_SHORTCUT_PLAY_AT_SAME_TIME,
    
    UNIT_3_FREQUENCY_PLAYER_FREQ_SOURCE,
    UNIT_3_FREQUENCY_PLAYER_CHOOSE_FREQ,
    UNIT_3_FREQUENCY_PLAYER_RANGE_MIN,
    UNIT_3_FREQUENCY_PLAYER_RANGE_MAX,
    UNIT_3_FREQUENCY_PLAYER_RANGE_LENGTH,
    UNIT_3_FREQUENCY_PLAYER_RANGE_LOG,
    
    // Manipulate Chosen Frequency
    UNIT_3_FREQUENCY_PLAYER_MANIPULATE_CHOSEN_FREQUENCY,    // boolean
    UNIT_3_FREQUENCY_PLAYER_MULTIPLY_OR_DIVISION,           // boolean
    UNIT_3_FREQUENCY_PLAYER_MULTIPLY_VALUE,                 // double
    UNIT_3_FREQUENCY_PLAYER_DIVISION_VALUE,                 // double
    
    // synth params
    UNIT_3_FREQUENCY_PLAYER_WAVEFORM_TYPE,                  // osc type {default, sine, triangle, squ, saw}
    UNIT_3_FREQUENCY_PLAYER_AMPLITUDE,
    UNIT_3_FREQUENCY_PLAYER_ATTACK,
    UNIT_3_FREQUENCY_PLAYER_DECAY,
    UNIT_3_FREQUENCY_PLAYER_SUSTAIN,
    UNIT_3_FREQUENCY_PLAYER_RELEASE,
    // repeats
    UNIT_3_FREQUENCY_PLAYER_NUM_REPEATS,                    // enum of millisecond values
    UNIT_3_FREQUENCY_PLAYER_NUM_PAUSE,                      // enum of millisecond values
    UNIT_3_FREQUENCY_PLAYER_NUM_DURATION,                   // duration ms of note downs
    
    // Chordplayer unit 1
    
    UNIT_1_CHORD_PLAYER_SHORTCUT_IS_ACTIVE,
    UNIT_1_CHORD_PLAYER_SHORTCUT_MUTE,
    UNIT_1_CHORD_PLAYER_SHORTCUT_PLAY_AT_SAME_TIME,     // not in use, but will leave
    
    // synth params
    UNIT_1_CHORD_PLAYER_WAVEFORM_TYPE,                  // osc type {default, sine, triangle, squ, saw}
    
    UNIT_1_CHORD_PLAYER_INSTRUMENT_TYPE,                // sampler enum
    UNIT_1_CHORD_PLAYER_ENV_AMPLITUDE,                  // double
    UNIT_1_CHORD_PLAYER_ENV_ATTACK,                     // double
    UNIT_1_CHORD_PLAYER_ENV_SUSTAIN,                    // double
    UNIT_1_CHORD_PLAYER_ENV_DECAY,                      // double
    UNIT_1_CHORD_PLAYER_ENV_RELEASE,                    // double
    
    UNIT_1_CHORD_PLAYER_CHORD_SOURCE,                   // boolean - list or frequency
    // frequency
    UNIT_1_CHORD_PLAYER_INSERT_FREQUENCY,               // double frequency
    UNIT_1_CHORD_PLAYER_OCTAVE,
    
    
    // Manipulate Chosen Frequency
    UNIT_1_CHORD_PLAYER_MANIPULATE_CHOSEN_FREQUENCY,    // boolean
    UNIT_1_CHORD_PLAYER_MULTIPLY_OR_DIVISION,           // boolean
    UNIT_1_CHORD_PLAYER_MULTIPLY_VALUE,                 // double
    UNIT_1_CHORD_PLAYER_DIVISION_VALUE,                 // double
    
    // repeats
    UNIT_1_CHORD_PLAYER_NUM_REPEATS,                    // enum of millisecond values
    UNIT_1_CHORD_PLAYER_NUM_PAUSE,                      // enum of millisecond values
    UNIT_1_CHORD_PLAYER_NUM_DURATION,                   // duration ms of note downs
    
    // chord params
    
    // list
    UNIT_1_CHORD_PLAYER_KEYNOTE,                        // enum// enum
    UNIT_1_CHORD_PLAYER_CHORD_TYPE,                     // enum
    UNIT_1_CHORD_PLAYER_ADD_ONS,                        // enum
    
    UNIT_1_CHORD_PLAYER_CUSTOM_CHORD,                   // boolean
    
    UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_1,
    UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_2,
    UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_3,
    UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_4,
    UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_5,
    UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_6,
    UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_7,
    UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_8,
    UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_9,
    UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_10,
    UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_11,
    UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_12,
    
    UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_1,
    UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_2,
    UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_3,
    UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_4,
    UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_5,
    UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_6,
    UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_7,
    UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_8,
    UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_9,
    UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_10,
    UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_11,
    UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_NOTE_12,
    
    UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_1,
    UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_2,
    UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_3,
    UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_4,
    UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_5,
    UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_6,
    UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_7,
    UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_8,
    UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_9,
    UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_10,
    UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_11,
    UNIT_1_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_12,
    
    UNIT_1_CHORD_PLAYER_CHORDPLAYER_SCALE, // normally default but can be changed
    
    // unit 2
    
    UNIT_2_CHORD_PLAYER_SHORTCUT_IS_ACTIVE,
    UNIT_2_CHORD_PLAYER_SHORTCUT_MUTE,
    UNIT_2_CHORD_PLAYER_SHORTCUT_PLAY_AT_SAME_TIME,     // not in use, but will leave
    
    // synth params
    UNIT_2_CHORD_PLAYER_WAVEFORM_TYPE,                  // osc type {default, sine, triangle, squ, saw}

    UNIT_2_CHORD_PLAYER_INSTRUMENT_TYPE,                // sampler enum
    UNIT_2_CHORD_PLAYER_ENV_AMPLITUDE,                  // double
    UNIT_2_CHORD_PLAYER_ENV_ATTACK,                     // double
    UNIT_2_CHORD_PLAYER_ENV_SUSTAIN,                    // double
    UNIT_2_CHORD_PLAYER_ENV_DECAY,                      // double
    UNIT_2_CHORD_PLAYER_ENV_RELEASE,                    // double
    
    UNIT_2_CHORD_PLAYER_CHORD_SOURCE,                   // boolean - list or frequency
    // frequency
    UNIT_2_CHORD_PLAYER_INSERT_FREQUENCY,               // double frequency
    UNIT_2_CHORD_PLAYER_OCTAVE,
    
    
    // Manipulate Chosen Frequency
    UNIT_2_CHORD_PLAYER_MANIPULATE_CHOSEN_FREQUENCY,    // boolean
    UNIT_2_CHORD_PLAYER_MULTIPLY_OR_DIVISION,           // boolean
    UNIT_2_CHORD_PLAYER_MULTIPLY_VALUE,                 // double
    UNIT_2_CHORD_PLAYER_DIVISION_VALUE,                 // double
    
    // repeats
    UNIT_2_CHORD_PLAYER_NUM_REPEATS,                    // enum of millisecond values
    UNIT_2_CHORD_PLAYER_NUM_PAUSE,                      // enum of millisecond values
    UNIT_2_CHORD_PLAYER_NUM_DURATION,                   // duration ms of note downs

    // chord params
    
    // list
    UNIT_2_CHORD_PLAYER_KEYNOTE,                        // enum// enum
    UNIT_2_CHORD_PLAYER_CHORD_TYPE,                     // enum
    UNIT_2_CHORD_PLAYER_ADD_ONS,                        // enum
    
    UNIT_2_CHORD_PLAYER_CUSTOM_CHORD,                   // boolean

    UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_1,
    UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_2,
    UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_3,
    UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_4,
    UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_5,
    UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_6,
    UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_7,
    UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_8,
    UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_9,
    UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_10,
    UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_11,
    UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_12,
    
    UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_1,
    UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_2,
    UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_3,
    UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_4,
    UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_5,
    UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_6,
    UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_7,
    UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_8,
    UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_9,
    UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_10,
    UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_11,
    UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_NOTE_12,
    
    UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_1,
    UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_2,
    UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_3,
    UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_4,
    UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_5,
    UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_6,
    UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_7,
    UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_8,
    UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_9,
    UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_10,
    UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_11,
    UNIT_2_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_12,

    UNIT_2_CHORD_PLAYER_CHORDPLAYER_SCALE, // normally default but can be changed
    
    
    // unit 3
    
    UNIT_3_CHORD_PLAYER_SHORTCUT_IS_ACTIVE,
    UNIT_3_CHORD_PLAYER_SHORTCUT_MUTE,
    UNIT_3_CHORD_PLAYER_SHORTCUT_PLAY_AT_SAME_TIME,     // not in use, but will leave
    
    // synth params
    UNIT_3_CHORD_PLAYER_WAVEFORM_TYPE,                  // osc type {default, sine, triangle, squ, saw}

    UNIT_3_CHORD_PLAYER_INSTRUMENT_TYPE,                // sampler enum
    UNIT_3_CHORD_PLAYER_ENV_AMPLITUDE,                  // double
    UNIT_3_CHORD_PLAYER_ENV_ATTACK,                     // double
    UNIT_3_CHORD_PLAYER_ENV_SUSTAIN,                    // double
    UNIT_3_CHORD_PLAYER_ENV_DECAY,                      // double
    UNIT_3_CHORD_PLAYER_ENV_RELEASE,                    // double
    
    UNIT_3_CHORD_PLAYER_CHORD_SOURCE,                   // boolean - list or frequency
    // frequency
    UNIT_3_CHORD_PLAYER_INSERT_FREQUENCY,               // double frequency
    UNIT_3_CHORD_PLAYER_OCTAVE,
    
    
    // Manipulate Chosen Frequency
    UNIT_3_CHORD_PLAYER_MANIPULATE_CHOSEN_FREQUENCY,    // boolean
    UNIT_3_CHORD_PLAYER_MULTIPLY_OR_DIVISION,           // boolean
    UNIT_3_CHORD_PLAYER_MULTIPLY_VALUE,                 // double
    UNIT_3_CHORD_PLAYER_DIVISION_VALUE,                 // double
    
    // repeats
    UNIT_3_CHORD_PLAYER_NUM_REPEATS,                    // enum of millisecond values
    UNIT_3_CHORD_PLAYER_NUM_PAUSE,                      // enum of millisecond values
    UNIT_3_CHORD_PLAYER_NUM_DURATION,                   // duration ms of note downs

    // chord params
    
    // list
    UNIT_3_CHORD_PLAYER_KEYNOTE,                        // enum// enum
    UNIT_3_CHORD_PLAYER_CHORD_TYPE,                     // enum
    UNIT_3_CHORD_PLAYER_ADD_ONS,                        // enum
    
    UNIT_3_CHORD_PLAYER_CUSTOM_CHORD,                   // boolean

    UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_1,
    UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_2,
    UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_3,
    UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_4,
    UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_5,
    UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_6,
    UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_7,
    UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_8,
    UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_9,
    UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_10,
    UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_11,
    UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_ACTIVE_12,

    UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_1,
    UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_2,
    UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_3,
    UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_4,
    UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_5,
    UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_6,
    UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_7,
    UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_8,
    UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_9,
    UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_10,
    UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_11,
    UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_NOTE_12,
    
    UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_1,
    UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_2,
    UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_3,
    UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_4,
    UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_5,
    UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_6,
    UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_7,
    UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_8,
    UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_9,
    UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_10,
    UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_11,
    UNIT_3_CHORD_PLAYER_CUSTOM_CHORD_OCTAVE_12,

    UNIT_3_CHORD_PLAYER_CHORDPLAYER_SCALE, // normally default but can be changed
    
    
    TOTAL_NUM_LISSAJOUS_CURVE_PARAMS
};

enum FEEDBACK_MODULE_PARAMS
{
    
    TOTAL_NUM_FEEDBACK_MODULE_PARAMS
    
};

enum FREQUENCY_TO_LIGHT_PARAMS
{
    FREQUENCY_LIGHT_CONVERSION_SOURCE = 0,
    
    FREQUENCY_LIGHT_KEYNOTE,
    FREQUENCY_LIGHT_CHORDTYPE,
    
    FREQUENCY_LIGHT_FREQUENCY_UNIT,
    FREQUENCY_LIGHT_WAVELENGTH_UNIT,
    
    FREQUENCY_LIGHT_FREQUENCY, // in Hz
    FREQUENCY_LIGHT_WAVELENGTH, // In Nm
    FREQUENCY_LIGHT_PHASESPEED,
    
    FREQUENCY_LIGHT_MANIPULATE_CHOSEN_FREQUENCY1,    // boolean
    FREQUENCY_LIGHT_MULTIPLY_OR_DIVISION1,           // boolean
    FREQUENCY_LIGHT_MULTIPLY_VALUE1,                 // double
    FREQUENCY_LIGHT_DIVISION_VALUE1,
    
    FREQUENCY_LIGHT_MANIPULATE_CHOSEN_FREQUENCY2,    // boolean
    FREQUENCY_LIGHT_MULTIPLY_OR_DIVISION2,           // boolean
    FREQUENCY_LIGHT_MULTIPLY_VALUE2,                 // double
    FREQUENCY_LIGHT_DIVISION_VALUE2,
    
    FREQUENCY_LIGHT_MANIPULATE_CHOSEN_FREQUENCY3,    // boolean
    FREQUENCY_LIGHT_MULTIPLY_OR_DIVISION3,           // boolean
    FREQUENCY_LIGHT_MULTIPLY_VALUE3,                 // double
    FREQUENCY_LIGHT_DIVISION_VALUE3,
    
    FREQUENCY_LIGHT_MANIPULATE_CHOSEN_FREQUENCY4,    // boolean
    FREQUENCY_LIGHT_MULTIPLY_OR_DIVISION4,           // boolean
    FREQUENCY_LIGHT_MULTIPLY_VALUE4,                 // double
    FREQUENCY_LIGHT_DIVISION_VALUE4,
    
    FREQUENCY_LIGHT_MANIPULATE_CHOSEN_FREQUENCY5,    // boolean
    FREQUENCY_LIGHT_MULTIPLY_OR_DIVISION5,           // boolean
    FREQUENCY_LIGHT_MULTIPLY_VALUE5,                 // double
    FREQUENCY_LIGHT_DIVISION_VALUE5,
    
    FREQUENCY_LIGHT_COLOUR_SLIDER_VALUE,
    
    TOTAL_NUM_FREQUENCY_LIGHT_PARAMS
};

enum WAVELENGTH_UNITS
{
        NANOMETER = 1,
        MICROMETER,
        MILLIMETER,
        CENTIMETER,
        METER
};
    
enum FREQUENCY_UNITS
{
    HZ = 1,
    KHZ,
    MHZ,
    GHZ,
    THZ
};

enum PHASE_SPEED_UNITS
{
    SPEED_LIGHT_IN_VACUUM = 1,
    SPEED_LIGHT_IN_WATER,
    SPEED_SOUND_IN_VACUUM,
    SPEED_SOUND_IN_WATER,
    
    NUM_PHASE_SPEED_UNITS
};

#define KHZ_FACTOR 0.001
#define MHZ_FACTOR 0.000001
#define GHZ_FACTOR 0.000000001
#define THZ_FACTOR 0.000000000001

// constants for phase speeds in meters per second
#define SPEEDLIGHTINVACUUM 299792458
#define SPEEDLIGHTINWATER 225000000
#define SPEEDSOUNDINVACUUM 331.5
#define SPEEDSOUNDINWATER 1498

// factors from m/s
#define CENTIMETER_FACTOR 0.0000001
#define MILLIMETER_FACTOR 0.000001
#define MICROMETER_FACTOR 0.001
#define NANOMETRE_FACTOR  1
#define METER_FACTOR 0.000000001


// String functions
enum SCALES_UNIT
{
    SHORTCUT_1 = 0, SHORTCUT_2,SHORTCUT_3,SHORTCUT_4,SHORTCUT_5,SHORTCUT_6,SHORTCUT_7,SHORTCUT_8,SHORTCUT_9,SHORTCUT_10,
    
    MAIN_SCALE,
    LISSAJOUS_SCALE
};


enum STRINGARRAYS
{
    STRING_ARRAY_KEYNOTE = 0,
    STRING_ARRAY_CHORDTYPE,
    STRING_ARRAY_OCTAVE,
    STRING_ARRAY_ADDONS,
    STRING_ARRAY_INSTRUMENTS,
    STRING_ARRAY_SHORTCUTS,
    STRING_ARRAY_WAVETYPES
};

class ProjectStrings
{
public:
//    static StringArray getKeynoteArray()
//    {
//        return StringArray("C", "C#",
//        "D", "D#",
//        "E",
//        "F", "F#",
//        "G", "G#",
//        "A", "A#",
//        "B"
//);
//    }
    
        static StringArray getKeynoteArray()
        {
            return StringArray("C", "C#",
            "D", "D#",
            "E",
            "F", "F#",
            "G", "G#",
            "A", "A#",
            "B", "C", "C#",
            "D", "D#",
            "E",
            "F", "F#",
            "G", "G#",
            "A", "A#",
            "B", "C", "C#",
            "D", "D#",
            "E",
            "F", "F#",
            "G", "G#",
            "A", "A#",
            "B" );
        }

    static StringArray getChordTypeArray()
    {
        
        return StringArray("Major",
        "Minor",
        "Augmented",
        "Diminished",
        "Suspended 4th",
        "Suspended 2nd",
        "5th",
        "6th",
        "6th Minor",
        "7th",
        "7th Major",
        "7th Minor",
        "7th Diminished",
        "7th Half Diminished",
        "7th #5",
        "9th",
        "7th #9",
        "9th Major",
        "9th Minor",
        "9th Added",
        "9th Minor Added",
        "11th",
        "11th Minor",
        "7th #11",
        "Major 7th #11",
        "13th",
        "Major 13th",
        "Minor 13th"
        );
    }
    
    static StringArray getOctaveArray()
    {
        return StringArray("0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10");
        
//        return StringArray("-2", "-1", "0", "1", "2", "3", "4", "5", "6", "7", "8");
    }
    
    static StringArray getOctaveArrayExtended(bool extended)
    {
        if (extended)
        {
            return StringArray("0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16");
        }
        else
            return StringArray("0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10");

    }
    
    static StringArray getAddons()
    {
        return StringArray("6", "7", "7 Major", "9", "9b", "9#", "11", "11#", "13", "13b", "Inverted");
    }
    
    static StringArray getShortcuts()
    {
        return StringArray("A","S","D","F","G","H","J","K","L","Z","X","C","V","B","N","M", "Q", "W", "E", "R", "T", "Y");
    }
    
    static StringArray getWavetypes()
    {
        return StringArray("Sampler","Sinewave","Triangle","Squarewave","Sawtooth", "Wavetable");
    }

    static StringArray getFundamentalFrequencyAlgorithms()
    {
        return {"Track Harmonics", "Yin", "NPM"};
    }
};

