#ifndef PATCH_H_
#define PATCH_H_

#include <Arduino.h>

namespace TeensySynth
{
    struct Envelope
    {
        float attack;
        float decay;
        float sustain;
        float release;
    };

    //Struct for storing synth parameters.
    struct Patch
    {
        int engine = 0;                                     // Plaits synth engine number
        float harmonics = 0.0f;                             // Plaits oscillator harmonics parameter
        float timbre = 0.0f;                                // Plaits oscillator timbre parameter
        float morph = 0.0f;                                 // Plaits oscillator morph parameter
        float decay = 1.0f;                                 // Plaits oscillator decay value
        float lpgColour = 0.0f;                             // Plaits oscillator Low Pass colour parameter
        float harmonicsMod = 0.0f;                          // Plaits oscillator harmonics modulation
        float freqMod = 0.0f;                               // Plaits oscillator freq modulation
        float timbreMod = 0.0f;                             // Plaits oscillator timbre modulation
        float morphMod = 0.0f;                              // Plaits oscillator morph modulation
        float volume = 1.0f;                                // Patch volume
        float balance = 0.0f;                               // Balance between main/aux outputs of the Plaits oscillator. 0.0f = main only, 1.0f = aux only
        float filterCutoff = 19200;                         // Master filter cutoff in Hz
        float filterResonance = 1.0f;                       // Master filter resonance
        float filterDrive = 1.0f;                           // Master filter drive
        bool velocityOn = false;                            // velocity enabled
        bool polyOn = true;                                 // polyphonic mode on
        float reverbSize = 0.7f;                            // Master reverb room size
        float reverbDepth = 0.1f;                           // Master reverb amount
        float chorusDepth = MIX_LEVEL;                      // Master chorus amount
        bool useExtEnvelope = false;                        // Use ADSR envelope instead of Plaits' internal
        Envelope ampEnvelope = {7.0f, 100.0f, 0.8f, 20.0f}; // ADSR envelope initial settings
        uint8_t pbRange = 2;
    };

} // namespace TeensySynth

#endif