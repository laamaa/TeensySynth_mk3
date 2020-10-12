#ifndef TEENSY_SYNTH_H
#define TEENSY_SYNTH_H

#include <stdint.h>
#include "settings.h"
#include "patch.h"
#include "dsp/synth_plaits_f32.h"
#include "dsp/filter_moog_f32.h"
#include "dsp/effect_ensemble_f32.h"
#include "dsp/effect_freeverb_f32.h"
#include "dsp/effect_envelope_f32.h"
#include <OpenAudio_ArduinoLibrary.h>

namespace TeensySynth
{
    class Synth
    {
    public:
        Synth(Settings *ptrSettings)
        {
            settings = ptrSettings;
        }
        ~Synth();

        // Create patch for Teensy audio library components
        void createAudioPatch();

        // Inititializes audio signal path and default values for its components
        void init();

        // Read a preset from the preset-table and update synth parameters accordingly
        void loadPreset(uint8_t newPreset);

        // Store current synth parameters to preset-table
        void savePreset(uint8_t newPreset);

        // Initialize default values for audio signal path components
        void resetAll();

        // Trigger a new voice
        void noteOn(uint8_t channel, uint8_t note, uint8_t velocity);

        // Kill an existing voice
        inline void noteOff(uint8_t channel, uint8_t note, uint8_t velocity)
        {
            noteOffReal(channel, note, velocity, false);
        }

        // Set Plaits synth engine. Range: 0-15.
        inline void setSynthEngine(int engine)
        {
            CONSTRAIN(engine, 0, 15);
            currentPatch.engine = engine;
            updateOscillator();
        }
        inline uint8_t getSynthEngine() { return currentPatch.engine; }

        // Set lowpass filter cutoff frequency in Hz. Affects all oscillators. Range: 20.0f - 19200.0f (on 48kHz sample rate)
        inline void setFilterCutoff(float cutoff)
        {
            CONSTRAIN(cutoff, 20.0f, filterMaxFreq);
            currentPatch.filterCutoff = cutoff;
            updateFilter();
        }
        inline float getFilterCutoff() { return currentPatch.filterCutoff; }

        // Set lowpass filter resonance (Q value). Affects all oscillators. Range: 0.7f - 5.0f
        inline void setFilterResonance(float resonance)
        {
            CONSTRAIN(resonance, 0.7f, 5.0f);
            currentPatch.filterResonance = resonance;
            updateFilter();
        }
        inline float getFilterResonance() { return currentPatch.filterResonance; }

        // Set lowpass filter overdrive amount. Affects all oscillators. Range: 0.1f - 10.0f
        inline void setFilterDrive(float drive)
        {
            CONSTRAIN(drive, 0.1f, 10.0f);
            currentPatch.filterDrive = drive;
            updateFilter();
        }
        inline float getFilterDrive() { return currentPatch.filterDrive; }

        // Set oscillator parameter "harmonics". Affects all oscillators.
        inline void setOscillatorHarmonics(float harmonics)
        {
            CONSTRAIN(harmonics, 0.0f, 1.0f);
            currentPatch.harmonics = harmonics;
            updateOscillator();
        }
        inline float getOscillatorHarmonics() { return currentPatch.harmonics; }

        // Set oscillator parameter "timbre". Affects all oscillators. Range: 0.0f - 1.0f.
        inline void setOscillatorTimbre(float timbre)
        {
            CONSTRAIN(timbre, 0.0f, 1.0f);
            currentPatch.timbre = timbre;
            updateOscillator();
        }
        inline float getOscillatorTimbre() { return currentPatch.timbre; }

        // Set oscillator parameter "morph". Affects all oscillators. Range: 0.0f - 1.0f.
        inline void setOscillatorMorph(float morph)
        {
            CONSTRAIN(morph, 0.0f, 1.0f);
            currentPatch.morph = morph;
            updateOscillator();
        }
        inline float getOscillatorMorph() { return currentPatch.morph; }

