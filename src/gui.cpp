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
    GUI::GUI(Synth *ptrSynth, Settings *ptrSettings)
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
                menuIsOpen = false;
                clearDisplay();
                updateDisplay = true;
            }
            if (showFlashText && flashTextTimer > 1000)
            {
                menuTimer = 0;
                showFlashText = false;
                obdMenuShow(&sm, -1);
                currentMenuItem = 0;
                menuIsOpen = true;
                updateDisplay = true;
            }
        }
        else
        {
            if (updateDisplay)
            {
                obdWriteString(&obd, 0, 0, 2, (char *)"PRESET ", FONT_NORMAL, 0, 1);
                obdWriteString(&obd, 0, -1, -1, charNumbers[ts->getActivePresetNumber()], FONT_NORMAL, 0, 1);
                obdWriteString(&obd, 0, 0, 4, synthEngineList[ts->getSynthEngine()], FONT_LARGE, 0, 1);
                updateDisplay = false;
            }
        }
    }

    void GUI::init()
    {
        //Store menu items
        menu1[0] = (char *)"";
        menu1[1] = (char *)"SAVE PRESET  ";
        menu1[2] = (char *)"USE ADSR ENVL";
        menu1[3] = (char *)"MIDI CHANNEL ";
        menu1[4] = (char *)"SAVE SETTINGS";
        menu1[5] = NULL;

        setting[2] = settings->getMidiChannel();

        obdI2CInit(&obd, OLED_128x64, -1, 0, 0, 1, 19, 18, -1, 2000000L);
        obdSetBackBuffer(&obd, pBuffer);
        if (menuIsOpen && !menuIsInitialized)
        {
            //If menu should be open on GUI initialization, check if the menu system is initialized first
            obdMenuInit(&obd, &sm, currentMenu, FONT_NORMAL, 0, -1, -1, -1, -1, false);
            obdMenuSetCallback(&sm, GlobalMenuCallback);
        }
        else
        {
            //Otherwise just clear the display and go to normal operation
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
                setting[currentMenuItem]++;
                updateDisplay = true;
                break;
            case EVENT_PREV:
                setting[currentMenuItem]--;
                updateDisplay = true;
                break;
            case EVENT_OK:
                handleMenuEventOk(0);
                break;
            default:
                break;
            }
            switch (currentMenuItem)
            {
            case 0:
                // Save Preset
                CONSTRAIN(setting[0], 0, PRESETS);
            case 1:
                // Use ADSR envelope
                ts->setUseExtEnvelope(!ts->getUseExtEnvelope());
                break;
            case 2:
                // Midi Channel
                CONSTRAIN(setting[2], 0, 15);
                settings->setMidiChannel(setting[2]);
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

        switch (iIndex)
        {
        case 1:
            // Use ext envelope
            if (ts->getUseExtEnvelope())
                return (char *)" Y";
            else
                return (char *)" N";
            break;
        case 3:
            return charNumbers[16]; // blank
        default:
            return __itoa(setting[iIndex] + 1, menuCallBackStr, 10);
        }
    }

    void GUI::handleMenuEventOk(uint8_t control)
    {
        if (control == 0)
        {
            if (menuIsOpen)
            {
                switch (currentMenuItem)
                {
                case 0:
                    // Save preset
                    ts->savePreset(setting[1]);
                    flashText(textSaved);
                    break;
                case 1:
                    // Use ADSR env
                    ts->setUseExtEnvelope(!ts->getUseExtEnvelope());
                    updateDisplay = true;
                    break;
                case 3:
                    // Save settings
                    ts->saveSettings();
                    flashText(textSaved);
                    break;
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

    void GUI::flashText(char *text)
    {
        menuTimer = 0;
        showFlashText = true;
        flashTextTimer = 0;
        clearDisplay();
        obdWriteString(&obd, 0, 0, 3, text, FONT_LARGE, 0, 1);
    }

} // namespace TeensySynth
