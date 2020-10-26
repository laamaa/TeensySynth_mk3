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
        GUI(TeensySynth::Synth *ptrSynth, TeensySynth::Settings *ptrSettings);

        //Does the display need to be refreshed?
        bool updateDisplay = true;

        //Should the main menu be visible?
        bool menuIsOpen = false;

        //Menu event codes
        enum EventType
        {
            EVENT_NEXT,
            EVENT_PREV,
            EVENT_OK,
        };

        //Initialize OneBitDisplay library
        void init();

        //Main GUI update loop
        void update();

        //Menu event handler. Takes a controller number and type of event as parameter
        void menuEvent(uint8_t control, EventType eventType);

        //Wrapper for drawing a pixel on the screen
        inline void drawPixel(uint8_t x, uint8_t y, uint8_t color)
        {
            obdSetPixel(&obd, x, y, color, 1);
        }

        //Resets the timer that's used to calculate when to hide the menu
        inline void resetMenuTimer()
        {
            menuTimer = 0;
        }

        //Draws a big box so we won't see a thing
        inline void clearDisplay()
        {
            obdFill(&obd, 0, 1);
        }

        //Callback function for showing values in the main menu
        char *menuCallback(int iIndex);

    private:
        //List of synth engine names
        char synthEngineList[16][11] = {
            " ANALOG ",
            "WAVESHAP",
            "   FM   ",
            " GRAIN  ",
            "ADDITIVE",
            "WAVETABL",
            " CHORD  ",
            " SPEECH ",
            " SWARM  ",
            " NOISE  ",
            "PARTICLE",
            " STRING ",
            " MODAL  ",
            "BASSDRUM",
            " SNARE  ",
            " HI-HAT "};

        //Table for converting int to char*
        char charNumbers[17][3] = {
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

        char *textSaved = (char *)"Saved";

        char *menuCallBackStr = (char *)"    ";

        // Main menu table
        char *menu1[6];     

        // Pointer to currently open menu
        char **currentMenu = menu1;

        // Current values in main menu items
        uint8_t setting[4] = {0, 0, 0, 0};

        // Time since last menu action in ms
        elapsedMillis menuTimer;

        // Flash text stuff
        elapsedMillis flashTextTimer;
        bool showFlashText = false;

        // Pointer to main synth object
        Synth *ts;

        // Pointer to settings object
        Settings *settings;

        // Objects needed by OneBitDisplay library
        SIMPLEMENU sm;
        OBDISP obd;
        uint8_t pBuffer[1024]; //display buffer

        // Currently selected menu item
        int8_t currentMenuItem = -1;

        // Should the main menu be initialized when opening?
        bool menuIsInitialized = false;

        // Handle OK (enter) menu events. Takes a controller number as parameter.
        void handleMenuEventOk(uint8_t control);

        void flashText(char* text);

    };

} // namespace TeensySynth
#endif