        // Set the balance between main and aux signals that the oscillators produce. Range: 0.0f - 1.0f.
        inline void setOscillatorBalance(float balance)
        {
            CONSTRAIN(balance, 0.0f, 1.0f);
            balance = balance * OSC_LEVEL;
            currentPatch.balance = balance;
            updateOscillatorBalance();
        }
        inline float getOscillatorBalance() { return currentPatch.balance; }

        // Set oscillator interal decay envelope value. Affects all oscillators. Range: 0.0f - 1.0f.
        inline void setOscillatorDecay(float decay)
        {
            CONSTRAIN(decay, 0.0f, 1.0f);
            currentPatch.decay = decay;
            updateOscillator();
        }
        inline float getOscillatorDecay() { return currentPatch.decay; }

        // Set master reverb room size. Range: 0.1f-0.95f.
        inline void setReverbSize(float size)
        {
            CONSTRAIN(size, 0.1f, 0.95f);
            currentPatch.reverbSize = size;

            updateChorusAndReverb();
        }
        inline float getReverbSize() { return currentPatch.reverbSize; }

        // Set master reverb level. Range: 0.0f - MIX_LEVEL in settings.h
        inline void setReverbDepth(float depth)
        {
            CONSTRAIN(depth, 0.0f, MIX_LEVEL);
            currentPatch.reverbDepth = depth;
            updateChorusAndReverb();
        }
        inline float getReverbDepth() { return currentPatch.reverbDepth; }

        // Set chorus depth (mix%). Range: 0.0f-1.0f.
        inline void setChorusDepth(float depth)
        {
            CONSTRAIN(depth, 0.0f, 1.0f);
            currentPatch.chorusDepth = depth;
            updateChorusAndReverb();
        }
        inline float getChorusDepth() { return currentPatch.chorusDepth; }

        // Set frequency modulation amount. Range: 0.0f-1.0f.
        inline void setFreqMod(float freqMod)
        {
            CONSTRAIN(freqMod, 0.0f, 1.0f);
            currentPatch.freqMod = freqMod;
            updateOscillator();
        }
        inline float getFreqMod() { return currentPatch.freqMod; }

        // Set timbre modulation amount. Range: 0.0f-1.0f.
        inline void setTimbreMod(float timbreMod)
        {
            CONSTRAIN(timbreMod, 0.0f, 1.0f);
            currentPatch.timbreMod = timbreMod;
            updateOscillator();
        }
        inline float getTimbreMod() { return currentPatch.timbreMod; }

        // Set morph modulation amount. Range: 0.0f-1.0f.
        inline void setMorphMod(float morphMod)
        {
            CONSTRAIN(morphMod, 0.0f, 1.0f);
            currentPatch.morphMod = morphMod;
            updateOscillator();
        }
        inline float getMorphMod() { return currentPatch.morphMod; }

        // Set Plaits low pass filter colour (affected by Plaits internal decay envelope setting). Range: 0.0f - 1.0f.
        inline void setLpgColour(float lpgColour)
        {
            CONSTRAIN(lpgColour, 0.0f, 1.0f);
            currentPatch.lpgColour = lpgColour;
            updateOscillator();
        }
        inline float getLpgColour() { return currentPatch.lpgColour; }

        inline void setVolume(float newVolume)
        {
            CONSTRAIN(newVolume, 0.0f, 1.0f);
            currentPatch.volume = newVolume;
            updateOscillatorBalance();
        }
        inline float getVolume() { return currentPatch.volume; }

        // Return a pointer to audio master output left, for use with Oscilloscope or similiar analysis
        AudioStream &getMasterL() { return float2Int1; }

        // Return a pointer to audio master output right, for use with Oscilloscope or similiar analysis
        AudioStream &getMasterR() { return float2Int2; }

        uint8_t getActivePresetNumber() { return activePresetNumber; }

        char *getActivePresetName() { return currentPatch.presetName; }

#if SYNTH_DEBUG > 0
        float statsCpu = 0;
        uint8_t statsMemI16 = 0;
        uint8_t statsMemF32 = 0;
        void performanceCheck();
        inline void printResources(float cpu, uint8_t memF32, uint8_t memI16);
        void selectCommand(char c);
#endif

    private:
        Settings *settings;

