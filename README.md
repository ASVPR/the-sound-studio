# The Sound Studio

A professional audio synthesis and analysis platform built with the [JUCE](https://juce.com/) framework. The Sound Studio replaces traditional sample-based audio with real-time synthesis algorithms, delivering a lightweight application with rich sound design capabilities.

## Features

### Synthesis Engine
- **Physical Modeling Synthesis** — Piano with inharmonic partials, strings with bowed modeling
- **Karplus-Strong Algorithm** — Guitar, harp, and plucked string instruments
- **Wavetable Synthesis** — Church organ, synthesizers, and pads
- **24+ instruments** across six categories (Keys, Strings, Wind, Percussion, Ethnic, Synth)

### Audio Analysis & Visualization
- **Frequency Color Spectrogram** — 40-octave frequency-to-light color mapping
- **Spectra Harmonics Chart** — Real-time harmonic overtone analysis
- **Standing Wave Spectrogram** — Physics-based wave visualization
- **Compare Spectrograms** — Side-by-side analysis with synced zoom and pan
- **Multi-dimensional Lissajous Curves** — 2D through 6D expandable visualization

### Core Capabilities
- Real-time chord playback with multiple voicings
- Frequency analysis and pitch detection
- Audio recording and export
- MIDI input support
- Configurable synthesis parameters (ADSR, filters, modulation)

## Building

### Requirements
- macOS with Xcode
- JUCE framework (included via Projucer project)
- C++17 compatible compiler

### Build Steps

1. Open the Xcode project:
   ```
   Codebase/TheSoundStudio/Builds/MacOSX/The Sound Studio.xcodeproj
   ```

2. Select the **The Sound Studio - App** scheme.

3. Build and run (⌘R) or archive for release.

Alternatively, build from the command line:
```bash
cd Codebase/TheSoundStudio/Builds/MacOSX
xcodebuild -project "The Sound Studio.xcodeproj" \
  -scheme "The Sound Studio - App" \
  -configuration Release \
  CODE_SIGNING_ALLOWED=NO
```

## Project Structure

```
tss/
├── Codebase/TheSoundStudio/
│   ├── Source/              # Application source code
│   │   ├── Main.cpp         # Application entry point
│   │   ├── MainComponent.*  # Root UI component
│   │   ├── Synth*/          # Synthesis engine components
│   │   ├── Spectrogram*/    # Visualization components
│   │   ├── ChordPlayer*/    # Chord playback system
│   │   └── ...
│   ├── Builds/MacOSX/       # Xcode project files
│   └── TheSoundStudio.jucer # Projucer project file
├── Assets/                  # UI assets, images, and fonts
├── Graphics/                # Visual design assets
└── README.md
```

## License

Copyright (c) 2026 Ziv Elovitch. All rights reserved.
