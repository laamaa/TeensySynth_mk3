#ifndef TEENSY_SYNTH_H
#define TEENSY_SYNTH_H

#include <stdint.h>
#include "settings.h"
#include "dsp/synth_plaits_f32.h"
#include <OpenAudio_ArduinoLibrary.h>

class TeensySynth
{
public:
    void init();
    void OnNoteOn(uint8_t channel, uint8_t note, uint8_t velocity);
    inline void OnNoteOff(uint8_t channel, uint8_t note, uint8_t velocity)
    {
        OnNoteOffReal(channel, note, velocity, false);
    }
    void OnControlChange(uint8_t channel, uint8_t control, uint8_t value);

private:
    //Audio signal path components
    AudioSynthPlaits_F32 waveform[NVOICES];
    AudioMixer4_F32 amp[NVOICES];
    AudioMixer8_F32 mix;
    AudioConvert_F32toI16 float2Int1, float2Int2;
    AudioOutputI2S i2s1;

    //Pointers for audio signal path connections
    AudioConnection_F32 *patchOscAmp[NVOICES * 2];
    AudioConnection_F32 *patchAmpMix[NVOICES];
    AudioConnection_F32 *patchMixMasterL;
    AudioConnection_F32 *patchMixMasterR;
    AudioConnection *patchMasterL;
    AudioConnection *patchMasterR;

    //Structure for storing envelope parameters
    struct Envelope
    {
        float attack = 0.0f;
        float decay = 0.0f;
        float sustain = 1.0f;
        float release = 0.0f;
    };

    //Structure for storing presets and current instrument settings
    struct Patch
    {
        float engine = 0.0f; // Plaits synth engine number
        float color = 0.0f;
        float timbre = 0.0f;
        float morph = 0.0f;
        float balance = 0.5f; // Balance between main/aux outputs of the oscillator
        Envelope ampEnvelope = {0.0f, 0.0f, 0.0f, 0.0f};
        Envelope filterEnvelope = {0.0f, 0.0f, 0.0f, 0.0f};
        float filterEnvelopeDepth = 0.0f;
        float filterLfoDepth = 0.0f;
        float filterLfoRate = 0.0f;
        bool portamentoOn = false;
        uint16_t portamentoTime = 200;
        bool velocityOn = false; //velocity enabled
        bool polyOn = true;      //polyphonic mode on
    };

    struct Oscillator
    {
        AudioSynthPlaits_F32 *wf;
        AudioMixer4_F32 *amp;
        int8_t note;
        uint8_t velocity;
    };

    //These are used to store active notes
    int8_t notesOn[NVOICES] = {-1};
    int8_t notesPressed[NVOICES] = {-1};

    //MIDI omnichannel setting
    bool omniOn = false;

    //Pointers to components in audio signal path that we like to edit in batch operations
    Oscillator oscs[NVOICES] = {
        {&waveform[0], &amp[0], -1, 0},
        {&waveform[1], &amp[1], -1, 0},
        {&waveform[2], &amp[2], -1, 0},
        {&waveform[3], &amp[3], -1, 0},
    };

    Patch currentPatch;
    Patch preset[PRESETS]; // Allocate memory for storing presets

    //Variables related to momentary information
    int8_t portamentoDir;
    float portamentoStep;
    float portamentoPos;
    bool sustainPressed;

    inline void initOscillators();

    void notesAdd(int8_t *notes, uint8_t note);
    int8_t notesDel(int8_t *notes, uint8_t note);
    bool notesFind(int8_t *notes, uint8_t note);
    inline void notesReset(int8_t *notes);
    void oscOn(Oscillator &osc, int8_t note, uint8_t velocity);
    inline void oscOff(Oscillator &osc);
    inline void allOff();
    Oscillator *OnNoteOffReal(uint8_t channel, uint8_t note, uint8_t velocity, bool ignoreSustain);
};

#endif