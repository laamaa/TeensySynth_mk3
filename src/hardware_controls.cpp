#include "hardware_controls.h"
#include "settings.h"
#include <Encoder.h>

void HardwareControls::checkControlValues(bool update)
{
    int i, ctlValue;

    //Potentiometers: all are connected to a single multiplexer so we just read them in a for loop
    for (i = 0; i < 16; i++)
    {
        ctlValue = readMux(i);
        if (ctlValue < 10)
            ctlValue = 0;
        if ((ctlValue == 1023 && currentCtlValue[i] != 1023) || ctlValue > currentCtlValue[i] + potThreshold[i] || ctlValue < currentCtlValue[i] - potThreshold[i])
        {
            if (update == 1)
            {
#if SYNTH_DEBUG > 1
                Serial.printf("Pot upd, num: %d, val: %d, currentVal: %d, th: %d\n", i, ctlValue, currentCtlValue[i], potThreshold[i]);
#endif
                if (ctlValue < currentCtlValue[i] + 50 && ctlValue > currentCtlValue[i] - 50) //prevent pot noise spikes
                {
                    currentCtlValue[i] = ctlValue;
                    updateTeensySynth(i, ctlValue);
                }
            }
            else
                currentCtlValue[i] = ctlValue;
        }
    }

    //Check encoder readings. Sorry for this mess.
    for (i = 0; i < 2; i++)
    {
        ctlValue = encoder[i]->readAndReset();
        if (ctlValue < 0)
        {
            //The crappy encoders that I bought from Aliexpress seem to be like 1 click = 4 increments.. trying to work around that
            if (encValue[i] < 0)
                encValue[i] = 0;
            encValue[i]++;
            if (encValue[i] % 4 == 0)
            {
                currentCtlValue[CTL_ENC_1 + i]++;
                updateTeensySynth(CTL_ENC_1 + i, currentCtlValue[CTL_ENC_1 + i]);
            }
        }
        else if (ctlValue > 0)
        {
            if (encValue[i] > 0)
                encValue[i] = 0;
            encValue[i]--;
            if (encValue[i] % 4 == 0)
            {
                currentCtlValue[CTL_ENC_1 + i]--;
                updateTeensySynth(CTL_ENC_1 + i, currentCtlValue[CTL_ENC_1 + i]);
            }
        }
    }
}

void HardwareControls::updateTeensySynth(uint8_t ctl, int value)
{
    switch (ctl)
    {
    case CTL_ENC_1:
        CONSTRAIN(currentCtlValue[CTL_ENC_1], 0, 16);
        Serial.println(currentCtlValue[CTL_ENC_1]);
        ts->setSynthEngine(currentCtlValue[CTL_ENC_1]);
        break;
    case CTL_ENC_2:
        Serial.println(currentCtlValue[CTL_ENC_2]);
        break;
    case CTL_FREQMOD_AMOUNT:
        ts->setFreqMod((float)value / 1023.0f);
        break;
    case CTL_TIMBREMOD_AMOUNT:
        ts->setTimbreMod((float)value / 1023.0f);
        break;
    case CTL_MORPHMOD_AMOUNT:
        ts->setMorphMod((float)value / 1023.0f);
        break;
    case CTL_LPG_COLOUR:
        ts->setLpgColour((float)value / 1023.0f);
        break;        
    case CTL_DECAY:
        ts->setOscillatorDecay((float)value / 1023.0f);
        break;
    case CTL_REV_SIZE:
        ts->setReverbSize((float)value / 1023.0f);
        break;
    case CTL_REV_DEPTH:
        ts->setReverbDepth(powf((value / 101.53), 3) / 1023.0f * MIX_LEVEL);
        break;
    case CTL_CHORUS_DEPTH:
        ts->setChorusDepth(((float)value / 1023.0f));
        break;
    case CTL_HARMONICS:
        ts->setOscillatorHarmonics((float)value / 1023.0f);
        break;
    case CTL_MORPH:
        ts->setOscillatorMorph((float)value / 1023.0f);
        break;
    case CTL_TIMBRE:
        ts->setOscillatorTimbre((float)value / 1023.0f);
        break;
    case CTL_BALANCE:
        ts->setOscillatorBalance((float)value / 1023.0f);
        break;
    case CTL_FLT_CUTOFF:
    {
        float new_cutoff = 30 + 12 * powf((value / 101.53), 3);

        // The filter has a pretty nasty self-oscillation in higher frequencies when the Q is large enough, trying to prevent this..
        if (ts->getFilterResonance() > 3.07 && new_cutoff > 10000)
            ts->setFilterResonance(3.0f);
        ts->setFilterCutoff(new_cutoff);
        break;
    }
    case CTL_FLT_RESO:
    {
        float new_reso = ((float)value / 1023.0f) * 4.0f;
        if (ts->getFilterCutoff() > 10000 && new_reso > 3.0)
            new_reso = 3.0f;
        ts->setFilterResonance(new_reso);
        break;
    }
    default:
        break;
    }
}

void HardwareControls::init()
{
    encoder[0] = new Encoder(rotaryPin[0][0], rotaryPin[0][1]);
    encoder[1] = new Encoder(rotaryPin[1][0], rotaryPin[1][1]);

    //Set 16ch multiplexer control pins to mode OUTPUT
    for (uint8_t i = 0; i < 4; i++)
        pinMode(muxControlPin[i], OUTPUT);

    //Read current hardware control values to memory, but do not update synth parameters
    checkControlValues(false);

    //Initialize potentiometer threshold array
    for (uint8_t i = 0; i < LAST_CTL; i++)
        potThreshold[i] = POT_THRESHOLD;
}