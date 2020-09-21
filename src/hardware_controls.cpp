#include "hardware_controls.h"
#include "settings.h"

void HardwareControls::checkControlValues(uint8_t update)
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
    case CTL_FLT_CUTOFF:
        ts->setFilterCutoff(30 + 12 * powf((value / 101.53), 3));
        break;
    case CTL_FLT_RESO:
        ts->setFilterResonance(((float)value / 1024.0f) * 4.0f);
        break;
    default:
        break;
    }
}

void HardwareControls::initPins()
{
    for (uint8_t i = 0; i < 4; i++)
        pinMode(muxControlPin[i], OUTPUT);
    checkControlValues(0);
    
    //Initialize potentiometer threshold array
    for (uint8_t i = 0; i < LAST_CTL; i++)
        potThreshold[i] = POT_THRESHOLD;
}