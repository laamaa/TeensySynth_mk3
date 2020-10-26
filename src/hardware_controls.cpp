#include "inc/hardware_controls.h"
#include "inc/settings.h"
#include <Encoder.h>

namespace TeensySynth
{
    void HardwareControls::readAndProcessEncoders(bool update)
    {
        int i, ctlValue;

        for (i = 0; i < 2; i++)
        {
            //Read encoder button presses
            button[i].update();
            if (button[i].rose())
            {
                //Send an OK/Enter event to GUI object, further handling is done there.
                gui->menuEvent(i, GUI::EventType::EVENT_OK);
            }

            //Read the current encoder turning direction
            ctlValue = encoder[i]->readAndReset();
            //The crappy encoders that I bought from Aliexpress seem to be like 1 click = 4 value increments.. trying to work around that
            if (ctlValue < 0)
            {
                //If the encoder has switched directions before reaching 4 value increments, reset the value
                if (encValue[i] < 0)
                    encValue[i] = 0;
                encValue[i]++;
                if (encValue[i] % 4 == 0)
                {
                    //If menu is open in the GUI, send an event to the GUI object. Otherwise send the message to synth object.
                    if (gui->menuIsOpen == true)
                    {
                        gui->menuEvent(i, GUI::EventType::EVENT_NEXT);
#if SYNTH_DEBUG > 1
                        Serial.println(F("Menu active, EVENT_NEXT"));
#endif
                    }
                    else
                    {
#if SYNTH_DEBUG > 1
                        Serial.println(F("Encoder value increase"));
#endif
                        currentCtlValue[CTL_ENC_1 + i]++;
                        if (update)
                        {
                            updateTeensySynth(CTL_ENC_1 + i, currentCtlValue[CTL_ENC_1 + i]);
                            gui->updateDisplay = true;
                        }
                    }
                }
            }
            else if (ctlValue > 0)
            {
                if (encValue[i] > 0)
                    encValue[i] = 0;
                encValue[i]--;
                if (encValue[i] % 4 == 0)
                {
                    if (gui->menuIsOpen == true)
                    {
                        gui->menuEvent(i, GUI::EventType::EVENT_PREV);
#if SYNTH_DEBUG > 1
                        Serial.println(F("Menu active, EVENT_NEXT"));
#endif
                    }
                    else
                    {
#if SYNTH_DEBUG > 1
                        Serial.println(F("Encoder value decrease"));
#endif
                        currentCtlValue[CTL_ENC_1 + i]--;
                        if (update)
                        {
                            updateTeensySynth(CTL_ENC_1 + i, currentCtlValue[CTL_ENC_1 + i]);
                            gui->updateDisplay = true;
                        }
                    }
                }
            }
        }
    }

    void HardwareControls::readAndProcessPotentiometers(bool update)
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
                    Serial.printf("Pot: %d, V: %d, cV: %d, t: %d\n", i, ctlValue, currentCtlValue[i], potThreshold[i]);
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

    void HardwareControls::checkControlValues(bool update)
    {
        readAndProcessPotentiometers(update);
        readAndProcessEncoders(update);
    }

