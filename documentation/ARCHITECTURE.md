# The Sound Studio - Architecture Overview

The Sound Studio (TSS) is a modular audio synthesis and analysis platform built on the **JUCE** framework. It follows a decoupled architecture separating audio processing, data management, and user interface.

## Core Components

### 1. Application Layer (`Main.cpp`)
- **TSSApplication**: Inherits from `juce::JUCEApplication`. Manages the application lifecycle, window creation, and global initialization.
- **MainViewComponent**: The root UI container that hosts the main navigation and sub-modules.

### 2. Coordination Layer (`ProjectManager.h/cpp`)
- **ProjectManager**: The central "brain" of the application. It holds global state, manages audio settings, and coordinates communication between the UI and the synthesis engine.
- Acts as a singleton-like service for various components to access shared resources (FFT data, sample rate, settings).

### 3. Synthesis Engine (`SynthesisEngine.h/cpp`)
- A multi-algorithm synthesis platform support:
    - Physical Modeling
    - Karplus-Strong
    - Wavetable Synthesis
- Managed via `SynthesisProcessor` and individual instrument implementations.

### 4. Analysis & Visualisation
- **AnalyzerPool**: Manages multiple FFT analyzers for different audio sources.
- **VisualiserContainerComponent2**: A flexible layout container that allows users to swap and resize different visualization modules:
    - `SpectrogramComponent`
    - `OscilloscopeComponent`
    - `LissajousCurveViewerComponent`
    - `OctaveVisualiserComponent2`
    - `SpectraHarmonicsChart`

### 5. Data Management
- **ProfileManager**: Handles saving and loading of user presets and application states.
- **RecordingManager**: Manages audio export and recording workflows.

## Design Patterns
- **Listener Pattern**: Extensively used for UI updates (e.g., `ProjectManager::UIListener`).
- **Processor/Editor Split**: Follows JUCE's paradigm for separating audio logic from UI components.
- **Thread Safety**: Uses `juce::CriticalSection` for protecting shared audio buffers and FFT data.

## Module Map
- `Source/`: All C++ source code.
- `Source/hiir/`: Halfband IIR filter library for high-quality audio processing.
- `Source/utilities/`: Generic helper classes for math and data structures.
- `Assets/`: External binary resources (fonts, icons).
