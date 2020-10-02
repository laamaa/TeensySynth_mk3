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
            init();
        }

        void update();

    private:
        Synth *ts;
        void init();
        void OnControlChange(uint8_t channel, uint8_t control, uint8_t value);
        void OnProgramChange(uint8_t channel, uint8_t program);
    };
} // namespace TeensySynth

#endif