    void HardwareControls::updateTeensySynth(uint8_t ctl, int value)
    {
        float newValue = 0;
        switch (ctl)
        {
        case CTL_ENC_1:
            //Enc1 = set synth engine
            CONSTRAIN(currentCtlValue[CTL_ENC_1], 0, 15); //16 is the number of synth engines available
            ts->setSynthEngine(currentCtlValue[CTL_ENC_1]);
            break;
        case CTL_ENC_2:
            //Enc2 = Load preset
            CONSTRAIN(currentCtlValue[CTL_ENC_2], 0, PRESETS - 1);
            ts->loadPreset(currentCtlValue[CTL_ENC_2]);
            break;
        case CTL_SW_1:
            //SW1 = mono/poly
            break;
        case CTL_SW_2:
            //SW2 = select knob layer 1/2
            break;
        case CTL_HARMONICS:
            newValue = (float)value / 1023.0f;
            if (valueInLatchRange(newValue, ts->getOscillatorHarmonics()))
                ts->setOscillatorHarmonics(newValue);
            break;
        case CTL_MORPH:
            newValue = (float)value / 1023.0f;
            if (valueInLatchRange(newValue, ts->getOscillatorMorph()))
                ts->setOscillatorMorph(newValue);
            break;
        case CTL_TIMBRE:
            newValue = (float)value / 1023.0f;
            if (valueInLatchRange(newValue, ts->getOscillatorTimbre()))
                ts->setOscillatorTimbre(newValue);
            break;
        case CTL_DECAY:
            newValue = (float)value / 1023.0f;
            if (valueInLatchRange(newValue, ts->getOscillatorDecay()))
                ts->setOscillatorDecay(newValue);
            break;
        case CTL_BALANCE:
            newValue = (float)value / 1023.0f;
            if (valueInLatchRange(newValue, ts->getOscillatorBalance()))
                ts->setOscillatorBalance(newValue);
            break;
        case CTL_LPG_COLOUR:
            newValue = (float)value / 1023.0f;
            if (valueInLatchRange(newValue, ts->getLpgColour()))
                ts->setLpgColour(newValue);
            break;
        case CTL_FREQMOD_AMOUNT:
            newValue = -1.0f + ((float)value / 511.5f);
            if (valueInLatchRange(newValue, ts->getFreqMod()))
                ts->setFreqMod(newValue);
            break;            
        case CTL_TIMBREMOD_AMOUNT:
            newValue = -1.0f + ((float)value / 511.5f);
            if (valueInLatchRange(newValue, ts->getTimbreMod()))
                ts->setTimbreMod(newValue);
            break;
        case CTL_MORPHMOD_AMOUNT:
            newValue = -1.0f + ((float)value / 511.5f);
            if (valueInLatchRange(newValue, ts->getMorphMod()))
                ts->setMorphMod(newValue);
            break;
        case CTL_VOLUME:
            newValue = (float)value / 1023.0f;
            if (valueInLatchRange(newValue, ts->getVolume()))
                ts->setVolume(newValue);
            break;
        case CTL_REV_SIZE:
            newValue = (float)value / 1023.0f;
            if (valueInLatchRange(newValue, ts->getReverbSize()))
                ts->setReverbSize(newValue);
            break;
        case CTL_REV_DEPTH:
            newValue = powf((value / 101.53), 3) / 1023.0f * MIX_LEVEL;
            if (valueInLatchRange(newValue, ts->getReverbDepth()))
                ts->setReverbDepth(newValue);
            break;
        case CTL_CHORUS_DEPTH:
            newValue = (float)value / 1023.0f;
            if (valueInLatchRange(newValue, ts->getChorusDepth()))
                ts->setChorusDepth(newValue);
            break;
        case CTL_FLT_CUTOFF:
            newValue = 30 + 12 * powf((value / 101.53), 3);
            //if (valueInLatchRange(newValue, ts->getFilterCutoff()))
            //{
            // The filter has a pretty nasty self-oscillation in higher frequencies when the Q is large enough, trying to prevent this..
            if (ts->getFilterResonance() > 3.07 && newValue > 10000)
                ts->setFilterResonance(3.0f);
            ts->setFilterCutoff(newValue);
            //}
            break;
        case CTL_FLT_RESO:
            newValue = ((float)value / 1023.0f) * 4.0f;
            //if (valueInLatchRange(newValue, ts->getFilterResonance()))
            //{
            if (ts->getFilterCutoff() > 10000 && newValue > 3.0)
                newValue = 3.0f;
            ts->setFilterResonance(newValue);
            //}
            break;
        default:
            break;
        }
    }

    void HardwareControls::init()
    {
        encoder[0] = new Encoder(rotaryPin[0][0], rotaryPin[0][1]);
        encoder[1] = new Encoder(rotaryPin[1][0], rotaryPin[1][1]);

        //Setup button pins for bounce library
        button[0].attach(rotaryPin[0][2], INPUT);
        button[1].attach(rotaryPin[1][2], INPUT);
        button[0].interval(100);
        button[1].interval(100);

        //Set 16ch multiplexer control pins to mode OUTPUT
        for (uint8_t i = 0; i < 4; i++)
            pinMode(muxControlPin[i], OUTPUT);

        //Read current hardware control values to memory, but do not update synth parameters
        checkControlValues(false);

        //Initialize potentiometer threshold array
        for (uint8_t i = 0; i < LAST_CTL; i++)
            potThreshold[i] = POT_THRESHOLD;
    }
} // namespace TeensySynth