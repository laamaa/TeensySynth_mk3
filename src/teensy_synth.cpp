#include <Arduino.h>
#include "teensy_synth.h"
#include "settings.h"

//Inititializes audio signal path and default values for its components
void TeensySynth::init()
{
    for (int i = 0; i < NVOICES; i++)
    {
        //Store pointers for oscillator components
        oscs[i] = Oscillator({&waveform[i], &amp[i], &flt[i], -1, 0});

        //Create audio signal path
        patchOscAmp[i] = new AudioConnection_F32(waveform[i], 0, amp[i], 0);           //Main output connection
        patchOscAmp[i + NVOICES] = new AudioConnection_F32(waveform[i], 1, amp[i], 1); //Aux output connection
        patchAmpFlt[i] = new AudioConnection_F32(amp[i], 0, flt[i], 0);
        patchFltMix[i] = new AudioConnection_F32(flt[i], 0, mix, i);
    }
    patchMixMasterL = new AudioConnection_F32(mix, float2Int1);
    patchMixMasterR = new AudioConnection_F32(mix, float2Int2);
    patchMasterL = new AudioConnection(float2Int1, 0, i2s1, 0);
    patchMasterR = new AudioConnection(float2Int2, 0, i2s1, 1);

    //Initialize default values for signal path components
    {
        Oscillator *o = oscs, *end = oscs + NVOICES;
        do
        {
            o->amp->gain(0, 0.8f);
            o->amp->gain(1, 0.0f);
            o->flt->frequency(AUDIO_SAMPLE_RATE_EXACT / 2.5);
            o->flt->resonance(1.0f);
            o->flt->drive(1.0f);
        } while (++o < end);
    }
}

//Handles MIDI note on events
void TeensySynth::OnNoteOn(uint8_t channel, uint8_t note, uint8_t velocity)
{
    if (omniOn || channel != SYNTH_MIDICHANNEL)
        return;

    notesAdd(notesPressed, note);
    currentPatch.polyOn = true;

    Oscillator *o = oscs;
    if (currentPatch.portamentoOn)
    {
        if (currentPatch.portamentoTime == 0 || portamentoPos < 0)
        {
            portamentoPos = note;
            portamentoDir = 0;
        }
        else if (portamentoPos > -1)
        {
            portamentoDir = note > portamentoPos ? 1 : -1;
            portamentoStep = fabs(note - portamentoPos) / (currentPatch.portamentoTime);
        }
        *notesOn = -1;
        oscOn(*o, note, velocity);
    }
    else if (currentPatch.polyOn)
    {
        Oscillator *curOsc = 0, *end = oscs + NVOICES;
        if (sustainPressed && notesFind(notesOn, note))
        {
            do
            {
                if (o->note == note)
                {
                    curOsc = o;
                    break;
                }
            } while (++o < end);
        }
        for (o = oscs; o < end && !curOsc; ++o)
        {
            if (o->note < 0)
            {
                curOsc = o;
                break;
            }
        }
        if (!curOsc && *notesOn != -1)
        {
#if SYNTH_DEBUG > 0
            Serial.println("Stealing voice");
#endif
            curOsc = OnNoteOffReal(channel, *notesOn, velocity, true);
        }
        if (!curOsc)
            return;
        oscOn(*curOsc, note, velocity);
    }
    else
    {
        *notesOn = -1;
        oscOn(*o, note, velocity);
    }

    return;
}

