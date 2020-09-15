//************LIBRARIES USED**************
#include "settings.h"
#include "teensy_synth.h"

TeensySynth ts;

IntervalTimer myTimer;

// Debug functions
#if SYNTH_DEBUG > 0
float statsCpu = 0;
uint8_t statsMemI16 = 0;
uint8_t statsMemF32 = 0;

inline void printResources(float cpu, uint8_t memF32, uint8_t memI16)
{
    Serial.printf("CPU Usage: %.2f%%, Memory: %d (F32) %d (I16)\n", cpu, memF32, memI16);
}

void performanceCheck()
{
    static unsigned long last = 0;
    unsigned long now = millis();
    if ((now - last) > 1000)
    {
        last = now;
        float cpu = AudioProcessorUsageMax();
        uint8_t memI16 = AudioMemoryUsageMax();
        uint8_t memF32 = AudioMemoryUsageMax_F32();
        if ((statsMemF32 != memF32) || (statsMemI16 != memI16) || fabs(statsCpu - cpu) > 1)
        {
            printResources(cpu, memF32, memI16);
        }
        AudioProcessorUsageMaxReset();
        AudioMemoryUsageMaxReset();
        AudioMemoryUsageMaxReset_F32();
        last = now;
        statsCpu = cpu;
        statsMemF32 = memF32;
        statsMemI16 = memI16;
    }
}

void selectCommand(char c)
{
    switch (c)
    {
    case '\r':
        Serial.println();
        break;
    case 's':
        // print cpu and mem usage
        printResources(statsCpu, statsMemF32, statsMemI16);
        break;
    case '\t':
        // reboot Teensy
        *(uint32_t *)0xE000ED0C = 0x5FA0004;
        break;
    default:
        break;
    }
}
#endif

void readMidi()
{
    if (usbMIDI.read())
    {
        switch (usbMIDI.getType())
        {
        case usbMIDI.NoteOn:
            ts.OnNoteOn(usbMIDI.getChannel(), usbMIDI.getData1(), usbMIDI.getData2());
            break;
        case usbMIDI.NoteOff:
            ts.OnNoteOff(usbMIDI.getChannel(),usbMIDI.getData1(), usbMIDI.getData2());
            break;
        case usbMIDI.ControlChange:
            ts.OnControlChange(usbMIDI.getChannel(),usbMIDI.getData1(), usbMIDI.getData2());
            break;
        default:
            break;
        }
    }
}

//************SETUP**************
void setup()
{
#if SYNTH_DEBUG > 0
    // Open serial communications and wait for port to open:
    Serial.begin(115200);
#endif
    //Allocate audio memory. Floating point and integer versions need their own blocks.
    AudioMemory(2);
    AudioMemory_F32(20);

    //Initialize the synth only after Serial is ok and audiomemory is allocated
    ts.init(); 
}

//************LOOP**************
void loop()
{
    readMidi();

#if SYNTH_DEBUG > 0
    performanceCheck();
    while (Serial.available())
        selectCommand(Serial.read());
#endif
}
