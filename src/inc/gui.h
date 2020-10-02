#ifndef OLED_DISPLAY_H_
#define OLED_DISPLAY_H_

#include "core_pins.h"
#include "AudioStream.h"
#include <U8g2lib.h>
#include "synth.h"

namespace TeensySynth
{

    class GUI
    {
    public:
        GUI(TeensySynth::Synth *tsPointer)
        {
            ts = tsPointer;
            init();
        }

        enum EventType
        {
            EVENT_NEXT,
            EVENT_PREV,
            EVENT_OK,
        };

        void update();
        void menuEvent(uint8_t control, EventType eventType);
        void drawPixel(uint16_t x, uint16_t y)
        {
            u8g2->drawPixel(x, y);
        }
        void setDrawColor(uint8_t color)
        {
            u8g2->setDrawColor(color);
        }

        bool updateDisplay = true;
        void clearBuffer() {
            u8g2->clearBuffer();
        }        

    private:
        //List of synth engine names
        /*         const char synthEngineList[16][11] = {
            "Analog",
            "Waveshaper",
            "FM",
            "Grain",
            "Additive",
            "Wavetable",
            "Chord",
            "Speech",
            "Swarm",
            "Noise",
            "Particle",
            "String",
            "Modal",
            "Bassdrum",
            "Snare",
            "Hihat"};

        const char mainMenuList[3][14] = {
            "Load preset",
            "Save preset",
            "Select engine"}; */

        U8G2_SH1106_128X64_NONAME_F_HW_I2C *u8g2;

        Synth *ts;

        uint8_t currentSelection[2] = {0};
        uint8_t currentMenu = 0;
        void init();
    };

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
        unsigned long last = millis;
    };

} // namespace TeensySynth
#endif