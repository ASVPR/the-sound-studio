# The Sound Studio

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform](https://img.shields.io/badge/Platform-macOS-blue.svg)](https://www.apple.com/macos/)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/)
[![JUCE](https://img.shields.io/badge/JUCE-Framework-orange.svg)](https://juce.com/)

A professional audio synthesis and analysis platform built with the [JUCE](https://juce.com/) framework. The Sound Studio replaces traditional sample-based audio with real-time synthesis algorithms, delivering a lightweight application with rich sound design capabilities.

## Features

### Synthesis Engine
- **Physical Modeling Synthesis** — Piano with inharmonic partials, strings with bowed modeling.
- **Karplus-Strong Algorithm** — Guitar, harp, and plucked string instruments.
- **Wavetable Synthesis** — Church organ, synthesizers, and pads.
- **24+ Instruments** — Across categories including Keys, Strings, Wind, Percussion, Ethnic, and Synth.

### Audio Analysis & Visualization
- **Frequency Color Spectrogram** — High-resolution 40-octave frequency-to-light color mapping.
- **Spectra Harmonics Chart** — Real-time harmonic overtone analysis.
- **Standing Wave Spectrogram** — Physics-based wave visualization.
- **Dual Spectrogram Comparison** — Side-by-side analysis with synchronized zoom and pan.
- **Multi-dimensional Lissajous Curves** — 2D through 6D expandable phase visualization.

### Core Capabilities
- Real-time chord playback with multiple voicings and scales.
- Frequency analysis and high-precision pitch detection.
- Multi-format audio recording and session logging.
- Full MIDI input support for external controllers.
- Deeply configurable synthesis parameters (ADSR, filters, modulation).

## Screenshots

| Chord Player Settings | Frequency Player |
|:---:|:---:|
| ![Chord Player Settings](Screenshots/Chord%20Player%20Settings.png) | ![Frequency Player](Screenshots/Frequency%20Player.png) |

## Building

### Requirements
- macOS 10.13+
- Xcode 15+
- JUCE 8.0+
- C++17 compatible compiler

### Build Steps

1. **Clone the repository:**
   ```bash
   git clone https://github.com/ASVPR/the-sound-studio.git
   cd the-sound-studio
   ```

2. **Open the Projucer file:**
   Open `Codebase/TheSoundStudio/TheSoundStudio.jucer` in the JUCE Projucer and click "Save and Open in IDE" to generate the Xcode project.

3. **Build in Xcode:**
   Select the **The Sound Studio - App** scheme and build (⌘B).

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
The Sound Studio/
├── Codebase/TheSoundStudio/
│   ├── Source/              # Application source code
│   │   ├── Main.cpp         # Application entry point
│   │   ├── Synth/           # Synthesis engine components
│   │   ├── UI/              # Modern UI components
│   │   ├── FeedbackModule/  # Real-time feedback processing
│   │   └── ...
│   ├── Builds/MacOSX/       # Xcode project files
│   └── TheSoundStudio.jucer # Projucer project file
├── Assets/                  # UI assets, images, and fonts
├── documentation/           # Technical documentation
└── README.md
```

## Documentation

Technical documentation is available in the [documentation](./documentation) folder:
- [Architecture Overview](./documentation/ARCHITECTURE.md)
- [Codebase Overview](./documentation/CODEBASE_OVERVIEW.md)
- [Changelog](./documentation/CHANGELOG.md)

## Contributing

Contributions are welcome! Please read our [Contributing Guidelines](CONTRIBUTING.md) before submitting a pull request.

## License

This project is licensed under the MIT License — see the [LICENSE](LICENSE) file for details.

Copyright (c) 2026 Ziv Elovitch. All rights reserved.