#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>
#include <EEPROM.h>

namespace TeensySynth
{

// Enable debug messages - 0=off, 1=less, 2=more
#define SYNTH_DEBUG 1

// Should the flash memory be initialized even though it looks ok
#define FORCE_INITIALIZE_FLASH 0

// Amount of polyphony
#define NVOICES 5

// Number of presets
#define PRESETS 9

// Oscillator signal attenuation, happens right after the oscillator. Range = 0.0f-1.0f.
#define OSC_LEVEL 0.6f

// Reference level for mixing signals in master bus. Range isn't limited, but sensible values are 0.2f-0.9f.
#define MIX_LEVEL 0.8f

// Initial potentiometer update threshold value, to prevent noise from triggering parameter updates
#define POT_THRESHOLD 6

// Idle timeout in ms for hiding menu
#define MENU_TIMEOUT 8000

// Constrain macro, borrowed from stmlib
#define CONSTRAIN(var, min, max) \
  if (var < (min))               \
  {                              \
    var = (min);                 \
  }                              \
  else if (var > (max))          \
  {                              \
    var = (max);                 \
  }

  class Settings
  {

  public:
    // A struct of the needed variables is required in order to save/load to eeprom
    struct settings_t
    {
      uint8_t midiChannel;      // Default midi channel
      float chorusReverbLevel;  // How much reverb should be sent to chorus by default. Range = 0.0f - 1.0f
      float reverbHighPassFreq; // Highpass filter frequency before reverb
      bool latch;               // Enable latching for controls to prevent sudden value changes when using presets
      float latchThreshold;
    };

    // Set the synth's incoming MIDI channel
    inline void setMidiChannel(uint8_t newMidiChannel)
    {
      CONSTRAIN(newMidiChannel, 1, 16);
      _settings.midiChannel = newMidiChannel;
    }
    inline uint8_t getMidiChannel() { return _settings.midiChannel; }

    // Set how much reverb should be sent to chorus. Range = 0.0f - 1.0f.
    inline void setChorusReverbLevel(float newChorusReverbLevel)
    {
      CONSTRAIN(newChorusReverbLevel, 0.0f, 1.0f);
      _settings.chorusReverbLevel = newChorusReverbLevel;
    }

    inline float getChorusReverbLevel() { return _settings.chorusReverbLevel; }

    // Set highpass filter frequency before reverb
    inline void setReverbHighPassFreq(float newReverbHighPassFreq)
    {
      CONSTRAIN(newReverbHighPassFreq, 0.1f, 10000.0f);
      _settings.reverbHighPassFreq = newReverbHighPassFreq;
    }
    inline float getReverbHighPassFreq() { return _settings.reverbHighPassFreq; }

    // Set whether synth parameters should update only after they've passed their initial value
    inline void setLatch(bool newLatch) { _settings.latch = newLatch; }
    inline bool getLatch() { return _settings.latch; }

    inline void setLatchThreshold(bool newLatchThreshold)
    {
      CONSTRAIN(newLatchThreshold, 0.0f, 1.0f);
      _settings.latchThreshold = newLatchThreshold;
    }
    inline float getLatchThreshold() { return _settings.latchThreshold; }

    inline void loadSettings(int memoryOffset)
    {
#if SYNTH_DEBUG > 0
      Serial.println(F("Loading settings from flash"));
#endif
      EEPROM.get(memoryOffset, _settings);
    }

    inline void saveSettings(int memoryOffset)
    {
#if SYNTH_DEBUG > 0
      Serial.println(F("Saving settings to flash"));
#endif
      EEPROM.put(memoryOffset, _settings);
    }

  private:
    settings_t _settings = {5, 0.8f, 150.0f, true, 0.1f}; //default values
  };

} // namespace TeensySynth

#endif