#ifndef MIDI_CONTROLS_H_
#define MIDI_CONTROLS_H_

#include "synth.h"

namespace TeensySynth
{
    class MidiControls
    {
    public:
        MidiControls(Synth *synth)
        {
            ts = synth;
        }

        void update();
        void init();

    private:
        Synth *ts;
        
        void OnControlChange(uint8_t channel, uint8_t control, uint8_t value);
        void OnProgramChange(uint8_t channel, uint8_t program);
    };
} // namespace TeensySynth

#endif