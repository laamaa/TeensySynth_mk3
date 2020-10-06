#include "inc/gui.h"

// Pointer to the current GUI instance, needed by the global menu callback function
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
    GUI::GUI(TeensySynth::Synth *ptrSynth, TeensySynth::Settings *ptrSettings)
    {
        ptrGui = this;
        ts = ptrSynth;
        settings = ptrSettings;
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
                // If the menu timeout limit defined in settings.h has passed, hide the menu
                menuIsOpen = false;
                clearDisplay();
                updateDisplay = true;
            }
        }
        else
        {
            // Menu is not open, draw the "normal" info screen
            if (updateDisplay)
            {
                obdDrawLine(&obd, 0, 32, 127, 32, 1, 1);
                obdWriteString(&obd, 0, 0, 2, synthEngineList[ts->getSynthEngine()], FONT_NORMAL, 0, 1);
                obdWriteString(&obd, 0, 0, 5, charNumbers[ts->getActivePresetNumber()], FONT_NORMAL, 0, 1);
                obdWriteString(&obd, 0, 25, 5, ts->getActivePresetName(), FONT_NORMAL, 0, 1);
                updateDisplay = false;
            }
        }
    }

    void GUI::init()
    {
        // Initialize menu item table
        menu1[0] = (char *)"";
        menu1[1] = (char *)"LOAD PRESET    ";
        menu1[2] = (char *)"SAVE PRESET    ";
        menu1[3] = (char *)"MIDI CHANNEL   ";
        menu1[4] = (char *)"SAVE TO FLASH  ";
        menu1[5] = NULL;

        //Initialize values for main menu settings
        setting[MENU_ITEM_SET_MIDI_CHANNEL] = settings->getMidiChannel(); 

        //Initialize OneBitDisplay library via I2C and inform it that there's a buffer available in the microcontroller
        obdI2CInit(&obd, OLED_128x64, -1, 0, 0, 1, 19, 18, -1, 2000000L);
        obdSetBackBuffer(&obd, pBuffer);

        if (menuIsOpen && !menuIsInitialized)
        {
            // If menu should be open on GUI initialization, check if the menu system is initialized first
            obdMenuInit(&obd, &sm, menu1, FONT_NORMAL, 0, -1, -1, -1, -1, false);
            obdMenuSetCallback(&sm, GlobalMenuCallback);
        }
        else
        {
            // Otherwise just clear the display and go to normal operation
            clearDisplay();
        }
    }

    void GUI::menuEvent(uint8_t control, TeensySynth::GUI::EventType eventType)
    {
        menuTimer = 0;
        switch (control)
        {
        case 0:
            switch (eventType)
            {
            case EVENT_NEXT:
                if (setting[currentMenuItem] < 15)
                    setting[currentMenuItem]++;
                updateDisplay = true;
                break;
            case EVENT_PREV:
                if (setting[currentMenuItem] > 0)
                    setting[currentMenuItem]--;
                updateDisplay = true;
                break;
            case EVENT_OK:
                handleMenuEventOk(0);
                break;
            default:
                break;
            }
            break;
        case 1:
            switch (eventType)
            {
            case EVENT_NEXT:
                currentMenuItem = obdMenuDelta(&sm, 1);
                break;
            case EVENT_PREV:
                currentMenuItem = obdMenuDelta(&sm, -1);
                break;
            case EVENT_OK:
                handleMenuEventOk(1);
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }

    char *GUI::menuCallback(int iIndex)
    {
        if (iIndex == 3)
            return charNumbers[16];
        else
            return charNumbers[setting[iIndex]];
    }

    void GUI::handleMenuEventOk(uint8_t control)
    {
        if (control == 0)
        {
            if (menuIsOpen)
            {
                switch (currentMenuItem)
                {
                case MENU_ITEM_LOAD_PRESET:
#if SYNTH_DEBUG > 0
                    Serial.printf("Load preset %d", setting[currentMenuItem]);
#endif
                    break;
                case MENU_ITEM_SAVE_PRESET:
#if SYNTH_DEBUG > 0
                    Serial.printf("Save preset %d", setting[currentMenuItem]);
#endif
                    break;
                case MENU_ITEM_SET_MIDI_CHANNEL:
#if SYNTH_DEBUG > 0
                    Serial.printf("Set MIDI channel to  %d", setting[currentMenuItem]);
#endif                
                }
            }
        }
        else
        {
            if (!menuIsOpen)
            {
                if (!menuIsInitialized)
                {
                    obdMenuInit(&obd, &sm, menu1, FONT_NORMAL, 0, -1, -1, -1, -1, false);
                    obdMenuSetCallback(&sm, GlobalMenuCallback);
                }
                obdMenuShow(&sm, -1);
                currentMenuItem = 0;
                menuIsOpen = true;
                updateDisplay = true;
            }
        }
    }

} // namespace TeensySynth
