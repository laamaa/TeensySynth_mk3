#ifndef TEENSY_SYNTH_H
#define TEENSY_SYNTH_H

#include <stdint.h>
#include "settings.h"
#include "dsp/synth_plaits_f32.h"
#include "dsp/filter_moog_f32.h"
#include "dsp/effect_ensemble_f32.h"
#include "dsp/effect_freeverb_f32.h"
#include <OpenAudio_ArduinoLibrary.h>

namespace TeensySynth
{
    class Synth
    {
    public:
        ~Synth();

        void createAudioPatch();
        void init();

        //Trigger a new voice
        void noteOn(uint8_t channel, uint8_t note, uint8_t velocity);

        //Kill an existing voice
        inline void noteOff(uint8_t channel, uint8_t note, uint8_t velocity)
        {
            noteOffReal(channel, note, velocity, false);
        }

        inline void setSynthEngine(int engine)
        {
            CONSTRAIN(engine, 0, 16);
            currentPatch.engine = engine;
            updateOscillator();
        }

        inline uint8_t getSynthEngine()
        {
            return currentPatch.engine;
        }

        //Set lowpass filter cutoff frequency in Hz. Affects all oscillators. Range: 20.0f - 19200.0f (on 48kHz sample rate)
        inline void setFilterCutoff(float cutoff)
        {
            CONSTRAIN(cutoff, 20.0f, filterMaxFreq);
            currentPatch.filterCutoff = cutoff;
            updateFilter();
        }

        inline float getFilterCutoff()
        {
            return currentPatch.filterCutoff;
        }

        //Set lowpass filter resonance (Q value). Affects all oscillators. Range: 0.7f - 5.0f
        inline void setFilterResonance(float resonance)
        {
            CONSTRAIN(resonance, 0.1f, 10.0f);
            currentPatch.filterResonance = resonance;
            updateFilter();
        }

        inline float getFilterResonance()
        {
            return currentPatch.filterResonance;
        }

        //Set lowpass filter overdrive amount. Affects all oscillators. Range: 0.1f - 10.0f
        inline void setFilterDrive(float drive)
        {
            CONSTRAIN(drive, 0.1f, 10.0f);
            currentPatch.filterDrive = drive;
            updateFilter();
        }

        inline float getFilterDrive()
        {
            return currentPatch.filterDrive;
        }

        //Set oscillator parameter "harmonics". Affects all oscillators.
        inline void setOscillatorHarmonics(float harmonics)
        {
            CONSTRAIN(harmonics, 0.0f, 1.0f);
            currentPatch.harmonics = harmonics;
            updateOscillator();
        }

        inline float getOscillatorHarmonics()
        {
            return currentPatch.harmonics;
        }

        //Set oscillator parameter "timbre". Affects all oscillators.
        inline void setOscillatorTimbre(float timbre)
        {
            CONSTRAIN(timbre, 0.0f, 1.0f);
            currentPatch.timbre = timbre;
            updateOscillator();
        }

        inline float getOscillatorTimbre()
        {
            return currentPatch.timbre;
        }

        //Set oscillator parameter "morph". Affects all oscillators.
        inline void setOscillatorMorph(float morph)
        {
            CONSTRAIN(morph, 0.0f, 1.0f);
            currentPatch.morph = morph;
            updateOscillator();
        }

        inline float getOscillatorMorph()
        {
            return currentPatch.morph;
        }

        inline void setOscillatorBalance(float balance)
        {
            CONSTRAIN(balance, 0.0f, 1.0f);
            balance = balance * OSC_LEVEL;
            currentPatch.balance = balance;
            updateOscillatorBalance();
        }

        inline float getOscillatorBalance()
        {
            return currentPatch.balance;
        }

        inline void setOscillatorDecay(float decay)
        {
            CONSTRAIN(decay, 0.0f, 1.0f);
            currentPatch.decay = decay;
            updateOscillator();
        }

        inline float getOscillatorDecay()
        {
            return currentPatch.decay;
        }

        inline void setReverbSize(float size)
        {
            CONSTRAIN(size, 0.1f, 0.95f);
            currentPatch.reverbSize = size;

            updateChorusAndReverb();
        }

