# TeensySynth mk3

## Overview
This is a new iteration of my Teensy Polysynth project.
It's still in a very early phase, so there's not many things working yet.

## Features:
Current and planned features

### Current:
* Mutable Instruments Plaits oscillators 
* Moog style filters
* 32-bit floating point audio path (at least until passing the signal to i2s)
* USB Midi in
* Freeverb reverb
* [Chorus effect by quarterturn](https://github.com/quarterturn/teensy3-ensemble-chorus) (ported to Openaudio library)
* Moog style filter from [DragonSifu's Digital Handdrum project](https://github.com/DragonSifu/Digital-Handdrum) (ported to Openaudio library)
* Hardware controls
* Presets
* UART Midi
* OLED Display support

### Planned:
* Full control with MIDI CCs


## Development
The project uses VisualTeensy, build/deploy is done using the tools found in VsCode after creating the project files.
