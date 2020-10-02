#include "inc/gui.h"

namespace TeensySynth
{
    void GUI::update()
    {
        if (updateDisplay)
        {
            u8g2->sendBuffer();
            updateDisplay = false;
        }
    }

    void GUI::init()
    {
        u8g2 = new U8G2_SH1106_128X64_NONAME_F_HW_I2C(U8G2_R0, 19, 18);
        //Set OLED display pins here
        u8g2->begin();
    }

    void GUI::menuEvent(uint8_t control, TeensySynth::GUI::EventType eventType) {}

    void Oscilloscope::AudioToPixel(int16_t *audio)
    {
        if (!display || display->updateDisplay == true || (millis() - last) < 100)
            return;
        const int16_t *begin = audio;
        const int16_t *end = audio + AUDIO_BLOCK_SAMPLES;
        __disable_irq();
        display->clearBuffer();
        do
        {
            int16_t wave_data = *audio;
            int16_t pixel_y = map(wave_data, -32768, 32767, -63, 63) + 63;
            int16_t pixel_x = audio - begin;
            display->drawPixel(pixel_x, pixel_y);
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
