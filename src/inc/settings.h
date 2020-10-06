#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>

namespace TeensySynth
{

// Enable debug messages - 0=off, 1=less, 2=more
#define SYNTH_DEBUG 1

// Should the flash memory be initialized even though it looks ok
#define FORCE_INITIALIZE_FLASH 0

// Amount of polyphony
#define NVOICES 6

// Number of presets
#define PRESETS 16

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
    inline void setMidiChannel(uint8_t newMidiChannel)
    {
      CONSTRAIN(newMidiChannel, 1, 16);
      midiChannel = newMidiChannel;
    }

    inline uint8_t getMidiChannel()
    {
      return midiChannel;
    }

    // Set how much reverb should be sent to chorus. Range = 0.0f - 1.0f.
    inline void setChorusReverbLevel(float newChorusReverbLevel)
    {
      CONSTRAIN(newChorusReverbLevel, 0.0f, 1.0f);
      chorusReverbLevel = newChorusReverbLevel;
    }

    inline float getChorusReverbLevel()
    {
      return chorusReverbLevel;
    }

    // Set highpass filter frequency before reverb
    inline void setReverbHighPassFreq(float newReverbHighPassFreq)
    {
      CONSTRAIN(newReverbHighPassFreq, 0.1f, 10000.0f);
      reverbHighPassFreq = newReverbHighPassFreq;
    }

    inline float getReverbHighPassFreq()
    {
      return reverbHighPassFreq;
    }

  private:
    uint8_t midiChannel = 7;           // Default midi channel
    float chorusReverbLevel = 0.8f;    // How much reverb should be sent to chorus by default. Range = 0.0f - 1.0f
    float reverbHighPassFreq = 150.0f; // Highpass filter frequency before reverb
  };

} // namespace TeensySynth

#endif