//************LIBRARIES USED**************
#include <Arduino.h>
#include "inc/settings.h"
#include "inc/synth.h"
#include "inc/gui.h"
//#include "inc/oscilloscope.h"
#include "inc/hardware_controls.h"
#include "inc/midi_controls.h"

using namespace TeensySynth;

Settings settings;
Synth ts(&settings);
GUI gui(&ts, &settings);
HardwareControls hw(&ts, &gui, &settings);
MidiControls midi(&ts, &settings);
//Oscilloscope oscilloscope(&ts,&gui); //unfortunately I2C is too slow for this to work nicely :((

//************SETUP**************
void setup()
{
    ts.init();
    gui.init();
    hw.init();
    midi.init();
}

//************LOOP**************
void loop()
{
    midi.update();
    hw.update();
    gui.update();

#if SYNTH_DEBUG > 0
    ts.performanceCheck();
    while (Serial.available())
        ts.selectCommand(Serial.read());
#endif
}
