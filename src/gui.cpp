#include "inc/gui.h"

namespace TeensySynth
{

    void GUI::update()
    {
        u8g2->clearBuffer();
        u8g2->setFont(u8g2_font_ncenB10_tr);
        u8g2->drawStr(0, 24, "beep boop!");
        u8g2->sendBuffer();
    }

    void GUI::init(){
            u8g2 = new U8G2_SH1106_128X64_NONAME_1_SW_I2C(U8G2_R0, 19, 18);
            //Set OLED display pins here
            u8g2->begin();

    }

} // namespace TeensySynth