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
        // Load patches & settings saved in flash memory, takes a pointer to an array with enough space for all presets/settings
        void loadPatchesFromFlash(Patch (*ptrPatches)[PRESETS]);
        void loadSettingsFromFlash(Settings *ptrSettings);

        // Save patches/settings to flash memory
        void savePatchesToFlash(Patch (*ptrPatches)[PRESETS]);
        void saveSettingsToFlash(Settings *ptrSettings);

        // Checks if flash contains older data version. Returns true if it's ok to load, false if data version differs
        bool checkFlash();

        // Initialize with default data
        void initializeFlash();

    private:
  
        
        //Checks if the memVersion in flash matches the one we just calculated. Returns true if the values match, false if the values differ.
        
    };

} // namespace TeensySynth

#endif