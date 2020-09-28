#include <Arduino.h>
#include "inc/synth.h"
#include "inc/settings.h"

namespace TeensySynth
{

    //Inititializes audio signal path and default values for its components
    void Synth::init()
    {
        for (int i = 0; i < NVOICES; i++)
        {
            //Store pointers for oscillator components
            oscs[i] = Oscillator({&waveform[i], &amp[i], -1, 0});

            //Create audio signal path for voice components
            patchOscAmp[i] = new AudioConnection_F32(waveform[i], 0, amp[i], 0);           //Main output connection
            patchOscAmp[i + NVOICES] = new AudioConnection_F32(waveform[i], 1, amp[i], 1); //Aux output connection
            patchAmpMix[i] = new AudioConnection_F32(amp[i], 0, mixOsc, i);
        }

        //Create audio signal path for master & fx
        patchMixOscMixChorus = new AudioConnection_F32(mixOsc, 0, mixChorus, 0);
        patchMixOscFxReverbHighpass = new AudioConnection_F32(mixOsc, 0, fxReverbHighpass, 0);
        patchFxReverbHighpassFxReverb = new AudioConnection_F32(fxReverbHighpass, fxReverb);
        patchFxReverbMixChorus = new AudioConnection_F32(fxReverb, 0, mixChorus, 1);
        patchMixChorusFxchorus = new AudioConnection_F32(mixChorus, fxChorus);
        patchMixOscMixMaster[0] = new AudioConnection_F32(mixOsc, 0, mixMasterL, 0);
        patchMixOscMixMaster[1] = new AudioConnection_F32(mixOsc, 0, mixMasterR, 0);
        patchFxChorusMixMaster[0] = new AudioConnection_F32(fxChorus, 0, mixMasterL, 1);
        patchFxChorusMixMaster[1] = new AudioConnection_F32(fxChorus, 1, mixMasterR, 1);
        patchFxReverbMixMaster[0] = new AudioConnection_F32(fxReverb, 0, mixMasterL, 2);
        patchFxReverbMixMaster[1] = new AudioConnection_F32(fxReverb, 1, mixMasterR, 2);
        patchMixMasterFxFlt[0] = new AudioConnection_F32(mixMasterL, fxFlt[0]);
        patchMixMasterFxFlt[1] = new AudioConnection_F32(mixMasterR, fxFlt[1]);
        patchFxFltConverter[0] = new AudioConnection_F32(fxFlt[0], float2Int1);
        patchFxFltConverter[1] = new AudioConnection_F32(fxFlt[1], float2Int2);
        patchConverterI2s[0] = new AudioConnection(float2Int1, 0, i2s1, 0);
        patchConverterI2s[1] = new AudioConnection(float2Int2, 0, i2s1, 1);

        //Initialize default values for signal path components
        {
            Oscillator *o = oscs, *end = oscs + NVOICES;
            do
            {
                o->amp->gain(0, OSC_LEVEL);
                o->amp->gain(1, 0.0f);
            } while (++o < end);
        }

        mixMasterL.gain(0, MIX_LEVEL - 0.1f); //dry signal L
        mixMasterL.gain(1, MIX_LEVEL - 0.1f); //chorus signal L
        mixMasterL.gain(2, 0.1f);             //reverb signal L
        mixMasterR.gain(0, MIX_LEVEL - 0.1f); //dry signal R
        mixMasterR.gain(1, MIX_LEVEL - 0.1f); //chorus signal R
        mixMasterR.gain(2, 0.1f);             //reverb signal R

        fxReverbHighpass.setHighpass(0, REV_HIGHPASS); // Highpass filter before reverb
        fxReverb.roomsize(0.7f);
        fxReverb.damping(0.7f);

        mixChorus.gain(0, 0.8f);             // Chorus input level
        mixChorus.gain(1, CHORUS_REV_LEVEL); // Reverb -> Chorus level

        resetAll();
    }

    //Handles MIDI note on events
    void Synth::noteOn(uint8_t channel, uint8_t note, uint8_t velocity)
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
                curOsc = noteOffReal(channel, *notesOn, velocity, true);
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

    Synth::Oscillator *Synth::noteOffReal(uint8_t channel, uint8_t note, uint8_t velocity, bool ignoreSustain)
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

    inline void Synth::notesReset(int8_t *notes)
    {
        memset(notes, -1, NVOICES * sizeof(int8_t));
    }

    void Synth::notesAdd(int8_t *notes, uint8_t note)
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

    int8_t Synth::notesDel(int8_t *notes, uint8_t note)
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

