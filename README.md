# TeensySynth mk3

## Overview
This is a new iteration of my Teensy Polysynth project.
It's still in a very early phase, so there's not many things working yet.
It can take USB Midi in from channel 7, play some notes and update the filter from HW controls, that's pretty much it for now.

## Features:

### Current:
* Mutable Instruments Plaits oscillators 
* Moog style filters
* 32-bit floating point audio path (at least until passing the signal to i2s)
* USB Midi in

### Planned:
* Hardware controls
* OLED Display support
* Full control with MIDI CCs
* Envelopes
* Presets
* UART Midi
* Chorus effect

## Development
The project uses VisualTeensy, build/deploy is done using the tools found in VsCode after creating the project files.
