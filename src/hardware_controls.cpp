#include "hardware_controls.h"
#include "settings.h"

void HardwareControls::checkControlValues(bool update)
{
    //Potentiometers: all are connected to a single multiplexer so we just read them in a for loop
    for (int i = 0; i < 16; i++)
    {
        int ctlValue = readMux(i);
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
}

void HardwareControls::updateTeensySynth(uint8_t ctl, int value)
{
    switch (ctl)
    {
    case CTL_LFO_RATE:
        ts->setSynthEngine((value / 1023) *10);
    case CTL_HARMONICS:
        ts->setOscillatorHarmonics((float)value / 1023.0f);
        break;
    case CTL_MORPH:
        ts->setOscillatorMorph((float)value / 1023.0f);
        break;
    case CTL_TIMBRE:
        ts->setOscillatorTimbre((float)value / 1023.0f);
        break;
    case CTL_FLT_CUTOFF:
    {
        float new_cutoff = 30 + 12 * powf((value / 101.53), 3);
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
    //Set 16ch multiplexer control pins to mode OUTPUT
    for (uint8_t i = 0; i < 4; i++)
        pinMode(muxControlPin[i], OUTPUT);

    //Read current hardware control values to memory, but do not update synth parameters
    checkControlValues(false);

    //Initialize potentiometer threshold array
    for (uint8_t i = 0; i < LAST_CTL; i++)
        potThreshold[i] = POT_THRESHOLD;
}