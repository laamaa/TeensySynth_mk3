#ifndef PATCH_H_
#define PATCH_H_

namespace TeensySynth {
    //Class for storing & adjusting synth parameters. TODO: move getters&setters from synth.h here.
    class Patch {
        public:
            char presetName[12] = "Init";  // Title of the preset
            int engine = 0;                // Plaits synth engine number
            float harmonics = 0.0f;        // Plaits oscillator harmonics parameter
            float timbre = 0.0f;           // Plaits oscillator timbre parameter
            float morph = 0.0f;            // Plaits oscillator morph parameter
            float balance = 0.0f;          // Balance between main/aux outputs of the Plaits oscillator. 0.0f = main only, 1.0f = aux only
            float decay = 1.0f;            // Plaits oscillator decay value
            float level = 1.0f;            // Patch volume
            float lpgColour = 0.0f;        // Plaits oscillator Low Pass colour parameter
            float harmonicsMod = 0.0f;     // Plaits oscillator harmonics modulation
            float freqMod = 0.0f;          // Plaits oscillator freq modulation
            float timbreMod = 0.0f;        // Plaits oscillator timbre modulation
            float morphMod = 0.0f;         // Plaits oscillator morph modulation
            float filterCutoff = 19200.0f; // Master filter cutoff in Hz
            float filterResonance = 1.0f;  // Master filter resonance
            float filterDrive = 1.0f;      // Master filter drive
            bool portamentoOn = false;     // Is portamento enabled (this might get removed)
            uint16_t portamentoTime = 200; // Portamento time
            bool velocityOn = true;        // velocity enabled
            bool polyOn = true;            // polyphonic mode on
            float reverbSize = 0.7f;       // Master reverb room size
            float reverbDepth = 0.1f;      // Master reverb amount
            float chorusDepth = MIX_LEVEL; // Master chorus amount
    };
}

#endif