TeensySynth::Oscillator *TeensySynth::OnNoteOffReal(uint8_t channel, uint8_t note, uint8_t velocity, bool ignoreSustain)
{
    if (!omniOn && channel != SYNTH_MIDICHANNEL)
        return 0;

    int8_t lastNote = notesDel(notesPressed, note);

    if (sustainPressed && !ignoreSustain)
        return 0;

    Oscillator *o = oscs;
    if (currentPatch.portamentoOn)
    {
        if (o->note == note)
        {
            if (lastNote != -1)
            {
                notesDel(notesOn, note);
                if (currentPatch.portamentoTime == 0)
                {
                    portamentoPos = lastNote;
                    portamentoDir = 0;
                }
                else
                {
                    portamentoDir = lastNote > portamentoPos ? 1 : -1;
                    portamentoStep = fabs(lastNote - portamentoPos) / (currentPatch.portamentoTime);
                }
                oscOn(*o, lastNote, velocity);
            }
            else
            {
                oscOff(*o);
                portamentoPos = -1;
                portamentoDir = 0;
            }
        }
        if (oscs->note == note)
        {
            if (lastNote != -1)
            {
                notesDel(notesOn, o->note);
                oscOn(*o, lastNote, velocity);
            }
            else
            {
                oscOff(*o);
            }
        }
    }
    else if (currentPatch.polyOn)
    {
        Oscillator *end = oscs + NVOICES;
        do
        {
            if (o->note == note)
                break;
        } while (++o < end);
        if (o == end)
            return 0;
        oscOff(*o);
    }
    else
    {
        if (oscs->note == note)
        {
            if (lastNote != -1)
            {
                notesDel(notesOn, o->note);
                oscOn(*o, lastNote, velocity);
            }
            else
            {
                oscOff(*o);
            }
        }
    }

    return o;
}

inline void TeensySynth::notesReset(int8_t *notes)
{
    memset(notes, -1, NVOICES * sizeof(int8_t));
}

void TeensySynth::notesAdd(int8_t *notes, uint8_t note)
{
    int8_t *end = notes + NVOICES;
    do
    {
        if (*notes == -1)
        {
            *notes = note;
            break;
        }
    } while (++notes < end);
}

int8_t TeensySynth::notesDel(int8_t *notes, uint8_t note)
{
    int8_t lastNote = -1;
    int8_t *pos = notes, *end = notes + NVOICES;
    while (++pos < end && *(pos - 1) != note)
        ;
    if (pos - 1 != notes)
        lastNote = *(pos - 2);
    while (pos < end)
    {
        *(pos - 1) = *pos;
        if (*pos++ == -1)
            break;
    }
    if (*(end - 1) == note || pos == end)
        *(end - 1) = -1;
    return lastNote;
}

bool TeensySynth::notesFind(int8_t *notes, uint8_t note)
{
    int8_t *end = notes + NVOICES;
    do
    {
        if (*notes == note)
            return true;
    } while (++notes < end);
    return false;
}

void TeensySynth::oscOn(Oscillator &osc, int8_t note, uint8_t velocity)
{
    float v = currentPatch.velocityOn ? velocity / 127. : 1;
    if (osc.note != note)
    {
        osc.wf->setPatchParameter(AudioSynthPlaits_F32::Parameters::note, note);
        osc.wf->setModulationsParameter(AudioSynthPlaits_F32::Parameters::trigger, 1.0f);
        notesAdd(notesOn, note);
        if (!osc.velocity)
        {
            // osc.env->noteOn();
            // osc.flt_env->noteOn();
        }
        // osc.amp->gain(GAIN_OSC * v);
        osc.velocity = velocity;
        osc.note = note;
    }
}

inline void TeensySynth::oscOff(Oscillator &osc)
{
    notesDel(notesOn, osc.note);
    osc.note = -1;
    osc.velocity = 0;
    osc.wf->setModulationsParameter(AudioSynthPlaits_F32::Parameters::trigger, 0.0f);
}

inline void TeensySynth::allOff()
{
    Oscillator *o = oscs, *end = oscs + NVOICES;
    do
    {
        oscOff(*o);
        // o->env->noteOff();
        // o->flt_env->noteOff();
    } while (++o < end);
    notesReset(notesOn);
}

void TeensySynth::OnControlChange(uint8_t channel, uint8_t control, uint8_t value)
{
    if (!omniOn && channel != SYNTH_MIDICHANNEL)
        return;

    switch (control)
    {
    case 0: // bank select, do nothing (switch sounds via program change only)
        break;
    default:
        break;
    };
}

void TeensySynth::updateFilter()
{
    Oscillator *o = oscs, *end = oscs + NVOICES;
    do
    {
        o->flt->frequency(currentPatch.filterCutoff);
        o->flt->resonance(currentPatch.filterResonance);
        o->flt->drive(currentPatch.filterDrive);
    } while (++o < end);
}