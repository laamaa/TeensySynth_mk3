#ifndef PATCH_MANAGER_H_
#define PATCH_MANAGER_H_

#include "settings.h"
#include "patch.h"

namespace TeensySynth
{
    // Handles saving & loading synth patches (presets) in Teensy's flash
    class FlashMemoryManager
    {
    public:
        //load patches saved in flash memory, needs a pointer to an array with enough space for all presets (use NUM_PRESETS defined in settings.h)
        void loadPatchesFromFlash(Patch *ptrPatches);
        void savePatchesToFlash(Patch *ptrPatches);
        

    private:
        //Constant for checking if the flash contents match what we're expecting
        const uint16_t memVersion = sizeof(Patch) * PRESETS + sizeof(Settings);

        //Offset for reading settings from the flash (stored after patches)
        const uint16_t settingsOffset = sizeof(Patch) * PRESETS;      
        
        //Checks if the memVersion in flash matches the one we just calculated. Returns true if the values match, false if the values differ.
        bool checkFlash();
    };

} // namespace TeensySynth

#endif