#include "inc/midi_controls.h"
#include "inc/settings.h"

namespace TeensySynth
{
    void MidiControls::init()
    {

    }

    void MidiControls::update()
    {
        while (usbMIDI.read(SYNTH_MIDICHANNEL))
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
    { //todo
    }

} // namespace TeensySynth