        //Audio signal path components
        AudioSynthPlaits_F32 waveform[NVOICES];
        AudioMixer4_F32 amp[NVOICES];
        AudioEffectEnvelope_F32 env[NVOICES];
        AudioMixer8_F32 mixOsc;
        AudioFilterBiquad_F32 fxReverbHighpass;
        AudioEffectFreeverbStereo_F32 *fxReverb; //Reverb is initialized dynamically in order to get the big buffers in RAM2
        AudioMixer4_F32 mixChorus;
        AudioEffectEnsemble_F32 fxChorus;
        AudioMixer4_F32 mixMasterL;
        AudioMixer4_F32 mixMasterR;
        AudioFilterMoog_F32 fxFlt[2];
        AudioConvert_F32toI16 float2Int1, float2Int2;
        AudioOutputI2S i2s1;

        //Pointers for audio signal path connections
        AudioConnection_F32 *patchOscAmp[NVOICES * 2];
        AudioConnection_F32 *patchAmpEnv[NVOICES];
        AudioConnection_F32 *patchEnvMix[NVOICES];
        AudioConnection_F32 *patchMixOscMixChorus;
        AudioConnection_F32 *patchMixOscFxReverbHighpass;
        AudioConnection_F32 *patchFxReverbHighpassFxReverb;
        AudioConnection_F32 *patchFxReverbMixChorus;
        AudioConnection_F32 *patchFxReverbMixMaster[2];
        AudioConnection_F32 *patchMixChorusFxchorus;
        AudioConnection_F32 *patchMixOscMixMaster[2];
        AudioConnection_F32 *patchFxChorusMixMaster[2];
        AudioConnection_F32 *patchMixMasterFxFlt[2];
        AudioConnection_F32 *patchFxFltConverter[2];
        AudioConnection *patchConverterI2s[2];

        //Structure for storing envelope parameters
        struct Envelope
        {
            float attack = 0.0f;
            float decay = 0.0f;
            float sustain = 1.0f;
            float release = 0.0f;
        };

        struct Oscillator
        {
            AudioSynthPlaits_F32 *wf;
            AudioMixer4_F32 *amp;
            AudioEffectEnvelope_F32 *env;
            int8_t note;
            uint8_t velocity;
        };

        //These are used to store active notes
        int8_t notesOn[NVOICES] = {-1};
        int8_t notesPressed[NVOICES] = {-1};

        //MIDI omnichannel setting
        bool omniOn = false;

        //Pointers to components in audio signal path that we like to edit in batch operations
        Oscillator oscs[NVOICES];

        //Current synth voice settings
        Patch currentPatch;

        // Allocate memory for storing presets
        Patch preset[PRESETS];

        //Constant for checking if the flash contents match what we're expecting
        const uint16_t memVersion = sizeof(Patch) * PRESETS + sizeof(Settings);

        //Offset for reading settings from the flash (stored after memory version and patches)
        const uint16_t settingsOffset = sizeof(uint16_t) + sizeof(Patch) * PRESETS;

        //Variables related to momentary information
        int8_t portamentoDir;
        float portamentoStep;
        float portamentoPos;
        bool sustainPressed;
        uint8_t activePresetNumber;

        // Maximum filter frequency, let's keep it a little below Nyquist frequency
        const float filterMaxFreq = AUDIO_SAMPLE_RATE_EXACT / 2.5;

        inline void initOscillators();
        void notesAdd(int8_t *notes, uint8_t note);
        int8_t notesDel(int8_t *notes, uint8_t note);
        bool notesFind(int8_t *notes, uint8_t note);
        inline void notesReset(int8_t *notes);
        void oscOn(Oscillator &osc, int8_t note, uint8_t velocity);
        void oscOff(Oscillator &osc);
        void allOff();
        Oscillator *noteOffReal(uint8_t channel, uint8_t note, uint8_t velocity, bool ignoreSustain);
        void updateFilter();
        void updateOscillator();
        void updateOscillatorBalance();
        void updateDecay();
        void updateChorusAndReverb();
        void updateAll();
        bool checkFlash();
    };
} // namespace TeensySynth
#endif