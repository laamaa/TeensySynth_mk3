#ifndef HARDWARE_CONTROLS_H
#define HARDWARE_CONTROLS_H

#include <Arduino.h>
#include "teensy_synth.h"

class HardwareControls
{
public:
    HardwareControls(TeensySynth *tsPointer)
    {
        init();
        ts = tsPointer;
    }

    void update()
    {
        checkControlValues(1);
    }

private:
    //Teensy pins where 16ch multiplexer control pins are connected
    const uint8_t muxControlPin[4] = {2, 3, 4, 5};

    //Teensy pin where 16ch multiplexer value pin is connected
    const uint8_t muxValuePin = 14;

    //Teensy pins where rotary encoders are connected
    const uint8_t rotaryPin[2][2] = {{8, 9}, {10, 11}};

    //HW control layout enum: potentiometers first (left to right, top to bottom), encoders second and switches last
    enum HwControl
    {
        CTL_HARMONICS,
        CTL_TIMBRE,
        CTL_MORPH,
        CTL_BALANCE,
        CTL_DECAY,
        CTL_AMP_DEC,
        CTL_AMP_SUS,
        CTL_AMP_REL,
        CTL_LFO_RATE,
        CTL_LFO_DEPTH,
        CTL_FLT_ATK,
        CTL_FLT_DEC,
        CTL_FLT_SUS,
        CTL_FLT_DEPTH,
        CTL_FLT_RESO,
        CTL_FLT_CUTOFF,
        CTL_ENC_1,
        CTL_ENC_2,
        CTL_SW_1,
        CTL_SW_2,
        LAST_CTL
    };

    //Signal multiplexer channel select binary values
    const int muxChannel[16][4] = {
        {0, 0, 0, 0}, //channel 0
        {1, 0, 0, 0}, //channel 1
        {0, 1, 0, 0}, //channel 2
        {1, 1, 0, 0}, //channel 3
        {0, 0, 1, 0}, //channel 4
        {1, 0, 1, 0}, //channel 5
        {0, 1, 1, 0}, //channel 6
        {1, 1, 1, 0}, //channel 7
        {0, 0, 0, 1}, //channel 8
        {1, 0, 0, 1}, //channel 9
        {0, 1, 0, 1}, //channel 10
        {1, 1, 0, 1}, //channel 11
        {0, 0, 1, 1}, //channel 12
        {1, 0, 1, 1}, //channel 13
        {0, 1, 1, 1}, //channel 14
        {1, 1, 1, 1}  //channel 15
    };

    TeensySynth *ts;

    //Current pot/switch readings
    int currentCtlValue[LAST_CTL];

    /* Thresholds for updating parameters from potentiometer readings
     * The default value is defined in settings.h, but this is also an array 
     * so that we can change thresholds also individually if there is a single noisy pot */
    uint8_t potThreshold[LAST_CTL];

    //Switch multiplexer channel and read a value
    uint16_t readMux(uint8_t ch)
    {
        for (int i = 0; i < 4; i++)
            digitalWrite(muxControlPin[i], muxChannel[ch][i]);
        delayMicroseconds(5); // A little delay so the multiplexer can sort itself out after switching channels
        return analogRead(muxValuePin);
    }

    void updateTeensySynth(uint8_t ctl, int value);

    void checkControlValues(bool update);

    void init();
};

#endif