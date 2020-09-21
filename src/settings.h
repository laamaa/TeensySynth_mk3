#ifndef SETTINGS_H
#define SETTINGS_H

// Enable debug messages - 0=off, 1=less, 2=more
#define SYNTH_DEBUG 2

// Amount of polyphony
#define NVOICES 6

// Midi channel
#define SYNTH_MIDICHANNEL 7

// Number of presets
#define PRESETS 16

// Oscillator signal attenuation, happens right after the oscillator. Range = 0.0f-1.0f.
#define OSC_LEVEL 0.8f

// Initial potentiometer update threshold value, to prevent noise from triggering parameter updates
#define POT_THRESHOLD 5

#endif