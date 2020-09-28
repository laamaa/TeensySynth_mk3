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
        void OnNoteOn(uint8_t channel, uint8_t note, uint8_t velocity);
        void OnNoteOff(uint8_t channel, uint8_t note, uint8_t velocity);
        void OnAfterTouchPoly(uint8_t channel, uint8_t note, uint8_t value);
        void OnControlChange(uint8_t channel, uint8_t control, uint8_t value);
        void OnPitchChange(uint8_t channel, int pitch);
        void OnProgramChange(uint8_t channel, uint8_t program);
        void OnAfterTouch(uint8_t channel, uint8_t pressure);
        void OnSysEx( const uint8_t *data, uint16_t length, bool complete);
        void OnRealTimeSystem(uint8_t realtimebyte);
    };
} // namespace TeensySynth

#endif