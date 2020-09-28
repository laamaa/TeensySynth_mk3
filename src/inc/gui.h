#ifndef OLED_DISPLAY_H_
#define OLED_DISPLAY_H_

#include <U8g2lib.h>
#include "synth.h"

namespace TeensySynth
{
    class GUI
    {
    public:
        GUI(Synth *tsPointer)
        {
            ts = tsPointer;
            init();
        }

        void update();
        void init();

    private:
        U8G2_SH1106_128X64_NONAME_1_SW_I2C *u8g2;
        Synth *ts;
        
        //List of synth engine names
        char synthEngine[16][11] = {"Analog",
                                    "Waveshaper",
                                    "FM",
                                    "Grain",
                                    "Additive",
                                    "Wavetable",
                                    "Chord",
                                    "Speech",
                                    "Swarm",
                                    "Noise",
                                    "Particle",
                                    "String",
                                    "Modal",
                                    "Bassdrum",
                                    "Snare",
                                    "Hihat"};
    };
} // namespace TeensySynth
#endif