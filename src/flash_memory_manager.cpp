#include "inc/settings.h"
#include "inc/flash_memory_manager.h"
#include <Arduino.h>
#include <EEPROM.h>

namespace TeensySynth
{

    void FlashMemoryManager::loadPatchesFromFlash(Patch *ptrPatches)
    {
    }

    void FlashMemoryManager::savePatchesToFlash(Patch *ptrPatches)
    {
#if SYNTH_DEBUG > 1
        Serial.print(F("Patch array size is "));
        Serial.println(sizeof(ptrPatches));
#elif SYNTH_DEBUG > 0
        Serial.println(F("Saving preset data to flash"));
#endif
    }

    bool FlashMemoryManager::checkFlash()
    {
        uint16_t memVersionFlash;
        EEPROM.get(0, memVersionFlash);
        if (memVersionFlash == memVersion)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
} // namespace TeensySynth