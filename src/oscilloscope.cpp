#include "inc/oscilloscope.h"

namespace TeensySynth
{
    void Oscilloscope::AudioToPixel(int16_t *audio)
    {
        if (!display || display->updateDisplay == true || (millis() - last) < 100)
            return;
        const int16_t *begin = audio;
        const int16_t *end = audio + AUDIO_BLOCK_SAMPLES;
        __disable_irq();
        display->clearDisplay();
        do
        {
            int16_t wave_data = *audio;
            int16_t pixel_y = map(wave_data, -25000, 25000, -32, 32) + 32;
            int16_t pixel_x = audio - begin;
            display->drawPixel(pixel_x, pixel_y, 1);
            audio++;
        } while (audio < end);
        __enable_irq();
        display->updateDisplay = true;
        last = millis();
        return;
    }

    void Oscilloscope::update(void)
    {
        if (!display)
        {
            return;
        }
        audio_block_t *block = NULL;

        block = receiveReadOnly(0);
        if (!block)
            return;
        AudioToPixel(block->data);
        release(block);
    }
} // namespace TeensySynth