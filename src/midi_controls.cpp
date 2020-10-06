#include "inc/midi_controls.h"
#include "inc/settings.h"

// MIDI CC numbers
#define CC_MODULATION 1
/* #define CC_PORTAMENTO_TIME 5
#define CC_VOLUME 7
#define CC_FIX_VOLUME 9
#define CC_PANORAMA 10
#define CC_FLT_FREQ 74
#define CC_FLT_RES 71
#define CC_POLY_MODE 18
#define CC_ENV_MODE 19
#define CC_ENV_DELAY 20
#define CC_ENV_HOLD 21
#define CC_ENV_ATK 73
#define CC_ENV_DECAY 75
#define CC_ENV_SUSTAIN 76
#define CC_ENV_REL 72
#define CC_PULSE_WIDTH 14
#define CC_PB_RANGE 25
#define CC_FLT_LFO_RATE 12
#define CC_FLT_LFO_DEPTH 13
#define CC_FLT_ENV_ATK 77
#define CC_FLT_ENV_DECAY 78
#define CC_FLT_ENV_SUSTAIN 79
#define CC_FLT_ENV_DEPTH 70
#define CC_PITCH_LFO_RATE 22
#define CC_PITCH_LFO_DEPTH 23
#define CC_PITCH_LFO_MODE 24
#define CC_OSC_SHAPE 26
#define CC_OSC_COLOR 27
#define CC_OSC_TIMBRE 28
#define CC_OSC_DETUNE 94
#define CC_OSC_FM 29
#define CC_SELECT_PRESET 30
#define CC_LOAD_PRESET 85
#define CC_SAVE_PRESET 86
#define CC_CHORUS_TOGGLE 93
#define CC_FILTER_DRIVE 87
#define CC_SUSTAIN 64
#define CC_PORTAMENTO 65
#define CC_PORTAMENTO_CONTROL 84
#define CC_ALL_NOTES_OFF 123
#define CC_OMNI_OFF 124
#define CC_OMNI_ON 125
#define CC_MONO_ON 126
#define CC_POLY_ON 127 */

namespace TeensySynth
{
    void MidiControls::init()
    {

    }

    void MidiControls::update()
    {
        while (usbMIDI.read(settings->getMidiChannel()))
        {
            switch (usbMIDI.getType())
            {
            case usbMIDI.NoteOn:
                ts->Synth::noteOn(usbMIDI.getChannel(), usbMIDI.getData1(), usbMIDI.getData2());
                break;
            case usbMIDI.NoteOff:
                ts->Synth::noteOff(usbMIDI.getChannel(), usbMIDI.getData1(), usbMIDI.getData2());
                break;
            case usbMIDI.ControlChange:
                MidiControls::OnControlChange(usbMIDI.getChannel(), usbMIDI.getData1(), usbMIDI.getData2());
                break;
            default:
                break;
            }
        }
    }

    void MidiControls::OnControlChange(uint8_t channel, uint8_t control, uint8_t value)
    {
        switch (control){
            case CC_MODULATION:
                
                break;
            default:
                break;
        }
    }

} // namespace TeensySynth