#ifndef OSCILLOSCOPE_H_
#define OSCILLOSCOPE_H_
#include "gui.h"
#include "AudioStream.h"

namespace TeensySynth
{
    class Oscilloscope : public AudioStream
    {
    public:
        Oscilloscope(TeensySynth::Synth *ts, TeensySynth::GUI *gui) : AudioStream(1, inputQueueArray)
        {
            display = gui;
            synth = ts;
            patchConverterOscilloscope = new AudioConnection(ts->getMasterL(), 0, *this, 0);
        }
        ~Oscilloscope()
        {
            delete patchConverterOscilloscope;
        }
        virtual void update(void);
        void AudioToPixel(int16_t *);
        int16_t color;
        int16_t backgroundcolor;

    private:
        AudioConnection *patchConverterOscilloscope;
        TeensySynth::Synth *synth;
        audio_block_t *inputQueueArray[1];
        volatile bool new_output;
        GUI *display;
        int16_t old_val[AUDIO_BLOCK_SAMPLES];
        unsigned long last = millis();
    };
} // namespace TeensySynth
#endif