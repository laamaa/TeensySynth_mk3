#ifndef SETTINGS_H
#define SETTINGS_H

// Enable debug messages - 0=off, 1=less, 2=more
#define SYNTH_DEBUG 0

// Amount of polyphony
#define NVOICES 4

// Midi channel
#define SYNTH_MIDICHANNEL 7

// Number of presets
#define PRESETS 16

// Oscillator signal attenuation, happens right after the oscillator. Range = 0.0f-1.0f.
#define OSC_LEVEL 0.6f

// Reference level for mixing signals in master bus. Range isn't limited, but sensible values are 0.2f-0.9f.
#define MIX_LEVEL 0.8f

// How much reverb should be sent to chorus. Range = 0.0f - 1.0f
#define CHORUS_REV_LEVEL 0.4f

// Pre-reverb highpass filter frequency. Range = 0.0f >
#define REV_HIGHPASS 150.0f

// Initial potentiometer update threshold value, to prevent noise from triggering parameter updates
#define POT_THRESHOLD 6

//Constrain macro, borrowed from stmlib
#define CONSTRAIN(var, min, max) \
  if (var < (min)) { \
    var = (min); \
  } else if (var > (max)) { \
    var = (max); \
  }

#endif