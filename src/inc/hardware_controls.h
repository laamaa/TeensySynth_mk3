#ifndef HARDWARE_CONTROLS_H
#define HARDWARE_CONTROLS_H

#include <Arduino.h>
#include "synth.h"
#include <Encoder.h>
#include <Bounce2.h>
#include "gui.h"

namespace TeensySynth
{

    class HardwareControls
    {
    public:
        HardwareControls(Synth *ptrSynth, GUI *ptrGui, Settings *ptrSettings)
        {
            ts = ptrSynth;
            gui = ptrGui;
            settings = ptrSettings;
        }

        inline void update()
        {
            checkControlValues(true);
        }

        void init();

    private:
        //Teensy pins where 16ch multiplexer control pins are connected
        const uint8_t muxControlPin[4] = {2, 3, 4, 5};

        //Teensy pin where 16ch multiplexer value pin is connected
        const uint8_t muxValuePin = 14;

        //Teensy pins where rotary encoders are connected. First two in array are the encoder pins, 3. is the encoder push button
        const uint8_t rotaryPin[2][3] = {{8, 9, 10}, {12, 11, 15}};

        //HW control layout enum: potentiometers first (left to right, top to bottom), encoders second and switches last
        enum HwControl
        {
            CTL_HARMONICS,
            CTL_TIMBRE,
            CTL_MORPH,
            CTL_DECAY,
            CTL_BALANCE,
            CTL_LPG_COLOUR,
            CTL_FREQMOD_AMOUNT,
            CTL_TIMBREMOD_AMOUNT,
            CTL_MORPHMOD_AMOUNT,
            CTL_LFO_DEPTH,
            CTL_VOLUME,
            CTL_REV_SIZE,
            CTL_REV_DEPTH,
            CTL_CHORUS_DEPTH,
            CTL_FLT_RESO,
            CTL_FLT_CUTOFF,
            CTL_ENC_1,
            CTL_ENC_2,
            CTL_SW_1,
            CTL_SW_2,
            LAST_CTL // Not an actual control, just a value for allocating arrays
        };

        //Signal multiplexer channel select binary values
        const uint8_t muxChannel[16][4] = {
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

        // Pointer to the synth controller
        Synth *ts;

        // Pointer to GUI (display) controller
        GUI *gui;

        // Pointer to Settings controller
        Settings *settings;

        // Use Teensy's encoder library to make working with the encoders easier
        Encoder *encoder[2];

        // Create bounce library instances for handling button presses
        Bounce button[2] = {Bounce()};

        // Current pot/switch readings
        int16_t currentCtlValue[LAST_CTL];

        // Current encoders in the synth hw give out non-standard readings so we need to have special treatment for them
        int16_t encValue[2];

        /* Thresholds for updating parameters from potentiometer readings
         * The default value is defined in settings.h, but this is also an array 
         * so that we can change thresholds also individually if there is a single noisy pot 
         */
        uint8_t potThreshold[LAST_CTL];

        // Switch multiplexer channel and read a value
        inline uint16_t readMux(uint8_t ch)
        {
            for (int i = 0; i < 4; i++)
                digitalWrite(muxControlPin[i], muxChannel[ch][i]);
            delayMicroseconds(5); // A little delay so the multiplexer can sort itself out after switching channels
            return analogRead(muxValuePin);
        }

        // Sends the control changes to the synth engine
        void updateTeensySynth(uint8_t ctl, int value);

        // Check encoder readings. Sorry for this mess.
        void readAndProcessPotentiometers(bool update);
        void readAndProcessEncoders(bool update);
        void checkControlValues(bool update);

        // Return true if latch is disabled or value is inside the latch threshold range
        inline bool valueInLatchRange(float newValue, float originalValue)
        {
            return (!settings->getLatch() || (newValue > originalValue - settings->getLatchThreshold() && newValue < originalValue + settings->getLatchThreshold()));
        }
    };
} // namespace TeensySynth

#endif