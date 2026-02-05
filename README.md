# The Sound Studio

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform](https://img.shields.io/badge/Platform-macOS%20%7C%20Windows%20%7C%20Linux-blue.svg)](#supported-platforms)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/)
[![JUCE](https://img.shields.io/badge/JUCE-8.0+-orange.svg)](https://juce.com/)

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

## Supported Platforms

| Platform | Status | Build System |
|----------|--------|--------------|
| macOS 10.13+ | Fully Supported | Xcode |
| Windows 10+ | Supported | Visual Studio 2019+ |
| Linux (Ubuntu 20.04+) | Supported | Make |

## Prerequisites

### Install JUCE Framework

JUCE is required to build The Sound Studio. Choose one of these methods:

#### Option 1: Download from JUCE website (Recommended)
1. Go to [https://juce.com/download](https://juce.com/download)
2. Download JUCE 8.0 or later
3. Extract to a location (e.g., `~/JUCE` on macOS/Linux or `C:\JUCE` on Windows)

#### Option 2: Clone from GitHub
```bash
git clone https://github.com/juce-framework/JUCE.git
cd JUCE
git checkout master  # or a specific version tag like 8.0.0
```

#### Option 3: Package Manager

**macOS (Homebrew):**
```bash
brew install juce
```

**Linux (Ubuntu/Debian):**
```bash
# Install dependencies first
sudo apt-get update
sudo apt-get install -y \
    libasound2-dev libjack-jackd2-dev ladspa-sdk libcurl4-openssl-dev \
    libfreetype6-dev libx11-dev libxcomposite-dev libxcursor-dev \
    libxext-dev libxinerama-dev libxrandr-dev libxrender-dev \
    libwebkit2gtk-4.0-dev libglu1-mesa-dev mesa-common-dev

# Then download JUCE from https://juce.com/download
```

### Platform-Specific Requirements

#### macOS
- macOS 10.13 (High Sierra) or later
- Xcode 14+ (Xcode 15+ recommended)
- Command Line Tools: `xcode-select --install`

#### Windows
- Windows 10 or later
- Visual Studio 2019 or later (with C++ Desktop Development workload)
- Or Visual Studio Build Tools

#### Linux
- GCC 9+ or Clang 10+
- Development libraries (see package manager commands above)

## Building

### 1. Clone the repository
```bash
git clone https://github.com/ASVPR/the-sound-studio.git
cd the-sound-studio
```

### 2. Configure JUCE path

Open `Codebase/TheSoundStudio/TheSoundStudio.jucer` in JUCE Projucer and:
1. Set your JUCE modules path in **Global Paths**
2. Click **Save and Open in IDE**

Or set the environment variable:
```bash
export JUCE_DIR=/path/to/JUCE  # macOS/Linux
set JUCE_DIR=C:\JUCE           # Windows
```

### 3. Build

#### macOS
```bash
cd Codebase/TheSoundStudio/Builds/MacOSX
xcodebuild -project "The Sound Studio.xcodeproj" \
  -scheme "The Sound Studio - App" \
  -configuration Release
```

#### Windows (Visual Studio)
1. Open `Codebase/TheSoundStudio/TheSoundStudio.jucer` in Projucer
2. Add a Visual Studio exporter if not present (File → Export Targets)
3. Click **Save and Open in IDE**
4. In Visual Studio, select **Release** configuration
5. Build → Build Solution (Ctrl+Shift+B)

#### Linux
```bash
cd Codebase/TheSoundStudio/Builds/LinuxMakefile
make CONFIG=Release -j$(nproc)
```

## Project Structure

```
The Sound Studio/
├── Codebase/TheSoundStudio/
│   ├── Source/              # Application source code
│   │   ├── Main.cpp         # Application entry point
│   │   ├── UI/              # Modern UI components
│   │   ├── FeedbackModule/  # Real-time feedback processing
│   │   └── ...
│   ├── Builds/
│   │   ├── MacOSX/          # Xcode project
│   │   ├── VisualStudio2022/# Visual Studio project
│   │   └── LinuxMakefile/   # Linux Makefile
│   └── TheSoundStudio.jucer # Projucer project file
├── Assets/                  # UI assets, images, and fonts
├── Screenshots/             # Application screenshots
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
