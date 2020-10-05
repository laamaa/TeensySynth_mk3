#ifndef OLED_DISPLAY_H_
#define OLED_DISPLAY_H_

#include "core_pins.h"
#include <OneBitDisplay.h>
#include "settings.h"
#include "synth.h"

namespace TeensySynth
{

    class GUI
    {
    public:
        GUI(TeensySynth::Synth *tsPointer);

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

        inline void resetMenuTimer()
        {
            menuTimer = 0;
        }

        void clearDisplay()
        {
            obdFill(&obd, 0, 1);
        }

        char *menuCallback(int iIndex);

    private:
        //List of synth engine names
        char synthEngineList[16][11] = {
            "ANALOG    ",
            "WAVESHAPER",
            "FM        ",
            "GRAIN     ",
            "ADDITIVE  ",
            "WAVETABLE ",
            "CHORD     ",
            "SPEECH    ",
            "SWARM     ",
            "NOISE     ",
            "PARTICLE  ",
            "STRING    ",
            "MODAL     ",
            "BASSDRUM  ",
            "SNARE     ",
            "HIHAT     "};

        char settingValueList[17][3] = {
            "01",
            "02",
            "03",
            "04",
            "05",
            "06",
            "07",
            "08",
            "09",
            "10",
            "11",
            "12",
            "13",
            "14",
            "15",
            "16",
            "  "};

        char *menu1[6];

        uint8_t setting[4] = {0, 0, 0, 0};
        char *szOnOff[2];

        elapsedMillis menuTimer;

        Synth *ts;

        SIMPLEMENU sm;
        OBDISP obd;
        uint8_t pBuffer[1024]; //display buffer

        int8_t currentMenuItem = -1;

        inline void initMenu();
        void handleMenuEventOk(uint8_t control);
    };

} // namespace TeensySynth
#endif