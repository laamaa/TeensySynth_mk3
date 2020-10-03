#ifndef OLED_DISPLAY_H_
#define OLED_DISPLAY_H_

#include "core_pins.h"
#include <OneBitDisplay.h>
#include "synth.h"

namespace TeensySynth
{

    class GUI
    {
    public:
        GUI(TeensySynth::Synth *tsPointer)
        {
            ts = tsPointer;
        }

        enum EventType
        {
            EVENT_NEXT,
            EVENT_PREV,
            EVENT_OK,
        };

        void init();        

        void update();

        void menuEvent(uint8_t control, EventType eventType);

        void drawPixel(uint8_t x, uint8_t y, uint8_t color)
        {
            obdSetPixel(&obd, x, y, color, 1);
        }

        bool updateDisplay = true;
        bool menuIsOpen = true;

        inline void resetMenuTimer(){
            menuTimer = 0;
        }

        void clearDisplay()
        {
            obdFill(&obd, 0, 1);
        }

        char * myCallback(int iIndex)
        {
            return szOnOff[ucToggle[iIndex]];
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

        char *menu1[4];

        uint8_t ucToggle[3] = {0, 1, 0};
        char *szOnOff[2];

        elapsedMillis menuTimer;

        Synth *ts;

        SIMPLEMENU sm;
        OBDISP obd;
        uint8_t pBuffer[1056]; //display buffer

        int8_t currentMenuItem = -1;
        
    };

} // namespace TeensySynth
#endif