        inline float getReverbSize()
        {
            return currentPatch.reverbSize;
        }

        inline void setReverbDepth(float depth)
        {
            CONSTRAIN(depth, 0.0f, MIX_LEVEL);
            currentPatch.reverbDepth = depth;
            updateChorusAndReverb();
        }

        inline float getReverbDepth()
        {
            return currentPatch.reverbDepth;
        }

        //Set chorus depth (mix%). Range 0.0f-1.0f.
        inline void setChorusDepth(float depth)
        {
            CONSTRAIN(depth, 0.0f, 1.0f);
            currentPatch.chorusDepth = depth;
            updateChorusAndReverb();
        }

        void resetAll();

        //Set frequency modulation amount. Range 0.0f-1.0f.
        inline void setFreqMod(float freqMod)
        {
            CONSTRAIN(freqMod, 0.0f, 1.0f);
            currentPatch.freqMod = freqMod;
            updateOscillator();
        }

        inline float getFreqMod()
        {
            return currentPatch.freqMod;
        }

        //Set timbre modulation amount. Range 0.0f-1.0f.
        inline void setTimbreMod(float timbreMod)
        {
            CONSTRAIN(timbreMod, 0.0f, 1.0f);
            currentPatch.timbreMod = timbreMod;
            updateOscillator();
        }

        inline float getTimbreMod()
        {
            return currentPatch.timbreMod;
        }

        //Set morph modulation amount. Range 0.0f-1.0f.
        inline void setMorphMod(float morphMod)
        {
            CONSTRAIN(morphMod, 0.0f, 1.0f);
            currentPatch.morphMod = morphMod;
            updateOscillator();
        }

        inline float getMorphMod()
        {
            return currentPatch.morphMod;
        }

        inline void setLpgColour(float lpgColour)
        {
            CONSTRAIN(lpgColour, 0.0f, 1.0f);
            currentPatch.lpgColour = lpgColour;
            updateOscillator();
        }

        inline float getLpgColour()
        {
            return currentPatch.lpgColour;
        }

        AudioStream &getMasterL()
        {
            return float2Int1;
        }

        AudioStream &getMasterR()
        {
            return float2Int2;
        }

#if SYNTH_DEBUG > 0
        float statsCpu = 0;
        uint8_t statsMemI16 = 0;
        uint8_t statsMemF32 = 0;
        void performanceCheck();
        inline void printResources(float cpu, uint8_t memF32, uint8_t memI16);
        void selectCommand(char c);
#endif

    private:
        //Audio signal path components
        AudioSynthPlaits_F32 waveform[NVOICES];
        AudioMixer4_F32 amp[NVOICES];
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
        AudioConnection_F32 *patchAmpMix[NVOICES];
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
            int8_t note;
            uint8_t velocity;
        };

        struct Patch
        {
            int engine = 0; // Plaits synth engine number
            float harmonics = 0.0f;
            float timbre = 0.0f;
            float morph = 0.0f;   // Plaits oscillator morph parameter
            float balance = 0.0f; // Balance between main/aux outputs of the Plaits oscillator. 0.0f = main only, 1.0f = aux only
            float decay = 1.0f;   // Plaits oscillator decay value
            float level = 1.0f;
            float lpgColour = 0.0f;
            float freqMod = 0.0f;
            float timbreMod = 0.0f;
            float morphMod = 0.0f;
            float filterCutoff = 19200.0f;
            float filterResonance = 1.0f;
            float filterDrive = 1.0f;
            bool portamentoOn = false;
            uint16_t portamentoTime = 200;
            bool velocityOn = false; //velocity enabled
            bool polyOn = true;      //polyphonic mode on
            float reverbSize = 0.7f;
            float reverbDepth = 0.1f;
            float chorusDepth = MIX_LEVEL;
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

        //Variables related to momentary information
        int8_t portamentoDir;
        float portamentoStep;
        float portamentoPos;
        bool sustainPressed;

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
        //load patches saved in flash memory, needs a pointer to an array with enough space for all presets (use NUM_PRESETS defined in settings.h)
        void loadPatchesFromFlash(Patch *patches);
        void savePatchesToFlash(Patch *patches);
    };
} // namespace TeensySynth
#endif