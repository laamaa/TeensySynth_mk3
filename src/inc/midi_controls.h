#ifndef MIDI_CONTROLS_H_
#define MIDI_CONTROLS_H_

#include "synth.h"
#include "settings.h"

namespace TeensySynth
{
    class MidiControls
    {
    public:
        MidiControls(Synth *ptrSynth, Settings *ptrSettings)
        {
            ts = ptrSynth;
            settings = ptrSettings;
        }

        void update();
        void init();

    private:
        Synth *ts;
        Settings *settings;
        
        void OnControlChange(uint8_t channel, uint8_t control, uint8_t value);
        void OnProgramChange(uint8_t channel, uint8_t program);
    };
} // namespace TeensySynth

#endif