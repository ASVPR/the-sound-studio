# Changelog

All notable changes to The Sound Studio will be documented in this file.

## [1.0.0] - 2026-02-03
### Added
- **Synthesis Engine**: Integrated Additive Synthesis and Convolution-based FFT Impulse Response.
- **Visualizers**: Added Lissajous Curves (2D-6D), Standing Wave Spectrogram, and Spectra Harmonics Chart.
- **Analysis**: Real-time Chord Scanner and Frequency Scanner modules.
- **UI**: Implemented Responsive Layout Manager and Retina screen handling.
- **Recording**: Added Audio Recording and FFT Data export capabilities.

### Fixed
- Resolved memory leaks and crashes in `ColourSpectrumVisualiserComponent` during resizing.
- Optimized FFT processing for lower CPU usage on ARM-64 (Apple Silicon).
- Fixed background grid rendering issues on macOS.

### Improved
- **Spectrogram**: Added side-by-side comparison mode with synced zoom/pan.
- **Lissajous**: Improved performance and added multi-axis support.
- **Chord Player**: Added Custom Chord support with individual note amplitude control.
- **FFT**: Expanded analysis range and improved bin resolution.

## [0.9.0] - 2025-12-15
### Added
- Initial beta release of The Sound Studio.
- Basic Synthesis Engine (Piano, Guitar, Organ).
- Simple Spectrogram and Waveform displays.
- Core Project Management and Profile saving.
- MIDI input support.
