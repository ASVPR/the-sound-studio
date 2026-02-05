# Codebase Overview

This document provides a directory and file-level map of the The Sound Studio codebase to help developers navigate the project.

## Core Application
- `Main.cpp`: Application entry point and lifecycle management.
- `MainComponent.h/cpp`: The main container for the UI.
- `MainViewComponent.h/cpp`: Orchestrates the primary modules (Synth, Analysis, Settings).

## Synthesis System
- `SynthesisEngine.h/cpp`: Main entry for the synthesis engine.
- `PhysicalModelingSynth.h/cpp`: Implementation of physical modeling algorithms.
- `KarplusStrongSynth.h/cpp`: Implementation of string-based synthesis.
- `WavetableSynth.h/cpp`: Wavetable-based oscillator system.
- `Oscillators.h/cpp`: Low-level oscillator implementations (Sine, Saw, Square, Noise).
- `ADSR2.h/cpp`: Custom envelope generation.

## Audio Analysis
- `AnalyzerNew.h`: High-performance FFT analysis implementation.
- `EnhancedFFTEngine.h/cpp`: Wraps FFT logic for various visualisers.
- `FrequencyManager.h/cpp`: Manages pitch detection and frequency analysis logic.
- `ChordManager.h/cpp`: Logic for chord detection and generation.

## Visualisation Components
- `SpectrogramComponent.h/cpp`: Multi-mode spectrogram and FFT plotter.
- `ColourSpectrumVisualiserComponent`: (Defined in `SpectrogramComponent.h`) Real-time color-coded frequency mapping.
- `OscilloscopeVisualiserComponent.h/cpp`: Time-domain wave visualization.
- `LissajousCurveComponent.h/cpp`: Phase relationship visualization.
- `SpectraHarmonicsChart.h/cpp`: Harmonic series analysis.
- `StandingWaveSpectrogram.h/cpp`: Physics-based wave visualization.

## Infrastructure & Utilities
- `ProjectManager.h/cpp`: Global state coordination.
- `ProfileManager.h/cpp`: Serialization and persistence of settings.
- `LogFileWriter.h/cpp`: Application logging system.
- `CustomLookAndFeel.h/cpp`: Global UI styling and theme.
- `ResponsiveLayout.h/cpp`: Helpers for fluid UI across different window sizes.

## Third-Party Libraries
- `hiir/`: Laurent de Soras' HIIR library for oversampling and filtering.
- `JuceLibraryCode/`: Generated JUCE glue code and module headers.