    bool Synth::notesFind(int8_t *notes, uint8_t note)
    {
        int8_t *end = notes + NVOICES;
        do
        {
            if (*notes == note)
                return true;
        } while (++notes < end);
        return false;
    }

    void Synth::oscOn(Oscillator &osc, int8_t note, uint8_t velocity)
    {
        float v = currentPatch.velocityOn ? velocity / 127. : 1;
        if (osc.note != note)
        {
            osc.wf->setPatchParameter(AudioSynthPlaits_F32::Parameters::note, note);
            osc.wf->setModulationsParameter(AudioSynthPlaits_F32::Parameters::trigger, 1.0f);
            osc.wf->setPatchParameter(AudioSynthPlaits_F32::Parameters::decay, currentPatch.decay);
            notesAdd(notesOn, note);
            if (!osc.velocity)
            {
                // osc.flt_env->noteOn();
            }
            // osc.amp->gain(GAIN_OSC * v);
            osc.velocity = velocity;
            osc.note = note;
        }
    }

    void Synth::oscOff(Oscillator &osc)
    {
        notesDel(notesOn, osc.note);
        osc.note = -1;
        osc.velocity = 0;
        osc.wf->setModulationsParameter(AudioSynthPlaits_F32::Parameters::trigger, 0.0f);
        osc.wf->setPatchParameter(AudioSynthPlaits_F32::Parameters::decay, 0.0f);
    }

    void Synth::allOff()
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

    void Synth::OnControlChange(uint8_t channel, uint8_t control, uint8_t value)
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

    void Synth::updateFilter()
    {
        for (int i = 0; i < 2; i++)
        {
            fxFlt[i].frequency(currentPatch.filterCutoff);
            fxFlt[i].resonance(currentPatch.filterResonance);
            fxFlt[i].drive(currentPatch.filterDrive);
        }
    }

    void Synth::updateOscillator()
    {
        Oscillator *o = oscs, *end = oscs + NVOICES;
        do
        {
            o->wf->setPatchParameter(AudioSynthPlaits_F32::Parameters::harmonics, currentPatch.harmonics);
            o->wf->setPatchParameter(AudioSynthPlaits_F32::Parameters::timbre, currentPatch.timbre);
            o->wf->setPatchParameter(AudioSynthPlaits_F32::Parameters::morph, currentPatch.morph);
            o->wf->setPatchParameter(AudioSynthPlaits_F32::Parameters::engine, currentPatch.engine);
            o->wf->setPatchParameter(AudioSynthPlaits_F32::Parameters::lpgColour, currentPatch.lpgColour);
            o->wf->setPatchParameter(AudioSynthPlaits_F32::Parameters::morphModulationAmount, currentPatch.morphMod);
            o->wf->setPatchParameter(AudioSynthPlaits_F32::Parameters::timbreModulationAmount, currentPatch.timbreMod);
            o->wf->setPatchParameter(AudioSynthPlaits_F32::Parameters::frequencyModulationAmount, currentPatch.freqMod);
        } while (++o < end);
    }

    void Synth::updateDecay()
    {
        Oscillator *o = oscs, *end = oscs + NVOICES;
        do
        {
            if (o->note != -1)
                o->wf->setPatchParameter(AudioSynthPlaits_F32::Parameters::decay, currentPatch.decay);
        } while (++o < end);
    }

    void Synth::updateOscillatorBalance()
    {
        Oscillator *o = oscs, *end = oscs + NVOICES;
        do
        {
            o->amp->gain(0, OSC_LEVEL - currentPatch.balance);
            o->amp->gain(1, currentPatch.balance);
        } while (++o < end);
    }

    void Synth::updateChorusAndReverb()
    {
        fxReverb.roomsize(currentPatch.reverbSize);
        mixMasterL.gain(0, MIX_LEVEL - currentPatch.reverbDepth);                              //dry signal L
        mixMasterL.gain(1, currentPatch.chorusDepth * (MIX_LEVEL - currentPatch.reverbDepth)); //chorus signal L
        mixMasterL.gain(2, currentPatch.reverbDepth);                                          //reverb signal L
        mixMasterR.gain(0, MIX_LEVEL - currentPatch.reverbDepth);                              //dry signal R
        mixMasterR.gain(1, currentPatch.chorusDepth * (MIX_LEVEL - currentPatch.reverbDepth)); //chorus signal R
        mixMasterR.gain(2, currentPatch.reverbDepth);                                          //reverb signal R
        mixChorus.gain(1, CHORUS_REV_LEVEL * currentPatch.reverbDepth);                        //Reverb -> Chorus level
    }
} // namespace TeensySynth