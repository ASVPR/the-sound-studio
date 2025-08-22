/*
  ==============================================================================

    notes.h
    Created: 17 Oct 2019 9:57:21am
    Author:  Gary Jones

  ==============================================================================
*/

#pragma once
/*
 
 
 Logs
 
 full synth, popup, define the synth parameters.
 
 
 
 
 Sprint 4.5

 - 229
 chose pitch detection algorithm paramater
 - 228
 
 - 236
 Inputs
 
 - 222
 positive feedback
 basically pitch detect the input, replicate the frequency to the outputs..
 
 
 
 
 
 
 
 Sprint 4:
 
 Settings
 
 FFT
 165 Add color, delay, zoom to octave, color spectrum
 216 sprint 3 bugs, image in linux not holding..
 193 lissajous .. dont forget color/delay
 210 higher limits - done i think..
 184 Frequency To Chord specifications in settings.. and max peaks etc
 206
 

 

 
 Others
 54 note frequency calc svp-54 + svp 221
 
 Inputs for Realtime Analysis
 
 190 FFT Popouts
 - single module can switch from dropdown..
 Lissajous - visualiser
 
 z axis - fft 3D surface
 
 
 
 
 
 
 
 
 
 
Transducer project
 1. Test Environment
 - in water, spehere containing water, brass or copper.. sphere has unique geometry , focusing into the centre
 - in air, objects such as rock..
 
 - software, creating the frequency generation...
 - analyse the results
 
 - Mayer..
 
 
 2. Objects - size, density etc
 3. Project Sound waves ?
 4. How to measure results ?
 
 
 
 // Spectrum Windows..
 // expand to desktop size or not.. yes..
 // opens unique windows
 // 
 



 // make the gradient / harmonics
 // middle main base frequency, with harmonics extending from
 // labels need to display nm/freq of each note..
 //
 
 inventright.com
 
 
Chord Scanner Bugs
 1. revise background, remove num repeats..
 2. Not playing
 3. bug might be related to +1 Keynote enumeration bugs and others....
 
 
Estimates
 
 1. Chord Player
 - Choose Chord From Frequency
 - spectrogram
 - scales
 
 ** estimate 0
 
 2. Chord Scanner
 - Some bugs
 - Closet Chord Algorithm
 
 ** estiamte 4 hours for bugs
 
 3. Frequency Player
 - Done
 - upgrade spectrum graphics
 
 ** 0
 
 4. Frequency To Light
 
zero 0...
 
 octave... add parameter...
 

 4. Frequency Scanner
 - Done
 - Check Spectrum views
 - works in both ways..
 
 ** 0 hours
 
 5. Realtime Analysis
 - Inputs are ready
 - need GUI layout
 - Enhanced visualisers
 - File button to implement - save log ?
 - realtime log.. peak frequencies, harmonic, db...whatever we can get from the FFT..
 
 ** Gui layout 5 hours
 
 6. Lissajous
 - load wav file / file player
 - Viewer           ** ?
 
 Total 14-16 hours ?
 
 
 Extras
 ----------------------------------------
 ** file access, directories,
 1. Logging
 - whole application
 - parameter changes,
 
 2. Presets
 - Undo Manager
 
 3. Recordings
 - reccording. rut into the directory..
 
 ----------------------------------------
 
 Cross Issues
 
 1. Scales
 ----------------------------------------------------
 - refactoring required.
 
 for simplicity, only chromatic for now....
 need way to change scale.. change in settings, chromatic, dianotic, inharmonic, in settings screen.. ** why ? in chordplayer,,
 added issue.. choose which scale to use, first chord chromatic,, second chord in diatoic scale...
 chord construction.. chromatic, cents.. 1/12 -> ratios are very important.. revise documentations...
 
 ** do chromatic first..
 
 2. Chord to Frequency / Frequency to Chord
 ---------------------------------------------------
 - check algorithm find best solution
 - need estimate
 - final harmonics..
 - central a pitch. a = 432... minimum shift, from central A.. to find base frequency of a chord
 
 3. Spectragrams / FFT
 ---------------------------------------------------
 - harmonograph
 - ?? will be ongoing refinements
 
 - FFT bugs
 - Octave spectrogram, wrong..
 - Colour
 - issue with peak
 - colour
 
 4. Visual Bugs
 --------------------------------------------------
 - a day, but maybe less
 - dropdown menus..
 - size of text..
 - rotary knobs..


 I suggest following flow
 1. Chord Scanner Bugs
 2. Visual Bugs
 
 - deliver new version
 
 3. Layouts (realtime analysis)
 4. FFTs / Lissajous
 5. Scales / Chord Frequency etc
 
 
 // This week...
 
 1. Chord to Frequencies.
 -
 -
 
 
 1.OpenGL
 - spectragrams, oscilloscopes, text dB / freq readings.. maybe some VU meters
 - decide options, maybe use third party solutions as fall back..
 - Lissajous..
 - plugin vst / au..
 - open source / linux solution
 
 2.Notes/Playing Instruments
 - Sample File Player
 - wavetables..
 - SVP-13...
 
 
 
 
 // Add Wavetable to other modules
 
 1. ChordScanner Component
 - Add Synth object
 - redesign front end
 - trigger note on / off
 - panic
 
 *** ChordScanner needs revision. the OCTAVE parameter is being used in the calculation of the chord sequences.. In new frequency setting for synth, sampler and wavetbale, this takes place in triggerNoteOn
 
 2. FrequencyPlayer
 - uses its own synth voice class.. need to go over this, and frequency scanner voice
 and check if 
 
 
 3.
 
 
 
 
 
 
 
 
 
 New Bugs
 
 1. Frequency / Chord Scanner
 - SVP 72 / SVP-93
 - Play / Pause / Stop(reset)
 ** DONE **
 
 3. SVP-73
 - Minimise button
 - Maximum - look into
 ** DONE **
 
 18. SVP 89
 - Decimal Frequency
 ** DONE **
 
 4. Change Application name
 *** DONE ***
 
 13. SVP-84
 - Align the button
 *** DONE ***
 
 2. SVP 83
 - Delete, return parameter to default..
 *** DONE ***
 
 5. SVP-74
 - close delete, return to default
 *** DONE ***
 
 16. SVP-87
 - Chord to Frequency on Chord Player, base note frequency
 *** DONE ***
 
 11. SVP 81
 - No X delete on Custom Chord, might be hidden
 *** DONE *** plus extra bugs
 
 7. SVP-78
 - Instead of Add, Apply..
 *** DONE ***
 
 // ChordScanner - *** DONE ****
 SVP-108, SVP-107, SVP-100, SVP-102, SVP-101, SVP-99
 
 // Settings ** DONE *** SVP-103, SVP-104,
 
 
 // Frequency To Light
 19. SVP-92
 - Divide Multiply back to front..
** - Check if final results hsould be in NM or Thz ?
 
 // ChordPlayer
 
 6. SVP-75
 - re-order upon deletion.
 - maybe swap the objects..
 
 10. SVP-80 *** DONE ***

 

// FFT Visuliasers
 14. SVP-85 *** DONE ***
 
 8. FFT
 - add min/max Frequency
 
 17. SVP-88
 - Screen Lock
 - Will be FFT buffer backlog
 - Check for OSX function to clear memory, or restart, prepare to play after timeout !!!
 


 
 
 
 
 // Other
 20. SVP-86
 - Add to the Session concept.. press record all data is collected from them
 - check Friture for similar feature
 
 
 15. SVP-86 / 94
 - Take Recording
 - Take to Audacity, or run diagnostics on it..
 - Open the Main FFT, Colour Spectrum , Octave.. fixed Moving average static Spectrum FFT ,
 - Export Diagnostics on full recording.
 
 
 21. SVP-53
 - Harmonographc / Chaldi
 
 
 
 12. SVP-82
 - Missing instruments to research and find, Sitar, Tibetan Trumpet, Harmonica
 
 
9. SVP-79
- Check all scales/ Check SVP-95
** do at the end of other issues **
 
 
 
 
 (Sprint 2)

* ** Release a Linux version**
- In addition to Mac version
* Bugs
- All the stuff except the notes and scale
- I'll go on Sunday on all the notes and scales and check the calculations
 
 1.  Realtime analysis
 1.1 SVP-22
 1.2 Additions - SVP-86
 
 2.  Recording
 2.1 SVP-94, SVP-29
 
 3   Logs - SVP-8, SVP-18
 - Please also add an option for logging crushes and problems.

 4. Small Additions
 4.1 Add More Frequency And Chord Boxes
 4.1.1 SVP-96
 
 5. Change Software Name
 5.1 SVP-97
 
 6 Add Tooltip For Input Devices
 6.1 SVP-98
 
 7. Settings - Add Tooltip Next To Scan Plugins
 7.1 See SVP-105

-------------------------------------------------------------------------------------------------------------------------------------------------
(Sprint 3)

1 Lissajous Curves
1.1 See SVP-52
2. Special Spectrograms And Charts
2.1 Only the Harmonograpgh and Chlandi plates
2.1.1 check out the applications attached and SVP-53
2.1.1.1 See examples I've sent on 08.07.20
3. Settings
3.1 See SVP-8
3.1.1 Especially the record option SVP-29
4. Change default chord
4.1 See SVP-70
5 Load/Save
5.1 See SVP-34
6. Chord Player - Add An Option To Play File - Vocoder
6.1 SVP-24, SVP-31
7. Panic button
8.1 See SVP-2, SVP-3 , SVP-6, SVP-35
9. White Noise / Pink Noise
9.1 SVP - 68 or plugin
10. Add Licensing Module - Must be discuses before
10.1 See SVP-111

-------------------------------------------------------------------------------------------------------------------------------------------------
(Sprint 4)
1. Add Oscilloscope
1.1 See SVP-109
2. Set Notes Table and Add Ra Scale - Big task - must be discussed before
2.1 See SVP-106 and SVP-63
3 Presets / Application settings
4. Fundamental Harmonics
4.1 SVP-69
5. Add Note Frequency Calculation On Add Custom Chord
5.1 SVP-54
6. Scales And Chords Extra Layer
7.1 SVP-65
8. Chord to frequency (not the other way around)
8.1 - See SVP-3
9. Optimization
 
 
 
 
 
 
 
 Todays chat..
 
 1. Only in popups. top, slider, textfield to enter min/max, SVP148
 
 
 
 
*/
