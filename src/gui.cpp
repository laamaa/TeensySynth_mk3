#include "inc/gui.h"

TeensySynth::GUI *ptrGui;

/*
 * Define a global function for display library's menu callback
 * this is probably the easiest way to get this working inside a class.
 */
char *GlobalMenuCallback(int iIndex)
{
    //reroute to function inside of the class
    return ptrGui->menuCallback(iIndex);
}

namespace TeensySynth
{
    GUI::GUI(TeensySynth::Synth *tsPointer)
    {
        ptrGui = this;
        ts = tsPointer;
    }

    void GUI::update()
    {
        if (menuIsOpen)
        {
            if (updateDisplay)
            {
                obdMenuShow(&sm, currentMenuItem);
                updateDisplay = false;
            }
            if (menuTimer > MENU_TIMEOUT)
            {
                menuIsOpen = false;
                clearDisplay();
                updateDisplay = true;
            }
        }
        else
        {
            if (updateDisplay)
            {
                obdDrawLine(&obd, 0, 32, 127, 32, 1, 1);
                obdWriteString(&obd, 0, 0, 0, synthEngineList[ts->getSynthEngine()], FONT_NORMAL, 0, 1);
                updateDisplay = false;
            }
        }
    }

    void GUI::init()
    {
        //Store menu items
        menu1[0] = (char *)"* Main menu *";
        menu1[1] = (char *)"Load Preset";
        menu1[2] = (char *)"Save Preset";
        menu1[3] = NULL;
        szOnOff[0] = (char *)"Off";
        szOnOff[1] = (char *)"On";

        obdI2CInit(&obd, OLED_128x64, -1, 0, 0, 1, 19, 18, -1, 2000000L);
        obdSetBackBuffer(&obd, pBuffer);
        obdMenuInit(&obd, &sm, menu1, FONT_NORMAL, 0, -1, -1, -1, -1, false);
        obdMenuSetCallback(&sm, GlobalMenuCallback);
    }

    void GUI::menuEvent(uint8_t control, TeensySynth::GUI::EventType eventType)
    {
        switch (control)
        {
        case 0:
            switch (eventType)
            {
            case EVENT_NEXT:
                obdMenuDelta(&sm, 1);
                //ucToggle[currentMenuItem] = !ucToggle[currentMenuItem];
                updateDisplay = true;
                break;
            case EVENT_PREV:
                obdMenuDelta(&sm, -1);
                //ucToggle[currentMenuItem] = !ucToggle[currentMenuItem];
                updateDisplay = true;
                break;
            case EVENT_OK:
                break;
            }
            break;
        case 1:
            break;
        }
    }

} // namespace TeensySynth
