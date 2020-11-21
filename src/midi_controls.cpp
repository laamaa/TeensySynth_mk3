#include "inc/midi_controls.h"
#include "inc/settings.h"
#include <MIDI.h>

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, serialMidi);

namespace TeensySynth
{
    void MidiControls::init()
    {
        serialMidi.begin();      
    }

    void MidiControls::update()
    {
        if (usbMIDI.read(settings->getMidiChannel()))
            MessageHandler(usbMIDI.getType(), usbMIDI.getChannel(), usbMIDI.getData1(), usbMIDI.getData2());
        if (serialMidi.read(settings->getMidiChannel())) {
            MessageHandler(serialMidi.getType(),serialMidi.getChannel(), serialMidi.getData1(), serialMidi.getData2());
        }
            
    }

    void MidiControls::MessageHandler(byte midiMessageType, byte channel, byte data1, byte data2)
    {
        switch (midiMessageType)
        {
        case midi::NoteOn:
            ts->Synth::noteOn(channel, data1, data2);
            break;
        case midi::NoteOff:
            ts->Synth::noteOff(channel, data1, data2);
            break;
        case midi::ControlChange:
            MidiControls::OnControlChange(channel, data1, data2);
            break;
        default:
            break;
        }
    }

    void MidiControls::OnControlChange(uint8_t channel, uint8_t control, uint8_t value)
    {
        switch (control)
        {
        case ControlChange::Modulation:
            break;
        default:
            break;
        }
    }

} // namespace TeensySynth