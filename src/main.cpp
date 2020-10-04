//************LIBRARIES USED**************
#include <Arduino.h>
#include "inc/settings.h"
#include "inc/synth.h"
#include "inc/hardware_controls.h"
#include "inc/gui.h"
//#include "inc/oscilloscope.h"
#include "inc/progmem_strings.h"
#include "inc/midi_controls.h"

using namespace TeensySynth;

Synth ts;
GUI gui(&ts);
HardwareControls hw(&ts, &gui);
MidiControls midi(&ts);
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
